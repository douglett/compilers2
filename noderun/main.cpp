#include <iostream>
#include <string>
#include <vector>
using namespace std;


struct Node {
	string val;
	vector<Node> kids;
};


int main() {
	printf("hello world\n");
	Node prog = { "test prog", {
		{ "globals", {
			{ "consts", {
				{ "a", {
					{ "LIT 1" }
				}}
			}},
			{ "vars", {
				{ "b", {
					{ "LIT 1" }
				}}
			}}
		}},
		{ "subroutines", {
			{ "main", {
				{ "set", {
					{ "VAR a" },
					{ "expression", {
						{ "+", {
							{ "LIT 10" },
							{ "LIT 10" }
						}}
					}}
				}}
			}}
		}}
	}};
}