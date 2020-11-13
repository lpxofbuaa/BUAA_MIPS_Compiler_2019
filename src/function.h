#pragma once

#include "main.h"
#include <vector>

void return_function_call(Tag *tag);	// has token = "(", return with token 
void non_return_function_call(Tag *tag); // has token = "(", return with token
void value_parameter_list(Tag *tag); // has token ,return with token