#!/bin/bash

print_error() {
    echo "$1"
    exit 1
}

command -v docker >/dev/null 2>&1 || print_error "please install docker first"
container_name="tpe_arqui_2023"

# if container exists then start it, otherwise initialize it
if docker ps -a | grep "$container_name" >/dev/null 2>&1; then
    docker start "$container_name"
else
    docker run -d -v ${PWD}:/sources/ --security-opt=seccomp:unconfined \
        -e 'GCC_COLORS="error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01"' \
        --name "$container_name" -it agodio/itba-so:1.0
    docker exec "$container_name" \
        /bin/bash -c "groupadd -g $(id -g) -o user && useradd -m -u $(id -u) -g $(id -g) -o -s /bin/bash user"
fi
