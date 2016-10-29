#include <cstdio>
#include <vector>

typedef unsigned int uint;

const int MAXBUFF = 256;

class Source {
private:
	FILE *parent;

public:
	Source(const char *file) {
		parent = fopen(file, "r");
	}

	bool get(char *str) {
		return fgets(str, MAXBUFF, parent);
	}

	~Source() {
		fclose(parent);
	}
};

class Map {
private:
	std::vector<uint> separators;
	uint start;

public:
	Map(const char *countfile, uint _start, uint threshold, const char *indexfile) : start(_start) {
		FILE *countfp = fopen(countfile, "rb");
		FILE *indexfp = fopen(indexfile, "w");

		uint mappair[2], cnt = 0;
		while (fread(mappair, sizeof(uint), 2, countfp) == 2) {
			if (cnt * 2 <= threshold || cnt + mappair[1] <= threshold) {
				if (separators.empty()) { // start
					separators.push_back(mappair[0]);
					fprintf(indexfp, "%u %u\n", mappair[0], _start);
					++_start;
				}
				cnt += mappair[1];
			} else { // new
				separators.push_back(mappair[0]);
				cnt = 0;
				fprintf(indexfp, "%u %u\n", mappair[0], _start);
				++_start;
			}
		}

		fclose(countfp);
		fclose(indexfp);

		printf("%u\n", _start);
	}

	uint get(const char *str) {
		if (*str == '\n') {
			return 0;
		}
		int x = -1;
		sscanf(str, "%d", &x);
		if (x < 0) {
			return 0;
		}
		int l = 0, r = separators.size() - 1;
		for (int mid = (l + r) >> 1; l <= r; mid = (l + r) >> 1) {
			(separators[mid] <= x) ? l = mid + 1 : r = mid - 1;
		}
		return start + r;
	}
};

class Target {
private:
	FILE *parent;

public:
	Target(const char *file) {
		parent = fopen(file, "wb");
	}

	void push(uint x) {
		fwrite(&x, sizeof(uint), 1, parent);
	}

	~Target() {
		fclose(parent);
	}
};

int main(int argc, char *argv[]) {
	if (argc < 7) {
		fprintf(stderr, "No enough arguments.\n");
		return 1;
	}

	uint start, threshold;
	sscanf(argv[3], "%u", &start);
	sscanf(argv[4], "%u", &threshold);

	Source source(argv[1]);
	Map m(argv[2], start, threshold, argv[5]);
	Target target(argv[6]);

	char str[MAXBUFF];
	while (source.get(str)) {
		target.push(m.get(str));
	}

	return 0;
}
