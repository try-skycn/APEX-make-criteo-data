#include <cstdio>

typedef unsigned int uint;

const uint MAXNUM = 1 << 22;

const char *sourcefile;
const char *targetfile;

FILE *sourcefp, *targetfp;
uint cnt[MAXNUM];

void up(const char *file) {
	FILE *fp = fopen(file, "r");
	int x;
	while (fscanf(fp, "%d", &x) != EOF) {
		if (x < 0) {
			continue;
		} else if (x >= MAXNUM) {
			fprintf(stderr, "Number %d out of bound.\n", x);
			throw 0;
		}
		++cnt[x];
	}
	fclose(fp);
}

void down(const char *file) {
	FILE *fp = fopen(file, "wb");
	for (uint i = 0; i < MAXNUM; ++i) {
		if (cnt[i]) {
			fwrite(&i, sizeof(uint), 1, fp);
			fwrite(cnt + i, sizeof(uint), 1, fp);
		}
	}
	fclose(fp);
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "No enough arguments.\n");
		return 1;
	}

	up(argv[1]);
	down(argv[2]);

	return 0;
}
