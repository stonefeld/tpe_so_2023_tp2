#!/bin/bash

print_error() {
    echo "$1"
    exit 1
}

command -v docker >/dev/null 2>&1 || print_error "please install docker first"
docker run -v ${PWD}:/sources/ tpe_arqui_2023
./run.sh
