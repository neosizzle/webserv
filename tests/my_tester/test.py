import sys
import os
import time
import subprocess

RESET =  "\033[0m"
BLACK =  "\033[30m"
RED =    "\033[31m"
GREEN =  "\033[32m"
YELLOW = "\033[33m"
BLUE =   "\033[34m"
MAGENTA ="\033[35m"
CYAN =   "\033[36m"
WHITE =  "\033[37m"
BOLDBLACK =  "\033[1m\033[30m"      
BOLDRED =    "\033[1m\033[31m"      
BOLDGREEN =  "\033[1m\033[32m"      
BOLDYELLOW = "\033[1m\033[33m"      
BOLDBLUE =   "\033[1m\033[34m"      
BOLDMAGENTA ="\033[1m\033[35m"      
BOLDCYAN =   "\033[1m\033[36m"
BOLDWHITE =  "\033[1m\033[37m"

#Custom tester that tests for config parsing of webserv

#tests good configs
def test_good_cfg(path, webserv):
	og_dir = os.getcwd()
	os.chdir("/".join(webserv.split("/")[:-1]))
	p = subprocess.Popen([webserv, path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	time.sleep(1)
	if p.poll() is None:
		#webserv successfully execute
		p.kill()
		print(f'{BOLDGREEN}[webserv tester]{RESET + GREEN} {path.split("/")[-1]} OK {RESET}')
	else:
		#webserv didnt execute
		print(f'{BOLDRED}[webserv tester]{RESET + RED} {path.split("/")[-1]} KO [webserv crash] {RESET}')
	os.chdir(og_dir)

#tests bad configs
def test_bad_cfg(path, webserv):
	og_dir = os.getcwd()
	os.chdir("/".join(webserv.split("/")[:-1]))
	p = subprocess.Popen([webserv, path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	time.sleep(1)
	if p.poll() is None:
		#webserv successfully execute
		p.kill()
		print(f'{BOLDRED}[webserv tester]{RESET + RED} {path.split("/")[-1]} KO [webserv didnt crash] {RESET}')
	else:
		#webserv didnt execute
		print(f'{BOLDGREEN}[webserv tester]{RESET + GREEN} {path.split("/")[-1]} OK {RESET}')

	os.chdir(og_dir)

#get argv
if len(sys.argv) != 2 :
	print("Usage: python test.py [Path to webserv executable]")
	exit()

webserv = sys.argv[1]

print(f'{BOLDBLUE}[webserv tester]{RESET + BLUE} testing good cfgs.. {webserv} {RESET}')
if not os.path.exists(webserv):
	print(f'{BOLDRED}[webserv tester]{RESET + RED} webserv not found {webserv} {RESET}')
	exit()

for filename in os.listdir("./good_configs"):
	cfgpath = os.getcwd() + "/good_configs/" + filename
	test_good_cfg(cfgpath, webserv)

print(f'{BOLDBLUE}[webserv tester]{RESET + BLUE} testing bad cfgs.. {webserv} {RESET}')
for filename in os.listdir("./bad_configs"):
	cfgpath = os.getcwd() + "/bad_configs/" + filename
	test_bad_cfg(cfgpath, webserv)
