import weka.core.Attribute;


public class KeyContainer {
Attribute attr;
String attrVal;
String classval;
public KeyContainer(Attribute attr, String attrVal, String classval) {
	super();
	this.attr = attr;
	this.attrVal = attrVal;
	this.classval = classval;
}
public Attribute getAttr() {
	return attr;
}
public void setAttr(Attribute attr) {
	this.attr = attr;
}
public String getAttrVal() {
	return attrVal;
}
public void setAttrVal(String attrVal) {
	this.attrVal = attrVal;
}
public String getClassval() {
	return classval;
}
public void setClassval(String classval) {
	this.classval = classval;
}
@Override
public int hashCode() {
	final int prime = 31;
	int result = 1;
	result = prime * result + ((attr == null) ? 0 : attr.hashCode());
	result = prime * result + ((attrVal == null) ? 0 : attrVal.hashCode());
	result = prime * result + ((classval == null) ? 0 : classval.hashCode());
	return result;
}
@Override
public boolean equals(Object obj) {
	if (this == obj)
		return true;
	if (obj == null)
		return false;
	if (getClass() != obj.getClass())
		return false;
	KeyContainer other = (KeyContainer) obj;
	if (attr == null) {
		if (other.attr != null)
			return false;
	} else if (!attr.equals(other.attr))
		return false;
	if (attrVal == null) {
		if (other.attrVal != null)
			return false;
	} else if (!attrVal.equals(other.attrVal))
		return false;
	if (classval == null) {
		if (other.classval != null)
			return false;
	} else if (!classval.equals(other.classval))
		return false;
	return true;
}
}
