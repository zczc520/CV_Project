#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

int main()
{
	ifstream fsin;
	fsin.open("neg.txt");
	ofstream fsout;
	fsout.open("negcopy.txt");

	string linestr;
	while (getline(fsin, linestr))
	{
		stringstream ss(linestr);
		string filename;
		ss >> filename;
		string output = "neg/"+ filename + "\n";
		fsout << output;
	}

	/*ifstream fsin;
	fsin.open("pos.txt");
	ofstream fsout;
	fsout.open("poscopy.txt");

	string linestr;
	while (getline(fsin, linestr))
	{
		stringstream ss(linestr);
		string filename;
		ss >> filename;
		string output = filename + " 1 0 0 24 24\n";
		fsout << output;
	}*/

	return 0;
}