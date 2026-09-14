#!/bin/bash
# Build the exact engine used by the game. May also run in an existing dev container.
set -euo pipefail

sm_version=153.0.4
sm_sha256=f7aa83924c66bb3b04cf139b3b00612d388a9f024c92fe7834161553a6028a48
sm_build_root=${SPIDERMONKEY_BUILD_DIR:-/tmp/kinslayer-mozjs-153}
sm_prefix=${SPIDERMONKEY_PREFIX:-/usr/local}
sm_jobs=${GCC_THREADS:-4}
sm_archive=firefox-${sm_version}.source.tar.xz
sm_script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
# Firefox 153 requires Clang/LLVM >=19. Pin 19 on Ubuntu 24.04 and 26.04.
export CC=${CC:-clang-19}
export CXX=${CXX:-clang++-19}
export LLVM_CONFIG=${LLVM_CONFIG:-llvm-config-19}

mkdir -p "$sm_build_root"
cd "$sm_build_root"
if [ ! -f "$sm_archive" ]; then
    curl -fL --retry 3 -o "$sm_archive.part" \
        "https://archive.mozilla.org/pub/firefox/releases/${sm_version}/source/${sm_archive}"
    mv "$sm_archive.part" "$sm_archive"
fi
# Pinned to Mozilla's SHA256SUMS for this release, not a mutable latest tarball.
printf '%s  %s\n' "$sm_sha256" "$sm_archive" | sha256sum -c -
if [ ! -d "firefox-${sm_version}/js/src" ]; then
    tar -xf "$sm_archive"
fi
# 153.0.4 links the Rust regexp property glue even without Intl, but omits its
# C++ callback in that configuration. Compile the existing callback in both
# configurations; no regexp implementation or Intl behavior is changed.
for sm_patch_name in mozjs-153-no-intl.patch mozjs-153-install-headers.patch mozjs-153-embedder-allocators.patch; do
sm_patch="$sm_script_dir/patches/$sm_patch_name"
(
    cd "firefox-${sm_version}"
    if patch -p1 --batch --forward --dry-run --silent < "$sm_patch"; then
        patch -p1 --batch --forward < "$sm_patch"
    elif ! patch -p1 --batch -R --dry-run --silent < "$sm_patch"; then
        echo "SpiderMonkey source does not match $sm_patch_name." >&2
        exit 1
    fi
)
done
mkdir -p "firefox-${sm_version}/js/src/_build"
cd "firefox-${sm_version}/js/src/_build"
../configure --prefix="$sm_prefix" --disable-jemalloc --disable-debug \
    --enable-optimize --without-intl-api
make -j"$sm_jobs"
make install
ldconfig
PKG_CONFIG_PATH="$sm_prefix/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}" \
    pkg-config --modversion mozjs-153
