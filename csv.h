#include <string>
#include <fstream>
#include <vector>

using namespace std;

namespace csv {

const int BLOCK_SIZE = 1000;
const int MAX_STR_LEN = 100000;

enum Mode {
	OPENED,
	UNDEF
};

class CSVFile {
protected:
	string filename;
	Mode file_mode;
	char separator;
public:
	CSVFile(const string & name, char separator) : 
		filename(name), file_mode(UNDEF), separator(separator) {}

	bool is_open() const;

	~CSVFile() {}
protected:
	vector<string> parse(const string &s) const;
};

class CSVin : public CSVFile {
	ifstream file;
public:
	CSVin(const string & s, char c = ' ') : CSVFile(s, c) {
		open();
	}

	void close();

	void open();

	vector<double> getRow();
	vector<string> getColNames();
};

class CSVout : public CSVFile {
	ofstream file;
public:
	CSVout(const string & s, char c = ' ') : CSVFile(s, c) {
		open();
	}

	void open();

	void close();

	void flush();

	void addColNames(const vector<string> &row);
	void addRow(const vector<double> &row);
};
}