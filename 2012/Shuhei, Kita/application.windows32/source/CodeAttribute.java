/*
 * コードアトリビュートクラス。
 * メソッドの実際の処理に関する情報保持。
*/
import java.util.*;
public class CodeAttribute{
	public int ID;
	private String method_name;
	private Byte[] code_length 	= new Byte[4];
	private Byte[] code;
	private String[] invoke;
	private VMCode[] mnemonic;
	private Byte[] code_set;	//命令セット  
	private int max_stack;
	private int max_local;
	private int max_memory;
	
	private boolean view = true;
	private int x = -1;
	private int y = -1;
	private int hue = 0;
	private int alpha = 0;
	
	private static int LOOP = 0;
	private static int IF = 1;
	private static int GO = 2;
	private static double BIAS = 0.0;
	
	private int maxJump = 0;
	private int[][] jump;
	private int jumpTotal;
	
	private static double LOOP_BIAS;
	
	//CONSTRACOR
	CodeAttribute(String mName, Byte[] bytes, int id){
		method_name = mName;
		ID = id;
		int index = 0;
		//max_stack
		//max_locals
		max_stack = ByteFunc.getInt(bytes[index++], bytes[index++]);
		max_local = ByteFunc.getInt(bytes[index++], bytes[index++]);
		//System.out.println("max:"+max_stack+"_ "+max_local);
		max_memory = max_stack + max_local;
		//code_length
		code_length[0] = bytes[index++];
		code_length[1] = bytes[index++];
		code_length[2] = bytes[index++];
		code_length[3] = bytes[index++];
		
		//code
		int clength = ByteFunc.getInt(code_length);
		code = new Byte[clength];
		for(int i=0; i<clength; i++){
			code[i] = bytes[index++];
		}
		
		//set nimonic
		List<VMCode> ls = new ArrayList<VMCode>();
		//命令セット 
		List<Byte> ls1 = new ArrayList<Byte>();
		
		index = 0;
		while(index < code.length){
			VMCode mnemo = new VMCode(code[index], index);
			ls1.add(code[index]);
			index++;
			int count = 0;
			while(count < mnemo.getNum()){
				mnemo.addBytes(code[index++]);
				count++;
			}
			//kind
			///IF
			if( (153<=mnemo.getBinary()&&mnemo.getBinary()<=166) || (198<=mnemo.getBinary()&&mnemo.getBinary()<=199) ){
				int d1 = ByteFunc.getInt(mnemo.getBytes());
				if(d1 > Math.pow(2,15)){
					d1 -= (int)Math.pow(2,16);
				}
				//get max
				if(maxJump>d1)
					maxJump = d1;
				mnemo.setJump( d1 + mnemo.getIndex() );
			}
			///GO_TO
			else if(mnemo.getBinary()==167 || mnemo.getBinary()==200){
				int d2 = 31;
				if(mnemo.getBinary()==167)
					d2 = 15;
				
				int d1 = ByteFunc.getInt(mnemo.getBytes());
				if(d1 > Math.pow(2,d2)){
					d1 -= (int)Math.pow(2,d2+1);
				}
				//get max
				if(maxJump>d1)
					maxJump = d1;
				mnemo.setJump( d1 + mnemo.getIndex() );
			}
			
			//
			ls.add(mnemo);
		}
		mnemonic = ls.toArray(new VMCode[0]);
		code_set = ls1.toArray(new Byte[0]);
		
		//jump set
		setJump();
		//printJump();
		
	}
	///constract --END ----
	
	//METHOD
	public static void setBias(double bias){
		LOOP_BIAS = bias;
	}
	///print
	public void print(){
		System.out.println("------------------------");
		System.out.println(ID+": "+method_name);
		for(int i=0; i<mnemonic.length; i++){
			mnemonic[i].print();
		}
	}
	
