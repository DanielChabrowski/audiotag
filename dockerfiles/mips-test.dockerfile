FROM debian:bookworm-slim

ENTRYPOINT ["/bin/bash", "-c"]

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    git \
    cmake \
    ninja-build \
    ca-certificates \
    gcc-mips-linux-gnu \
    g++-mips-linux-gnu \
    qemu-user
