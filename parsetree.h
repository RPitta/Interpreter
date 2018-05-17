/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
#include <string>
#include <exception>
#include <memory>
using std::vector;
using std::map;

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE };

#include "value.h"
 
// a "forward declaration" for a class to hold values
class Value;
 
extern map<string, Value> symTable;  // Every class needs to see this  
 
class ParseTree {
protected:
	int			linenum;
	ParseTree	*left;
	ParseTree	*right;
 	bool 		firsttime = true;					 // This is for SliceOperand.

	

public:
	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
		: linenum(linenum), left(l), right(r) {}

	virtual ~ParseTree() {
		delete left;
		delete right;
	}

	int GetLineNumber() const { return linenum; }

	virtual NodeType GetType() const { return ERRTYPE; }

	int LeafCount() const {
		int lc = 0;
		if( left ) lc += left->LeafCount();
		if( right ) lc += right->LeafCount();
		if( left == 0 && right == 0 )
			lc++;
		return lc;
	}

	virtual bool IsIdent() const { return false; }
	virtual bool IsVar() const { return false; }
	virtual string GetId() const { return ""; }

	int IdentCount() const {
		int cnt = 0;
		if( left ) cnt += left->IdentCount();
		if( right ) cnt += right->IdentCount();
		if( IsIdent() )
			cnt++;
		return cnt;
	}

	void GetVars(map<string,bool>& var) {
		if( left ) left->GetVars(var);
		if( right ) right->GetVars(var);
		if( IsVar() )
			var[ this->GetId() ] = true;
	}
 	
 	// Eval is pure virtual so every class needs its own copy of Eval
 	virtual Value Eval() = 0;
};

class StmtList : public ParseTree {

public:
	StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}
 
 	Value Eval() {
 		Value t = left->Eval();
 		if (right) {
			t = right->Eval();
 		}
 		return Value(t);
 	}
};

class VarDecl : public ParseTree {
	string id;

public:
	VarDecl(Token& t, ParseTree *ex) : ParseTree(t.GetLinenum(), ex), id(t.GetLexeme()) {}

	bool IsVar() const { return true; }
	string GetId() const { return id; }
 
 	Value Eval() {
 		Value v = left->Eval();
 		if (symTable.count(id)) {
 			cout << left->GetLineNumber() << ": Duplicate variables" << endl;
 			return 0;
 		}
 		else {
 			symTable[id] = v;
 		}
 		
 		return v;
 	}
};

class Assignment : public ParseTree {
	string id;

public:
	Assignment(Token& t, ParseTree *e) : ParseTree(t.GetLinenum(), e), id(t.GetLexeme()) {}
 
 	string GetId() const { return id; }
 
 	Value Eval() {
 		Value expres = left->Eval();
 		if (symTable.count(id)) { 
 			if (expres.GetType() == symTable.at(id).GetType()) {
				symTable.at(id) = expres;
 				return expres;
 			}	
 			else {
 				cout << left->GetLineNumber() << ": Types do not match" << endl;
 			}
 		}
 		else {
 			cout << left->GetLineNumber() << ": Must define variable before using" << endl;
 		}
 		return 0; 
 	}
};

class Print : public ParseTree {
public:
	Print(int line, ParseTree *e) : ParseTree(line, e) {}
 	
 	Value Eval() {
 		Value expres = left->Eval();
 		cout << expres;
 		return expres;
 	}
};

class Repeat : public ParseTree {

public:
	Repeat(int line, ParseTree *e, ParseTree *s) : ParseTree(line, e, s) {}
 
 	Value Eval() {
 		Value e1 = left->Eval();
 		if (e1.GetIntValue() >= 0) {
 			Value s1;
 			for (int i = 0; i < e1.GetIntValue(); ++i)
				Value s1 = right->Eval();
 			return s1;
 		}
 		
 		cout << "RUNTIME ERROR: \n";
 		throw exception();
 	}
 
};


class PlusExpr : public ParseTree {
public:
	PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

 	Value Eval() {
 		Value op1 = left->Eval();
 		Value op2 = right->Eval();
 
 		if (op1.GetType() == INTTYPE && op2.GetType() == INTTYPE) {
 			return Value(op1.GetIntValue() + op2.GetIntValue());
 		}
 		
 		return Value(op1.GetStrValue() + op2.GetStrValue());
 	}
};

