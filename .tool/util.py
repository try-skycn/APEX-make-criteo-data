import sys

def enum(gen, start=1):
	for i, x in enumerate(gen, start=start):
		print("\033[K" + str(i), end='\r', flush=True, file=sys.stderr)
		yield i, x
	print("\033[K" + str(i), file=sys.stderr)