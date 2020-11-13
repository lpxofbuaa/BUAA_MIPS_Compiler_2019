#include "statement.h"
#include "declare.h"
#include "function.h"
#include "expression.h"
#include <sstream>
#include <iostream>

extern vector<Intermediate_code> str;

void compound_statement() {
	if (type == CONSTTK) {
		const_declare();
	}
	if ((type == INTTK) || (type == CHARTK)) {
		value_type = (type == INTTK) ? INT : CHAR;
		lexical_analysis();
		if (type != IDENFR) {
			error();
		}
		name = token;

		// output for var_declare!!!
		//////////////
		out.output();
		//////////////
		lexical_analysis();
		var_declare();
	}
	statements_collection();
	out.print("<复合语句>\n");
}

void statements_collection() {
	while (statements()) {}
	out.print("<语句列>\n");
}

bool statements() {
	bool re;
	Tag *find;
	switch (type)
	{
	case IFTK:
		out.output();
		if_statement();
		re = true;
		break;
	case FORTK:
		out.output();
		for_statement();
		re = true;
		break;
	case WHILETK:
		out.output();
		while_statement();
		re = true;
		break;
	case DOTK:
		out.output();
		if (do_while_statement()) {
			lexical_analysis();
		}
		re = true;
		break;
	case LBRACE:
		out.output();
		lexical_analysis();
		statements_collection();
		if (type != RBRACE) {
			error();
		}
		out.output();
		lexical_analysis();
		re = true;
		break;
	case IDENFR:
		out.output();
		name = token;
		find = symbols->getFromFathers(name);
		if (find == NULL) {
			errorMessage('c', identifyLine);
		}
		lexical_analysis();
		if (type == ASSIGN) {
			assign_statement(find);
		}
		else if (type == LBRACK) {
			if (find != NULL && find->isConst()) {
				errorMessage('j', identifyLine);
			}
			assign_statement(find);
		}
		else if (type == LPARENT) {
			if (find == NULL) {
				return_function_call(find);
			} else if (find->isReturn()) {
				return_function_call(find);
			} else {
				non_return_function_call(find);
			}
		}
		if (type != SEMICN) {
			// error();
			errorMessage('k', lastWordLine);
		}
		else {
			out.output();
			lexical_analysis();
		}
		re = true;
		break;
	case SCANFTK:
		out.output();
		if (scanf_statement()) {
			lexical_analysis();
		}
		if (type != SEMICN) {
			// error();
			errorMessage('k', lastWordLine);
		}
		else {
			out.output();
			lexical_analysis();
		}
		re = true;
		break;
	case PRINTFTK:
		out.output();
		if (printf_statement()) {
			lexical_analysis();
		}
		if (type != SEMICN) {
			// error();
			errorMessage('k', lastWordLine);
		}
		else {
			out.output();
			lexical_analysis();
		}
		re = true;
		break;
	case RETURNTK:
		out.output();
		return_statement();
		if (type != SEMICN) {
			// error();
			errorMessage('k', lastWordLine);
		}
		else {
			out.output();
			lexical_analysis();
		}
		re = true;
		break;
	case SEMICN:
		out.output();
		lexical_analysis();
		re = true;
		break;
	default:
		re = false;
		break;
	}
	if (re) {
		out.print("<语句>\n");
	}
	return re;
}

bool scanf_statement() {
	lexical_analysis();
	if (type != LPARENT) {
		error();
	}
	lexical_analysis();
	while (true) {
		if (type != IDENFR) {
			error();
		}
		else {
			Tag *find = symbols->getFromFathers(token);
			if (find == NULL) {
				errorMessage('c', identifyLine);
			}
			else if (find->isConst()) {
				errorMessage('j', identifyLine);
			}
			else if (find->getDimension() != 0) {
				error();
			}
			else {
				codes.push_back(Intermediate_code(READ, find->getName(), to_string(find->valueType()), ""));
			}
		}
		lexical_analysis();
		if (type != COMMA) {
			break;
		}
		lexical_analysis();
	}
	if (type != RPARENT) {
		// error();
		errorMessage('l', lastWordLine);
		return false;
	} 
	out.output();
	out.print("<读语句>\n");
	return true;
}

