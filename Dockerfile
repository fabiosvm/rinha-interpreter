FROM alpine:latest AS builder

RUN apk add --no-cache cmake make gcc g++ bash

WORKDIR /app

COPY src/ ./src/
COPY build.sh ./
COPY CMakeLists.txt ./

RUN rm -rf build/ && ./build.sh

FROM scratch

COPY --from=builder /app/build/rinha /rinha

ENTRYPOINT ["/rinha"]
