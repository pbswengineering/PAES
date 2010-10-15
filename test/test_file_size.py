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
# This test checks *if* PAES works well with different file sizes; in particular
# it checks if the relation decrypt(encrypt(data)) = data holds true for
# different file sizes.
#

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