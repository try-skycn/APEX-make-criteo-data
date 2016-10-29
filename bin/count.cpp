#include <cstdio>
#include <cstring>
#include <queue>

typedef unsigned int uint;

const uint MAXFILE = 1 << 5;
const uint MAXPOOL = 1 << 20;
const uint MAXBUFF = 256;

class Source {
private:
	FILE *parent;

public:
	Source(const char *file) {
		parent = fopen(file, "rb");
	}

	bool get(uint &x) {
		uint readsize = fread(&x, sizeof(uint), 1, parent);
		return readsize == 1;
	}

	~Source() {
		fclose(parent);
	}
};

class MultiQueue {
private:
	struct FPAIR {
		uint x, fi;

		FPAIR(uint _x, uint _fi) : x(_x), fi(_fi) {
		}

		bool operator<(const FPAIR &r) const {
			return x > r.x;
		}
	};

	Source **parents;
	uint cnt;
	std::priority_queue<FPAIR> q;

	void push(int i) {
		uint x;
		if (parents[i]->get(x)) {
			q.push(FPAIR(x, i));
		}
	}

public:
	MultiQueue(const char *prefix, uint _cnt) : cnt(_cnt) {
		parents = new Source*[cnt];
		char file[MAXBUFF];
		for (int i = 0; i < cnt; ++i) {
			sprintf(file, "%s.%d", prefix, i);
			parents[i] = new Source(file);
		}

		for (int i = 0; i < cnt; ++i) {
			push(i);
		}
	}

	bool get(uint &x) {
		if (q.empty()) {
			return false;
		}

		FPAIR fpair = q.top();
		q.pop();

		x = fpair.x;

		push(fpair.fi);
		return true;
	}

	~MultiQueue() {
		for (int i = 0; i < cnt; ++i) {
			delete parents[i];
			parents[i] = 0;
		}

		delete[] parents;
		parents = 0;
	}
};

class Target {
private:
	FILE *parent;

public:
	Target(const char *file) {
		parent = fopen(file, "wb");
	}

	void push(uint x, uint cnt) {
		fwrite(&x, sizeof(uint), 1, parent);
		fwrite(&cnt, sizeof(uint), 1, parent);
	}

	~Target() {
		fclose(parent);
	}
};

class TargetCounter {
private:
	Target *parent;
	uint x, cnt;

	void flush() {
		if (cnt) {
			parent->push(x, cnt);
			x = 0;
			cnt = 0;
		}
	}

public:
	TargetCounter(const char *file) : x(0), cnt(0) {
		parent = new Target(file);
	}

	void push(uint _x) {
		if (x == _x) {
			++cnt;
		} else {
			flush();
			x = _x;
			cnt = 1;
		}
	}

	~TargetCounter() {
		flush();

		delete parent;
		parent = 0;
	}
};

int main(int argc, char *argv[]) {
	if (argc < 4) {
		fprintf(stderr, "No enough arguments.\n");
		return 1;
	}

	uint cnt;
	sscanf(argv[2], "%u", &cnt);

	MultiQueue source(argv[1], cnt);
	TargetCounter target(argv[3]);

	uint x;
	while (source.get(x)) {
		target.push(x);
	}

	return 0;
}
