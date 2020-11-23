import random

from string import ascii_letters

f = open('input.txt', 'w')

f.write('32\n')


for i in range(32):
	a = random.randint(1, 100)
	f.write(''.join(random.choice(ascii_letters) for i in range(a)))
	f.write('\n')

f.close()