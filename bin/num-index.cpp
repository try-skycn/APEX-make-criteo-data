#include <cstdio>
#include <vector>
#include <cstring>

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

	void push(uint x, uint num, uint threshold, uint &cnt) {
		if (separators.empty() || !(cnt * 2 <= threshold || cnt + num <= threshold)) { // new
			separators.push_back(x);
			cnt = num;
		} else { // new
			cnt += num;
		}
	}

public:
	Map(const char *counterfile, const char *featmapfile, uint threshold) {
		FILE *counterfp = fopen(counterfile, "rb");

		uint mappair[2], cnt = 0;
		while (fread(mappair, sizeof(uint), 2, counterfp) == 2) {
			push(mappair[0], mappair[1], threshold, cnt);
		}

		fclose(counterfp);

		FILE *featmapfp = fopen(featmapfile, "w");

		for (std::vector<uint>::const_iterator it = separators.begin(); it != separators.end(); ++it) {
			fprintf(featmapfp, "%u\n", *it);
		}

		fclose(featmapfp);
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
		return r + 1;
	}

	uint size() {
		return separators.size();
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

struct Args {
	const char *sourcefile;
	const char *counterfile;
	const char *featmapfile;
	const char *targetfile;
	uint threshold;
};

void parse_args(Args &args, int argc, char *argv[]) {
	// default values
	args.threshold = 0;

	bool is_sourcefile = false;
	bool is_counterfile = false;
	bool is_featmapfile = false;
	bool is_targetfile = false;

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--help")) {
			throw 0;
		} else if (!strcmp(argv[i], "--threshold")) {
			if (!(i + 1 < argc)) throw 0;
			sscanf(argv[++i], "%u", &args.threshold);
		} else if (!strcmp(argv[i], "--featmap")) {
			if (!(i + 1 < argc)) throw 0;
			args.featmapfile = argv[++i];
			is_featmapfile = true;
		} else if (!strcmp(argv[i], "--counter")) {
			if (!(i + 1 < argc)) throw 0;
			args.counterfile = argv[++i];
			is_counterfile = true;
		} else if (!is_sourcefile) {
			args.sourcefile = argv[i];
			is_sourcefile = true;
		} else if (!is_targetfile) {
			args.targetfile = argv[i];
			is_targetfile = true;
		}
	}

	if (!is_sourcefile) throw 0;
	if (!is_counterfile) throw 0;
	if (!is_featmapfile) throw 0;
	if (!is_targetfile) throw 0;
}

int main(int argc, char *argv[]) {
	Args args;
	try {
		parse_args(args, argc, argv);
	} catch (int err) {
		fprintf(stderr, "usage: sourcefile targetfile --counter counterfile --featmap featmapfile --threshold threshold\n");
		fprintf(stderr, "print: num of categories\n");
		return 0;
	}

	Source source(args.sourcefile);
	Map m(args.counterfile, args.featmapfile, args.threshold);
	Target target(args.targetfile);

	char str[MAXBUFF];
	while (source.get(str)) {
		target.push(m.get(str));
	}

	printf("%u\n", m.size());

	return 0;
}
