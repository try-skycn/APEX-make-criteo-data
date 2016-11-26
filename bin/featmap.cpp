#include <cstdio>
#include <cstring>

typedef unsigned int uint;

const uint MAXBUFF = 65536;

void pass(const char *sourcedir, int idx, FILE *t) {
	static char file[MAXBUFF], buff[MAXBUFF];
	sprintf(file, "%s/%d", sourcedir, idx);
	FILE *s = fopen(file, "r");
	while (fscanf(s, "%s", buff) != EOF) {
		fprintf(t, "%d %s\n", idx, buff);
	}
	fclose(s);
}

struct Args {
	const char *sourcedir;
	const char *targetfile;
	int num_fields;
};

void parse_args(Args &args, int argc, char *argv[]) {
	bool is_sourcedir = false;
	bool is_targetfile = false;

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--help")) {
			throw 0;
		} else if (!is_sourcedir) {
			args.sourcedir = argv[i];
			is_sourcedir = true;
		} else if (!is_targetfile) {
			args.targetfile = argv[i];
			is_targetfile = true;
		}
	}
	if (!is_sourcedir) throw 0;
	if (!is_targetfile) throw 0;
}

int main(int argc, char *argv[]){
	Args args;
	try {
		parse_args(args, argc, argv);
	} catch (int err) {
		fprintf(stderr, "usage: sourcedir targetfile\n");
		fprintf(stderr, "input: a list of field indices\n");
		return 0;
	}

	FILE *t = fopen(args.targetfile, "w");
	uint idx = 0;
	while (scanf("%u", &idx) != EOF) {
		pass(args.sourcedir, idx, t);
	}
	fclose(t);

	return 0;
}