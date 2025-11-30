FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG GCC_THREADS
ARG BOOST_VERSION="1_84_0"
ARG BOOST_VERSION_DOT="1.84.0"

# Install pre-requisites
RUN apt update
RUN apt install libmysqlclient-dev cmake g++ gcc wget git-all dos2unix cron nano \
    autoconf2.13 python3 python3-pip llvm clang pkg-config curl -y

# Install Rust via rustup (need newer version than Ubuntu packages provide)
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
ENV PATH="/root/.cargo/bin:${PATH}"
RUN rustup update stable
RUN cargo install cbindgen

# sqlDatabase
RUN git clone https://github.com/kinslayermud/kinslayer-sqlDatabase /kinslayer-sqlDatabase
WORKDIR /kinslayer-sqlDatabase/mysql/
RUN chmod ug+x ./install.sh
RUN ./install.sh

# SpiderMonkey 131 (from Firefox ESR)
WORKDIR /
RUN wget https://archive.mozilla.org/pub/firefox/releases/131.0/source/firefox-131.0.source.tar.xz
RUN tar -xf firefox-131.0.source.tar.xz
WORKDIR /firefox-131.0/js/src
RUN mkdir _build
WORKDIR /firefox-131.0/js/src/_build
RUN ../configure --disable-jemalloc --disable-debug --enable-optimize --without-intl-api
RUN make -j${GCC_THREADS}
RUN make install
RUN ldconfig

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
RUN rm -rf /boost_${BOOST_VERSION} /boost_${BOOST_VERSION}.tar.gz /firefox-131.0 /firefox-131.0.source.tar.xz /kinslayer-sqlDatabase

COPY crontab /etc/cron.d/

EXPOSE 2230
EXPOSE 2222
EXPOSE 666
EXPOSE 4001
EXPOSE 4002

ENTRYPOINT ["/kinslayer/docker-start.sh"]
