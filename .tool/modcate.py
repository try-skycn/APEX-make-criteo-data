import collections

class Counter(collections.Counter):
	def add(self, element):
		super(Counter, self).update([element])

	def index2cate(self, threshold):
		return [None, ] + [x for x, cnt in self.most_common() if cnt >= threshold]

def cate2index(lst):
	return {x: i for i, x in enumerate(lst[1:], start=1)}

def catemap(m):
	return lambda x: m.get(x, 0)