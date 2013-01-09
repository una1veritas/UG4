import java.util.List;


public class FieldInfo{
	private int ID;
	private Byte[] access_flags 	= new Byte[2];
	//private Byte[] name_index 		= new Byte[2];
	private int name_index;
	//private Byte[] descriptor_index = new Byte[2];
	private int descriptor_index;
	//private Byte[] attributes_count = new Byte[2];
	private int attributes_count;
	private AttributeInfo[] attributes;
	private int next_index;
	
	FieldInfo(List<Byte> bytes, int index, int id){
		ID = id;
		//access_flags
		access_flags[0] = bytes.get(index++);
		access_flags[1] = bytes.get(index++);
		//name_index
		name_index = ByteFunc.getInt(bytes.get(index++), bytes.get(index++)) - 1;
		//name_index[0] = bytes.get(index++);
		//name_index[1] = bytes.get(index++);
		//descriptor_index
		//descriptor_index[0] = bytes.get(index++);
		//descriptor_index[1] = bytes.get(index++);
		descriptor_index = ByteFunc.getInt(bytes.get(index++), bytes.get(index++)) - 1;
		//attributes_count
		//attributes_count[0] = bytes.get(index++);
		//attributes_count[1] = bytes.get(index++);
		attributes_count = ByteFunc.getInt(bytes.get(index++), bytes.get(index++));
		
		//attributes
		attributes = new AttributeInfo[attributes_count];
		for(int i=0; i<attributes_count; i++){
			attributes[i] = new AttributeInfo(bytes, index, ID);
			index = attributes[i].getNextIndex();
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


