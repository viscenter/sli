#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	ifstream in;
	ofstream out;
	in.open(argv[1]);
	out.open(argv[2]);
	if(!in) {
		cout << "Could not open file to remove head" << endl;
		return 1;
	}
	string line;

	getline(in,line);
	while(line != "end_header")
		getline(in,line);


	while(getline(in,line))
	{
		out << line;
		out << endl;
	}

	in.close(); out.close();
	return 0;
}
