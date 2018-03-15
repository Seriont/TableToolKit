#include <iostream>
#include <vector>
#include <string>

using namespace std;

namespace parser {

enum OperationType {
	LB, RB, POW, UNM, ADD, SUB, MUL, DIV, LESS, EQ, GR, AND, OR, NOT, UNDEF
};


enum ObjectType {
	OP, WD
};

const double EPS = 1e-6;


struct Operation {
	OperationType type;
	char symbol;

	Operation() : symbol('#'), type(UNDEF) {}
	Operation(OperationType type);
	Operation(char c);
	Operation(const Operation &operation) : type(operation.type), symbol(operation.symbol) {}

	Operation & operator= (Operation operation);

	static bool is_operation(char c);
	int getPriority() const;
};


class Expression {
protected:
	string polis;
public:
	Expression() : polis("") {}
	Expression(const string &s, const vector<string> & col_names);
	Expression(const Expression &expr) : polis(expr.polis) {}
	double getNumericValue(const vector<double> &variables) const; 
	bool getBoolValue(const vector<double> &variables) const; 
	void print() const;
	string getPolis() const;

	Expression & operator= (const Expression &expr);
	~Expression() {}
protected:
	string addToPolis(const string & tmp, const vector<string> & col_names);
	void updateOperationStack(vector<Operation> &operation_stack, Operation operation);
};

}