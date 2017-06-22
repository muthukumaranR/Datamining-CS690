//===========================================================================================
//Submission By: muthukumaran Ramasubramanian(mr0051@uah.edu)
//   			 Deepak Kumar Nandagopal (dn0022@uah.edu)
//Dated		   : March 1, 2017
//Source Name  : mr0051dn0022ClusterBasic.cpp
//===========================================================================================
#include <iostream>
#include <fstream>
#include<cmath>
//#include<cstdio.h>
#include<cstdlib>
#include<sstream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <time.h>

using namespace std;

vector<string> attribNames;
vector<string> attribType;
vector<vector<int> > clusterVector(0);
vector<vector<float> > clusterCenterVector(0);
string relationName;
int classAttrPos = -1; // class attribute position
int getNoAttributes(ifstream &in, string ClassName);
int getNoSamples(ifstream &in);
float sumArray(float* data, int NoSamples);
float stddevArray(float* data, int NoSamples, float mean);
float* zscorenorm(float* data, int NoSamples, float mean, float stddev);


int getNoAttributes(ifstream &in, string ClassName) {//get number of attributes in relation
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
			if (pos2 != line.npos && ClassName != "") {				// TODO: extract class name and type
				classAttrPos = num;
				tempAttr = line.substr(pos2);
				pos3 = tempAttr.find(' ', 0);
				tempAttr = tempAttr.substr(0, pos3);
				attribNames.push_back(tempAttr);
				tempAttr = line.substr(pos2 + pos3 + 1);
				attribType.push_back(tempAttr);
			}
			else {									// TODO:  extract attribute name and type
				tempAttr = line.substr(pos + 11);
				pos3 = tempAttr.find(' ', 0);
				tempAttr = tempAttr.substr(pos, pos3);
				attribNames.push_back(tempAttr);
				tempAttr = line.substr(pos + 11 + pos3 + 1);
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


float stddevArray(float** data, int nthAttrib, int NoSamples, float mean) {
	float stddev = 0;
	for (int i = 0; i < NoSamples; i++) {
		stddev += pow(data[i][nthAttrib] - mean, 2);
	}
	return sqrt(stddev / (NoSamples));
}


float** zscorenorm(float** data, int NoSamples, int NoAttributes, float* mean, float* stddev) {
	const int no = NoSamples;
	//int **arr = (int **)malloc(r * sizeof(int *));
	//for (i = 0; i<r; i++)
	//	arr[i] = (int *)malloc(c * sizeof(int));

	float** zscore = (float**)malloc(sizeof(float*) *NoSamples);
	for (int i = 0; i<NoSamples; i++)
		zscore[i] = (float *)malloc(NoAttributes * sizeof(float));
	for (int i = 0; i < NoSamples; i++) {
		for (int j = 0; j < NoAttributes; j++) {
			zscore[i][j] = (data[i][j] - mean[j]) / stddev[j];

		}
	}
	return zscore;
}

float unnorm(float data, float mean, float stddev) {

	float unnorm = 0;

	unnorm = (data *stddev) + mean;

	return unnorm;
}

float distanceCalculate(float* x1, float* y1, int noAttributes)
{
	float dist = 0;
	for (int i = 0; i < noAttributes; i++) {

		//calculating Euclidean distance
		if (i != classAttrPos - 1)
			dist += pow(x1[i] - y1[i], 2);
	}
	dist = sqrt(dist);
	return dist;
}

long stoi(const char *s)
{
	long i;
	i = 0;
	while (*s >= '0' && *s <= '9')
	{
		i = i * 10 + (*s - '0');
		s++;
	}
	return i;
}

float distanceCalculate(vector<float> x1, float* y1, int noAttributes)
{
	float dist = 0;
	for (int i = 0; i < noAttributes; i++) {
		//calculating Euclidean distance
		if (i != classAttrPos - 1) // ignore if class attribute
			dist += pow(x1[i] - y1[i], 2);
	}
	dist = sqrt(dist);
	return dist;
}

float* ClusterAverage(float* ClusterCenterAttribs, float* data, int noAttributes, float occurence) {
	for (int i = 0; i < noAttributes; i++) {
		if (i != classAttrPos - 1) // ignore if class attribute
			ClusterCenterAttribs[i] = (((occurence - 1)*ClusterCenterAttribs[i]) + data[i]) / occurence;
	}
	return ClusterCenterAttribs;
}

float ClusterAverage(float ClusterCenterAttribs, float data, float occurence) {
	ClusterCenterAttribs = (((occurence - 1)*ClusterCenterAttribs) + data) / occurence;
	return ClusterCenterAttribs;
}


void kmeansCluster(float* data[], float * ClusterCenters[], float* PrevClusterCenters[], float * ClusterData, int noAttributes, int noSamples, int k);


void kmeansCluster(float* data[], float * ClusterCenters[], float* PrevClusterCenters[], float * ClusterData, int noAttributes, int noSamples, int k) {
	srand(time(0));
	bool noChange = 0;
	int x;
	float dist;
	float prev_dist = 2000000;
	float minDist = prev_dist;
	vector<int> randgen;
	randgen.reserve(k);
	for (int i = 0; i < k; i++) {// select random points as clusters
		int randExists = 0;
		do {
			x = rand() % (noSamples);
			randExists = 0;
			for (int i = 0; i < randgen.size(); i++) {
				if (x == randgen[i]) {
					randExists = 1;
				}
			}
		} while (randExists == 1);
		randgen.push_back(x);

		for (int j = 0; j < noAttributes; j++) {
			PrevClusterCenters[i][j] = data[x][j];
		}
	}
	int run = 0;
	while (noChange == 0) {// do until no change
		run++;
		//cout << "\n\n\n run: " << run << " \n\n";
		for (int i = 0; i < noSamples; i++) {
			// find least dist to cluster_center from each point and assign cluster
			minDist = 2000000;
			for (int j = 0; j < k; j++) {
				dist = distanceCalculate(data[i], PrevClusterCenters[j], noAttributes);
				if (dist < minDist) {
					minDist = dist;
					ClusterData[i] = j;
				}
			}
			//cout << i << " assigned : " << ClusterData[i] << "\n";
		}
		// update cluster centers
		int occurence = 0;
		for (int j = 0; j < k; j++) {
			occurence = 0;
			//cout << "\n average cluster center of cluster " << j;
			for (int i = 0; i < noSamples; i++) {
				if (ClusterData[i] == j) {
					occurence += 1;
					ClusterCenters[j] = ClusterAverage(ClusterCenters[j], data[i], noAttributes, occurence);
				}
			}
			for (int m = 0; m < noAttributes; m++) {
				//cout << " changed to " << ClusterCenters[j][m] << " from " << PrevClusterCenters[j][m] << " , ";
			}
			//cout << "\n";
		}
		int noChangeFlag = 1;
		for (int i = 0; i < k; i++) { // see if there is change in cluster centers
			for (int j = 0; j < noAttributes; j++) {
				if (ClusterCenters[i][j] != PrevClusterCenters[i][j]) {
					noChangeFlag = 0;
				}
			}
		}
		if (noChangeFlag == 1) {
			noChange = 1;
		}
		if (noChange == 0) { // if there is change then update
			for (int i = 0; i < k; i++) { // see if there is change in cluster centers
				for (int j = 0; j < noAttributes; j++) {
					PrevClusterCenters[i][j] = ClusterCenters[i][j];
					ClusterCenters[i][j] = 0;
				}
			}
		}
	}
}
int main(int argc, char *argv[])
{
	ostringstream ostr;
	if (argc < 3)
	{
		cout << "usage: " << argv[0] << " -i <filename> -c ClassAttribute -K NoOfClusters\n";
		return 1;
	}
	else
	{
		vector <string> sources;
		string inpFileName;								// input file name
		string outFileName;
		string outFileName2;
		string outFileName3;
		string outFileName4;
		string ClassName;
		ifstream inFile;
		ofstream temp_file;
		ofstream outFile;
		ofstream outFile2;
		ofstream outFile3;
		ofstream outFile4;
		string line = "";
		const int streamsize = 1024;
		int noAttributes;
		int noSamples;
		float zscore;
		int K = 0;
		bool NormalizeFlag = 0;
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
			else if (string(argv[i]) == "-K")
			{
				if (i + 1 < argc)
				{
					K = stoi(argv[i + 1]);
					ostr << K;
					i++;
				}
			}
			else if (string(argv[i]) == "-normalize" || string(argv[i]) == "-Normalize")
			{
				NormalizeFlag = 1;

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
		int numc = 0;
		int data_flag = 0;
		outFile.open("temp.txt");  // creating a temp file which is a cpoy of input file , with '\r' removed
		while (getline(inFile, line)) {
			//outFile << "\n";
			numc++;
		}
		//rewind(inFile);
		inFile.open(inpFileName.c_str());
		inFile.clear();
		inFile.seekg(0, ios::beg);
		while (getline(inFile, line)) {
			line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
			outFile << line;

			numc--;
			if (numc != 0) {
				outFile << "\n";
			}
		}
		outFile.close();
		inFile.close();
		inFile.open("temp.txt");
		inFile.clear();
		inFile.seekg(0, ios::beg);
		noSamples = getNoSamples(inFile);
		noAttributes = getNoAttributes(inFile, ClassName);

		float** data = new float*[noSamples];
		for (int i = 0; i < noSamples; i++) {
			data[i] = new float[noAttributes];
		}


		float* mean = new float[noAttributes]();

		float* stddev = new float[noAttributes]();

		float sum = 0;
		while (getline(inFile, line)) {
			if (strcmp(line.c_str(), "@data") == 0) {		// scan data after "@data" until EOF
				int row = 0;
				while (row < noSamples) {
					int col = 0;							// seek by column for the given row
					if (strcmp(line.c_str(), "") != 0) {
						do {
							inFile >> data[row][col];
							// << data[row][col] << " ";
							col++;
						} while (col < noAttributes);
						row++;
						//cout << "\n";
					}
					else {								// if broken line occur
														// TODO : skip broken lines
					}
				}
			}
		}

		for (int i = 0; i < noAttributes; i++) {
			if (i != classAttrPos) {
				sum = 0;
				for (int j = 0; j < noSamples; j++) {			// loop to calculate mean , stddev, normalized data						// if attribute not a class

					sum += data[j][i];
				}
				mean[i] = sum / noSamples;
				stddev[i] = stddevArray(data, i, noSamples, mean[i]);
			}
		}

		float **norm_data = zscorenorm(data, noSamples, noAttributes, mean, stddev);

		inFile.close();

		if (remove("temp.txt") != 0)
			puts("Error deleting file");
		else
			puts("File successfully deleted");


		// Assingment : 2 ::clustering
		float** ClusterCenters = new float*[K];
		for (int i = 0; i < K; i++) {
			ClusterCenters[i] = new float[noAttributes]; //leak
														 //ClusterCenters[i] = {};
		}
		for (int i = 0; i < K; i++) {
			for (int j = 0; j < noAttributes; j++)
				ClusterCenters[i][j] = 0; //leak
										  //ClusterCenters[i] = {};
		}

		float** PrevClusterCenters = new float*[K];
		for (int i = 0; i < K; i++) {
			PrevClusterCenters[i] = new float[noAttributes];
		}

		for (int i = 0; i < K; i++) {
			for (int j = 0; j < noAttributes; j++)
				PrevClusterCenters[i][j] = 0;
		}

		float* ClusterData = new float[noSamples]();


		// formatting output in .arff format
		// written to output file named mr0051dn0022MeanStd<relationName>.arff
		if (NormalizeFlag == 0) {

			outFileName = "mr0051dn0022ClusterCenterBasic" + ostr.str() + relationName + ".arff";
			outFileName2 = "mr0051dn0022ClusteringBasic" + ostr.str() + relationName + ".arff";
			outFile.open(outFileName.c_str());
			outFile2.open(outFileName2.c_str());
			outFile << "@relation " << "ClusterCenter_" << relationName << endl << endl;
			outFile2 << "@relation " << "Clustering_" << relationName << endl << endl;
			for (int i = 0; i < noAttributes; i++) {
				if (i != classAttrPos - 1) {						// if attribute not a class
					outFile << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
					outFile2 << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
				}
				else {											// if the current attribute is a class
					outFile << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
					outFile2 << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
				}
			}
			outFile << "@attribute " << "cluster real" << endl;
			outFile2 << "@attribute " << "cluster real" << endl;
			outFile << endl;
			outFile << "@data" << endl;
			outFile2 << endl;
			outFile2 << "@data" << endl;

			kmeansCluster(data, ClusterCenters, PrevClusterCenters, ClusterData, noAttributes, noSamples, K);

			for (int i = 0; i < K; i++) {//printing cluster centers 
				for (int j = 0; j < noAttributes; j++) {
					outFile << "\t" << std::fixed << ClusterCenters[i][j];
				}
				outFile << "\t" << i + 1;
				if (i < K - 1) {
					outFile << endl;

				}
			}
			outFile.close();

			for (int i = 0; i < noSamples; i++) {
				for (int j = 0; j < noAttributes; j++) {
					outFile2 << "\t" << std::fixed << data[i][j];
				}
				outFile2 << "\t" << std::fixed << (int)ClusterData[i] + 1;
				if (i < noSamples - 1) {
					outFile2 << endl;

				}
			}
			outFile2.close();


			for (int i = 0; i < K; i++) {
				for (int j = 0; j < noAttributes; j++)
					ClusterCenters[i][j] = 0; //leak
											  //ClusterCenters[i] = {};
			}



			//memset(ClusterCenters, 0, sizeof(ClusterCenters[0][0]) * K * noAttributes);



		}

		if (NormalizeFlag == 1) { //if normalize flag is set

			outFileName = "mr0051dn0022ClusterCenterNormalizedBasic" + ostr.str() + relationName + ".arff";
			outFileName2 = "mr0051dn0022ClusteringNormalizedBasic" + ostr.str() + relationName + ".arff";
			outFileName3 = "mr0051dn0022ClusterCenterUnnormalizedBasic" + ostr.str() + relationName + ".arff";
			outFileName4 = "mr0051dn0022ClusteringUnnormalizedBasic" + ostr.str() + relationName + ".arff";
			outFile.open(outFileName.c_str());
			outFile2.open(outFileName2.c_str());
			outFile << "@relation " << "ClusterCenter_" << relationName << endl << endl;
			outFile2 << "@relation " << "Clustering_" << relationName << endl << endl;
			outFile3.open(outFileName3.c_str());
			outFile4.open(outFileName4.c_str());
			outFile3 << "@relation " << "ClusterCenter_" << relationName << endl << endl;
			outFile4 << "@relation " << "Clustering_" << relationName << endl << endl;
			for (int i = 0; i < noAttributes; i++) {
				if (i != classAttrPos - 1) {						// if attribute not a class
					outFile << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
					outFile2 << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
					outFile3 << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
					outFile4 << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
				}
				else {											// if the current attribute is a class
																//outFile << "@attribute " << attribNames[i] << " " << attribType[i] << endl;bisect
					outFile2 << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
					outFile4 << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
				}
			}
			outFile << "@attribute " << "cluster real" << endl;
			outFile2 << "@attribute " << "cluster real" << endl;
			outFile3 << "@attribute " << "cluster real" << endl;
			outFile4 << "@attribute " << "cluster real" << endl;
			outFile << endl;
			outFile << "@data" << endl;
			outFile2 << endl;
			outFile2 << "@data" << endl;
			outFile3 << endl;
			outFile3 << "@data" << endl;
			outFile4 << endl;
			outFile4 << "@data" << endl;

			kmeansCluster(norm_data, ClusterCenters, PrevClusterCenters, ClusterData, noAttributes, noSamples, K);

			for (int i = 0; i < K; i++) {//printing cluster centers 
				for (int j = 0; j < noAttributes; j++) {
					if (j != classAttrPos - 1)
						outFile << "\t" << std::fixed << ClusterCenters[i][j];
					outFile3 << "\t" << std::fixed << unnorm(ClusterCenters[i][j], mean[j], stddev[j]);
				}
				outFile << "\t" << i + 1;
				outFile3 << "\t" << i + 1;
				if (i < K - 1) {
					outFile << endl;
					outFile3 << endl;

				}
			}
			outFile.close();
			outFile3.close();
			for (int i = 0; i < noSamples; i++) {
				for (int j = 0; j < noAttributes; j++) {
					outFile2 << "\t" << std::fixed << norm_data[i][j];
					outFile4 << "\t" << std::fixed << unnorm(norm_data[i][j], mean[j], stddev[j]);
				}
				outFile2 << "\t" << std::fixed << (int)ClusterData[i] + 1;
				outFile4 << "\t" << std::fixed << (int)ClusterData[i] + 1;
				if (i < noSamples - 1) {
					outFile2 << endl;
					outFile4 << endl;

				}
			}
			outFile2.close();
			outFile4.close();



			for (int i = 0; i < K; i++) {
				for (int j = 0; j < noAttributes; j++)
					ClusterCenters[i][j] = 0; //leak
											  //ClusterCenters[i] = {};
			}

			//memset(ClusterCenters, 0, sizeof(ClusterCenters[0][0]) * K * noAttributes);
		}

		clusterVector.clear();
		clusterCenterVector.clear();
		for (int i = 0; i<noSamples; i++) {
			delete[] data[i];
			free(norm_data[i]);
		}

		for (int i = 0; i<K; i++) {

			delete[] ClusterCenters[i];
			delete[] PrevClusterCenters[i];
		}
		delete[] ClusterCenters;
		delete[] PrevClusterCenters;
		delete[] ClusterData;
		delete[] data;
		free(norm_data);
		delete[] mean;
		delete[] stddev;
		cout << "\n==================================================================";
		cout << "\n program completed and files are generated.";
		cout << "\n==================================================================\n\n\n\n\n";
		cout << "\n==================================================================\n";
	}

}
