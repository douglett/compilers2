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

} // end dbas