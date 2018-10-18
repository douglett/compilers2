#include <iostream>
#include "helpers.hpp"
#include "parse.hpp"
using namespace std;


int main() {
	printf("hello world\n");

	dbas::ParseStructure p;
	if (p.load("test.bas")) return 1;
	if (p.parse()) return 1;
	printf("\n");
	p.show();
}
