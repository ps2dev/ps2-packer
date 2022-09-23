# First stage of Dockerfile
ARG BASE_DOCKER_IMAGE

FROM $BASE_DOCKER_IMAGE

COPY . /src

RUN apk add build-base zlib-dev
RUN cd /src && make clean all install

# Second stage of Dockerfile
FROM alpine:latest

ENV PS2DEV /usr/local/ps2dev
ENV PATH   $PATH:${PS2DEV}/ee/bin

COPY --from=0 ${PS2DEV} ${PS2DEV}
