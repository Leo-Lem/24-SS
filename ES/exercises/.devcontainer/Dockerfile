FROM --platform=linux/amd64 ubuntu

# Install dependencies
SHELL ["/bin/bash", "-c"]
RUN apt-get update && apt-get install -y wget git make clang lld libc6-dev llvm

# Install wasi-libc
RUN git clone https://github.com/CraneStation/wasi-libc.git && \
    cd /wasi-libc && \
    make install INSTALL_DIR=/opt/wasi-libc && \
    rm -rf /wasi-libc

# Install libclang_rt.builtins-wasm32.a
RUN mkdir -p /usr/lib/llvm-18/lib/clang/18/lib/wasi/ && \
    wget -O /usr/lib/llvm-18/lib/clang/18/lib/wasi/libclang_rt.builtins-wasm32.a \
    https://github.com/jedisct1/libclang_rt.builtins-wasm32.a/blob/d4f1ea2bbdb0a2243363126e4e9871f9be96ff05/precompiled/llvm-18/libclang_rt.builtins-wasm32.a?raw=true

# Install arduino-cli
RUN  wget -O /tmp/cli.tar.gz https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz && \
    tar -C /usr/local/bin -zxvf /tmp/cli.tar.gz && \
    rm /tmp/cli.tar.gz