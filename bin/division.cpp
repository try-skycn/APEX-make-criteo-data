#include <cstdio>

typedef unsigned int uint;

const int MAXBUFF = 65536;
const char NEWLINE = '\n';

class Source {
private:
	FILE *parent;
	uint cnt;

public:
	Source(const char *file) : cnt(0) {
		parent = fopen(file, "r");
	}

	bool get(char *str) {
		const char *suss = fgets(str, MAXBUFF, parent);
		if (suss) {
			++cnt;
			return true;
		} else {
			return false;
		}
	}

	~Source() {
		printf("%u\n", cnt);
		fclose(parent);
	}
};

class Target {
private:
	FILE **parents;
	uint cnt;

public:
	Target(const char *prefix, uint _cnt) : cnt(_cnt) {
		parents = new FILE*[cnt];
		char file[MAXBUFF];
		for (uint i = 0; i < cnt; ++i) {
			sprintf(file, "%s.%d", prefix, i);
			parents[i] = fopen(file, "wb");
		}
	}

	void push(const char *str) {
		for (uint i = 0; i < cnt; ++i) {
			for (; *str && *str != ' ' && *str != '\t' && *str != '\n'; ++str) {
				fwrite(str, sizeof(char), 1, parents[i]);
			}
			++str;
			fwrite(&NEWLINE, sizeof(char), 1, parents[i]);
		}
	}

	~Target() {
		for (uint i = 0; i < cnt; ++i) {
			fclose(parents[i]);
		}

		delete[] parents;
		parents = 0;
	}
};

int main(int argc, char *argv[]){
	if (argc < 4) {
		printf("No enough arguments.\n");
		return 1;
	}

	uint cnt;
	sscanf(argv[3], "%u", &cnt);
	
	Source source(argv[1]);
	Target target(argv[2], cnt);

	char str[MAXBUFF];
	while (source.get(str)) {
		target.push(str);
	}

	return 0;
}