#pragma once
#include <string>
#include <map>
#include <queue>
#include <set>
#include "main.h"
using namespace std;

class VarInfo {
public:
	string name;
	int regNum;
	int paraIndex;
	int memOffset; // word
	string label;
	bool isGlobal;
	bool isFake;
	bool needLoad;
	bool isArray;
	bool isDirty;
	VarInfo();
	VarInfo(string n, string l);
	VarInfo(string n, string l, int memOffset);
	int getRegNum();
	int getMemOffset();
	//void allocReg(int r, int sp);
	void allocReg(int r, int sp, bool needLoad);
	void setMemOffset(int of);
	int freeReg(int sp, bool isBlock=false, bool isReturn=false);
	string & getName();
};

static VarInfo placeHolder("", "");

class Reg {
public:
	int num;
	VarInfo *info = NULL;
	bool isGlobal;
	Reg(int n, bool isGlobal);
};

class BlockController {
public:
	vector<Intermediate_code> block_codes;
	vector<Reg> busyRegs;
	queue<Reg> validRegs;
	set<string> use_set;
	set<string> def_set;
	set<string> active_in_set;
	set<string> active_out_set;
	int freeReg(int sp, VarInfo* info, bool isBlock = false, bool isReturn = false);
	bool union_set(set<string> *result, set<string> *first, set<string> *second);
	BlockController();
	string name;
	void regs_alloc_and_free(vector<VarInfo*> *vars, int sp, vector<string> *result);
};

class FunctionInfo {
public:
	int paraDigCount;// word
	int varDigCount; // word
	int tmpCount;	 // word
	int spCount;	 // word
	int globalRegsSum;
	int globalRegMinIndex;
	int globalRegMaxIndex;
	FunctionInfo();
	void save_global_regs(int sp, BlockController *block, bool isBlock, bool isReturn);
	void load_global_regs(int sp, BlockController *block);
	map<string, int> name_use_times;
	static bool times_sort(const pair<string, int> &p1, const pair<string, int> &p2);
	vector<BlockController*> blocks;
	map<int, Reg> global_regs;
	map<string, BlockController*> blocks_name_book;
	map<BlockController*, vector<BlockController*>> data_stream_after_map;
};



void reg_init();
void data_init();
void function_generate();
void mips_code_generate(string context);
bool check_temp_statistic(string & name);
VarInfo* find_var_info(string & name);
string block_name_generate();
