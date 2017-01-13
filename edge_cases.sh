#!/bin/bash

FILE=data/signed_chars.txt
echo "File: ${FILE}"
echo "Description: text file with negative chars"
echo "Expected: "
 sort -u ${FILE} | wc -l
 bin/test3opt3 ${FILE}
 bin/test3opt4 ${FILE}
 bin/test3opt5 ${FILE}
 bin/test3opt6 ${FILE}

FILE=data/5.txt
echo "File: ${FILE}"
echo "Description: 4 + 1 blank line"
echo "Expected: "
 sort -u ${FILE} | wc -l
 bin/test3opt3 ${FILE}
 bin/test3opt4 ${FILE}
 bin/test3opt5 ${FILE}
 bin/test3opt6 ${FILE}

FILE=data/9.txt
echo "File: ${FILE}"
echo "Description: 8 + 1 blank line"
echo "Expected: "
 sort -u ${FILE} | wc -l
 bin/test3opt3 ${FILE}
 bin/test3opt4 ${FILE}
 bin/test3opt5 ${FILE}
 bin/test3opt6 ${FILE}

FILE=data/2852.txt
echo "File: ${FILE}"
echo "Description: Sherlock Holmes"
echo "Expected:"
 sort -u ${FILE} | wc -l
 bin/test3opt3 ${FILE}
 bin/test3opt4 ${FILE}
 bin/test3opt5 ${FILE}
 bin/test3opt6 ${FILE}

