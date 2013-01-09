import java.util.Arrays;

//宣言の前後関係・代入式  変更
public class Filter2 {
	public static int[] filter_2(int[] array, int num){
		int COUNT = 0;
		int[] array1;
		for(int i=0; i<array.length; i++){
			if(array[i] <= num){
				COUNT++;
			}
		}
		array1 = new int[COUNT];
		for(int i=array.length-1; i>-1; i--){
			if(array[i] <= num){
				array1[--COUNT] = array[i];
			}
		}
		return array1;
	}
	public static void main(String[] args){
		int[] array = new int[] { 5, 4, 6, 7, 1, 9, 8, 3, 2 };
	
		array = filter_2(array, 4);
		System.out.println(Arrays.toString(array));
	}
}