#include <cstdio>
#include <cstring>
#include <vector>
#include <utility>

typedef unsigned int uint;
typedef unsigned char uchar;

void pass(FILE *s, uint entrysize, uint count, FILE *t, uchar *buff) {
	for (; count; --count) {
		if (fread(buff, sizeof(uchar), entrysize, s) == entrysize) {
			fwrite(buff, sizeof(uchar), entrysize, t);
		}
	}
}

void pass_remain(FILE *s, uint entrysize, FILE *t, uchar *buff) {
	while (fread(buff, sizeof(uchar), entrysize, s) == entrysize) {
		fwrite(buff, sizeof(uchar), entrysize, t);
	}
}

struct Args {
	const char *sourcefile;
	uint entrysize;
	std::vector<std::pair<const char *, uint> > targets;
	const char *remainfile;
};

void parse_args(Args &args, int argc, const char *argv[]) {
	bool is_sourcefile = false;
	bool is_entrysize = false;
	bool is_target = false;
	bool is_remain = false;

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--help")) {
			throw 0;
		} else if (!is_sourcefile) {
			args.sourcefile = argv[i];
			is_sourcefile = true;
		} else if(!is_entrysize) {
			sscanf(argv[i], "%u", &args.entrysize);
			is_entrysize = true;
		} else {
			if (i + 1 < argc) { // target
				uint count;
				const char *targetfile = argv[i];
				sscanf(argv[++i], "%u", &count);
				args.targets.push_back(std::pair<const char *, uint>(targetfile, count));
				is_target = true;
			} else { // remain
				args.remainfile = argv[i];
				is_remain = true;
			}
		}
	}

	if (!is_sourcefile) throw 0;
	if (!is_entrysize) throw 0;
	if (!is_target) throw 0;
	if (!is_remain) throw 0;
}

int main(int argc, const char *argv[]) {
	Args args;
	try {
		parse_args(args, argc, argv);
	} catch (int err) {
		fprintf(stderr, "usage: sourcefile entrysize file count [file count] [...] file\n");
		return 0;
	}

	uchar *buff = new uchar[args.entrysize];
	FILE *s = fopen(args.sourcefile, "rb");

	for (std::vector<std::pair<const char *, uint> >::iterator it = args.targets.begin(); it != args.targets.end(); ++it) {
		const char *targetfile = it->first;
		uint count = it->second;
		FILE *t = fopen(targetfile, "wb");
		pass(s, args.entrysize, count, t, buff);
		fclose(t);
	}
	const char *targetfile = args.remainfile;
	FILE *t = fopen(targetfile, "wb");
	pass_remain(s, args.entrysize, t, buff);
	fclose(t);

	fclose(s);
	return 0;
}
