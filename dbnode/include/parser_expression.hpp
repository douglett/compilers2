#pragma once
#include <string>
#include <vector>
#include "node.hpp"

namespace dbas {

using namespace std;
class Parser_Expression {
public:
	typedef   vector<string>::iterator  vs_iter;

	int parse(Node& n, const vs_iter begin, const vs_iter end) {
		return parse( n, vector<string>( begin, end ) );
	}

	int parse(Node& n, const vector<string>& vs) {
		// initialise
		n = { "expression" }, tok = vs, pos = 0;
		// for (auto s : vs)  cout << s << " ";  
		// cout << endl;
		// throw string("expression parser not implemented");
		n.kids.push_back( parse_expression() );
		// must be complete parse
		if (curtok() != "<EOL>")
			throw string("expression: expected <EOL>, got ["+curtok()+"] at pos "+to_string(pos));
		return 0;
	}

	// helpful test function for odd expressions
	int test() {
		static const vector<string> TEST_CASES = { "(a + b) * (b + 2)" };
		Node n;
		for (const auto& test : TEST_CASES)
			try {
				printf("parsing: [%s]\n", test.c_str());
				// split
				auto vs = helpers::split_special(test);
				for (int i = 0; i < (int)vs.size(); i++)
					printf("  %02d :: %s\n", i, vs[i].c_str());
				// parse
				parse(n, vs);
				streamnode(cout, n);
			}
			catch (const string& err) {
				cerr << "error: " << err << endl;
				return 1;
			}
		return 0;
	}


private:
	vector<string> tok;
	int pos = 0;
	const string T_EOL = "<EOL>";

	const string& lasttok() {
		if (pos < 0 || pos > (int)tok.size()) return T_EOL;
		return tok[pos-1];
	}
	const string& curtok() {
		if (pos < 0 || pos >= (int)tok.size()) return T_EOL;
		return tok[pos];
	}
	int expect(const string& type, const string& val="") {
		if (pos >= (int)tok.size())  return 0;  // token in range
		if (val.size() && val == tok[pos])  return ++pos;  // value equality
		if (type == "identifier" && helpers::is_ident(tok[pos]))  return ++pos;  // identifier type
		if (type == "number" && helpers::is_integer(tok[pos]))  return ++pos;  // integer type
		return 0;
	}
	int require(const string& type, const string& val="") {
		if (expect(type, val))  return 1;  // found
		throw string("require: expected ["+type+" "+val+"], got ["+curtok()+"] at pos "+to_string(pos));
	}

	Node parse_factor() {
		if (expect("identifier")) return {"VAR", { {lasttok()} }};
		if (expect("number"))     return {"LIT", { {lasttok()} }};
		// bracketed expression ()
		if (expect("operator", "(")) {
			printf("here %d\n", pos);
			Node n = {"()", { parse_expression() }};
			require("operator", ")");
			return n;
		}
		throw string("unexpected token in factor: "+curtok());
	}

	Node parse_term() {
		Node n = parse_factor();
		// term operators
		while (expect("operator", "*") || expect("operator", "/")) {
			auto& op = lasttok();
			auto nn = n;  // temp copy
			n = { op, { 
				nn,
				parse_factor()
			}};
		}
		// OK
		// return {"term", { n }};
		return n;
	}
	
	Node parse_expression() {
		Node n;
		// add or sub
		if (expect("operator", "+") || expect("operator", "-")) {
			auto& op = lasttok();
			n = { op, {
				parse_term()
			}};
		}
		// none - just parse term
		else {
			n = parse_term();
		}
		// add or sub
		while (expect("operator", "+") || expect("operator", "-")) {
			auto& op = lasttok();
			auto nn = n;  // temp copy
			n = { op, {
				nn,
				parse_term()
			}};
		}
		// return {"expr", { n }};
		return n;
	}

	// Node parse_condition() {}

};

} // end dbas