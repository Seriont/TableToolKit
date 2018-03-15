#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include "csv.h"

using namespace std;

namespace csv {

void CSVout::addColNames(const vector<string> &row) {
	for (int i = 0; i < (int)row.size(); i++) {
		file << row[i];
		if (i != (int) row.size() - 1)
			file << separator;
	}
	file << endl;
}


void CSVout::addRow(const vector<double> &row) {
	for (int i = 0; i < (int)row.size(); i++) {
		file << row[i];
		if (i != (int) row.size() - 1)
			file << separator;
	}
	file << endl;
}


void CSVout::flush() {
	file.flush();
}


vector<string> 
CSVFile::parse(const string &s) const {
	string tmp = "";
	vector<string> result;
	for (int i = 0; i < (int)s.size(); i++) {
		if (s[i] == separator) {
			result.push_back(tmp);
			tmp = "";
		} else {
			tmp += s[i];
		}
	}
	if (tmp != "") {
		result.push_back(tmp);
	}
	return result;
}

vector<string> CSVin::getColNames() {
	char *str = new char[MAX_STR_LEN];
	file.getline(str, MAX_STR_LEN);
	string s = str;
	delete [] str;
	vector<string> result = parse(s);
	return result;
}

vector<double> CSVin::getRow() {
	char *str = new char[MAX_STR_LEN];
	if (!file.getline(str, MAX_STR_LEN)) {
		delete [] str;
		return vector<double>();
	}
	string s = str;
	delete [] str;
	vector<string> row = parse(s);
	vector<double> result;
	for (int i = 0; i < (int)row.size(); i++) {
		if (!isdigit(row[i][0]))
			throw "Invalid element in CSV";
		result.push_back(atof(row[i].c_str()));
	}
	return result;
}

bool CSVFile::is_open() const {
	return file_mode == OPENED;
}

void CSVin::open() {
	if (is_open())
		throw "Trying to reopen a file";
	try {
		file = ifstream(filename);
	}
	catch (...) {
		throw "Cought error while file opening";
	}
	file_mode = OPENED;
}

void CSVout::open() {
	if (is_open())
		throw "Trying to reopen a file";
	try {
		file = ofstream(filename);
	}
	catch (...) {
		throw "Cought error while file opening";
	}
	file_mode = OPENED;
}

void CSVin::close() {
	if (file_mode == OPENED) {
		file.close();
	}
	file_mode = UNDEF;
}

void CSVout::close() {
	if (file_mode == OPENED) {
		file.close();
	}
	file_mode = UNDEF;
}

} // namespace csv