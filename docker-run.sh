#!/bin/bash

print_error() {
    echo "$1"
    exit 1
}

command -v docker >/dev/null 2>&1 || print_error "please install docker first"
container_name="tpe_arqui_2023"

if docker ps | grep "$container_name" >/dev/null 2>&1; then
    docker exec -u user -t "$container_name" \
        /bin/bash -lc 'cd /sources/Toolchain && make && cd .. && make clean all'
    [ "$?" -eq 0 ] && ./run.sh || print_error "there was an error during compilation"
else
    print_error "you have tu run ./docker-start.sh first to start the container"
fi
