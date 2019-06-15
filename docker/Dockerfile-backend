FROM alpine:3.8 as builder

RUN apk update
RUN apk add build-base \
            cmake \
            curl-dev \
            git \
            go \
            linux-headers \
            perl

WORKDIR /root/mavsdk

COPY . .

RUN make BUILD_BACKEND=YES INSTALL_PREFIX=/root/dcsdk_install default install

FROM alpine:3.8

RUN apk update
RUN apk add libcurl \
            libgcc \
            libstdc++

COPY --from=builder /root/dcsdk_install /usr/local
COPY --from=builder /root/mavsdk/build/default/backend/src/backend_bin /root/backend_bin

EXPOSE 14540/udp
ENTRYPOINT /root/backend_bin
