#include "inline.h"

void var_reset() {
	inline_it = codes.begin();
	map<string, string> global_var_name_map;
	// global
	while ((*inline_it).operation != FUNCTIONBEGIN) {
		switch ((*inline_it).operation)
		{
		case VAR:
			global_var_name_map.insert({ 
				(*inline_it).rd,
				(*inline_it).rd 
			});
			//(*inline_it).rd += "@global";
			break;
		default:
			break;
		}
		++inline_it;
	}
	while (inline_it != codes.end()) {
		map<string, string> function_var_name_map;
		string function_name;
		map<string, string>::iterator find;
		while ((*inline_it).operation != FUNCTIONEND) {
			switch ((*inline_it).operation)
			{
			case FUNCTIONBEGIN:
				function_name = (*inline_it).rd;
				break;
			case VAR:
				function_var_name_map.insert({
					(*inline_it).rd,
					(*inline_it).rd + "@" + function_name
				});
				(*inline_it).rd += "@" + function_name;
				break;
			case ADD:
			case SUB:
			case MULTOP:
			case DIVOP:
			case MOVE:
			case ARRLOAD:
			case ARRSAVE:
			case READ:
			case PRINT:
				// rs
				if ((*inline_it).rs != "") {
					if ((*inline_it).rs.at(0) == '$' && (*inline_it).rs.at(1) == 't') {
						(*inline_it).rs += "@" + function_name;
					}
					else {
						find = function_var_name_map.find((*inline_it).rs);
						if (find != function_var_name_map.end()) {
							(*inline_it).rs = find->second;
						}
						else {
							find = global_var_name_map.find((*inline_it).rs);
							if (find != global_var_name_map.end()) {
								(*inline_it).rs = find->second;
							}
						}
					}
				}

				// rt
				if ((*inline_it).rt != "") {
					if ((*inline_it).rt.at(0) == '$' && (*inline_it).rt.at(1) == 't') {
						(*inline_it).rt += "@" + function_name;
					}
					else {
						find = function_var_name_map.find((*inline_it).rt);
						if (find != function_var_name_map.end()) {
							(*inline_it).rt = find->second;
						}
						else {
							find = global_var_name_map.find((*inline_it).rt);
							if (find != global_var_name_map.end()) {
								(*inline_it).rt = find->second;
							}
						}
					}
				}

				// rd
				if ((*inline_it).rd != "") {
					if ((*inline_it).rd.at(0) == '$' && (*inline_it).rd.at(1) == 't') {
						(*inline_it).rd += "@" + function_name;
					}
					else {
						find = function_var_name_map.find((*inline_it).rd);
						if (find != function_var_name_map.end()) {
							(*inline_it).rd = find->second;
						}
						else {
							find = global_var_name_map.find((*inline_it).rd);
							if (find != global_var_name_map.end()) {
								(*inline_it).rd = find->second;
							}
						}
					}
				}
				break;
			case BLE:
			case BEQ:
			case BNE:
			case BLT:
				// rs
				if ((*inline_it).rs != "") {
					if ((*inline_it).rs.at(0) == '$' && (*inline_it).rs.at(1) == 't') {
						(*inline_it).rs += "@" + function_name;
					}
					else {
						find = function_var_name_map.find((*inline_it).rs);
						if (find != function_var_name_map.end()) {
							(*inline_it).rs = find->second;
						}
						else {
							find = global_var_name_map.find((*inline_it).rs);
							if (find != global_var_name_map.end()) {
								(*inline_it).rs = find->second;
							}
						}
					}
				}

				//rt
				if ((*inline_it).rt != "") {
					if ((*inline_it).rt.at(0) == '$' && (*inline_it).rt.at(1) == 't') {
						(*inline_it).rt += "@" + function_name;
					}
					else {
						find = function_var_name_map.find((*inline_it).rt);
						if (find != function_var_name_map.end()) {
							(*inline_it).rt = find->second;
						}
						else {
							find = global_var_name_map.find((*inline_it).rt);
							if (find != global_var_name_map.end()) {
								(*inline_it).rt = find->second;
							}
						}
					}
				}
				break;
			default:
				break;
			}
			++inline_it;
		}
		++inline_it;
	}
}

string inline_label_reset(string label) {
	++inline_label_count;
	return label + "__inline__" + to_string(inline_label_count);
}


