#!/usr/bin/env python3
"""Run the real gateway against a fake MUD on loopback; no game/database access."""
import base64
import os
from pathlib import Path
import queue
import secrets
import socket
import subprocess
import sys
import tempfile
import threading
import time


def free_port():
    with socket.socket() as s:
        s.bind(('127.0.0.1', 0))
        return s.getsockname()[1]


def test(binary, trusted):
    with tempfile.TemporaryDirectory(prefix='kinslayer-gateway-net-') as directory:
        root = Path(directory)
        (root / 'lib/misc').mkdir(parents=True)
        token = secrets.token_hex(32)
        key = root / 'gateway.key'
        key.write_text(token)
        key.chmod(0o600)
        ports = set()
        while len(ports) < 3:
            ports.add(free_port())
        game, telnet, websocket = ports
        (root / 'lib/misc/BasicConfig').write_text(
            f'Gateway Port: {telnet}\nWebsocket Ports: {websocket}\nMUD Port: {game}\n'
            f'MUD Host: 127.0.0.1\nMUD Root Directory: {root}/\nRestart On Shutdown: 0\n'
            'MUD Executable Path: unused\n')
        hosts, failures = queue.Queue(), queue.Queue()
        listener = socket.socket()
        listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        listener.bind(('127.0.0.1', game))
        listener.listen()
        listener.settimeout(10)

        def fake_mud():
            try:
                mother, _ = listener.accept()
                with mother:
                    mother.settimeout(10)
                    control = mother.makefile('rb')
                    assert control.readline().strip() == f'Validate {token}'.encode()
                    mother.sendall(b'FinishedBooting\n')
                    while True:
                        line = control.readline().decode().split()
                        if not line: return
                        if line[0] == 'Ping':
                            mother.sendall(b'PingResponse\n')
                        elif line[0] == 'Host':
                            hosts.put(line[2])
                            session = secrets.token_hex(32)
                            mother.sendall(f'Session {line[1]} {session}\n'.encode())
                            peer, _ = listener.accept()
                            with peer:
                                peer.settimeout(10)
                                assert peer.makefile('rb').readline().strip() == f'Session {session}'.encode()
                                peer.sendall(b'{"method":"test"}\x06')
                                assert peer.recv(1024) == b'look\x06'
                                peer.sendall(b'{"method":"echo"}\x06')
                                time.sleep(.1)
            except (OSError, ValueError):
                pass  # Sockets close during gateway shutdown.
            except BaseException as error:
                failures.put(error)

        worker = threading.Thread(target=fake_mud, daemon=True)
        worker.start()
        env = dict(os.environ, KINSLAYER_GATEWAY_SECRET_FILE=str(key),
                   KINSLAYER_TRUSTED_PROXY='127.0.0.1' if trusted else '')
        with (root / 'gateway.log').open('w+') as log:
            process = subprocess.Popen([str(Path(binary).resolve())], cwd=root, env=env, stdout=log, stderr=log)
            try:
                deadline = time.monotonic() + 10
                while True:
                    try:
                        client = socket.create_connection(('127.0.0.1', websocket), timeout=.2)
                        break
                    except OSError:
                        assert process.poll() is None and time.monotonic() < deadline
                        time.sleep(.05)
                with client:
                    client.settimeout(10)
                    address = '2001:db8:ffff:ffff:ffff:ffff:ffff:ffff'
                    client.sendall((f'GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\n'
                                    'Connection: Upgrade\r\nSec-WebSocket-Version: 13\r\n'
                                    f'Sec-WebSocket-Key: {base64.b64encode(secrets.token_bytes(16)).decode()}\r\n'
                                    f'Sec-WebSocket-Protocol: mud-protocol\r\nX-Forwarded-For: {address}\r\n\r\n').encode())
                    received = b''
                    while b'"test"' not in received:
                        data = client.recv(4096)
                        assert data
                        received += data
                    assert b'101' in received
                    assert hosts.get(timeout=2) == (address if trusted else '127.0.0.1')
                    # A masked ping between fragments must not reach the MUD.
                    def frame(opcode, data, fin=True):
                        mask = b'abcd'
                        return bytes([(128 if fin else 0) | opcode, 128 | len(data)]) + mask + bytes(c ^ mask[i % 4] for i, c in enumerate(data))
                    client.sendall(frame(1, b'lo', False) + frame(9, b'ping') + frame(0, b'ok\x06'))
                    received = b''
                    while b'"echo"' not in received:
                        data = client.recv(4096)
                        assert data
                        received += data
                    assert b'\x8a\x04ping' in received
                assert process.poll() is None
                if not failures.empty(): raise failures.get()
            except BaseException:
                log.seek(0)
                print(log.read(), file=sys.stderr)
                raise
            finally:
                process.terminate()
                process.wait(timeout=5)
                listener.close()
                worker.join(timeout=2)


test(sys.argv[1], trusted=False)
test(sys.argv[1], trusted=True)
print('PASS: real gateway authentication, trusted/untrusted forwarding, IPv6, fragmented messages and ping/pong')
