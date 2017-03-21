import pickle as pkl

from collections import Counter
def count_num(infile, outfile):
	counter = Counter()
	with open(infile, "r") as fin:
		for i, line in enumerate(fin, start=1):
			if line.strip() != "":
				counter.update([int(line.strip())])
			if i % 100000 == 0:
				print("\r\033[K{}: {}".format(infile, i), end='')
	print("\r\033[K{}: {}".format(infile, i))
	with open(outfile, "wb") as fout:
		pkl.dump(counter, fout)

def count_cate(infile, outfile):
	counter = Counter()
	with open(infile, "r") as fin:
		for i, line in enumerate(fin, start=1):
			if line.strip() != "":
				counter.update([line.strip()])
			if i % 100000 == 0:
				print("\r\033[K{}: {}".format(infile, i), end='')
	print("\r\033[K{}: {}".format(infile, i))
	with open(outfile, "wb") as fout:
		pkl.dump(counter, fout)

def idmap_num(infile, outfile, threshold):
	print(infile, end='', flush=True)
	with open(infile, "rb") as fin:
		d = pkl.load(fin)
	d = list(d.items())
	d.sort(key=lambda x: x[0])
	
	rst = [None, ]
	l, tot = 0, 0
	for x, cnt in d:
		tot += cnt
		if tot >= threshold:
			rst.append((l, x + 1))
			l = x + 1
			tot = 0
	with open(outfile, "wb") as fout:
		pkl.dump(rst, fout)
	print(" Done: {}".format(len(rst)))

def idmap_cate(infile, outfile, threshold):
	print(infile, end='', flush=True)
	with open(infile, "rb") as fin:
		d = pkl.load(fin)
	rst = [None, ]
	for x, cnt in d.most_common():
		if cnt >= threshold:
			rst.append(x)
		else:
			break
	with open(outfile, "wb") as fout:
		pkl.dump(rst, fout)
	print(" Done: {}".format(len(rst)))

def index_num(infile, idmapfile, outfile):
	print(infile, end=' ', flush=True)
	with open(idmapfile, "rb") as fin:
		d = pkl.load(fin)
	d = {k: i for i, ran in enumerate(d[1:], start=1) for k in range(*ran)}
	with open(infile, "r") as fin, open(outfile, "w") as fout:
		for i, line in enumerate(fin, start=1):
			if line.strip() == '':
				print(0, file=fout)
			else:
				print(d.get(int(line.strip()), 0), file=fout)
			if i % 100000 == 0:
				print("\033[K{}".format(i), end='\033[{}D'.format(len(str(i))), flush=True)
	print("\033[KDone. {}".format(i))

def index_cate(infile, idmapfile, outfile):
	print(infile, end=' ', flush=True)
	with open(idmapfile, "rb") as fin:
		d = pkl.load(fin)
	d = {k: i for i, k in enumerate(d[1:], start=1)}
	with open(infile, "r") as fin, open(outfile, "w") as fout:
		for i, line in enumerate(fin, start=1):
			print(d.get(line.strip(), 0), file=fout)
			if i % 100000 == 0:
				print("\033[K{}".format(i), end='\033[{}D'.format(len(str(i))), flush=True)
	print("\033[KDone. {}".format(i))

