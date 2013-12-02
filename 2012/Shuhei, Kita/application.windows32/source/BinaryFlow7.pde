import java.awt.event.*;
import java.awt.datatransfer.DataFlavor;  
import java.awt.datatransfer.Transferable;  
import java.awt.datatransfer.UnsupportedFlavorException;  
import java.awt.dnd.DnDConstants;  
import java.awt.dnd.DropTarget;  
import java.awt.dnd.DropTargetDragEvent;  
import java.awt.dnd.DropTargetDropEvent;  
import java.awt.dnd.DropTargetEvent;  
import java.awt.dnd.DropTargetListener;  
import org.apache.commons.math3.*;
import java.awt.event.*;

//view Mode
int CLASS = 1;
int METHOD = 2;
int VIEW_MODE = METHOD;

//graph area
int plotX1;
int plotY1;
int plotX2;
int plotY2;

//plot center
int centerX;
int centerY;

//matrix
double maxX;
double maxY;

//select program
boolean Select = false;
int selectX1 = -1;
int selectY1 = -1;
int selectX2 = -1;
int selectY2 = -1;

//class obj list
List<ClassRead> classes;// = new ArrayList<ClassData>();
int IDCount;

int maxMemory;

//event
boolean selected;
List<Integer> clicks;
float zoomLv = 1.0;
boolean Trans = false;
boolean Drag = false;
boolean Selected;
int VX, VY;
int lastX, lastY; 

double LoopBias;

//select directory
String home;

//draw
int fileNameW = 30;
int r = 8;

void setup(){ 
	size(700,700); 
	smooth();
	selected = false;
	plotX1 = 50;
	plotY1 = 50;
	plotX2 = width-50;
	plotY2 = height-100;
	centerX = 300;
	centerY = 300;
	//getVNCode
	String[] vmList = loadStrings("./VMCode.csv");
	VMCode.setVMCodes(vmList);
	
	//過去のフォルダー選択
	String[] logs = loadStrings("log.txt");
	if(logs!=null && logs.length>0 && logs[0]!=""){
		home = logs[0];
		setData();
		selected =true;
	}
	//フォルダーセレクト 
	if(home==null)
		selectDir("Select a folder to visualizing:");
	
	//ホイールイベント 
	addMouseWheelListener(new MouseWheelListener(){
	    public void mouseWheelMoved(MouseWheelEvent mwe){
	      mouseWheel(mwe.getWheelRotation());
	    }
	});
	//
	//ファイル　ドラッグ&ドロップ 
	DropTarget dropTarget = new DropTarget(this, new DropTargetListener() {  
	    public void dragEnter(DropTargetDragEvent dtde) {}  
	    public void dragOver(DropTargetDragEvent dtde) {}  
	    public void dropActionChanged(DropTargetDragEvent dtde) {}  
	    public void dragExit(DropTargetEvent dte) {}  
	    public void drop(DropTargetDropEvent dtde) {  
	      dtde.acceptDrop(DnDConstants.ACTION_COPY_OR_MOVE);  
	      Transferable trans = dtde.getTransferable();  
	      List<File> fileNameList = null;  
	      if(trans.isDataFlavorSupported(DataFlavor.javaFileListFlavor)) {  
	        try {  
	          fileNameList = (List<File>)  
	            trans.getTransferData(DataFlavor.javaFileListFlavor);  
	        } catch (UnsupportedFlavorException ex) {  
	          /* 例外処理 */  
	        } catch (IOException ex) {  
	          /* 例外処理 */  
	        }  
	      }  
	      if(fileNameList == null) return;  
	      // ドラッグ&ドロップされたファイルの一覧は一時リストに格納される  
	      // 以下のように書くと、ファイルのフルパスを表示  
	      //for(File f : fileNameList) println(f.getAbsolutePath()); 
	      if(fileNameList.size() == 1   &&  fileNameList.get(0).isDirectory()){
	      	folderSelected(fileNameList.get(0));
	      }else if(fileNameList.size() > 0){
	      	addClass(fileNameList);
	      }
	  }  
	}); 
} 