void function_inline() {
	vector<Intermediate_code> new_code;
	vector<Intermediate_code>::iterator var_def_end_insert_it;
	vector<Intermediate_code>::iterator function_inline_it;
	function_inline_it = codes.begin();

	// skip var def
	while ((*function_inline_it).operation != FUNCTIONBEGIN) {
		new_code.push_back(*function_inline_it);
		++function_inline_it;
	}

	while (function_inline_it != codes.end()) {
		bool can_inline = true;
		bool old_has_var = false;
		string function_name;
		Intermediate_code branch;
		string return_var;
		vector<Intermediate_code> this_function_context;
		vector<Intermediate_code> new_var_info;
		map<string, string> old_new_label_map;
		map<string, string>::iterator old_new_label_find;
		vector<Intermediate_code> para_list;
		vector<Intermediate_code>::iterator tmp;
		vector<Intermediate_code>::iterator tmp_2;
		vector<Intermediate_code>::iterator tmp_3;
		vector<Intermediate_code>::iterator tmp_4;
		vector<string> has_inlined_functions;
		map<string, function_inline_info>::iterator map_find;
		bool need_move_v0;
		string call_function_name;
		vector<Intermediate_code>::iterator function_begin_it = function_inline_it;
		// make inline
		while ((*function_inline_it).operation != FUNCTIONEND) {
			switch ((*function_inline_it).operation)
			{
			case VARDEF:
				old_has_var = true;
				this_function_context.push_back(*function_inline_it);
				break;
			case PRECALL:
				old_new_label_map.clear();
				para_list.clear();
				tmp = function_inline_it;
				while ((*function_inline_it).operation != CALL) {
					switch ((*function_inline_it).operation)
					{
					case MOVE:
						para_list.push_back(*function_inline_it);
						break;
					default:
						break;
					}
					++function_inline_it;
				}
				call_function_name = (*function_inline_it).rs;
				if (call_function_name == function_name) {
					can_inline = false;
				}
				++function_inline_it;
				if ((*function_inline_it).operation == MOVE && (*function_inline_it).rs == "$v0") {
					need_move_v0 = true;
					return_var = (*function_inline_it).rd;
				}
				else {
					need_move_v0 = false;
					--function_inline_it;
				}
				map_find = function_inline_map.find(call_function_name);
				if (map_find == function_inline_map.end()) {
					while (tmp != function_inline_it) {
						this_function_context.push_back(*tmp);
						++tmp;
					}
					this_function_context.push_back(*tmp);
					break;
				}
				  //////////////////
				 // make inline  //
				//////////////////

				// erase
				

				// move para
				tmp_4 = para_list.begin();
				for (tmp_3 = map_find->second.para.begin(); tmp_3 != map_find->second.para.end(); ++tmp_3) {
					this_function_context.push_back(Intermediate_code(MOVE, (*tmp_3).rd, (*tmp_4).rs, ""));
					++tmp_4;
				}

				// insert new var
				if (find(has_inlined_functions.begin(), has_inlined_functions.end(), call_function_name) == has_inlined_functions.end()) {
					// insert para
					for (tmp_3 = map_find->second.para.begin(); tmp_3 != map_find->second.para.end(); ++tmp_3) {
						new_var_info.push_back(*tmp_3);
					}

					// insert var
					for (tmp_3 = map_find->second.var.begin(); tmp_3 != map_find->second.var.end(); ++tmp_3) {
						new_var_info.push_back(*tmp_3);
					}
					has_inlined_functions.push_back(call_function_name);
				}

				// insert codes
				for (tmp_2 = map_find->second.inline_context.begin(); tmp_2 != map_find->second.inline_context.end(); ++tmp_2) {
					switch ((*tmp_2).operation)
					{
					case BLE:
					case BEQ:
					case BNE:
					case BLT:
						branch = (*tmp_2);
						old_new_label_find = old_new_label_map.find((*tmp_2).rd);
						if (old_new_label_find == old_new_label_map.end()) {
							branch.rd = inline_label_reset((*tmp_2).rd);
							old_new_label_map.insert({ (*tmp_2).rd, branch.rd });
						}
						else {
							branch.rd = old_new_label_find->second;
						}
						
						this_function_context.push_back(branch);
						break;
					case MOVE:
					case ADD:
					case SUB:
					case MULTOP:
					case DIVOP:
					case ARRLOAD:
						if ((*tmp_2).rd == "$v0") {
							if (need_move_v0) {
								branch = *tmp_2;
								branch.rd = return_var;
								this_function_context.push_back(branch);
							}
						}
						else {
							this_function_context.push_back(*tmp_2);
						}
						break;
					case SETLABEL:
						branch = *tmp_2;
						old_new_label_find = old_new_label_map.find((*tmp_2).rs);
						if (old_new_label_find == old_new_label_map.end()) {
							branch.rs = inline_label_reset((*tmp_2).rs);
							old_new_label_map.insert({ (*tmp_2).rs, branch.rs });
						}
						else {
							branch.rs = old_new_label_find->second;
						}

						this_function_context.push_back(branch);
						break;
					default:
						this_function_context.push_back(*tmp_2);
						break;
					}
				}

				break;
			case FUNCTIONBEGIN:
				function_name = (*function_inline_it).rd;
			default:
				this_function_context.push_back((*function_inline_it));
				break;
			}
			++function_inline_it;
		}
		this_function_context.push_back(*function_inline_it);

		// insert new var
		
		if (not new_var_info.empty()) {
			if (not old_has_var) {
				this_function_context.insert(this_function_context.begin(), Intermediate_code(VARDEFEND, "", "", ""));
				this_function_context.insert(this_function_context.begin(), Intermediate_code(VARDEF, "", "", ""));
			}
			tmp = this_function_context.begin();
			while ((*tmp).operation != VARDEFEND) {
				++tmp;
			}
			for (tmp_2 = new_var_info.begin(); tmp_2 != new_var_info.end(); ++tmp_2) {
				tmp = this_function_context.insert(tmp, *tmp_2);
				++tmp;
			}
		}
		

		// prepare inline for other functions
		function_begin_it = this_function_context.begin();
		if (can_inline) {
			function_inline_info inline_info;
			bool isPara = false;
			bool isVar = false;
			int return_count = 0;
			string return_label = "";
			while ((*function_begin_it).operation != FUNCTIONEND) {
				switch ((*function_begin_it).operation)
				{
				case FUNCTIONBEGIN:
					break;
				case PARADEF:
					isPara = true;
					break;
				case PARADEFEND:
					isPara = false;
					break;
				case VARDEF:
					isVar = true;
					break;
				case VARDEFEND:
					isVar = false;
					break;
				case VAR:
					if (isPara) {
						inline_info.para.push_back(*function_begin_it);
					}
					else if (isVar) {
						inline_info.var.push_back(*function_begin_it);
					}
					break;
				case RET:
					++return_count;
					if (return_label == "") {
						return_label = label_generate(function_name + "__return__");
					}
					inline_info.inline_context.push_back(Intermediate_code(BEQ, return_label, "0", "0"));
					break;
				default:
					inline_info.inline_context.push_back((*function_begin_it));
					break;
				}
				++function_begin_it;
			}
			
			if (return_label != "") {
				int tail_branch_count = 0;
				tmp = inline_info.inline_context.end() - 1;
				bool needBreak = false;
				while (tmp != inline_info.inline_context.begin()) {
					switch ((*tmp).operation)
					{
					case BEQ:
					case BLE:
					case BNE:
					case BLT:
						if ((*tmp).rd == return_label) {
							++tail_branch_count;
						}
						else {
							needBreak = true;
						}
						break;
					default:
						needBreak = true;
						break;
					}
					--tmp;
					if (needBreak) {
						break;
					}
				}
				if (tmp == inline_info.inline_context.begin()) {
					switch ((*tmp).operation)
					{
					case BEQ:
					case BLE:
					case BNE:
					case BLT:
						if ((*tmp).rd == return_label) {
							++tail_branch_count;
						}
						break;
					default:
						break;
					}
				}

				if (tail_branch_count == return_count) {
					inline_info.inline_context.erase(inline_info.inline_context.end() - return_count, inline_info.inline_context.end());
				}
				else {
					inline_info.inline_context.erase(inline_info.inline_context.end() - tail_branch_count, inline_info.inline_context.end());
					inline_info.inline_context.push_back(Intermediate_code(SETLABEL, "", return_label, ""));
				}
				
			}
			function_inline_map.insert({ function_name, inline_info });
		}

		

		++function_inline_it;
		new_code.insert(new_code.end(), this_function_context.begin(), this_function_context.end());
	}
	codes = new_code;
}

