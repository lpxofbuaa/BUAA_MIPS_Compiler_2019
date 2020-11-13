#pragma once

#include "main.h"

bool expression(string & expr);	// has token, return with token
bool term(bool & isFirst, string & t);	// has token, return with token
bool factor(bool & isFirst, string & f);	// has token, return with token