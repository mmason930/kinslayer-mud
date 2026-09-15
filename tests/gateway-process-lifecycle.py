#!/usr/bin/env python3
"""Exercise the real gateway against a fake MUD, without game data or a database.

Usage: python3 tests/gateway-process-lifecycle.py /path/to/bin/gateway
The normal restart cooldown is 60 seconds; allow about 75 seconds for this test.
"""
import os
from pathlib import Path
import signal
import socket
import subprocess
import sys
import tempfile
import time


def free_port():
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        return sock.getsockname()[1]


def fake_mud(root, port):
    with (root / "children").open("a") as output:
        output.write(f"{os.getpid()} {os.getppid()}\n")
    generation = len((root / "children").read_text().splitlines())
    with socket.socket() as listener:
        listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        listener.bind(("127.0.0.1", port))
        listener.listen()
        listener.settimeout(10)
        conn, _ = listener.accept()
        with conn:
            conn.settimeout(10)
            assert conn.recv(4096).startswith(b"Validate ")
            conn.sendall(f"ProcessID {os.getpid()}\nFinishedBooting\n".encode())
            time.sleep(0.5)
            if generation == 2:
                os._exit(7)  # An unexpected exit must also be reaped/restarted.
            if generation == 3:
                conn.sendall(b"ShutdownOnReboot\n")
            conn.sendall(b"Reboot\n")
            time.sleep(0.5)  # Allow the gateway to observe a living shutting-down PID.


def test_gateway(binary):
    with tempfile.TemporaryDirectory(prefix="gateway-lifecycle-") as directory:
        root = Path(directory)
        (root / "lib/misc").mkdir(parents=True)
        game_port, gateway_port = free_port(), free_port()
        while gateway_port == game_port:
            gateway_port = free_port()
        launcher = root / "fake-mud"
        import shlex
        launcher.write_text("#!/bin/sh\nexec " + " ".join(map(shlex.quote, [
            sys.executable, str(Path(__file__).resolve()), "--fake", str(root), str(game_port)
        ])) + "\n")
        launcher.chmod(0o755)
        (root / "lib/misc/BasicConfig").write_text(
            f"Gateway Port: {gateway_port}\nMUD Port: {game_port}\n"
            f"MUD Host: 127.0.0.1\nMUD Root Directory: {root}/\n"
            "Restart On Shutdown: 1\nMUD Executable Path: fake-mud\n"
        )
        with (root / "gateway.log").open("w+") as log:
            gateway = subprocess.Popen([str(Path(binary).resolve())], cwd=root,
                                       stdout=log, stderr=subprocess.STDOUT,
                                       start_new_session=True)
            try:
                deadline = time.monotonic() + 80
                reaped_while_running = set()
                while gateway.poll() is None:
                    assert time.monotonic() < deadline, "Gateway failed to finish reboot cycles"
                    if (root / "children").exists():
                        rows = (root / "children").read_text().splitlines()
                        for row in rows[:-1]:
                            pid, _ = map(int, row.split())
                            assert not Path(f"/proc/{pid}").exists(), f"Unreaped child {pid}"
                            reaped_while_running.add(pid)
                    time.sleep(0.05)
                assert gateway.returncode == 0
                assert len(reaped_while_running) == 2, reaped_while_running
                children = [tuple(map(int, row.split())) for row in
                            (root / "children").read_text().splitlines()]
                assert len(children) == 3, children
                assert all(parent == gateway.pid for _, parent in children), children
                assert all(not Path(f"/proc/{pid}").exists() for pid, _ in children), children
                log.seek(0)
                output = log.read()
                assert output.count("finished booting") == 3, output
                assert output.count("completed with shutdown") == 2, output
                assert "Game has crashed" in output, output
                print("Gateway normal reboot, crash restart, and child cleanup passed")
            except BaseException:
                log.seek(0)
                print(log.read(), file=sys.stderr)
                raise
            finally:
                try:
                    os.killpg(gateway.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
                gateway.wait()


if __name__ == "__main__":
    if sys.argv[1] == "--fake":
        fake_mud(Path(sys.argv[2]), int(sys.argv[3]))
    else:
        test_gateway(sys.argv[1])
