#include "declare.h"
#include "statement.h"

void const_declare() {
	while (true) {
		const_definition();
		if (type != SEMICN) {
			// error();
			errorMessage('k', lastWordLine);
		}
		else {
			out.output();
			lexical_analysis();
		}
		if (type != CONSTTK) {
			break;
		}
	}
	out.print("<常量说明>\n");
}

void const_definition() {
	int lineCount;
	Value_type constType;
	string constName;
	lexical_analysis();
	if ((type != INTTK) && (type != CHARTK)) {
		error();
	}
	constType = (type == INTTK) ? INT : CHAR;
	lexical_analysis();
	while (true) {
		if (type != IDENFR) {
			error();
		}
		lineCount = line;
		constName = token;
		Tag t = Tag(constName, VARORCONST, constType);
		t.setConst(true);
		// name = token;
		lexical_analysis();
		if (type != ASSIGN) {
			error();
		}
		lexical_analysis();
		if (type == INTCON) {
			if (constType != INT) {
				// error();
				errorMessage('o', line);
			}
			out.output();
			out.print("<整数>\n");
			int intValue = parseInt(token);
			if (intValue < 0) {
				error();
			}
			t.setValue(intValue);
		}
		else if (type == CHARCON) {
			if (constType != CHAR) {
				// error();
				errorMessage('o', line);
			}
			t.setValue(token.at(0));
		}
		else if ((type == PLUS) || (type == MINU)) {
			Type tmp_type = type;
			lexical_analysis();
			if (type != INTCON) {
				// error();
				errorMessage('o', line);
			}
			else if (constType != INT) {
				errorMessage('o', line);
			}
			else {
				int intValue = parseInt(token);
				if (tmp_type == MINU) {
					if (intValue == -1) {
						t.setValue(int(-2147483648));
					}
					else if (intValue >= 0){
						t.setValue(int(-intValue));
					}
				} else if (intValue < 0) {
					error();
				}
				else {
					t.setValue(intValue);
				}
			}
			out.output();
			out.print("<整数>\n");
		}
		else {
			// error();
			errorMessage('o', line);
		}
		out.output();
		if (symbols->get(constName) == NULL) {
			symbols->put(constName, t);
		} else {
			// error();
			errorMessage('b', lineCount);
		}
		lexical_analysis();
		if (type != COMMA) {
			out.print("<常量定义>\n");
			break;
		}
		lexical_analysis();
	}
	return;
}

void var_declare() {
	codes.push_back(Intermediate_code(VARDEF, "", "", ""));
	while (true) {
		var_definition();
		if (type != SEMICN) {
			// error();
			errorMessage('k', lastWordLine);
		}
		else {
			out.output();

			//////////////////////////////////
			lexical_analysis();
		}
		if ((type == INTTK) || (type == CHARTK)) {
			value_type = (type == INTTK) ? INT : CHAR;
			lexical_analysis();
			if (type != IDENFR) {
				error();
			}
			name = token;
			lexical_analysis();
			if (type == LPARENT && not isVarFinish) {
				out.print("<变量说明>\n");
				out.output(2);
				function_type = value_type;
				codes.push_back(Intermediate_code(VARDEFEND, "", "", ""));
				return;
			}
			else if (type == SEMICN) {
				out.output(2);
			}
		}
		else {
			if (not isVarFinish) {
				out.print("<变量说明>\n");
				if (type == VOIDTK) {
					function_type = NON;
					lexical_analysis();
					if (type == MAINTK) {
						isCatchMain = true;
						out.output();
						codes.push_back(Intermediate_code(VARDEFEND, "", "", ""));
						return;
					}
					else if (type != IDENFR) {
						error();
					}
					name = token;
					out.output();
					lexical_analysis();
					codes.push_back(Intermediate_code(VARDEFEND, "", "", ""));
					return;
				}
				else {
					error();
					break;
				}
			}
			else {
				break;
			}
		}
	}
	codes.push_back(Intermediate_code(VARDEFEND, "", "", ""));
	out.print("<变量说明>\n");
}

