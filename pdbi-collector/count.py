"""
dip/acc/LHC/Beam/BPM/LSS1L/B1:errorH[]  1523461672420   null    null    {3.25961,1.59607,2.10357,1.53423,1.67253,3.72992,4.24779,2.43739}
dip/acc/LHC/Beam/BPM/LSS1L/B1:errorV[]  1523461672420   null    null    {2.73809,2.38364,2.3735,2.21393,2.2231,2.44486,4.01066,2.73912}
dip/acc/LHC/Beam/BPM/LSS1L/B1:horizontalPos[]   1523461672420   null    null    {183.639,117.053,-454.394,-837.056,-937.994,-2305.25,-1363.31,-1266.68}
dip/acc/LHC/Beam/BPM/LSS1L/B1:statusH[] 1523461672420   null    null    {0,0,0,0,0,0,0,0}
dip/acc/LHC/Beam/BPM/LSS1L/B1:statusV[] 1523461672420   null    null    {0,0,0,0,0,0,0,0}
dip/acc/LHC/Beam/BPM/LSS1L/B1:verticalPos[]     1523461672420   null    null    {-108.965,637.774,-379.901,292.604,-1459.98,-6769.76,-4131.08,-3875.39}
dip/acc/LHC/Beam/BPM/LSS1R/B1:errorH[]  1523461672420   null    null    {2.24875,2.91577,3.65429,1.73573,2.2703,2.9424,2.10534,5.39199}
dip/acc/LHC/Beam/BPM/LSS1R/B1:errorV[]  1523461672420   null    null    {2.18705,4.25172,2.44481,2.34369,3.8791,2.08235,4.73581,4.18876}
dip/acc/LHC/Beam/BPM/LSS1R/B1:horizontalPos[]   1523461672420   null    null    {-2135.93,-974.732,-1115.77,334.288,45.2941,-239.703,138.253,3569.17}
dip/acc/LHC/Beam/BPM/LSS1R/B1:statusH[] 1523461672420   null    null    {0,0,0,0,0,0,0,0}
"""

import sys

l = sys.stdin.readline()
while l and not l.startswith("*"):
	l = sys.stdin.readline()

count = 0
l = sys.stdin.readline()
while l and not l.startswith("*"):
	l = l.strip()
	words = l.split( None, 4)
	if len(words) == 5:
		scalar = words[3]
		array = words[4]
		if scalar != "null":
			count += 1
		elif array != "{}":
			count += len(array.split(","))
	l = sys.stdin.readline()

print count
