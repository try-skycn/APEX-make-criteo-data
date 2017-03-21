import collections

class Counter(collections.Counter):
	def add(self, element):
		super(Counter, self).update([element])

	def index2cate(self, threshold):
		lst = list(self.items())
		lst.sort(key=lambda x: x[0])

		rst = [None, ]
		l, tot = 0, 0
		for x, cnt in lst:
			tot += cnt
			if tot >= threshold:
				rst.append((l, x + 1))
				l, tot = x + 1, 0
		return rst

def cate2index(lst):
	return {j: i for i, (l, r) in enumerate(lst[1:], start=1) for j in range(l, r)}

def catemap(m):
	return lambda x: m.get(int(x), 0) if x != "" else 0

def size(m):
	return len(m)

def dtype():
	return "cate"