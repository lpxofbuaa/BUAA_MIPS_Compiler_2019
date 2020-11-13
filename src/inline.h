#pragma once
#include <string>
#include <map>
#include <queue>
#include <vector>
#include "main.h"
#include "Lexical_analysis.h"
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;


class function_inline_info {
public:
	vector<Intermediate_code> para;
	vector<Intermediate_code> var;
	vector<Intermediate_code> inline_context;
	string v0;
};


extern vector<Intermediate_code>::iterator inline_it;
extern int inline_label_count;
extern map<string, function_inline_info> function_inline_map;

void var_reset();
void function_inline();
void code_better();
void move_better();
string inline_label_reset(string label);