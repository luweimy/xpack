#!/bin/sh

python ../automake.py . && \
make && echo  && \
./bin/test
echo