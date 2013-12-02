/*
 * アトリビュート（属性）クラス。
 */
import java.util.List;
public class AttributeInfo{
	private int ID;
	private int attribute_name_index;
	private int attribute_length;
	private Byte[] info;	//[attribute_length][1byte]
	
	private int next_index;
	
	AttributeInfo(List<Byte> bytes, int index, int id){
		ID = id;
		attribute_name_index = ByteFunc.getInt(bytes.get(index++), bytes.get(index++))-1;
		//attribute_length
		Byte[] array = new Byte[4];
		array[0] = bytes.get(index++);
		array[1] = bytes.get(index++);
		array[2] = bytes.get(index++);
		array[3] = bytes.get(index++);
		attribute_length = ByteFunc.getInt(array);
		
		//info
		info = new Byte[attribute_length];
		for(int i=0; i<attribute_length; i++){
			info[i] = bytes.get(index++);
		}
		
		//next_index
		next_index = index;
	}
	public int getNextIndex(){
		return next_index;
	}
	
	public int getNameIndex(){
		//System.out.println("cp index: "+ByteFunc.getInt(attribute_name_index[0], attribute_name_index[1]));
		return attribute_name_index;
	}
	/*
	 * コードアトリビュートの生成。
	 * @param name メソッドの名前
	 * @return CodeAttribute メソッドの実際の処理情報。
	 */
	public CodeAttribute getCode(String name){
		return  new CodeAttribute(name, info, ID);
		
	}
}


