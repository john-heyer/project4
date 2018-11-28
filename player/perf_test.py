import os
import re
import sys
import numpy as np

# EXAMPLE USAGE: python perf_test.py false 
#			or : python perf_test.py true
# (no args remakes by default)

SCALE_DOWN_FACTOR = 10000.0

regression_binary = './leiserchess_beta1_v2'
binary = './leiserchess'

if len(sys.argv) == 0:
	os.system('make clean; make')
elif sys.argv[1] == 'true':
	os.system('make clean; make')


print ("Running test...")	

os.system(binary + ' perf_test.txt > temp_binary.txt')
os.system(regression_binary + ' perf_test.txt > temp_regression_binary.txt')
f1 = open('temp_regression_binary.txt'); f2 = open('temp_binary.txt');
x1 = f1.read(); x2 = f2.read();
f1.close(); f2.close();
lines1 = x1.split("\n"); lines2 = x2.split("\n");
bad = False

nps_regression = []
nps_new = []

def geo_mean(iterable):
    a = np.array(iterable)
    return a.prod()**(1.0/len(a))

for i, l1 in enumerate(lines1):
	l2 = lines2[i]
	if "microsec" in l1:
		nodes1 = int(re.findall("nodes\s[0-9]+", l1)[0].split(" ")[-1])
		nodes2 = int(re.findall("nodes\s[0-9]+", l2)[0].split(" ")[-1])
		print ("nodes 1 here: ")
		print(nodes1)
		print ("nodes 2 here: ")
		print(nodes2)
		print (" ++++++++")
		nps_regression.append(int(re.findall("nps\s[0-9]+", l1)[0].split(" ")[-1])/SCALE_DOWN_FACTOR)
		print(nps_regression[-1])
		nps_new.append(int(re.findall("nps\s[0-9]+", l2)[0].split(" ")[-1])/SCALE_DOWN_FACTOR)
		print(nps_new[-1])
		if (nodes1 != nodes2): # it's fine if nps or other things do not match
			bad = True
			print(nodes1)
			print(nodes2)
	else:
		if (l1 != l2):
			bad = True
			print(l1)
			print(l2)

geo_mean_regression = geo_mean(nps_regression)
geo_mean_new = geo_mean(nps_new)
percentage_change = (float(geo_mean_new) / float(geo_mean_regression))* 100  			

if (not bad):
	print("========== REGRESSION PASSED ==========")
	print("OLD GEO MEAN NPS: " + str(geo_mean_regression))
	print("NEW GEO MEAN NPS: " + str(geo_mean_new))
	print(str(percentage_change - 100) + " % Change")
else:
	print("========== REGRESSION FAILURE ==========")
	print("OLD GEO MEAN NPS: " + str(geo_mean_regression))
	print("NEW GEO MEAN NPS: " + str(geo_mean_new))
	print(str(percentage_change - 100) + " % Change")

os.remove('temp_binary.txt')
os.remove('temp_regression_binary.txt')
