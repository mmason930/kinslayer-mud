FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

# Install pre-requisites
RUN apt update
RUN apt install libmysqlclient-dev cmake g++ gcc wget git-all -y

# sqlDatabase
WORKDIR /
RUN git clone https://github.com/kinslayermud/kinslayer-sqlDatabase
WORKDIR /kinslayer-sqlDatabase/mysql/
RUN chmod ug+x ./install.sh
RUN ./install.sh

# Spidermonkey
WORKDIR /
RUN git clone https://github.com/kinslayermud/kinslayer-spidermonkey
WORKDIR kinslayer-spidermonkey/src
RUN ./configure
RUN make -j1
RUN make install

# Boost
WORKDIR /
RUN wget http://sourceforge.net/projects/boost/files/boost/1.52.0/boost_1_52_0.tar.gz
RUN tar -xvzf boost_1_52_0.tar.gz
WORKDIR /boost_1_52_0
RUN ./bootstrap.sh
RUN ./b2 install -j 1 ; exit 0

# Flusspferd
WORKDIR /
RUN git clone https://github.com/kinslayermud/kinslayer-flusspferd
WORKDIR /kinslayer-flusspferd/src
RUN mkdir obj
RUN mkdir ../lib
RUN make -j1
RUN make install

WORKDIR /
RUN ldconfig
RUN rm -rf /boost_1_52_0 /boost_1_52_0.tar.gz /kinslayer-spidermonkey /kinslayer-sqlDatabase /kinslayer-flusspferd

EXPOSE 2230
EXPOSE 2222
EXPOSE 666
EXPOSE 4001
EXPOSE 4002

ENTRYPOINT ["/kinslayer/docker-start.sh"]

