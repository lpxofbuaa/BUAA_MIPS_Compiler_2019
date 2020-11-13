#pragma once

#include <string>
#include "main.h"
#include "Lexical_analysis.h"
#include "Symbols.h"
#include "tag.h"
using namespace std;


void const_declare();		// has const, no token, return with token
void const_definition();	// non token, return with token


void var_declare();			// has type, name and token; if declare, return with "void name (" or "int name (" or "void main", else return with token
void var_definition();		// while begin has type, name and token, return with token

void parameter_list(Tag & func, Symbols *nextSymbols);		// token = "(", may return with ")"

void function_definition();		// token = "(", return with main, or error