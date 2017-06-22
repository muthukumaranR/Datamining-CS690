/*
mr0051Normalize.cpp
author:Muthukumaran Ramasubramanian (mr0051@cs.uah.edu)
CS 641:01 Datamining Assignment 1
date : 1/31/17
desc: Gets input from file given following -i command, uses z-score Normalization  
      outputs to the file: 
note: the program assumes that the  number of samples are the same for each attribute
*/

#include <iostream>
#include <fstream>
#include<cmath>
#include<cstdio>
#include<cstdlib>
#include <cstring>
#include <vector>

using namespace std;
vector<string> attribNames;
vector<string> attribType;
string relationName;
int classAttrPos; // class attribute position

int getNoAttributes(ifstream &in, string ClassName);

int getNoSamples(ifstream &in);

float sumArray(float* data, int NoSamples);

float stddevArray(float* data, int NoSamples, float mean);

float* zscorenorm(float* data, int NoSamples, float mean, float stddev);


int getNoAttributes(ifstream &in,string ClassName) {//get number of attributes in relation
	string line = "";
	int num = 0;
	int pos = 0;
	int pos2 = 0;
	int pos3 = 0;
	int rpos = 0;
	string tempAttr = " ";
	while (getline(in, line)) {
		rpos = line.find("@relation");
		if (rpos != line.npos) {					// extract relation name
			relationName = line.substr(rpos + 10);
		}
		pos = line.find("@attribute");
		if (pos != line.npos) {						// find attribute names and store in global vector
			num++;
													
			pos2 = line.find(ClassName);
			if (pos2 != line.npos) {				// TODO: extract class name and type
				classAttrPos = num;
				tempAttr = line.substr(pos2);
				pos3 = tempAttr.find(' ', 0);
				tempAttr = tempAttr.substr(0, pos3);
				attribNames.push_back(tempAttr);
				tempAttr = line.substr(pos2+pos3+1);
				attribType.push_back(tempAttr);

			}
			else {									// TODO:  extract attribute name and type
				tempAttr = line.substr(pos+11);
				pos3 = tempAttr.find(' ', 0);
				tempAttr = tempAttr.substr(pos,pos3);
				attribNames.push_back(tempAttr);
				tempAttr = line.substr(pos + 11 + pos3+1);
				attribType.push_back(tempAttr);
			}
		}

	}
	in.clear();
	in.seekg(0, ios::beg);
	return num;
	
}

int getNoSamples(ifstream &in) {//get number of samples in relation
	string line = "";
	int num = 0;
	while (getline(in, line)) {
		if (strcmp(line.c_str(), "@data") == 0) {
			while (getline(in, line)) {
				num++;
			}
			break;
		}

	}
	in.clear();
	in.seekg(0, ios::beg);
	return num;
}

float sumArray(float* data, int NoSamples) {
	float sum = 0;
	for (int i = 0; i < NoSamples; i++) {
		sum += data[i];
	}
	return sum;
}

float stddevArray(float* data, int NoSamples,float mean) {
	float stddev = 0;
	
	for (int i = 0; i < NoSamples; i++) {
		stddev += pow(data[i] - mean, 2);
	}
	return sqrt(stddev / (NoSamples));

}

