#include <cstdio>
#include <cstring>
#include <unordered_map>

typedef unsigned int uint;

const uint MAXBUFF = 256;

class Source {
private:
	FILE *fp;

public:
	Source(const char *file) {
		fp = fopen(file, "r");
	}

	bool get(char *str) {
		return fgets(str, MAXBUFF, fp);
	}

	~Source() {
		fclose(fp);
	}
};

class Map {
private:
	std::unordered_map<uint, uint> m;

public:
	Map(const char *counterfile, const char *featmapfile, uint threshold) {
		FILE *counterfp = fopen(counterfile, "rb");
		FILE *featmapfp = fopen(featmapfile, "w");

		uint mappair[2];
		while (fread(mappair, sizeof(uint), 2, counterfp) == 2) {
			if (mappair[1] >= threshold) {
				m.insert(std::pair<uint, uint>(mappair[0], m.size() + 1));
				fprintf(featmapfp, "%08x\n", mappair[0]);
			}
		}

		fclose(counterfp);
		fclose(featmapfp);
	}

	uint get(const char *str) {
		if (*str == '\n') {
			return 0;
		}
		uint x;
		sscanf(str, "%x", &x);
		return m.count(x) ? m.at(x) : 0;
	}

	uint size() {
		return m.size();
	}
};

class Target {
private:
	FILE *fp;

public:
	Target(const char *file) {
		fp = fopen(file, "wb");
	}

	void push(uint x) {
		fwrite(&x, sizeof(uint), 1, fp);
	}

	~Target() {
		fclose(fp);
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
		fprintf(stderr, "usage: sourcefile targetfile --counter counterfile --featmap featmapfile [--threshold threshold]\n");
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
