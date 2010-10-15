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
# This is the base class for every PAES test 
# (be it a performance or correctness test or whatever....).
# In order to use it you just need to:
#   1) subclass it;
#   2) implement the test(self) method
#   3) instantiate your subclass
#   4) call the run(self) method

from datetime import datetime
from os import chdir, mkdir, popen, rmdir, system
from os.path import abspath, basename, exists
from socket import gethostname
from sys import argv, exit
from time import time
import re

class BaseTest:
	def __init__(self):
		print "\n\n"
		
		self.hostname = gethostname()
		
		# Selects the device (cpu|gpu) according to
		# user preferences or, if no specified, using
		# a default value according to the current host
		if len(argv) > 1:
			self.device = argv[1]
		else:
			# dijkstra and turing are my computers,
			# their poor old GPUs aren't supported :-)
			if self.hostname in ("dijkstra", "turing"):
				self.device = "cpu"
			else:
				self.device = "gpu"
		
		# The test's temporary directory name depends on the test script's name
		# e.g test_something.py   ---->    temp-test_something/
		# The -3 is because the .py extension is 3 characters long
		self.name = basename(argv[0])[:-3]
		self.base_dir = abspath(".")
		self.directory = abspath("temp-" + self.name)
		self.paes_dir = abspath("../paes")
		self.aes_dir = abspath("../aes")
		
		if not exists("reports"):
			mkdir("reports")
		self.report_file = "../reports/" + self.name + "_" + datetime.now().strftime("%F_%T") + ".log"
		
		# Deletes the old temporary directory, in case it's still there
		# after previous test runs
		system("rm -fr '%s'" % self.directory)
		
		try:
			mkdir(self.directory)
		except Exception as e:
			print "Unable to create the temporary test directory", self.directory
			print e
			exit(1)
		
		chdir(self.directory)
		
		print "\n\n", "-" * 60, "\n"
		print "####  RUNNING", self.name.replace("_", " ").upper(), " ####\n"
		print "DEVICE   :", self.device
		print "HOSTNAME :", self.hostname
		print "\n\n"
		
		self.echo("Test name: %s\n" % self.name.replace("_", " "))
		self.echo("Device: %s\n" % self.device)
		self.echo("Hostname: %s\n\n" % self.hostname)
		self.echo("Start time %s\n\n" % str(datetime.now()))
		
		# In case anything goes wrong, setting this variable to False
		# avoids temporary directory's deletion
		self.ok = True
	
	def compile_paes(self, operation = ""):
		print "\n\nCompiling PAES\n\n"
		chdir(self.paes_dir)
		if len(operation) > 0:
			defines = "DEFINES='-D %s'" % operation
		else:
			defines = ""
		if system("make clean && make %s" % defines) != 0:
			print "\n\nDANGER: error compiling PAES\n\n"
			chdir(self.base_dir)
			exit(2)
		system("cp paes *.cl '%s'" % self.directory)
		chdir(self.directory)

	def compile_aes(self, operation = ""):
		print "\n\nCompiling AES\n\n"
		chdir(self.aes_dir)
		if len(operation) > 0:
			defines = "DEFINES='-D %s'" % operation
		else:
			defines = ""
		if system("make clean && make %s" % defines) != 0:
			print "\n\nDANGER: error compiling AES\n\n"
			chdir(self.base_dir)
			exit(2)
		system("cp aes '%s'" % self.directory)
		chdir(self.directory)

	def cleanup(self):
		print "\n\n", "-" * 60, "\n"
		print "%s took %d seconds" % (self.name, self.elapsed)
		self.echo("\nEnd time: %s\n" % str(datetime.now()))
		self.echo("Time elapsed: %d seconds\n" % self.elapsed)
		
		chdir(self.directory + "/..")
		system("rm -f *.pyc")
		if self.ok == True:
			system("rm -fr '%s'" % self.directory)
		else:
			print "\nWARNING: something went wrong, avoiding temporary directory's deletion"
		
		print "\n\n"
		
	def run(self):
		start_time = time()
		self.test()
		self.elapsed = time() - start_time
		self.cleanup()
	
	def create_dummy(self, size):
		dummy_name = "dummy-%d" % size
		system("dd if=/dev/urandom of=%s bs=%d count=1 > /dev/null 2>&1" % (dummy_name, size))
		return dummy_name

	def paes(self, infile, outfile, mode, keysize, password):
		command = "./paes"
		command += " -i %s" % infile
		command += " -o %s" % outfile
		command += " -m %s" % mode
		command += " -k %d" % keysize
		command += " -p '%s'" % password
		command += " -d %s" % self.device
		output = popen(command).read()
		
		#   --- SAMPLE OUTPUT ---
		#
		# Encrypt time:	8625.341 ms
		# Write time:	142.132 ms
		# Read time:	94.820 ms
		
		encrypt_time = float(re.search('crypt time.*?([0-9.]*?) ms', output, re.DOTALL).group(1))
		write_time = float(re.search('Write time.*?([0-9.]*?) ms', output, re.DOTALL).group(1))
		read_time = float(re.search('Read time.*?([0-9.]*?) ms', output, re.DOTALL).group(1))

		return (encrypt_time, write_time, read_time)

	def aes(self, infile, outfile, mode, keysize, password):
		command = "./aes "
		command += " %s" % mode
		command += " %s" % infile
		command += " %s" % outfile
		command += " %d" % keysize
		command += " '%s'" % password
		output = popen(command).read()
		
		#   --- SAMPLE OUTPUT ---
		#
		# Encrypt time:	8625.341 ms
		# Write time:	142.132 ms
		# Read time:	94.820 ms
		
		encrypt_time = float(re.search('Time of computation ([0-9.]*?) ms', output, re.DOTALL).group(1))
		write_time = 0
		read_time = 0

		return (encrypt_time, write_time, read_time)
		
	def diff(self, file1, file2):
		return system("diff %s %s > /dev/null 2>&1" % (file1, file2))
		
	def echo(self, string):
		system("echo -n '%s' >> %s" % (string, self.report_file)) 
