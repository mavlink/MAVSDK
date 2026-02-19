FROM debian:bookworm-slim AS builder

RUN apt-get update \
    && apt-get -y --no-install-recommends install \
        ca-certificates \
        git \
        build-essential \
        gcc \
        g++ \
        cmake \
        python3 \
        python3-pip \
        python3-future \
        python3-tqdm

COPY ./cpp /mavsdk
WORKDIR /mavsdk

ARG MAVLINK_DIALECT
ARG BUILD_THREADS=6

RUN cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=install \
        -DBUILD_MAVSDK_SERVER=ON \
        -DSUPERBUILD=ON \
        -DBUILD_TESTING=OFF \
        -DMAVLINK_DIALECT=${MAVLINK_DIALECT} \
        -Bbuild -S. \
        && cmake --build build -j${BUILD_THREADS} --target install

FROM debian:bookworm-slim

WORKDIR /mavsdk
COPY --from=builder /mavsdk/install/bin /mavsdk/bin
COPY --from=builder /mavsdk/install/lib /mavsdk/lib
ENV LD_LIBRARY_PATH=/mavsdk/lib
ENTRYPOINT ["/mavsdk/bin/mavsdk_server"]
