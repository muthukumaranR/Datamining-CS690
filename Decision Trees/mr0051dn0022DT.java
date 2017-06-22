/*
 * Author: muthukumaran Ramasubramanian (mr0051@uah.edu)
 * 		   deepak kumar nandagopal (dn0022@uah.edu)
 * class main: main class (mr0051dn0022DT.java)
 * date : 2/4/2017
 * 
 */


import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import weka.core.Instances;

public class mr0051dn0022DT {

	public static String arg0 = "bcwdisc.arff";
	public static String ClassAttr = "class";
	public static double percent_train = 10;
	
	
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
				if(args[i].equals("-t")||args[i].equals("-T")){
					percent_train = Double.parseDouble(args[i+1]);
					i++;
				}
			}
		}
		 BufferedReader reader = new BufferedReader(new FileReader(arg0));
		 Instances data = new Instances(reader);
		 reader.close();
		 
		 for(int i = 0 ; i< data.numAttributes();i++){// setting Class Attribute
			 if(data.attribute(i).name().equals(ClassAttr))
				 data.setClassIndex(i);
		}
		 
		 DTTrainer train_model = new DTTrainer(data,percent_train,arg0);
		 train_model.train();
		 System.out.println("Accuracy : " +train_model.test(arg0,ClassAttr));

	}// end of loop (if args length > 3)
}

