#include "function.h"
#include "expression.h"
#include "Lexical_analysis.h"
#include "Intermediate_code.h"

void return_function_call(Tag *tag) {
	if (type != LPARENT) {
		error();
	}
	out.output();
	lexical_analysis();
	value_parameter_list(tag);
	if (type != RPARENT) {
		// error();
		errorMessage('l', lastWordLine);
	}
	else {
		out.output();
		lexical_analysis();
	}
	if (tag != NULL) {
		codes.push_back(Intermediate_code(CALL, "", tag->getName(), ""));
	}
	out.print("<有返回值函数调用语句>\n");
}

void non_return_function_call(Tag *tag) {
	if (type != LPARENT) {
		error();
	}
	out.output();
	lexical_analysis();
	value_parameter_list(tag);
	if (type != RPARENT) {
		// error();
		errorMessage('l', lastWordLine);
	}
	else {
		out.output();
		lexical_analysis();
	}
	out.print("<无返回值函数调用语句>\n");
	if (tag != NULL) {
		codes.push_back(Intermediate_code(CALL, "", tag->getName(), ""));
	}
}

void value_parameter_list(Tag *tag) {
	int lines = identifyLine;
	vector<Value_type> *paralist = new vector<Value_type>();
	vector<string> paraTemp;
	// value_type = INT;
	if (type == RPARENT) {
		out.print("<值参数表>\n");
		if (tag != NULL && tag->tagType() == FUNCTION) {
			if (tag->getParameters().size() != 0) {
				errorMessage('d',lines);
			}
		}
		codes.push_back(Intermediate_code(PRECALL, "", "", ""));
		codes.push_back(Intermediate_code(PRECALLEND, "", "", ""));
		return;
	}
	string expr;
	string temp_para;
	if (not expression(expr)) {
		codes.push_back(Intermediate_code(PRECALL, "", "", ""));
		codes.push_back(Intermediate_code(PRECALLEND, "", "", ""));
		return;
	}
	// temp_para = temp_var_generate();
	// codes.push_back(Intermediate_code(MOVE, temp_para, expr, ""));
	paraTemp.push_back(expr);
	// free_temp_var(expr);
	paralist->push_back(value_type);
	while (type == COMMA) {
		out.output();
		lexical_analysis();
		expression(expr);
		// temp_para = temp_var_generate();
		// codes.push_back(Intermediate_code(MOVE, temp_para, expr, ""));
		paraTemp.push_back(expr);
		// free_temp_var(expr);
		paralist->push_back(value_type);
	}
	if (tag != NULL && tag->tagType() == FUNCTION) {
		vector<Value_type> & tagPara = tag->getParameters();
		if (tagPara.size() != paralist->size()) {
			errorMessage('d',lines);
		}
		else {
			for (int i = 0; i < paralist->size(); i++) {
				if (tagPara[i] != (*paralist)[i] && (*paralist)[i] != NON) {
					errorMessage('e',lines);
					break;
				}
			}
		}
	}
	para_init();
	codes.push_back(Intermediate_code(PRECALL, "", "", ""));
	for (int i = 0; i < paraTemp.size(); ++i) {
		codes.push_back(Intermediate_code(MOVE, para_generate(), paraTemp[i], ""));
		free_temp_var(paraTemp[i]);
	}
	out.print("<值参数表>\n");
	codes.push_back(Intermediate_code(PRECALLEND, "", "", ""));
	delete paralist;
}