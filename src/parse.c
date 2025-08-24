#include <stdio.h>
#include <stdbool.h>

#include "../include/extra_functions.h"

int parsePostIndex(char** output, NumNode** numberStack) {
	/*
	this function parses given string in postfix notation. it goes through line, collects numbers in stack
	and when it sees a sign, it pops numbers from stack, performs an operation and pushes the result back
	in the end, the final result should be the only number in stack
	*/  
	char num[512] = { 0 };  //buffer for building a number
	size_t iter = 0;  //iterator of number in string format
	char unar_sign = '0';
	char prev_unar_sign = '0';  // flags of unar signs for current number in buffer

	for (size_t i = 0; (*output)[i] != '\0'; i++) {
		if ((*output)[i] >= '0' && (*output)[i] <= '9') {
			num[iter++] = (*output)[i];
			continue;
		}

		switch ((*output)[i]) {  //sign parce
		case ' ':
			if (iter > 0) {  //buffer for number not empty
				num[iter] = '\0';
				long long intnum = charToInt(num);
				pushPostOrd(numberStack, intnum);
				if (unar_sign != '0') {
					unar(numberStack, unar_sign);
					if (prev_unar_sign != '0')
						unar(numberStack, prev_unar_sign);
					unar_sign = '0';
				}
				iter = 0;
			}
			break;
		case '-':
			if ((*output)[i + 1] == '-') {  // '-' + '-' == '+'
				i++;
				iter = 0;
				break;
			}
		case '*':
		case '/':
		case '^':
		case '&':
		case '|':
		case '%':
		case '$':
		case '<':
		case '>':
		case '+':
			if (iter > 0) {  //buffer for number not empty
				num[iter] = '\0';
				long long intnum = charToInt(num);
				pushPostOrd(numberStack, intnum);
				unar_sign = '0';
				iter = 0;
			}
			int succes_code = count(numberStack, (*output)[i]);  //pop numbers from numbers stack and perform an operation
			if (succes_code != 0)
				return succes_code;
			break;
		case '~':
			unar_sign = (*output)[i];
			break;
		case '!':
			if ((*output)[i + 1] == '!') {  //two unary '-' == '+'
				i++;
				continue;
			}
			if (i > 0 && (*output)[i - 1] == ' ' && (*output)[i + 1] == ' ') {  //binary '-' 
				i = i + 2;
				continue;
			}
			if (unar_sign == '~')
				prev_unar_sign = '~';
			unar_sign = (*output)[i];
			break;
		case '(':
		case ')':
			return EXTRA_BRACKETS;  // there must be no brackets in postfix notation
		}
	}
	if (iter > 0) {  //push last number in expression
		num[iter] = '\0';
		long long intnum = charToInt(num);
		pushPostOrd(numberStack, intnum);
	}
	return SUCCESS;
}

int parse(const char* input) {  //makes postfix notation from infix notation
	char* postfix_expr = malloc(1024);
	SignNode* signHead = NULL;  // stack for signs
	NumNode* numHead = NULL;  //stack for numbers
	bool unary_sign = false;  //ready to get an unary sign
	size_t numiter = 0;
	for (size_t i = 0; input[i] != '\0'; i++) {
		if (input[i] >= '0' && input[i] <= '9') {
			postfix_expr[numiter++] = input[i];
			unary_sign = false;
			continue;
		}
		if (input[i] == ' ')
			postfix_expr[numiter++] = input[i];
		switch (input[i]) {
		case '(':
			unary_sign = true;
			push(&signHead, &postfix_expr, &numiter, input[i]);
			break;
		case ')':
			unary_sign = false;
			if (push(&signHead, &postfix_expr, &numiter, input[i]) != SUCCESS)
				return freeMemory(postfix_expr, signHead, numHead, EXTRA_BRACKETS);
			break;
		case '-':
			if (unary_sign == true || numiter == 0)
				postfix_expr[numiter++] = '!'; //replace unary '-' to '!' for distinction
			else {
				push(&signHead, &postfix_expr, &numiter, '-');
				unary_sign = true;
			}
			break;
		case '~':
			postfix_expr[numiter++] = '~';
			unary_sign = true;
			break;
		case '+':
			if (unary_sign == true || numiter == 0)  //skip unary '+', binary '+' fall down to handling binary signs
 				continue;
		case '*':
			if (input[i + 1] == '*') {
				push(&signHead, &postfix_expr, &numiter, '$'); //handle "**" as exponentiation, replace in postfix as '$'
				i++;
				unary_sign = true;
				break;
			}
		case '/':
		case '^':
		case '&':
		case '|':
		case '%':
			if (i == 0 || unary_sign == true || input[i - 1] != ' ' || input[i + 1] != ' ')
				return freeMemory(postfix_expr, signHead, numHead, CANNOT_BE_PARSED);  //error if binary sign is putted as unary
			unary_sign = 1;
			push(&signHead, &postfix_expr, &numiter, input[i]);
			break;
		case '<':
		case '>':
			if (input[i + 1] == input[i]) {
				push(&signHead, &postfix_expr, &numiter, input[i]);
				i++;
				unary_sign = 1;
				break;
			}
		case ' ':
			continue;
		default:
			return freeMemory(postfix_expr, signHead, numHead, INVALID_OPERATION);
		}
	}
	postfix_expr[numiter++] = ' ';

	while (signHead != NULL) {  //poping all signs from stack at the end of expression
		SignNode* temp = signHead;
		postfix_expr[numiter++] = signHead->sign;
		signHead = signHead->prev;
		free(temp);
	}
	postfix_expr[numiter] = '\0';

	int parse_status = parsePostIndex(&postfix_expr, &numHead);
	if (parse_status != 0)
		return freeMemory(postfix_expr, signHead, numHead, parse_status);
	if (numHead == NULL)
		return freeMemory(postfix_expr, signHead, numHead, TOO_MANY_OPERANDS);
	long long result = numHead->number;
	NumNode* temp = numHead;
	numHead = numHead->prev;
	free(temp);
	if (numHead != NULL) //check that there's only 1 result in stack
		return freeMemory(postfix_expr, signHead, numHead, NOT_ENOUGH_OPERANDS);  
	printf("%lld\n", result);
	return freeMemory(postfix_expr, signHead, numHead, SUCCESS);
}
