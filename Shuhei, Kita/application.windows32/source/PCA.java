import org.apache.commons.math3.linear.*;


import java.util.*;
/*
* 主座標分析（principal coordinates analysis）
*/
import org.apache.commons.math3.*;
public class PCA{
	/*
	 * 主座標分析
	 * @param array ノード間の距離の対称行列
	 * @return ２次元マップ上の座標
	 */
	public static double[][] getXY(double[][] array){
		if(array == null) return null;
		
		RealMatrix matrix = MatrixUtils.createRealMatrix(array);
		//get value
	  	EigenDecomposition ed = new EigenDecomposition(matrix,1);
	  	double[] ls = ed.getRealEigenvalues();
	  	double[] ls2 = ed.getRealEigenvalues();
		int max = ls.length;
		
		Arrays.sort(ls);
		//println(ls);
		int k = 0;
		int[] index = getIndex(ls2, ls);
		double[][] array1 = new double[max][max];
		//固有値  ０を飛ばす 
		while(k<2){
			if(ls[max-1-k] == 0){
				double tmp = ls[max-1-k];
				int j;
				for(j=max-2-k; j>-1; j--){
					ls[j+1] = ls[j];
					if(ls[j+1] != 0){ j--; break;}
				}
				ls[j+1] = tmp;
			}
			if(ls[max-1-k] != 0) k++;
		}
		//固有値 
		for(int i=0; i<2; i++){
	    	array1[i][i] = ls[max-i-1];
	    }
	  	//get vector
	  	double[][] array2 = new double[max][max];
	  	for(int i=0; i<max; i++){
	   	 	double[] aa = ed.getEigenvector(i).toArray();
	    	for(int j=0; j<max; j++){
	      		array2[j][ index[i] ] = aa[j];
	    	}
	  	}

	  	//get x,y
	  	RealMatrix matrix1 = MatrixUtils.createRealMatrix(array1);
	  	RealMatrix matrix2 = MatrixUtils.createRealMatrix(array2);
	  	
	  	RealMatrix matrix3 = matrix2.multiply(matrix1);
	  	
	  	return matrix3.getData();
	}
	/*
	 * ソート前後で配列の添字の移動を捉える。
	 * @param ls1 ソート前の配列
	 * @param ls2 ソート後の配列
	 * @return ソート後の添字が前の添字ではどこを指すかの配列
	 */
	private static int[] getIndex(double[] ls1, double[] ls2){
		//init
		ArrayList array1 = new ArrayList();
		ArrayList array2 = new ArrayList();
		for(int i=0; i<ls1.length; i++){
			array1.add(ls1[i]);
			array2.add(ls2[i]);
		}
		ArrayList<Integer> array = new ArrayList<Integer>();
		//int[] ls = new int[ls1.length];
		for(int i=0; i<array.size(); i++){  array.add(-1);  }
		
		for(int i=0; i<array1.size(); i++){
			int index = array2.indexOf(array1.get(i));
			if( index>-1 ){
				while(true){
					if(!array.contains(index)) break;
					else{
						index++;
					}
				}
				array.add(index);
			}
		}
		int[] ls = new int[array.size()];
		for(int i=0; i<ls.length; i++){
			ls[i] = array.get(i).intValue();
		}
		return ls;
	}
}