//データ取得処理**************************************************************************
//folder選択  
/*
* ファイルシステム呼び出し。
* @param txt ファイルシステムのトップの表示テキスト
*/
void selectDir(String txt){
	selectFolder(txt, "folderSelected");
}
/*
* フォルダー選択後の処理。
*/
boolean folderSelected(File selection){
	if (selection == null) {
    	//selectDir("Select a folder to visualizing:");
  	} else {
    	home = selection.getAbsolutePath();
    	println("User selected " + home);
    	PrintWriter pw = createWriter("./data/log.txt");
    	pw.println(home);
    	pw.flush(); 
    	pw.close();
    	
    	//データ取得  
    	setData();
    	if(classes.size() > 0){
    		selected = true;
    	}
    	return true;
  	}
  	return false;
}
/*
* クラスの各種データのセット。
*/
void setData(){
	//GET class data
	setClass(home);
	setXY();
}
/*
* classファイルの追加。
* @param f 追加するファイルリスト
*/
void addClass(List<File> f){
	for(int i=0; i<f.size(); i++){
		if(f.get(i).getName().matches(".*\\.class")){
			if(f.get(i).isFile()){
				String path = f.get(i).getAbsolutePath();
				println(path);
				path = path.substring(0, path.lastIndexOf("\\"));
				ClassRead cl = new ClassRead(path, f.get(i).getName(), IDCount);
				//cl.print();
				if(!cl.check()){
					if(classes == null){
						classes = new ArrayList<ClassRead>();
					}
					classes.add(cl);
					IDCount++;
					println("add: "+f.get(i).getName());
				}
			}
		}
	}
	IDCount = classes.size();
	if(IDCount > 0){
		fileNameW = (height-80)/classes.size();
		if(fileNameW > 30) fileNameW = 30;
		//set color
		for(int i=0; i<classes.size(); i++){
			classes.get(i).setHue( (int)(100*i/classes.size()) );
		}
		setXY();
		selected = true;
	}
}
/*
* classファイルの読み込み。
* @param p 対象ディレクトリ。
*/
void setClass(String p){
	classes = new ArrayList<ClassRead>();
	try{
		int ID = 0;
		File fp = new File(p);
		String[] fList = fp.list();
		println("file num: "+fList.length);
		for(int i=0; i<fList.length; i++){
			if(fList[i].matches(".*\\.class")){
				println("");
				println("FILE name: "+fList[i]);
				ClassRead cl = new ClassRead(p, fList[i], ID++);
				//cl.print();
				if(!cl.check()){
					classes.add(cl);
				}
			}
		}
		IDCount = classes.size();
		fileNameW = (height-80)/classes.size();
		if(fileNameW > 30) fileNameW = 30;
		//set color
		for(int i=0; i<classes.size(); i++){
			classes.get(i).setHue( (int)(100*i/classes.size()) );
		}
	}catch(Exception e){
		println("ERROR: "+e);
	}
}
/*
* メソッドのメモリサイズ最大量取得。
*/
void getMaxSize(){
	maxMemory = 0;
	for(int i=0; i<classes.size(); i++){
		for(int j=0; j<classes.get(i).getCode().length; j++){
			int ds = classes.get(i).getCode()[j].getMaxStack();
			int dl = classes.get(i).getCode()[j].getMaxLocal();
			if(ds+dl > maxMemory)
				maxMemory = ds+dl;
		}
	}
}
/*
* メソッドの位置をセット
*/
void setXY(){
	if(classes.size()>0){
		//get maxStack & maxLocal
		getMaxSize();
		//ループバイアスセット  
		classes.get(0).getCode()[0].setBias(LoopBias);
		println("OK: "+classes.size());
		//get distance & XY
		double[][] XY = PCA.getXY(distance());
		//get maxXY
		maxX = -1;
	  	maxY = -1;
	  	for(int i=0; i<XY.length; i++){
	    	if(Math.abs(XY[i][0]) > maxX) maxX = Math.abs(XY[i][0]);
	    	if(Math.abs(XY[i][1]) > maxY) maxY = Math.abs(XY[i][1]); 
	  	}
		//get xyMatrix
		int w = (width-100)/2 - 50;
		int h = (height-100)/2 -50;
		int index = 0;
		for(int i=0; i<classes.size(); i++){
			ClassRead cl = classes.get(i);
			if(cl.isView()){
				for(int j=0; j<cl.getCode().length; j++){
					CodeAttribute code = cl.getCode()[j];
					if(code.isView()){
						code.setXY( (int)(XY[index][0]/maxX*w)+centerX, (int)(XY[index][1]/maxY*h)+centerY );
						index++;
					}
				}
			}
			//重心    
			cl.setXY();
		}
	}
}
/*
* メソッド間距離の計算
* @return double[][] メソッド間の距離 
*/
double[][] distance(){
	//可視化対象となるメソッドリスト
	List<CodeAttribute> viewCodes = new ArrayList<CodeAttribute>();
	for(int i=0; i<classes.size(); i++){
		if(classes.get(i).isView()){
			ClassRead cl = classes.get(i);
			for(int j=0; j<cl.getCode().length; j++){
				CodeAttribute code = cl.getCode()[j];
				if(code.isView()){
					viewCodes.add(code);
				}
			}
		}
	}
	if(viewCodes.size() > 1){
		//focus
		clicks = new ArrayList<Integer>();
		for(int i=0; i<classes.size(); i++)
			classes.get(i).setFocus(1);
		
		//get  method count 
		int len = viewCodes.size();
		//get distance
		double[][] datas = new double[len][len];
		for(int i=0; i<len; i++){
			datas[i][i] = 0;
			for(int j=i+1; j<len; j++){
				CodeAttribute c1 = viewCodes.get(i);
				CodeAttribute c2 = viewCodes.get(j);
				double dis = (double)c1.edit_codeSet(c2);
				//if(dis != 0)
					//dis = Math.log(dis);
				//println(dis);
				datas[i][j] = dis;
				datas[j][i] = dis;
				//debug
				//println("("+c1.getID()+"."+c1.getName() +", "+c2.getID()+"."+c2.getName()+") = "+dis);
			}
		}
		return datas;
	}
	return null;
}
//*データ取得処理　ここまで**************************************************************************
/*
* 描画メソッドの大元。
*/
void draw(){ 
	colorMode(RGB, 255);
	background(255);
	drawPanel();
	if(selected){
		colorMode(HSB, 100);
		textSize(15);
		for(int i=0; i<classes.size(); i++){
			drawClass(classes.get(i));
		} 
		//select
		if(Drag && Select){
			fill(70, 100,100,30);
			rect(getViewX(selectX1), getViewY(selectY1), mouseX-getViewX(selectX1), mouseY-getViewY(selectY1));
		}
	}
} 

