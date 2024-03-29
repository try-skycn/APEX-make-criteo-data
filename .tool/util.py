import sys

def enum(gen, start=1, loop=100000):
	for i, x in enumerate(gen, start=start):
		if i == 1 or i % loop == 0:
			print("\033[K" + str(i), end='\r', flush=True, file=sys.stderr)
		yield i, x
	print("\033[K" + str(i), file=sys.stderr)