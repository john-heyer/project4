import os
import re
os.system('make clean; make')
os.system('./leiserchess test.txt > temp.txt')
f1 = open('expected.txt'); f2 = open('temp.txt');
x1 = f1.read(); x2 = f2.read();
f1.close(); f2.close();
lines1 = x1.split("\n"); lines2 = x2.split("\n");
bad = False
for i, l1 in enumerate(lines1):
	l2 = lines2[i]
	if "microsec" in l1:
		nodes1 = int(re.findall("nodes\s\d", l1)[0].split(" ")[-1])
		nodes2 = int(re.findall("nodes\s\d", l2)[0].split(" ")[-1])
		if (nodes1 != nodes2): # it's fine if nps or other things do not match
			bad = True
			print(nodes1)
			print(nodes2)
	else:
		if (l1 != l2):
			bad = True
			print(l1)
			print(l2)

if (not bad):
	print("GOOD")
else:
	print("BAD! FAILURE!")
os.remove('temp.txt')
