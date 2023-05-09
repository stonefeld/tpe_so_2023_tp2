#!/bin/sh

print_error() {
    echo "$1"
    exit 1
}

command -v docker >/dev/null 2>&1 || print_error "please install docker first"
docker build --build-arg UID=$(id -u) --build-arg GID=$(id -g) -t tpe_arqui_2023 .
echo "image ready! now run ./docker-run.sh"
