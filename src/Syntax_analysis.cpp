#include "main.h"
#include "Lexical_analysis.h"
#include "declare.h"
#include "statement.h"
#include "tag.h"
#include "function.h"
#include "expression.h"
#include "Symbols.h"
#include <string.h>
#include <map>
#include <vector>
#include "Syntax_analysis.h"

int Syntax_analysis() {
	errorFlag = false;
	errorFile.open("error.txt");
	lexical_analysis();
	if (type == CONSTTK) {
		const_declare();
	}
	if ((type == INTTK) || (type == CHARTK)) {
		value_type = (type == INTTK) ? INT : CHAR;
		function_type = value_type;
		lexical_analysis();
		if (type != IDENFR)
			error();
		name = token;

		// output for var_declare and function_definition !!!
		//////////////////////////
		out.output();
		//////////////////////////

		lexical_analysis();
		if (type != LPARENT) {
			var_declare();
		}
		isVarFinish = true;
		if (type == LPARENT) {
			function_definition();
		}
		else if (not isCatchMain) {
			error();
		}
	}
	else if (type == VOIDTK) {
		isVarFinish = true;
		function_type = NON;
		lexical_analysis();
		if (type == IDENFR) {
			name = token;
			out.output();
			lexical_analysis();
			if (type != LPARENT) {
				error();
			}
			function_definition();
		}
	}
	else {
		error();
	}
	if (type == MAINTK) {
		name = "main";
		isCatchMain = true;
		lexical_analysis();
		if (type != LPARENT) {
			error();
		}
		lexical_analysis();
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
		//////////// output for statements!
		out.output();
		lexical_analysis();

		Symbols *mainSymbols = new Symbols(symbols);
		symbols = mainSymbols;
		function_type = NON;
		codes.push_back(Intermediate_code(FUNCTIONBEGIN, "main", "void", ""));
		compound_statement();
		if (type != RBRACE) {
			error();
		}
		symbols = symbols->findPrev();
		//mainSymbols->output();
		delete mainSymbols;
		out.output();
		out.print("<主函数>\n");
		codes.push_back(Intermediate_code(FUNCTIONEND, "", "", ""));
		lexical_analysis();
	}
	else {
		error();
	}
	if (type != EMPTY) {
		error();
	}
	out.print("<程序>\n");
	errorFile.close();
	if (errorFlag) {
		return -1;
	}

	  ///////////////////////////////////////
	 //          Code Generate            //
	///////////////////////////////////////

	return 0;
}

void error() {
	errorFlag = true;
	//cout << line << " " << token << "  error!" << endl;
	// while (true) {}
	// errorMessage('f', line);
}

void errorMessage(char type, int lineCount) {
	errorFlag = true;
	errorFile << lineCount << " " << type << endl;
}

