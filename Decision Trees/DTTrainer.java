/*
 * Author: muthukumaran Ramasubramanian (mr0051@uah.edu)
 * class DTTrainer: class to split instances, train a 
 * decision tree and test on terst data
 */

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import weka.core.Attribute;
import weka.core.Instances;
import weka.filters.Filter;
import weka.filters.unsupervised.attribute.Remove;

public class DTTrainer {
	public Instances trainInstance;
	public Instances testInstance;
	DTrees dt;
	int classAttr ;
	int M;
	String filename;
	/*
	 *  Constructor DTTrainer: takes in the Instances of data, and splits
	 *  them with the percent of data given in percent_to_train
	*/
	public DTTrainer(Instances data, double percent_to_train,String arg0) {
		
		trainInstance = new Instances(data);
		classAttr = data.classIndex();
		M = (int)percent_to_train;
		int intcount = data.numInstances();
		filename = arg0;
//		ArrayList<Integer> list = new ArrayList<Integer>();
//        for (int i=0; i<data.numInstances(); i++) {
//            list.add(new Integer(i));
//        }
//        Collections.shuffle(list);
        
        for (int i=(int) (data.numInstances()*((100-percent_to_train)/100)); i>0; i--) {
        	trainInstance.delete(i);   // deleting 100-percent_to_train % of data
        }	
        
	}
	
	/*
	 * method : train 
	 * desc   : trains the Decision tree model with the training instances 
	 */
	public void train() throws Exception{
		dt = new DTrees(trainInstance);
        EntropySplit(trainInstance);
        PrintWriter out = new PrintWriter("mr0051dn0022DTTrain"+Integer.toString(M)+filename.substring(0, filename.length()-5)+".dt");
        out.print(dt.toString());
        out.close();
	}
	
	
	
	/*
	 * method : EntropySplit 
	 * desc   : returns the best split Attribute for the given data 
	 * 			and recursively calls itself until only classAttribute 
	 * 			present or Attribute cannot be split anymore
	 */
	private void EntropySplit(Instances data) throws Exception{
		
		// finding global IG for data
		if(data.numAttributes()>1){
			float baseEntropy = Entropy(data);
			int bestAttrIdx = -1;
			//finding IG for each attributer in global data
			
			//TODO: Condition to check base case: if Instances only have classes
			float[] IG = new float[data.numAttributes()];
			for(int attr = 0; attr < data.numAttributes();attr++){
				if(!data.attribute(attr).equals(data.classAttribute())){// if attribute not a class
					
					int noAttrValues = numUniqueVals(data,data.attribute(attr));
					Instances[] splitInstances = new Instances[noAttrValues];
					ArrayList<String> attrvals = findValsinAttr(data,data.attribute(attr));
		//mark for change			
					for(int attrValIdx = 0; attrValIdx < noAttrValues ; attrValIdx++){// splitting the instances
						splitInstances[attrValIdx] = splitInstance(
															data,data.attribute(attr),
															attrvals.get(attrValIdx));
					}
					IG[attr] = InfoGain(baseEntropy,splitInstances);
				}
				else{
					IG[attr] = -9999;
				}
			}
			float maxIG = -999;
			
			for(int attr = 0; attr < data.numAttributes();attr++){
				
				if(maxIG < IG[attr] && !Double.isNaN(IG[attr])) {
					maxIG = IG[attr];
					bestAttrIdx = attr;
				}
			}
			if(bestAttrIdx == -1 || bestAttrIdx == data.classIndex() || Double.isNaN(bestAttrIdx) || maxIG <= 0)return;
		
			int noBestAttrValues = numUniqueVals(data,data.attribute(bestAttrIdx));
			Instances[] splitInstances = new Instances[noBestAttrValues];
			ArrayList<String> attrvals = findValsinAttr(data,data.attribute(bestAttrIdx));
			// mark for eplace
			for(int attrValIdx = 0; attrValIdx < noBestAttrValues ; attrValIdx++){// splitting the instances
				splitInstances[attrValIdx] = splitInstance(data,data.attribute(bestAttrIdx),
																attrvals.get(attrValIdx));
				Pair<Attribute,String> link = new Pair<Attribute, String>(data.attribute(bestAttrIdx),attrvals.get(attrValIdx));
				dt.addLeaf(data, splitInstances[attrValIdx], link);
				if(numUniqueVals(splitInstances[attrValIdx], splitInstances[attrValIdx].classAttribute()) >1){
					EntropySplit(splitInstances[attrValIdx]);
				}
			}
		

		}
		else{ // if only class attribute is present
			return;
		}
	}
	
	

