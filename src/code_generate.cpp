#include "code_generate.h"
#include "main.h"
#include "inline.h"
#include "Lexical_analysis.h"
#include <sstream>
#include <iostream>
extern map<string, VarInfo> global_var_info;
extern map<string, VarInfo> function_var_info;
extern queue<Reg> all_valid_regs;
extern vector<Intermediate_code>::iterator it;
extern int tab;
extern map<string, function_inline_info> function_inline_map;
extern int block_index;
extern ofstream middle_before;
extern FunctionInfo *now_function_info;

bool isMain;

using namespace std;


VarInfo::VarInfo() {
	name = "";
	label = "";
	paraIndex = -1;
	isGlobal = false;
	isFake = true;
	isArray = false;
	isDirty = false;
}

VarInfo::VarInfo(string n, string l) {
	name = n;
	paraIndex = -1;
	isDirty = false;
	if (l == "") {
		isGlobal = false;
	}
	else {
		this->label = l;
		isGlobal = true;
	}
	isFake = false;
	isArray = false;
	regNum = -1;
	memOffset = -1;
}

VarInfo::VarInfo(string n, string l, int of) {
	name = n;
	paraIndex = -1;
	isDirty = false;
	if (l == "") {
		isGlobal = false;
	}
	else {
		this->label = l;
		isGlobal = true;
	}
	isFake = false;
	isArray = false;
	regNum = -1;
	memOffset = of;
}

int VarInfo::getRegNum() {
	return this->regNum;
}

int VarInfo::getMemOffset() {
	return this->memOffset;
}

/*void VarInfo::allocReg(int r, int sp) {
	this->regNum = r;
	if (this->isGlobal) {
		mips_code_generate("la $" + to_string(r) + ", " + this->label);
		mips_code_generate("lw $" + to_string(r) + ", " + "0($" + to_string(r) + ")");
	}
	else {
		mips_code_generate("lw $" + to_string(r) + ", " + to_string((sp - this->memOffset) * 4) + "($sp)");
	}
}*/

void VarInfo::allocReg(int r, int sp, bool needLoad) {
	this->regNum = r;
	if (this->isArray) {
		if (this->isGlobal) {
			mips_code_generate("la $" + to_string(r) + ", " + this->label);
		}
		else {
			mips_code_generate("addiu $" + to_string(r) + ", $sp, " + to_string((sp - this->memOffset) * 4));
		}
	} 
	else if (needLoad) {
		if (this->isGlobal) {
			// mips_code_generate("la $" + to_string(r) + ", " + this->label);
			mips_code_generate("lw $" + to_string(r) + ", " + this->label + "($0)");
		}
		else {
			mips_code_generate("lw $" + to_string(r) + ", " + to_string((sp - this->memOffset) * 4) + "($sp)");
		}
	}
}

void VarInfo::setMemOffset(int of) {
	this->memOffset = of;
}

int VarInfo::freeReg(int sp, bool isBlock, bool isReturn) {
	if (this->regNum == -1) {
		this->isDirty = false;
		return -1;
	}
	int preReg = this->regNum;
	if (this->regNum > now_function_info->globalRegMaxIndex || this->regNum < now_function_info->globalRegMinIndex) {
		this->regNum = -1;
	}
	if (this->isArray) {
		this->isDirty = false;
		return preReg;
	}
	if (this->isGlobal) {
		// mips_code_generate("la $a3, " + this->label);
		if (isReturn && isMain) {}
		else if (this->regNum >= now_function_info->globalRegMinIndex && this->regNum <= now_function_info->globalRegMaxIndex) {
			mips_code_generate("la $a0, " + this->label);
			mips_code_generate("sw $" + to_string(preReg) + ", 0($a0)");
		}
		else if (this->isDirty){
			mips_code_generate("la $a0, " + this->label);
			mips_code_generate("sw $" + to_string(preReg) + ", 0($a0)");
		}
	}
	else {
		if (not isReturn) {
			
			if (this->isDirty) {
				mips_code_generate("sw $" + to_string(preReg) + ", " + to_string((sp - this->memOffset) * 4) + "($sp)");
			}
				/*if (this->name.at(0) != '$') {
					mips_code_generate("sw $" + to_string(preReg) + ", " + to_string((sp - this->memOffset) * 4) + "($sp)");
				}*/
			
		}
	}
	this->isDirty = false;
	return preReg;
}

int BlockController::freeReg(int sp, VarInfo *info, bool isBlock, bool isReturn) {
	if (this->active_out_set.find(info->name) == this->active_out_set.end() && not info->isGlobal) {
		if (info->regNum > now_function_info->globalRegMaxIndex || info->regNum < now_function_info->globalRegMinIndex) {
			info->regNum = -1;
		}
		return -1;
	}
	else {
		return info->freeReg(sp, isBlock, isReturn);
	}
}

void FunctionInfo::save_global_regs(int sp, BlockController *block, bool isBlock=false, bool isReturn=false) {
	for (map<int, Reg>::iterator i = this->global_regs.begin(); i != this->global_regs.end(); ++i) {
		if (i->second.info != NULL) {
			if (block->active_out_set.find(i->second.info->name) != block->active_out_set.end() || i->second.info->isGlobal) {
				block->freeReg(sp, i->second.info, isBlock, isReturn);
			}
		}
	}
}

void FunctionInfo::load_global_regs(int sp, BlockController *block) {
	for (map<int, Reg>::iterator i = this->global_regs.begin(); i != this->global_regs.end(); ++i) {
		if (i->second.info != NULL) {
			if (block->active_out_set.find(i->second.info->name) != block->active_out_set.end()) {
				i->second.info->allocReg(i->second.info->regNum, sp, true);
			}
		}
	}
}

string & VarInfo::getName() {
	return this->name;
}

Reg::Reg(int n, bool isGlobal) {
	this->num = n;
	this->isGlobal = isGlobal;
}

BlockController::BlockController() {
	this->validRegs = all_valid_regs;
}

bool BlockController::union_set(set<string> *result, set<string> *first, set<string> *second) {
	bool re = false;
	set<string> tmp;
	tmp = *first;
	for (set<string>::iterator it = second->begin(); it != second->end(); ++it) {
		tmp.insert(*it);
	}
	if (tmp.size() != result->size()) {
		*result = tmp;
		return true;
	}
	for (set<string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
		if (result->find(*it) == result->end()) {
			re = true;
			break;
		}
	}
	*result = tmp;
	return re;
}

string block_name_generate() {
	++block_index;
	return "block_" + to_string(block_index);
}

VarInfo* find_var_info(string & name) {
	map<string, VarInfo>::iterator iter;
	iter = function_var_info.find(name);
	if (iter != function_var_info.end()) {
		return &iter->second;
	}
	iter = global_var_info.find(name);
	if (iter != global_var_info.end()) {
		return &iter->second;
	}
	return NULL;
}

void mips_code_generate(string context) {
	int tmp_tab = (tab < 0) ? 0 : tab;
	for (int i = 0; i < tmp_tab; ++i) {
		mips << "    ";
	}
	mips << context << endl;
}


bool check_temp_statistic(string & name) {
	return function_var_info.find(name) == function_var_info.end();
}

void reg_init() {
	for (int regNum = 8; regNum <= 15; ++regNum) {
		all_valid_regs.push(Reg(regNum, false));
	}
	all_valid_regs.push(Reg(24, false));
	all_valid_regs.push(Reg(25, false));
	/*for (int regNum = 8; regNum <= 10; ++regNum) {
		all_valid_regs.push(Reg(regNum));
	}*/
}

FunctionInfo::FunctionInfo() {
	for (int i = 16; i <= 23; ++i) {
		this->global_regs.insert({ 
			i,
			(Reg(i, true)) 
		});
	}
	this->globalRegsSum = 8;
	this->globalRegMinIndex = 16;
	this->globalRegMaxIndex = 23;
}

