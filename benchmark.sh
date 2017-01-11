#!/bin/bash

# test3.pl 0.060s
# test3v2  0.906s
# test3v3  0.911s
# test3v4  0.185s
# test3v5  0.183s

cp data/words.txt words.txt

time perl src/test3.pl
time      bin/test3opt2
time      bin/test3opt3
time      bin/test3opt4
time      bin/test3opt5

