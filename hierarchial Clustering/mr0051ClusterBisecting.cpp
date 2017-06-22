//===========================================================================================
//Submission By: muthukumaran Ramasubramanian(mr0051@uah.edu)
//   			 Deepak Kumar Nandagopal (dn0022@uah.edu)
//Dated		   : March 1, 2017
//Source Name  : mr0051dn0022ClusterBisecting.cpp
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



void kmeansBisectingCluster(float* data[], float * ClusterCenters[], float* PrevClusterCenters[], float * ClusterData, int noAttributes, int noSamples, int k) {
	int noClusters = 1;
	float dist;
	float max_dist;
	int occurence = 0;
	int splitCluster;

	vector<int> clusterIDvector;
	vector<float> clusterCenter;
	vector<int> clusterIDvector0;
	vector<int> clusterIDvector1;
	vector<float> clusterCenter0;
	vector<float> clusterCenter1;
	int Clocation;
	for (int i = 0; i < noSamples; i++)
	{
		try {
			clusterIDvector.push_back((int)i);
		}
		catch (int e) {

		}
	}
	clusterVector.push_back(clusterIDvector);
	clusterIDvector.clear();
	//vector<int> Clustermembers = 
	for (size_t i = 0; i < noAttributes; i++)
	{
		try {
			clusterCenter.push_back(0);
		}
		catch (int e) {

		}
	}
	clusterCenterVector.push_back(clusterCenter);
	for (int cl = 0; cl < clusterVector.size(); cl++) {     // find cluster centroid initially
		for (int i = 0; i < clusterVector[cl].size(); i++) {
			for (size_t j = 0; j < noAttributes; j++)
			{
				if (j != classAttrPos - 1)                    // if attribute is a class, then do not take its average.
					clusterCenterVector[cl][j] = ClusterAverage(clusterCenterVector[cl][j], data[i][j], i + 1);
			}
		}
	}
	do {

		max_dist = 0;
		for (int cl = 0; cl < clusterVector.size(); cl++) {// for each cluster find the max SSE cluster
			dist = 0;
			for (int i = 0; i < clusterVector[cl].size(); i++) {
				for (int j = 0; j < noSamples; j++) {
					if (j == clusterVector[cl][i])
						dist += distanceCalculate(clusterCenterVector[cl], data[j], noAttributes);
				}
			}
			if (max_dist < dist) {
				max_dist = dist;
				splitCluster = cl;
			}
		}// splitcluster is the cluster with largest SSE
		 // take the cluster marked by splitcluster and do k-means on the data
		float** Cdata = new float*[clusterVector[splitCluster].size()]; // new kmeans data
		for (int i = 0; i < clusterVector[splitCluster].size(); i++) {
			Cdata[i] = new float[noAttributes];
		}
		for (int i = 0; i < clusterVector[splitCluster].size(); i++) {  // get data with 'splitcluster'
			for (int j = 0; j < noAttributes; j++)
				Cdata[i][j] = data[clusterVector[splitCluster][i]][j];
		}
		float** CClusterCenters = new float*[2]; // new 2 clustercenter (bisecting)
		for (int i = 0; i < 2; i++) {
			CClusterCenters[i] = new float[noAttributes]();
		}
		float** CPrevClusterCenters = new float*[2]; // new 2 clustercenter (bisecting)
		for (int i = 0; i < 2; i++) {
			CPrevClusterCenters[i] = new float[noAttributes]();
		}
		float* CClusterData = new float[clusterVector[splitCluster].size()](); // new clusterdata, always starts with 0
		for (int i = 0; i < 1; i++) {
			kmeansCluster(Cdata, CClusterCenters, CPrevClusterCenters, CClusterData, noAttributes, clusterVector[splitCluster].size(), 2);
		}
		// pop out the cluster that is to be split using CClusterData info and insert 2 new clusters in place
		for (int i = 0; i < clusterVector[splitCluster].size(); i++)
		{
			for (int j = 0; j < noSamples; j++)
			{
				if (distanceCalculate(Cdata[i], data[j], noAttributes) == 0) {
					Clocation = j;
					if (CClusterData[i] == 0) {
						try {
							clusterIDvector0.push_back((int)Clocation);
						}
						catch (int e) {

						}
					}
					else {
						try {
							clusterIDvector1.push_back((int)Clocation);
						}
						catch (int e) {

						}
					}
				}
			}
		}
		for (int i = 0; i<clusterVector[splitCluster].size(); i++) {
			delete[] Cdata[i];

		}
		delete[] Cdata;
		clusterVector.push_back(clusterIDvector0);
		clusterVector.push_back(clusterIDvector1);
		clusterIDvector0.clear();
		clusterIDvector1.clear();
		clusterVector.erase(clusterVector.begin() + splitCluster);

		for (int i = 0; i < noAttributes; i++) {
			try {
				clusterCenter0.push_back(CClusterCenters[0][i]);
				clusterCenter1.push_back(CClusterCenters[1][i]);
			}
			catch (int e) {

			}
		}
		for (int i = 0; i<2; i++) {
			delete[] CClusterCenters[i];
			delete[] CPrevClusterCenters[i];
		}
		delete[] CClusterCenters;
		delete[] CPrevClusterCenters;
		delete[] CClusterData;
		clusterCenterVector.push_back(clusterCenter0);
		clusterCenterVector.push_back(clusterCenter1);
		clusterCenter0.clear();
		clusterCenter1.clear();
		clusterCenterVector.erase(clusterCenterVector.begin() + splitCluster);
		noClusters++;


	} while (noClusters < k);
}
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

			


			outFileName = "mr0051dn0022ClusterCenterBisecting" + ostr.str() + relationName + ".arff";
			outFileName2 = "mr0051dn0022ClusteringBisecting" + ostr.str() + relationName + ".arff";
			outFile.open(outFileName.c_str());
			outFile2.open(outFileName2.c_str());
			outFile.clear();
			outFile2.clear();
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

			kmeansBisectingCluster(data, ClusterCenters, PrevClusterCenters, ClusterData, noAttributes, noSamples, K);

			for (int i = 0; i < clusterCenterVector.size(); i++) {//printing cluster centers 
				for (int j = 0; j < noAttributes; j++) {
					outFile << "\t" << std::fixed << clusterCenterVector[i][j];
				}
				outFile << "\t" << i + 1;
				if (i < clusterCenterVector.size() - 1) {
					outFile << endl;

				}
			}
			outFile.close();
			int cluster = -1;
			for (int i = 0; i < noSamples; i++) {
				for (int j = 0; j < clusterVector.size(); j++) {
					for (int k = 0; k < clusterVector[j].size(); k++) {
						if (distanceCalculate(data[i], data[clusterVector[j][k]], noAttributes) == 0) {
							cluster = j;
							for (int l = 0; l < noAttributes; l++) {
								outFile2 << "\t" << data[i][l];
							}

						}
					}


				}
				outFile2 << "\t" << cluster + 1;
				if (i < noSamples - 1) {
					outFile2 << endl;
				}
			}
			outFile2.close();

		}

		if (NormalizeFlag == 1) { //if normalize flag is set


			outFileName = "mr0051dn0022ClusterCenterNormalizedBisecting" + ostr.str() + relationName + ".arff";
			outFileName2 = "mr0051dn0022ClusteringNormalizedBisecting" + ostr.str() + relationName + ".arff";
			outFileName3 = "mr0051dn0022ClusterCenterUnnormalizedBisecting" + ostr.str() + relationName + ".arff";
			outFileName4 = "mr0051dn0022ClusteringUnnormalizedBisecting" + ostr.str() + relationName + ".arff";
			outFile.open(outFileName.c_str());
			outFile2.open(outFileName2.c_str());
			outFile3.open(outFileName3.c_str());
			outFile4.open(outFileName4.c_str());
			outFile.clear();
			outFile2.clear();
			outFile << "@relation " << "ClusterCenter_" << relationName << endl << endl;
			outFile2 << "@relation " << "Clustering_" << relationName << endl << endl;
			outFile3.clear();
			outFile4.clear();
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
																//outFile << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
					outFile2 << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
					outFile4 << "@attribute " << attribNames[i] << " " << attribType[i] << endl;
				}
			}
			outFile << "@attribute " << "cluster real" << endl;
			outFile2 << "@attribute " << "cluster real" << endl;
			outFile << endl;
			outFile << "@data" << endl;
			outFile2 << endl;
			outFile2 << "@data" << endl;
			outFile3 << "@attribute " << "cluster real" << endl;
			outFile4 << "@attribute " << "cluster real" << endl;
			outFile3 << endl;
			outFile3 << "@data" << endl;
			outFile4 << endl;
			outFile4 << "@data" << endl;
			kmeansBisectingCluster(norm_data, ClusterCenters, PrevClusterCenters, ClusterData, noAttributes, noSamples, K);

			//vector<vector<int>> clusterVector(0);
			//vector<vector<float>> clusterCenterVector(0);

			for (int i = 0; i < clusterCenterVector.size(); i++) {//printing cluster centers 
				for (int j = 0; j < noAttributes; j++) {
					if (j != classAttrPos - 1) {
						outFile << "\t" << std::fixed << clusterCenterVector[i][j];
						outFile3 << "\t" << std::fixed << unnorm(clusterCenterVector[i][j], mean[j], stddev[j]);

					}
				}
				outFile << "\t" << i + 1;
				outFile3 << "\t" << i + 1;
				if (i < clusterCenterVector.size() - 1) {
					outFile << endl;
					outFile3 << endl;

				}
			}
			outFile.close();
			outFile3.close();
			int cluster = -1;
			for (int i = 0; i < noSamples; i++) {
				for (int j = 0; j < clusterVector.size(); j++) {
					for (int k = 0; k < clusterVector[j].size(); k++) {
						if (distanceCalculate(norm_data[i], norm_data[clusterVector[j][k]], noAttributes) == 0) {
							cluster = j;
							for (int l = 0; l < noAttributes; l++) {
								outFile2 << "\t" << norm_data[i][l];
								outFile4 << "\t" << unnorm(norm_data[i][l], mean[l], stddev[l]);
							}

						}
					}


				}
				outFile2 << "\t" << cluster + 1;
				outFile4 << "\t" << cluster + 1;
				if (i < noSamples - 1) {
					outFile2 << endl;
					outFile4 << endl;
				}
			}
			outFile2.close();
			outFile4.close();
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
