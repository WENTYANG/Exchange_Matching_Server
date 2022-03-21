FROM ubuntu:20.04
RUN mkdir /code
ADD . /code/
WORKDIR /code

RUN ls -a && apt-get update && apt-get -y install libpam-sshauth libboost-all-dev build-essential libpqxx-dev 
#&& g++ -O3 main.cpp -std=gnu++11 -o engine -lpthread -lboost_system -lboost_thread server.cpp parser.cpp pugixml.cpp database.cpp -lpqxx -lpq
# RUN gcc -o server test.c