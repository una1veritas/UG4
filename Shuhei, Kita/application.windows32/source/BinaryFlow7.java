import processing.core.*; 
import processing.data.*; 
import processing.opengl.*; 

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

import org.apache.commons.math3.analysis.integration.*; 
import org.apache.commons.math3.util.*; 
import org.apache.commons.math3.stat.descriptive.summary.*; 
import org.apache.commons.math3.ode.nonstiff.*; 
import org.apache.commons.math3.geometry.partitioning.*; 
import org.apache.commons.math3.geometry.euclidean.twod.*; 
import org.apache.commons.math3.optimization.*; 
import org.apache.commons.math3.random.*; 
import org.apache.commons.math3.distribution.*; 
import org.apache.commons.math3.exception.util.*; 
import org.apache.commons.math3.stat.ranking.*; 
import org.apache.commons.math3.exception.*; 
import org.apache.commons.math3.linear.*; 
import org.apache.commons.math3.filter.*; 
import org.apache.commons.math3.analysis.*; 
import org.apache.commons.math3.stat.descriptive.rank.*; 
import org.apache.commons.math3.fraction.*; 
import org.apache.commons.math3.optimization.general.*; 
import org.apache.commons.math3.geometry.*; 
import org.apache.commons.math3.optimization.direct.*; 
import org.apache.commons.math3.geometry.euclidean.oned.*; 
import org.apache.commons.math3.stat.descriptive.*; 
import org.apache.commons.math3.dfp.*; 
import org.apache.commons.math3.genetics.*; 
import org.apache.commons.math3.stat.inference.*; 
import org.apache.commons.math3.*; 
import org.apache.commons.math3.stat.clustering.*; 
import org.apache.commons.math3.analysis.function.*; 
import org.apache.commons.math3.geometry.partitioning.utilities.*; 
import org.apache.commons.math3.optimization.linear.*; 
import org.apache.commons.math3.transform.*; 
import org.apache.commons.math3.ode.events.*; 
import org.apache.commons.math3.stat.correlation.*; 
import org.apache.commons.math3.analysis.solvers.*; 
import org.apache.commons.math3.complex.*; 
import org.apache.commons.math3.analysis.interpolation.*; 
import org.apache.commons.math3.stat.*; 
import org.apache.commons.math3.optimization.fitting.*; 
import org.apache.commons.math3.special.*; 
import org.apache.commons.math3.ode.sampling.*; 
import org.apache.commons.math3.stat.descriptive.moment.*; 
import org.apache.commons.math3.optimization.univariate.*; 
import org.apache.commons.math3.analysis.polynomials.*; 
import org.apache.commons.math3.ode.*; 
import org.apache.commons.math3.geometry.euclidean.threed.*; 
import org.apache.commons.math3.stat.regression.*; 

import java.applet.*; 
import java.awt.Dimension; 
import java.awt.Frame; 
import java.awt.event.MouseEvent; 
import java.awt.event.KeyEvent; 
import java.awt.event.FocusEvent; 
import java.awt.Image; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class BinaryFlow7 extends PApplet {


  
  
  
  
  
  
  
  
  



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
float zoomLv = 1.0f;
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

public void setup(){ 
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
	
	//\u904e\u53bb\u306e\u30d5\u30a9\u30eb\u30c0\u30fc\u9078\u629e
	String[] logs = loadStrings("log.txt");
	if(logs!=null && logs.length>0 && logs[0]!=""){
		home = logs[0];
		setData();
		selected =true;
	}
	//\u30d5\u30a9\u30eb\u30c0\u30fc\u30bb\u30ec\u30af\u30c8 
	if(home==null)
		selectDir("Select a folder to visualizing:");
	
	//\u30db\u30a4\u30fc\u30eb\u30a4\u30d9\u30f3\u30c8 
	addMouseWheelListener(new MouseWheelListener(){
	    public void mouseWheelMoved(MouseWheelEvent mwe){
	      mouseWheel(mwe.getWheelRotation());
	    }
	});
	//
	//\u30d5\u30a1\u30a4\u30eb\u3000\u30c9\u30e9\u30c3\u30b0&\u30c9\u30ed\u30c3\u30d7 
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
	          /* \u4f8b\u5916\u51e6\u7406 */  
	        } catch (IOException ex) {  
	          /* \u4f8b\u5916\u51e6\u7406 */  
	        }  
	      }  
	      if(fileNameList == null) return;  
	      // \u30c9\u30e9\u30c3\u30b0&\u30c9\u30ed\u30c3\u30d7\u3055\u308c\u305f\u30d5\u30a1\u30a4\u30eb\u306e\u4e00\u89a7\u306f\u4e00\u6642\u30ea\u30b9\u30c8\u306b\u683c\u7d0d\u3055\u308c\u308b  
	      // \u4ee5\u4e0b\u306e\u3088\u3046\u306b\u66f8\u304f\u3068\u3001\u30d5\u30a1\u30a4\u30eb\u306e\u30d5\u30eb\u30d1\u30b9\u3092\u8868\u793a  
	      //for(File f : fileNameList) println(f.getAbsolutePath()); 
	      if(fileNameList.size() == 1   &&  fileNameList.get(0).isDirectory()){
	      	folderSelected(fileNameList.get(0));
	      }else if(fileNameList.size() > 0){
	      	addClass(fileNameList);
	      }
	  }  
	}); 
} 

