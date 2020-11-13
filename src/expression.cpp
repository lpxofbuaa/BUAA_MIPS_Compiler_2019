#include "expression.h"
#include "function.h"
#include "Lexical_analysis.h"
#include "Intermediate_code.h"
#include <sstream>

using namespace std;

bool expression(string & expr) {
	// value_type = INT;
	bool isFirst = true;
	bool getMinu = false;
	if ((type == PLUS) || (type == MINU)) {
		isFirst = false;
		if (type == MINU) {
			getMinu = true;
		}
		out.output();
		lexical_analysis();
	}
	string term_1;
	bool re = term(isFirst, term_1);
	if (not re) {
		return re;
	}
	if (getMinu) {
		string tmp_term = temp_var_generate();
		codes.push_back(Intermediate_code(SUB, tmp_term, "$0", term_1));
		free_temp_var(term_1);
		term_1 = tmp_term;
	}
	string term_2;
	while ((type == PLUS) || (type == MINU)) {
		Type tmp_type = type;
		isFirst = false;
		out.output();
		lexical_analysis();
		re &= term(isFirst, term_2);
		string tmp_term = temp_var_generate();
		if (tmp_type == PLUS) {
			stringstream check_1(term_1);
			stringstream check_2(term_2);
			int term_int_1;
			int term_int_2;
			if ((check_1 >> term_int_1) && (check_2 >> term_int_2)) {
				codes.push_back(Intermediate_code(ADD, tmp_term, "$0", to_string(term_int_1 + term_int_2)));
			}
			else {
				codes.push_back(Intermediate_code(ADD, tmp_term, term_1, term_2));
			}
		}
		else {
			stringstream check_1(term_1);
			stringstream check_2(term_2);
			int term_int_1;
			int term_int_2;
			if (check_1 >> term_int_1) {
				if (check_2 >> term_int_2) {
					codes.push_back(Intermediate_code(ADD, tmp_term, to_string(term_int_1 - term_int_2), "$0"));
				}
				else {
					codes.push_back(Intermediate_code(SUB, tmp_term, term_2, term_1));
					codes.push_back(Intermediate_code(SUB, tmp_term, "$0", tmp_term));
				}
			}
			else {
				codes.push_back(Intermediate_code(SUB, tmp_term, term_1, term_2));
			}
		}
		free_temp_var(term_1);
		free_temp_var(term_2);
		term_1 = tmp_term;
	}
	if (isFirst == false) {
		value_type = INT;
	}
	out.print("<表达式>\n");
	expr = term_1;
	return re;
}

bool term(bool & isFirst, string & t) {
	string factor_1;
	bool re = factor(isFirst, factor_1);
	if (not re) {
		return re;
	}
	string factor_2;
	while ((type == MULT) || (type == DIV)) {
		Type tmp_type = type;
		isFirst = false;
		out.output();
		lexical_analysis();
		re &= factor(isFirst, factor_2);
		string tmp_factor = temp_var_generate();
		if (tmp_type == MULT) {
			stringstream check_1(factor_1);
			stringstream check_2(factor_2);
			int factor_int_1;
			int factor_int_2;
			if ((check_1 >> factor_int_1) && (check_2 >> factor_int_2)) {
				codes.push_back(Intermediate_code(ADD, tmp_factor, "$0", to_string(factor_int_1 * factor_int_2)));
			}
			else {
				codes.push_back(Intermediate_code(MULTOP, tmp_factor, factor_1, factor_2));
			}
		}
		else {
			codes.push_back(Intermediate_code(DIVOP, tmp_factor, factor_1, factor_2));
		}
		free_temp_var(factor_1);
		free_temp_var(factor_2);
		factor_1 = tmp_factor;
	}
	out.print("<项>\n");
	t = factor_1;
	return re;
}

bool factor(bool & isFirst, string & f) {
	out.output();
	if (type == IDENFR) {
		string identify = token;
		Tag *find = symbols->getFromFathers(token);
		if (find == NULL) {
			// value_type = NON;
			errorMessage('c', identifyLine);
		}
		lexical_analysis();
		if (type == LBRACK) {
			out.output();
			lexical_analysis();
			int expressionLine = line;
			string tmp_expr;
			expression(tmp_expr);
			if (type != RBRACK) {
				// error();
				errorMessage('m', lastWordLine);
			}
			else {
				if (value_type == CHAR) {
					errorMessage('i', expressionLine);
				}
				out.output();
				lexical_analysis();
			}
			f = temp_var_generate();
			codes.push_back(Intermediate_code(ARRLOAD, f, identify, tmp_expr));
			free_temp_var(tmp_expr);
		}
		else if (type == LPARENT) {
			return_function_call(find);
			f = temp_var_generate();
			codes.push_back(Intermediate_code(MOVE, f, "$v0", ""));
		}
		else {
			if (find != NULL && find->isConst()) {
				if (find->valueType() == INT) {
					identify = to_string(find->getIntValue());
				} else if(find->valueType() == CHAR) {
					identify = to_string(int(find->getCharValue()));
				}
			}
			f = identify;
		}
		
		if (find != NULL) {
			value_type = find->valueType();
		}
		else {
			value_type = NON;
		}
	}
	else if (type == LPARENT) {
		isFirst = false;
		lexical_analysis();
		expression(f);
		if (type != RPARENT) {
			// error();
			errorMessage('l', lastWordLine);
		}
		else {
			out.output();
			lexical_analysis();
		}
	}
	else if (type == INTCON) {
		f = token;
		value_type = INT;
		out.print("<整数>\n");
		lexical_analysis();
	}
	else if ((type == PLUS) || (type == MINU)) {
		Type tmp_type = type;
		value_type = INT;
		lexical_analysis();
		if (type != INTCON) {
			error();
		}
		if (tmp_type == MINU) {
			f = "-" + token;
		}
		else {
			f = token;
		}
		out.output();
		out.print("<整数>\n");
		lexical_analysis();
	}
	else if (type == CHARCON) {
		value_type = CHAR;
		f = to_string(int(token.at(0)));
		lexical_analysis();
	}
	else {
		value_type = NON;
		// error();
		return false;
	}
	out.print("<因子>\n");
	return true;
}