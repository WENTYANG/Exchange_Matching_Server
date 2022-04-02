#!/bin/bash

make clean

make

ls -al

taskset -c 0,1,2,3 ./server

while true
do
    sleep 1
done