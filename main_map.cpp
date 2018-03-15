#include <iostream>
#include <string>
#include <cstdio>

#include "map.h"

using namespace std;

int main(int argc, char **argv) {
	char sep = ' ';
	string flt = "";
	string prj = "";
	string inname = "", outname = "";
	string tmp, tmp1 = "", tmp2 = "", arg = "";
	char key[10];
	char expr[1000];
	if (argc == 2) {
		tmp = argv[1];
		if (tmp == "--help") {
			cout << endl << "Make a map request using a command:" << endl;
			cout << "./csvmap \n--sep='<separator>' \n--select='<arithmetic expression>' \n";
			cout << "--filter='<logical expression>' \n";
			cout << "<input csv filename> <output csv filename>";
			cout << endl << endl << "For detailed information see README.txt" << endl; 
			return 0;
		}
	}
	try {
	for (int i = 1; i < argc; i++) {
		tmp = argv[i];
		int j;
		for (j = 0; j < (int)tmp.size(); j++) {
			if (tmp[j] == '=') {
				break;
			}
			tmp1 += tmp[j];
		}
		j++;
		
		while (j < (int)tmp.size()) {
			tmp2 += tmp[j];
			j++;
		}
		if (tmp1 == "--sep" && (int)tmp2.size() == 1) {
			sep = tmp2[0];
		} else if (tmp1 == "--select") {
			prj = tmp2;
		} else if (tmp1 == "--filter") {
			flt = tmp2;
		} else if (i == argc - 2) {
			inname = tmp1;
		} else if (i == argc - 1) {
			outname = tmp1;
		} else {
			throw "Invalid command line arguments";
		}
		tmp1 = "";
		tmp2 = "";
	}
	if (prj == "" || inname == "" || outname == "")
		throw "Invalid command line arguments";
	MapRequest(prj, flt, inname, outname, sep).handle();
	} catch(const char *error) {
		cout << error << endl;
	} catch (char *error) {
		cout << error << endl;
		delete [] error;
	}

	return 0;
}