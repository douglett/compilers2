#pragma once
#include "tokenize.hpp"

namespace dbas {
using namespace std;

static int pos = 0;
static vector<Token> tok;

int parse_expression(const vector<Token>& vt) {
	tok = vt;
	return 0;
}

} // end dbas