#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace dbas {
using namespace std;

struct Node {
	std::string val;
	std::vector<Node> kids;
};

void streamnode(ostream& stream, const Node& n, int indent=0) {
	string pad(indent*2, ' ');
	if (n.kids.size() == 0) {
		stream << pad << "{ \"" << n.val << "\" }" << endl;
		return;
	}
	stream << pad << "{ \"" << n.val << "\"" << endl;
	for (auto nn : n.kids)
		streamnode(stream, nn, indent+1);
	stream << pad << "}" << endl;
}

int streamnodefile(const string& fname, const Node& n) {
	fstream fs(fname, ios::out);
	if (!fs.is_open())
		return fprintf(stderr, "error: could not open file: %s\n", fname.c_str()), 1;
	fs << __DATE__ << " -- " << __TIME__ << endl;
	fs << fname << endl;
	fs << "-----" << endl;
	streamnode(fs, n);
	return 0;
}

} // end dbas