#include "Syntax_analysis.h"
#include "global.h"
#include "Intermediate_code.h"
#include "code_generate.h"
#include "main.h"
#include "inline.h"
extern int Syntax_analysis();
extern vector<Intermediate_code>::iterator inline_it;
extern vector<Intermediate_code> str;
extern ofstream middle_before;
extern ofstream middle_after;

int main() {
	middle_before.open("17231162_before.txt");
	
	codeFile.open("code.txt");
	int re = Syntax_analysis(); 
	for (vector<Intermediate_code>::iterator i = str.begin(); i != str.end(); ++i) {
		codes.insert(codes.begin(), *i);
	}
	for (int i = 0; i < codes.size(); ++i) {
		codeFile << codes[i] << endl;
	}
	codeFile.close();
	middle_before.close();
	//cout << "analysis result: " << re << endl;
	if (re != 0) {
		return 0;
	}
	var_reset();
	codeFile.open("var_reset_code.txt");
	for (int i = 0; i < codes.size(); ++i) {
		codeFile << codes[i] << endl;
	}
	codeFile.close();
	function_inline();
	code_better();
	move_better();
	middle_before.open("17231162_after.txt");
	codeFile.open("inline_code.txt");
	for (int i = 0; i < codes.size(); ++i) {
		codeFile << codes[i] << endl;
	}
	codeFile.close();
	middle_before.close();
	mips.open("mips.txt");
	reg_init();
	data_init();
	mips_code_generate(".text");
	mips_code_generate("j __main__");
	while (it != codes.end()) {
		function_generate();
	}
	string k = "-3532";
	cout << atoi(k.c_str()) + 2 << endl;
}