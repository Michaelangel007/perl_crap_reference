#!/bin/bash

# To save the timings:
#
#    benchmark.sh >& timings.txt

cp data/words.txt words.txt

 time perl src/test3.pl
 time      bin/test3opt1
 time      bin/test3opt2
 time      bin/test3opt3
 time      bin/test3opt4
 time      bin/test3opt5

 time      bin/test3opt6 -j1
 time      bin/test3opt6 -j2
 time      bin/test3opt6 -j3
 time      bin/test3opt6 -j4
 time      bin/test3opt6 -j5
 time      bin/test3opt6 -j6
 time      bin/test3opt6 -j7
 time      bin/test3opt6 -j8

