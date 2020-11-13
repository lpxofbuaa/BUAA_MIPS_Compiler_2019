#pragma once
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include "input.h"
#include "output.h"
#include "tag.h"
#include "Symbols.h"
#include "Intermediate_code.h"
#include "code_generate.h"
using namespace std;

extern bool debug;
extern string token;
extern char pick;
extern Input in;
extern Output out;
extern Type type;
extern Value_type function_type;
extern Value_type value_type;
extern string name;
extern Symbols *symbols;
extern bool isVarFinish;
extern bool isCatchMain;
extern int line;
extern int identifyLine;
extern Value_type returnType;
extern bool isCatchReturn;
extern int lastWordLine;
extern ofstream errorFile;
extern bool errorFlag;
extern ofstream codeFile;
// extern vector<Value_type> *paralist;
extern queue<string> temp_var;
extern int temp_counter;
extern vector<Intermediate_code> codes;
extern int para_num;
extern int labelCount;
extern int stringCount;
extern int stringLabelCount;
extern int varLabelCount;
extern int funcLabelCount;

// code generate
extern ofstream mips;

void error();
void errorMessage(char type, int line);