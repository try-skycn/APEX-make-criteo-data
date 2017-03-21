import sys

def showenum(gen, start=1):
	for i, x in enumerate(gen, start=start):
		print("\033[K" + str(i), end='\r', flush=True, file=stderr)
		yield i, x
	print("\033[K" + str(i), file=sys.stderr)