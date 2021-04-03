FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

# Install pre-requisites
RUN apt update
RUN apt install libmysqlclient-dev -y
RUN apt install cmake g++ gcc python wget git-all -y

# Boost
RUN wget http://sourceforge.net/projects/boost/files/boost/1.52.0/boost_1_52_0.tar.gz
RUN tar -xvzf boost_1_52_0.tar.gz
WORKDIR /boost_1_52_0
RUN ./bootstrap.sh
RUN ./b2 install -j 12 ; exit 0

# Spidermonkey
WORKDIR /
RUN git clone https://github.com/kinslayermud/kinslayer-spidermonkey
WORKDIR kinslayer-spidermonkey/src
RUN ./configure
RUN make -j12
RUN make install

# sqlDatabase
WORKDIR /
RUN git clone https://github.com/kinslayermud/kinslayer-sqlDatabase
WORKDIR /kinslayer-sqlDatabase/mysql/
RUN chmod ug+x ./install.sh
RUN ./install.sh

# Flusspferd
WORKDIR /
RUN git clone https://github.com/kinslayermud/kinslayer-flusspferd
WORKDIR /kinslayer-flusspferd/src
RUN mkdir obj
RUN mkdir ../lib
RUN make -j12
RUN make install

WORKDIR /
ADD . /kinslayer
RUN chmod ug+x /kinslayer/docker-start.sh

RUN ldconfig
RUN ln -s /kinslayer/scripts /kinslayer/lib/scripts

WORKDIR /kinslayer
RUN rm -rf /boost_1_52_0 /boost_1_52_0.tar.gz /kinslayer-spidermonkey /kinslayer-sqlDatabase /kinslayer-flusspferd

EXPOSE 2230
EXPOSE 2222
EXPOSE 666
EXPOSE 4001
EXPOSE 4002

# ENTRYPOINT ["/kinslayer/docker-start.sh"]
ENTRYPOINT ["sleep", "100000"]

