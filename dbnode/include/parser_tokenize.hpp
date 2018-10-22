#pragma once
#include <string>
#include <vector>

namespace dbas {
using namespace std;

class Parser_Tokenize {
public:
	vector<string> parse(const string& str) {
		return split_oper_ws(str);
	}

	int test() {
		static const vector<string> TEST_CASES = {
			"1+2 = 3+a + (12)",
			"const a = 1"
			"a = \"hello world\""
		};
		try {
			for (const auto& test : TEST_CASES) {
				printf("parsing: %s\n", test.c_str());
				for (const auto& s : parse(test))
					printf("[%s] ", s.c_str());
				printf("\n");
			}
			return 0;
		}
		catch (const string& err) {
			cerr << "tokenize test error:" << endl
				<< "\t" << err << endl;
			return 1;
		}
	}

private:
	// special operators
	const string OP_LIST = "+-*/=!()";

	// split on operators and whitespace
	vector<string> split_oper_ws(const string& str) {
		vector<string> vs;
		string s;
		for (int i = 0; i < (int)str.length(); i++) {
			// space - next word
			if ( isspace(str[i]) ) {
				if (s.length())  vs.push_back(s), s = "";  
			}
			// operator - split current word on operator, and add both
			else if ( OP_LIST.find(string()+str[i]) != string::npos ) {
				if (s.length())  vs.push_back(s), s = "";
				vs.push_back(string() + str[i]);
			}
			// strings
			// else if (str[i] == '"') {
			// 	if (s.length())  vs.push_back(s), s = "";
			// 	split_string();
			// }
			// continue current word
			else
				s += str[i];
		}
		if (s.length()) vs.push_back(s);
		return vs;
	}

	// string splitting
	int split_string() {
		throw string("string splitting unimplemented");
	}

};

} // end dbas