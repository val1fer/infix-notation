#include <stdlib.h>

typedef struct SignNode SignNode;
typedef struct NumNode NumNode;

struct SignNode {
	char sign;
	SignNode* prev;
};

struct NumNode {
	long long number;
	NumNode* prev;
};

int freeMemory(char* numbers, SignNode* signStack, NumNode* numStack, int error_code);
int getPrior(char sign);
int push(SignNode** head, char** output, size_t* outiter, char sign);
void pushPostOrd(NumNode** head, long long num);
long long charToInt(char* charnum);
long long Pow(long long num, size_t base);
int count(NumNode** head, char sign);
void unar(NumNode** head, char sign);

enum ErrorCodes {
	SUCCESS,

	INVALID_OPERATION,

	DIVISION_BY_ZERO,
	NOT_INTEGER,

	CANNOT_BE_PARSED,
	NOT_ENOUGH_BRACKETS,
	EXTRA_BRACKETS,
	NOT_ENOUGH_OPERANDS,
	TOO_MANY_OPERANDS,
};
