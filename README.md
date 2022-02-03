# kinslayer-mud
This repository hosts the core game code for KinslayerMUD(http://www.kinslayermud.org) as well as runtime scripts. The game can be compiled and run natively on your machine, assuming you install all required dependencies(see `Dockerfile`). However, the recommended approach is to use Docker and Docker Compose for simplicity. This document will walk through that process.

Note that in order to use Docker Compose, you'll need to clone the following repository: https://github.com/kinslayermud/kinslayer-loader

All commands in this document should be executed from the kinslayer-loader directory.

## Start game container
The simplest way to star the container is to run the following:
```
./docker-compose up game
```

By default, the container will perform a clean build of the game, resulting in a full compilation of the game code.

Becuase this is time consuming, you can alternatively run the container and perform a partial build:

```
./docker-compose run game gateway partial
```

Note that running docker-compose run may change the name of the running container(from game.kinslayermud.org below). You can run docker container ls to get the actual name of the container if you need it for subsequent commands.

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
./docker-compose down game
```