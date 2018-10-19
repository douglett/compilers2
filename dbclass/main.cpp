#include <iostream>
#include <map>
#include "helpers.hpp"
#include "parse.hpp"
// #include "parse_expr.hpp"
using namespace std;
using namespace dbas;


vector<pair<string, map<string, int>>> var_stack;


int run_def(const Def& def, const string& name) {
	var_stack.push_back({ name, {} });
	auto& stacktop = var_stack.back().second;
	// get consts into global list
	for (auto& c : def.constlist) {
		printf("%s\n", c.join().c_str());
		if (c.tok.size() != 4)  throw string("bad const format");
		stacktop["c__"+c.tok[1].val] = stoi(c.tok[3].val, 0, 10);
	}
	// get vars into global list
	for (auto& c : def.dimlist) {
		printf("%s\n", c.join().c_str());
		if (c.tok.size() != 4)  throw string("bad dim format");
		stacktop["v__"+c.tok[1].val] = stoi(c.tok[3].val, 0, 10);
	}
	// OK
	return 0;
}

// int run_expression(const vector<Token>& vt, int& pos) {

// }

// int run_condition(const vector<Token>& vt, int pos=0) {
// 	// LHS
// 	int lhs = run_expression(vt, pos);
// 	// int rhs = run_expression();
// }

int run_stmt(const Stmt& stmt) {
	printf("%s\n", stmt.line.join().c_str());
	// run if block
	auto& tok = stmt.line.tok;
	if (tok.size() >= 5 && tok[0].val == "if") {
		// validate
		if (tok.back().val != "then")
			throw string("bad if format: line "+to_string(stmt.line.lineno+1));
		// handle expression
		vector<Token> vt(tok.begin()+1, tok.end()-1);
		// HERE
		// run_condition(vt);
	}
	// OK
	return 0;
}

int run_sub(const ParseStructure& prog, const string& name) {
	// find sub
	const Sub* subp = NULL;
	for (auto& s : prog.sublist)
		if (s.name == name)
			subp = &s;
	if (subp == NULL)  throw string("sub not found: "+name);
	printf("running sub: %s\n", name.c_str());
	// define
	run_def(subp->defblock, name);
	// run statements
	for (auto& stmt : subp->statements)
		run_stmt(stmt);
	// un-define
	var_stack.pop_back();
	// OK
	return 0;
}

int run(const ParseStructure& prog) {
	printf("-- parse_global --\n");
	try {
		// define
		run_def(prog.defblock, "global");
		// run main
		run_sub(prog, "main");
	}
	catch (const string& err) {
		fprintf(stderr, "error: %s\n", err.c_str());
		return 1;
	}
	// OK
	printf("-- end prog --\n");
	return 0;
}


int main() {
	printf("hello world\n");

	ParseStructure p;
	if (p.load("test.bas")) return 1;
	if (p.parse()) return 1;
	printf("\n");
	p.show();

	printf("\n");
	if (run(p)) return 1;
}
