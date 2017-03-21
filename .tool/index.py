import argparse, util, sys, json
import modlabel, modnum, modcate
from h5py import File
import numpy as np

def parse_args():
	parser = argparse.ArgumentParser()
	parser.add_argument("source")
	parser.add_argument("target")
	parser.add_argument("--threshold", type=int)
	parser.add_argument("--map", dest="maps", action="append", nargs=2)
	return parser.parse_args()

args = parse_args()

mods = [modlabel] + [modnum for i in range(1, 14)] + [modcate for i in range(14, 40)]
counters = [mod.Counter() for mod in mods]

print("Reading source file ...", file=sys.stderr)
with open(args.source, "r") as f:
	for _, line in util.enum(f):
		for counter, x in zip(counters, line.strip().split('\t')):
			counter.add(x)

print("Making index map ...", file=sys.stderr)
index2cate_maps = [counter.index2cate(args.threshold) for counter in counters]
del counters
catemaps = [mod.catemap(mod.cate2index(lst)) for mod, lst in zip(mods, index2cate_maps)]

def count_lines(filename):
	print("Counting lines for file " + filename + " ...", file=sys.stderr)
	with open(filename, "r") as f:
		for i, _ in util.enum(f): pass
	return i

def indexfile(filename, dsetname, ftarget):
	size = count_lines(filename)
	print("Indexing file " + filename + " ...", file=sys.stderr)
	dset = ftarget.create_dataset(dsetname, [size, len(mods)], dtype=np.int32)
	with open(filename, "r") as f:
		for i, line in util.enum(f):
			dset[i - 1] = np.array([m(x) for m, x in zip(catemaps, line.strip().split('\t'))])

ftarget = File(args.target, "w")

print("Dumping attribute sizes ...")
ftarget.attrs["sizes"] = json.dumps([mod.size(lst) for mod, lst in zip(mods, index2cate_maps)])
print("Dumping attribute dtypes ...")
ftarget.attrs["dtypes"] = json.dumps([mod.dtype() for mod in mods])
print("Dumping attribute index2cate ...")
ftarget.attrs["index2cate"] = json.dumps(index2cate_maps)

for filename, dsetname in args.maps:
	indexfile(filename, dsetname, ftarget)
ftarget.close()
