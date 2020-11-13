#pragma once

#include <string>
#include "tag.h"
#include "main.h"
#include "Lexical_analysis.h"

void compound_statement();	// has token, return with token
void statements_collection();	// has token, return with token
bool statements();		// has token, return with token
void assign_statement(Tag *tag);	// has token = "="|"[" , return with token
bool scanf_statement();		// token = "scanf", return token = ")"
bool printf_statement();	// token = "printf", return token = ")"
void return_statement();	// token = "return" , return with token
void if_statement();	// token = "if", return with token
void for_statement();	// token = "for", return with token
void while_statement();	// token = "while", return with token
bool do_while_statement(); // token = "do", return token = ")"

Intermediate_code condition(bool correct);	// no token, return with token	