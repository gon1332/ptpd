FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    autoconf \
    automake \
    libtool \
    && rm -rf /var/lib/apt/lists/*

COPY . /project
WORKDIR /project

RUN autoreconf -fi && ./configure && make

ENTRYPOINT ["src/ptpd2"]

