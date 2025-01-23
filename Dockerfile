FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    libsnmp-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /project
WORKDIR /project

RUN cmake --preset package && cmake --build --preset package

ENTRYPOINT ["cmake-build-package/src/ptpd2"]

