/*
 * javaVM マシンコードデータ。
 * <pre>
 * machine code object.
 * <pre>
 */
import java.util.*;
public class VMCode {
	/* @serial
	 * 
	 */
	private int index = -1;
	private int binary;
	private String name;
	private int num;
	private Byte[] bytes = new Byte[0];
	private int jump = -1;
	private String invoke = null;
	private String invoke_class = null;
	
	private int x = -1;
	private int y = -1;
	
	private int deep = 0;
	
	private static VMCode[] VMCodes;
	
	/*
	 * read csv file & create code object.
	 * @param sts csvFile
	 */
	public static void setVMCodes(String[] sts){
		VMCodes = new VMCode[sts.length];
		for(int i=0; i<sts.length; i++){
			VMCodes[i] = new VMCode(sts[i]);
		}
	}
	/*
	 * get code object list.
	 * @return list of code object
	 */
	public static VMCode[] getVMCodes(){
		return VMCodes;
	}
	/*
	 * マシンコードテーブル用。
	 * @param st csvファイルの行。
	 */
	VMCode(String st){
		String[] sts = st.split(",");
		binary = Integer.parseInt(sts[0]);
		name = sts[1];
		num = Integer.parseInt(sts[2]) - 1;
	}
	/*
	 * メソッドの命令バイト列用
	 * @param b 命令バイト
	 * @param d 命令バイト列でのindex
	 */
	VMCode(Byte b, int d){
		index = d;
		int d1 = (b.intValue()&0xff);
		for(int i=0; i<VMCodes.length; i++){
			if(VMCodes[i].getBinary() == d1){
				binary = VMCodes[i].getBinary();
				name = VMCodes[i].getName();
				num = VMCodes[i].getNum();
				break;
			}
		}
	}
	
	
	///METHOD --------------------------------------
	/*
	 * デバッグ用。
	 */
	public void print(){
		System.out.println(index + " ["+String.format("%02X ", binary)+", "+name+"]");
		System.out.println(Arrays.toString(bytes));
		if(this.isIF())
			System.out.println("--IF to "+jump);
		else if(this.isLOOP())
			System.out.println("--LOOP to "+jump);
		else if(this.isGOTO())
			System.out.println("--GO to "+jump);
		else if(this.isINVOKE())
			System.out.println("--invoke "+invoke);
	}
	
	//is a IF
	public boolean isIF(){
		if(jump!=-1 && jump>index && binary!=167 && binary!=200)
			return true;
		return false;
	}
	//is a LOOP
	public boolean isLOOP(){
		if(jump!=-1 && jump<index && binary!=167 && binary!=200)
			return true;
		return false;
	}
	//is a GO_TO
	public boolean isGOTO(){
		if(jump!=-1 && jump<index && (binary==167 || binary==200) )
			return true;
		return false;
	}
	//is a invoke
	public boolean isINVOKE(){
		if(182<=binary && binary<=184)
			return true;
		return false;
	}
	
	//SETTER
	public void addBytes(Byte b){
		Byte[] array = new Byte[bytes.length+1];
		System.arraycopy(bytes, 0, array, 0, bytes.length);
		array[bytes.length] = b;
		bytes = array;
	}
	public void setJump(int d){
		jump = d;
	}
	/*
	 * 関数呼び出しの呼び出し先情報セット。
	 * @param cp コンスタントプール。
	 * @param num コンスタントプールのズレ。
	 */
	public void setInvoke(CPInfo[] cp, int num){
		//numは、cpのズレ
		String st;
		if(this.isINVOKE()){
			int d1 = this.getByteNum() -1 -num;
			int d3 = 0;
			boolean flag = false;
			while(true){
				//
				if(invoke==null){
					int d2 = (cp[d1].getTag().intValue()&0xff);
					//utf-8 only
					if(d2 == 1){
						invoke = cp[d1].getUTF8();
						//System.out.println("invoke: "+invoke);
					//class only
					}else if(d2 == 10){
						Byte[] bs = cp[d1].getInfo();
						d1 = ByteFunc.getInt(bs[2], bs[3]) -1 -num;
						//
						d3 = ByteFunc.getInt(bs[0], bs[1]) -1 -num;
						bs = cp[d3].getInfo();
						d3 = ByteFunc.getInt(bs[0], bs[1]) -1 -num;
						invoke_class = cp[d3].getUTF8();
					}else{
						Byte[] bs = cp[d1].getInfo();
						d1 = ByteFunc.getInt(bs[0], bs[1]) -1 -num;
					}
				}
				if(invoke!=null && invoke_class!=null)
					break;
			}
		}
	}
	public void setXY(int X, int Y){
		x = X;
		y = Y;
	}
	public void setDeep(int d){
		deep = d;
	}
	
	//GETTER
	public int getIndex(){
		return index;
	}
	public int getBinary(){
		return binary;
	}
	public String getName(){
		return name;
	}
	public int getNum(){
		return num;
	}
	public Byte[] getBytes(){
		return bytes;
	}
	public int getByteNum(){
		return ByteFunc.getInt(bytes);
	}
	public int getJump(){
		return jump;
	}
	public int getX(){
		return x;
	}
	public int getY(){
		return y;
	}
	public String getInvoke(){
		return invoke;
	}
	public String getInvokeClass(){
		return invoke_class;
	}
	public int getDeep(){
		return deep;
	}
}
