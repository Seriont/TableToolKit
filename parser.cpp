#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>
#include <cstdlib>

#include "parser.h"

using namespace std;

namespace parser {


void Expression::print() const {
	cout << polis << endl;
}


void Expression::updateOperationStack(vector<Operation> &operation_stack, Operation operation) {
	if (operation.type == RB) {
		Operation cur;
		do {
			if (operation_stack.empty())
				throw "Invalid bracket sequence";
			cur = operation_stack[(int) operation_stack.size() - 1];
			operation_stack.pop_back();
			if (cur.type != LB) {
				polis += cur.symbol;
				polis += " ";
			}
		} while (cur.type != LB);
	} else {
		Operation cur;
		while (!operation_stack.empty()) {
			cur = operation_stack[(int) operation_stack.size() - 1];
			if (cur.getPriority() < operation.getPriority() || cur.type == LB) {
				break;
			} else {
				polis += cur.symbol;
				polis += " ";
				operation_stack.pop_back();
			}
		}
		operation_stack.push_back(operation);
	}
}


string Expression::addToPolis(const string & tmp, const vector<string> & col_names) {
	if (isdigit(tmp[0])) {
		return tmp + " ";
	}
	for (int i = 0; i < (int) col_names.size(); i++) {
		if (tmp == col_names[i]) {
			ostringstream oss;
			oss << i;
			return "#" + oss.str() + " ";
		}
	}
	throw "Invalid variable";
}


Expression::Expression(const string &s, const vector<string> & col_names) : polis("") {
	string tmp = "";
	vector<Operation> operation_stack;
	ObjectType flag = OP;
	//cout << s << endl;
	for (int i = 0; i < (int)s.size(); i++) {
		if (s[i] == ' ') {
			if (tmp == "!" || tmp == "&" || tmp == "|" || tmp == "~")
				throw "Invalid expression";
			if (tmp == "and") {
				updateOperationStack(operation_stack, Operation(AND));
			} else if (tmp == "or") {
				updateOperationStack(operation_stack, Operation(OR));
			} else if (tmp == "not") {
				updateOperationStack(operation_stack, Operation(NOT));
			} else if (tmp == "") {
				continue;
			} else {
				if (flag == WD) 
					throw "Invalid Expression";
				polis += addToPolis(tmp, col_names);
				tmp = "";
				flag = WD;
				continue;
			}
			flag = OP;
			tmp = "";
		} else if (Operation::is_operation(s[i])) {
			if (tmp != "") {
				if (flag == WD)
					throw "Invalid Expression";
				polis += addToPolis(tmp, col_names);
				flag = WD;
				tmp = "";
			}
			if (flag == OP && s[i] == '-') {
				updateOperationStack(operation_stack, Operation(UNM));
				continue;
			} 
			Operation operation(s[i]);
			updateOperationStack(operation_stack, operation);
			if (operation.type != LB && operation.type != RB) {
				if (flag == OP)
					throw "Invalid Expression";
				flag = OP;
			}
		} else {
			tmp += s[i];
		}
	}
	if (tmp != "") {
		polis += addToPolis(tmp, col_names);
	}
	while (!operation_stack.empty()) {
		Operation operation = operation_stack[operation_stack.size() - 1];
		if (operation.type == LB) 
			throw "Invalid bracket sequence";
		operation_stack.pop_back();
		polis += operation.symbol;
		polis += " ";
	}
	if (!operation_stack.empty())
		throw "Invalid expression";
	if (polis == "")
		throw "Empty expression"; 
}


double Expression::getNumericValue(const vector<double> &variables) const {
	istringstream iss(polis);
	string tmp;
	vector<double> values;
	while (iss >> tmp) {
		if (tmp.size() == 1 && Operation::is_operation(tmp[0])) {
			Operation operation(tmp[0]);
			double rvalue;
			if (values.empty())
				throw "Invalid Expression";
			double value1 = values[(int)values.size() - 1];
			values.pop_back();
			if (operation.type == UNM) {
				rvalue = -value1;
				values.push_back(rvalue);
				continue;
			} else if (operation.type == NOT) {
				rvalue = !value1;
				values.push_back(rvalue);
				continue;
			}
			if (values.empty())
				throw "Invalid Expression";
			double value2 = values[(int)values.size() - 1];
			values.pop_back();
			switch (operation.type) {
			case ADD:
				rvalue = value1 + value2;
				break;
			case SUB:
				rvalue = value2 - value1;
				break;
			case MUL:
				rvalue = value1 * value2;
				break;
			case DIV:
				if (abs(value1) < EPS) 
					throw "Zero division";
				rvalue = value2 / value1;
				break;
			case POW:
				try {
					rvalue = pow(value2, value1);
				}
				catch(...) {
					throw "Invalid pow";
				}
				break;
			case LESS:
				rvalue = value2 < value1;
				break;
			case EQ:
				rvalue = value1 == value2;
				break;
			case GR:
				rvalue = value2 > value1;
				break;
			case AND:
				rvalue = value2 && value1;
				break;
			case OR:
				rvalue = value2 || value1;
				break;
			}
			values.push_back(rvalue);
		} else if (tmp[0] == '#') {
			string col_num = "";
			for (int i = 1; i < (int) tmp.size(); i++) {
				col_num += tmp[i];
			}
			values.push_back(variables[atoi(col_num.c_str())]);
		} else if (isdigit(tmp[0])) {
			istringstream tmp_iss(tmp);
			double rvalue;
			tmp_iss >> rvalue;
			string check;
			if (tmp_iss >> check) 
				throw "Unknown element";
			values.push_back(rvalue);
		} else {
			throw "Unknown element";
		}
	}
}


bool Expression::getBoolValue(const vector<double> &variables) const {
	return (bool) getNumericValue(variables);
}

Expression & Expression::operator= (const Expression & expr) {
	if (this != &expr)
		polis = expr.polis;
	return *this;
}


string Expression::getPolis() const {
	return polis;
}


Operation & Operation::operator=(Operation operation) {
	type = operation.type;
	symbol = operation.symbol;
	return *this;
}

Operation::Operation(OperationType type) : type(type) {
	switch (type) {
	case UNM:
		symbol = '~';
		break;
	case LB:
		symbol = '(';
		break;
	case RB:
		symbol = ')';
		break;
	case ADD:
		symbol = '+';
		break;
	case SUB:
		symbol = '-';
		break;
	case MUL:
		symbol = '*';
		break;
	case DIV:
		symbol = '/';
		break;
	case POW:
		symbol = '^';
		break;
	case LESS:
		symbol = '<';
		break;
	case EQ:
		symbol = '=';
		break;
	case GR:
		symbol = '>';
		break;
	case NOT:
		symbol = '!';
		break;
	case AND:
		symbol = '&';
		break;
	case OR:
		symbol = '|';
		break;
	}
}

Operation::Operation(char c) {
	symbol = c;
	switch(c) {
		case '~':
			type = UNM;
			break;
		case '+':
			type = ADD;
			break;
		case '^':
			type = POW;
			break;
		case '-':
			type = SUB;
			break;
		case '*':
			type = MUL;
			break;
		case '/':
			type = DIV;
			break;
		case '(':
			type = LB;
			break;
		case ')':
			type = RB;
			break;
		case '<':
			type = LESS;
			break;
		case '=':
			type = EQ;
			break;
		case '>':
			type = GR;
			break;
		case '!':
			type = NOT;
			break;
		case '|':
			type = OR;
			break;
		case '&':
			type = AND;
			break;
	}
}

bool Operation::is_operation(char c) {
	switch(c) {
		case '+':
		case '-':
		case '^':
		case '*':
		case '/':
		case '(':
		case ')':
		case '~':
		case '<':
		case '=':
		case '>':
		case '!':
		case '|':
		case '&':
			return true;
		default:
			return false;
	}
}

int Operation::getPriority() const {
	switch (type) {
		case OR:
		return 1;
		case AND:
		return 2;
		case NOT:
		return 3;
		case EQ: case LESS: case GR:
		return 4;
		case ADD: case SUB:
		return 5;
		case MUL: case DIV:
		return 6;
		case UNM:
		return 7;
		case POW:
		return 8;
		case LB: case RB:
		return 9;
	}
}


} // namespace parser
