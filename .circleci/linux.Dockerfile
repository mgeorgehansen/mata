FROM debian:buster

RUN apt-get update && \
    apt-get install -y build-essential curl unzip tar git cmake && \
    rm -rf /var/lib/apt/lists/*

RUN cd /opt && \
    git clone https://github.com/Microsoft/vcpkg.git && \
    cd vcpkg && \
    ./bootstrap-vcpkg.sh && \
    ./vcpkg integrate install

RUN apt-get update && \
    apt-get install -y libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev && \
    /opt/vcpkg/vcpkg install fmt glfw3 glbinding && \
    rm -rf /var/lib/apt/lists/*
