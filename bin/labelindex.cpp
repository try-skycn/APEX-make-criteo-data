#include <cstdio>
#include <cstring>

typedef unsigned int uint;

const size_t MAXBUFF = 256;

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

	FILE *s = fopen(args.sourcefile, "r");
	FILE *t = fopen(args.targetfile, "wb");

	uint x;
	while (fscanf(s, "%u", &x) != EOF) {
		fwrite(&x, sizeof(uint), 1, t);
	}

	fclose(s);
	fclose(t);

	return 0;
}
