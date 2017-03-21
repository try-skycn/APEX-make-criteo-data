from h5py import File
import pickle as pkl, json, numpy as np

dtypes = ["num"] + ["num" for _ in range(1, 14)] + ["cate" for _ in range(14, 40)]
index2cate = [None] + [pkl.load(open("idmap/train.{}".format(i), "rb")) for i in range(1, 40)]
sizes = [None] + [len(lst) for lst in index2cate[1:]]

train_size = 86882588
test_size = 12733079

f = File("data", "w")

print("Dumping attribute dtypes ...")
f.attrs["dtypes"] = json.dumps(dtypes)

print("Dumping attribute sizes ...")
f.attrs["sizes"] = json.dumps(sizes)

print("Dumping attribute index2cate ...")
f.attrs["index2cate"] = json.dumps(index2cate)

def dump_dataset(dsetname, dsetsize):
	dset = f.create_dataset("{}".format(dsetname), (dsetsize, 40), dtype=np.int32)
	fins = [open("index/{}.{}".format(dsetname, i), "r") for i in range(40)]
	for i, lst in enumerate(zip(*fins)):
		dset[i] = np.array([int(line.strip()) for line in lst])
		if (i + 1) % 100000 == 0:
			print("\033[K{}".format(i + 1), end='\r', flush=True)
	print("\033[K{}".format(i + 1))

print("Dumping dataset train ...")
dump_dataset("train", train_size)

print("Dumping dataset test ...")
dump_dataset("test", test_size)

f.close()
