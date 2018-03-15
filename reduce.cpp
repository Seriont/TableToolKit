#include "reduce.h"


ReduceAggregateRequest::ReduceAggregateRequest(const string & str, const vector<string> & col_names) {
	string tmp, cur_str = "";
	bool flag = true;
	istringstream iss(str);
	while (iss >> tmp) {
		if (tmp == "as") {
			if (flag) {
				aggr_expr = parser::AggregateExpression(cur_str, col_names);
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
		if (flag)
			aggr_expr = parser::AggregateExpression(cur_str, col_names);
		else 
			col_to.push_back(cur_str);
	} else {
		throw "Empty column name";
	}
	if (!col_to.size()) {
		col_to.push_back(str);
	}
}


ReduceRequest::ReduceRequest(const string & req_str, const string & keys_str,
	const string & inname, const string & outname, char separator, int n_process) : 
	input(inname, separator), output(outname, separator), n_process(n_process) {

	col_names = input.getColNames();
	string tmp = "";
	for (int i = 0; i < (int)req_str.size(); i++) {
		if (req_str[i] == ',') {
			aggr_requests.push_back(ReduceAggregateRequest(tmp, col_names));
			tmp = "";
		} else {
			tmp += req_str[i];
		}
	}
	if (tmp != "")
		aggr_requests.push_back(ReduceAggregateRequest(tmp, col_names));
	
	tmp = "";
	for (int i = 0; i < (int)keys_str.size(); i++) {
		if (keys_str[i] == ' ')
			continue;
		if (keys_str[i] == ',') {
			keys.push_back(tmp);
			tmp = "";
		} else {
			tmp += keys_str[i];
		}
	}
	if (tmp != "")
		keys.push_back(tmp);
}
							 
							 
ReduceRequest::~ReduceRequest() {
	if (input.is_open()) 
		input.close();
	if (output.is_open())
		output.close();
}


bool ReduceRequest::is_key(int n) const {
	for (int i = 0; i < (int)keys.size(); i++) {
		if (keys[i] == col_names[n])
			return true;
	}
	return false;
}


bool ReduceRequest::keys_equal(const vector<double> & data1, 
	const vector<double> & data2) const {

	for (int i = 0; i < (int) col_names.size(); i++) {

		if (is_key(i) && data1[i] != data2[i])
			return false;
	}
	return true;
}


void ReduceRequest::threadFunction(vector<double> & data, recursive_mutex & read_lock, 
	recursive_mutex & write_lock, char * error) {

	while (true) {
		read_lock.lock();
		if (!data.size()) {
			read_lock.unlock();
			break;
		}

		if (data.size() != col_names.size()) {
			strcpy(error, "Invalid file");
			data.resize(0);
			read_lock.unlock();
			break;
		}
		vector<double> cur_values = data;
		vector< vector<double> > buffer;
		for (int i = 0; i < (int) data.size(); i++) {
			buffer.push_back(vector<double>());
		}
 		
		while (keys_equal(cur_values, data)) {
			for (int i = 0; i < (int) data.size(); i++)
				buffer[i].push_back(data[i]);
			cur_values = data;
			data = input.getRow();
			if (!data.size()) 
				break;
			if (data.size() != col_names.size()) {
				strcpy(error, "Invalid file");
				data.resize(0);
				read_lock.unlock();
				return;
			}
		}
		read_lock.unlock();

		vector<double> result;
		for (int i = 0; i < (int)col_names.size(); i++) {
			if (is_key(i)) {
				result.push_back(buffer[i][0]);
			}
		}
		for (int i = 0; i < (int) aggr_requests.size(); i++) {
			double value;
			try {
				value = aggr_requests[i].aggr_expr.getValue(buffer);
			} catch (const char * message){
				strcpy(error, message);
				return;
			}
			for (int j = 0; j < (int) aggr_requests[i].col_to.size(); j++) {
				result.push_back(value);
			}
		}
		write_lock.lock();
		output.addRow(result);
		//cout << this_thread::get_id() << endl;
		//cout.flush();
		output.flush();
		write_lock.unlock();
		if (!data.size())
			break;
	}
}


void ReduceRequest::startThreadFunction(ReduceRequest * req, 
	vector<double> & data, recursive_mutex & read_lock, 
	recursive_mutex & write_lock, char * error) {

	req->threadFunction(data, read_lock, write_lock, error);
}


void ReduceRequest::handle() {
	vector<string> out_col_names;
	for (int i = 0; i < (int)col_names.size(); i++) {
		if (is_key(i)) {
			out_col_names.push_back(col_names[i]);

		}
	}

	for (int i = 0; i < (int) aggr_requests.size(); i++) {
		for (int j = 0; j < (int) aggr_requests[i].col_to.size(); j++) {
			out_col_names.push_back(aggr_requests[i].col_to[j]);
		}
	}
	output.addColNames(out_col_names);

	// start handling

	vector<double> data = input.getRow();
	recursive_mutex read_lock, write_lock;
	vector<thread> threads;
	char *error = new char[50];
	error[0] = '\0';
	for (int i = 0; i < n_process; i++) {
		threads.emplace_back(ReduceRequest::startThreadFunction, this, ref(data), 
			ref(read_lock), ref(write_lock), error);
	}
	for (int i = 0; i < n_process; i++) {
		threads[i].join();
	}

	if (strlen(error))
		throw error;
}