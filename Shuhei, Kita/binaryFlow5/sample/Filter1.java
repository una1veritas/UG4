import java.util.Arrays;

//関数名・変数名変更
public class Filter1 {
	public static void main(String[] args){
		int[] array = new int[] { 5, 4, 6, 7, 1, 9, 8, 3, 2 };
	
		array = filter_1(array, 4);
		System.out.println(Arrays.toString(array));
	}
	public static int[] filter_1(int[] array, int num){
		int NUM = 0;
		for(int i=0; i<array.length; i++){
			if(array[i] <= num){
				NUM++;
			}
		}
		int[] array1 = new int[NUM];
		for(int i=array.length-1; i>-1; i--){
			if(array[i] <= num){
				array1[--NUM] = array[i];
			}
		}
		return array1;
	}
}
