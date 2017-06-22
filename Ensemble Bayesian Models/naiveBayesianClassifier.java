/*
	* Author: muthukumaran Ramasubramanian (mr0051@uah.edu)
 * 		   
 * classifier: class that maintains attribute and value pairs
 * 			   and uses training data to find prior and likelihood
 */
import java.util.ArrayList;
import java.util.HashMap;

import weka.core.Instances;


public class naiveBayesianClassifier {

	Instances trainset;
	Instances testset;
	Instances classifiedTestset;
	double[] prior;
	HashMap<KeyContainer,Double> likelihood = new HashMap<KeyContainer,Double>();
	ArrayList<Pair<String,ArrayList<Pair<String,Integer>>>> confusion_mtx = 
								new ArrayList<Pair<String,ArrayList<Pair<String,Integer>>>>();
	int[][] conf_mtx = null;
	double Accuracy;
	ArrayList<String> prediction;
	public naiveBayesianClassifier(Pair<Instances, Instances> pair) {
		trainset = pair.getFirst();
		testset = pair.getSecond();
	}

	public void train() {
		int numInstances = trainset.numInstances();
		int numClasses = trainset.numClasses();
		int numAttributes = trainset.numAttributes();
		prior = new double[numClasses];
		String[] classvals = new String[numClasses];
		
		
		for(int j = 0;j < numClasses;j++){
			classvals[j] = trainset.classAttribute().value(j);
		}
		int[] numInstancesforClass = new int[numClasses];
		for(int i =0;i < numInstances;i++){
			for(int j = 0;j < numClasses;j++){
				if(trainset.instance(i).toString(trainset.classAttribute()).equals(classvals[j])){
					numInstancesforClass[j]++;
				}
			}
		}
		for(int j = 0;j < numClasses;j++){
			prior[j] = (double)numInstancesforClass[j] /(double) numInstances;
		}
		
		for(int j = 0;j < numAttributes-2;j++){
			for(int h = 0 ; h < trainset.attribute(j).numValues();h++){
				KeyContainer temp = null;
				int InstancesWithAVPair = 0;
				if(j != trainset.classIndex()){
					int[] Classoccurences = new int[numClasses];
					for(int c = 0; c < numClasses;c++){
						InstancesWithAVPair = 0;
						for(int i =0;i < numInstances;i++){
							if(trainset.instance(i).toString(trainset.classAttribute()).equals(trainset.classAttribute().value(c))){
								InstancesWithAVPair++;
								if(trainset.attribute(j).value(h).equals(trainset.instance(i).toString(j))){
									Classoccurences[c]++;
								}
								
							}
							
					}// end loop for instances
						temp = new KeyContainer(trainset.attribute(j),
								trainset.attribute(j).value(h), 
								trainset.classAttribute().value(c));
						likelihood.put(temp, (double)Classoccurences[c]/(double)InstancesWithAVPair);
				}// end loop for class vals
					
				}
			}// end loop for attr vals
		}// end loop for attributes
	}

	public void test() {
		int numInstances = testset.numInstances();
		int numAttributes = testset.numAttributes();
		int numClasses = testset.numClasses();
		int correctclass = 0;
	    prediction = new ArrayList<String>();
		for(int i = 0; i < numInstances; i++){
			
			double[] prob_for_class = new double[numClasses];
			double[] cum_Lhood = new double[numClasses];
			double maxprob = 0;
			int maxIdx = -1;
			for(int c = 0; c < numClasses;c++){
				cum_Lhood[c] = 1;
				double[] ithLHood = new double[numAttributes];
				
				for(int at = 0; at < numAttributes-2; at++){
					
					if(at != testset.classIndex()){
						ithLHood[at] = likelihood.get(new KeyContainer(testset.attribute(at),
										testset.instance(i).toString(at), testset.classAttribute().value(c)));
						cum_Lhood[c]*=ithLHood[at];
					}
					
				}
				prob_for_class[c] = prior[c]*cum_Lhood[c];
				if(prob_for_class[c] > maxprob){
					maxprob = prob_for_class[c];
					maxIdx = c;
				}
				
			}
			
			if(testset.classAttribute().value(maxIdx).equals(testset.instance(i).toString(
										testset.classAttribute())) ){
				correctclass++;
			}
			prediction.add(testset.classAttribute().value(maxIdx));
		}
		Accuracy = (double)correctclass / (double)testset.numInstances();
	}

}
