from h5py import File
import argparse, struct, sys
import numpy as np

class MultiRead:
	def __init__(self, filelist):
		self.filelist = filelist
		self.fplist = None

	def __enter__(self):
		self.fplist = [open(file, 'rb') for file in self.filelist]
		return self
	
	def __exit__(self, *args):
		for fp in self.fplist:
			fp.close()

	def dump(self, dset, total, chunks=100000):
		bottom, up = 0, 0
		while up < total:
			up = min(up + chunks, total)
			print("{} {}".format(dset.name, up), end='\r')
			need = up - bottom
			data = [list(struct.unpack("{}I".format(need), fp.read(need * 4))) for fp in self.fplist]
			dset[bottom : up] = np.array(data).transpose()
			bottom = up
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
parser.add_argument("--chunks", type=int, default=1048576)
args = parser.parse_args()

sourcelist = [args.sourcefile.format(i) for i in range(args.fieldnum)]
with open(args.intervals, "r") as f:
	intervals = [[int(x) for x in line.strip('\n').split(' ')] for line in f]
vocabulary = intervals[-1][1]

with MultiRead(sourcelist) as fin, File(args.targetfile) as hf:
	hf.attrs["intervals"] = intervals
	hf.attrs["vocabulary"] = vocabulary

	trainset = hf.create_dataset("train", (args.train, args.fieldnum), dtype=int, chunks=(args.chunks, args.fieldnum))
	fin.dump(trainset, args.train, args.chunks)

	testset = hf.create_dataset("test", (args.test, args.fieldnum), dtype=int, chunks=(args.chunks, args.fieldnum))
	fin.dump(testset, args.test, args.chunks)
