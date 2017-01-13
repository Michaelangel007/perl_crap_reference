#!/bin/bash

# Copied from:
# https://raw.githubusercontent.com/crioux/turbo-linecount/master/tests/create_testfiles.sh
# Added directory

mkdir -p rnd_data
cd       rnd_data

dd if=/dev/urandom of=test_10MB.txt  bs=1000000 count=10
dd if=/dev/urandom of=test_100MB.txt bs=1000000 count=100
dd if=/dev/urandom of=test_1GB.txt   bs=1000000 count=1000
cat test_1GB.txt test_1GB.txt test_1GB.txt test_1GB.txt test_1GB.txt test_1GB.txt test_1GB.txt test_1GB.txt test_1GB.txt test_1GB.txt > test_10GB.txt

cd ..

