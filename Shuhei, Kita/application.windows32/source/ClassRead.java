import java.util.*;
import java.io.*;

import com.sun.org.apache.bcel.internal.classfile.Code;

import sun.management.MethodInfo;
public class ClassRead{
	private  List<Byte> binary = new ArrayList<Byte>();
	private  int	ID; 
	private String name;
	private int viewID;
	
	private boolean check = false;	//Error flag
	
	private  Byte[] magic 	= {-54, -2, -70, -66};	//4byte
	private  int cp_count;
	private  CPInfo[] cp;	// = new ArrayList<CPInfo>();	//[1~cp_count-1][]
	private  int methods_count;
	private  FieldInfo[] methods;
	private  int attributes_count;  	//2byte
	private  AttributeInfo[] attributes;	// = new ArrayList<AttributeInfo>();	//[attributes_count][]
	
	//cpのズレ解消用
	private int NUMBER = 0;
	
	//Code list
	private boolean view = true;
    private CodeAttribute[] codes;
    private int focus = 1;
    private int hue = 0;
    private int alpha = 50;
   
	private boolean onMouse = false;
	private boolean onName = false;
	
	//表示用
	private int x;
	private int y;
    
	//constract---------------------------------------------------
	ClassRead(String Path, String Name, int Id){
		initClass(Path, Name);
		System.out.println("binary_size: "+binary.size());
		ID = Id;
		viewID = -1;
		
		int index = 0;
		int maj_index = 0;
		int min_index = 0;
		
		boolean flag = false;	//error flag
		//class file info  get ---------------------------------
		///magic number
		for(int i=0; i<4; i++){
			if(binary.get(index++) != magic[i]){
				check = true;
				break;
			}
		}
		if(!check){
			//minner ver
			//majorver
			index += 4;
			
			//cp_count
			//cp
			cp_count = ByteFunc.getInt(binary.get(index++), binary.get(index++)) - 1;
			cp = new CPInfo[cp_count];
			for(int i=0; i<cp_count; i++){
				//System.out.print(""+i+": "+String.format("%02X ", index)+"  ");
				CPInfo cpInfo = new CPInfo(binary, index, ID);
				//ズレ時カウントアップ
				int tag = cpInfo.getTag();
				if(tag==0 || tag==2 || tag>12){
					NUMBER++;
				}
				index = cpInfo.getNextIndex();
				cp[i] = cpInfo;
			}
			System.out.println("zure: "+NUMBER);
			//access flag
			//super class
			//this class
			index += 6;
			//interfaces count
			//interfaces
			int interfaces_count = ByteFunc.getInt(binary.get(index++), binary.get(index++));
			index += interfaces_count * 2;
			//fields_count
			//fields
			int fields_count = ByteFunc.getInt(binary.get(index++), binary.get(index++));
			for(int i=0; i<fields_count; i++){
				FieldInfo tmp = new FieldInfo(binary, index, ID);
				index = tmp.getNextIndex();
			}
			//methods_count
			//methods
			methods_count = ByteFunc.getInt(binary.get(index++), binary.get(index++));
			methods = new FieldInfo[methods_count];
			for(int i=0; i<methods_count; i++){
				methods[i] = new FieldInfo(binary, index, ID);
				index = methods[i].getNextIndex();
			}
			//attributes_count
			//attributes
			attributes_count = ByteFunc.getInt(binary.get(index++), binary.get(index++));
			attributes = new AttributeInfo[attributes_count];
			for(int i=0; i<attributes_count; i++){
				attributes[i] = new AttributeInfo(binary, index, ID);
				index = attributes[i].getNextIndex();
			}
			//class file info get---end-----------------------------
			
			//set codeAttrivute data
			this.getMethodCode();
			//set invoke data
			this.setMethodInvoke();
			
			
		}
		
		
	}
	//--constract----END-----------------------------------------------
	
