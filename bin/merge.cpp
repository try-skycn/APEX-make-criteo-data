#include <cstdio>
#include <cstring>
#include <vector>

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
		fprintf(stderr, "input: a list of starting index of fields\n");
		fprintf(stderr, "       - \'.\' stands for \'label\' field\n");
		fprintf(stderr, "       - positive numbers stands for the starting index\n");
		return 0;
	}

	static char file[MAXBUFF];

	std::vector<bool> label;
	std::vector<uint> start;
	while (scanf("%s", file) != EOF) {
		if (*file == '.') {
			label.push_back(true);
			start.push_back(0);
		} else {
			uint x;
			sscanf(file, "%u", &x);
			label.push_back(false);
			start.push_back(x);
		}
	}

	uint num_fields = start.size();

	FILE **sfps = new FILE *[num_fields], *t = fopen(args.targetfile, "wb");
	for (int i = 0; i < num_fields; ++i) {
		sprintf(file, "%s/%d", args.sourcedir, i);
		sfps[i] = fopen(file, "rb");
	}

	uint *buff = new uint[num_fields];
	while (get(sfps, num_fields, buff)) {
		for (int i = 0; i < num_fields; ++i) {
			if (!label[i] && buff[i]) {
				buff[i] += start[i] - 1;
			}
		}
		fwrite(buff, sizeof(uint), num_fields, t);
	}
	
	fclose(t);
	for (int i = 0; i < num_fields; ++i) {
		fclose(sfps[i]);
	}
	delete[] sfps;

	return 0;
}
