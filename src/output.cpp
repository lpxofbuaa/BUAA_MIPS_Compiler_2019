#include "output.h"
#include "main.h"
using namespace std;

void Output::init() {
	outfile0.open(outfilename);
}

void Output::push(Type t,const string & s) {
	buf.push(type2string(t) + " " + s + "\n");
	// print(outp);
}

void Output::push(const string s) {
	buf.push(s);
}

void Output::output() {
	while (!buf.empty()) {

		outfile0 << buf.front();
		buf.pop();

	}
}

void Output::output(int n) {
	int i = 0;
	while ((!buf.empty()) && (i < n)) {
		i++;

		outfile0 << buf.front();
		buf.pop();
	}
}

void Output::print(const string s) {
	outfile0 << s;
}

void Output::close() {
	outfile0.close();
}

Output::Output() {
	init();
}