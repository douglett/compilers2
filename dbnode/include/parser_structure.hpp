#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "helpers.hpp"
#include "node.hpp"
#include "parser_tokenize.hpp"
#include "parser_expression.hpp"

namespace dbas {
using namespace std;

class Parser_Structure {
public:
	// output
	Node prog;

	int parse(const string& fname, const vector<string>& lines) {
		this->fname = fname, this->lines = lines;  // copy input (TODO: save space here?)
		lineno = 0, prog = { "program" };  // reset state
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
	// sub-parsers
	Parser_Tokenize tokz;
	Parser_Expression expr;


	// token management
	int tok_exists() {
		return (lineno < (int)lines.size());
	}
	vector<string> tok_tokline() {
		if (!tok_exists()) return {};
		// return helpers::split( lines[lineno] );
		return tokz.parse( lines[lineno] );
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


	// error checking
	// int check_vars_exist() {
	// 	// global vars
	// 	auto gnames = get_var_names( "<global>", prog.kids.at(0) );
	// 	for (const auto& sub : prog.kids.at(1)) {
	// 		// local vars
	// 		auto lnames = get_var_names( sub.val, sub.kids.at(0) );
	// 		// combine
	// 		auto names = gnames;
	// 		names.insert(names.end(), lnames.begin(), lnames.end());
	// 	}
	// 	return 0;
	// }
	// vector<string> get_var_names(const string& name, const Node& def) {
	// 	vector<string> vs;
	// 	// check consts, then vars
	// 	for (int i = 0; i <= 1; i++)
	// 	for (auto& n : def.kids.at(i).kids) {
	// 		if (find(vs.begin(), vs.end(), n.val) != vs.end())
	// 			throw string("duplicate value in ["+name+"]: "+n.val);  // check for duplicates
	// 		vs.push_back(n.val);
	// 	}
	// 	return vs;
	// }


	// parsing block
	int parse_def_block(Node& n) {
		// initialise def_block node
		int count = 0;
		Node ntemp;
		n = { "def_block" };
		vector<string> name_list;
		// consts
		n.kids.push_back({ "const_list" });
		auto& const_list = n.kids.back();
		while (tok_exists()) {
			auto tok = tok_tokline();
			if (tok[0] != "const")  break;
			if (tok.size() != 4 || tok[2] != "=") throw string("bad const format");
			if (!helpers::is_ident(tok[1])) throw string("const: expected ident");
			string name = tok[1];
			// check for duplicates
			if (find(name_list.begin(), name_list.end(), name) != name_list.end())
				throw string("duplicate definition: "+name);
			name_list.push_back(name);
			// save
			const_list.kids.push_back({
				name, {
					{"LIT", {{ tok[3] }}}
				}
			});
			count++;
			tok_next();
		}
		// vars
		n.kids.push_back({ "dim_list" });
		auto& dim_list = n.kids.back();
		while (tok_exists()) {
			auto tok = tok_tokline();
			if (tok[0] != "dim")  break;
			if (tok.size() < 4 || tok[2] != "=") throw string("bad dim format");
			if (!helpers::is_ident(tok[1])) throw string("dim: expected ident");
			string name = tok[1];
			// check for duplicates
			if (find(name_list.begin(), name_list.end(), name) != name_list.end())
				throw string("duplicate definition: "+name);
			name_list.push_back(name);
			// save
			// dim_list.kids.push_back(tok_rawline());
			expr.parse(ntemp, tok.begin()+3, tok.end());
			dim_list.kids.push_back({
				name, {
					ntemp
				}
			});
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
		if (tok.size() != 2 || !helpers::is_ident(tok[1])) throw string("expected sub name");
		string name = tok[1];
		// initialize sub node
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
		Node ntemp;
		auto tok = tok_tokline();
		// control blocks
		if (tok[0] == "if") {
			n.kids.push_back({ "if_block" });
			return parse_if_block(n.kids.back());
		}
		// check for valid statements
		if (tok[0] == "let") {
			if (tok.size() < 4 || tok[2] != "=") throw string("bad let format");
			if (!helpers::is_ident(tok[1])) throw string("let: expected ident");
			string name = tok[1];
			// parse expression
			expr.parse( ntemp, tok.begin()+3, tok.end() );
			n.kids.push_back({
				"let", {
					{ name },
					ntemp
				}
			});
			tok_next();
			return 1;
		}
		if (tok[0] == "call") {
			if (tok.size() != 2 || !helpers::is_ident(tok[1])) throw string("invalid call statement");
			string name = tok[1];
			n.kids.push_back({
				"call", {
					{ name }
				}
			});
			tok_next();
			return 1;
		}
		if (tok[0] == "print") {
			ntemp = { "print" };
			for (int i = 1; i < (int)tok.size(); i++)
				ntemp.kids.push_back({ tok[i] });
			// parse OK
			n.kids.push_back(ntemp);
			tok_next();
			return 1;
		}
		// unknown - fail
		throw string("unknown statement");
	}

	int parse_if_block(Node& n) {
		// error checks
		if (!tok_exists()) return 0;
		auto tok = tok_tokline();
		if (tok[0] != "if") throw string("expected 'if'");
		if (tok.back() != "then") throw string("expected 'then' after 'if'");
		// initialise node
		Node ntemp;
		expr.parse(ntemp, tok.begin()+1, tok.end()-1);
		n = { "if_block", {
			{ "condition", {
				ntemp
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