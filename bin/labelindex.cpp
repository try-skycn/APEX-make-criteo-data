#include <cstdio>

typedef unsigned int uint;

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "No enough arguments.\n");
		return 1;
	}

	FILE *sourcefp = fopen(argv[1], "r");
	FILE *targetfp = fopen(argv[2], "wb");

	uint x;
	while (fscanf(sourcefp, "%u", &x) != EOF) {
		fwrite(&x, sizeof(uint), 1, targetfp);
	}

	fclose(sourcefp);
	fclose(targetfp);

	return 0;
}
