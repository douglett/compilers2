#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "helpers.hpp"
#include "tokenize.hpp"
using namespace std;
using namespace dbas;

class Def {
public:
	vector<SrcLine> constlist;
	vector<SrcLine> dimlist;
};

class Stmt {
public:
	SrcLine line;
	vector<Stmt> children;
};

class Sub {
public:
	string name;
	Def defblock;
	// vector<string> lines;
	vector<Stmt> statements;
};

class Parse {
public:
	// input
	vector<string> lines;
	int lineno = 0;
	// structure
	Def defblock;
	vector<Sub> sublist;

	int load(const string& fname) {
		fstream fs(fname, ios::in);
		if (!fs.is_open())
			return fprintf(stderr, "error: could not open file [%s]\n", fname.c_str()), 1;
		lines = {}, lineno = 0;
		string s;
		while (getline(fs, s))
			lines.push_back(s);
		return 0;
	}

	int parse() {
		try {
			printf("--parse start\n");
			Def def;
			Sub sub;
			tok_clear_blank();
			// global def block
			parse_def_block(defblock);
			// parse SUBs
			while (parse_sub_block(sub)) 
				sublist.push_back(sub);
			// force EOF
			if (tok_clear_blank(), tok_exists())
				throw string("expected EOF at line: "+to_string(lineno));
			printf("--parse OK\n");
		}
		catch (const string& err) {
			fprintf(stderr, "parse error: %s\n", err.c_str());
			return 1;
		}
		return 0;
	}

	void show() {
		// defines
		printf("--defblock--\n");
		for (const auto& sl : defblock.constlist)
			printf("  [%s]\n", sl.join().c_str());
		for (const auto& sl : defblock.dimlist)
			printf("  [%s]\n", sl.join().c_str());
		// sub contents
		printf("--subblock--\n");
		for (const auto& sub : sublist) {
			printf("SUB [%s]\n", sub.name.c_str());
			for (const auto& sl : sub.defblock.constlist)
				printf("  [%s]\n", sl.join().c_str());
			for (const auto& sl : sub.defblock.dimlist)
				printf("  [%s]\n", sl.join().c_str());
			for (const auto& sl : sub.statements)
				printf("    [%s]\n", sl.line.join().c_str());
		}
	}

private:
	int tok_exists() {
		return (lineno < (int)lines.size());
	}
	void tok_clear_blank() {
		while (tok_exists()) {
			// if (tok_tokenizeln().size() > 0)  break;
			if (tok_srcln().tok.size() > 0)  break;
			lineno++;
		}
	}
	int tok_next() {
		if (tok_exists()) lineno++;
		tok_clear_blank();
		return tok_exists();
	}
	SrcLine tok_srcln() {
		// return tokenized version of current line
		if (!tok_exists())
			throw string("missing token at line: " +to_string(lineno));
		return SrcLine( lines[lineno], lineno );
	}
	void tok_show_line() {
		printf("	[%d] [%s]\n", lineno+1, lines[lineno].c_str());
	}

	int parse_def_block(Def& def) {
		def = Def();
		int count = 0;
		// consts
		// printf("parsing consts\n");
		while (tok_exists()) {
			auto sl = tok_srcln();
			if (sl.tok[0].val != "const")  break;
			// save
			def.constlist.push_back(sl);
			count++;
			tok_next();
		}
		// dim
		// printf("parsing dim\n");
		while (tok_exists()) {
			auto sl = tok_srcln();
			if (sl.tok[0].val != "dim")  break;
			// save
			def.dimlist.push_back(sl);
			count++;
			tok_next();
		}
		return count;
	}

	int parse_sub_block(Sub& sub) {
		sub = Sub();  // reset
		Stmt stmt;
		// sub definition
		// printf("parsing sub\n");
		if (!tok_exists())  return 0;
		auto sl = tok_srcln();
		if (!(sl.tok.size() == 2 && sl.tok[0].val == "sub"))  return 0;
		// save sub
		sub.name = sl.tok[1].val;
		tok_next();
		// sub def block
		parse_def_block(sub.defblock);
		// sub body (statements)
		// printf("parsing sub body\n");
		while (tok_exists()) {
			auto sl = tok_srcln();
			// check for end sub
			if (sl.tok.size() == 2 && sl.tok[0].val == "end" && sl.tok[1].val == "sub") {
				tok_next();
				return 1;
			}
			// save line
			// sub.statements.push_back({ sl });
			// tok_next();
			// parse statement
			if (parse_statement(stmt))
				sub.statements.push_back(stmt);
			else
				throw string("expected statement at: "+to_string(lineno));
		}
		// early EOF before end sub
		throw string("unexpected EOF in sub at: "+to_string(lineno));
	}

	int parse_statement(Stmt& stmt) {
		stmt = Stmt();
		stmt.line = tok_srcln();  // accept all
		tok_next();
		return 1;
	}
};


int main() {
	printf("hello world\n");

	Parse p;
	if (p.load("test.bas")) return 1;
	if (p.parse()) return 1;
	printf("\n");
	p.show();
}
