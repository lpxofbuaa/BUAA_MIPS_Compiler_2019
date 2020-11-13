#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <ostream>
using namespace std;

enum Value_type {
	INT,
	CHAR,
	NON
};

string to_string(Value_type v);

enum Tag_type {
	FUNCTION,
	VARORCONST
};

class Tag {
private:
	string name;
	Value_type value_type;
	Tag_type tag_type;
	vector<Value_type> paralist;
	bool isreturn;
	bool isconst;
	int int_value;
	int dimension;
	char char_value;
public:
	Tag();
	Tag(string name, Tag_type tt, Value_type vt);
	// Tag(Tag & t);
	// ~Tag();
	void setConst(bool isConst);
	void setReturn(bool isReturn);
	void pushParameter(Value_type type);
	void setValue(int value);
	void setValue(char value);
	void setDimension(int d);
	Tag_type tagType();
	Value_type valueType();
	string getName();
	bool isReturn();
	bool isConst();
	int getIntValue();
	char getCharValue();
	int getDimension();
	vector<Value_type> & getParameters();
	friend ostream &operator<<(ostream &output, const Tag &t);
};