#include <cstdio>
#include <cstring>
#include <unordered_map>

typedef unsigned int uint;

const uint MAXBUFF = 256;

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
	std::unordered_map<uint, uint> m;

public:
	Map(const char *countfile, uint start, uint threshold, const char *indexfile) {
		FILE *countfp = fopen(countfile, "rb");
		FILE *indexfp = fopen(indexfile, "w");

		uint mappair[2], cnt = 0;
		while (fread(mappair, sizeof(uint), 2, countfp) == 2) {
			if (mappair[1] >= threshold) {
				m.insert(std::pair<uint, uint>(mappair[0], start));
				fprintf(indexfp, "%08x %u\n", mappair[0], start);
				++start;
			}
		}

		fclose(countfp);
		fclose(indexfp);

		printf("%u\n", start);
	}

	uint get(const char *str) {
		if (*str == '\n') {
			return 0;
		}
		uint x;
		sscanf(str, "%x", &x);
		return m.count(x) ? m.at(x) : 0;
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
