#!/bin/bash
echo the number of arguments was $#

if [ $# -eq 0 ]; then
	LD_PRELOAD=./speedBoost.so ./PwnAdventure3-Linux-Shipping	
	exit 0
fi

# a self-extracting script header
#	Extract sampled from: https://stackoverflow.com/questions/29418050/package-tar-gz-into-a-shell-script

# this can be any preferred output directory
mkdir ./pwnhaxdirdontdelete

# determine the line number of this script where the payload begins
PAYLOAD_LINE=`awk '/^__PAYLOAD_BELOW__/ {print NR + 1; exit 0; }' $0`

# use the tail command and the line number we just determined to skip
# past this leading script code (Getting whatever we are storing) and pipe the payload to tar
tail -n+$PAYLOAD_LINE $0 | tar xv -C ./pwnhaxdirdontdelete

# now we are free to run code in pwnhaxdirdontdelete or do whatever we want
cd pwnhaxdirdontdelete/Inputs/Source
g++ speedBoost.cpp -shared -fPIC -o speedBoost.so
cp speedBoost.so ../../../speedBoost.so
cd ../../..
\rm -rf pwnhaxdirdontdelete

exit 0

# the 'exit 0' immediately above prevents this line from being executed
__PAYLOAD_BELOW__
