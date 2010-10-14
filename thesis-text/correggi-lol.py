#!/usr/bin/env python
import os, re, sys

print 'Opening', sys.argv[1]
f = sys.argv[1]
t = f + '.tmp'
input = open(f, 'r');
output = open(t, 'w')
l = input.readline()
c = 'azz'
while l:
	nc = re.findall('numberline.*?{(.*?)}', l)[0].split('.')[0]
	if c != "azz" and c != nc:
		output.write('\\addvspace {10\\p@ }\n')
	if nc != c:
		c = nc
	output.write(l)
	l = input.readline()
input.close()
output.close()

os.rename(t, f)
