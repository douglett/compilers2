#pragma once
#include <map>
#include "node.hpp"
using namespace std;


class RunTreeA {
public:
	typedef dbas::Node Node;
	Node prog;
	vector<map<string, vector<int>>> vars;
	
	int run() {
		try {
			run_prog(prog);
			return 0;
		}
		catch (const string& err) {
			cerr << "run error: " << err << endl;
			return 1;
		}
	}

	int debug_vars() {
		printf("-------------------\n");
		for (const auto& frame : vars) {
			for (const auto& v : frame)
				printf("  %10s [%2d]: %4d\n", 
					v.first.c_str(), 
					v.second.size(), v.second[0] );
			printf("-------------------\n");
		}
		return 0;
	}

private:
	void run_prog(const Node& n) {
		if (n.val != "program")  throw string("expected program");
		if (!(n.kids.size() == 2 && n.kids[0].val == "def_block" && n.kids[1].val == "subroutines"))
			throw string("bad program format");
		run_def_block( n.kids[0] );
		run_sub("main");
	}

	const Node& get_named_node(const Node& n, const string& name) {
		for (auto& n : n.kids)
			if (n.val == name) return n;
		throw string("missing node name: "+name);
	}

	void run_def_block(const Node& n) {
		vars.push_back({});
		// consts
		for (auto& nn : n.kids.at(0).kids) {
			auto& name = nn.val;
			auto val = run_expression( get_named_node(nn, "value" ).kids.at(0) );
			auto len = 1;
			vars.back()[name].resize(len, val);  // create and fill
		}
		// vars
		for (auto& nn : n.kids[1].kids) {
			auto& name = nn.val;
			auto val = run_expression( get_named_node(nn, "value" ).kids.at(0) );
			auto len = run_expression( get_named_node(nn, "length").kids.at(0) );
			vars.back()[name].resize(len, val);  // create and fill
		}
	}

	// WARNING: this doesn't care for const!
	int& get_var(const string& name) {
		if (vars.back().count(name)) return vars.back()[name][0];  // local
		if (vars.at(0).count(name))  return vars[0][name][0];  // global
		throw string("var not found: "+name);
	}

	int run_lit(const Node& n) {
		if (n.val != "LIT") throw string("expected LIT");
		if (n.kids.size() != 1) throw string("bad LIT format");
		return stoi( n.kids[0].val, NULL, 10 );
	}

	int run_expression(const Node& n) {
		if (n.val == "LIT") return run_lit( n );
		if (n.val == "VAR") return get_var( n.kids.at(0).val );
		if (n.val == "expression") return run_expression( n.kids.at(0) );
		// operators
		static const vector<string> OP_LIST = { "+","-","*","/","=","<","<=",">",">=" };
		if (helpers::str_match_any( n.val, OP_LIST )) {
			auto lhs = run_expression( n.kids.at(0) );
			auto rhs = run_expression( n.kids.at(1) );
			if (n.val == "+" )  return lhs +  rhs;
			if (n.val == "-" )  return lhs -  rhs;
			if (n.val == "*" )  return lhs *  rhs;
			if (n.val == "/" )  return lhs /  rhs;
			if (n.val == "=" )  return lhs == rhs;
			if (n.val == "<" )  return lhs <  rhs;
			if (n.val == "<=")  return lhs <= rhs;
			if (n.val == ">" )  return lhs >  rhs;
			if (n.val == ">=")  return lhs >= rhs;
		}
		throw string("unknown expression: "+n.val);
	}

	void run_sub(const string& name) {
		for (const auto& sub : prog.kids.at(1).kids)
			if (sub.val == name) {
				run_def_block( sub.kids.at(0) );
				for (int i = 1; i < (int)sub.kids.size(); i++)
					run_statement( sub.kids[i] );
				vars.pop_back();
				return;
			}
		throw string("sub not found: "+name);
	}

	void run_statement(const Node& n) {
		auto& stmt = n.kids.at(0);
		if (stmt.val == "if_block") {
			run_if_block( stmt );
		}
		else if (stmt.val == "let") {
			auto& name = stmt.kids.at(0).val;
			get_var(name) = run_expression( stmt.kids.at(1) );
		}
		else if (stmt.val == "call") {
			auto& name = stmt.kids.at(0).val;
			run_sub(name);
		}
		else if (stmt.val == "print") {
			string s;
			for (auto v : stmt.kids) {
				if (v.val == "STR_LIT")
					s += v.kids.at(0).val + " ";
				else if (v.val == "VAR")
					s += to_string(get_var( v.kids.at(0).val )) + " ";
				else 
					throw string("unknown in string: "+v.val);
			}
			printf("> %s\n", s.c_str());
		}
		else
			throw string("unknown statement: "+stmt.val);
	}

	void run_if_block(const Node& n) {
		auto& condition = n.kids.at(0);
		if (run_expression( condition.kids.at(0) ))
			for (int i = 1; i < (int)n.kids.size(); i++)
				run_statement(n.kids[i]);
	}

};