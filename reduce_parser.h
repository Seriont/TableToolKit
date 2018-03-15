#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "parser.h"

namespace parser {

class AggregateExpression {
	parser::Expression expression;
public:
	AggregateExpression() : expression() {}
	AggregateExpression(const string & str, const vector<string> & col_names);
	AggregateExpression(const AggregateExpression & aggr_expr);

	void print() const;

	double getValue(const vector< vector<double> > & values);
};

}