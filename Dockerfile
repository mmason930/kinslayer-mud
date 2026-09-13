FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG GCC_THREADS
ARG BOOST_VERSION="1_90_0"
ARG BOOST_VERSION_DOT="1.90.0"
ARG RUST_VERSION="1.93.0"

# Install pre-requisites
RUN apt update
RUN apt install libmysqlclient-dev cmake g++ gcc gdb gdbserver wget git-all dos2unix cron nano \
    autoconf2.13 python3 python3-pip llvm-19 clang-19 lld-19 pkg-config curl patch openssh-server rsync valgrind -y

# Pin Rust: 1.98's new Linux targets break Firefox 153 target detection
# (Mozilla bug 2053518). Keep fresh builds on the validated toolchain.
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --profile minimal --default-toolchain ${RUST_VERSION}
ENV PATH="/root/.cargo/bin:${PATH}"
ENV RUSTUP_TOOLCHAIN="${RUST_VERSION}"
RUN cargo install --locked cbindgen --version 0.29.4

# sqlDatabase
RUN git clone https://github.com/kinslayermud/kinslayer-sqlDatabase /kinslayer-sqlDatabase
WORKDIR /kinslayer-sqlDatabase/mysql/
RUN chmod ug+x ./install.sh
RUN ./install.sh

# SpiderMonkey 153.0.4, built from the pinned Mozilla source release.
COPY build-spidermonkey.sh /tmp/build-spidermonkey.sh
COPY patches/mozjs-153-*.patch /tmp/patches/
RUN GCC_THREADS=${GCC_THREADS:-4} bash /tmp/build-spidermonkey.sh \
    && rm -rf /tmp/kinslayer-mozjs-153 /tmp/build-spidermonkey.sh /tmp/patches

# Boost
WORKDIR /
RUN wget http://sourceforge.net/projects/boost/files/boost/${BOOST_VERSION_DOT}/boost_${BOOST_VERSION}.tar.gz
RUN tar -xvzf boost_${BOOST_VERSION}.tar.gz
WORKDIR /boost_${BOOST_VERSION}
RUN ./bootstrap.sh
RUN ./b2 install -j ${GCC_THREADS} ; exit 0

# cpp-httplib
RUN git clone https://github.com/yhirose/cpp-httplib /kinslayer-cpp-httplib
RUN cp /kinslayer-cpp-httplib/httplib.h /usr/local/include/
RUN rm -rf /kinslayer-cpp-httplib

WORKDIR /
RUN ulimit -S -c unlimited
RUN ldconfig
RUN rm -rf /boost_${BOOST_VERSION} /boost_${BOOST_VERSION}.tar.gz /kinslayer-sqlDatabase

COPY crontab /etc/cron.d/

EXPOSE 2230
EXPOSE 2222
EXPOSE 666
EXPOSE 4001
EXPOSE 4002

ENTRYPOINT ["/kinslayer/docker-start.sh"]
