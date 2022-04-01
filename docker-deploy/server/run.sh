#!/bin/bash

make clean

make

ls -al

taskset -c 0 ./server

while true
do
    sleep 1
done