bool printf_statement() {
	lexical_analysis();
	if (type != LPARENT) {
		error();
	}
	out.output();
	lexical_analysis();
	string expr;
	if (type == STRCON) {
		string string_name = string_name_generate();
		//codes.insert(codes.begin(), Intermediate_code(STR, string_name, token, ""));
		str.push_back(Intermediate_code(STR, string_name, token, ""));
		codes.push_back(Intermediate_code(PRINT, string_name, "string", ""));
		lexical_analysis();
		if (type == COMMA) {
			out.output();
			lexical_analysis();
			expression(expr);
			codes.push_back(Intermediate_code(PRINT, expr, to_string(value_type), ""));
			free_temp_var(expr);
		}
	}
	else {
		expression(expr);
		codes.push_back(Intermediate_code(PRINT, expr, to_string(value_type), ""));
		free_temp_var(expr);
	}
	codes.push_back(Intermediate_code(PRINT, "10", "char", ""));
	if (type != RPARENT) {
		// error();
		errorMessage('l', lastWordLine);
		return false;
	}
	out.output();
	out.print("<写语句>\n");
	return true;
}

Intermediate_code condition(bool correct) {
	if (correct) {
		lexical_analysis();
	}
	Value_type v1;
	Value_type v2;
	int expression1Line;
	int expression2Line;
	int middleLine;
	string expr_1;
	string expr_2;
	expression(expr_1);
	expression1Line = line;
	middleLine = line;
	v1 = value_type;
	Type operator_type = type;
	Intermediate_code branch;
	if ((type == LSS) || (type == LEQ) || (type == GRE) || (type == GEQ) || (type == EQL) || (type == NEQ)) {
		out.output();
		lexical_analysis();
		if (not expression(expr_2)) {
			error();
		}
		else {
			free_temp_var(expr_2);
		}
		expression2Line = line;
		v2 = value_type;
		if (v1 == CHAR || v2 == CHAR) {
			errorMessage('f', middleLine);
		}

		// intermediate code
		switch (operator_type)
		{
		case GEQ:
			branch = Intermediate_code(BLT, "", expr_1, expr_2);
			break;
		case GRE:
			branch = Intermediate_code(BLE, "", expr_1, expr_2);
			break;
		case LEQ:
			branch = Intermediate_code(BLT, "", expr_2, expr_1);
			break;
		case LSS:
			branch = Intermediate_code(BLE, "", expr_2, expr_1);
			break;
		case EQL:
			branch = Intermediate_code(BNE, "", expr_1, expr_2);
			break;
		case NEQ:
			branch = Intermediate_code(BEQ, "", expr_1, expr_2);
			break;
		default:
			break;
		}
	}
	else if (v1 == CHAR) {
		errorMessage('f', expression1Line);
	}
	else {
		// intermediate code
		branch = Intermediate_code(BEQ, "", expr_1, "0");
	}
	out.print("<条件>\n");
	free_temp_var(expr_1);
	return branch;
}

void if_statement() {
	lexical_analysis();
	if (type != LPARENT) {
		error();
	}
	out.output();
	Intermediate_code branch = condition(true);
	if (type != RPARENT) {
		// error();
		errorMessage('l', lastWordLine);
	}
	else {
		out.output();
		lexical_analysis();
	}
	string label = label_generate("else");
	
	branch.rd = label;
	codes.push_back(branch);
	// codes.push_back(Intermediate_code(ADDINDENT, "", "", ""));
	if (not statements()) {
		error();
	}
	// codes.push_back(Intermediate_code(REDUCEINDENT, "", "", ""));
	
	
	if (type == ELSETK) {
		string label_end = label_generate("if_end");
		codes.push_back(Intermediate_code(BEQ, label_end, "0", "0"));
		codes.push_back(Intermediate_code(SETLABEL, "", label, ""));
		// codes.push_back(Intermediate_code(ADDINDENT, "", "", ""));
		lexical_analysis();
		if (not statements()) {
			error();
		}
		// codes.push_back(Intermediate_code(REDUCEINDENT, "", "", ""));
		codes.push_back(Intermediate_code(SETLABEL, "", label_end, ""));
	}
	else {
		codes.push_back(Intermediate_code(SETLABEL, "", label, ""));
	}
	
	out.print("<条件语句>\n");
}