bool FunctionInfo::times_sort(const pair<string, int> &p1, const pair<string, int> &p2) {
	return p1.second > p2.second;
}


void BlockController::regs_alloc_and_free(vector<VarInfo*> *vars, int sp, vector<string> *result) {
	int vars_count = vars->size();
	for (int i = 0; i < vars_count; ++i) {
		stringstream check((*vars)[i]->getName());
		int holder;
		if ((*vars)[i]->getName().at(0) == '$' && (*vars)[i]->getName().at(1) != 't') {
			result->push_back((*vars)[i]->getName());
		}
		else if (check >> holder) {
			if (holder == 0) {
				result->push_back("$0");
			}
			else {
				mips_code_generate("li $a" + to_string(i) + ", " + (*vars)[i]->getName());
				result->push_back("$a" + to_string(i));
			}
		} 
		else {
			if ((*vars)[i]->regNum >= now_function_info->globalRegMinIndex && (*vars)[i]->regNum <= now_function_info->globalRegMaxIndex) {
				map<int, Reg>::iterator global_reg_it = now_function_info->global_regs.find((*vars)[i]->regNum);
				if (global_reg_it != now_function_info->global_regs.end()) {
					result->push_back("$" + to_string((*vars)[i]->regNum));
					continue;
				}
			}
			if ((*vars)[i]->isFake) {
				//cout << "error!!!!!!!!!!!!" << endl;
			}
			if ((*vars)[i]->getRegNum() != -1) {
				result->push_back("$" + to_string((*vars)[i]->getRegNum()));
				continue;
			}
			if (not this->validRegs.empty()) {
				Reg reg = this->validRegs.front();
				this->validRegs.pop();
				reg.info = (*vars)[i];
				(*vars)[i]->allocReg(reg.num, sp, reg.info->needLoad);
				result->push_back("$" + to_string(reg.num));
				this->busyRegs.push_back(reg);
			}
			else {
				vector<Reg>::iterator remove;
				for (vector<Reg>::iterator j = this->busyRegs.begin(); j != this->busyRegs.end(); ++j) {
					bool canSet = true;
					for (int k = 0; k < vars_count; ++k) {
						if ((*vars)[k]->getRegNum() == j->num) {
							canSet = false;
							break;
						}
					}
					if (canSet) {
						remove = j;
						break;
					}
				}
				remove->info->freeReg(sp);
				remove->info = (*vars)[i];
				(*vars)[i]->allocReg(remove->num, sp, remove->info->needLoad);
				Reg tmp = *remove;
				this->busyRegs.erase(remove);
				result->push_back("$" + to_string(tmp.num));
				this->busyRegs.push_back(tmp);
			}
		}
	}
}

void data_init() {
	mips << ".data" << endl;
	global_var_info.clear();
	it = codes.begin();
	Intermediate_code & code = *it;
	string tmp_label;
	stringstream parser;
	int dig;
	while (code.operation != FUNCTIONBEGIN) {
		VarInfo tmp;
		switch (code.operation)
		{
		case STR:
			tmp_label = string_label_generate();
			global_var_info.insert({ code.rd, VarInfo(code.rd, tmp_label) });
			mips_code_generate(tmp_label + ": .asciiz \"" + code.rs + "\"");
			break;
		case VAR:
			parser = stringstream(code.rs);
			parser >> dig;
			tmp_label = var_label_generate();
			tmp = VarInfo(code.rd, tmp_label);
			tmp.isArray = (dig != 0);
			global_var_info.insert({ code.rd, tmp });
			if (dig == 0) {
				mips_code_generate(tmp_label + ": .word 0");
			}
			else {
				mips_code_generate(".align 2");
				mips_code_generate(tmp_label + ": .space " + to_string(4 * dig));
			}
		case FUNCTIONBEGIN:
		case VARDEF:
		case VARDEFEND:
			break;
		default:
			//cout << "code generate Error!!!!" << endl;
			break;
		}
		++it;
		code = *it;
	}
	// mips_code_generate("__enter__: .asciiz \"\\n\"");
}

