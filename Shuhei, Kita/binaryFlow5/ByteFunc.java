import java.util.ArrayList;
import java.util.List;

public class ByteFunc{
	//	byte list
	//	start index
	//	byte number
	public static int getInt(Byte b1, Byte b2){
		
		int l1 = (b1.intValue()&0xff) * 256;
		int l2 = (b2.intValue()&0xff);
		int l = l1+l2;
		return l;
	}
	
	public static int getInt(Byte[] bytes){
		int num = 0;
		for(int i=0; i<bytes.length; i++){
			num = num*256 + (bytes[i].intValue()&0xff);
		}
		return num;
	}
	
	//編集距離　ゲット
	int edit(Byte[] bytes1, Byte[] bytes2){
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
	 return result;
	}
}
