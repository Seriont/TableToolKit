#include <sstream>

#include "reduce.h"

using namespace std;


int main(int argc, char **argv) {
	char separator = ' ';
	string keys_str = "", aggr_str = "";
	string inname = "", outname = "";
	int n_process = 1;
	if (argc == 2 && string(argv[1]) == "--help") {
		cout << endl << "Make a map request using a command:" << endl;
		cout << "./csvreduce \n--sep='<separator>' \n--keys='<key names>' \n";
		cout << "--aggrs='<aggregation expression>' \n--nprocess=<process number>\n";
		cout << "<input csv filename> <output csv filename>";
		cout << endl << endl << "For detailed information see README.txt" << endl; 
		return 0;
	}
	string tmp = "", tmp1 = "";
	try {
	for (int i = 1; i < argc; i++) {
		string arg = argv[i];
		int j = 0;
		while (j < (int) arg.size() && arg[j] != '=') 
			tmp += arg[j++];

		if (arg[j++] == '=') {
			while (j < (int) arg.size())
				tmp1 += arg[j++];
		}
		if (tmp == "--sep") {
			if ((int)tmp1.size() != 1)
				throw "Invalid command line args";
			separator = tmp1[0];
		} else if (tmp == "--keys") {
			keys_str = tmp1;
		} else if (tmp == "--aggrs") {
			aggr_str = tmp1;
		} else if (tmp == "--nprocess") {
			istringstream iss(tmp1);
			iss >> n_process;
			if (iss >> tmp)
				throw "Invalid command line args";
		} else if (i == argc - 2) {
			inname = tmp;
		} else if (i == argc - 1) {
			outname = tmp;
		} else {
			throw "Invalid command line args";
		}
		tmp = "";
		tmp1 = "";
	}
	if (aggr_str == "" || keys_str == "" || inname == "" || outname == "") 
		throw "Invalid command line arguments";
	ReduceRequest(aggr_str, keys_str, inname, outname, separator, n_process).handle();
	} catch (const char * error) {
		cout << error << endl;
	}
	return 0;
}