#!/bin/bash
#
# A simple test to see if this serial AES implementation, unregarding its
# correctness, respects the following relation: decrypt(encrypt(data)) = data
#

function test_aes {
	echo -n "Testing $1... "
	( make clean > /dev/null && \
	make DEFINES="$2" > /dev/null && \
	./aes encrypt dummy dummy.e 192 "hola cola" > /dev/null && \
	./aes decrypt dummy.e dummy.e.d 192 "hola cola" > /dev/null && \
	diff dummy dummy.e.d ) && \
	echo "OK" || echo "KO"
}

dd if=/dev/urandom of=dummy bs=1M count=1
test_aes ShiftRows '-D SHIFT_ROWS'
test_aes SubBytes '-D SUB_BYTES'
test_aes MixColumns '-D MIX_COLUMNS'
test_aes AddRoundKey '-D ADD_ROUND_KEY'
test_aes AES
make clean
rm dummy
