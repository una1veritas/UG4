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

//Mode
int CLASS = 1;
int METHOD = 2;
int VIEW_MODE = CLASS;


//draw area
int plotX1;// = 100;
int plotY1;// = 50;
int plotX2;// = width-150;
int plotY2;// = height-100;

int plotCX;
int plotCY;

//matrix 
double maxX;
double maxY;
double[][] XY;
//distance matrix
double[][] datas;

int r = 8;

//select program
boolean SELECT = false;
int selectX1 = -1;
int selectY1 = -1;
int selectX2 = -1;
int selectY2 = -1;

//List
List<ClassRead> classes;// = new ArrayList<ClassRead>();
int IDCount = 0;
int viewIDCount = 0;
List<Integer> viewIDList = new ArrayList<Integer>();

int maxStack = 0;
int maxLocal = 0;
int maxMemory = 0;

//event
List<Integer> CLICKS = new ArrayList<Integer>();
int FOCUS = -1;
boolean ZOOM = false;
double zoomLv = 1.0;

boolean TRANS = false;
boolean DRAG = false;
int VX = 0;
int VY = 0;
int lastX = 0;
int lastY = 0;

int fileNameW = 30;

int pathIndex = 0;
String[] path = new String[]{"C:/Users/tukushimbo/workspace/sampleProgram/bin", "C:/Users/tukushimbo/workspace/visialSample/bin"};

String home;	//選択フォルダー 
boolean selected = false;

//ループ構造の編集距離反映のバイアス 
double LOOP_BIAS = 1.0;

//拡大縮小 
float zoom = 1.0;
//名前表示フラグ 
boolean ALL_NAME = false;

void setup(){
	size(700,700);
	smooth();
	plotX1 = 100;
	plotY1 = 50;
	plotX2 = width - 50;
	plotY2 = height-100;
	plotCX = 300;
	plotCY = 300;
	
	//GET VMCode data
	String[] vmList = loadStrings("VMCode.csv");
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
	
	//ホイール
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

//folder選択  
void selectDir(String txt){
	selectFolder(txt, "folderSelected");
}
//フォルダー選択後 
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
void setData(){
	//GET class data
	setClass(home);
	//ループバイアスセット  
	if(classes.size()>0){
		//get maxStack & maxLocal
		getMaxSize();
		classes.get(0).getCode()[0].setBias(LOOP_BIAS);
		println("OK: "+classes.size());
		//get distance & XY
		setXY();
		//distance output  at csv
		disOut();
		//debug
		//codeOut();
	}
}
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
					classes.add(cl);
					IDCount++;
					println("add: "+f.get(i).getName());
				}
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
	//ループバイアスセット  
	if(classes.size()>0){
		//get maxStack & maxLocal
		getMaxSize();
		classes.get(0).getCode()[0].setBias(LOOP_BIAS);
		println("OK: "+classes.size());
		//get distance & XY
		setXY();
		//distance output  at csv
		disOut();
		//debug
		//codeOut();
	}
}
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
void getMaxSize(){
	maxStack = 0;
	maxLocal = 0;
	maxMemory = 0;
	for(int i=0; i<classes.size(); i++){
		for(int j=0; j<classes.get(i).getCode().length; j++){
			int ds = classes.get(i).getCode()[j].getMaxStack();
			int dl = classes.get(i).getCode()[j].getMaxLocal();
			if(ds+dl > maxMemory)
				maxMemory = ds+dl;
			if(ds > maxStack)
				maxStack = ds;
			if(dl > maxLocal)
				maxLocal = dl;
		}
	}
}
void draw(){ 
	//フォルダー選択後　実行  
	if(selected){
		int focusPlot = -1;
		int focusX = -1, focusY = -1;
		
		//back
		colorMode(RGB, 255);
		background(255);
		
		colorMode(HSB, 100);
		textSize(15);
		
		//draw class
		for(int i=0; i<classes.size(); i++){
			drawClass(classes.get(i));
		}
		
		//select
		if(DRAG && SELECT){
			fill(70, 100,100,30);
			rect(getViewX(selectX1), getViewY(selectY1), mouseX-getViewX(selectX1), mouseY-getViewY(selectY1));
		}
	}
	//操作パネル　描画  
	drawPanel();
	
	retrans(VX, VY);
}

