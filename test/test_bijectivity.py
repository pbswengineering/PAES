#!/usr/bin/env python

from common import BaseTest
from os import system

class TestBijectivity(BaseTest):
	def test(self):
		self.compile_paes()
		for size in (128, 1024, 1048576, 8388608, 67108864):
			print "%d" % size,
			self.echo("%d" % size)
			
			clearfile_in = self.create_dummy(size)
			cypherfile = clearfile_in + ".e"
			clearfile_out = cypherfile + ".d"
			try:
				self.paes(clearfile_in, cypherfile, "encrypt", 192, "hola cola")
				self.paes(cypherfile, clearfile_out, "decrypt", 192, "hola cola")
				if self.diff(clearfile_in, clearfile_out) == 0:
					res = "ok"
				else:
					res = "ko"
			except Exception as e:
				print "EXCEPTION:", e
				self.echo("\n\nEXCEPTION: %s\n" % str(e))
				res = "ko"
				
			# Avoids temporary directory's deletion
			if res == "ko":
				self.ok = False
				
			print res
			self.echo(" %s\n" % res)

TestBijectivity().run()