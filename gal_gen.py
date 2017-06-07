from itertools import product

import sys

sys.argv

x = int(sys.argv[1])
y = int(sys.argv[2])

n = x * y

mass1 = 1
mass2 = 25

vx1=1
vy1=1
vx2=-3
vy2=3

start_diff = int(sys.argv[3])

with open('gal_{}_{}_{}_1.txt'.format(x, y, start_diff), 'w') as f1:
    with open('gal_{}_{}_{}_2.txt'.format(x, y, start_diff), 'w') as f2:
        f1.write('{}\n{} {}\n{}\n'.format(n, vx1, vy1, mass1))
        f2.write('{}\n{} {}\n{}\n'.format(n, vx2, vy2, mass2))

        for i, j in product(range(start_diff, x + start_diff), range(start_diff+5, y + start_diff+5)):
            f1.write('{} {}\n'.format(i, j))
            f2.write('-{}.5 -{}.5\n'.format(i, j))
