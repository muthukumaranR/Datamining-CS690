/*
 * Author: muthukumaran Ramasubramanian (mr0051@uah.edu)
 * 			deepak kumar nandagopal (dn0022@uah.edu)
 * class Dtrees: data structure that maintains the decision tree
 *
 */

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;

import weka.core.Instance;
import weka.core.Instances;
import weka.core.Attribute;
import weka.estimators.CheckEstimator.AttrTypes;


public class DTrees {

  private Instances head;
  
  //private ArrayList<Pair<Attribute,String>> link ;
  private Pair<Attribute,String> linkToParent = null;

  private ArrayList<DTrees> leafs = new ArrayList<DTrees>();

  private DTrees parent = null;

  private HashMap<Instances, DTrees> locate = new HashMap<Instances, DTrees>();

  public DTrees(Instances head) {
    this.head = head;
    
    locate.put(head, this);
  }

  
  public void addLeaf(Instances root, Instances leaf,Pair<Attribute, String> linkPair) {
	    if (locate.containsKey(root)) {
	      locate.get(root).addLeaf(leaf);
	      //locate.get(root).link.add(linkPair);
	      locate.get(leaf).linkToParent = new Pair<Attribute, String>(linkPair.getFirst(), 
	    		  													  linkPair.getSecond());
	    } 
	    else {
	     // addLeaf(root,linkPair).addLeaf(leaf);
	    }
	  }

  public DTrees addLeaf(Instances leaf) {
    DTrees t = new DTrees(leaf);
    leafs.add(t);
    t.parent = this;
    t.locate = this.locate;
    locate.put(leaf, t);
    return t;
  }
  
/*
  public DTrees<T> addLeaf(T leaf,Pair<Attribute, String> link) {
	    DTrees<T> t = new DTrees<T>(leaf);
	    t.link.add(link);
	    leafs.add(t);
	    t.parent = this;
	    t.locate = this.locate;
	    locate.put(leaf, t);
	    return t;
	  }
*/
  public DTrees setAsParent(Instances parentRoot) {
    DTrees t = new DTrees(parentRoot);
    t.leafs.add(this);
    this.parent = t;
    t.locate = this.locate;
    t.locate.put(head, this);
    t.locate.put(parentRoot, t);
    return t;
  }

  public Instances getHead() {
    return head;
  }

  public DTrees getTree(Instances element) {
    return locate.get(element);
  }

  public DTrees getParent() {
    return parent;
  }

  public Collection<Instances> getSuccessors(Instances root) {
    Collection<Instances> successors = new ArrayList<Instances>();
    DTrees tree = getTree(root);
    if (null != tree) {
      for (DTrees leaf : tree.leafs) {
        successors.add(leaf.head);
      }
    }
    return successors;
  }

  public Collection<DTrees> getSubTrees() {
    return leafs;
  }

  public static  Collection<Instances> getSuccessors(Instances of, Collection<DTrees> in) {
    for (DTrees tree : in) {
      if (tree.locate.containsKey(of)) {
        return tree.getSuccessors(of);
      }
    }
    return new ArrayList<Instances>();
  }

  @Override
  public String toString() {
    return printTree(-1);
  }

  private static final int indent = 1;

  public String printTree(int increment) {
    String s = "";
    String inc = "";
    for (int i = 0; i <  increment ; ++i) {
    	if(this.linkToParent == null || this.parent.linkToParent== null){}
        	//s = null;
    	else{
    		inc = inc + " | ";
    	}
    }
    if(this.linkToParent == null){
    	//s = null;
    }
    else{
    	s = inc + this.linkToParent.getFirst().name() + " = "+ this.linkToParent.getSecond() ;
    }
    for (DTrees child : leafs) {
      s += "\n" + child.printTree(increment + indent);
    }
    
    
    if(this.leafs.isEmpty()){ // return max class value aka: maximum voting
    	
    	int[] noClassVals = new int[this.head.classAttribute().numValues()]; 
    	for(int i = 0; i < this.head.numInstances(); i++){
    		for(int j = 0; j < head.classAttribute().numValues();j++){
    			if(this.head.instance(i).toString(head.classAttribute()).equals( this.head.classAttribute().value(j))){
    				noClassVals[j]++;
    			}
    		}	
    	}
    	int maxval = 0;
    	int maxidx = -1;
    	for(int i = 0; i< noClassVals.length; i++){
    		if(maxval < noClassVals[i]){
    			maxval = noClassVals[i];
    			maxidx = i;
    		}
    	}
    	if(maxidx != -1)
    		s+= " "+ this.head.attribute(this.head.classIndex()).value(maxidx);
    	else
    		s+= " "+ null;
    }
    return s;
  }



public String getClassforInstance(Instance instance,int count) {
	boolean classFound = false;
	DTrees testtree = this;
	
	while(!classFound){
		if(count > instance.numAttributes()){
			return testtree.getMaxClass();
		}
		if (testtree.leafs.isEmpty()){
			int[] noClassVals = new int[numUniqueVals(testtree.head, testtree.head.classAttribute())]; 
			 ArrayList<String> attrvals =findValsinAttr(testtree.head, testtree.head.classAttribute());
	    	for(int i = 0; i < testtree.head.numInstances(); i++){
	    		for(int j = 0; j < numUniqueVals(testtree.head, testtree.head.classAttribute());j++){
	    			if(testtree.head.instance(i).toString(testtree.head.classAttribute()).equals(attrvals.get(j))){
	    				noClassVals[j]++;
	    			}
	    		}	
	    	}
	    	int maxval = 0;
	    	int maxidx = -1;
	    	for(int i = 0; i< noClassVals.length; i++){
	    		if(maxval < noClassVals[i]){
	    			maxval = noClassVals[i];
	    			maxidx = i;
	    		}
	    	}
	    	if(maxidx != -1) return attrvals.get(maxidx);
	    	
		}
		count++;
		outerloop:
		for(int k = 0; k < testtree.leafs.size(); k++){
			for(int i = 0; i < instance.numAttributes();i++ ){
			if(instance.attribute(i).equals(testtree.leafs.get(k).linkToParent.getFirst())){
				if(instance.toString(instance.attribute(i)).equals(testtree.leafs.get(k).linkToParent.getSecond())){
					testtree = testtree.leafs.get(k);
					break outerloop;
				}
			}
			}
		//if(instance.stringValue(testtree.leafs.get(k).linkToParent.getFirst()).equals(testtree.leafs.get(k).linkToParent.getSecond())){
			
			
		}
	//return "-1";
	}
	return "-1";
}
private String getMaxClass() {
	int[] noClassVals = new int[numUniqueVals(this.head, this.head.classAttribute())]; 
	 ArrayList<String> attrvals =findValsinAttr(this.head, this.head.classAttribute());
	for(int i = 0; i < this.head.numInstances(); i++){
		for(int j = 0; j < numUniqueVals(this.head, this.head.classAttribute());j++){
			if(this.head.instance(i).toString(this.head.classAttribute()).equals(attrvals.get(j))){
				noClassVals[j]++;
			}
		}	
	}
	int maxval = 0;
	int maxidx = -1;
	for(int i = 0; i< noClassVals.length; i++){
		if(maxval < noClassVals[i]){
			maxval = noClassVals[i];
			maxidx = i;
		}
	}
	if(maxidx != -1) return attrvals.get(maxidx);
	
	return null;
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

}

