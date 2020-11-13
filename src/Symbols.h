#pragma once

#include <map>
#include <string>
#include "tag.h"

using namespace std;

class Symbols {
private:
	Symbols *prev;
	map<string, Tag> list;
public:
	Symbols();
	Symbols(Symbols *p);
	void put(string name, Tag t);
	Tag* get(string & name);
	Tag* getFromFathers(string & name);
	Symbols* findPrev();
	void output();
};