#include "tag.h"

Tag::Tag() {}

Tag::Tag(string name, Tag_type tt, Value_type vt) {
	this->name = name;
	this->value_type = vt;
	this->tag_type = tt;
	this->dimension = 0;
	this->isreturn = false;
	this->isconst = false;
}

void Tag::setConst(bool isc) {
	this->isconst = isc;
}

void Tag::setReturn(bool isr) {
	this->isreturn = isr;
}

void Tag::setDimension(int d) {
	this->dimension = d;
}

void Tag::pushParameter(Value_type type) {
	this->paralist.push_back(type);
}

void Tag::setValue(int value) {
	int_value = value;
}

void Tag::setValue(char value) {
	char_value = value;
}

Tag_type Tag::tagType() {
	return tag_type;
}

Value_type Tag::valueType() {
	return value_type;
}

bool Tag::isReturn() {
	return isreturn;
}

bool Tag::isConst() {
	return isconst;
}

int Tag::getIntValue() {
	return int_value;
}

char Tag::getCharValue() {
	return char_value;
}

int Tag::getDimension() {
	return this->dimension;
}

vector<Value_type> & Tag::getParameters() {
	return this->paralist;
}

string Tag::getName() {
	return this->name;
}

ostream &operator<<(ostream &output, const Tag &t) {
	string tag = (t.tag_type == VARORCONST) ? "varOrConst" : "function";
	string type = (t.value_type == INT) ? "int" : ((t.value_type == CHAR) ? "char" : "none");
	output << "{ name: " << t.name;
	output << ", tag: " << tag << ", type: " << type;
	if (t.isconst)
		output << ", isConst: true";
	if (t.isreturn)
		output << ", isReturn: true";
	if (t.dimension != 0)
		output << ", dimension: " << t.dimension;
	if (t.tag_type == FUNCTION) {
		output << ", paralist: [ ";
		for (int i = 0; i < t.paralist.size(); i++) {
			string pType = (t.paralist[i] == INT) ? "int" : ((t.paralist[i] == CHAR) ? "char" : "none");
			output << pType;
			if (i != t.paralist.size() - 1) {
				output << ", ";
			}
		}
		output << "]";
	}
	output << "}";
	return output;
}

string to_string(Value_type v) {
	if (v == INT) {
		return "int";
	}
	else if (v == CHAR) {
		return "char";
	}
	else {
		return "non";
	}
}