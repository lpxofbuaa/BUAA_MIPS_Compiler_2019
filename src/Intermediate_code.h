#pragma once
#include <string>
#include <ostream>
#include <queue>

using namespace std;

enum CodeOperation
{
	VARDEF,
	CONST,
	VAR,
	VARDEFEND,
	BLE,
	BEQ,
	BNE,
	BLT,
	ADD,
	SUB,
	MULTOP,
	DIVOP,
	PRECALL,
	CALL,
	PRECALLEND,
	RET,
	MOVE,
	ARRLOAD,
	ARRSAVE,
	SETLABEL,
	FUNCTIONBEGIN,
	FUNCTIONEND,
	PARADEF,
	PARADEFEND,
	END,
	READ,
	PRINT,
	STR,
	ADDINDENT,
	REDUCEINDENT
};

class Intermediate_code
{
public:
	CodeOperation operation;
	string rd;
	string rs;
	string rt;
	Intermediate_code();
	Intermediate_code(CodeOperation, string rd, string rs, string rt);
	friend ostream &operator<<(ostream &output, const Intermediate_code &code);
};

string temp_var_generate();
void free_temp_var(string name);
string para_generate();
void para_init();
string label_generate(string tag);
string string_label_generate();
string var_label_generate();
string func_label_generate();
string string_name_generate();