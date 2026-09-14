# kinslayer-mud
This repository hosts the core game code for KinslayerMUD(http://www.kinslayermud.org) as well as runtime scripts. The game can be compiled and run natively on your machine, assuming you install all required dependencies(see `Dockerfile`). However, the recommended approach is to use Docker and Docker Compose for simplicity. This document will walk through that process.

Note that in order to use Docker Compose, you'll need to clone the following repository: https://github.com/kinslayermud/kinslayer-loader

All commands in this document should be executed from the kinslayer-loader directory.

## Build or upgrade the game image

The game image uses Ubuntu 26.04, GCC 15, and Ubuntu's Boost 1.90 packages.
SpiderMonkey remains pinned to 153.0.4 with LLVM 19 and Rust 1.93.0.
The image contains the build dependencies; Compose mounts this repository at
`/kinslayer`, and the entrypoint compiles the executables there.

After pulling these changes, build the new image and recreate the game container:

```sh
docker compose build game
docker compose up -d --no-deps --force-recreate game
docker compose logs -f game
```

Recreating the container interrupts the running game. Startup keeps its object
files in `/tmp/kinslayer-obj` inside the container, so objects from the previous
Ubuntu image cannot be reused accidentally. The first startup compiles from
scratch; subsequent starts of the same container can build incrementally.
`GCC_THREADS` controls compilation parallelism (default 4).

## Start game container
The simplest way to start the container is to run the following:
```
docker compose up game
```

The loader's default `ssh` target starts SSH and the gateway. With
`RESTART_ON_SHUTDOWN=0`, it builds only the gateway and leaves the game build and
launch to CLion. After upgrading the image, reset CLion's CMake cache and perform
a clean rebuild before launching the game. With `RESTART_ON_SHUTDOWN=1` (the
entrypoint default), startup builds both executables and the gateway starts the
game automatically.

To run the gateway and game without SSH, use the `gateway` target and set
`RESTART_ON_SHUTDOWN=1`. For example, with the existing game container stopped:

```
docker compose run --service-ports -e RESTART_ON_SHUTDOWN=1 game gateway partial
```

Note that running docker compose run may change the name of the running container(from game.kinslayermud.org below). You can run docker container ls to get the actual name of the container if you need it for subsequent commands.

## Incremental build
While the container is running, you can perform a partial build by running:

```
docker exec -it game.kinslayermud.org bash -c 'make all -j${GCC_THREADS:-4} -C /kinslayer/src OBJDIR=/tmp/kinslayer-obj'
```

## Full build
While the container is running, you can perform a full build by running:

```
docker exec -it game.kinslayermud.org bash -c 'make clean -C /kinslayer/src OBJDIR=/tmp/kinslayer-obj && make all -j${GCC_THREADS:-4} -C /kinslayer/src OBJDIR=/tmp/kinslayer-obj'
```
 
## Stop game container
To stop the container, run:

```
docker compose down game
```

## After starting the container
You will be able to connect to the MUD using telnet or a MUD client by connecting to 127.0.0.1 on port 2222

Alternatively, if you clone the [kinslayer-mud-client](https://github.com/mmason930/kinslayer-mud-client) repository, you can connect to the game by running the client locally on your machine. Each user account's password is set to `iwontsay`.
## Main-loop timing and database recovery

The game targets seven ticks per second using absolute monotonic deadlines.
Time spent processing commands, sockets, database work, and heartbeats counts
toward the next tick instead of being added to a full tick's sleep. Partial
ticks carry forward. After a long stall, at most two seconds of heartbeats run
in one batch; older overdue ticks are dropped and logged to bound catch-up work.

The main database connection is checked at most once per second of loop time,
including when no queries run. A failed check triggers the existing reconnect
routine, with retries no more often than every 15 seconds during an outage.
Normal query error handling is unchanged. Idle disconnect detection takes about
a second plus any main-thread/network delay; ping and reconnect remain synchronous.

Run the deterministic scheduling/recovery tests without starting the game or a
database: `sh tests/run-game-loop-timing.sh`. Set
`TIMING_TEST_CXXFLAGS=-fsanitize=address,undefined` to run with sanitizers.

## Pathfinding and performance counters

The four native path searches share a BFS implementation with reusable queue
storage and per-room generation stamps. Full paths are reconstructed once from
predecessors. Searches keep their original direction ordering and traversal
policies: NPC hunting excludes `ROOM_NOTRACK`; the room/JavaScript APIs do not.
These APIs still traverse closed, hidden, and disabled exits as before. Actual
movement and the client mini-map apply their own rules. Routes are not cached
between calls, so changed exits are visible on the next search.

JavaScript can call `room.routeTo(destination)` to obtain both `firstStep` and
`distance` from one search. Same-room results are `{-2, 0}`, unreachable results
are `{-3, -1}`, and invalid destinations produce `{-1, -1}` (listed as
`firstStep, distance`). The existing APIs remain available. Raid movement and
the Alder quest movement scripts use the combined call, with a fallback for
older binaries during deployment. Quest routes are refreshed after commands
that might run movement or exit-editing triggers.

`show lag` (existing level 104+ access) includes cumulative per-API search counts,
rooms visited, maximum rooms visited, total/average/maximum time, and the source
and destination vnums of the slowest search. Times are milliseconds. Pathfinding
time is already included in the enclosing heartbeat/input routine; do not add
these counters to those totals. Counters reset on process restart.

Run `sh tests/run-pathfinding.sh` and `js153 tests/pathfinding-scripts.js` from the
game directory. `PATH_TEST_CXXFLAGS=-fsanitize=address,undefined` instruments the
C++ regressions. Rebuild all game objects when deploying: `Room` now has transient
search metadata, and the JavaScript room binding has a new method.

## SpiderMonkey 153 and JIT

The game builds against **SpiderMonkey 153.0.4** (`mozjs-153`). The Docker image
builds the library from Mozilla's [153.0.4 source release](https://archive.mozilla.org/pub/firefox/releases/153.0.4/),
with the source SHA-256 pinned in `build-spidermonkey.sh`. That script is also
usable inside an existing development container. It needs Clang/LLVM 19 or newer,
Rust 1.93.0, and cbindgen 0.29.4; the Dockerfile installs these dependencies.
Rust is pinned rather than tracking `stable`: Rust 1.98 adds Linux target triples
that Firefox 153's configure script cannot disambiguate on x86_64
([Mozilla bug 2053518](https://bugzilla.mozilla.org/show_bug.cgi?id=2053518)).
For a manual build, install `rustup toolchain install 1.93.0 --profile minimal`
and run the build script with `RUSTUP_TOOLCHAIN=1.93.0`.
The engine remains optimized, with jemalloc and Intl disabled as before.
The pinned source needs a small build-list fix for `--without-intl-api`:
`patches/mozjs-153-no-intl.patch` includes the existing regexp C++ callback that
the Rust glue references even in a no-Intl build. The build script applies this
patch and recognizes an already patched source cache. An install-only patch
exports the generated `ProfilingCategoryList.h` and `PrefsGenerated.h` in the
locations required by public headers. The allocator patch adds an opt-out for
Mozilla's global C++ new/delete overrides, newly pulled into embedding code by
public headers. The shim opts out so game allocations continue to use standard
C++ operators; this prevents the allocation mismatch caught by AddressSanitizer.
SpiderMonkey's own allocation behavior is unchanged.

JIT compilation is enabled by default. Startup reports the linked library's actual
version, `JavaScript-C153.0.4: JIT enabled`. Set `KINSLAYER_DISABLE_JIT=1` in the game
process environment for the interpreter fallback; changing this setting requires
a process restart.

For a fresh container, rebuild the game image (`docker compose build game` from
the loader repository). For an already running development container with the
prerequisites installed, run `bash /kinslayer/build-spidermonkey.sh`. Set
`SPIDERMONKEY_BUILD_DIR` to retain a source/build cache and `GCC_THREADS` to
control build parallelism (default 4). Versions 131 and 153 can be installed
side by side, but Make, CMake, and the embedding tests select 153. Rebuild every
game object after changing the engine major version; do not mix objects compiled
against different SpiderMonkey headers. The shim checks the header major version
and supplies the XP_UNIX/XP_WIN definitions required by the public 153 headers.

The compatibility wrappers own shared, scoped roots for garbage-collected values;
scalars do not allocate roots. Native objects trace their JavaScript back-reference
instead of rooting themselves. Filesystem reload keeps normal JavaScript behavior:
new invocations use replacement functions, while suspended generators and retained
callbacks keep their original function bodies.

Run the standalone embedding regressions in the development container:

```sh
docker exec game.kinslayermud.org /kinslayer/tests/run-js-jit.sh
```

The suite checks actual Ion execution, moving collection, copied arrays and entity
cache identity, native finalization and exception handling, hot reload, suspended
generators, callback receivers, nested execution deadlines, and context shutdown.
It runs with JIT both enabled and disabled. Testing-only engine functions are
exposed solely inside this standalone executable.

Run the script-level regressions with the matching shell from the game directory:

```sh
js153 tests/gatekeeper-timeout.js
js153 tests/help-browser.js
js153 tests/quest-browser.js
```

To additionally instrument the embedding with AddressSanitizer and UBSan:

```sh
docker exec -e JS_TEST_CXXFLAGS=-fsanitize=address,undefined \
  -e ASAN_OPTIONS=detect_leaks=1 game.kinslayermud.org /kinslayer/tests/run-js-jit.sh
```

The installed SpiderMonkey library itself is not sanitizer-instrumented by that
command. Full engine instrumentation/GC zeal requires a matching debug engine build.

## Help access and client sidebar

Apply `migrations/20260913_180000_01--Help-file-minimum-level.sql` once to the game
schema **before deploying the updated scripts**:

```sql
ALTER TABLE `helpFile`
  ADD COLUMN `minimum_level` TINYINT UNSIGNED NOT NULL DEFAULT 0;
```

Existing files remain public. The migration runner in `DatabaseMigrationUtil` is
not implemented, so this migration must be applied manually. Deploy the game
scripts, rebuild the game and restart it to load the levels and register the new
`helpedit` command. Deploy `kinslayer-web` for the portal editor field and
`kinslayer-mud-client` for the sidebar changes; refresh any open browser tabs.
No change to the separate website's `websiteHelpFile` table is needed.

Help files accept a minimum level from **0–105**; 0 means everyone, 100 means
immortals. A page also inherits all ancestor restrictions. The highest minimum
in the ancestry applies, with no holylight bypass. Missing parents and cycles
are inaccessible until corrected. Player help search, numbered guide navigation,
browser topic metadata and direct article requests all use the same game-side
access check. Restricted descriptions are not evaluated. Existing staff editors
can still edit all files: the web portal socket now checks level 100 itself,
and the new level-100 `helpedit` command opens the existing in-game editor. Choose
**L** in its file menu to set the minimum. New child files now receive creator
metadata so they can be saved successfully.

`Browse Quests` includes `issuer`, taken from `getMobName(quest.giverVnum(actor))`
just like the journal. An absent or deleted issuer produces an empty string.
The client displays this in the active list, pins and details, and remembers a
resizable desktop sidebar width in browser storage. Help caches are cleared on
disconnect or character change.

Validation: `js153 tests/help-browser.js`, `js153 tests/help-access.js`, and
`js153 tests/quest-browser.js`. The companion repositories contain Playwright
checks for sidebar resizing/issuers, help navigation/access changes and the web
help editor. The SQL migration can be tested against a temporary copy of the
help table without touching persistent data.