void var_definition() {
	int dimension = 0;
	while (true) {
		dimension = 0;
		if (type == LBRACK) {
			lexical_analysis();
			if (type != INTCON) {
				// error();
				errorMessage('i', line);
			}
			else {
				dimension = parseInt(token);
				if (dimension < 0) {
					error();
					dimension = 0;
				}
			}
			lexical_analysis();
			if (type != RBRACK) {
				// error();
				errorMessage('m', lastWordLine);
			}
			else {
				out.output();
				lexical_analysis();
			}
		}
		Tag var = Tag(name, VARORCONST, value_type);
		var.setConst(false);
		var.setDimension(dimension);
		codes.push_back(Intermediate_code(VAR, var.getName(), to_string(var.getDimension()), to_string(var.valueType())));
		if (symbols->get(name) == NULL) {
			symbols->put(name, var);
		} else {
			// error();
			errorMessage('b', identifyLine);
		}
		if (type != COMMA)
			break;
		lexical_analysis();
		if (type != IDENFR) {
			error();
		}
		name = token;
		out.output();
		lexical_analysis();
	}
	out.print("<变量定义>\n");
}

void function_definition() {
	Value_type function = function_type;
	int functionLine;
	int functionEndLine;
	
	while (true) {
		string str_function_type;
		if (function_type == INT) {
			str_function_type = "int";
		}
		else if (function_type == NON) {
			str_function_type = "void";
		}
		else if (function_type == CHAR) {
			str_function_type = "char";
		}
		codes.push_back(Intermediate_code(FUNCTIONBEGIN, name, str_function_type, ""));
		functionLine = identifyLine;
		function = function_type;
		returnType = NON;
		isCatchReturn = false;
		Tag func;
		if (function != NON) {
			out.print("<声明头部>\n");
		}
		if (function == NON) {
			func = Tag(name, FUNCTION, function);
			func.setReturn(false);
		}
		else {
			func = Tag(name, FUNCTION, function);
			func.setReturn(true);
		}
		Symbols *functionSym = new Symbols(symbols);
		parameter_list(func, functionSym);

		if (symbols->get(name) == NULL) {
			symbols->put(name, func);
		}
		else {
			// error();
			errorMessage('b', functionLine);
		}

		if (type != RPARENT) {
			// error();
			errorMessage('l', lastWordLine);
		}
		else {
			lexical_analysis();
		}
		if (type != LBRACE) {
			error();
		}

		/////////// output for statements
		out.output();
		////////////////////

		lexical_analysis();
		// statements
		symbols = functionSym;
		compound_statement();
		if (type != RBRACE) {
			error();
		}
		else {
			functionEndLine = line;
			if (function_type != NON && not isCatchReturn) {
				errorMessage('h', functionEndLine);
			}
		}
		out.output();

		if (function != NON) {
			out.print("<有返回值函数定义>\n");
		}
		else {
			out.print("<无返回值函数定义>\n");
		}
		codes.push_back(Intermediate_code(FUNCTIONEND, "", "", ""));

		symbols = symbols->findPrev();

		//functionSym->output();
		delete functionSym;

		//////////////////////////////////////
		lexical_analysis();
		if (type == INTTK) {
			function_type = INT;
			lexical_analysis();
			if (type != IDENFR)
				error();
		}
		else if (type == CHARTK) {
			function_type = CHAR;
			lexical_analysis();
			if (type != IDENFR)
				error();
		}
		else if (type == VOIDTK) {
			function_type = NON;
			lexical_analysis();
			if (type != IDENFR)
				break;
		}
		else {
			error();
			break;
		}
		
		name = token;
		///////////// output for 声明头部
		out.output();
		lexical_analysis();
		if (type != LPARENT) {
			error();
		}
	}
}

void parameter_list(Tag & functionTag, Symbols *nextSymbols) {
	Value_type t;
	Tag paraTag;
	out.output();
	lexical_analysis();
	if (type == RPARENT) {
		out.print("<参数表>\n");
		return;
	}
	else if ((type != INTTK) && (type != CHARTK)) {
		return;
	}
	codes.push_back(Intermediate_code(PARADEF, "", "", ""));
	while (true) {
		if ((type != INTTK) && (type != CHARTK)) {
			error();
		} else {
			t = (type == INTTK) ? INT : CHAR;
		}
		lexical_analysis();
		if (type != IDENFR) {
			error();
		} else {
			paraTag = Tag(token, VARORCONST, t);
			paraTag.setConst(false);
			if (nextSymbols->get(token) == NULL) {
				nextSymbols->put(token, paraTag);
				functionTag.pushParameter(t);
				codes.push_back(Intermediate_code(VAR, paraTag.getName(), "0", to_string(paraTag.valueType())));
			} else {
				// error();
				errorMessage('b',identifyLine);
			}
		}
		out.output();
		lexical_analysis();
		if (type == COMMA) {
			lexical_analysis();
		}
		else {
			break;
		}
	}
	codes.push_back(Intermediate_code(PARADEFEND, "", "", ""));
	out.print("<参数表>\n");
}

