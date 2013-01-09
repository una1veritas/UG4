import java.util.Arrays;
//無意味な　変数宣言
public class Filter6 {
	public static void main(String[] args){
		int a,b,c,d,e;
		int[] array = new int[] { 5, 4, 6, 7, 1, 9, 8, 3, 2 };
	
		array = filter_6(array, 4);
		System.out.println(Arrays.toString(array));
	}
	public static int[] filter_6(int[] array, int num){
		int COUNT = 0;
		for(int i=0; i<array.length; i++){
			if(array[i] <= num){
				COUNT++;
			}
		}
		int[] array1 = new int[COUNT];
		for(int i=array.length-1; i>-1; i--){
			if(array[i] <= num){
				array1[--COUNT] = array[i];
			}
		}
		return array1;
	}
}
