#pragma once
#include <map>
#include "node.hpp"
using namespace std;


class RunTreeA {
public:
	typedef dbas::Node Node;
	Node prog;
	vector<Node*> pos;
	vector<map<string, int>> vars;
	
	int run() {
		try {
			pos.push_back(&prog);
			run_prog();
			return 0;
		}
		catch (const string& err) {
			cerr << "run error: " << err << endl;
			return 1;
		}
	}

private:

	void expect(const string& v) {
		if (pos.back().val != v) 
			throw string("expected ["+v+"] got ["+pos.back().val+"]");
	}

	void run_prog() {
		expect("program");
		// expectc("def_block")
	}

};