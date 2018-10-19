#pragma once
#include <string>
#include <vector>
#include <sstream>

namespace helpers {
	using namespace std;

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

} // end helpers