#pragma once
#include <string>
#include <vector>
#include <sstream>

namespace helpers {
	using namespace std;

	// string manipulation
	inline vector<string> split(const string& str) {
		vector<string> vs;
		stringstream ss(str);
		string s;
		while (ss >> s)
			vs.push_back(s);
		return vs;
	}
	inline string join(const vector<string>& vs) {
		string s;
		for (int i = 0; i < (int)vs.size(); i++)
			s += (i > 0 ? " " : "") + vs[i];
		return s;
	}
	inline string chomp(const string& str) {
		int i, j;
		for (i = 0; i < (int)str.length(); i++)
			if (!isspace(str[i]))  break;
		for (j = str.length()-1; j >= 0; j--)
			if (!isspace(str[j]))  break;
		return str.substr( i, j + 1 - i );
	}

	// special string
	const string OP_LIST = "+-*/=!()\"";
	inline vector<string> split_special(const string& str) {
		vector<string> vs;
		string s;
		for (int i = 0; i < (int)str.length(); i++) {
			if ( isspace(str[i]) ) {
				if (s.length())  vs.push_back(s), s = "";  
			}
			else if ( OP_LIST.find(string()+str[i]) ) {
				if (s.length())  vs.push_back(s), s = "";
				vs.push_back(string() + str[i]);
			}
			else
				s += str[i];
		}
		if (s.length()) vs.push_back(s);
		return vs;
	}

	// string validation
	inline int is_alpha(char c) {
		if (c >= 'a' && c <= 'z') return 1;
		if (c >= 'A' && c <= 'Z') return 1;
		if (c == '_') return 1;
		return 0;
	}
	inline int is_numeric(char c) {
		if (c >= '0' && c <= '9') return 1;
		return 0;
	}
	inline int is_alphanumeric(char c) {
		return is_alpha(c) || is_numeric(c);
	}
	inline int is_ident(const string& s) {
		if (s.size() == 0) return 0;
		if (!is_alpha(s[0])) return 0;
		for (auto c : s)
			if (!is_alphanumeric(c)) return 0;
		return 1;
	}
	inline int is_integer(const string& s) {
		int pos = 0;
		if (s.size() == 0) return 0;
		if (s[0] == '-' || s[0] == '+') pos++;
		for ( ; pos < (int)s.size(); pos++)
			if (!is_numeric(s[pos]))  return 0;
		return 1;
	}

} // end helpers