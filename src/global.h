#include <string>
#include "input.h"
#include "output.h"
#include "tag.h"
#include "Type.h"
#include "Symbols.h"
#include "Intermediate_code.h"
#include "code_generate.h"
#include "inline.h"

using namespace std;

bool debug = true;
string outfilename = "output.txt";
string infilename = "testfile.txt";
Input in;
Output out;
string token;
Type type;
Value_type function_type;
Value_type value_type;
bool isVarFinish = false;
bool isCatchMain = false;
bool isCatchReturn = false;
Value_type returnType;
string name;
char pick = ' ';
int line = 1;
int lastWordLine = line;
int identifyLine = 0;
ofstream errorFile;
ofstream codeFile;
// vector<Value_type> *paralist;
bool errorFlag;
Symbols *symbols = new Symbols();

// Intermediate code generate
queue<string> temp_var;
int temp_counter = 0;
vector<Intermediate_code> codes;
vector<Intermediate_code> str;
int para_num = -1;
int labelCount = -1;
int stringCount = -1;
int stringLabelCount = -1;
int varLabelCount = -1;
int funcLabelCount = -1;
bool isPara = false;

// code generate
map<string, VarInfo> global_var_info;
map<string, VarInfo> function_var_info;
queue<Reg> all_valid_regs;
ofstream mips;
vector<Intermediate_code>::iterator it;
vector<Intermediate_code>::iterator inline_it;
int tab = 0;
int block_index;
FunctionInfo *now_function_info;

// inline
int inline_label_count = -1;
map<string, function_inline_info> function_inline_map;

// output for word
ofstream middle_before;
ofstream middle_after;