class MinusExpr : public ParseTree {
public:
	MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
 
 	Value Eval() {
 		Value op1 = left->Eval();
 		Value op2 = right->Eval();
 
 		if (op1.GetType() == INTTYPE && op2.GetType() == INTTYPE) {
 			return Value(op1.GetIntValue() - op2.GetIntValue());
 		}
 		
 		// Subtraction on strings.
 		string a = op1.GetStrValue();
 		string b = op2.GetStrValue();
 		string substring;
 		
 		size_t pos = a.find(b);
 		if (pos > a.length())
 			substring = a;
 		else {
            string s2 = a.substr(0, pos);
            string s3 = a.substr (pos + b.length());
            substring = s2 + s3;
 		}
 		return substring;
 	}		
};

class TimesExpr : public ParseTree {
public:
	TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
 
 	Value Eval() {
 		Value op1 = left->Eval();
 		Value op2 = right->Eval();
 
 		if (op1.GetType() == INTTYPE && op2.GetType() == INTTYPE) {
 			return Value(op1.GetIntValue() * op2.GetIntValue());
 		}
 		
 		// Multiplication on strings.
 		if (op1.GetType() == STRTYPE && op2.GetType() == INTTYPE) {
 			string s;
 			
 			for (int i = 0; i < op2.GetIntValue(); ++i) {                              	
				 s += op1.GetStrValue();
            }
             
            return s;
		}
                                           
        if (op1.GetType() == INTTYPE && op2.GetType() == STRTYPE) {
 			string s;
 			
 			for (int i = 0; i < op1.GetIntValue(); ++i) {                               	
				 s += op2.GetStrValue();
            }
            
            return s;
		}                                   
                                          
		return 0;
 	}   
};

class SliceExpr : public ParseTree {  
public:
	SliceExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
 
 	Value Eval() {
 		Value s = left->Eval();	
 		
 		Value e1 = right->Eval();
 		Value e2 = right->Eval();
 	
 		if (s.GetType() == INTTYPE) {
 			cout << left->GetLineNumber() << ": Cannot slice an integer\n";
 			return 0;			
 		}
 
 		unsigned int e = e2.GetIntValue();
 		
 		if (e > s.GetStrValue().length()) {
 			cout << "RUNTIME ERROR: \n";
 			throw exception();
 		}
 
 		string newstring;
 		for (int begin = e1.GetIntValue(); begin < e2.GetIntValue() + 1; begin++){
        	newstring += s.GetStrValue().at(begin);
        }
 		return newstring;
 	}
};

class SliceOperand : public ParseTree {		
public:
	SliceOperand(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
 
 	Value Eval() {
 		Value e1 = left->Eval();		
 		Value e2 = right->Eval();
 
 		if (e1.GetType() != INTTYPE || e1.GetIntValue() < 0) {
        	cout << "RUNTIME ERROR: \n";
        	throw exception();
		}
 		
 		if (e2.GetType() != INTTYPE || e2.GetIntValue() < e1.GetIntValue()) {
        	cout << "RUNTIME ERROR: \n";
        	throw exception();
		}
 
 		// If this is the first time Eval is being called, return only the first expression.
 		// Next time around, return the second expression.
 		if (firsttime) {
 			firsttime = false;
 			return e1;
 		}
 		return e2;
 	}
};

class IConst : public ParseTree {
	int val;

public:
	IConst(Token& t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}

	NodeType GetType() const { return INTTYPE; }
 
 	Value Eval() {
 		return Value(val);
 	}
};

class SConst : public ParseTree {
	string val;

public:
	SConst(Token& t) : ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}

	NodeType GetType() const { return STRTYPE; }
 
 	Value Eval() {
 		return Value(val);
 	}
};

class Ident : public ParseTree {
	string id;

public:
	Ident(Token& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}

	bool IsIdent() const { return true; }
	string GetId() const { return id; }
 
 	Value Eval() {
 		return symTable.at(id);
 	}
};

#endif /* PARSETREE_H_ */
