/*
 * value.h
 */

#ifndef VALUE_H_
#define VALUE_H_

#include "parsetree.h"

class Value {
	int	ival;
	string sval;
	NodeType type;

public:
	Value() : ival(0),type(ERRTYPE) {}
	Value(int i) : ival(i),type(INTTYPE) {}
	Value(string s) : ival(0),sval(s),type(STRTYPE) {}

	NodeType GetType() const { return type; }

	int GetIntValue() const {
		if( type != INTTYPE )
			throw std::runtime_error("using GetIntValue on a Value that is not an INT");
		return ival;
	}

	string GetStrValue() const {
		if( type != STRTYPE )
			throw std::runtime_error("using GetStrValue on a Value that is not a STRING");
		return sval;
	}

	//  We overload "<<" so that we can print "Values"
	friend ostream& operator<<(ostream& out, const Value& v) {
		if(v.GetType()==STRTYPE)
			out << v.GetStrValue() << endl;
		else if(v.GetType()==INTTYPE)
			out << v.GetIntValue() << endl;
		return out;
	}
};

#endif /* VALUE_H_ */
