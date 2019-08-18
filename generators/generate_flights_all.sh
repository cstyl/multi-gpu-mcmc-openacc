#!/bin/sh

./flights.sh -o ../data/flights/maxabs -s MaxAbs
./flights.sh -o ../data/flights/standard -s Standard
./flights.sh -o ../data/flights/robust -s Robust
./flights.sh -o ../data/flights/normal -s Normal
./flights.sh -o ../data/flights/qgaussian -s QuantileGaussian
./flights.sh -o ../data/flights/quniform -s QuantileUniform
./flights.sh -o ../data/flights/power -s Power
