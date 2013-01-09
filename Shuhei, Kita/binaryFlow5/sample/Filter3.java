import java.util.Arrays;

//ループ文変更
public class Filter3 {
	public static void main(String[] args){
		int[] array = new int[] { 5, 4, 6, 7, 1, 9, 8, 3, 2 };
	
		array = filter_3(array, 4);
		System.out.println(Arrays.toString(array));
	}
	public static int[] filter_3(int[] array, int num){
		int NUM = 0;
		int index = 0;
		while(index < array.length){
			if(array[index] <= num){
				NUM++;
			}
			index++;
		}
		int[] array1 = new int[NUM];
		int index1 = array.length-1;
		while(index1 >= 0){
			if(array[index1] <= num){
				array1[--NUM] = array[index1];
			}
			index1--;
		}
		return array1;
	}
}
