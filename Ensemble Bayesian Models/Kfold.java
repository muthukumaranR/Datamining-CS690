import java.util.ArrayList;

import weka.core.Attribute;
import weka.core.Instance;
import weka.core.Instances;


public class Kfold {
	Instances OG_data;
	Instances temp_data;
	int id_idx ;
	int isVisited_idx;
	ArrayList<Pair<Instances,Instances>> KfoldPairs = null;
	double[] class_ratio;
	public Kfold(Instances data, int num_Kfold) {
		OG_data = new Instances(data);
		
		OG_data.insertAttributeAt(new Attribute("id"), OG_data.numAttributes());
		id_idx = OG_data.numAttributes();
		for (int i = 0; i < OG_data.numInstances(); i++) {
	        // 
	        // adding indexes for trace back
			OG_data.instance(i).setValue(OG_data.numAttributes()-1, i);

	      }
		
		OG_data.insertAttributeAt(new Attribute("isVisited"), OG_data.numAttributes());
		isVisited_idx = OG_data.numAttributes();
		for (int i = 0; i < OG_data.numInstances(); i++) {
	        // 
	        // adding indexes for trace back
			OG_data.instance(i).setValue(OG_data.numAttributes()-1, 0);

	      }
		temp_data = new Instances(OG_data);
		class_ratio = getClassRatio();
		KfoldPairs = new ArrayList<Pair<Instances,Instances>>();
		int numInstances = OG_data.numInstances();
		double segLength = (double)numInstances /(double) num_Kfold;
		Instances trainset ;
		Instances testset ;
		int InstancesFilled = 0;
		int numInstancesLeft = temp_data.numInstances();
		int[] numInstancesPerSplit = new int[temp_data.numClasses()];
		
		for(int i = 0; i < OG_data.numClasses();i++){
			numInstancesPerSplit[i] = (int)(segLength*class_ratio[i]);
		}
		
		//KfoldPairs.add(new Pair<Instances,Instances>(trainset,testset));
		for(int i =0 ; i < num_Kfold;i++){
			trainset = new Instances(temp_data,0);
			testset = new Instances(temp_data,0);
			Instances data_for_train = new Instances(OG_data);
			if(i != num_Kfold-1){
				int[] InstanceCounters = numInstancesPerSplit.clone();
				boolean isFilled = false;
				
				while(!isFilled){
					int rand =(int) (Math.random()*temp_data.numInstances());
					Instance selectedInst =temp_data.get(rand);
					int classIdx = selectedInst.classAttribute().indexOfValue(
							selectedInst.toString(selectedInst.classAttribute()));
					if(InstanceCounters[classIdx] != 0){
						testset.add(selectedInst);
						int idx = Integer.parseInt(selectedInst.toString(selectedInst.attribute(id_idx-1)));
						temp_data.delete(rand);
						Instance temp_inst = data_for_train.get(idx);
						temp_inst.setValue(isVisited_idx-1, 1);
						data_for_train.set(idx, temp_inst);
						
						InstanceCounters[classIdx]--;
					}
					
				
					
					int inc = 0;
					for(int j = 0; j < InstanceCounters.length;j++){
						if(InstanceCounters[j]!=0){
							inc++;
						}
					}
					if(inc == 0){
						isFilled = true;
					}
				
				}
				
			}
			else{
				for(int j = 0;j < temp_data.numInstances();j++){
					testset.add(temp_data.instance(j));
					Instance selectedInst =temp_data.get(j);
					int classIdx = selectedInst.classAttribute().indexOfValue(
							selectedInst.toString(selectedInst.classAttribute()));
						int idx = Integer.parseInt(selectedInst.toString(selectedInst.attribute(id_idx-1)));
						Instance temp_inst = data_for_train.get(idx);
						temp_inst.setValue(isVisited_idx-1, 1);
						data_for_train.set(idx, temp_inst);
						
					
				}
			}
			for(int j = 0;j < data_for_train.numInstances();j++){
				if(data_for_train.instance(j).toString(isVisited_idx-1).equals("0")){
					trainset.add(data_for_train.instance(j));
				}
			}
			KfoldPairs.add(new Pair<Instances,Instances>(trainset,testset));
		}
	}
	private double[] getClassRatio() {
		double[] ratios = new double[OG_data.numClasses()];
		for(int i =0; i < OG_data.numClasses();i++){
			for(int j =0 ; j < OG_data.numInstances();j++){
				if(OG_data.instance(j).toString(OG_data.classAttribute()).equals(
													OG_data.classAttribute().value(i)))
					ratios[i]++;
				}
		}
		for(int i = 0; i < OG_data.numClasses();i++){
			ratios[i]/= (double)OG_data.numInstances();
		}
		return ratios;
	}

	
}
/* unmodified code
 * import java.util.ArrayList;

import weka.core.Instances;


public class Kfold {
	Instances OG_data;
	Instances temp_data;
	ArrayList<Pair<Instances,Instances>> KfoldPairs = null;
	double[] class_ratio;
	public Kfold(Instances data, int num_Kfold) {
		OG_data = new Instances(data);
		temp_data = new Instances(data);
		class_ratio = getClassRatio();
		KfoldPairs = new ArrayList<Pair<Instances,Instances>>();
		int numInstances = data.numInstances();
		double segLength = (double)numInstances /(double) num_Kfold;
		Instances trainset ;
		Instances testset ;
		for(int i =0 ; i < num_Kfold;i++){
			trainset = new Instances(data,0);
			testset = new Instances(data,0);
			for(int j =0 ; j < data.numInstances();j++){
				if(j >= i*segLength && j < (i+1)*(segLength) ){
					testset.add(data.instance(j));
				}
				else{
					trainset.add(data.instance(j));
				}
			}
			KfoldPairs.add(new Pair<Instances,Instances>(trainset,testset));
		}
	}
	private double[] getClassRatio() {
		double[] ratios = new double[OG_data.numClasses()];
		for(int i =0; i < OG_data.numClasses();i++){
			for(int j =0 ; j < OG_data.numInstances();j++){
				if(OG_data.instance(j).toString(OG_data.classAttribute()).equals(
													OG_data.classAttribute().value(i)))
					ratios[i]++;
				}
		}
		for(int i = 0; i < OG_data.numClasses();i++){
			ratios[i]/= (double)OG_data.numInstances();
		}
		return ratios;
	}

	
}

 * 
 */
  
