#!/usr/bin/env python

from common import BaseTest
from os import system
from random import randint

class TestFileSize(BaseTest):
	def test(self):
		MAX_SIZE = 128 #2048
		self.compile_paes()
		system("rm -f testfile ; echo X > testfile")
		size = 1
		while size < MAX_SIZE:
			try:
				self.paes("testfile", "testfile.e", "encrypt", 192, "hola cola")
				self.paes("testfile.e", "testfile.e.d", "decrypt", 192, "hola cola")
				if self.diff("testfile", "testfile.e.d") == 0:
					res = "ok"
				else:
					res = "ko"
			except:
				res = "ko"
			
			# Avoids temporary directory's deletion
			if res == "ko":
				self.ok = False
			
			print "%d %s" % (size, res)
			self.echo("%d %s\n" % (size, res))
			size = size + 1
			if size < MAX_SIZE:
				system("echo -n '%s' >> testfile" % chr(ord('0') + randint(0, 9)))

TestFileSize().run()