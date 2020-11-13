#include <iostream>
#include "main.h"
#include "canbe.h"
#include "is.h"
#include "Type.h"
#include "Lexical_analysis.h"
using namespace std;

int lexical_analysis()
{
	while (true) {
		token.clear();
		lastWordLine = line;
		pick = skipSpace(pick);
		if (pick == 'c') {
			token += pick;
			pick = in.getc();
			if (pick == 'o') {
				token += pick;
				if (canbeConst()) {
					type = CONSTTK;
					out.push(type, token);
					return  0;
				}
			}
			else if (pick == 'h') {
				token += pick;
				if (canbeChar()) {
					type = CHARTK;
					out.push(type, token);
					return  0;
				}
			}
		}
		else if (pick == 'i') {
			token += pick;
			pick = in.getc();
			if (pick == 'f') {
				token += pick;
				if (canbeIf()) {
					type = IFTK;
					out.push(type, token);
					return  0;
				}
			}
			else if (pick == 'n') {
				token += pick;
				if (canbeInt()) {
					type = INTTK;
					out.push(type, token);
					return  0;
				}
			}
		}
		else if (pick == 'v') {
			token += pick;
			if (canbeVoid()) {
				type = VOIDTK;
				out.push(type, token);
				return  0;
			}
		}
		else if (pick == 'm') {
			token += pick;
			if (canbeMain()) {
				type = MAINTK;
				out.push(type, token);
				return  0;
			}
		}
		else if (pick == 'e') {
			token += pick;
			if (canbeElse()) {
				type = ELSETK;
				out.push(type, token);
				return  0;
			}
		}
		else if (pick == 'd') {
			token += pick;
			if (canbeDo()) {
				type = DOTK;
				out.push(type, token);
				return  0;
			}
		}
		else if (pick == 'w') {
			token += pick;
			if (canbeWhile()) {
				type = WHILETK;
				out.push(type, token);
				return  0;
			}
		}
		else if (pick == 'f') {
			token += pick;
			if (canbeFor()) {
				type = FORTK;
				out.push(type, token);
				return  0;
			}
		}
		else if (pick == 's') {
			token += pick;
			if (canbeScanf()) {
				type = SCANFTK;
				out.push(type, token);
				return  0;
			}
		}
		else if (pick == 'p') {
			token += pick;
			if (canbePrintf()) {
				type = PRINTFTK;
				out.push(type, token);
				return  0;
			}
		}
		else if (pick == 'r') {
			token += pick;
			if (canbeReturn()) {
				type = RETURNTK;
				out.push(type, token);
				return  0;
			}
		}
		else if (isDigital(pick)) {
			token += pick;
			inputNum();
			type = INTCON;
			out.push(type, token);
			out.push("<无符号整数>\n");
			if (token.length() > 1 && token.at(0) == '0') {
				errorMessage('a', line);
			}
			return  0;
		}
		else if (pick == '+') {
			token += pick;
			pick = in.getc();
			type = PLUS;
			out.push(type, token);
			return  0;
		}
		else if (pick == '-') {
			token += pick;
			pick = in.getc();
			type = MINU;
			out.push(type, token);
			return  0;
		}
		else if (pick == '*') {
			token += pick;
			pick = in.getc();
			type = MULT;
			out.push(type, token);
			return  0;
		}
		else if (pick == '/') {
			token += pick;
			pick = in.getc();
			type = DIV;
			out.push(type, token);
			return  0;
		}
		else if (pick == '<') {
			token += pick;
			pick = in.getc();
			if (pick == '=') {
				token += pick;
				pick = in.getc();
				type = LEQ;
				out.push(type, token);
				return  0;
			}
			type = LSS;
			out.push(type, token);
			return  0;
		}
		else if (pick == '>') {
			token += pick;
			pick = in.getc();
			if (pick == '=') {
				token += pick;
				pick = in.getc();
				type = GEQ;
				out.push(type, token);
				return  0;
			}
			type = GRE;
			out.push(type, token);
			return  0;
		}
		else if (pick == '=') {
			token += pick;
			pick = in.getc();
			if (pick == '=') {
				token += pick;
				pick = in.getc();
				type = EQL;
				out.push(type, token);
				return  0;
			}
			type = ASSIGN;
			out.push(type, token);
			return  0;
		}
		else if (pick == '!') {
			token += pick;
			pick = in.getc();
			if (pick == '=') {
				token += pick;
				pick = in.getc();
				type = NEQ;
				out.push(type, token);
				return  0;
			}
			type = ERROR;
			return  -1;
		}
		else if (pick == ';') {
			token += pick;
			pick = in.getc();
			type = SEMICN;
			out.push(type, token);
			return  0;
		}
		else if (pick == ',') {
			token += pick;
			pick = in.getc();
			type = COMMA;
			out.push(type, token);
			return  0;
		}
		else if (pick == '(') {
			token += pick;
			pick = in.getc();
			type = LPARENT;
			out.push(type, token);
			return  0;
		}
		else if (pick == ')') {
			token += pick;
			pick = in.getc();
			type = RPARENT;
			out.push(type, token);
			return  0;
		}
		else if (pick == '[') {
			token += pick;
			pick = in.getc();
			type = LBRACK;
			out.push(type, token);
			return  0;
		}
		else if (pick == ']') {
			token += pick;
			pick = in.getc();
			type = RBRACK;
			out.push(type, token);
			return  0;
		}
		else if (pick == '{') {
			token += pick;
			pick = in.getc();
			type = LBRACE;
			out.push(type, token);
			return  0;
		}
		else if (pick == '}') {
			token += pick;
			pick = in.getc();
			type = RBRACE;
			out.push(type, token);
			return  0;
		}
		else if (pick == '\'') {
			pick = in.getc();
			while ((pick != '\'') && (pick != '\0')) {
				if (pick != '+' && pick != '-' && pick != '*' && pick != '/' && not isAlpha(pick) && not isDigital(pick)) {
					errorMessage('a', line);
				}
				token += pick;
				pick = in.getc();
			}
			pick = in.getc();
			type = CHARCON;
			out.push(type, token);
			if (token.length() > 1) {
				errorMessage('a', line);
			}
			return  0;
		}
		else if (pick == '"') {
			pick = in.getc();
			while ((pick != '"') && (pick != '\0')) {
				if (pick != 32 && pick != 33 && not((pick >= 35) && (pick <= 126))) {
					errorMessage('a', line);
				}
				if (pick == '\\') {
					token += pick;
				}
				token += pick;
				pick = in.getc();
			}
			pick = in.getc();
			type = STRCON;
			out.push(type, token);
			out.push("<字符串>\n");
			return  0;
		}
		if (isAlpha(pick) || isDigital(pick)) {
			token += pick;
			inputWord();
			type = IDENFR;
			identifyLine = line;
			out.push(type, token);
			return  0;
		}
		else if (not token.empty()) {
			type = IDENFR;
			identifyLine = line;
			out.push(type, token);
			return  0;
		}
		if (pick == EOF) {
			type = EMPTY;
			return  0;
		}
		errorMessage('a', line);
		pick = in.getc();
	}
}

int parseInt(string num) {
	int re = 0;
	if (num.length() > 10)
		return -1;
	for (int i = 0; i < num.length(); ++i) {
		re = re * 10 + int(num.at(i)) - 48;
	}
	return re;
}

