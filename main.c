#include "include/parse.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "%s", "invalid arguments");
		return 4;
	}
	char* input = argv[1];
	return parse(input);
}
