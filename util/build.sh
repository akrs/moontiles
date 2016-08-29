#!/bin/sh
make clean
make
./moontool > ../src/c/moonphase.h
