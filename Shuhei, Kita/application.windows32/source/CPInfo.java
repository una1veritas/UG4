import java.util.*;
import java.io.*;


public class CPInfo{
	private int ID;
	private Byte cp_info_tag;	//1byte
	private List<Byte> info = new ArrayList<Byte>();
	private int next_index;
	//static Byte cp_info_tag;	//1byte
	//static List<Byte> info = new ArrayList<Byte>();
	
	CPInfo(List<Byte> bytes, int index, int id){
		ID = id;
		cp_info_tag = bytes.get(index++);
		int byteNum = 0;
		switch(cp_info_tag&0xff){
			case 7:	//Class-info
					//name_index 		2byte
				byteNum = 2;
				break;
			case 9:	//Fieldref-info
			case 10://Methodref-info
			case 11://Interfacemethodref-info
					//class_index 		2byte
					//name&type_index  	2byte
				byteNum = 4;
				break;
			case 8:	//String-info
					//string_index 		2byte
				byteNum = 2;
				break;
			case 3:	//Integer-info
			case 4:	//Float-info
					//bytes				4byte
				byteNum = 4;
				break;
			case 5:	//Long-info
			case 6:	//Double-info
					//Hight_bytes	4byte
					//Low_bytes		4byte
				byteNum = 8;
				break;
			case 12://Name&Type-info
					//name_index		2byte
					//discriptor_index	2byte
				byteNum = 4;
				break;
			case 1:	//Utf8-info
					//length	2byte
					//bytes		1byte * length
				byteNum = 2;
				break;
			default:
				index--;
		}
		//get byte (* byteNum)
		for(int i=0; i<byteNum; i++){
			info.add(bytes.get(index++));
		}
		//Utf8-info only
		if(cp_info_tag == 1){
			int bl = ByteFunc.getInt(info.get(0), info.get(1));
			for(int i=0; i<bl; i++){
				info.add(bytes.get(index++));
			}
		}
		next_index = index;
		
		//System.out.print(" "+(cp_info_tag&0xff));
		//if(cp_info_tag == 1)
			//System.out.print("("+this.getUTF8()+")");
	}
	
	
	//getter 
	public int getNextIndex(){
		return next_index;
	}
	public Byte getTag(){
		return cp_info_tag;
	}
	public int getInfoSize(){
		return info.size();
	}
	public Byte[] getInfo(){
		return info.toArray(new Byte[0]);
	}
	public String getUTF8(){
		if(cp_info_tag != 1) return null;
		int length = ByteFunc.getInt(info.get(0), info.get(1));
		byte[] bytes = new byte[length];
		for(int i=0; i<length; i++){
			bytes[i] = info.get(i+2);
		}
		return new String(bytes);
	}
	
}

