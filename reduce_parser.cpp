#include "reduce_parser.h"

namespace parser {


AggregateExpression::AggregateExpression(const string & str, const vector<string> & col_names) {
	string tmp = "";
	string result = "";
	bool flag = false;
	for (int i = 0; i < (int)str.size(); i++) {
		if (str[i] == ' ') {
			continue;
		} else if (str[i] == '(') {
			if (tmp == "min") {
				tmp = "m";
				flag = true;
			} else if (tmp == "max") {
				tmp = "M";
				flag = true;
			} else if (tmp == "avg") {
				tmp = "a";
				flag = true;
			} else if (tmp != "") {
				throw "Invalid aggregate expression";
			} else {
				result += '(';
			}
		} else if (str[i] == ')') {
			if (tmp != "") {
				if (flag) {
					if (isdigit(tmp[0]))
						throw "Invalid min/max/avg argument";
					result += tmp;
					tmp = "";
				} else {
					if (!isdigit(tmp[0]))
						throw "Invalid aggregate expression";
					result += tmp;
					tmp = "";
				}
			}
			tmp = "";
			if (flag) {
				flag = false;
			} else {
				result += ')';
			}
		} else if (parser::Operation::is_operation(str[i])) {
			if (flag) {
				throw "Invalid max/min/avg argument";
			} else {
				if (tmp != "") {
					if (!isdigit(tmp[0]))
						throw "Invalid aggregate expression";
					result += tmp;
					tmp = "";
				} 
				result += str[i];
			}
		} else {
			tmp += str[i];
		}
	}
	if (tmp != "") {
		if (!isdigit(tmp[0]))
			throw "Invalid aggregate expression";
		result += tmp;
	}
	vector<string> new_col_names;
	for (int i = 0; i < (int) col_names.size(); i++) {
		new_col_names.push_back("a" + col_names[i]);
		new_col_names.push_back("m" + col_names[i]);
		new_col_names.push_back("M" + col_names[i]);
	}
	expression = Expression(result, new_col_names);
}

AggregateExpression::AggregateExpression(const AggregateExpression & aggr_expr)
	: expression(aggr_expr.expression) {}


void AggregateExpression::print() const {
	cout << expression.getPolis() << endl;
}

double AggregateExpression::getValue(
	const vector< vector<double> > & values) {

	istringstream iss(expression.getPolis());
	string value;
	vector<double> vars((int)values.size() * 3, 0);
	vector<bool> flags((int)values.size(), false);
	while (iss >> value) {
		if (value[0] == '#') {
			int n;
			string tmp = "";
			for (int i = 1; i < (int)value.size(); i++) {
				tmp += value[i];
			}
			n = atoi(tmp.c_str());
			if (flags[n])
				continue;
			double minv = values[n / 3][0];
			double maxv = minv;
			double sumv = minv;
			int i;
			for (i = 1; i < (int) values[0].size(); i++) {
				double cur_val = values[n / 3][i];
				if (cur_val > maxv)
					maxv = cur_val;
				if (cur_val < minv) 
					minv = cur_val;
				sumv += cur_val;
			}
			vars[(n / 3) * 3] = sumv / (double) i;
			vars[(n / 3) * 3 + 1] = minv;
			vars[(n / 3) * 3 + 2] = maxv;
			flags[n / 3] = true;
		}
	}

	return expression.getNumericValue(vars);
}

} // namespace parser