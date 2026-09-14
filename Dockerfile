FROM ubuntu:26.04

ARG DEBIAN_FRONTEND=noninteractive
ARG GCC_THREADS=4
ARG RUST_VERSION="1.93.0"
ARG SQL_DATABASE_REVISION="145099d91714b9e5f08e6645b84429897068be8a"
ARG CPP_HTTPLIB_REVISION="278c2979e8c68468960c3073e28e1c51b098d6a4"

# Ubuntu 26.04 supplies Boost 1.90 and LLVM 19 on both amd64 and arm64.
# Install development libraries explicitly rather than relying on transitive packages.
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential ca-certificates cmake gdb gdbserver git wget curl patch \
        dos2unix cron nano openssh-server rsync valgrind xz-utils \
        libmysqlclient-dev libssl-dev libcrypt-dev zlib1g-dev \
        libboost-filesystem-dev libboost-thread-dev libboost-regex-dev \
        autoconf2.13 python3 python3-pip llvm-19 clang-19 lld-19 pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Pin Rust: 1.98's new Linux targets break Firefox 153 target detection
# (Mozilla bug 2053518). Keep fresh builds on the validated toolchain.
RUN curl --proto '=https' --tlsv1.2 -fsS https://sh.rustup.rs -o /tmp/rustup-init.sh \
    && sh /tmp/rustup-init.sh -y --profile minimal --default-toolchain "${RUST_VERSION}" \
    && rm /tmp/rustup-init.sh
ENV PATH="/root/.cargo/bin:${PATH}"
ENV RUSTUP_TOOLCHAIN="${RUST_VERSION}"
RUN cargo install --locked cbindgen --version 0.29.4

# sqlDatabase's install script lacks set -e; do not let a compile failure be hidden.
RUN git clone https://github.com/kinslayermud/kinslayer-sqlDatabase /tmp/kinslayer-sqlDatabase \
    && cd /tmp/kinslayer-sqlDatabase \
    && git checkout --detach "${SQL_DATABASE_REVISION}" \
    && cd mysql \
    && bash -e ./install.sh \
    && test -s /usr/local/lib/libsqlDatabase.a \
    && rm -rf /tmp/kinslayer-sqlDatabase

# SpiderMonkey 153.0.4, built from the pinned, checksum-verified Mozilla release.
COPY build-spidermonkey.sh /tmp/build-spidermonkey.sh
COPY patches/mozjs-153-*.patch /tmp/patches/
RUN GCC_THREADS=${GCC_THREADS:-4} bash /tmp/build-spidermonkey.sh \
    && rm -rf /tmp/kinslayer-mozjs-153 /tmp/build-spidermonkey.sh /tmp/patches

RUN git clone https://github.com/yhirose/cpp-httplib /tmp/kinslayer-cpp-httplib \
    && cd /tmp/kinslayer-cpp-httplib \
    && git checkout --detach "${CPP_HTTPLIB_REVISION}" \
    && install -m 644 httplib.h /usr/local/include/httplib.h \
    && rm -rf /tmp/kinslayer-cpp-httplib \
    && ldconfig

COPY crontab /etc/cron.d/kinslayer
RUN chmod 0644 /etc/cron.d/kinslayer

# Game sources and runtime data are supplied by the existing Compose bind mount.
WORKDIR /kinslayer
EXPOSE 22 2230 2222 666 4001 4002
ENTRYPOINT ["/kinslayer/docker-start.sh"]
