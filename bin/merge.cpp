#include <cstdio>
#include <cstring>

typedef unsigned int uint;

const uint MAXBUFF = 65536;

uint *get(FILE **sfps, uint num_fields, uint *buff) {
	for (int i = 0; i < num_fields; ++i) {
		if (!fread(buff + i, sizeof(uint), 1, sfps[i])) {
			return NULL;
		}
	}
	return buff;
}

struct Args {
	const char *sourcedir;
	const char *targetfile;
	int num_fields;
};

void parse_args(Args &args, int argc, char *argv[]) {
	bool is_sourcedir = false;
	bool is_targetfile = false;
	bool is_num_fields = false;

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--help")) {
			throw 0;
		} else if (!strcmp(argv[i], "--num")) {
			if (!(i + 1 < argc)) throw 0;
			sscanf(argv[++i], "%u", &args.num_fields);
			is_num_fields = true;
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
	if (!is_num_fields) throw 0;
}

int main(int argc, char *argv[]){
	Args args;
	try {
		parse_args(args, argc, argv);
	} catch (int err) {
		fprintf(stderr, "usage: sourcedir targetfile --num num_fields\n");
		return 0;
	}

	static char file[MAXBUFF];

	FILE **sfps = new FILE *[args.num_fields], *t = fopen(args.targetfile, "wb");
	for (int i = 0; i < args.num_fields; ++i) {
		sprintf(file, "%s/%d", args.sourcedir, i);
		sfps[i] = fopen(file, "rb");
	}

	uint *buff = new uint[args.num_fields];
	
	fclose(t);
	for (int i = 0; i < args.num_fields; ++i) {
		fclose(sfps);
	}
	delete[] sfps;

	return 0;
}