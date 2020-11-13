#pragma once

#include "Type.h"
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
using namespace std;

extern string outfilename;

class Output {
private:
	ofstream outfile0;
	queue<string> buf;
public:
	Output();
	void init();
	void push(Type t,const string & s);
	void push(const string s);
	void output();
	void output(int a);
	void print(const string s);
	void close();
};
