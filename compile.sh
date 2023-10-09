#!/bin/bash

print_error() {
    echo "$1"
    exit 1
}

command -v docker >/dev/null 2>&1 || print_error "please install docker first"
container_name="tpe_so_2023_tp2"

DEBUG=false
RUN=false

for i in $@; do
    case "$i" in
        "-d") DEBUG=true;;
        "-r") RUN=true;;
    esac
done

[ "$DEBUG" = true ] && makecmd="clean debug" || makecmd="clean all"
[ "$DEBUG" = true ] && runcmd="-d"

# if container doesn't exist create it
if ! docker ps -a | grep "$container_name" >/dev/null 2>&1; then
    docker run -d -v ${PWD}:/sources/ --security-opt=seccomp:unconfined \
        -e 'GCC_COLORS="error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01"' \
        --name "$container_name" -it agodio/itba-so:2.0
    docker exec "$container_name" \
        /bin/bash -c "groupadd -g $(id -g) -o user && useradd -m -u $(id -u) -g $(id -g) -o -s /bin/bash user"
fi

# start the container
docker ps | grep "$container_name" >/dev/null 2>&1 || docker start "$container_name"

# compile the code and run
docker exec -u user -t "$container_name" \
    /bin/bash -lc "cd /sources/Toolchain && make && cd .. && make $makecmd"
status="$?"
[ "$status" -eq 0 ] && [ "$RUN" = true ] && ./run.sh $runcmd
exit $status