//mouse EVENT------------------------
/*
* マウスの移動時イベント。
*/
void mouseMoved(){
	//file name focus
	if(selected){
	for(int i=0; i<classes.size(); i++){
		classes.get(i).setOnName(false);
	}
	if(width-100<mouseX){
		int num = (int)Math.floor((mouseY-30)/fileNameW);
		if(num < classes.size() && num>=0){
			classes.get(num).setOnName(true);
		}
	}
	}
}
/*
* マウスのドラッグ時イベント。
*/
void mouseDragged(){
	//translate
	if(selected && Drag && !Select && mouseButton!=RIGHT){
		VX += mouseX - lastX;
		VY += mouseY - lastY;
		lastX = mouseX;
		lastY = mouseY;
	}
}
/*
* マウスボタンが離された時のイベント。
*/
void mouseReleased(){
	if(selected && Drag && !Select && mouseButton!=RIGHT){
		Drag = false;
		lastX = 0;
		lastY = 0;
	}else if(selected && Drag && Select && mouseButton!=RIGHT){
		Drag = false;
		clicks = new ArrayList<Integer>();
		selectX2 = getMouseX();
		selectY2 = getMouseY();
		boolean change = false;
		List<List<Integer>> ls = getViewList();
		
		for(int i=0; i<classes.size(); i++){
			boolean flag = false;
			ClassRead cl = classes.get(i);
			if(cl.isView()){
				//METHOD
				if(VIEW_MODE==METHOD){
					for(int j=0; j<cl.getCode().length; j++){
						CodeAttribute code = cl.getCode()[j];
						if(code.isView()){
							int x = code.getX();
							int y = code.getY();
							if(selectX1<x && selectY1<y  &&  x<selectX2 && y<selectY2){
								flag = true;
								code.setView(true);
							}else{
								code.setView(false);
							}
						}
					}
				//CLASS
				}else if(VIEW_MODE==CLASS){
					int x = cl.getX();
					int y = cl.getY();
					if(selectX1<x && selectY1<y  &&  x<selectX2 && y<selectY2){
						flag = true;
					}
				}
				if(flag){
					change = true;
					clicks.add(new Integer(i));
				}else{
					cl.setView(false);
				}
			}
		}
		if(change){
			changeFocus();
			Select = false;
		}else{
			setViewList(ls);
		}
	}
	
}
/*
* マウスボタンが押された時のイベント。
*/
void mousePressed(){
	//
	//translate
	if(selected){
	if(width-100>mouseX  && plotY2>mouseY && !Select  &&  mouseButton!=RIGHT){
		Drag = true;
		lastX = mouseX;
		lastY = mouseY;
	}
	//select
	if(width-100>mouseX && plotY2>mouseY && Select  &&  mouseButton!=RIGHT){
		Drag = true;
		selectX1 = getMouseX();
		selectY1 = getMouseY();
	}
	//select cancel
	if(mouseButton == RIGHT){
		clicks = new ArrayList<Integer>();
		for(int i=0; i<classes.size(); i++)
			classes.get(i).setFocus(1);
	}else{
		//file name click
		if(width-100<mouseX){
			int num = (int)Math.floor((mouseY-(50-fileNameW/2))/fileNameW);
			if(num < classes.size() && num>=0){
				//add || remove
				if(classes.get(num).isView()){
					int d = clicks.indexOf(new Integer(num)); 
					if(d == -1){
						clicks.add(new Integer(num));
					}else{
						clicks.remove(d);
					}
				}
				//classes focs change
				changeFocus();
			}
			println(Arrays.toString(clicks.toArray(new Integer[0])));
		}
	}
	}
	//操作パネル操作  
	if(mouseButton==LEFT && mouseY>plotY2){
		int left = 0;
		int p_w = (int)((width-100)/5);
		int p_h = 100;
		//redraw
		if(left<mouseX && mouseX<left+p_w){
			if(clicks.size() == 0){
				for(int i=0; i<classes.size(); i++){
					classes.get(i).setView(true);
					for(int j=0; j<classes.get(i).getCode().length; j++){
						classes.get(i).getCode()[j].setView(true);
					}
				}
			}else{
				for(int i=0; i<classes.size(); i++)
					classes.get(i).setView(false);
				for(int i=0; i<clicks.size(); i++){
					classes.get(clicks.get(i)).setView(true);
				}
			}
			clicks = new ArrayList<Integer>();
			setXY();
		}
		left+=p_w;
		//trans & select
		if(left<mouseX && mouseX<left+p_w){
			if(mouseY<plotY2+p_h/2) Select=false;
			else Select=true;
		}
		left+=p_w;
		//class or method
		if(left<mouseX && mouseX<left+p_w){
			if(mouseY<plotY2+p_h/2) VIEW_MODE = CLASS;
			else VIEW_MODE = METHOD;
		}
		left+=p_w;
		//BIAS
		if(left<mouseX && mouseX<left+p_w){
			for(int i=0; i<3; i++){
				if(mouseY>plotY2+p_h/2 && left+i*p_w/3<mouseX && mouseX<left+(i+1)*p_w/3){
					if(LoopBias != i*0.5){
						LoopBias = i*0.5;
						//ループバイアスセット  
						if(classes.size()>0)
							classes.get(0).getCode()[0].setBias(LoopBias);
						setXY();
					}
				}
			}
		}
		left+=p_w;
		if(left<mouseX && mouseX<left+p_w){
			selectDir("Select a folder to visualizing:");
		}
		
	}
	
}
//claases focus change
/*
* クラスのフォーカス（透過率）更新。
*/
void changeFocus(){
	for(int i=0; i<classes.size(); i++){
		if(clicks.size() > 0){
			if(clicks.indexOf(new Integer(i)) == -1)
				classes.get(i).setFocus(0);
			else 
				classes.get(i).setFocus(2);
		}else{
			classes.get(i).setFocus(1);
		}
	}
} 
//transrate
/*
* 画面の平行移動。
*/
void trans(int vx, int vy){
	translate((int)(vx + (1.0-zoomLv)*width/2), (int)(vy + (1.0-zoomLv)*(height-30)/2));
	scale(zoomLv);
	//translate(vx, vy);
}
/*
* 画面の平行移動(戻す際に使用)。
*/
void retrans(int vx, int vy){
	scale(1/zoomLv);
	translate(-vx - (1.0-zoomLv)*width/2, -vy - (1.0-zoomLv)*(height-30)/2);
	//translate(vx, vy);
}
//get mouseXY translated
/*
* マウスの押された位置取得　（マッピング座標上の）。
*/
int getMouseX(){
	return mouseX-VX;
}
/*
* マウスの押された位置取得　（マッピング座標上の）。
*/
int getMouseY(){
	return mouseY-VY;
}
/*
* マッピング座標から画面上座標への変換。
*/
int getViewX(int x){
	return x+VX;
}
/*
* マッピング座標から画面上座標への変換。
*/
int getViewY(int y){
	return y+VY;
}
//clear view  class&method
/*
* クラス・メソッドの選択の初期化
*/
void clearView(){
	for(int i=0; i<classes.size(); i++){
		classes.get(i).setView(false);
		for(int j=0; j<classes.get(i).getCode().length; j++){
			classes.get(i).getCode()[j].setView(false);
		}
	}
}
//get view class & method befor select event
/*
* 現在表示されているメソッドリストの取得。
*/
List<List<Integer>> getViewList(){
	List<List<Integer>> ls = new ArrayList<List<Integer>>();
	for(int i=0; i<classes.size(); i++){
		List<Integer> ls1 = new ArrayList<Integer>();
		ClassRead cl = classes.get(i);
		if(cl.isView()){
			for(int j=0; j<cl.getCode().length; j++){
				CodeAttribute code = cl.getCode()[j];
				if(code.isView()){
					ls1.add(new Integer(j));
				}
			}
		}
		ls.add(ls1);
	}
	return ls;
}
//set view class & method
/*
* クラス・メソッドの表示可否の更新。
* @param ls 表示するクラスリスト
*/
void setViewList(List<List<Integer>> ls){
	clearView();
	for(int i=0; i<ls.size(); i++){
		if(ls.get(i).size() > 0)
			classes.get(i).setView(true);
		for(int j=0; j<ls.get(i).size(); j++){
			classes.get(i).getCode()[ls.get(i).get(j).intValue()].setView(true);
		}
	}
}
////mouse event----END-----------
///key EVENT-----------------------
/*
* キーイベント。
*/ 
void keyPressed(){
	if(key=='1'){
		if(VIEW_MODE==CLASS)
			VIEW_MODE = METHOD;
		else if(VIEW_MODE==METHOD)
			VIEW_MODE = CLASS;
	}
	if(key=='2'){
		retrans(VX,VY);
		VX = 0;
		VY = 0;
		zoomLv = 1.0;
	}
	if(key=='3'){
		if(clicks.size() == 0){
			for(int i=0; i<classes.size(); i++){
				classes.get(i).setView(true);
				for(int j=0; j<classes.get(i).getCode().length; j++){
					classes.get(i).getCode()[j].setView(true);
				}
			}
		}else{
			for(int i=0; i<classes.size(); i++)
				classes.get(i).setView(false);
			for(int i=0; i<clicks.size(); i++){
				classes.get(clicks.get(i)).setView(true);
			}
		}
		clicks = new ArrayList<Integer>();
		setXY();
	}
	if(key=='4'){
		if(!Select){
			Select = true;
			selectX1=selectX2=selectY1=selectY2 = -1;
		}else{
			Select = false;
			selectX1=selectX2=selectY1=selectY2 = -1;
		}
	}
	//ループバイアス　変更
	if(key=='z'){
		if(LoopBias==0.0) LoopBias=0.0;
		else if(LoopBias==0.5) LoopBias=0.0;
		else if(LoopBias==1.0) LoopBias=0.5;
		println("LOOP BIAS "+LoopBias);
		//ループバイアスセット  
		if(classes.size()>0)
			classes.get(0).getCode()[0].setBias(LoopBias);
		
		setXY();
	}
	if(key=='x'){
		if(LoopBias==0.0) LoopBias=0.5;
		else if(LoopBias==0.5) LoopBias=1.0;
		else if(LoopBias==1.0) LoopBias=1.0;
		println("LOOP BIAS "+LoopBias);
		//ループバイアスセット  
		if(classes.size()>0)
			classes.get(0).getCode()[0].setBias(LoopBias);
		setXY();
	}
	if(key=='s'){
		save("./data/outpic.jpg");
	}
}

/////key EVENT---END--------------


/*
* ズームイン・アウト。
*@param delta ホイールの回転量
*/
void mouseWheel(int delta){
  zoomLv -= (float)(delta/20.0);
  if(zoomLv < 0.1)
    zoomLv = 0.1;
  if(zoomLv > 10.0)
    zoomLv = 10.0;
}
