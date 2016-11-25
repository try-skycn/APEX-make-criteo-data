#include <cstdio>
#include <cstring>
#include <vector>
#include <queue>
#include <algorithm>

typedef unsigned int uint;

const uint MAXPOOL = 1 << 28;
const uint MAXBUFF = 256;

void dump(uint *pool, uint &head, const char *tmpdir, uint &cnt) {
	static char file[MAXBUFF];
	if (head) { // if something to dump
		std::sort(pool, pool + head);
		sprintf(file, "%s/%d", tmpdir, cnt++);
		FILE *fp = fopen(file, "wb");
		fwrite(pool, sizeof(uint), head, fp);
		fclose(fp);
		head = 0; // clear the pool
	}
}

uint divide(const char *sourcefile, const char *tmpdir, uint pool_size) {
	uint *pool = new uint[pool_size], head = 0;

	FILE *fp = fopen(sourcefile, "r");

	uint x = 0, cnt = 0;
	while (fscanf(fp, "%x", &x) != EOF) {
		pool[head++] = x;
		if (head == pool_size) {
			dump(pool, head, tmpdir, cnt);
		}
	}
	dump(pool, head, tmpdir, cnt);

	fclose(fp);

	delete[] pool;

	return cnt;
}

struct FPAIR {
	uint x, fi;

	FPAIR(uint _x, uint _fi) : x(_x), fi(_fi) {
	}

	bool operator<(const FPAIR &r) const {
		return x > r.x;
	}
};

void merge(const char *tmpdir, uint cnt, const char *targetfile) {
	char file[MAXBUFF];
	FILE **tmpfps = new FILE *[cnt], *fp = fopen(targetfile, "wb");
	for (int i = 0; i < cnt; ++i) {
		sprintf(file, "%s/%d", tmpdir, i);
		tmpfps[i] = fopen(file, "rb");
	}

	std::priority_queue<FPAIR> q;
	uint x;
	for (int i = 0; i < cnt; ++i) {
		if (fread(&x, sizeof(uint), 1, tmpfps[i])) {
			q.push(FPAIR(x, i));
		}
	}

	uint nowx = 0, num = 0;
	bool start = true;
	while (!q.empty()) {
		FPAIR fpair = q.top();
		q.pop();

		if (start) {
			nowx = fpair.x;
			num = 1;
			start = false;
		} else if (nowx == fpair.x) {
			++num;
		} else {
			fwrite(&nowx, sizeof(uint), 1, fp);
			fwrite(&num, sizeof(uint), 1, fp);
			nowx = fpair.x;
			num = 1;
		}

		if (fread(&x, sizeof(uint), 1, tmpfps[fpair.fi])) {
			q.push(FPAIR(x, fpair.fi));
		}
	}
	fwrite(&nowx, sizeof(uint), 1, fp);
	fwrite(&num, sizeof(uint), 1, fp);

	fclose(fp);
	for (int i = 0; i < cnt; ++i) {
		fclose(tmpfps[i]);
	}
	delete[] tmpfps;
}

struct Args {
	const char *sourcefile;
	const char *targetfile;
	const char *tmpdir;
	uint pool_size;
};

void parse_args(Args &args, int argc, const char *argv[]) {
	// default values
	args.tmpdir = ".tmp";
	args.pool_size = 1 << 28;

	bool is_sourcefile = false;
	bool is_targetfile = false;
	bool is_tmpdir = false;

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--help")) {
			throw 0;
		} else if (!strcmp(argv[i], "--tmp")) {
			if (!(i + 1 < argc)) throw 0;
			args.tmpdir = argv[++i];
			is_tmpdir = true;
		} else if (!strcmp(argv[i], "--pool")) {
			if (!(i + 1 < argc)) throw 0;
			sscanf(argv[++i], "%u", &args.pool_size);
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
	if (!is_tmpdir) throw 0;
}

int main(int argc, const char *argv[]) {
	Args args;
	try {
		parse_args(args, argc, argv);
	} catch (int err) {
		fprintf(stderr, "usage: sourcefile targetdir --tmp tmpdir [--pool pool_size]\n");
		return 0;
	}

	uint cnt = divide(args.sourcefile, args.tmpdir, args.pool_size);
	merge(args.tmpdir, cnt, args.targetfile);

	return 0;
}