//\u30c7\u30fc\u30bf\u53d6\u5f97\u51e6\u7406**************************************************************************
//folder\u9078\u629e  
/*
* \u30d5\u30a1\u30a4\u30eb\u30b7\u30b9\u30c6\u30e0\u547c\u3073\u51fa\u3057\u3002
* @param txt \u30d5\u30a1\u30a4\u30eb\u30b7\u30b9\u30c6\u30e0\u306e\u30c8\u30c3\u30d7\u306e\u8868\u793a\u30c6\u30ad\u30b9\u30c8
*/
public void selectDir(String txt){
	selectFolder(txt, "folderSelected");
}
/*
* \u30d5\u30a9\u30eb\u30c0\u30fc\u9078\u629e\u5f8c\u306e\u51e6\u7406\u3002
*/
public boolean folderSelected(File selection){
	if (selection == null) {
    	//selectDir("Select a folder to visualizing:");
  	} else {
    	home = selection.getAbsolutePath();
    	println("User selected " + home);
    	PrintWriter pw = createWriter("./data/log.txt");
    	pw.println(home);
    	pw.flush(); 
    	pw.close();
    	
    	//\u30c7\u30fc\u30bf\u53d6\u5f97  
    	setData();
    	if(classes.size() > 0){
    		selected = true;
    	}
    	return true;
  	}
  	return false;
}
/*
* \u30af\u30e9\u30b9\u306e\u5404\u7a2e\u30c7\u30fc\u30bf\u306e\u30bb\u30c3\u30c8\u3002
*/
public void setData(){
	//GET class data
	setClass(home);
	setXY();
}
/*
* class\u30d5\u30a1\u30a4\u30eb\u306e\u8ffd\u52a0\u3002
* @param f \u8ffd\u52a0\u3059\u308b\u30d5\u30a1\u30a4\u30eb\u30ea\u30b9\u30c8
*/
public void addClass(List<File> f){
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
* class\u30d5\u30a1\u30a4\u30eb\u306e\u8aad\u307f\u8fbc\u307f\u3002
* @param p \u5bfe\u8c61\u30c7\u30a3\u30ec\u30af\u30c8\u30ea\u3002
*/
public void setClass(String p){
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
* \u30e1\u30bd\u30c3\u30c9\u306e\u30e1\u30e2\u30ea\u30b5\u30a4\u30ba\u6700\u5927\u91cf\u53d6\u5f97\u3002
*/
public void getMaxSize(){
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
* \u30e1\u30bd\u30c3\u30c9\u306e\u4f4d\u7f6e\u3092\u30bb\u30c3\u30c8
*/
public void setXY(){
	if(classes.size()>0){
		//get maxStack & maxLocal
		getMaxSize();
		//\u30eb\u30fc\u30d7\u30d0\u30a4\u30a2\u30b9\u30bb\u30c3\u30c8  
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
			//\u91cd\u5fc3    
			cl.setXY();
		}
	}
}
/*
* \u30e1\u30bd\u30c3\u30c9\u9593\u8ddd\u96e2\u306e\u8a08\u7b97
* @return double[][] \u30e1\u30bd\u30c3\u30c9\u9593\u306e\u8ddd\u96e2 
*/
public double[][] distance(){
	//\u53ef\u8996\u5316\u5bfe\u8c61\u3068\u306a\u308b\u30e1\u30bd\u30c3\u30c9\u30ea\u30b9\u30c8
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
//*\u30c7\u30fc\u30bf\u53d6\u5f97\u51e6\u7406\u3000\u3053\u3053\u307e\u3067**************************************************************************
/*
* \u63cf\u753b\u30e1\u30bd\u30c3\u30c9\u306e\u5927\u5143\u3002
*/
public void draw(){ 
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
* \u30de\u30a6\u30b9\u306e\u79fb\u52d5\u6642\u30a4\u30d9\u30f3\u30c8\u3002
*/
public void mouseMoved(){
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
* \u30de\u30a6\u30b9\u306e\u30c9\u30e9\u30c3\u30b0\u6642\u30a4\u30d9\u30f3\u30c8\u3002
*/
public void mouseDragged(){
	//translate
	if(selected && Drag && !Select && mouseButton!=RIGHT){
		VX += mouseX - lastX;
		VY += mouseY - lastY;
		lastX = mouseX;
		lastY = mouseY;
	}
}
/*
* \u30de\u30a6\u30b9\u30dc\u30bf\u30f3\u304c\u96e2\u3055\u308c\u305f\u6642\u306e\u30a4\u30d9\u30f3\u30c8\u3002
*/
public void mouseReleased(){
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
* \u30de\u30a6\u30b9\u30dc\u30bf\u30f3\u304c\u62bc\u3055\u308c\u305f\u6642\u306e\u30a4\u30d9\u30f3\u30c8\u3002
*/
public void mousePressed(){
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
	//\u64cd\u4f5c\u30d1\u30cd\u30eb\u64cd\u4f5c  
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
					if(LoopBias != i*0.5f){
						LoopBias = i*0.5f;
						//\u30eb\u30fc\u30d7\u30d0\u30a4\u30a2\u30b9\u30bb\u30c3\u30c8  
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
* \u30af\u30e9\u30b9\u306e\u30d5\u30a9\u30fc\u30ab\u30b9\uff08\u900f\u904e\u7387\uff09\u66f4\u65b0\u3002
*/
public void changeFocus(){
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
* \u753b\u9762\u306e\u5e73\u884c\u79fb\u52d5\u3002
*/
public void trans(int vx, int vy){
	translate((int)(vx + (1.0f-zoomLv)*width/2), (int)(vy + (1.0f-zoomLv)*(height-30)/2));
	scale(zoomLv);
	//translate(vx, vy);
}
/*
* \u753b\u9762\u306e\u5e73\u884c\u79fb\u52d5(\u623b\u3059\u969b\u306b\u4f7f\u7528)\u3002
*/
public void retrans(int vx, int vy){
	scale(1/zoomLv);
	translate(-vx - (1.0f-zoomLv)*width/2, -vy - (1.0f-zoomLv)*(height-30)/2);
	//translate(vx, vy);
}
//get mouseXY translated
/*
* \u30de\u30a6\u30b9\u306e\u62bc\u3055\u308c\u305f\u4f4d\u7f6e\u53d6\u5f97\u3000\uff08\u30de\u30c3\u30d4\u30f3\u30b0\u5ea7\u6a19\u4e0a\u306e\uff09\u3002
*/
public int getMouseX(){
	return mouseX-VX;
}
/*
* \u30de\u30a6\u30b9\u306e\u62bc\u3055\u308c\u305f\u4f4d\u7f6e\u53d6\u5f97\u3000\uff08\u30de\u30c3\u30d4\u30f3\u30b0\u5ea7\u6a19\u4e0a\u306e\uff09\u3002
*/
public int getMouseY(){
	return mouseY-VY;
}
/*
* \u30de\u30c3\u30d4\u30f3\u30b0\u5ea7\u6a19\u304b\u3089\u753b\u9762\u4e0a\u5ea7\u6a19\u3078\u306e\u5909\u63db\u3002
*/
public int getViewX(int x){
	return x+VX;
}
/*
* \u30de\u30c3\u30d4\u30f3\u30b0\u5ea7\u6a19\u304b\u3089\u753b\u9762\u4e0a\u5ea7\u6a19\u3078\u306e\u5909\u63db\u3002
*/
public int getViewY(int y){
	return y+VY;
}
//clear view  class&method
/*
* \u30af\u30e9\u30b9\u30fb\u30e1\u30bd\u30c3\u30c9\u306e\u9078\u629e\u306e\u521d\u671f\u5316
*/
public void clearView(){
	for(int i=0; i<classes.size(); i++){
		classes.get(i).setView(false);
		for(int j=0; j<classes.get(i).getCode().length; j++){
			classes.get(i).getCode()[j].setView(false);
		}
	}
}
//get view class & method befor select event
/*
* \u73fe\u5728\u8868\u793a\u3055\u308c\u3066\u3044\u308b\u30e1\u30bd\u30c3\u30c9\u30ea\u30b9\u30c8\u306e\u53d6\u5f97\u3002
*/
public List<List<Integer>> getViewList(){
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
* \u30af\u30e9\u30b9\u30fb\u30e1\u30bd\u30c3\u30c9\u306e\u8868\u793a\u53ef\u5426\u306e\u66f4\u65b0\u3002
* @param ls \u8868\u793a\u3059\u308b\u30af\u30e9\u30b9\u30ea\u30b9\u30c8
*/
public void setViewList(List<List<Integer>> ls){
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
* \u30ad\u30fc\u30a4\u30d9\u30f3\u30c8\u3002
*/ 
public void keyPressed(){
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
		zoomLv = 1.0f;
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
	//\u30eb\u30fc\u30d7\u30d0\u30a4\u30a2\u30b9\u3000\u5909\u66f4
	if(key=='z'){
		if(LoopBias==0.0f) LoopBias=0.0f;
		else if(LoopBias==0.5f) LoopBias=0.0f;
		else if(LoopBias==1.0f) LoopBias=0.5f;
		println("LOOP BIAS "+LoopBias);
		//\u30eb\u30fc\u30d7\u30d0\u30a4\u30a2\u30b9\u30bb\u30c3\u30c8  
		if(classes.size()>0)
			classes.get(0).getCode()[0].setBias(LoopBias);
		
		setXY();
	}
	if(key=='x'){
		if(LoopBias==0.0f) LoopBias=0.5f;
		else if(LoopBias==0.5f) LoopBias=1.0f;
		else if(LoopBias==1.0f) LoopBias=1.0f;
		println("LOOP BIAS "+LoopBias);
		//\u30eb\u30fc\u30d7\u30d0\u30a4\u30a2\u30b9\u30bb\u30c3\u30c8  
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
* \u30ba\u30fc\u30e0\u30a4\u30f3\u30fb\u30a2\u30a6\u30c8\u3002
*@param delta \u30db\u30a4\u30fc\u30eb\u306e\u56de\u8ee2\u91cf
*/
public void mouseWheel(int delta){
  zoomLv -= (float)(delta/20.0f);
  if(zoomLv < 0.1f)
    zoomLv = 0.1f;
  if(zoomLv > 10.0f)
    zoomLv = 10.0f;
}
	/*
	* \u64cd\u4f5c\u30d1\u30cd\u30eb\u63cf\u753b\u3002
	*/ 
	public void drawPanel(){
		colorMode(RGB, 255);
		int p_w = (int)((width-100)/5);
		int p_h = 100;
		textAlign(CENTER, CENTER);
		//\u5de6\u7aef
		int left = 0;
		//color
		int yes = 180;
		int no = 230; 
		
		//redraw
		textSize(30);
		stroke(100);
		if(height-100<mouseY && mouseX<left+p_w){
			fill(yes); rect(left,plotY2, p_w, p_h);
			fill(255); text("REDRAW", left+p_w/2, plotY2+p_h/2);
		}else{
			fill(no); rect(left,plotY2, p_w, p_h);
			fill(0); text("REDRAW", left+p_w/2, plotY2+p_h/2);
		}
		left+=p_w;
		//translate or select
		if(!Select){
			textSize(30);
			fill(yes); rect(left,plotY2, p_w, p_h/2);
			fill(255); text("TRANS", left+p_w/2, plotY2+p_h/4);
			textSize(20);
			fill(no); rect(left,plotY2+p_h/2, p_w, p_h/2);
			fill(0); text("SELECT", left+p_w/2, plotY2+p_h/4*3);
		}else{
			textSize(30);
			fill(no); rect(left,plotY2, p_w, p_h/2);
			fill(0); text("TRANS", left+p_w/2, plotY2+p_h/4);
			textSize(20);
			fill(yes); rect(left,plotY2+p_h/2, p_w, p_h/2);
			fill(255); text("SELECT", left+p_w/2, plotY2+p_h/4*3);
		}
		left+=p_w;
		//MODE\u5207\u308a\u66ff\u3048 
		fill(100);
		stroke(100);
		if(VIEW_MODE==CLASS){
			textSize(30);
			fill(yes); rect(left,plotY2, p_w, p_h/2);
			fill(255); text("CLASS", left+p_w/2, plotY2+p_h/4);
			textSize(20);
			fill(no); rect(left,plotY2+p_h/2, p_w, p_h/2);
			fill(0); text("METHOD", left+p_w/2, plotY2+p_h/4*3);
		}else{
			textSize(30);
			fill(no); rect(left,plotY2, p_w, p_h/2);
			fill(0); text("CLASS", left+p_w/2, plotY2+p_h/4);
			textSize(20);
			fill(yes); rect(left,plotY2+p_h/2, p_w, p_h/2);
			fill(255); text("METHOD", left+p_w/2, plotY2+p_h/4*3);
		}
		left+=p_w;
		//bias
		textSize(20);
		fill(255); rect(left,plotY2, p_w, p_h/2);
		fill(0); text("LOOP BIAS", left+p_w/2, plotY2+p_h/4);
		textSize(20);
		for(int i=0; i<3; i++){
			if(i==0 && LoopBias==0.0f  ||  i==1 && LoopBias==0.5f  ||  i==2 && LoopBias==1.0f){
				fill(yes); rect(left,plotY2+p_h/2, p_w/3, p_h/2);
				fill(255); text(""+(0.5f*i), left+p_w/6, plotY2+p_h/4*3);
			}else{
				fill(no); rect(left,plotY2+p_h/2, p_w/3, p_h/2);
				fill(0); text(""+(0.5f*i), left+p_w/6, plotY2+p_h/4*3);
			}
			left+=p_w/3;
		}
		//directory select
		textSize(20);
		if(height-100<mouseY && left<mouseX && mouseX<left+p_w){
			fill(yes); rect(left,plotY2, p_w, p_h);
			fill(255);
			text("CHANGE", left+p_w/2, plotY2+p_h/4+10);
			text("DIRECTORY", left+p_w/2, plotY2+p_h/4*3-10);
		}else{
			fill(no); rect(left,plotY2, p_w, p_h);
			fill(0);
			text("CHANGE", left+p_w/2, plotY2+p_h/4+10);
			text("DIRECTORY", left+p_w/2, plotY2+p_h/4*3-10);
		}
		
		colorMode(HSB, 100);
		textSize(15);
	}
	/*
	* \u30af\u30e9\u30b9\u306e\u3082\u3064\u30e1\u30bd\u30c3\u30c9\u306e\u63cf\u753b\u3002
	* @param cl \u53ef\u8996\u5316\u5bfe\u8c61\u306e\u30af\u30e9\u30b9
	*/
	public  boolean drawClass(ClassRead cl){
		trans(VX,VY);
		if(!cl.isView()) return false;
		if(VIEW_MODE == METHOD){
			drawLine(cl);
			drawPlot(cl);
			if(cl.getOnMouse() || cl.getOnName())
				drawMethodName(cl);
		}else if(VIEW_MODE == CLASS){
			drawClassLine(cl);
			drawClassPlot(cl);
			//draw class name
			if(cl.getOnMouse() || cl.getOnName())
				drawClassName(cl);
		}
		retrans(VX,VY);
		drawFileName(cl);
		return true;
	}
	/*
	* \u30e1\u30bd\u30c3\u30c9\u9593\u306e\u547c\u3073\u51fa\u3057\u95a2\u4fc2\u63cf\u753b\u3002
	* @param cl \u53ef\u8996\u5316\u5bfe\u8c61\u306e\u30af\u30e9\u30b9
	*/
	public  void drawLine(ClassRead cl){
		noFill();
		strokeWeight(2);
		stroke(cl.getHue(), 100, 100, cl.getAlpha());
		if(cl.getFocus() == 2)
			drawLine_mnemonic(cl);
		else
			drawLine_method(cl);
	}
	/*
	* \u30e1\u30bd\u30c3\u30c9\u30ce\u30fc\u30c9\u9593\u306e\u547c\u3073\u51fa\u3057\u95a2\u4fc2\u3002
	* @param cl \u53ef\u8996\u5316\u5bfe\u8c61\u306e\u30af\u30e9\u30b9
	*/
	 public void drawLine_method(ClassRead cl){
		for(int i=0; i<cl.getCode().length; i++){
			CodeAttribute code = cl.getCode()[i];
			if(code.isView()){
				//class\u304c\u547c\u3073\u51fa\u3055\u308c\u3066\u3044\u308b\u304b   
				for(int k=0; k<classes.size(); k++){
					ClassRead cl1 = classes.get(k);
					if(cl1.isView() && Arrays.asList(code.getInvokeClass()).indexOf(cl1.getName()) != -1) 
						for(int j=0; j<cl1.getCode().length; j++){
							if(cl1.getCode()[j].isView()){
								CodeAttribute code1 = cl1.getCode()[j];
								int num = Arrays.asList(code.getInvoke()).indexOf(code1.getName());
								if(num>-1){
									int x1 = code.getX();
									int y1 = code.getY();
									int x2 = code1.getX();
									int y2 = code1.getY();
									int vx = x2 - x1;
									int vy = y2 - y1;
									if(vx!=0 || vy!=0){
										line(x1, y1, x2, y2);
							    		double s = Math.acos(vx/Math.sqrt(vx*vx+vy*vy));
							    		if(vy < 0) s = Math.PI*2 - s; 
							    		beginShape();
							    		vertex((x1+x2)/2, (y1+y2)/2);
							    		vertex((int)((x1+x2)/2+20*Math.cos(s+Math.PI*5/6)), (int)((y1+y2)/2+20*Math.sin(s+Math.PI*5/6)));
							    		vertex((int)((x1+x2)/2+20*Math.cos(s-Math.PI*5/6)), (int)((y1+y2)/2+20*Math.sin(s-Math.PI*5/6)));
							    		endShape(CLOSE);
									}else{
										x2 = x1;
							    		y2 = y1 + r*2;
							    		noFill();
							    		arc(x2, y2, r*3, r*3, (int)(-1*Math.PI*1/3), (int)(Math.PI*4/3));
							    		int x3 = (int)(x2 + r*3/2 * Math.cos(Math.PI*4/3));
							    		int y3 = (int)(y2 + r*3/2 * Math.sin(Math.PI*4/3));
							    		beginShape();
							    		vertex(x3, y3);
							    		vertex(x3-20, y3);
							    		vertex(x3, y3+20);
							    		endShape(CLOSE);
		}}}}}}}
	}
	/*
	* \u547d\u4ee4\u30d0\u30a4\u30c8\u30b3\u30fc\u30c9\u30d7\u30ed\u30c3\u30c8\u304b\u3089\u306e\u547c\u3073\u51fa\u3057\u95a2\u4fc2\u3002
	* @param cl \u53ef\u8996\u5316\u5bfe\u8c61\u306e\u30af\u30e9\u30b9
	*/
	 public void drawLine_mnemonic(ClassRead cl){
		for(int i=0; i<cl.getCode().length; i++){
			if(cl.getCode()[i].isView()){
				CodeAttribute code = cl.getCode()[i];
				for(int j=0; j<code.getMnemonic().length; j++){
					//invoke
					if(code.getMnemonic()[j].isINVOKE()){
						//class\u306e\u547c\u3073\u51fa\u3057\u3000\u304c\u3042\u308b\u304b  
						for(int l=0; l<classes.size(); l++){
							ClassRead cl1 = classes.get(l);
							if(cl1.isView() && code.getMnemonic()[j].getInvokeClass().equals( cl1.getName() ))
								for(int k=0; k<cl1.getCode().length; k++){
									if(cl1.getCode()[k].isView()){
										CodeAttribute code1 = cl1.getCode()[k];
										if(code.getMnemonic()[j].getInvoke().equals( code1.getName() )){
											int x1 = code.getMnemonic()[j].getX();
						  					int y1 = code.getMnemonic()[j].getY();
						  					int x2 = code1.getX();
									    	int y2 = code1.getY();
									    	int vx = x2-x1;
									    	int vy = y2-y1;
									    	//invoke else method
									    	if(i != k  ||  l != cl.getID()){
									    		line(x1, y1, x2, y2);
									    		double s = Math.acos(vx/Math.sqrt(vx*vx+vy*vy));
									    		if(vy < 0) s = Math.PI*2 - s; 
									    		beginShape();
									    		vertex((x1+x2)/2, (y1+y2)/2);
									    		vertex((int)((x1+x2)/2+20*Math.cos(s+Math.PI*5/6)), (int)((y1+y2)/2+20*Math.sin(s+Math.PI*5/6)));
									    		vertex((int)((x1+x2)/2+20*Math.cos(s-Math.PI*5/6)), (int)((y1+y2)/2+20*Math.sin(s-Math.PI*5/6)));
									    		endShape(CLOSE);
									    	}
									    	//invoke myself method
									    	else{
									    		line(x1, y1, x1+30, y1);
									    		line(x1+30, y1, x1+30, y2);
									    		line(x1+30, y2, x2, y2);
									    		beginShape();
									    		vertex(x1+30, (y1+y2)/2);
									    		vertex(x1+40, (y1+y2)/2+10);
									    		vertex(x1+20, (y1+y2)/2+10);
									    		endShape(CLOSE);
									    	}
					}}}}}
		}}}
	}
	/*
	* \u30e1\u30bd\u30c3\u30c9\u30ce\u30fc\u30c9\u306e\u63cf\u753b\u3002
	* @param cl \u53ef\u8996\u5316\u5bfe\u8c61\u306e\u30af\u30e9\u30b9
	*/
	 public void drawPlot(ClassRead cl){
		boolean flag = false;
		if(cl.getFocus() == 2){
			drawPlotMnemo(cl);
		}else{
			for(int i=0; i<cl.getCode().length; i++){
				if(cl.getCode()[i].isView()){
					float dis = dist(cl.getCode()[i].getX(), cl.getCode()[i].getY(), getMouseX(), getMouseY());
					if(dis<r){
						drawPlotMnemo(cl, i);
						flag = true;
					}
					else{
						noStroke();
						fill(cl.getHue(), 100, 100, cl.getAlpha());
						arc(cl.getCode()[i].getX(), cl.getCode()[i].getY(), r*2, r*2, 0, PI*2);
						//stack size
						stroke(cl.getHue(), 100, 100, cl.getAlpha());
						strokeWeight(1);
						noFill();
						//memory circle
						int d = (int)Math.sqrt(map(cl.getCode()[i].getMaxMemory(),0,maxMemory, r*r*4, r*r*25));
						arc(cl.getCode()[i].getX(), cl.getCode()[i].getY(), d, d, 0, PI*2);
					}
				}
			}
			cl.setOnMouse(flag);
		}
	}
	/*
	* \u30e1\u30bd\u30c3\u30c9\u306e\u547d\u4ee4\u30d0\u30a4\u30c8\u30d7\u30ed\u30c3\u30c8\u63cf\u753b\u3002
	*\u3000@param cl \u53ef\u8996\u5316\u5bfe\u8c61\u306e\u30af\u30e9\u30b9
	*/
	 public void drawPlotMnemo(ClassRead cl){
		for(int i=0; i<cl.getCode().length; i++){
			drawPlotMnemo(cl, i);
		}
	}
	/*
	* \u30e1\u30bd\u30c3\u30c9\u306e\u547d\u4ee4\u30d0\u30a4\u30c8\u30d7\u30ed\u30c3\u30c8\u63cf\u753b (\u30e1\u30bd\u30c3\u30c9\u6307\u5b9a)\u3002
	*\u3000@param cl \u53ef\u8996\u5316\u5bfe\u8c61\u306e\u30af\u30e9\u30b9
	* @param index \u30e1\u30bd\u30c3\u30c9\u306e\u6dfb\u5b57
	*/
	 public void drawPlotMnemo(ClassRead cl, int index){
		strokeWeight(2);
		fill(cl.getHue(), 100, 100, cl.getAlpha());
		CodeAttribute code = cl.getCode()[index];
		List<VMCode> loops = new ArrayList<VMCode>();
		for(int j=0; j<code.getMnemonic().length; j++){
			VMCode vc = code.getMnemonic()[j];
			int x1 = vc.getX();
		   	int y1 = vc.getY();
		   	//draw plot
		   	noStroke();
		    arc(x1, y1, 4 , 4, 0 ,PI*2);
		    //draw loop line
		    if(vc.isLOOP()){
		    	stroke(cl.getHue(), 100, 100, cl.getAlpha());
		    	//this loop's deepth
		    	int deep = 1;
		    	for(int l=0; l<loops.size(); l++){
			    	//this mnemonic > loops[m]
			    	if(vc.getIndex()>loops.get(l).getIndex() && loops.get(l).getIndex()>vc.getJump()){
			    		deep++;
			    }}
			    loops.add(vc);
			    int jumpTo = code.getJumpIndex(vc);
			    int x2 = code.getMnemonic()[jumpTo].getX();
			    int y2 = code.getMnemonic()[jumpTo].getY();
			    line(x1, y1, code.getX()-10*deep, y1);
			    line(code.getX()-10*deep, y1, code.getX()-10*deep, y2);
			    line(code.getX()-10*deep, y2, x2, y2);
		    }
		}
	}
	/*
	* \u30e1\u30bd\u30c3\u30c9\u540d\u63cf\u753b\u3002
	*\u3000@param cl \u53ef\u8996\u5316\u5bfe\u8c61\u306e\u30af\u30e9\u30b9
	*/
	 public void drawMethodName(ClassRead cl){
		textSize(20);
		//only click
		//only focus plot
		for(int i=0; i<cl.getCode().length; i++){
			if(cl.getCode()[i].isView()){
				//float dis = dist(cl.getCode()[i].getX(), cl.getCode()[i].getY(), getMouseX(), getMouseY());
				//if(dis<r){
					fill(0,0,0,60);
					textAlign(CENTER, BASELINE);
					text(cl.getCode()[i].getName(), cl.getCode()[i].getX(), cl.getCode()[i].getY()-30);
				//}
			}
		}	
	}
	/*
	* \u30d5\u30a1\u30a4\u30eb\u540d\u63cf\u753b\u3002
	*\u3000@param cl \u53ef\u8996\u5316\u5bfe\u8c61\u306e\u30af\u30e9\u30b9
	*/
	 public void drawFileName(ClassRead cl){
		noStroke();
		textAlign(LEFT, CENTER);
		textSize(fileNameW/2);
		fill(100);
		rect(width-100, 50+fileNameW*cl.getID()-fileNameW/2, 100, fileNameW);
		
		if(cl.isView()){
			//only click
			if(clicks.indexOf(new Integer(cl.getID())) > -1){
				fill(cl.getHue(), 100, 100, 20);
				rect(width-100, 50+fileNameW*cl.getID()-fileNameW/2, 100, fileNameW);
			}
			//only onMouse plot
			if(cl.getOnMouse() || cl.getOnName()){
				stroke(cl.getHue(), 100, 100, 80);
				strokeWeight(3);
				line(width-100, 50+fileNameW*cl.getID()+fileNameW/2-3, width, 50+fileNameW*cl.getID()+fileNameW/2-3);
				noStroke();
			}
		}
		if(cl.isView())
			fill(cl.getHue(), 100, 100, 100);
		else
			fill(cl.getHue(), 100, 100, 20);
		rect(width-100, 50+fileNameW*cl.getID(), 10, 10);
		text(cl.getName(), width-90, 50+fileNameW*cl.getID());
	}
	/*
	* \u30af\u30e9\u30b9\u30ce\u30fc\u30c9\u540d\u63cf\u753b\u3002
	*/
	public void drawClassName(ClassRead cl){
		textSize(20);
		fill(0,0,0,60);
		textAlign(CENTER, BASELINE);
		text(cl.getName(), cl.getX(), cl.getY());
	}
	/*
	* \u30af\u30e9\u30b9\u30ce\u30fc\u30c9\u63cf\u753b\u3002
	*/
	public void drawClassPlot(ClassRead cl){
		boolean flag = false;
		float dis = dist(cl.getX(), cl.getY(), getMouseX(), getMouseY());
		if(dis<r)
			flag = true;
		//draw plot
		noStroke();
		fill(cl.getHue(), 100, 100, cl.getAlpha());
		arc(cl.getX(), cl.getY(), r*4, r*4, 0, PI*2);
		
		cl.setOnMouse(flag);
	}
	/*
	* \u30af\u30e9\u30b9\u30ce\u30fc\u30c9\u9593\u306e\u547c\u3073\u51fa\u3057\u95a2\u4fc2\u3002
	*/
	public void drawClassLine(ClassRead cl){
		strokeWeight(3);
		noFill();
		stroke(cl.getHue(), 100, 100, cl.getAlpha());
		for(int i=0; i<cl.getCode().length; i++){
			CodeAttribute code = cl.getCode()[i];
			for(int j=0; j<classes.size(); j++){
				ClassRead cl2 = classes.get(j);
				if(cl2.isView()){
					int num = Arrays.asList(code.getInvokeClass()).indexOf(cl2.getName());
					if(num>-1){
						int x1 = cl.getX();
						int y1 = cl.getY();
						int x2 = cl2.getX();
						int y2 = cl2.getY();
						int vx = x2 - x1;
						int vy = y2 - y1;
						if(vx!=0 || vy!=0){
							line(x1, y1, x2, y2);
							double s = Math.acos(vx/Math.sqrt(vx*vx+vy*vy));
						  	if(vy < 0) s = Math.PI*2 - s; 
						   	beginShape();
						    vertex((x1+x2)/2, (y1+y2)/2);
						    vertex((int)((x1+x2)/2+20*Math.cos(s+Math.PI*5/6)), (int)((y1+y2)/2+20*Math.sin(s+Math.PI*5/6)));
						    vertex((int)((x1+x2)/2+20*Math.cos(s-Math.PI*5/6)), (int)((y1+y2)/2+20*Math.sin(s-Math.PI*5/6)));
						    endShape(CLOSE);
						}
					}
				}
			}
		}
	}
		
		
		
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "BinaryFlow7" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
