# kinslayer-mud

## Start game container
./docker-compose up game

## Incremental build
docker exec -it game.kinslayermud.org bash -c 'make -j${GCC_THREADS} -C /kinslayer/src'

## Full build
docker exec -it game.kinslayermud.org bash -c 'make clean -C /kinslayer/src && make -j${GCC_THREADS} -C /kinslayer/src'

## Stop game container
./docker-compose down game