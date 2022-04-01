FROM ubuntu:20.04
RUN mkdir /code
ADD . /code/
WORKDIR /code

# RUN ls -a && apt-get update && apt-get -y install g++ make libpqxx-dev
# RUN make
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
RUN ls 
RUN apt-get update 
RUN apt-get -y install build-essential libpqxx-dev postgresql 
RUN chmod u+r+x run.sh



