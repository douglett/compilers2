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
		n = { "expression_temp" }, tok = vs, pos = 0;
		// throw string("expression parser not implemented");
		for (auto s : vs)
			n.kids.push_back({ s });
		return 0;
	}


private:
	vector<string> tok;
	int pos = 0;

	int expect(const string& type, const string& val="") {
		return 0;
	}
	
	// int parse_expression() {
	// 	// add or sub
	// 	if (expect("operator", "+") || expect("operator", "-")) {
	// 		auto& op = lasttok();
	// 		parse_term();
	// 		emit({ "OPR", op.val });
	// 	}
	// 	// none - just parse term
	// 	else {
	// 		parse_term();
	// 	}
	// 	// if add or sub, loop
	// 	while (expect("operator", "+") || expect("operator", "-")) {
	// 		auto& op = lasttok();
	// 		parse_term();
	// 		emit({ "OPR", op.val });
	// 	}
	// }

};

} // end dbas