//操作パネル描画 
void drawPanel(){
	colorMode(RGB, 255);
	int p_w = (int)((width-100)/5);
	int p_h = 100;
	textAlign(CENTER, CENTER);
	//左端
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
	if(!SELECT){
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
	//MODE切り替え 
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
		if(i==0 && LOOP_BIAS==0.0  ||  i==1 && LOOP_BIAS==0.5  ||  i==2 && LOOP_BIAS==1.0){
			fill(yes); rect(left,plotY2+p_h/2, p_w/3, p_h/2);
			fill(255); text(""+(0.5*i), left+p_w/6, plotY2+p_h/4*3);
		}else{
			fill(no); rect(left,plotY2+p_h/2, p_w/3, p_h/2);
			fill(0); text(""+(0.5*i), left+p_w/6, plotY2+p_h/4*3);
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

//draw class
void drawClass(ClassRead cl){
	//focus
	//if(cl.getViewID() == FOCUS)
		//cl.setFocus(2);
	if(cl.isView()){
		trans(VX, VY);
		if(VIEW_MODE == METHOD){
			//draw line
			drawLine(cl);
			//draw plot
			drawPlot(cl);
			
			//draw method name
			if(cl.getOnMouse() || cl.getOnName() || ALL_NAME)
				drawMethodName(cl);
		}else if(VIEW_MODE == CLASS){
			drawClassLine(cl);
			drawClassPlot(cl);
			//draw class name
			if(cl.getOnMouse() || cl.getOnName())
				drawClassName(cl);
		}
		retrans(VX, VY);
	}
	//draw file name
	drawFileName(cl);
	
}

//draw file name
void drawFileName(ClassRead cl){
	noStroke();
	textAlign(LEFT, CENTER);
	textSize(fileNameW/2);
	fill(100);
	rect(width-100, 50+fileNameW*cl.getID()-fileNameW/2, 100, fileNameW);
	
	if(cl.isView()){
		//only click
		if(CLICKS.indexOf(new Integer(cl.getID())) > -1){
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
//draw class name
void drawClassName(ClassRead cl){
	textSize(20);
	fill(0,0,0,60);
	textAlign(CENTER, BASELINE);
	text(cl.getName(), cl.getX(), cl.getY());
}
//draw class plot
void drawClassPlot(ClassRead cl){
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
//draw class line
void drawClassLine(ClassRead cl){
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
//draw plot
void drawPlot(ClassRead cl){
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
//draw mnemonic plot
void drawPlotMnemo(ClassRead cl){
	boolean flag = false;
	strokeWeight(2);
	fill(cl.getHue(), 100, 100, cl.getAlpha());
	for(int i=0; i<cl.getCode().length; i++){
		if(cl.getCode()[i].isView()){
			CodeAttribute code = cl.getCode()[i];
			//マウス
			float dis = dist(code.getX(), code.getY(), getMouseX(), getMouseY());
			if(dis<r)
				flag = true;
			//呼び出し関係  
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
	}
	cl.setOnMouse(flag);
}
//draw mnemonic plot  (select index)
void drawPlotMnemo(ClassRead cl, int index){
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
//draw line
void drawLine(ClassRead cl){
	noFill();
	strokeWeight(2);
	stroke(cl.getHue(), 100, 100, cl.getAlpha());
	if(cl.getFocus() == 2)
		drawLine_mnemonic(cl);
	else
		drawLine_method(cl);
}
//draw line of method plot
void drawLine_method(ClassRead cl){
	for(int i=0; i<cl.getCode().length; i++){
		CodeAttribute code = cl.getCode()[i];
		if(code.isView()){
			//classが呼び出されているか   
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
//draw line of mnemonic plot
void drawLine_mnemonic(ClassRead cl){
	for(int i=0; i<cl.getCode().length; i++){
		if(cl.getCode()[i].isView()){
			CodeAttribute code = cl.getCode()[i];
			for(int j=0; j<code.getMnemonic().length; j++){
				//invoke
				if(code.getMnemonic()[j].isINVOKE()){
					//classの呼び出し　があるか  
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
void drawMethodName(ClassRead cl){
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
////draw---END------------------------

//mouse EVENT------------------------
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

void mouseDragged(){
	//translate
	if(selected && DRAG && !SELECT && mouseButton!=RIGHT){
		VX += mouseX - lastX;
		VY += mouseY - lastY;
		lastX = mouseX;
		lastY = mouseY;
	}
}
void mouseReleased(){
	if(selected && DRAG && !SELECT && mouseButton!=RIGHT){
		DRAG = false;
		lastX = 0;
		lastY = 0;
	}else if(selected && DRAG && SELECT && mouseButton!=RIGHT){
		DRAG = false;
		CLICKS = new ArrayList<Integer>();
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
					CLICKS.add(new Integer(i));
				}else{
					cl.setView(false);
				}
			}
		}
		if(change){
			changeFocus();
			SELECT = false;
		}else{
			setViewList(ls);
		}
	}
	
}

void mousePressed(){
	//
	//translate
	if(selected){
	if(width-100>mouseX  && plotY2>mouseY && !SELECT  &&  mouseButton!=RIGHT){
		DRAG = true;
		lastX = mouseX;
		lastY = mouseY;
	}
	//select
	if(width-100>mouseX && plotY2>mouseY && SELECT  &&  mouseButton!=RIGHT){
		DRAG = true;
		selectX1 = getMouseX();
		selectY1 = getMouseY();
	}
	//select cancel
	if(mouseButton == RIGHT){
		CLICKS = new ArrayList<Integer>();
		for(int i=0; i<classes.size(); i++)
			classes.get(i).setFocus(1);
	}else{
		//file name click
		if(width-100<mouseX){
			int num = (int)Math.floor((mouseY-(50-fileNameW/2))/fileNameW);
			if(num < classes.size() && num>=0){
				//add || remove
				if(classes.get(num).isView()){
					int d = CLICKS.indexOf(new Integer(num)); 
					if(d == -1){
						CLICKS.add(new Integer(num));
					}else{
						CLICKS.remove(d);
					}
				}
				//classes focs change
				changeFocus();
			}
			println(Arrays.toString(CLICKS.toArray(new Integer[0])));
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
			if(CLICKS.size() == 0){
				for(int i=0; i<classes.size(); i++){
					classes.get(i).setView(true);
					for(int j=0; j<classes.get(i).getCode().length; j++){
						classes.get(i).getCode()[j].setView(true);
					}
				}
			}else{
				for(int i=0; i<classes.size(); i++)
					classes.get(i).setView(false);
				for(int i=0; i<CLICKS.size(); i++){
					classes.get(CLICKS.get(i)).setView(true);
				}
			}
			CLICKS = new ArrayList<Integer>();
			setXY();
		}
		left+=p_w;
		//trans & select
		if(left<mouseX && mouseX<left+p_w){
			if(mouseY<plotY2+p_h/2) SELECT=false;
			else SELECT=true;
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
					if(LOOP_BIAS != i*0.5){
						LOOP_BIAS = i*0.5;
						//ループバイアスセット  
						if(classes.size()>0)
							classes.get(0).getCode()[0].setBias(LOOP_BIAS);
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
void changeFocus(){
	for(int i=0; i<classes.size(); i++){
		if(CLICKS.size() > 0){
			if(CLICKS.indexOf(new Integer(i)) == -1)
				classes.get(i).setFocus(0);
			else 
				classes.get(i).setFocus(2);
		}else{
			classes.get(i).setFocus(1);
		}
	}
} 
//transrate
void trans(int vx, int vy){
	translate(vx + (1.0-zoom)*width/2, vy + (1.0-zoom)*(height-30)/2);
	scale(zoom);
	//translate(vx, vy);
}
void retrans(int vx, int vy){
	scale(1/zoom);
	translate(-vx - (1.0-zoom)*width/2, -vy - (1.0-zoom)*(height-30)/2);
	//translate(vx, vy);
}
//get mouseXY translated
int getMouseX(){
	return mouseX-VX;
}
int getMouseY(){
	return mouseY-VY;
}
int getViewX(int x){
	return x+VX;
}
int getViewY(int y){
	return y+VY;
}
//clear view  class&method
void clearView(){
	for(int i=0; i<classes.size(); i++){
		classes.get(i).setView(false);
		for(int j=0; j<classes.get(i).getCode().length; j++){
			classes.get(i).getCode()[j].setView(false);
		}
	}
}
//get view class & method befor select event
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
		zoom = 1.0;
	}
	if(key=='3'){
		if(CLICKS.size() == 0){
			for(int i=0; i<classes.size(); i++){
				classes.get(i).setView(true);
				for(int j=0; j<classes.get(i).getCode().length; j++){
					classes.get(i).getCode()[j].setView(true);
				}
			}
		}else{
			for(int i=0; i<classes.size(); i++)
				classes.get(i).setView(false);
			for(int i=0; i<CLICKS.size(); i++){
				classes.get(CLICKS.get(i)).setView(true);
			}
		}
		CLICKS = new ArrayList<Integer>();
		setXY();
	}
	if(key=='4'){
		if(!SELECT){
			SELECT = true;
			selectX1=selectX2=selectY1=selectY2 = -1;
		}else{
			SELECT = false;
			selectX1=selectX2=selectY1=selectY2 = -1;
		}
	}
	//対象バイナリ切り替え 
	if(key=='9'){
		pathIndex = (pathIndex+1) % path.length;
		//GET class data
		setClass(path[pathIndex]);
		//get maxStack & maxLocal
		getMaxSize();
		//get distance & XY
		CLICKS = new ArrayList<Integer>();
		setXY();
	}
	//初期化 
	if(key=='0'){
		pathIndex = (pathIndex+1)%path.length;
		setClass(path[pathIndex]);
		setXY();
	}
	//ループバイアス　変更
	if(key=='z'){
		if(LOOP_BIAS==0.0) LOOP_BIAS=0.0;
		else if(LOOP_BIAS==0.5) LOOP_BIAS=0.0;
		else if(LOOP_BIAS==1.0) LOOP_BIAS=0.5;
		println("LOOP BIAS "+LOOP_BIAS);
		//ループバイアスセット  
		if(classes.size()>0)
			classes.get(0).getCode()[0].setBias(LOOP_BIAS);
		
		setXY();
	}
	if(key=='x'){
		if(LOOP_BIAS==0.0) LOOP_BIAS=0.5;
		else if(LOOP_BIAS==0.5) LOOP_BIAS=1.0;
		else if(LOOP_BIAS==1.0) LOOP_BIAS=1.0;
		println("LOOP BIAS "+LOOP_BIAS);
		//ループバイアスセット  
		if(classes.size()>0)
			classes.get(0).getCode()[0].setBias(LOOP_BIAS);
		setXY();
	}
	if(key=='s'){
		save("./data/outpic.jpg");
	}
	
	//名前表示
	if(key=='n'){
		if(ALL_NAME) ALL_NAME=false;
		else ALL_NAME=true;
	}
}

/////key EVENT---END--------------



//get  code's distance & XY
void setXY(){
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
		CLICKS = new ArrayList<Integer>();
		for(int i=0; i<classes.size(); i++)
			classes.get(i).setFocus(1);
		
		//get  method count 
		int len = viewCodes.size();
		//get distance
		datas = new double[len][len];
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
		//get xyMatrix
		int w = (width-100)/2 - 50;
		int h = (height-100)/2 -50;
		double[][] XY = getXY(datas);
		int index = 0;
		for(int i=0; i<classes.size(); i++){
			ClassRead cl = classes.get(i);
			if(cl.isView()){
				for(int j=0; j<cl.getCode().length; j++){
					CodeAttribute code = cl.getCode()[j];
					if(code.isView()){
						code.setXY( (int)(XY[index][0]/maxX*w)+plotCX, (int)(XY[index][1]/maxY*h)+plotCX );
						index++;
					}
				}
			}
			//重心    
			cl.setXY();
		}
	}
}
//get xyMatrix
double[][] getXY(double[][] array){
	RealMatrix matrix = MatrixUtils.createRealMatrix(array);
	//get value
  	EigenDecomposition ed = new EigenDecomposition(matrix,1);
  	double[] ls = ed.getRealEigenvalues();
  	double[] ls2 = ed.getRealEigenvalues();
	int max = ls.length;
	
	Arrays.sort(ls);
	//println(ls);
	int[] index = getIndex(ls2, ls);
	double[][] array1 = new double[max][max];
	//固有値  ０を飛ばす
	int k = 0;
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
  	double[][] array3 = matrix3.getData();
  	maxX = -1;
  	maxY = -1;
  	
  	for(int i=0; i<array3.length; i++){
  		/*print
    	for(int j=0; j<array3[i].length; j++){
      		print(""+array3[i][j]+" ");
    	}
    	*/
    	if(Math.abs(array3[i][0]) > maxX) maxX = Math.abs(array3[i][0]);
    	if(Math.abs(array3[i][1]) > maxY) maxY = Math.abs(array3[i][1]); 
    	//println(" ");
  	}
  	
  	return array3;
}

//ls1: ソート前
//ls2: ソート後
//ソート後のindexを返す
int[] getIndex(double[] ls1, double[] ls2){
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

//distance output
void disOut(){
	List<String> ls = new ArrayList<String>();
	PrintWriter p = createWriter("distance.csv");
	p.print(" ");
	for(int i=0; i<classes.size(); i++){
		for(int j=0; j<classes.get(i).getCode().length; j++){
			p.print(","+classes.get(i).getID()+":"+classes.get(i).getCode()[j].getName());
			ls.add(classes.get(i).getID()+":"+classes.get(i).getCode()[j].getName());
		}
	}
	p.println("");
	for(int i=0; i<datas.length; i++){
		p.print(ls.get(i));
		for(int j=0; j<datas.length; j++){
			p.print(","+ datas[i][j]);
		}
		p.println("");
	}
	p.flush();
	p.close();
}

//method code[] output
void codeOut(){
	for(int i=0; i<classes.size(); i++){
		for(int j=0; j<classes.get(i).getCode().length; j++){
			CodeAttribute code = classes.get(i).getCode()[j];
			if(!code.getName().equals("<init>")){
				PrintWriter p = createWriter("./data/"+classes.get(i).getName()+"/"+code.getName()+".txt");
				Byte[] codes = code.getCode();
				for(int k=0; k<codes.length; k++){
					p.print(""+String.format("%02X ", codes[k])+" ");
				}
				p.flush();
				p.close();
			}
		}
	}
}

//ホイール
void mouseWheel(int delta){
  zoom -= (float)(delta/20.0);
  if(zoom < 0.1)
    zoom = 0.1;
  if(zoom > 10.0)
    zoom = 10.0;
}