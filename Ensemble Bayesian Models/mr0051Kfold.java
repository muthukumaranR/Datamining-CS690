/*
 * Author: muthukumaran Ramasubramanian (mr0051@uah.edu)
 * Date : 17 april 2017		
 * class main: main class mr0051Kfold.java
 * 
 */


import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.PrintWriter;
import java.util.ArrayList;

import weka.core.Attribute;
import weka.core.Instances;

public class mr0051Kfold{

	public static String arg0 = "";
	public static String ClassAttr = "";
	public static int num_Kfold ;
	
	public static void main(String[] args) throws Exception{
		
		if(args.length<6){
			System.out.println("incorrect Arguement List");
			System.exit(0);
		}
		
		else{
			for(int i = 0 ; i< args.length;i++){// process args
				if(args[i].equals("-i")||args[i].equals("-I")){
					arg0 = args[i+1];
					i++;
				}
				if(args[i].equals("-c")||args[i].equals("-C")){
					ClassAttr = args[i+1];
					i++;
				}
				if(args[i].equals("-k")||args[i].equals("-K")){
					num_Kfold = Integer.parseInt(args[i+1]);
					i++;
				}
			}
		}
		
		 int[][] conf_mtx;
		 BufferedReader reader = new BufferedReader(new FileReader(arg0));
		 Instances data = new Instances(reader);
		 reader.close();
		 
		 for(int i = 0 ; i< data.numAttributes();i++){// setting Class Attribute
			 if(data.attribute(i).name().equals(ClassAttr))
				 data.setClassIndex(i);
		}
		 
		Kfold kfold_datasets = new Kfold(data,num_Kfold);
		
		naiveBayesianClassifier[] kclassifiers = new naiveBayesianClassifier[kfold_datasets.KfoldPairs.size()];
		
		for(int i = 0; i < kclassifiers.length ;i++){
			kclassifiers[i] = new naiveBayesianClassifier(kfold_datasets.KfoldPairs.get(i));
			kclassifiers[i].train();
			kclassifiers[i].test();
		}
		
		
		Instances newData = new Instances(data);
		ArrayList<String> classvals = findValsinAttr(newData, newData.classAttribute());
		ArrayList<String> class_expected = new ArrayList<String>();
		ArrayList<String> class_predicted = new ArrayList<String>();
		//,new ArrayList<String>(newData.numAttributes())
		newData.insertAttributeAt(new Attribute("bayesClass real",classvals), newData.numAttributes());
		conf_mtx = new int[classvals.size()][classvals.size()];
		int origidx = 0; 
		int clsidx = 0;
		for(int i=0;i < newData.numInstances();i++){
			class_expected.add(newData.instance(i).toString(newData.numAttributes()-2));
			origidx = classvals.indexOf(newData.instance(i).toString(newData.numAttributes()-2));
			outloop:
			for(int j = 0; j < kclassifiers.length ;j++){
				for(int k = 0; k < kclassifiers[j].testset.numInstances() ;k++){
					
					if(Integer.parseInt(kclassifiers[j].testset.instance(k).toString(
							kclassifiers[j].testset.numAttributes()-2))==i){
						newData.instance(i).setValue(newData.numAttributes()-1, 
								kclassifiers[j].prediction.get(k));
						class_predicted.add(kclassifiers[j].prediction.get(k));
						clsidx = classvals.indexOf(kclassifiers[j].prediction.get(k));

						break outloop;
					}
				}
			}
			conf_mtx[origidx][clsidx]++;
		
		}
		
		PrintWriter out = new PrintWriter("mr0051"+Integer.toString(num_Kfold)+"FoldClassification"+arg0.substring(0, arg0.length()-5)+".arff");
        PrintWriter txtout = new PrintWriter(new File("mr0051"+Integer.toString(num_Kfold)+"FoldConfusion"+arg0.substring(0,arg0.length()-5)+".txt"));
        out.write(newData.toString());
        out.close();
        
        txtout.println("confusion matrix:");
        txtout.print("Expected Class / Predicted Class\t");
        for (int j = 0; j < classvals.size(); j++) {
        	
			txtout.print(classvals.get(j)+"\t");
		}
        txtout.println();
		for (int i = 0; i < classvals.size(); i++) {
			txtout.print("\t\t");
			txtout.print(classvals.get(i)+"\t\t\t");
			for (int j = 0; j < classvals.size(); j++) {
				
				txtout.print(conf_mtx[i][j]+"\t");
			}
			txtout.println();
		}
		txtout.println();
		txtout.println();
		double cumAccuracy = 0;
		for(int i =0;i < kclassifiers.length;i++){
			txtout.println("Accuracy of fold "+(i+1)+" : "+(float)kclassifiers[i].Accuracy*100 +"%");
			System.out.println("Accuracy of fold "+(i+1)+" : "+(float)kclassifiers[i].Accuracy*100 +"%");

			cumAccuracy+=kclassifiers[i].Accuracy;
		}
		txtout.println();
		txtout.println();
		txtout.println("Overall Accuracy of the model: "+ ((float)cumAccuracy/num_Kfold)*100 +"%");
		System.out.println("Overall Accuracy of the model: "+ ((float)cumAccuracy/num_Kfold)*100 +"%");

		txtout.close();
	}
	
	private static ArrayList<String> findValsinAttr(Instances data, Attribute attribute) {
		ArrayList<String> vals = new ArrayList<String>();
		vals.add(data.instance(0).toString(attribute));
		//int foundIdx = -1;
		for(int i = 1; i < data.numInstances(); i++){
			if(!vals.contains(data.instance(i).toString(attribute))){
				vals.add(data.instance(i).toString(attribute));
			}
			
		}
		return vals;
	}
	
}
