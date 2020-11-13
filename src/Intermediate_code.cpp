#include "Intermediate_code.h"
#include "main.h"
extern ofstream middle_before;
extern ofstream middle_after;
extern bool isPara;

Intermediate_code::Intermediate_code() {

}

Intermediate_code::Intermediate_code(CodeOperation op, string rd, string rs, string rt) {
	this->operation = op;
	this->rd = rd;
	this->rs = rs;
	this->rt = rt;
}

ostream &operator<<(ostream &output, const Intermediate_code &code) {
	switch (code.operation)
	{
	case VARDEF:
		output << "VARDEF";
		break;
	case CONST:
		// rd = rs
		output << "CONST " << code.rd << " " << code.rs;
		break;
	case VAR:
		// var rd[rs] type rt
		output << "VAR " << code.rd << "[" << code.rs << "] " << code.rt;
		if (isPara) {
			middle_before << "para " << code.rt << " " << code.rd << endl;
		}
		else {
			middle_before << "var " << code.rt << " " << code.rd;
			if (code.rs != "0") {
				middle_before << "[" << code.rs << "]" << endl;
			}
			else {
				middle_before << endl;
			}
		}
		break;
	case VARDEFEND:
		output << "VARDEFEND";
		break;
	case BLE:
		// if rs <= rt branch rd(label)
		output << "BLE " << code.rs << " " << code.rt << " " << code.rd;
		middle_before << code.rs << " <= " << code.rt << endl;
		middle_before << "BZ " << code.rd << endl;
		break;
	case BEQ:
		// if rs == rt branch rd(label)
		output << "BEQ " << code.rs << " " << code.rt << " " << code.rd;
		middle_before << code.rs << " == " << code.rt << endl;
		middle_before << "BZ " << code.rd << endl;
		break;
	case BNE:
		// if rs != rt branch rd(label)
		output << "BNE " << code.rs << " " << code.rt << " " << code.rd;
		middle_before << code.rs << " != " << code.rt << endl;
		middle_before << "BZ " << code.rd << endl;
		break;
	case BLT:
		// if rs < rt branch rd(label)
		output << "BLT " << code.rs << " " << code.rt << " " << code.rd;
		middle_before << code.rs << " < " << code.rt << endl;
		middle_before << "BZ " << code.rd << endl;
		break;
	case ADD:
		// rd = rs + rt
		output << "ADD " << code.rd << " " << code.rs << " " << code.rt;
		middle_before << code.rd << " = " << code.rs << " + " << code.rt << endl;
		break;
	case SUB:
		// rd = rs - rt
		output << "SUB " << code.rd << " " << code.rs << " " << code.rt;
		middle_before << code.rd << " = " << code.rs << " - " << code.rt << endl;
		break;
	case MULTOP:
		// rd = rs * rt
		output << "MULT " << code.rd << " " << code.rs << " " << code.rt;
		middle_before << code.rd << " = " << code.rs << " * " << code.rt << endl;
		break;
	case DIVOP:
		// rd = rs / rt
		output << "DIV " << code.rd << " " << code.rs << " " << code.rt;
		middle_before << code.rd << " = " << code.rs << " / " << code.rt << endl;
		break;
	case PRECALL:
		output << "PRECALL";
		break;
	case CALL:
		// call rs(label)
		output << "CALL " << code.rs;
		middle_before << "CALL " << code.rs << endl;
		break;
	case PRECALLEND:
		output << "PRECALLEND";
		break;
	case MOVE:
		// rd = rs
		output << "MOVE " << code.rd << " " << code.rs;
		if (code.rd.at(0) == '$' && code.rd.at(1) == 'a') {
			middle_before << "push " << code.rs << endl;
		}
		else if (code.rs.at(0) == '$' && code.rs.at(1) == 'v') {
			middle_before << code.rd << " = RET" << endl;
		}
		else {
			middle_before << code.rd << " = " << code.rs << endl;
		}
		break;
	case ARRLOAD:
		// rd = rs[rt];
		output << "ARRLOAD " << code.rd << " " << code.rs << " " << code.rt;
		middle_before << code.rd << " = " << code.rs << "[" << code.rt << "]" << endl;
		break;
	case ARRSAVE:
		// rd[rs] = rt;
		output << "ARRSAVE " << code.rd << " " << code.rs << " " << code.rt;
		middle_before << code.rd << "[" << code.rs << "]" << " = " << code.rt << endl;
		break;
	case SETLABEL:
		// label = rs
		output << "SETLABEL " << code.rs;
		middle_before << code.rs << ":" << endl;
		break;
	case FUNCTIONBEGIN:
		// functionname rd
		output << "FUNCTIONBEGIN " << code.rd;
		middle_before << code.rs << " " << code.rd << "()" << endl;
		break;
	case FUNCTIONEND:
		output << "FUNCTIONEND";
		middle_before << endl;
		break;
	case RET:
		output << "RET";
		middle_before << "RET" << endl;
		break;
	case PARADEF:
		output << "PARADEF";
		isPara = true;
		break;
	case PARADEFEND:
		output << "PARADEFEND";
		isPara = false;
		break;
	case END:
		output << "END";
		break;
	case READ:
		// read rd type rs
		output << "READ " << code.rd << " " << code.rs;
		middle_before << "SCANF " << code.rs << " " << code.rd << endl;
		break;
	case PRINT:
		// print rd type rs
		output << "PRINT " << code.rd << " " << code.rs;
		middle_before << "PRINTF " << code.rs << " " << code.rd << endl;
		break;
	case STR:
		// label: rd, value: rs
		output << "STR " << code.rd << " " << code.rs;
		middle_before << code.rd << " : \"" << code.rs << "\"" << endl;
		break;
	case ADDINDENT:
		output << "ADD_INDENT";
		break;
	case REDUCEINDENT:
		output << "REDUCE_INDENT";
		break;
	default:
		break;
	}
	return output;
}

string temp_var_generate() {
	if (temp_var.empty()) {
		string re = "$t" + to_string(temp_counter);
		++temp_counter;
		return re;
	}
	else {
		string re = temp_var.front();
		temp_var.pop();
		return re;
	}
}

void free_temp_var(string name) {
	if (name.at(0) == '$' && name.at(1) == 't') {
		temp_var.push(name);
	}
}

string para_generate() {
	++para_num;
	return "$a" + to_string(para_num);
}

void para_init() {
	para_num = -1;
}

string label_generate(string tag) {
	++labelCount;
	return "__" + tag + "__label__" + to_string(labelCount);
}

string string_name_generate() {
	++stringCount;
	return "@str_" + to_string(stringCount);
}

string string_label_generate() {
	++stringLabelCount;
	return "__str__" + to_string(stringLabelCount);
}

string var_label_generate() {
	++varLabelCount;
	return "__var__" + to_string(varLabelCount);
}

string func_label_generate() {
	++funcLabelCount;
	return "__func__" + to_string(funcLabelCount);
}