#!/bin/sh

# This is a short-cut to execute tests on the parser.
# ./conf/torture_msgs{,2} contain various kind of messages.

echo "=="
echo "== Testing the parser contained in libosip."
echo "== You can use the verbose mode (-v) to see the full message."
echo "=="

echo "=="
echo "== Test for \\n (CR) and \\r (LF) instead of \\r\\n (CRLF)"
echo "== Data files from ./conf/torture_msgs"
echo "=="

i=0
while [ $i -lt 93 ]
do
    test/torture_test conf/torture_msgs $i "$1"
    i=`expr $i + 1`
done

echo "=="
echo "== Test for \\n (CR) and \\r (LF) instead of \\r\\n (CRLF)"
echo "== Data files from ./conf/torture_msgs"
echo "=="


i=0
while [ $i -lt 93 ]
do
    test/torture_test conf/torture_msgs2 $i "$1"
    i=`expr $i + 1`
done