void while_statement() {
	//string label_init = label_generate("while__init");
	//codes.push_back(Intermediate_code(SETLABEL, "", label_init, ""));
	codes.push_back(Intermediate_code(ADDINDENT, "", "", ""));
	lexical_analysis();
	if (type != LPARENT) {
		error();
	}
	out.output();
	int condition_begin = codes.size();
	Intermediate_code branch = condition(true);
	string label_continue = label_generate("while_continue");
	string label_end = label_generate("while_end");
	branch.rd = label_end;
	codes.push_back(branch);
	int condition_end = codes.size();
	codes.push_back(Intermediate_code(SETLABEL, "", label_continue, ""));
	if (type != RPARENT) {
		// error();
		errorMessage('l', lastWordLine);
	}
	else {
		out.output();
		lexical_analysis();
	}
	if (not statements()) {
		error();
	}
	for (int i = condition_begin; i != condition_end; ++i) {
		if (i == condition_end - 1) {
			branch = codes[i];
			string rs = branch.rs;
			string rt = branch.rt;
			switch (branch.operation)
			{
			case BLT:
				branch.operation = BLE;
				branch.rs = rt;
				branch.rt = rs;
				break;
			case BLE:
				branch.operation = BLT;
				branch.rs = rt;
				branch.rt = rs;
				break;
			case BEQ:
				branch.operation = BNE;
				break;
			case BNE:
				branch.operation = BEQ;
				break;
			default:
				break;
			}
			branch.rd = label_continue;
			codes.push_back(branch);
		}
		else {
			codes.push_back(codes[i]);
		}
	}
	//codes.push_back(Intermediate_code(BEQ, label_init, "0", "0"));
	codes.push_back(Intermediate_code(REDUCEINDENT, "", "", ""));
	codes.push_back(Intermediate_code(SETLABEL, "", label_end, ""));
	out.print("<循环语句>\n");
}

bool do_while_statement() {
	out.output();
	string label_init = label_generate("do_while_init");
	codes.push_back(Intermediate_code(SETLABEL, "", label_init, ""));
	codes.push_back(Intermediate_code(ADDINDENT, "", "", ""));
	lexical_analysis();
	if (not statements()) {
		error();
	}
	if (type != WHILETK) {
		// error();
		errorMessage('n', line);
		// return false;
	}
	else {
		lexical_analysis();
	}
	if (type != LPARENT) {
		error();
	}
	out.output();
	Intermediate_code branch = condition(true);
	branch.rd = label_init;
	switch (branch.operation)
	{
	case BEQ:
		branch.operation = BNE;
		break;
	case BNE:
		branch.operation = BEQ;
		break;
	case BLE:
		branch = Intermediate_code(BLT, branch.rd, branch.rt, branch.rs);
		break;
	case BLT:
		branch = Intermediate_code(BLE, branch.rd, branch.rt, branch.rs);
		break;
	default:
		break;
	}
	codes.push_back(branch);
	codes.push_back(Intermediate_code(REDUCEINDENT, "", "", ""));
	if (type != RPARENT) {
		// error();
		errorMessage('l', lastWordLine);
		return false;
	}
	out.output();
	out.print("<循环语句>\n");
	return true;
}

void for_statement() {
	string identify;
	string expr;
	string init_identify;
	Tag *find;
	lexical_analysis();
	if (type != LPARENT) {
		error();
	}
	lexical_analysis();
	if (type != IDENFR) {
		error();
	}
	else {
		Tag *find = symbols->getFromFathers(token);
		if (find == NULL) {
			errorMessage('c', identifyLine);
		}
		else if (find->isConst()) {
			errorMessage('j', identifyLine);
		}
		init_identify = token;
	}
	lexical_analysis();
	if (type != ASSIGN) {
		error();
	}
	out.output();
	lexical_analysis();
	expression(expr);
	codes.push_back(Intermediate_code(MOVE, init_identify, expr, ""));
	free_temp_var(expr);
	//string label_compare = label_generate("for_compare");
	//codes.push_back(Intermediate_code(SETLABEL, "", label_compare, ""));
	codes.push_back(Intermediate_code(ADDINDENT, "", "", ""));
	Intermediate_code branch;
	int condition_first = codes.size();
	if (type != SEMICN) {
		// error();
		errorMessage('k', lastWordLine);
		branch = condition(false);
	}
	else {
		out.output();
		branch = condition(true);
	}
	string label_end = label_generate("for_end");
	string label_continue = label_generate("for_continue");
	branch.rd = label_end;
	codes.push_back(Intermediate_code(branch));
	int condition_end = codes.size();
	codes.push_back(Intermediate_code(SETLABEL, "", label_continue, ""));
	if (type != SEMICN) {
		// error();
		errorMessage('k', lastWordLine);
	}
	else {
		lexical_analysis();
	}
	string change_identify_1;
	string change_identify_2;
	if (type != IDENFR) {
		error();
	}
	else {
		Tag *find = symbols->getFromFathers(token);
		if (find == NULL) {
			errorMessage('c', identifyLine);
		}
		else if (find->isConst()) {
			errorMessage('j', identifyLine);
		}
		change_identify_1 = token;
	}
	lexical_analysis();
	if (type != ASSIGN) {
		error();
	}
	lexical_analysis();
	find = symbols->getFromFathers(token);
	identify = token;
	if (type != IDENFR) {
		error();
	}
	else if (find == NULL) {
		errorMessage('c', identifyLine);
	}
	else if (find->isConst()) {
		if (find->valueType() == INT) {
			identify = to_string(find->getIntValue());
		}
		else if (find->valueType() == CHAR) {
			identify = to_string(int(find->getCharValue()));
		}
		else {
			error();
		}
	}
	change_identify_2 = identify;
	string num = "";
	lexical_analysis();
	if ((type != PLUS) && (type != MINU)) {
		error();
	}
	num += token;
	out.output();
	lexical_analysis();
	if (type != INTCON) {
		error();
	}
	num += token;
	// output
	out.output();
	out.print("<步长>\n");
	lexical_analysis();
	if (type != RPARENT) {
		// error();
		errorMessage('l', lastWordLine);
	}
	else {
		out.output();
		lexical_analysis();
	}
	if (not statements()) {
		error();
	}
	codes.push_back(Intermediate_code(ADD, change_identify_1, change_identify_2, num));
	for (int i = condition_first; i != condition_end; ++i) {
		if (i == condition_end - 1) {
			branch = codes[i];
			string rs = branch.rs;
			string rt = branch.rt;
			switch (branch.operation)
			{
			case BLT:
				branch.operation = BLE;
				branch.rs = rt;
				branch.rt = rs;
				break;
			case BLE:
				branch.operation = BLT;
				branch.rs = rt;
				branch.rt = rs;
				break;
			case BEQ:
				branch.operation = BNE;
				break;
			case BNE:
				branch.operation = BEQ;
				break;
			default:
				break;	
			}
			branch.rd = label_continue;
			codes.push_back(branch);
		}
		else {
			codes.push_back(codes[i]);
		}
	}
	//codes.push_back(Intermediate_code(BEQ, label_compare, "0", "0"));
	codes.push_back(Intermediate_code(REDUCEINDENT, "", "", ""));
	codes.push_back(Intermediate_code(SETLABEL, "", label_end, ""));
	out.print("<循环语句>\n");
}

