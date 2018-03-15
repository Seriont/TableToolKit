#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cctype>

#include "parser.h"
#include "csv.h"

using namespace std;

struct MapProjectionRequest {
	parser::Expression expr;
	vector<string> col_to;

	MapProjectionRequest(const string & prj_str, const vector<string> & col_names);
};

struct MapRequest {
	vector<MapProjectionRequest> proj_requests;
	parser::Expression filter;
	csv::CSVin input;
	csv::CSVout output;
	int col_num;

	MapRequest(const string & prj_str, const string & flt_str, 
			   const string &input_file, const string & output_filename,
			   char separator = ' ');

	void handle();

	~MapRequest();
};