	//SETTER
	public static void setBIAS(double b){
		BIAS += b;
		if(BIAS >1) BIAS = 1.0;
		else if(BIAS < 0) BIAS = 0.0;
	}
	public void setView(boolean b){
		view = b;
	}
	//set (x,y)
	public void setXY(int X, int Y){
		x = X;
		y = Y;
		
		//set mnemonic XY
		int D = 0;
		int dis = 10;
		int r = 4;
		List<Integer> next = new ArrayList<Integer>();
		for(int i=0; i<mnemonic.length; i++){
			//IF end
			int n = next.indexOf(new Integer(mnemonic[i].getIndex()));
			if( n >-1){
				D -= dis;
				next.remove(n);
			}
			//IF start
			if(mnemonic[i].isIF()){
				D += dis;
				next.add(mnemonic[i].getJump());
			}
			
			mnemonic[i].setXY(X+D ,Y+r*i);
		}
	}
	private void setJump(){
		List<VMCode> loops = new ArrayList<VMCode>();
		for(int i=0; i<mnemonic.length; i++){
			VMCode vc = mnemonic[i];
			if(vc.isLOOP()){
				int max_deep = 0;
				int index = -1;
		    	for(int l=0; l<loops.size(); l++){
			    	//this mnemonic > loops[m]
			    	if(vc.getIndex()>loops.get(l).getIndex() && loops.get(l).getIndex()>vc.getJump()){
			    		if(max_deep < loops.get(l).getDeep()){
			    			max_deep = loops.get(l).getDeep();
			    			index = l;
			    		}
			    	}
			    }
		    	vc.setDeep(1+max_deep);
		    	if(index != -1) loops.remove(index);
			    loops.add(vc);
			}
		}
		jump = new int[loops.size()][2];
		for(int i=0; i<loops.size(); i++){
			int index1 = 0;
			for(int j=0; j<mnemonic.length; j++){
				if(loops.get(i).getIndex() == mnemonic[j].getIndex())
					index1 = j;
			}
			jump[i][0] = loops.get(i).getDeep();
			jump[i][1] = index1 - this.getJumpIndex(loops.get(i));
		}
		jumpTotal = 0;
		for(int i=0; i<jump.length; i++){
			jumpTotal += (jump[i][0]*2-1)*jump[i][1];
		}
	}
	
	
	/////GETTER
	//get ID
	public int getID(){
		return ID;
	}
	//get invoke
	public String[] getInvoke(){
		List<String> list = new ArrayList<String>();
		for(int i=0; i<mnemonic.length; i++){
			if(mnemonic[i].isINVOKE()){
				list.add(mnemonic[i].getInvoke());
			}
		}
		return list.toArray(new String[0]);
	}
	//get invoke class
	public String[] getInvokeClass(){
		List<String> list = new ArrayList<String>();
		for(int i=0; i<mnemonic.length; i++){
			if(mnemonic[i].isINVOKE()){
				list.add(mnemonic[i].getInvokeClass());
			}
		}
		return list.toArray(new String[0]);
	}
	//get x
	public int getX(){
		return x;
	}
	//get y
	public int getY(){
		return y;
	}
	//get method_name
	public String getName(){
		return method_name;
	}
	//get code[]
	public Byte[] getCode(){
		return code;
	}
	public VMCode[] getMnemonic(){
		return mnemonic;
	}
	public Byte[] getCodeSet(){
		return code_set;
	}
	///get jump index
	public int getJumpIndex(VMCode mnemo){
		for(int i=0; i<mnemonic.length; i++){
			if(mnemonic[i].getIndex() == mnemo.getJump()){
				return i;
			}
		}
		return -1;
	}
	
	//get max_stack
	public int getMaxStack(){
		return max_stack;
	}
	//get max_local
	public int getMaxLocal(){
		return max_local;
	}
	//get max_memory
	public int getMaxMemory(){
		return max_memory;
	}
	//get view
	public boolean isView(){
		return view;
	}
	//get max jump
	private int getMaxJump(){
		return -1*maxJump;
	}
	//get jump total
	private int getJumpTotal(){
		return jumpTotal;
	}
	
	//print jump
	public void printJump(){
		for(int i=0; i<jump.length; i++)
			System.out.println(Arrays.toString(jump[i]));
	}
	
	//CodeAttribute edit distance
	/*
	 * バイト列の編集距離
	 */
	public int edit(CodeAttribute code1){
		//return ByteFunc.edit(code, code1.getCode());
		Byte[] bytes1 = code;
		Byte[] bytes2 = code1.getCode();
		int len1=bytes1.length,len2=bytes2.length;
		int[][] row = new int[len1+1][len2+1];
		int i,j;
		int result;
		for(i=0;i<len1+1;i++) row[i][0] = i;
			for(i=0;i<len2+1;i++) row[0][i] = i;
				for(i=1;i<=len1;++i){
	  				for(j=1;j<=len2;++j){
	   					row[i][j] = Math.min(Math.min(
	    					(Integer)(row[i-1][j-1])
	      					+ ((bytes1[i-1]==bytes2[j-1])?0:1) , // replace
	               			(Integer)(row[i][j-1]) + 1),     // delete
	            			(Integer)(row[i-1][j]) + 1);  // insert
	  		}
		}
		result=(Integer)(row[len1][len2]);
		//result += structureEdit(code1.getStructure()) * (bytes1.length+bytes2.length) * BIAS;
		//return result;
		return result + (int)(LOOP_BIAS * Math.abs(this.getMaxJump() - code1.getMaxJump()));
	}
	//CodeAttribute edit distance
	/*
	 * 命令バイト列の編集距離（引数を省く）
	 */
	public int edit_codeSet(CodeAttribute code1){
		//return ByteFunc.edit(code, code1.getCode());
		Byte[] bytes1 = code_set;
		Byte[] bytes2 = code1.getCodeSet();
		int len1=bytes1.length,len2=bytes2.length;
		int[][] row = new int[len1+1][len2+1];
		int i,j;
		int result;
		for(i=0;i<len1+1;i++) row[i][0] = i;
			for(i=0;i<len2+1;i++) row[0][i] = i;
				for(i=1;i<=len1;++i){
	  				for(j=1;j<=len2;++j){
	   					row[i][j] = Math.min(Math.min(
	    					(Integer)(row[i-1][j-1])
	      					+ ((bytes1[i-1]==bytes2[j-1])?0:1) , // replace
	               			(Integer)(row[i][j-1]) + 1),     // delete
	            			(Integer)(row[i-1][j]) + 1);  // insert
	  		}
		}
		result=(Integer)(row[len1][len2]);
		//result += structureEdit(code1.getStructure()) * (bytes1.length+bytes2.length) * BIAS;
		//return result;
		return result + (int)(LOOP_BIAS * Math.abs(jumpTotal - code1.getJumpTotal()));
	}
	
	//set invoke cp_Index
	public void setInvoke(CPInfo[] cp, int num){
		for(int i=0; i<mnemonic.length; i++){
			mnemonic[i].setInvoke(cp, num);
		}
	}
	
}