	private ArrayList<String> findValsinAttr(Instances data, Attribute attribute) {
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

	private int numUniqueVals(Instances data, Attribute attribute) {
		// TODO Auto-generated method stub
		return data.numDistinctValues(attribute);
	}

	/*
	 * method : InfoGain 
	 * desc   : calculates the INFOGAIN for given split of data
	 */
	private float InfoGain(float baseEntropy, Instances[] splitInstances) {
		float[] splitEntropy =  new float[splitInstances.length];
		int[] noInstances = new int[splitInstances.length];
		int totalInstances = 0;
		for(int attrValIdx = 0; attrValIdx < splitInstances.length;attrValIdx++){
			splitEntropy[attrValIdx] = Entropy(splitInstances[attrValIdx]);
			noInstances[attrValIdx] = splitInstances[attrValIdx].numInstances();
			totalInstances += noInstances[attrValIdx];
		}
		float cumEntropy =0;
		for(int attrValIdx = 0; attrValIdx < splitInstances.length;attrValIdx++){
			cumEntropy += ((float)noInstances[attrValIdx]/(float)totalInstances)  * splitEntropy[attrValIdx];
		}
		
		// TODO Auto-generated method stub
		return baseEntropy - cumEntropy;
	}

	/*
	 * method : splitInstance 
	 * desc   : splits the Instance by removing given attribute and value
	 */
	private Instances splitInstance(Instances data, Attribute attribute, String value) throws Exception {
		// TODO split data with given attribute value
		Instances newInstance = new Instances(data);
		int attrIdx = 0;
		for(int attr = 0;attr < data.numAttributes();attr++){
			if(data.attribute(attr).equals(attribute)){
				attrIdx = attr;
			}
		}
		
		for(int i = newInstance.numInstances()-1;i >= 0;i--){
			if(!newInstance.instance(i).stringValue(attrIdx).equals(value)){
				newInstance.delete(i);
			}
		}
		Remove remove = new Remove();
	    remove.setAttributeIndices(Integer.toString(attrIdx+1));
	    remove.setInputFormat(newInstance);
		newInstance = Filter.useFilter(newInstance, remove);
		return newInstance;
	}

	/*
	 * method: Entropy 
	 * desc  : calculates the entropy for the given instances
	 */
	private float Entropy(Instances data){
		classAttr =data.classIndex();
		int noClassValues = numUniqueVals(data,data.attribute(classAttr));
		ArrayList<String> attrvals = findValsinAttr(data,data.attribute(classAttr));
		HashMap<Integer, Integer> ClassNumPair = new HashMap<Integer, Integer>();
		
		for(int i =0;i < noClassValues; i++){
			ClassNumPair.put(Integer.decode(attrvals.get(i)),new Integer(0));
			int num = 0;
			for(int j =0;j < data.numInstances(); j++){
				//ClassNumPair.put(Integer.decode(data.attribute(classAttr-1).value(i)),new Integer(0));
				if(data.instance(j).stringValue(classAttr).equals(attrvals.get(i))){
					num++;
				}
			}
			ClassNumPair.replace(Integer.decode(attrvals.get(i)),new Integer(num));
		}
		
		//entropy 
		ArrayList<Integer> intAL = new ArrayList<Integer>() ;
		for(Integer key: ClassNumPair.keySet()){
			intAL.add(ClassNumPair.get(key));
		}
		//float baseEntropy = Entropy(intAL);
		
		float sum = 0;
		float globalDistr = 0;
		for(int i =0;i<intAL.size();i++){
			globalDistr += intAL.get(i); 
		}
		for(int i =0;i<intAL.size();i++){
			sum += (intAL.get(i)/globalDistr)*(Math.log(intAL.get(i)/globalDistr));
		}
		return -1*sum;
		
	}
	
	@SuppressWarnings("unused")
	public float test(String arffpath,String classAttr) throws IOException{
		
		FileReader fr = new FileReader(new File(arffpath));
		Instances testdata = new Instances(fr);
		fr.close();
		FileReader fr2 = new FileReader(new File(arffpath));
		Instances outdata = new Instances(fr2);
		fr2.close();
		boolean[] isCorrect = new boolean[testdata.numInstances()];
		int correctNum = 0;
		String cls = null;
		String[] clsstr = new String[testdata.numInstances()];
		
        
		
		
		for(int i = 0 ; i< testdata.numAttributes();i++){// setting Class Attribute
			 if(testdata.attribute(i).name().equals(classAttr))
				 testdata.setClassIndex(i);
		}
		int[][] conf_mat = new int[testdata.numClasses()][testdata.numClasses()];
		ArrayList<String> class_vals = new ArrayList<String>();
		class_vals = findValsinAttr(testdata, testdata.classAttribute());
		for(int i =0; i< isCorrect.length;i++){
			// mark isCorrect = 1 if current Instance is correctly predicted;
			// 		isCorrect = 0 if falsely predicted
			
			cls = dt.getClassforInstance(testdata.instance(i),0);
			clsstr[i] = cls;
			
			if(cls.equals(testdata.instance(i).toString(testdata.classIndex()))){
				isCorrect[i] = true;
				correctNum ++;
			}
			else{
				isCorrect[i] = false;
			}
			
			// Generating the confusion matrix
			int origidx = class_vals.indexOf(testdata.instance(i).toString(testdata.classAttribute())); 
			int clsidx = class_vals.indexOf(clsstr[i]);
		
			conf_mat[origidx][clsidx]++;
		}
		outdata.insertAttributeAt(new Attribute("class_dt", class_vals), outdata.numAttributes());
		
		for (int i = 0; i < outdata.numInstances(); i++) {
	        // 1. nominal
	        // index of labels A:0,B:1,C:2,D:3
			outdata.instance(i).setValue(outdata.numAttributes()-1, clsstr[i]);

	      }
		
		PrintWriter out = new PrintWriter("mr0051dn0022DTApply"+Integer.toString(M)+filename.substring(0, filename.length()-5)+".arff");
        PrintWriter txtout = new PrintWriter(new File("mr0051dn0022DTAccuracy"+Integer.toString(M)+filename.substring(0, filename.length()-5)+".txt"));
        out.write(outdata.toString());
        out.close();
        
        txtout.println("confusion matrix:");
        txtout.print("Expected Class / Predicted Class\t");
        for (int j = 0; j < class_vals.size(); j++) {
        	
			txtout.print(class_vals.get(j)+"\t");
		}
        txtout.println();
		for (int i = 0; i < class_vals.size(); i++) {
			txtout.print("\t\t");
			txtout.print(class_vals.get(i)+"\t\t\t");
			for (int j = 0; j < class_vals.size(); j++) {
				
				txtout.print(conf_mat[i][j]+"\t");
			}
			txtout.println();
		}
		txtout.println();
		txtout.println();
		txtout.println("Accuracy of the model: "+ ((float)correctNum / (float)isCorrect.length)*100 +"%");
		
		txtout.close();
		return (float)correctNum / (float)isCorrect.length;
	}
}


