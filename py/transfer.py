from h5py import File
import argparse, struct, sys
import numpy as np

class MultiRead:
	def __init__(self, filelist):
		self.filelist = filelist
		self.fplist = [open(file, 'rb') for file in self.filelist]
		self.width = len(filelist)

	def __enter__(self):
		return self
	
	def __exit__(self, *args):
		for fp in self.fplist:
			fp.close()

	def dump(self, dset, total, chunks=100000):
		head, tail = 0, 0
		while tail < total:
			tail = min(tail + chunks, total)
			print("{} {}".format(dset.name, tail), end='\r')
			need = tail - head

			data = np.zeros(shape=(need, self.width))
			for i, fp in enumerate(self.fplist):
				data[:, i] = list(struct.unpack("{}I".format(need), fp.read(4 * need)))
			dset[head : tail] = data
			head = tail
		print()

	def __iter__(self):
		while True:
			try:
				yield np.array([struct.unpack("I", fp.read(4))[0] for fp in self.fplist])
			except struct.error:
				raise StopIteration

parser = argparse.ArgumentParser()
parser.add_argument("sourcefile")
parser.add_argument("targetfile")
parser.add_argument("--train", type=int, required=True)
parser.add_argument("--test", type=int, required=True)
parser.add_argument("--fieldnum", type=int, required=True)
parser.add_argument("--intervals", required=True)
parser.add_argument("--index", required=True)
args = parser.parse_args()

sourcelist = [args.sourcefile.format(i) for i in range(args.fieldnum)]
indexlist = [args.index.format(i) for i in range(1, args.fieldnum)]
with open(args.intervals, "r") as f:
	intervals = [[int(x) for x in line.strip('\n').split(' ')] for line in f]
vocabulary = intervals[-1][1]

indices = ['' for _ in range(vocabulary)]
for indexfile in indexlist:
	with open(indexfile, "r") as f:
		for line in f:
			pair = line.strip('\n').split(' ')
			indices[int(pair[1])] = pair[0]

with MultiRead(sourcelist) as fin, File(args.targetfile) as hf:
	hf.attrs["intervals"] = intervals
	hf.attrs["vocabulary"] = vocabulary
	hf.create_dataset("__index__", data=np.array(indices, dtype="S"))

	trainset = hf.create_dataset("train", (args.train, args.fieldnum), dtype=int)
	fin.dump(trainset, args.train)

	testset = hf.create_dataset("test", (args.test, args.fieldnum), dtype=int)
	fin.dump(testset, args.test)
