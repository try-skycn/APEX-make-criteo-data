#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <algorithm>

typedef unsigned int uint;

const uint MAXNUM = 1 << 25;

struct CntPair {
	int x;
	uint cnt;

	CntPair(int _x, uint _cnt) : x(_x), cnt(_cnt) {
	}

	bool operator<(const CntPair& r) {
		return x < r.x;
	}
};

std::unordered_map<int, uint> m;

void up(const char *file) {
	FILE *fp = fopen(file, "r");
	int x;
	while (fscanf(fp, "%d", &x) != EOF) {
		if (x < 0) {
			continue;
		}
		if (m.find(x) == m.end()) {
			m[x] = 1;
		} else {
			++m[x];
		}
	}
	fclose(fp);
}

void down(const char *file) {
	std::vector<CntPair> v;
	for (std::unordered_map<int, uint>::const_iterator it = m.begin(); it != m.end(); ++it) {
		v.push_back(CntPair(it->first, it->second));
	}

	std::sort(v.begin(), v.end());

	FILE *fp = fopen(file, "wb");
	for (std::vector<CntPair>::const_iterator it = v.begin(); it != v.end(); ++it) {
		fwrite(&it->x, sizeof(int), 1, fp);
		fwrite(&it->cnt, sizeof(uint), 1, fp);
	}
	fclose(fp);
}

struct Args {
	const char *sourcefile;
	const char *targetfile;
};

void parse_args(Args &args, int argc, char *argv[]) {
	bool is_sourcefile = false;
	bool is_targetfile = false;

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--help")) {
			throw 0;
		} else if (!is_sourcefile) {
			args.sourcefile = argv[i];
			is_sourcefile = true;
		} else if (!is_targetfile) {
			args.targetfile = argv[i];
			is_targetfile = true;
		}
	}
	if (!is_sourcefile) throw 0;
	if (!is_targetfile) throw 0;
}

int main(int argc, char *argv[]) {
	Args args;
	try {
		parse_args(args, argc, argv);
	} catch (int err) {
		fprintf(stderr, "usage: sourcefile targetfile\n");
		return 0;
	}

	up(args.sourcefile);
	down(args.targetfile);

	return 0;
}
