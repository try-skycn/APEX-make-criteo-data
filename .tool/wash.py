import sys, util, argparse, random

def parse_args():
	parser = argparse.ArgumentParser()
	parser.add_argument("negtive_filter", type=float)
	return parser.parse_args()

args = parse_args()

for _, line in util.enum(sys.stdin):
	lst = line.strip().split('\t')
	if lst[8] == '-1':
		lst[8] = ''
	if lst[0] == '1' or random.random() < args.negtive_filter:
		print('\t'.join(lst))