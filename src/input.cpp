#include "input.h"

void Input::init() {
	infile.open(infilename);
}

char Input::getc() {
	char tmp;
	tmp = infile.get();
	return tmp;
}

void Input::close() {
	infile.close();
}

Input::Input() {
	init();
}