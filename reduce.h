#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <cstring>

#include "reduce_parser.h"
#include "csv.h"

using namespace std;

struct ReduceAggregateRequest {
	parser::AggregateExpression aggr_expr;
	vector<string> col_to;

	ReduceAggregateRequest(const string & str, const vector<string> & col_names);
};

class ReduceRequest {
	vector<ReduceAggregateRequest> aggr_requests;
	vector<string> keys;
	vector<string> col_names;
	csv::CSVin input;
	csv::CSVout output;
	int n_process;
public:
	ReduceRequest(const string & req_str, const string & keys_str,
				  const string & inname, const string & outname, 
				  char separator = ' ', int n_process = 1);

	void handle();

	~ReduceRequest();
private:
	bool is_key(int n) const;

	bool keys_equal(const vector<double> & data1, const vector<double> & data2) const;

	void threadFunction(vector<double> & data, recursive_mutex & read_lock, 
		recursive_mutex & write_lock, char * error);

	static void startThreadFunction(ReduceRequest * req, vector<double> & data, 
		recursive_mutex & read_lock, recursive_mutex & write_lock, char * error);
};