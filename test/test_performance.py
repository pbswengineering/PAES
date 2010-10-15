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
# This test checks PAES performances for different file sizes and for each of
# the 3 standard key lenghts; it tests both encryption and decryption times.
#

from common import BaseTest
from os import system
from time import sleep

KEY_SIZE = 192

class TestPerformance(BaseTest):
	def test(self):
		self.echo("Key size = %d\n\n" % KEY_SIZE)
		print "Key size = %d" % KEY_SIZE
		
		self.compile_paes()
		sizes = (128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 234700800)
		files = []
		print "\n\nCreating dummy files...\n\n"
		for s in sizes:
			files.append(self.create_dummy(s))
			print "   ", s
		
		sleep(10)
		
		self.echo("{0:<15} {1:>15} {2:>15} {3:>15} {4:>15}\n\n".format("Size", "Encrypt", "Decrypt", "Read", "Write"))
		print "\n\n{0:<15} {1:>15} {2:>15} {3:>15} {4:>15}\n".format("Size", "Encrypt", "Decrypt", "Read", "Write")
		
		for s, f in zip(sizes, files):
			e = f + ".e"
			d = f + ".d"
			wt = rt = ""
			self.echo("{0:<15}".format(s))
			print "{0:<15}".format(s),
			try:
				(et, wt1, rt1) = self.paes(f, e, "encrypt", KEY_SIZE, "hola cola")
				(dt, wt2, rt2) = self.paes(e, d, "decrypt", KEY_SIZE, "hola cola")
				
				if self.diff(f, d) == 0:
					res = "ok"
				else:
					res = "ko"
					
				wt = (float(wt1) + float(wt2)) / 2
				rt = (float(rt1) + float(rt2)) / 2
				
			except Exception as e:
				print "EXCEPTION:", e
				self.echo("\n\nEXCEPTION: %s\n" % str(e))
				res = "ko"
				
			# Avoids temporary directory's deletion
			if res == "ko":
				self.ok = False
				
			et = float(et)
			dt = float(dt)
			self.echo("{0:>15.3f} {1:>15.3f} {2:>15.3f} {3:>15.3f}\n".format(et, dt, wt, rt))
			print "{0:>15.3f} {1:>15.3f} {2:>15.3f} {3:>15.3f}".format(et, dt, wt, rt)
			
			sleep(20)
# The main code
TestPerformance().run()