import math

BOARD_WIDTH = 8
table = [[0 for i in range(BOARD_WIDTH)] for i in range(BOARD_WIDTH)]

for f in range(BOARD_WIDTH):
	for r in range(BOARD_WIDTH):
		df = BOARD_WIDTH / 2 - f - 1;
		if (df < 0):
		    df = f - BOARD_WIDTH / 2;
		dr = BOARD_WIDTH / 2 - r - 1;
		if (dr < 0):
			dr = r - BOARD_WIDTH / 2;
		bonus = 1 - math.sqrt(df * df + dr * dr) / (BOARD_WIDTH / math.sqrt(2));
		table[f][r] = "%.16f" % bonus 



for f in table:

	print('{' + ', '.join(f) + '},')
