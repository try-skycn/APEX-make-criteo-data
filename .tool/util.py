import sys

def showenum(gen, start=1):
	for i, x in enumerate(gen, start=start):
		print("\033[K" + i, end='\r', flush=True, file=stderr)
		yield i, x
	print("\033[K" + i, file=sys.stderr)