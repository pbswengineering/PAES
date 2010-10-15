#!/usr/bin/env python
#
#    PAES - Parallel AES for CPUs and GPUs
#    Copyright (C) 2009  Paolo Bernardi <paolo.bernardi@gmx.it>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, version 2 of the License.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
##############################################################################
#
# This test checks if PAES respects the following relation, regardless of its
# adherence to the AES standard: decrypt(encrypt(data)) = data
# This relation is tested for AES as whole and for each of the 4 sub-operations.
#

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