float* zscorenorm(float* data, int NoSamples, float mean, float stddev) {
	const int no = NoSamples;
	float* zscore = (float*)malloc(sizeof(float) * NoSamples);
	for (int i = 0; i < NoSamples; i++) {
		zscore[i] = (data[i] - mean) / stddev;
	}
	return zscore;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cout << "usage: " << argv[0] << " -i <filename> -c ClassAttribute \n";
		return 1;
	}
	else
	{
		vector <string> sources;
		string inpFileName;								// input file name
		string outFileName;
		string outFileName2;
		string ClassName;
		ifstream inFile;
		ofstream outFile;
		ofstream outFile2;
		string line = "";
		const int streamsize = 1024;
		int noAttributes;
		int noSamples;
		float zscore;


		for (int i = 1; i < argc; ++i)					// parsing the command line arguements
		{
			if (string(argv[i]) == "-i")
			{
				if (i + 1 < argc)
				{
					inpFileName = argv[i + 1];
					i++;
				}
			}

			else if (string(argv[i]) == "-c")
			{
				if (i + 1 < argc)
				{
					ClassName = argv[i + 1];
					i++;
				}

			}

			else
			{
				sources.push_back(argv[i]);
			}

		}

		inFile.open(inpFileName.c_str());

		if (inFile.fail()) {								// error if file not found
			cout << " file name cannot be found \n";
			system("pause");
			exit(1);
		}

		noSamples = getNoSamples(inFile);
		noAttributes = getNoAttributes(inFile,ClassName);

		float** data = new float*[noAttributes];

		for (int i = 0; i < noAttributes; i++) {
			data[i] = new float[noSamples];
		}

		float** norm_data = new float*[noAttributes];

		for (int i = 0; i < noAttributes; i++) {
			norm_data[i] = new float[noSamples];
		}

		float* mean = new float[noAttributes];

		for (int i = 0; i < noAttributes; i++) {
			norm_data[i] = new float;
		}

		float* stddev = new float[noAttributes];

		for (int i = 0; i < noAttributes; i++) {
			norm_data[i] = new float;
		}

		
		while (getline(inFile, line) ) {
			
			if (strcmp(line.c_str(), "@data") == 0) {		// scan data after "@data" until EOF
				int row = 0;
				while(row < noSamples){
					int col = 0;							// seek by column for the given row
						if(strcmp(line.c_str(), "") != 0){
						do {
							inFile >> data[col][row];
							col++;
						   } while (col < noAttributes);
						row++;
						
						}
						else {								// if broken line occur
						// TODO : skip broken lines
							
						}
				}
			}
		}

		// formatting output in .arff format
		// written to output file named mr0051MeanStd<relationName>.arff
		float sum;
		
		outFileName = "mr0051MeanStd" + relationName + ".arff";
		outFileName2 = "mr0051Normalize" + relationName + ".arff";
		outFile.open(outFileName.c_str());
		outFile2.open(outFileName2.c_str());

		outFile << "@relation " << relationName << endl<<endl;
		outFile2 << "@relation " << relationName << endl << endl;
		for (int i = 0; i < noAttributes; i++) {
			if (i != classAttrPos - 1) {						// if attribute not a class
				outFile << "@attribute " << attribNames[i] << " " << attribType[i]<<endl;
				outFile2 << "@attribute " << attribNames[i] << " " << attribType[i] << endl;

			}
			else {											// if the current attribute is a class
				outFile << "@attribute " << attribNames[i] << " " << attribType[i]<<endl;
				outFile2 << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
			}
			
		}
		outFile << endl;
		outFile << "@data" << endl;
		outFile2 << endl;
		outFile2 << "@data" << endl;


		for (int i = 0; i < noAttributes; i++) {			// loop to calculate mean , stddev, normalized data
			if (i != classAttrPos - 1) {						// if attribute not a class
				sum = sumArray(data[i], noSamples);
				mean[i] = sum / noSamples;
				stddev[i] = stddevArray(data[i], noSamples, mean[i]);
				norm_data[i] = zscorenorm(data[i], noSamples, mean[i], stddev[i]);
				//outFile << "\t" << mean << "\t" << stddev << endl;

			}
			else {											// if the current attribute is a class
				norm_data[i] = data[i];
			}
		}

			for (int i = 0; i < noAttributes; i++) {
				if (i != classAttrPos - 1) {						// if attribute not a class
					//TODO : set precision
					outFile << "\t" << std::fixed<< mean[i];
				}
				else {											// if the current attribute is a class

				}
			}

				outFile << endl;

				for (int i = 0; i < noAttributes; i++) {
					if (i != classAttrPos - 1) {						// if attribute not a class
																		//TODO : set precision
						outFile << "\t" << std::fixed<< stddev[i];
					}
					else {											// if the current attribute is a class

					}
				}

				for (int i = 0; i < noSamples; i++) {
					for (int j = 0; j < noAttributes; j++) {
						outFile2  << "\t" << std::fixed<<norm_data[j][i];
					}
					outFile2 << endl;
				}

			

		
	}
}

