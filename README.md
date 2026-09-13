# kinslayer-mud
This repository hosts the core game code for KinslayerMUD(http://www.kinslayermud.org) as well as runtime scripts. The game can be compiled and run natively on your machine, assuming you install all required dependencies(see `Dockerfile`). However, the recommended approach is to use Docker and Docker Compose for simplicity. This document will walk through that process.

Note that in order to use Docker Compose, you'll need to clone the following repository: https://github.com/kinslayermud/kinslayer-loader

All commands in this document should be executed from the kinslayer-loader directory.

## Start game container
The simplest way to star the container is to run the following:
```
docker compose up game
```

By default, the container will perform a clean build of the game, resulting in a full compilation of the game code.

Becuase this is time consuming, you can alternatively run the container and perform a partial build:

```
docker compose run --service-ports game gateway partial
```

Note that running docker compose run may change the name of the running container(from game.kinslayermud.org below). You can run docker container ls to get the actual name of the container if you need it for subsequent commands.

## Incremental build
While the container is running, you can perform a partial build by running:

```
docker exec -it game.kinslayermud.org bash -c 'make -j${GCC_THREADS} -C /kinslayer/src'
```

## Full build
While the container is running, you can perform a full build by running:

```
docker exec -it game.kinslayermud.org bash -c 'make clean -C /kinslayer/src && make -j${GCC_THREADS} -C /kinslayer/src'
```
 
## Stop game container
To stop the container, run:

```
docker compose down game
```

## After starting the container
You will be able to connect to the MUD using telnet or a MUD client by connecting to 127.0.0.1 on port 2222

Alternatively, if you clone the [kinslayer-mud-client](https://github.com/mmason930/kinslayer-mud-client) repository, you can connect to the game by running the client locally on your machine. Each user account's password is set to `iwontsay`.
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