	//init method
	private void initClass(String path, String Name){
		try{
			name = Name.split("\\.")[0];
			FileInputStream in = new FileInputStream(path+"/"+Name);
			while (true) {
	            Integer b = in.read();
	            if (b == -1) {
	                break;
	            }
	            Byte b1 = b.byteValue();
	            binary.add(b1);
	        }
		}catch(Exception e){
		}
	}
	
	//set invoke of method
	public void setMethodInvoke(){
		for(int i=0; i<codes.length; i++){
			codes[i].setInvoke(cp, NUMBER);
		}
	}
	
	//set hue
	public void setHue(int num){
		hue = num;
	}
	//set focus
	public void setFocus(int d){
		focus = d;
		if(focus == 2)
			alpha = 90;
		else if(focus == 1)
			alpha = 50;
		else if(focus == 0)
			alpha = 10;
	}
	//set viewID
	public void setViewID(int Id){
		viewID = Id;
	}
	public void setOnMouse(boolean b){
		onMouse = b;
	}
	public void setOnName(boolean b){
		onName = b;
	}
	//set view
	public void setView(boolean b){
		view = b;
	}
	///GETTER--------------------------------------
	//get color
	public int getHue(){
		return hue;
	}
	public int getFocus(){
		return focus;
	}
	public int getAlpha(){
		return alpha;
	}
	public CodeAttribute[] getCode(){
		return codes;
	}
	public String getName(){
		return name;
	}
	public int getViewID(){
		return viewID;
	}
	public int getID(){
		return ID;
	}
	public boolean check(){
		return check;
	}
	public boolean getOnMouse(){
		return onMouse;
	}
	public boolean getOnName(){
		return onName;
	}
	public boolean isView(){
		return view;
	}
	public int getX(){
		return x;
	}
	public int getY(){
		return y;
	}
	///GETTER----END----------------------------------
	
	/////Method----
	public void setXY(){
		//code位置の重心を求める      
		x=y=0;
		int m = 0;
		for(int i=0; i<codes.length; i++){
			int size = codes[i].getCode().length;
			x += codes[i].getX() * size;
			y += codes[i].getY() * size;
			m += size;
		}
		if(m!=0){
			x = x/m;
			y = y/m;
		}
	}
	
	public void getMethodName(){
		for(int i=0; i<methods.length; i++){
			//List<Byte> bytes = cp[methods[i].getNameIndex()].info;
			System.out.println("method name: "+cp[methods[i].getNameIndex()-1].getUTF8());
			for(int j=0; j<methods[i].getAttributeLength(); j++){
				System.out.println("attribute name: "+cp[methods[i].getAttribute(j).getNameIndex()-1].getUTF8());
			}
		}
	}
	//GET Code attribute   (all code)
	public void getMethodCode(){
		List<CodeAttribute> tmpList = new ArrayList<CodeAttribute>();
		for(int i=0; i<methods.length; i++){
			String mName = cp[methods[i].getNameIndex()-NUMBER].getUTF8();
			//System.out.println("attribute name: "+cp[methods[i].getAttribute(0).getNameIndex()].getUTF8());
			//if(!mName.equals("<init>") && !mName.equals("<clinit>")){
				for(int j=0; j<methods[i].getAttributeLength(); j++){
					String aName = cp[methods[i].getAttribute(j).getNameIndex()-NUMBER].getUTF8();
					if(aName.equals("Code")){
						tmpList.add(  methods[i].getAttribute(j).getCode(mName) );
					}
				}
			//}
		}
		codes = tmpList.toArray(new CodeAttribute[0]);
	}
	
	//debug
	public void print(){
		System.out.println("class: "+name);
		for(int i=0; i<codes.length; i++){
			codes[i].print();
		}
	}
	//Code output
	public void printCode(){
		for(int i=0; i<codes.length; i++){
			CodeAttribute code = codes[i];
			Byte[] bytes = code.getCode();
			System.out.println("method	: "+code.getName());
			System.out.println("bytes size 	: "+bytes.length);
			for(int j=0; j<bytes.length; j++){
				System.out.print(""+String.format("%02X ", bytes[j])+" ");
			}
			System.out.println(" ");
		}
	}
	
	
	
}

