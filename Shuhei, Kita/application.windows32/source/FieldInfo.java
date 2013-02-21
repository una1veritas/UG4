/*
 * field & method クラス。
 */
import java.util.List;

public class FieldInfo{
	private int ID;
	private Byte[] access_flags 	= new Byte[2];
	private int name_index;
	private int descriptor_index;
	private int attributes_count;
	private AttributeInfo[] attributes;
	private int next_index;
	
	FieldInfo(List<Byte> bytes, int index, int id){
		//System.out.println("start: "+String.format("%02X ", index));
		ID = id;
		//access_flags
		access_flags[0] = bytes.get(index++);
		access_flags[1] = bytes.get(index++);
		//name_index
		name_index = ByteFunc.getInt(bytes.get(index++), bytes.get(index++)) - 1;
		descriptor_index = ByteFunc.getInt(bytes.get(index++), bytes.get(index++)) - 1;
		attributes_count = ByteFunc.getInt(bytes.get(index++), bytes.get(index++));
		//System.out.println("attribute count: "+attributes_count);
		
		//attributes
		attributes = new AttributeInfo[attributes_count];
		for(int i=0; i<attributes_count; i++){
			//debug
			//System.out.println("method's attribute  index: "+String.format("%02X ", index));
			attributes[i] = new AttributeInfo(bytes, index, ID);
			index = attributes[i].getNextIndex();
			//System.out.println("last: "+String.format("%02X ", index));
		}
		next_index = index;

	}
	public int getNextIndex(){
		return next_index;
	}
	public int getID(){
		return ID;
	}
	public int getNameIndex(){
		return name_index;
	}
	public int getDescriptorIndex(){
		return descriptor_index;
	}
	public AttributeInfo getAttribute(int index){
		return attributes[index];
	}
	public int getAttributeLength(){
		return attributes_count;
	}
	
	public void access(){
		System.out.println("flag : "+String.format("%02X ", access_flags[0])+" "+String.format("%02X ", access_flags[1]));
	}
}


