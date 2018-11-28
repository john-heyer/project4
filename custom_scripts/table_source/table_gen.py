import math

# Scripts to compute lookup tables for mult_dist, rnk_of, and fil_of

# mult_dist
mult_dist_tbl = [[0 for i in range(10)]for j in range(10)]
for i in range(10):
    for j in range(10):
        mult_dist_tbl[i][j] = str( ( 1. / ( i + 1 ) ) * ( 1. / ( j + 1 ) ) )


# file of
fil_of_tbl = [[0 for i in range(10)]for j in range(10)]
for i in range(10):
    for j in range(10):
        s = i*10 + j
        fil_of_tbl[i][j] = str(int(math.floor((s/10)) - 1))


# rank of
rnk_of_tbl = [[0 for i in range(10)]for j in range(10)]
for i in range(10):
    for j in range(10):
        s = i + j*10
        rnk_of_tbl[i][j] = str(j - 1)

# table=mult_dist_tbl for mult dist table,
# table=fil_of_tbl for file table
# table=rnk_of_tbl for rank table
def print_tbl(table):
    for f in table:
        print('{' + ', '.join(f) + '},')

print_tbl(fil_of_tbl)