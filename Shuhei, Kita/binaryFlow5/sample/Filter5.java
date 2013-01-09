import java.util.*;
//アルゴリズム　違い
public class Filter5 {
	public static void main(String[] args){
		int[] array = new int[] { 5, 4, 6, 7, 1, 9, 8, 3, 2 };
	
		array = filter_5(array, 4);
		System.out.println(Arrays.toString(array));
	}
	public static int[] filter_5(int[] array, int num){
		List<Integer> list = getList(array, num);
		
		return getArray(list);
	}
	
	public static List<Integer> getList(int[] array, int num){
		List<Integer> list = new ArrayList<Integer>();
		for(int i=0; i<array.length; i++){
			if(array[i] <= num){
				list.add(new Integer(array[i]));
			}
		}
		return list;
	}
	public static int[] getArray(List<Integer> list){
		int[] array = new int[list.size()];
		for(int i=0; i<list.size(); i++){
			array[i] =  list.get(i).intValue();
		}
		return array;
	}
}
