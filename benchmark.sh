#!/bin/bash

# To save the timings:
#
#    benchmark.sh >& timings.txt

cp data/words.txt words.txt

 echo -e "\nFile: test3.pl"; time perl src/test3.pl
 echo -e "\nFile: test3v1 "; time      bin/test3opt1
 echo -e "\nFile: test3v2 "; time      bin/test3opt2
 echo -e "\nFile: test3v3 "; time      bin/test3opt3
 echo -e "\nFile: test3v4 "; time      bin/test3opt4
 echo -e "\nFile: test3v5 "; time      bin/test3opt5

 echo -e "\n";               time      bin/test3opt6 -j1
 echo -e "\n";               time      bin/test3opt6 -j2
 echo -e "\n";               time      bin/test3opt6 -j3
 echo -e "\n";               time      bin/test3opt6 -j4
 echo -e "\n";               time      bin/test3opt6 -j5
 echo -e "\n";               time      bin/test3opt6 -j6
 echo -e "\n";               time      bin/test3opt6 -j7
 echo -e "\n";               time      bin/test3opt6 -j8

