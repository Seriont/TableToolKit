#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cctype>

#include "map.h"

using namespace std;

MapProjectionRequest::MapProjectionRequest(const string & prj_str,
	const vector<string> & col_names) {

	string tmp, cur_str = "";
	bool flag = true;
	istringstream iss(prj_str);
	while (iss >> tmp) {
		if (tmp == "as") {
			if (flag) {
				expr = parser::Expression(cur_str, col_names);
			} else {
				col_to.push_back(cur_str);
			}
			cur_str = "";
			flag = false;
		} else {
			cur_str += tmp;
		}
	}

	if (cur_str != "") {
		if (flag) {
			expr = parser::Expression(cur_str, col_names);
		}
		else 
			col_to.push_back(cur_str);
	} else {
		throw "Empty column name";
	}
	if (!col_to.size()) {
		col_to.push_back(prj_str);
	}
}


MapRequest::MapRequest(const string & prj_str, const string & flt_str,
					   const string &input_filename, 
					   const string & output_filename, char separator) :
					   input(input_filename, separator), 
					   output(output_filename, separator) {
	
	vector<string> col_names = input.getColNames();
	string tmp = "";
	for (int i = 0; i < (int)prj_str.size(); i++) {
		if (prj_str[i] == ',') {
			proj_requests.push_back(MapProjectionRequest(tmp, col_names));
			tmp = "";
		} else {
			tmp += prj_str[i];
		}
	}
	proj_requests.push_back(MapProjectionRequest(tmp, col_names));
	string tmp_flt = flt_str;
	if (flt_str == "")
		tmp_flt = "1";
	filter = parser::Expression(tmp_flt, col_names);
	col_num = (int) col_names.size();
}

void MapRequest::handle() {
	try {

		vector<string> out_col_names;
		for (int i = 0; i < (int)proj_requests.size(); i++) {
			for (int j = 0; j < (int) proj_requests[i].col_to.size(); j++) {
				out_col_names.push_back(proj_requests[i].col_to[j]);
			}
		}
		output.addColNames(out_col_names);

		vector<double> values = input.getRow();
		vector<double> out_cols;
		while ((int)values.size()) {
			if (values.size() != col_num)
				throw "Invalid row in CSV";
			if (filter.getBoolValue(values)) {
				for (int i = 0; i < (int)proj_requests.size(); i++) {
					double result = proj_requests[i].expr.getNumericValue(values);
					for (int j = 0; j < (int) proj_requests[i].col_to.size(); j++) {
						out_cols.push_back(result);
					}
				}
				output.addRow(out_cols);
			}
			values = input.getRow();
			out_cols.resize(0);
		}
	}
	 catch (const char * error_message) {
	 	cout << error_message << endl;
	 }

}

MapRequest::~MapRequest() {
	if (input.is_open())
		input.close();
	if (output.is_open())
		output.close();
}