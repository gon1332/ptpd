FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY ../ /app

RUN cmake --preset linux && cmake --build cmake-build-linux

ENTRYPOINT ["cmake-build-linux/src/ptpd"]
