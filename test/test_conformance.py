#!/usr/bin/env python

from common import BaseTest
from os import system

class TestConformance(BaseTest):
	def test(self):
		size = 1048576 + 16 * 1022
		d = {"ShiftRows": "SHIFT_ROWS",
			"SubBytes": "SUB_BYTES",
			"MixColumns": "MIX_COLUMNS",
			"AddRoundKey": "ADD_ROUND_KEY",
			"AES": ""}
		for mode in ("encrypt", "decrypt"):
			print "AES MODE: %s" % mode
			self.echo("\nAES mode: %s\n\n" % mode)
			for name, operation in d.items():
				self.compile_paes(operation)
				self.compile_aes(operation)
				print "%s" % name,
				self.echo("%s" % name)
				
				clearfile = self.create_dummy(size)
				cypherfile_a = clearfile + ".aes"
				cypherfile_b = clearfile + ".paes"
				try:
					self.aes(clearfile, cypherfile_a, mode, 192, "hola cola")
					self.paes(clearfile, cypherfile_b, mode, 192, "hola cola")
					if self.diff(cypherfile_a, cypherfile_b) == 0:
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

TestConformance().run()
