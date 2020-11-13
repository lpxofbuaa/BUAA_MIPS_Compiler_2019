#include "Symbols.h"
#include "main.h"
#include <iostream>

using namespace std;

Symbols::Symbols() {
	this->prev = NULL;
}

Symbols::Symbols(Symbols *prev) {
	this->prev = prev;
}

void Symbols::put(string name, Tag t) {
	this->list.insert({ name, t });
	if (debug) {
		if (this->prev != NULL) {
			//cout << "\t" << name << " --> " << t << endl;
		}
		else {
			//cout << name << " --> " << t << endl;
		}
	}
}

Tag* Symbols::get(string & name) {
	map<string, Tag>::iterator iter;
	iter = this->list.find(name);
	if (iter != this->list.end()) {
		return &iter->second;
	}
	return NULL;
}

Tag* Symbols::getFromFathers(string & name) {
	Tag *re = NULL;
	Symbols *t = this;
	while (t != NULL) {
		re = t->get(name);
		if (re != NULL) {
			return re;
		}
		t = t->findPrev();
	}
	return re;
}

Symbols* Symbols::findPrev() {
	return this->prev;
}

void Symbols::output() {
	map<string, Tag>::iterator it;
	for (it = this->list.begin(); it != this->list.end(); it++) {
		//cout << it->first << " --> " << it->second << endl;
	}
}