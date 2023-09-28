#!/usr/bin/env bash

cmake -B build -DCMAKE_BUILD_TYPE=Release -DTAGGED_POINTER=1 -DTAIL_CALL_SWITCH=0
cmake --build build
