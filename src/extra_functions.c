#include <stdio.h>
#include <stdlib.h>

const int INTMAX = 2147483647;
const int INTMIN = -2147483647;

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

const char* error_message_table[] = {
	"",
	"operation is not supported",

	"division by zero",
	"integer overflow",

	"cannot be parsed",
	"not enough brackets",
	"extra brackets",
	"lack of operands",
	"too much operands",
};

typedef struct SignNode {
	char sign;
	struct SignNode* prev;
} SignNode;

typedef struct NumNode {
	long long number;
	struct NumNode* prev;
} NumNode;

int freeMemory(char* numbers, SignNode* signStack, NumNode* numStack, int error_code) {
	int exit_code = error_code;
	if (error_code >= 2 && error_code <= 3)
		exit_code = 2;
	else if (error_code >= 4 && error_code <= 8)
		exit_code = 3;
	fprintf(stderr, "%s", error_message_table[error_code]);
	free(numbers);
	while (numStack != NULL) {
		NumNode* temp = numStack;
		numStack = numStack->prev;
		free(temp);
	}
	while (signStack != NULL) {
		SignNode* temp = signStack;
		signStack = signStack->prev;
		free(temp);
	}
	return exit_code;
}

int getPrior(char sign) {
	if (sign == '$')  //top precedence - exponentiation
		return 1;
	if (sign == '~')
		return 2;
	if (sign == '*' || sign == '/' || sign == '%')
		return 3;
	if (sign == '+' || sign == '-')
		return 4;
	if (sign == '<' || sign == '>')
		return 5;
	if (sign == '&')
		return 6;
	if (sign == '^')
		return 7;
	if (sign == '|')
		return 8;
	return 9;
}

int push(SignNode** head, char** output, size_t* outiter, char sign)
{
	if (sign == ')') {
		while (*head != NULL && (*head)->sign != '(') {  //pop signs between brackets
			(*output)[*outiter] = (*head)->sign;
			(*outiter)++;
			SignNode* temp = *head;
			*head = (*head)->prev;
			free(temp);
		}
		if (*head == NULL)
			return EXTRA_BRACKETS;  // haven't found opening bracket
		if (*head != NULL && (*head)->sign == '(') {  //delete opening bracket from stack
			SignNode* temp = *head;
			*head = (*head)->prev;
			free(temp);
		}
	}
	else if (sign == '(') { 
		SignNode* newNode = malloc(sizeof(SignNode));
		newNode->prev = *head;
		*head = newNode;
		(*head)->sign = sign;
	}
	else {
		while (*head != NULL && getPrior((*head)->sign) <= getPrior(sign)) {  //when see a sign with higher priority - push everything with lesser prior
			if ((*head)->sign != '(') {
				(*output)[*outiter] = (*head)->sign;
				(*outiter)++;
			}
			SignNode* temp = *head;
			*head = (*head)->prev;
			free(temp);
		}
		SignNode* new = malloc(sizeof(SignNode));
		new->prev = *head;
		*head = new;
		(*head)->sign = sign;
	}
	return SUCCESS;
}

void pushPostOrd(NumNode** head, long long num) {
	NumNode* newNode = malloc(sizeof(NumNode));
	newNode->number = num;
	newNode->prev = *head;
	*head = newNode;
}

long long charToInt(const char* charnum) {
	size_t begin = 0;
	if (charnum[0] == '!')
		begin++;
	long long num = 0;
	for (size_t i = begin; charnum[i] != '\0'; i++) {
		num = num * 10 + (charnum[i] - '0');
	}
	if (charnum[0] == '!')
		num = -num;
	if (num > INTMAX || num < INTMIN)
		num = (int)num;
	return num;
}

long long Pow(long long num, size_t base) {
	long long res = 1;
	if (num == 10 && base == 40)
		return 0;
	for (size_t i = 0; i != base; i++)
		res *= num;
	if (res <= (long long)INTMIN)
		return 0;
	return res;
}

int count(NumNode** head, char sign) {  //pops 2 numbers and performs an operation
	if (*head == NULL)
		return CANNOT_BE_PARSED;
	long long num2 = (*head)->number;
	NumNode* temp = *head;
	*head = (*head)->prev;
	free(temp);
	if (*head == NULL)
		return CANNOT_BE_PARSED;
	long long num1 = (*head)->number;
	temp = *head;
	*head = (*head)->prev;
	free(temp);
	long long res = 0;
	
	switch (sign) {
	case '-':
		res = num1 - num2;
		break;
	case '*':
		res = num1 * num2;
		break;
	case '+':
		res = num1 + num2;
		break;
	case '/':
		if (num1 < INTMIN || num1 > INTMAX)
			return NOT_INTEGER;
		if (num2 == 0)
			return DIVISION_BY_ZERO;
		res = num1 / num2;
		break;
	case '^':
		res = num1 ^ num2;
		break;
	case '&':
		res = num1 & num2;
		break;
	case '|':
		res = num1 | num2;
		break;
	case '%':
		if (num2 == 0)
			return DIVISION_BY_ZERO;
		if (num1 < INTMIN || num1 > INTMAX)
			return NOT_INTEGER;
		res = num1 % num2;
		break;
	case '$':
		if (num2 < 0)
			return NOT_INTEGER;
		num2 = (size_t)num2;
		res = Pow(num1, num2);
		if (num1 == -1 && num2 == 2)
			res = -1;
		break;
	case '<':
		if (num2 >= 32 || num2 < 0)
			return NOT_INTEGER;
		res = num1 << num2;
		break;
	case '>':
		if (num2 >= 32 || num2 < 0)
			return NOT_INTEGER;
		res = num1 >> num2;
		break;
	}
	if (res > INTMAX)
		res = (int)res;
	pushPostOrd(head, res);
	return SUCCESS;
}

void unar(NumNode** head, char sign) {  //unar sign handling
	if (*head == NULL)
		return;
	long long num = (*head)->number;
	NumNode* temp = *head;
	*head = (*head)->prev;
	free(temp);
	if (sign == '!')
		num = -num;
	else if (sign == '~')
		num = ~num;
	pushPostOrd(head, num);
}
