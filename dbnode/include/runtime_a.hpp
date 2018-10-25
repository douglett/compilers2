#pragma once
#include <map>
#include "node.hpp"
using namespace std;
// using namespace dbas;


class RunTreeA {
public:
	typedef dbas::Node Node;
	Node prog;
	vector<map<string, int>> vars;
	
	int run() {
		try {
			run_prog(prog);
			for (auto v : vars.back())
				printf("  %10s : %4d\n", v.first.c_str(), v.second);
			return 0;
		}
		catch (const string& err) {
			cerr << "run error: " << err << endl;
			return 1;
		}
	}

private:
	void run_prog(const Node& n) {
		if (n.val != "program")  throw string("expected program");
		if (!(n.kids.size() == 2 && n.kids[0].val == "def_block" && n.kids[1].val == "subroutines"))
			throw string("bad program format");
		run_def_block( n.kids[0] );
		run_sub("main");
	}

	void run_def_block(const Node& n) {
		if (n.val != "def_block")  throw string("expected def_block");
		if (!(n.kids.size() == 2 && n.kids[0].val == "const_list" && n.kids[1].val == "dim_list"))
			throw string("bad def_block format");
		vars.push_back({});
		// consts
		for (auto& nn : n.kids[0].kids) {
			auto& name = nn.val;
			if (nn.kids.size() != 1) throw string("expected LIT");
			vars.back()[name] = run_lit( nn.kids[0] );
		}
		// vars
		for (auto& nn : n.kids[1].kids) {
			auto& name = nn.val;
			if (nn.kids.size() != 1) throw string("expected expression");
			vars.back()[name] = run_expression( nn.kids[0] );
		}
	}

	int run_lit(const Node& n) {
		if (n.val != "LIT") throw string("expected LIT");
		if (n.kids.size() != 1) throw string("bad LIT format");
		return stoi( n.kids[0].val, NULL, 10 );
	}

	int run_expression(const Node& n) {
		// current item
		if (n.val == "LIT") return run_lit( n );
		// one child
		if (n.kids.size() == 1) {
			if (n.val == "expression") return run_expression( n.kids[0] );
		}
		// 2 children
		if (n.kids.size() == 2) {
			if (n.val == "+") return run_expression(n.kids[0]) + run_expression(n.kids[1]);
		}
		throw string("unknown expression: "+n.val);
	}

	void run_sub(const string& name) {
		for (const auto& sub : prog.kids[1].kids)
			if (sub.val == name) {
				run_def_block( sub.kids[0] );
				for (int i = 1; i < (int)sub.kids.size(); i++)
					run_statement( sub.kids[i] );
				vars.pop_back();
				return;
			}
		throw string("sub not found: "+name);
	}

	void run_statement(const Node& n) {
		if (!(n.val == "statement" && n.kids.size() == 1))  throw string("expected statement");
		auto& stmt = n.kids[0];
		if (stmt.val == "if_block")  run_if_block( stmt );
		if (stmt.val == "let") {
			auto& name = stmt.kids[0].val;
			vars.back()[name] = run_expression( stmt.kids[1] );
		}
		// if (stmt.val == "call")  
		// if (stmt.val == "print") 
		throw string("unknown statement: "+stmt.val);
	}

	void run_if_block(const Node& n) {
		if (n.val != "if_block")  throw string("expected if_block");
	}

};