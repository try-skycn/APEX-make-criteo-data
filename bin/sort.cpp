#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>

typedef unsigned int uint;

const uint MAXPOOL = 1 << 28;
const uint MAXBUFF = 256;

class Source {
private:
	FILE *fp;
public:
	Source(const char *file) {
		fp = fopen(file, "r");
	}

	bool get(uint &x) {
		return fscanf(fp, "%x", &x) != EOF;
	}

	~Source() {
		fclose(fp);
	}
};

class Target {
private:
	const char *prefix;
	uint *pool, tail;
	uint cnt;

	void flush() {
		static char file[MAXBUFF];
		if (tail) {
			std::sort(pool, pool + tail);

			sprintf(file, "%s.%d", prefix, cnt++);
			FILE *fp = fopen(file, "wb");
			fwrite((void *) pool, sizeof(uint), tail, fp);
			fclose(fp);

			tail = 0;
		}
	}

public:
	Target(const char *_prefix) : prefix(_prefix), pool(new uint[MAXPOOL]), tail(0), cnt(0) {
	}

	void push(uint x) {
		if (tail == MAXPOOL) { // full
			flush();
		}
		pool[tail++] = x;
	}

	~Target() {
		flush();

		printf("%d\n", cnt);

		delete[] pool;
		pool = 0;
	}
};

int main(int argc, const char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "No enough arguments.\n");
		return 1;
	}

	Source source(argv[1]);
	Target target(argv[2]);

	uint x = 0;
	while (source.get(x)) {
		target.push(x);
	}

	return 0;
}
