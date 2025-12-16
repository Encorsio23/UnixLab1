FROM ubuntu:22.04

RUN apt update && apt install -y g++ libssl-dev && apt clean

WORKDIR /app

COPY main.cpp /app/

RUN g++ -std=c++17 -o program main.cpp -lssl -lcrypto

ENTRYPOINT ["/app/program"]