void return_statement() {
	isCatchReturn = true;
	int returnLine = line;
	out.output();
	lexical_analysis();
	string expr;
	if (type == LPARENT) {
		out.output();
		lexical_analysis();
		expression(expr);
		vector<Intermediate_code>::iterator tail = codes.end() - 1;
		if ((*tail).rd == expr && expr.at(0) == '$') {
			(*tail).rd = "$v0";
		}
		else {
			codes.push_back(Intermediate_code(MOVE, "$v0", expr, ""));
		}
		if (isCatchMain) {
			codes.push_back(Intermediate_code(END, "", "", ""));
		}
		else {
			codes.push_back(Intermediate_code(RET, "", "", ""));
		}
		free_temp_var(expr);
		if (function_type == NON) {
			errorMessage('g', returnLine);
		}
		else {
			if (value_type != NON && value_type != function_type) {
				errorMessage('h', returnLine);
			}
		}
		if (type != RPARENT) {
			// error();
			errorMessage('l', lastWordLine);
		}
		else {
			out.output();
			lexical_analysis();
		}
	}
	else if (function_type != NON) {
		errorMessage('h', returnLine);
	}
	else {
		if (isCatchMain) {
			codes.push_back(Intermediate_code(END, "", "", ""));
		}
		else {
			codes.push_back(Intermediate_code(RET, "", "", ""));
		}
	}
	out.print("<返回语句>\n");
}

void assign_statement(Tag *tag) {
	bool isArr = false;
	string exprValue;
	string exprIndex;
	if (tag != NULL && tag->isConst()) {
		errorMessage('j', identifyLine);
	}
	out.output();
	if (type == LBRACK) {
		isArr = true;
		lexical_analysis();
		int expressionLine = line;
		expression(exprIndex);
		if (type != RBRACK) {
			// error();
			errorMessage('m', lastWordLine);
		}
		else {
			if (value_type == CHAR) {
				errorMessage('i', expressionLine);
			}
			lexical_analysis();
		}
	}
	if (type != ASSIGN) {
		error();
	}
	out.output();
	lexical_analysis();
	expression(exprValue);
	if (tag != NULL) {
		if (isArr) {
			codes.push_back(Intermediate_code(ARRSAVE, tag->getName(), exprIndex, exprValue));
			free_temp_var(exprIndex);
			free_temp_var(exprValue);
		}
		else {
			vector<Intermediate_code>::iterator tail = codes.end() - 1;
			if ((*tail).rd == exprValue && exprValue.at(0) == '$') {
				(*tail).rd = tag->getName();
			}
			else {
				codes.push_back(Intermediate_code(MOVE, tag->getName(), exprValue, ""));
			}
			free_temp_var(exprValue);
		}
	}
	out.print("<赋值语句>\n");
}