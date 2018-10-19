#pragma once
#include <string>
#include <vector>
#include "helpers.hpp"
#include "node.hpp"
#include "parser_expression.hpp"

namespace dbas {
using namespace std;

class Parser_Structure {
public:
	// output
	Node prog;

	int parse(const string& fname, const vector<string>& lines) {
		this->fname = fname, this->lines = lines;  // copy input (TODO: save space here?)
		lineno = 0, prog = {};  // reset state
		Node tnode;
		tok_clear_blank();
		// global def block
		parse_def_block(tnode);
		prog.kids.push_back(tnode);
		// parse subroutines
		prog.kids.push_back({ "subroutines" });
		auto& subs = prog.kids.back();
		while (parse_subroutine(tnode)) 
			subs.kids.push_back(tnode);
		return 0;
	}

	Node tok_rawline() {
		return { "RAW_LINE", {
			{ fname },
			{ to_string(lineno+1) },
			{ helpers::chomp(lines[lineno]) }
		}};
	}


private:
	// input
	string fname;
	vector<string> lines;
	int lineno = 0;
	// expressions
	Parser_Expression expr;


	// token management
	int tok_exists() {
		return (lineno < (int)lines.size());
	}
	vector<string> tok_tokline() {
		if (!tok_exists()) return {};
		return helpers::split( lines[lineno] );
	}
	void tok_clear_blank() {
		while (tok_exists()) {
			if (tok_tokline().size() > 0)  break;
			lineno++;
		}
	}
	int tok_next() {
		if (tok_exists()) lineno++;
		tok_clear_blank();
		return tok_exists();
	}


	// parsing block
	int parse_def_block(Node& n) {
		// initialise def_block node
		int count = 0;
		n = { "def_block" };
		// consts
		n.kids.push_back({ "const_list" });
		auto& const_list = n.kids.back();
		while (tok_exists()) {
			auto tok = tok_tokline();
			if (tok[0] != "const")  break;
			// save
			const_list.kids.push_back(tok_rawline());
			count++;
			tok_next();
		}
		// vars
		n.kids.push_back({ "dim_list" });
		auto& dim_list = n.kids.back();
		while (tok_exists()) {
			auto tok = tok_tokline();
			if (tok[0] != "dim")  break;
			// save
			dim_list.kids.push_back(tok_rawline());
			count++;
			tok_next();
		}
		// OK
		return count;
	}

	int parse_subroutine(Node& n) {
		// check for valid sub
		if (!tok_exists()) return 0;
		auto tok = tok_tokline();
		if (tok[0] != "sub") throw string("expected sub");
		if (tok.size() != 2) throw string("expected sub name");
		// initialize sub node
		string name = tok[1];
		n = { name };
		tok_next();
		// def block
		n.kids.push_back({ "temp" });
		parse_def_block(n.kids.back());
		// sub contents
		Node tnode;
		while (tok_exists()) {
			tok = tok_tokline();
			// check for end of sub
			if (tok.size() == 2 && tok[0] == "end" && tok[1] == "sub")
				return tok_next(), 1;
			// sub contents
			if (parse_statement(tnode))
				n.kids.push_back(tnode);
		}
		// early EOF before end of block
		throw string("unexpected EOF in sub ["+name+"]");
	}

	int parse_statement(Node& n) {
		// error checks
		if (!tok_exists()) return 0;
		// initialise node
		n = { "statement" };
		auto tok = tok_tokline();
		// control blocks
		if (tok[0] == "if") {
			n.kids.push_back({ "if_block" });
			return parse_if_block(n.kids.back());
		}
		// check for valid statements
		if (tok[0] == "let") {
			if (!(tok.size() >= 4 && tok[2] == "=")) throw string("invalid let statement");
			n.kids.push_back({ "let" });
			auto& let = n.kids.back();
			// get variable name
			if (!helpers::is_ident( tok[1] )) throw string("expected identifier");
			let.kids.push_back({ tok[1] });
			// parse expression
			expr.parse( vector<string>( tok.begin()+3, tok.end() ) );
			throw string("WIP - let statement");
		}

		// accept all
		n.kids.push_back(tok_rawline());
		tok_next();
		return 1;
		// throw string("unknown statement");
	}

	int parse_if_block(Node& n) {
		// error checks
		if (!tok_exists()) return 0;
		auto tok = tok_tokline();
		if (tok[0] != "if") throw string("expected 'if'");
		if (tok.back() != "then") throw string("expected 'then' after 'if'");
		// initialise node
		n = { "if_block", {
			{ "condition", {
				tok_rawline()
			}}
		}};
		tok_next();
		// block contents
		Node tnode;
		while (tok_exists()) {
			tok = tok_tokline();
			// check for end of sub
			if (tok.size() == 2 && tok[0] == "end" && tok[1] == "if")
				return tok_next(), 1;
			// sub contents
			if (parse_statement(tnode))
				n.kids.push_back(tnode);
		}
		// early EOF before end of block
		throw string("unexpected EOF in if_block");
	}
};

} // end dbas