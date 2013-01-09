import java.util.Arrays;
//関数分割
public class Filter4 {
	public static void main(String[] args){
		int[] array = new int[] { 5, 4, 6, 7, 1, 9, 8, 3, 2 };
	
		array = filter_4(array, 4);
		System.out.println(Arrays.toString(array));
	}
	public static int[] filter_4(int[] array, int num){
		int COUNT = getCount(array, num);
		
		
		return getArray(array, num, COUNT);
	}
	public static int getCount(int[] array, int num){
		int count = 0;
		for(int i=0; i<array.length; i++){
			if(array[i] <= num){
				count++;
			}
		}
		return count;
	}
	public static int[] getArray(int[] array, int num, int count){
		int[] array1 = new int[count];
		for(int i=array.length-1; i>=0; i--){
			if(array[i] <= num){
				array1[--count] = array[i];
			}
		}
		return array1;
	}
}