void code_better() {
	vector<Intermediate_code>::iterator i = codes.begin();
	while (i != codes.end()) {
		if ((*i).operation == MOVE) {
			if (i != codes.begin()) {
				vector<Intermediate_code>::iterator j = i - 1;
				Intermediate_code code = *j;
				switch ((*j).operation)
				{
				case ADD:
				case SUB:
				case MULTOP:
				case DIVOP:
				case MOVE:
				case ARRLOAD:
				case READ:
					if (code.rd.at(0) == '$' && code.rd.at(1) == 't') {
						if ((*i).rd.at(0) == '$' && (*i).rd.at(1) != 't') {
							++i;
							break;
						}
						if ((*i).rs == code.rd) {
							(*j).rd = (*i).rd;
							i = codes.erase(i);
							break;
						}
					}
				default:
					++i;
					break;
				}
			}
		}
		else {
			++i;
		}
	}
}

void move_better() {
	vector<Intermediate_code>::iterator i = codes.begin();
	while (i != codes.end()) {
		Intermediate_code code = *i;
		vector<Intermediate_code>::iterator j;
		bool needBreak;
		bool cando;
		switch (code.operation)
		{
		case MOVE:
			if (code.rd.at(0) == '$' && code.rd.at(1) == 'a') {
				++i;
				cando = false;
				break;
			}
			if (code.rs.at(0) == '$' && code.rs.at(1) == 'v') {
				++i;
				cando = false;
				break;
			}
			j = i + 1;
			needBreak = false;
			cando = false;
			while (j != codes.end()) {
				switch ((*j).operation)
				{
				case BEQ:
				case BNE:
				case BLT:
				case BLE:
				case SETLABEL:
				case RET:
				case END:
				case CALL:
				case FUNCTIONEND:
					needBreak = true;
					break;
				case ADD:
				case SUB:
				case MULTOP:
				case DIVOP:
				case ARRLOAD:
				case ARRSAVE:
				case READ:
				case MOVE:
					if ((*j).rd == code.rd) {
						needBreak = true;
						cando = true;
					}
					if ((*j).rd == code.rs || (*j).rd == "$v0") {
						needBreak = true;
					}
					break;
				default:
					
					break;
				}
				if (needBreak) {
					break;
				}
				++j;
			}
			needBreak = false;
			j = i + 1;
			if (cando) {
				while (j != codes.end()) {
					switch ((*j).operation)
					{
					case BEQ:
					case BNE:
					case BLT:
					case BLE:
					case SETLABEL:
					case RET:
					case END:
					case CALL:
					case FUNCTIONEND:
						needBreak = true;
						break;
					case ADD:
					case SUB:
					case MULTOP:
					case DIVOP:
					case ARRLOAD:
					case ARRSAVE:
					case MOVE:
						if ((*j).rd == "$v0") {
							if ((*j).rs == code.rd) {
								(*j).rs = code.rs;
							}
							if ((*j).rt == code.rd) {
								(*j).rt = code.rs;
							}
							needBreak = true;
							break;
						}
						if ((*j).rd != code.rd && (*j).rd != code.rs) {
							if ((*j).rs == code.rd) {
								(*j).rs = code.rs;
							}
							if ((*j).rt == code.rd) {
								(*j).rt = code.rs;
							}
						}
						else {
							if ((*j).rs == code.rd) {
								(*j).rs = code.rs;
							}
							if ((*j).rt == code.rd) {
								(*j).rt = code.rs;
							}
							needBreak = true;
							break;
						}
						break;
					case READ:
						if ((*j).rd == code.rd || (*j).rd == code.rs) {
							needBreak = true;
							break;
						}
					case PRINT:
						if ((*j).rd == code.rd) {
							(*j).rd = code.rs;
						}
						break;
					default:

						break;
					}
					if (needBreak) {
						break;
					}
					++j;
				}
				
			}
			if (cando) {
				i = codes.erase(i);
			}
			else {
				++i;
			}
			break;
		default:
			++i;
			break;
		}
	}
}