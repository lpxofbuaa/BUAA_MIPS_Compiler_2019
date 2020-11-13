#pragma once

#include <string>
#include <fstream>

using namespace std;

extern string infilename;

class Input {
private:
	ifstream infile;
public:
	Input();
	void init();
	char getc();
	void close();
};