void function_generate() {
	
	int paraDigCount = 0;
	int varDigCount = 0;
	int tempCount = 0;
	int spCount = 0;
	int dig;
	isMain = false;
	function_var_info.clear();
	FunctionInfo function_context_info;
	string function_name;
	string function_label;
	Intermediate_code & code = *it;
	vector<Intermediate_code> function_context;
	stringstream parser;
	stringstream parser_2;
	vector<string> temp_array;
	vector<string> temp_para;
	int check_1;
	int check_2;
	block_index = 0;
	now_function_info = &function_context_info;

	if (code.operation == FUNCTIONBEGIN) {
		if (code.rd != "main" && function_inline_map.find(code.rd) != function_inline_map.end()) {
			// skip to end
			while ((*it).operation != FUNCTIONEND) {
				++it;
			}
			++it;
			return;
		}
		
	}
	
	while (code.operation != FUNCTIONEND) {
		switch (code.operation)
		{
		case FUNCTIONBEGIN:
			function_name = code.rd;
			if (function_name == "main") {
				isMain = true;
				function_label = "__main__";
			}
			else {
				function_label = func_label_generate();
			}
			global_var_info.insert({ code.rd, VarInfo(function_name, function_label) });
			mips_code_generate(function_label + ":");
			++tab;
			break;
		case PARADEF:
			++it;
			code = *it;
			while (code.operation != PARADEFEND) {
				++paraDigCount;
				++spCount;
				VarInfo var = VarInfo(code.rd, "", spCount);
				var.paraIndex = paraDigCount - 1;
				function_var_info.insert({ code.rd, var });
				temp_para.push_back(code.rd);
				++it;
				code = *it;
			}
			break;
		case VARDEF:
			++it;
			code = *it;
			while (code.operation != VARDEFEND) {
				parser = stringstream(code.rs);
				parser >> dig;
				bool isArray = (dig != 0);
				if (dig == 0) {
					dig = 1;
				}
				varDigCount += dig;
				spCount += dig;
				VarInfo tmp = VarInfo(code.rd, "", spCount);
				tmp.isArray = isArray;
				function_var_info.insert({ code.rd, tmp });
				if (isArray) {
					temp_array.push_back(code.rd);
				}
				++it;
				code = *it;
			}
			break;
		case BLE:
			parser = stringstream(code.rs);
			parser >> check_1;
			parser_2 = stringstream(code.rt);
			parser_2 >> check_2;
			if ((parser >> check_1) && (parser_2 >> check_2)) {
				if (check_1 <= check_2) {
					function_context.push_back(Intermediate_code(BEQ, code.rd, "0", "0"));
				}
			}
			else {
				function_context.push_back(code);
			}
			break;
		case BLT:
			parser = stringstream(code.rs);
			parser >> check_1;
			parser_2 = stringstream(code.rt);
			parser_2 >> check_2;
			if ((parser >> check_1) && (parser_2 >> check_2)) {
				if (check_1 < check_2) {
					function_context.push_back(Intermediate_code(BEQ, code.rd, "0", "0"));
				}
			}
			else {
				function_context.push_back(code);
			}
			break;
		case BEQ:
			parser = stringstream(code.rs);
			parser >> check_1;
			parser_2 = stringstream(code.rt);
			parser_2 >> check_2;
			if ((parser >> check_1) && (parser_2 >> check_2)) {
				if (check_1 == check_2) {
					function_context.push_back(Intermediate_code(BEQ, code.rd, "0", "0"));
				}
			}
			else {
				function_context.push_back(code);
			}
			break;
		case BNE:
			parser = stringstream(code.rs);
			parser >> check_1;
			parser_2 = stringstream(code.rt);
			parser_2 >> check_2;
			if ((parser >> check_1) && (parser_2 >> check_2)) {
				if (check_1 != check_2) {
					function_context.push_back(Intermediate_code(BEQ, code.rd, "0", "0"));
				}
			}
			else {
				function_context.push_back(code);
			}
			break;
		default:
			function_context.push_back(code);
			break;
		}
		++it;
		code = *it;
	}

	// block statistic
	
	BlockController *block;
	vector<Intermediate_code>::iterator context_iterator = function_context.begin();

	while (context_iterator != function_context.end()) {
		if ((*context_iterator).operation == RET) {
			++context_iterator;
			if (context_iterator == function_context.end()) {
				break;
			}
			while ((*context_iterator).operation != SETLABEL) {
				context_iterator = function_context.erase(context_iterator);
				if (context_iterator == function_context.end()) {
					break;
				}
			}
		}
		if (context_iterator != function_context.end()) {
			++context_iterator;
		}
	}

	context_iterator = function_context.begin();
	while (context_iterator != function_context.end()) {
		block = new BlockController();
		// check head
		if ((*context_iterator).operation == SETLABEL) {
			block->block_codes.push_back(*context_iterator);
			block->name = (*context_iterator).rs;
			++context_iterator;
		}
		else {
			block->name = block_name_generate();
		}
		function_context_info.blocks_name_book.insert({
			block->name,
			block
		});

		// until tail
		if (context_iterator != function_context.end()) {
			while (((*context_iterator).operation != SETLABEL)
				&& ((*context_iterator).operation != BLE)
				&& ((*context_iterator).operation != BEQ)
				&& ((*context_iterator).operation != BNE)
				&& ((*context_iterator).operation != BLT)
				&& ((*context_iterator).operation != CALL))
			{
				block->block_codes.push_back(*context_iterator);
				++context_iterator;
				if (context_iterator == function_context.end()) {
					break;
				}
			}
		}

		// put tail
		if (context_iterator != function_context.end()) {
			if ((*context_iterator).operation != SETLABEL) {
				block->block_codes.push_back(*context_iterator);
				++context_iterator;
			}
		}
		function_context_info.blocks.push_back(block);
	}

	vector<BlockController*>::iterator block_it;
	for (block_it = function_context_info.blocks.begin(); block_it != function_context_info.blocks.end(); ++block_it) {
		bool doubleAction = false;
		bool mustJump = false;
		Intermediate_code tmp;
		if ((*block_it)->block_codes.size() != 0) {
			tmp = *((*block_it)->block_codes.end() - 1);
			parser = stringstream(tmp.rs);
			parser_2 = stringstream(tmp.rt);
			switch (tmp.operation)
			{
			case BEQ:
				if ((parser >> check_1) && (parser_2 >> check_2)) {
					doubleAction = false;
					if (check_1 == check_2) {
						mustJump = true;
					}
					else {
						mustJump = false;
					}
				}
				else {
					doubleAction = true;
				}
				break;
			case BNE:
				if ((parser >> check_1) && (parser_2 >> check_2)) {
					doubleAction = false;
					if (check_1 != check_2) {
						mustJump = true;
					}
					else {
						mustJump = false;
					}
				}
				else {
					doubleAction = true;
				}
				break;
			case BLT:
				if ((parser >> check_1) && (parser_2 >> check_2)) {
					doubleAction = false;
					if (check_1 < check_2) {
						mustJump = true;
					}
					else {
						mustJump = false;
					}
				}
				else {
					doubleAction = true;
				}
				break;
			case BLE:
				if ((parser >> check_1) && (parser_2 >> check_2)) {
					doubleAction = false;
					if (check_1 <= check_2) {
						mustJump = true;
					}
					else {
						mustJump = false;
					}
				}
				else {
					doubleAction = true;
				}
				break;
			default:
				doubleAction = false;
				mustJump = false;
				break;
			} 
		}
		if (not doubleAction) {
			if (mustJump) {
				function_context_info.data_stream_after_map[*block_it].push_back(
					function_context_info.blocks_name_book[tmp.rd]
				);
			}
			else if (block_it != function_context_info.blocks.end() - 1) {
				function_context_info.data_stream_after_map[*block_it].push_back(
					*(block_it + 1)
				);
			}
		}
		else {
			function_context_info.data_stream_after_map[*block_it].push_back(
				function_context_info.blocks_name_book[tmp.rd]
			);
			if (block_it != function_context_info.blocks.end() - 1) {
				function_context_info.data_stream_after_map[*block_it].push_back(
					*(block_it + 1)
				);
			}
		}
	}

	// temp statistic
	for (int i = 0; i < function_context_info.blocks.size(); ++i) {
		block = function_context_info.blocks[i];
		for (int j = 0; j < block->block_codes.size(); ++j) {
			Intermediate_code & tmp_code = block->block_codes[j];
			if (tmp_code.rd.size() > 2 && tmp_code.rd.at(0) == '$' && tmp_code.rd.at(1) == 't') {
				if (check_temp_statistic(tmp_code.rd)) {
					++tempCount;
					++spCount;
					function_var_info.insert({ tmp_code.rd, VarInfo(tmp_code.rd, "", spCount) });
				}
			}
			if (tmp_code.rs.size() > 2 && tmp_code.rs.at(0) == '$' && tmp_code.rs.at(1) == 't') {
				if (check_temp_statistic(tmp_code.rs)) {
					++tempCount;
					++spCount;
					function_var_info.insert({ tmp_code.rs, VarInfo(tmp_code.rs, "", spCount) });
				}
			}
			if (tmp_code.rt.size() > 2 && tmp_code.rt.at(0) == '$' && tmp_code.rt.at(1) == 't') {
				if (check_temp_statistic(tmp_code.rt)) {
					++tempCount;
					++spCount;
					function_var_info.insert({ tmp_code.rt, VarInfo(tmp_code.rt, "", spCount) });
				}
			}
		}
	}

	  //////////////////
	 // block better //
	//////////////////

	// statistic use and def
	for (block_it = function_context_info.blocks.begin(); block_it != function_context_info.blocks.end(); ++block_it) {
		block = *block_it;
		for (context_iterator = block->block_codes.begin(); context_iterator != block->block_codes.end(); ++context_iterator) {
			Intermediate_code tmp = *context_iterator;
			switch (tmp.operation)
			{
			case BLE:
			case BLT:
			case BEQ:
			case BNE:
				if (block->def_set.find(tmp.rs) == block->def_set.end()) {
					parser = stringstream(tmp.rs);
					if (not ((parser >> check_1) || (tmp.rs.at(0) == '@') || ((tmp.rs.at(0) == '$') && (tmp.rs.at(1) != 't')))) {
						block->use_set.insert(tmp.rs);
					}
				}
				if (block->def_set.find(tmp.rt) == block->def_set.end()) {
					parser = stringstream(tmp.rt);
					if (not ((parser >> check_1) || (tmp.rt.at(0) == '@') || ((tmp.rt.at(0) == '$') && (tmp.rt.at(1) != 't')))) {
						block->use_set.insert(tmp.rt);
					}
				}
				break;
			case ADD:
			case SUB:
			case MULTOP:
			case DIVOP:
			case ARRLOAD:
				if (block->def_set.find(tmp.rs) == block->def_set.end()) {
					parser = stringstream(tmp.rs);
					if (not ((parser >> check_1) || (tmp.rs.at(0) == '@') || ((tmp.rs.at(0) == '$') && (tmp.rs.at(1) != 't')))) {
						block->use_set.insert(tmp.rs);
					}
				}
				if (block->def_set.find(tmp.rt) == block->def_set.end()) {
					parser = stringstream(tmp.rt);
					if (not ((parser >> check_1) || (tmp.rt.at(0) == '@') || ((tmp.rt.at(0) == '$') && (tmp.rt.at(1) != 't')))) {
						block->use_set.insert(tmp.rt);
					}
				}
				if (block->use_set.find(tmp.rd) == block->use_set.end()) {
					parser = stringstream(tmp.rd);
					if (not ((parser >> check_1) || (tmp.rd.at(0) == '@') || ((tmp.rd.at(0) == '$') && (tmp.rd.at(1) != 't')))) {
						block->def_set.insert(tmp.rd);
					}
				}
				break;
			case MOVE:
				if (block->def_set.find(tmp.rs) == block->def_set.end()) {
					parser = stringstream(tmp.rs);
					if (not ((parser >> check_1) || (tmp.rs.at(0) == '@') || ((tmp.rs.at(0) == '$') && (tmp.rs.at(1) != 't')))) {
						block->use_set.insert(tmp.rs);
					}
				}
				if (block->use_set.find(tmp.rd) == block->use_set.end()) {
					parser = stringstream(tmp.rd);
					if (not ((parser >> check_1) || (tmp.rd.at(0) == '@') || ((tmp.rd.at(0) == '$') && (tmp.rd.at(1) != 't')))) {
						block->def_set.insert(tmp.rd);
					}
				}
				break;
			case ARRSAVE:
				if (block->def_set.find(tmp.rs) == block->def_set.end()) {
					parser = stringstream(tmp.rs);
					if (not ((parser >> check_1) || (tmp.rs.at(0) == '@') || ((tmp.rs.at(0) == '$') && (tmp.rs.at(1) != 't')))) {
						block->use_set.insert(tmp.rs);
					}
				}
				if (block->def_set.find(tmp.rt) == block->def_set.end()) {
					parser = stringstream(tmp.rt);
					if (not ((parser >> check_1) || (tmp.rt.at(0) == '@') || ((tmp.rt.at(0) == '$') && (tmp.rt.at(1) != 't')))) {
						block->use_set.insert(tmp.rt);
					}
				}
				if (block->def_set.find(tmp.rd) == block->def_set.end()) {
					parser = stringstream(tmp.rd);
					if (not ((parser >> check_1) || (tmp.rd.at(0) == '@') || ((tmp.rd.at(0) == '$') && (tmp.rd.at(1) != 't')))) {
						block->use_set.insert(tmp.rd);
					}
				}
				break;
			case READ:
				if (block->use_set.find(tmp.rd) == block->use_set.end()) {
					parser = stringstream(tmp.rd);
					if (not ((parser >> check_1) || (tmp.rd.at(0) == '@') || ((tmp.rd.at(0) == '$') && (tmp.rd.at(1) != 't')))) {
						block->def_set.insert(tmp.rd);
					}
				}
				break;
			case PRINT:
				if (block->def_set.find(tmp.rd) == block->def_set.end()) {
					parser = stringstream(tmp.rd);
					if (not ((parser >> check_1) || (tmp.rd.at(0) == '@') || ((tmp.rd.at(0) == '$') && (tmp.rd.at(1) != 't')))) {
						block->use_set.insert(tmp.rd);
					}
				}
				break;
			default:
				break;
			}
		}
	}

	// statistic in out
	bool isChange = true;
	while (isChange) {
		isChange = false;
		for (block_it = function_context_info.blocks.begin(); block_it != function_context_info.blocks.end(); ++block_it) {
			// set<string> out;
			set<string> sub;
			vector<BlockController*>::iterator tmp_it;
			for (tmp_it = function_context_info.data_stream_after_map[*block_it].begin(); tmp_it != function_context_info.data_stream_after_map[*block_it].end(); ++tmp_it) {
				isChange |= (*block_it)->union_set(
					&((*block_it)->active_out_set),
					&((*block_it)->active_out_set),
					&((*tmp_it)->active_in_set)
				);
			}
			sub = (*block_it)->active_out_set;
			for (set<string>::iterator it = (*block_it)->def_set.begin(); it != (*block_it)->def_set.end(); ++it) {
				set<string>::iterator name_find = sub.find(*it);
				if (name_find != sub.end()) {
					sub.erase(name_find);
					// isChange |= true;
				}
			}
			isChange |= (*block_it)->union_set(
				&((*block_it)->active_in_set),
				&((*block_it)->use_set),
				&sub
			);
		}
	}

	// block dead clean

	for (block_it = function_context_info.blocks.begin(); block_it != function_context_info.blocks.end(); ++block_it) {
		block = *block_it;
		bool cando = true;
		bool isChange = true;
		while (cando) {
			isChange = true;
			cando = false;
			// statistic def use
			map<Intermediate_code*, set<string>> def_set;
			map<Intermediate_code*, set<string>> use_set;
			map<Intermediate_code*, set<string>> active_in_set;
			map<Intermediate_code*, set<string>> active_out_set;
			for (int i = 0; i != block->block_codes.size(); ++i) {
				switch (block->block_codes[i].operation)
				{
				case BLE:
				case BLT:
				case BEQ:
				case BNE:
					
					parser = stringstream(block->block_codes[i].rs);
					if (not ((parser >> check_1) || (block->block_codes[i].rs.at(0) == '@') || ((block->block_codes[i].rs.at(0) == '$') && (block->block_codes[i].rs.at(1) != 't')))) {
						use_set[&block->block_codes[i]].insert(block->block_codes[i].rs);
					}
		
					parser = stringstream(block->block_codes[i].rt);
					if (not ((parser >> check_1) || (block->block_codes[i].rt.at(0) == '@') || ((block->block_codes[i].rt.at(0) == '$') && (block->block_codes[i].rt.at(1) != 't')))) {
						use_set[&block->block_codes[i]].insert(block->block_codes[i].rt);
					}
					break;
				case ADD:
				case SUB:
				case MULTOP:
				case DIVOP:
				case ARRLOAD:
					parser = stringstream(block->block_codes[i].rs);
					if (not ((parser >> check_1) || (block->block_codes[i].rs.at(0) == '@') || ((block->block_codes[i].rs.at(0) == '$') && (block->block_codes[i].rs.at(1) != 't')))) {
						use_set[&block->block_codes[i]].insert(block->block_codes[i].rs);
					}

					parser = stringstream(block->block_codes[i].rt);
					if (not ((parser >> check_1) || (block->block_codes[i].rt.at(0) == '@') || ((block->block_codes[i].rt.at(0) == '$') && (block->block_codes[i].rt.at(1) != 't')))) {
						use_set[&block->block_codes[i]].insert(block->block_codes[i].rt);
					}

					parser = stringstream(block->block_codes[i].rd);
					if (not ((parser >> check_1) || (block->block_codes[i].rd.at(0) == '@') || ((block->block_codes[i].rd.at(0) == '$') && (block->block_codes[i].rd.at(1) != 't')))) {
						if (block->block_codes[i].rd != block->block_codes[i].rs && block->block_codes[i].rd != block->block_codes[i].rt) {
							def_set[&block->block_codes[i]].insert(block->block_codes[i].rd);
						}
					}
					
					break;
				case MOVE:
					parser = stringstream(block->block_codes[i].rd);
					if (not ((parser >> check_1) || (block->block_codes[i].rd.at(0) == '@') || ((block->block_codes[i].rd.at(0) == '$') && (block->block_codes[i].rd.at(1) != 't')))) {
						if (block->block_codes[i].rd != block->block_codes[i].rs) {
							def_set[&block->block_codes[i]].insert(block->block_codes[i].rd);
						}
					}

					parser = stringstream(block->block_codes[i].rs);
					if (not ((parser >> check_1) || (block->block_codes[i].rs.at(0) == '@') || ((block->block_codes[i].rs.at(0) == '$') && (block->block_codes[i].rs.at(1) != 't')))) {
						use_set[&block->block_codes[i]].insert(block->block_codes[i].rs);
					}

					break;
				case ARRSAVE:
					parser = stringstream(block->block_codes[i].rs);
					if (not ((parser >> check_1) || (block->block_codes[i].rs.at(0) == '@') || ((block->block_codes[i].rs.at(0) == '$') && (block->block_codes[i].rs.at(1) != 't')))) {
						use_set[&block->block_codes[i]].insert(block->block_codes[i].rs);
					}

					parser = stringstream(block->block_codes[i].rt);
					if (not ((parser >> check_1) || (block->block_codes[i].rt.at(0) == '@') || ((block->block_codes[i].rt.at(0) == '$') && (block->block_codes[i].rt.at(1) != 't')))) {
						use_set[&block->block_codes[i]].insert(block->block_codes[i].rt);
					}

					parser = stringstream(block->block_codes[i].rd);
					if (not ((parser >> check_1) || (block->block_codes[i].rd.at(0) == '@') || ((block->block_codes[i].rd.at(0) == '$') && (block->block_codes[i].rd.at(1) != 't')))) {
						use_set[&block->block_codes[i]].insert(block->block_codes[i].rd);
					}
					break;
				case READ:
					parser = stringstream(block->block_codes[i].rd);
					if (not ((parser >> check_1) || (block->block_codes[i].rd.at(0) == '@') || ((block->block_codes[i].rd.at(0) == '$') && (block->block_codes[i].rd.at(1) != 't')))) {
						def_set[&block->block_codes[i]].insert(block->block_codes[i].rd);
					}
					break;
				case PRINT:
					parser = stringstream(block->block_codes[i].rd);
					if (not ((parser >> check_1) || (block->block_codes[i].rd.at(0) == '@') || ((block->block_codes[i].rd.at(0) == '$') && (block->block_codes[i].rd.at(1) != 't')))) {
						use_set[&block->block_codes[i]].insert(block->block_codes[i].rd);
					}
					break;
				default:
					break;
				}
			}

			active_out_set[&block->block_codes[block->block_codes.size() - 1]] = block->active_out_set;
			// statistic in out

			while (isChange) {
				isChange = false;
				for (int i = 0; i != block->block_codes.size(); ++i) {
					if (i != block->block_codes.size() - 1) {
						isChange |= block->union_set(
							&active_out_set[&block->block_codes[i]],
							&active_in_set[&block->block_codes[i + 1]],
							&active_out_set[&block->block_codes[i]]
						);
					}
					set<string> sub;
					sub = active_out_set[&block->block_codes[i]];
					for (set<string>::iterator it = def_set[&block->block_codes[i]].begin(); it != def_set[&block->block_codes[i]].end(); ++it) {
						set<string>::iterator name_find = sub.find(*it);
						if (name_find != sub.end()) {
							sub.erase(name_find);
							// isChange |= true;
						}
					}

					isChange |= block->union_set(
						&active_in_set[&block->block_codes[i]],
						&use_set[&block->block_codes[i]],
						&sub
					);

				}
			}

			// remove dead
			vector<Intermediate_code>::iterator remove;
			for (vector<Intermediate_code>::iterator i = block->block_codes.begin(); i != block->block_codes.end(); ++i) {
				VarInfo *find;
				switch ((*i).operation)
				{
				case ADD:
				case SUB:
				case MULTOP:
				case DIVOP:
				case MOVE:
				//case READ:
				case ARRLOAD:
					parser = stringstream((*i).rd);
					if (not ((parser >> check_1) || ((*i).rd.at(0) == '@') || (((*i).rd.at(0) == '$') && ((*i).rd.at(1) != 't')))) {
						find = find_var_info((*i).rd);
						if (not (find != NULL && find->isGlobal)) {
							if (active_out_set[&(*i)].find((*i).rd) == active_out_set[&(*i)].end()) {
								cando = true;
								remove = i;
							}
						}
					}
					break;
				default:
					break;
				}
				if (cando) {
					break;
				}
			}

			if (cando) {
				block->block_codes.erase(remove);
			}
		}
	}

	if (function_name == "main") {
		ofstream code_out;
		code_out.open("dead.txt");
		middle_before.open("ffff.txt");
		for (block_it = function_context_info.blocks.begin(); block_it != function_context_info.blocks.end(); ++block_it) {
			for (int i = 0; i != (*block_it)->block_codes.size(); ++i) {
				code_out << (*block_it)->block_codes[i] << endl;
			}
		}
		middle_before.close();
		code_out.close();
	}

	/*for (block_it = function_context_info.blocks.begin(); block_it != function_context_info.blocks.end(); ++block_it) {
		cout << "name is :" << (*block_it)->name << endl;
		for (set<string>::iterator i = (*block_it)->active_out_set.begin(); i != (*block_it)->active_out_set.end(); ++i) {
			cout << *i << endl;
		}
		cout << "---------------" << endl;
	}*/

	// statistic use times
	for (context_iterator = function_context.begin(); context_iterator != function_context.end(); ++context_iterator) {
		Intermediate_code tmp = *context_iterator;
		switch ((*context_iterator).operation)
		{
		case ADD:
		case SUB:
		case MULTOP:
		case DIVOP:
		case ARRLOAD:
		case ARRSAVE:
			parser = stringstream(tmp.rs);
			if (not ((parser >> check_1) || (tmp.rs.at(0) == '@') || ((tmp.rs.at(0) == '$') && (tmp.rs.at(1) != 't')))) {
				if (function_context_info.name_use_times.find(tmp.rs) == function_context_info.name_use_times.end()) {
					function_context_info.name_use_times[tmp.rs] = 1;
				}
				else {
					function_context_info.name_use_times[tmp.rs] += 1;
				}
			}
			parser = stringstream(tmp.rt);
			if (not ((parser >> check_1) || (tmp.rt.at(0) == '@') || ((tmp.rt.at(0) == '$') && (tmp.rt.at(1) != 't')))) {
				if (function_context_info.name_use_times.find(tmp.rt) == function_context_info.name_use_times.end()) {
					function_context_info.name_use_times[tmp.rt] = 1;
				}
				else {
					function_context_info.name_use_times[tmp.rt] += 1;
				}
			}
			parser = stringstream(tmp.rd);
			if (not ((parser >> check_1) || (tmp.rd.at(0) == '@') || ((tmp.rd.at(0) == '$') && (tmp.rd.at(1) != 't')))) {
				if (function_context_info.name_use_times.find(tmp.rd) == function_context_info.name_use_times.end()) {
					function_context_info.name_use_times[tmp.rd] = 1;
				}
				else {
					function_context_info.name_use_times[tmp.rd] += 1;
				}
			}
			break;
		case BLE:
		case BEQ:
		case BNE:
		case BLT:
			parser = stringstream(tmp.rs);
			if (not ((parser >> check_1) || (tmp.rs.at(0) == '@') || ((tmp.rs.at(0) == '$') && (tmp.rs.at(1) != 't')))) {
				if (function_context_info.name_use_times.find(tmp.rs) == function_context_info.name_use_times.end()) {
					function_context_info.name_use_times[tmp.rs] = 1;
				}
				else {
					function_context_info.name_use_times[tmp.rs] += 1;
				}
			}
			parser = stringstream(tmp.rt);
			if (not ((parser >> check_1) || (tmp.rt.at(0) == '@') || ((tmp.rt.at(0) == '$') && (tmp.rt.at(1) != 't')))) {
				if (function_context_info.name_use_times.find(tmp.rt) == function_context_info.name_use_times.end()) {
					function_context_info.name_use_times[tmp.rt] = 1;
				}
				else {
					function_context_info.name_use_times[tmp.rt] += 1;
				}
			}
			break;
		case MOVE:
			parser = stringstream(tmp.rs);
			if (not ((parser >> check_1) || (tmp.rs.at(0) == '@') || ((tmp.rs.at(0) == '$') && (tmp.rs.at(1) != 't')))) {
				if (function_context_info.name_use_times.find(tmp.rs) == function_context_info.name_use_times.end()) {
					function_context_info.name_use_times[tmp.rs] = 1;
				}
				else {
					function_context_info.name_use_times[tmp.rs] += 1;
				}
			}
			parser = stringstream(tmp.rd);
			if (not ((parser >> check_1) || (tmp.rd.at(0) == '@') || ((tmp.rd.at(0) == '$') && (tmp.rd.at(1) != 't')))) {
				if (function_context_info.name_use_times.find(tmp.rd) == function_context_info.name_use_times.end()) {
					function_context_info.name_use_times[tmp.rd] = 1;
				}
				else {
					function_context_info.name_use_times[tmp.rd] += 1;
				}
			}
			break;
		case READ:
		case PRINT:
			parser = stringstream(tmp.rd);
			if (not ((parser >> check_1) || (tmp.rd.at(0) == '@') || ((tmp.rd.at(0) == '$') && (tmp.rd.at(1) != 't')))) {
				if (function_context_info.name_use_times.find(tmp.rd) == function_context_info.name_use_times.end()) {
					function_context_info.name_use_times[tmp.rd] = 1;
				}
				else {
					function_context_info.name_use_times[tmp.rd] += 1;
				}
			}
			break;
		default:
			break;
		}
	}


	

	// alloc global regs
	vector<pair<string, int>> use_times_pairs;
	for (map<string, int>::iterator i = function_context_info.name_use_times.begin(); i != function_context_info.name_use_times.end(); ++i) {
		use_times_pairs.push_back(make_pair(i->first, i->second));
	}
	sort(use_times_pairs.begin(), use_times_pairs.end(), function_context_info.times_sort);
	/*cout << function_name << endl;
	for (vector<pair<string, int>>::iterator i = use_times_pairs.begin(); i != use_times_pairs.end(); ++i) {
		cout << i->first << ": " << i->second << endl;
	}
	cout << endl;*/
	int globalRegsSum = function_context_info.global_regs.size();
	map<int, Reg>::iterator reg_alloc = function_context_info.global_regs.begin();
	for (vector<pair<string, int>>::iterator i = use_times_pairs.begin(); i != use_times_pairs.end(); ++i) {
		VarInfo *find = find_var_info(i->first);
		
		(*reg_alloc).second.info = find;
		find->regNum = reg_alloc->second.num;
		--globalRegsSum;
		++reg_alloc;
		if (globalRegsSum <= 0) {
			break;
		}
	}

	// push stack
	mips_code_generate("addiu $sp, $sp, -" + to_string((spCount) * 4));
	
	// block code generate
	for (int i = 0; i < function_context_info.blocks.size(); ++i) {
		block = function_context_info.blocks[i];
		int paraCount = 1;
		bool isPrecall = false;
		if (i == 0) {
			for (set<string>::iterator j = block->active_in_set.begin(); j != block->active_in_set.end(); ++j) {
				if (find(temp_array.begin(), temp_array.end(), *j) != temp_array.end()) {
					string name = *j;
					VarInfo *find = find_var_info(name);
					if (find->regNum != -1) {
						find->allocReg(find->regNum, spCount, true);
					}
				}
				else if (find(temp_para.begin(), temp_para.end(), *j) != temp_para.end()) {
					string name = *j;
					VarInfo *find = find_var_info(name);
					if (find->regNum != -1) {
						find->allocReg(find->regNum, spCount, true);
					}
				}
				else {
					map<string,VarInfo>::iterator find;
					find = global_var_info.find(*j);
					if (find != global_var_info.end()) {
						if (find->second.regNum != -1) {
							find->second.allocReg(find->second.regNum, spCount, true);
						}
					}
				}
			}
		}
		for (int j = 0; j < block->block_codes.size(); ++j) {
			code = block->block_codes[j];
			vector<string> *result = new vector<string>();
			vector<VarInfo*> *vars = new vector<VarInfo*>();
			VarInfo *find;
			VarInfo rs;
			VarInfo rt;
			VarInfo rd;
			rs.isFake = true;
			rt.isFake = true;
			rd.isFake = true;
			stringstream check;
			int holder;
			int holder_2;
			int intStatistic;
			string branch;
			switch (code.operation)
			{
			case ADD:
				intStatistic = 0;
				
				// rd
				find = find_var_info(code.rd);
				if (find == NULL) {
					rd.needLoad = false;
					rd.isDirty = true;
					rd.name = code.rd;
					vars->push_back(&rd);
				}
				else {
					find->needLoad = false;
					find->isDirty = true;
					vars->push_back(find);
				}

				// rs
				check = stringstream(code.rs);
				if (check >> holder) {
					++intStatistic;
					rs.name = code.rs;
				}
				else {
					find = find_var_info(code.rs);
					if (find == NULL) {
						rs.name = code.rs;
						vars->push_back(&rs);
					}
					else {
						find->needLoad = true;
						vars->push_back(find);
					}
				}

				// rt
				check = stringstream(code.rt);
				if (check >> holder) {
					intStatistic+=2;
					rt.name = code.rt;
				}
				else {
					find = find_var_info(code.rt);
					if (find == NULL) {
						rt.name = code.rt;
						vars->push_back(&rt);
					}
					else {
						find->needLoad = true;
						vars->push_back(find);
					}
				}

				// generate
				if (intStatistic == 3) {
					vars->push_back(&rs);
					block->regs_alloc_and_free(vars, spCount, result);
					// debug check
					if (result->size() != 2) {
						//cout << "error at 368!!!!!" << endl;
					}
					mips_code_generate("addiu " + (*result)[0]
										+ ", " + (*result)[1]
										+ ", " + rt.name);
				}
				else if (intStatistic != 0) {
					block->regs_alloc_and_free(vars, spCount, result);
					if (intStatistic == 1) {
						mips_code_generate("addiu " + (*result)[0]
										+ ", " + (*result)[1]
										+ ", " + rs.name);
					}
					else {
						mips_code_generate("addiu " + (*result)[0]
										+ ", " + (*result)[1]
										+ ", " + rt.name);
					}
				}
				else {
					block->regs_alloc_and_free(vars, spCount, result);
					mips_code_generate("addu " + (*result)[0]
										+ ", " + (*result)[1]
										+ ", " + (*result)[2]);
				}

				break;
			case SUB:
				intStatistic = 0;

				// rd
				find = find_var_info(code.rd);
				if (find == NULL) {
					rd.needLoad = false;
					rd.isDirty = true;
					rd.name = code.rd;
					vars->push_back(&rd);
				}
				else {
					find->needLoad = false;
					find->isDirty = true;
					vars->push_back(find);
				}
				

				// rs
				find = find_var_info(code.rs);
				if (find == NULL) {
					rs.name = code.rs;
					vars->push_back(&rs);
				}
				else {
					find->needLoad = true;
					vars->push_back(find);
				}
				

				// rt
				check = stringstream(code.rt);
				if (check >> holder) {
					++intStatistic;
					rt.name = code.rt;
				}
				else {
					find = find_var_info(code.rt);
					if (find == NULL) {
						rt.name = code.rt;
						vars->push_back(&rt);
					}
					else {
						find->needLoad = true;
						vars->push_back(find);
					}
				}

				// generate
				if (intStatistic != 0) {
					block->regs_alloc_and_free(vars, spCount, result);
					
					mips_code_generate("addiu " + (*result)[0]
										 + ", " + (*result)[1]
										 + ", " + to_string(-holder));
				}
				else {
					block->regs_alloc_and_free(vars, spCount, result);
					mips_code_generate("subu " + (*result)[0]
										+ ", " + (*result)[1]
										+ ", " + (*result)[2]);
				}
				break;
			case MOVE:
				if (isPrecall) {
					++paraCount;
					intStatistic = 0;

					// rs
					check = stringstream(code.rs);
					if (check >> holder) {
						++intStatistic;
						rs.name = code.rs;
						vars->push_back(&rs);
					}
					else {
						find = find_var_info(code.rs);
						if (find == NULL) {
							rs.name = code.rs;
							vars->push_back(&rs);
						}
						else {
							find->needLoad = true;
							vars->push_back(find);
						}
					}

					// generate
					
					block->regs_alloc_and_free(vars, spCount, result);
					mips_code_generate("sw " + (*result)[0]
							+ ", " + to_string(-paraCount * 4)
							+ "($sp)");
					
					
					break;
				}

				if (code.rd == code.rs) {
					break;
				}
				intStatistic = 0;

				//rd
				find = find_var_info(code.rd);
				if (find == NULL) {
					rd.name = code.rd;
					rd.isDirty = true;
					vars->push_back(&rd);
				}
				else {
					find->needLoad = false;
					find->isDirty = true;
					vars->push_back(find);
				}

				// rs
				check = stringstream(code.rs);
				if (check >> holder) {
					++intStatistic;
					rs.name = code.rs;
				}
				else {
					find = find_var_info(code.rs);
					if (find == NULL) {
						rs.name = code.rs;
						vars->push_back(&rs);
					}
					else {
						find->needLoad = true;
						vars->push_back(find);
					}
				}

				// generate
				if (intStatistic == 0) {
					block->regs_alloc_and_free(vars, spCount, result);
					mips_code_generate("addu " + (*result)[0]
										+ ", " + (*result)[1]
										+ ", $0");
				}
				else {
					block->regs_alloc_and_free(vars, spCount, result);
					mips_code_generate("addiu " + (*result)[0]
										+ ", $0, " + rs.name);
				}
				break;
			case MULTOP:
				intStatistic = 0;

				// rd
				find = find_var_info(code.rd);
				if (find == NULL) {
					rd.needLoad = false;
					rd.isDirty = true;
					rd.name = code.rd;
					vars->push_back(&rd);
				}
				else {
					find->needLoad = false;
					find->isDirty = true;
					vars->push_back(find);
				}

				// rs
				check = stringstream(code.rs);
				if (check >> holder) {
					++intStatistic;
					rs.name = code.rs;
				}
				else {
					find = find_var_info(code.rs);
					if (find == NULL) {
						rs.name = code.rs;
						vars->push_back(&rs);
					}
					else {
						find->needLoad = true;
						vars->push_back(find);
					}
				}

				// rt
				check = stringstream(code.rt);
				if (check >> holder) {
					intStatistic += 2;
					rt.name = code.rt;
				}
				else {
					find = find_var_info(code.rt);
					if (find == NULL) {
						rt.name = code.rt;
						vars->push_back(&rt);
					}
					else {
						find->needLoad = true;
						vars->push_back(find);
					}
				}

				// generate
				if (intStatistic == 3) {
					vars->push_back(&rs);
					block->regs_alloc_and_free(vars, spCount, result);
					// debug check
					if (result->size() != 2) {
						//cout << "error at 368!!!!!" << endl;
					}
					mips_code_generate("mul " + (*result)[0]
										+ ", " + (*result)[1]
										+ ", " + rt.name);
				}
				else if (intStatistic != 0) {
					block->regs_alloc_and_free(vars, spCount, result);
					if (intStatistic == 1) {
						mips_code_generate("mul " + (*result)[0]
											+ ", " + (*result)[1]
											+ ", " + rs.name);
					}
					else {
						mips_code_generate("mul " + (*result)[0]
											+ ", " + (*result)[1]
											+ ", " + rt.name);
					}
				}
				else {
					block->regs_alloc_and_free(vars, spCount, result);
					mips_code_generate("mul " + (*result)[0]
										+ ", " + (*result)[1]
										+ ", " + (*result)[2]);
				}

				break;
			case DIVOP:
				intStatistic = 0;

				// rd
				find = find_var_info(code.rd);
				if (find == NULL) {
					rd.needLoad = false;
					rd.isDirty = true;
					rd.name = code.rd;
					vars->push_back(&rd);
				}
				else {
					find->needLoad = false;
					find->isDirty = true;
					vars->push_back(find);
				}

				// rs
				find = find_var_info(code.rs);
				if (find == NULL) {
					rs.name = code.rs;
					vars->push_back(&rs);
				}
				else {
					find->needLoad = true;
					vars->push_back(find);
				}


				// rt
				
				find = find_var_info(code.rt);
				if (find == NULL) {
					rt.name = code.rt;
					vars->push_back(&rt);
				}
				else {
					find->needLoad = true;
					vars->push_back(find);
				}
				

				// generate
				/*if (intStatistic != 0) {
					block->regs_alloc_and_free(vars, spCount, result);

					mips_code_generate("div " + (*result)[0]
										+ ", " + (*result)[1]
										+ ", " + rt.name);
				}
				else {*/
				block->regs_alloc_and_free(vars, spCount, result);
				mips_code_generate("div "  + (*result)[1]
									+ ", " + (*result)[2]);
				mips_code_generate("mflo " + (*result)[0]);
				//}
				break;
			case READ:
				if (code.rs == "int") {
					mips_code_generate("li $v0, 5");
					mips_code_generate("syscall");
					// rd
					find = find_var_info(code.rd);
					find->needLoad = false;
					find->isDirty = true;
					vars->push_back(find);

					block->regs_alloc_and_free(vars, spCount, result);
					mips_code_generate("addu " + (*result)[0]
										+ ", $v0, $0");
				}
				else if (code.rs == "char") {
					mips_code_generate("li $v0, 12");
					mips_code_generate("syscall");
					// rd
					find = find_var_info(code.rd);
					find->needLoad = false;
					find->isDirty = true;
					vars->push_back(find);

					block->regs_alloc_and_free(vars, spCount, result);
					mips_code_generate("addu " + (*result)[0]
										+ ", $v0, $0");
				}
				break;
			case PRINT:
				if (code.rs == "string") {
					find = find_var_info(code.rd);
					mips_code_generate("la $a0, " + find->label);
					mips_code_generate("li $v0, 4");
					mips_code_generate("syscall");
				}
				else {
					intStatistic = 0;
					// rd
					check = stringstream(code.rd);
					if (check >> holder) {
						++intStatistic;
						rd.name = code.rd;
					}
					else {
						find = find_var_info(code.rd);
						if (find == NULL) {
							rd.name = code.rd;
							vars->push_back(&rd);
						}
						else {
							find->needLoad = true;
							vars->push_back(find);
						}
					}
					if (code.rs == "int") {
						mips_code_generate("li $v0, 1");
						if (intStatistic == 0) {
							block->regs_alloc_and_free(vars, spCount, result);
							if ((*result)[0] != "$a0")
								mips_code_generate("addu $a0, $0, " + (*result)[0]);
						}
						else {
							mips_code_generate("li $a0, " + rd.name);
						}
						mips_code_generate("syscall");
					}
					else if (code.rs == "char") {
						mips_code_generate("li $v0, 11");
						if (intStatistic == 0) {
							block->regs_alloc_and_free(vars, spCount, result);
							if ((*result)[0] != "$a0")
								mips_code_generate("addu $a0, $0, " + (*result)[0]);
						}
						else {
							mips_code_generate("li $a0, " + rd.name);
						}
						mips_code_generate("syscall");
					}
				}
				break;
			case ARRLOAD:
				intStatistic = 0;
				// rd
				find = find_var_info(code.rd);
				if (find == NULL) {
					rd.name = code.rd;
					rd.isDirty = true;
					vars->push_back(&rd);
				}
				else {
					find->needLoad = false;
					find->isDirty = true;
					vars->push_back(find);
				}

				// rs
				find = find_var_info(code.rs);
				find->needLoad = true;
				vars->push_back(find);

				// rt
				check = stringstream(code.rt);
				if (check >> holder) {
					++intStatistic;
					rt.name = code.rt;
				}
				else {
					find = find_var_info(code.rt);
					if (find == NULL) {
						rt.name = code.rt;
						vars->push_back(&rt);
					}
					else {
						find->needLoad = true;
						vars->push_back(find);
					}
				}

				if (intStatistic != 0) {
					block->regs_alloc_and_free(vars, spCount, result);
					mips_code_generate("lw " + (*result)[0]
										+ ", " + to_string(holder * 4) + "("
										+ (*result)[1] + ")");
				}
				else {
					block->regs_alloc_and_free(vars, spCount, result);
					mips_code_generate("sll " + (*result)[0]
										+ ", " + (*result)[2]
										+ ", 2");
					mips_code_generate("addu " + (*result)[0]
										+ ", " + (*result)[1]
										+ ", " + (*result)[0]);
					mips_code_generate("lw " + (*result)[0]
										+ ", 0(" + (*result)[0] + ")");
				}
				break;
			case ARRSAVE:
				intStatistic = 0;

				// rd
				find = find_var_info(code.rd);
				if (find == NULL) {
					rd.name = code.rd;
					vars->push_back(&rd);
				}
				else {
					find->needLoad = true;
					vars->push_back(find);
				}

				// rs
				check = stringstream(code.rs);
				if (check >> holder) {
					++intStatistic;
					rs.name = to_string(holder * 4);
				}
				else {
					find = find_var_info(code.rs);
					if (find == NULL) {
						rs.name = code.rs;
						vars->push_back(&rs);
					}
					else {
						find->needLoad = true;
						vars->push_back(find);
					}
				}

				// rt
				check = stringstream(code.rt);
				if (check >> holder) {
					intStatistic+=2;
					rt.name = code.rt;
				}
				else {
					find = find_var_info(code.rt);
					if (find == NULL) {
						rt.name = code.rt;
						vars->push_back(&rt);
					}
					else {
						find->needLoad = true;
						vars->push_back(find);
					}
				}

				// generate
				block->regs_alloc_and_free(vars, spCount, result);
				if (intStatistic == 0) {
					mips_code_generate("sll $a0, " + (*result)[1] + ", 2");
					mips_code_generate("addu $a0, " + (*result)[0]
											+ ", $a0");
					mips_code_generate("sw " + (*result)[2]
											+ ", 0($a0)");
				}
				else if (intStatistic == 1) {
					mips_code_generate("sw " + (*result)[1]
										+ ", " + rs.name
										+ "(" + (*result)[0] + ")");
				}
				else if (intStatistic == 2) {
					mips_code_generate("sll $a0, " + (*result)[1] + ", 2");
					mips_code_generate("addu $a0, " + (*result)[0]
											+ ", $a0");
					mips_code_generate("li $a1, " + rt.name);
					mips_code_generate("sw $a1, 0($a0)");
				}
				else if (intStatistic == 3) {
					mips_code_generate("li $a1, " + rt.name);
					mips_code_generate("sw $a1, " + rs.name + "("
											+ (*result)[0] + ")");
				}

				break;
			case PRECALL:
				paraCount = 1;
				mips_code_generate("sw $ra, -4($sp)");
				isPrecall = true;
				break;
			case PRECALLEND:
				isPrecall = false;
				
				break;
			case CALL:
				for (int j = 0; j < block->busyRegs.size(); ++j) {
					VarInfo *tmp = block->busyRegs[j].info;
					if (tmp != NULL) {
						tmp->freeReg(spCount);
					}
				}
				now_function_info->save_global_regs(spCount, block);
				block->busyRegs.clear();
				block->validRegs = all_valid_regs;
				find = find_var_info(code.rs);
				mips_code_generate("addiu $sp, $sp, -4");
				mips_code_generate("jal " + find->label);
				mips_code_generate("lw $ra, 0($sp)");
				mips_code_generate("addiu $sp, $sp, 4");
				now_function_info->load_global_regs(spCount, block);
				break;
			case SETLABEL:
				mips_code_generate(code.rs + ":");
				break;
			case ADDINDENT:
				++tab;
				break;
			case REDUCEINDENT:
				--tab;
				break;
			case BLE:
			case BLT:
			case BNE:
			case BEQ:
				switch (code.operation)
				{
				case BLE:
					branch = "ble";
					break;
				case BLT:
					branch = "blt";
					break;
				case BNE:
					branch = "bne";
					break;
				case BEQ:
					branch = "beq";
					break;
				default:
					break;
				}
				intStatistic = 0;

				// rd
				// label

				// rs
				check = stringstream(code.rs);
				if (check >> holder) {
					++intStatistic;
					rs.name = code.rs;
					vars->push_back(&rs);
				}
				else {
					find = find_var_info(code.rs);
					if (find == NULL) {
						rs.name = code.rs;
						vars->push_back(&rs);
					}
					else {
						find->needLoad = true;
						vars->push_back(find);
					}
				}

				// rt
				check = stringstream(code.rt);
				if (check >> holder_2) {
					intStatistic += 2;
					rt.name = code.rt;
					vars->push_back(&rt);
				}
				else {
					find = find_var_info(code.rt);
					if (find == NULL) {
						rt.name = code.rt;
						vars->push_back(&rt);
					}
					else {
						find->needLoad = true;
						vars->push_back(find);
					}
				}

				// alloc regs
				block->regs_alloc_and_free(vars, spCount, result);
				

				// free reg
				for (int j = 0; j < block->busyRegs.size(); ++j) {
					VarInfo *tmp = block->busyRegs[j].info;
					if (tmp != NULL) {
						block->freeReg(spCount, tmp, true);
					}
				}
				// generate

				mips_code_generate(branch + " " + (*result)[0]
										+ ", " + (*result)[1]
										+ ", " + code.rd);
	
				break;
			case END:
			case RET:
				for (int j = 0; j < block->busyRegs.size(); ++j) {
					VarInfo *tmp = block->busyRegs[j].info;
					if (tmp != NULL) {
						block->freeReg(spCount, tmp, true, true);
					}
				}
				now_function_info->save_global_regs(spCount, block, true, true);
				mips_code_generate("j " + function_label + "__end__");
				break;
			default:
				break;
			}
			delete result;
			delete vars;
		}
		bool isReturn = false;
		if (i == function_context_info.blocks.size() - 1) {
			isReturn = true;
		}
		for (int j = 0; j < block->busyRegs.size(); ++j) {
			
			VarInfo *tmp = block->busyRegs[j].info;
			if (tmp != NULL) {
				block->freeReg(spCount, tmp, true, isReturn);
			}
		}
		
	}
	if (not isMain) {
		mips_code_generate("li $v0, 0");
	}
	--tab;
	mips_code_generate(function_label + "__end__:");
	++tab;
	mips_code_generate("addiu $sp, $sp, " + to_string(spCount * 4));
	if (isMain) {
		mips_code_generate("li $v0, 10");
		mips_code_generate("syscall");
	}
	else {
		mips_code_generate("jr $ra");
	}
	--tab;
	++it;
	for (map<int, Reg>::iterator i = now_function_info->global_regs.begin(); i != now_function_info->global_regs.end(); ++i) {
		if (i->second.info != NULL) {
			i->second.info->regNum = -1;
		}
	}
}