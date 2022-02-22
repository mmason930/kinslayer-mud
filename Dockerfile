FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
ARG GCC_THREADS
ARG BOOST_VERSION="1_53_0"
ARG BOOST_VERSION_DOT="1.53.0"

# Install pre-requisites
RUN apt update
RUN apt install libmysqlclient-dev cmake g++ gcc wget git-all dos2unix -y

# sqlDatabase
RUN git clone https://github.com/kinslayermud/kinslayer-sqlDatabase /kinslayer-sqlDatabase
WORKDIR /kinslayer-sqlDatabase/mysql/
RUN chmod ug+x ./install.sh
RUN ./install.sh

# Spidermonkey
RUN git clone https://github.com/kinslayermud/kinslayer-spidermonkey /kinslayer-spidermonkey
WORKDIR /kinslayer-spidermonkey/src
RUN ./configure
RUN make -j${GCC_THREADS}
RUN make install

# Boost
WORKDIR /
RUN wget http://sourceforge.net/projects/boost/files/boost/${BOOST_VERSION_DOT}/boost_${BOOST_VERSION}.tar.gz
RUN tar -xvzf boost_${BOOST_VERSION}.tar.gz
WORKDIR /boost_${BOOST_VERSION}
RUN ./bootstrap.sh
RUN ./b2 install -j ${GCC_THREADS} ; exit 0

# Flusspferd
RUN git clone https://github.com/kinslayermud/kinslayer-flusspferd /kinslayer-flusspferd
WORKDIR /kinslayer-flusspferd/src
RUN mkdir obj
RUN mkdir ../lib
RUN make -j${GCC_THREADS}
RUN make install

# cpp-httplib
RUN git clone https://github.com/yhirose/cpp-httplib /kinslayer-cpp-httplib
RUN cp /kinslayer-cpp-httplib/httplib.h /usr/local/include/
RUN rm -rf /kinslayer-cpp-httplib

WORKDIR /
RUN ulimit -S -c unlimited
RUN ldconfig
RUN rm -rf /boost_${BOOST_VERSION} /boost_${BOOST_VERSION}.tar.gz /kinslayer-spidermonkey /kinslayer-sqlDatabase /kinslayer-flusspferd

EXPOSE 2230
EXPOSE 2222
EXPOSE 666
EXPOSE 4001
EXPOSE 4002

ENTRYPOINT ["/kinslayer/docker-start.sh"]