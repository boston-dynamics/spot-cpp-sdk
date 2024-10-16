#!/usr/bin/env bash
ARCH=$(dpkg --print-architecture)

if [ "$ARCH" == "amd64" ]; then
  docker build -t spot_builder -f Dockerfile.amd64 .
elif [ "$ARCH" == "arm64" ]; then
  docker build -t spot_builder -f Dockerfile.arm64 .
else
  echo "Unknown architecture: $ARCH" > /dev/stderr
  exit 1
fi

docker run -v ~/spot-cpp-sdk:/spot-cpp-sdk spot_builder /entrypoint.sh
