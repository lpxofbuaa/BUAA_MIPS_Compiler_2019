#include "is.h"
#include "main.h"

bool isSpace(char t) {
	if (t == '\n')
		line++;
	if ((t == ' ') || (t == '\t') || (t == '\n') || (t == '\r'))
		return true;
	else
		return false;
}

bool isDigital(char t) {
	int tmp = (int)t;
	return (tmp >= 48) && (tmp <= 57);
}

bool isAlpha(char t) {
	if (t == '_')
		return true;
	int tmp = (int)t;
	return ((tmp >= 97) && (tmp <= 122)) || ((tmp >= 65) && (tmp <= 90));
}

char skipSpace(char p) {
	while (isSpace(p)) {
		p = in.getc();
	}
	return p;
}

void inputWord() {
	char tmp;
	tmp = in.getc();
	while (isAlpha(tmp) || isDigital(tmp)) {
		token += tmp;
		tmp = in.getc();
	}
	pick = tmp;
}

void inputNum() {
	char tmp;
	tmp = in.getc();
	while (isDigital(tmp)) {
		token += tmp;
		tmp = in.getc();
	}
	pick = tmp;
}