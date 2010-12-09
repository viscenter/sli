#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	ifstream in;
	ofstream out;
	string numArg(argv[2]);
	string newfile = "headOut_" + numArg + ".ply";
	in.open(argv[1]);
	if(!in) {
		cout << "Could not open file to remove head" << endl;
		return 1;
	}
	string line;

	getline(in,line);
	while(line != "end_header")
		getline(in,line);

	out.open(newfile.c_str());
	while(getline(in,line))
	{
		out << line;
		out << endl;
	}

	in.close(); out.close();
	return 0;
}
