#include "Type.h"

string type2string(Type t) {
	switch (t) {
	case IDENFR:
		return "IDENFR";
		break;
	case INTCON:
		return "INTCON";
		break;
	case CHARCON:
		return "CHARCON";
		break;
	case STRCON:
		return "STRCON";
		break;
	case CONSTTK:
		return "CONSTTK";
		break;
	case INTTK:
		return "INTTK";
		break;
	case CHARTK:
		return "CHARTK";
		break;
	case VOIDTK:
		return "VOIDTK";
		break;
	case MAINTK:
		return "MAINTK";
		break;
	case IFTK:
		return "IFTK";
		break;
	case ELSETK:
		return "ELSETK";
		break;
	case DOTK:
		return "DOTK";
		break;
	case WHILETK:
		return "WHILETK";
		break;
	case FORTK:
		return "FORTK";
		break;
	case SCANFTK:
		return "SCANFTK";
		break;
	case PRINTFTK:
		return "PRINTFTK";
		break;
	case RETURNTK:
		return "RETURNTK";
		break;
	case PLUS:
		return "PLUS";
		break;
	case MINU:
		return "MINU";
		break;
	case MULT:
		return "MULT";
		break;
	case DIV:
		return "DIV";
		break;
	case LSS:
		return "LSS";
		break;
	case LEQ:
		return "LEQ";
		break;
	case GRE:
		return "GRE";
		break;
	case GEQ:
		return "GEQ";
		break;
	case EQL:
		return "EQL";
		break;
	case NEQ:
		return "NEQ";
		break;
	case ASSIGN:
		return "ASSIGN";
		break;
	case SEMICN:
		return "SEMICN";
		break;
	case COMMA:
		return "COMMA";
		break;
	case LPARENT:
		return "LPARENT";
		break;
	case RPARENT:
		return "RPARENT";
		break;
	case LBRACK:
		return "LBRACK";
		break;
	case RBRACK:
		return "RBRACK";
		break;
	case LBRACE:
		return "LBRACE";
		break;
	case RBRACE:
		return "RBRACE";
		break;
	default:
		return "ERROR:";
		break;
	}
}