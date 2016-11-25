#include <cstdio>
#include <cstring>

typedef unsigned int uint;

const int MAXBUFF = 65536;
const char NEWLINE = '\n';

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

class Target {
private:
	FILE **fps;
	uint cnt;

public:
	Target(const char *prefix, uint _cnt) : cnt(_cnt) {
		fps = new FILE*[cnt];
		char file[MAXBUFF];
		for (uint i = 0; i < cnt; ++i) {
			sprintf(file, "%s.%d", prefix, i);
			fps[i] = fopen(file, "wb");
		}
	}

	void push(const char *str) {
		for (uint i = 0; i < cnt; ++i) {
			for (; *str && *str != ' ' && *str != '\t' && *str != '\n'; ++str) {
				fwrite(str, sizeof(char), 1, fps[i]);
			}
			++str;
			fwrite(&NEWLINE, sizeof(char), 1, fps[i]);
		}
	}

	~Target() {
		for (uint i = 0; i < cnt; ++i) {
			fclose(fps[i]);
		}

		delete[] fps;
		fps = 0;
	}
};

struct Args {
	const char *sourcefile;
	const char *targetprefix;
	int num_fields;
};

void parse_args(Args &args, int argc, char *argv[]) {
	bool is_sourcefile = false;
	bool is_targetprefix = false;
	bool is_num_fields = false;

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--help")) {
			throw 0;
		} else if (!strcmp(argv[i], "--num")) {
			if (!(i + 1 < argc)) throw 0;
			sscanf(argv[++i], "%u", &args.num_fields);
			is_num_fields = true;
		} else if (!is_sourcefile) {
			args.sourcefile = argv[i];
			is_sourcefile = true;
		} else if (!is_targetprefix) {
			args.targetprefix = argv[i];
			is_targetprefix = true;
		}
	}
	if (!is_sourcefile) throw 0;
	if (!is_targetprefix) throw 0;
	if (!is_num_fields) throw 0;
}

int main(int argc, char *argv[]){
	Args args;
	try {
		parse_args(args, argc, argv);
	} catch (int err) {
		fprintf(stderr, "usage: sourcefile targetprefix --num num_fields\n");
		fprintf(stderr, "print: num of lines\n");
		return 0;
	}
	
	Source source(args.sourcefile);
	Target target(args.targetprefix, args.num_fields);

	char str[MAXBUFF];
	uint num_lines = 0;
	while (source.get(str)) {
		target.push(str);
		++num_lines;
	}
	printf("%u\n", num_lines);

	return 0;
}