import processing.core.*; 
import processing.xml.*; 

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

public class binaryFlow5 extends PApplet {




//draw area
int plotX1;// = 100;
int plotY1;// = 50;
int plotX2;// = width-50;
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

//event
List<Integer> CLICKS = new ArrayList<Integer>();
int FOCUS = -1;
boolean ZOOM = false;
double zoomLv = 1.0f;

boolean TRANS = false;
boolean DRAG = false;
int VX = 0;
int VY = 0;
int lastX = 0;
int lastY = 0;

int fileNameW = 40;

int MODE = 0;
int pathIndex = 0;
String[] path = new String[]{"C:/Users/tukushimbo/Documents/Processing/binaryFlow4/data"};

public void setup(){
	size(700,600);
	smooth();
	plotX1 = 100;
	plotY1 = 50;
	plotX2 = width - 50;
	plotY2 = height-100;
	plotCX = 300;
	plotCY = 300;
	
	//GET VMCode data
	String[] vmList = loadStrings("./VMCode.csv");
	VMCode.setVMCodes(vmList);
	
	//GET class data
	setClass(path[0]);
	//get maxStack & maxLocal
	for(int i=0; i<classes.size(); i++){
		for(int j=0; j<classes.get(i).getCode().length; j++){
			int ds = classes.get(i).getCode()[j].getMaxStack();
			int dl = classes.get(i).getCode()[j].getMaxLocal();
			if(ds > maxStack)
				maxStack = ds;
			if(dl > maxLocal)
				maxLocal = dl;
		}
	}
	println("OK: "+classes.size());
	//get distance & XY
	setXY();
} 
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
		//set color
		for(int i=0; i<classes.size(); i++){
			classes.get(i).setHue( (int)(100*i/classes.size()) );
		}
	}catch(Exception e){
		println("ERROR: "+e);
	}
}
public void draw(){ 
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
	
		
	trans(-VX, -VY);
	
	//select
	if(DRAG && SELECT){
		fill(70, 100,100,30);
		rect(getViewX(selectX1), getViewY(selectY1), mouseX-getViewX(selectX1), mouseY-getViewY(selectY1));
	}
}


//draw class
public void drawClass(ClassRead cl){
	//focus
	//if(cl.getViewID() == FOCUS)
		//cl.setFocus(2);
	if(cl.isView()){
		trans(VX, VY);
		//draw line
		drawLine(cl);
		//draw plot
		drawPlot(cl);
		
		//draw method name
		if(cl.getOnMouse() || cl.getOnName())
			drawMethodName(cl);
		
		trans(-VX, -VY);
	}
	//draw file name
	drawFileName(cl);
	
}

//draw file name
public void drawFileName(ClassRead cl){
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
	text(cl.getName().split("\\.")[0], width-90, 50+fileNameW*cl.getID());
}

//draw plot
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
					int d = 0;
					if(MODE == 0){
						//d = (int)Math.sqrt(map(cl.getCode()[i].getMaxLocal(),0,maxLocal, r*r*4, r*r*25));
						d = (int)map(cl.getCode()[i].getMaxLocal(),0,maxLocal, r*2, r*5);
					}else if(MODE == 1){
						//d = (int)Math.sqrt(map(cl.getCode()[i].getMaxStack(),0,maxStack, r*r*4, r*r*25));
						d = (int)map(cl.getCode()[i].getMaxStack(),0,maxStack, r*2, r*5);
					}
					arc(cl.getCode()[i].getX(), cl.getCode()[i].getY(), d, d, 0, PI*2);
				}
			}
		}
		cl.setOnMouse(flag);
	}
}
//draw mnemonic plot
public void drawPlotMnemo(ClassRead cl){
	strokeWeight(2);
	fill(cl.getHue(), 100, 100, cl.getAlpha());
	for(int i=0; i<cl.getCode().length; i++){
		if(cl.getCode()[i].isView()){
			CodeAttribute code = cl.getCode()[i];
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
}
//draw mnemonic plot  (select index)
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
//draw line
public void drawLine(ClassRead cl){
	noFill();
	strokeWeight(2);
	stroke(cl.getHue(), 100, 100, cl.getAlpha());
	if(cl.getFocus() == 2)
		drawLine_mnemonic(cl);
	else
		drawLine_method(cl);
}
//draw line of method plot
public void drawLine_method(ClassRead cl){
	for(int i=0; i<cl.getCode().length; i++){
		CodeAttribute code = cl.getCode()[i];
		if(code.isView()){
			for(int j=0; j<cl.getCode().length; j++){
				if(cl.getCode()[j].isView()){
					CodeAttribute code1 = cl.getCode()[j];
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
	}}}}}}
}
//draw line of mnemonic plot
public void drawLine_mnemonic(ClassRead cl){
	for(int i=0; i<cl.getCode().length; i++){
		if(cl.getCode()[i].isView()){
			CodeAttribute code = cl.getCode()[i];
			for(int j=0; j<code.getMnemonic().length; j++){
				//invoke
				if(code.getMnemonic()[j].isINVOKE()){
					for(int k=0; k<cl.getCode().length; k++){
						if(cl.getCode()[k].isView()){
							CodeAttribute code1 = cl.getCode()[k];
							if(code.getMnemonic()[j].getInvoke().equals( code1.getName() )){
								int x1 = code.getMnemonic()[j].getX();
			  					int y1 = code.getMnemonic()[j].getY();
			  					int x2 = code1.getX();
						    	int y2 = code1.getY();
						    	int vx = x2-x1;
						    	int vy = y2-y1;
						    	
						    	if(i != k){
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
				}}}}
	}}}
}
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
				text(cl.getCode()[i].getName(), cl.getCode()[i].getX(), cl.getCode()[i].getY());
			//}
		}
	}	
}
////draw---END------------------------

//mouse EVENT------------------------
public void mouseMoved(){
	//file name focus
	for(int i=0; i<classes.size(); i++){
		classes.get(i).setOnName(false);
	}
	if(width-100<mouseX){
		int num = (int)Math.floor((mouseY-30)/40);
		if(num < classes.size()){
			classes.get(num).setOnName(true);
		}
	}
}

public void mouseDragged(){
	//translate
	if(DRAG && !SELECT && mouseButton!=RIGHT){
		VX += mouseX - lastX;
		VY += mouseY - lastY;
		lastX = mouseX;
		lastY = mouseY;
	}
}
public void mouseReleased(){
	if(DRAG && !SELECT && mouseButton!=RIGHT){
		DRAG = false;
		lastX = 0;
		lastY = 0;
	}else if(DRAG && SELECT && mouseButton!=RIGHT){
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
		}else{
			setViewList(ls);
		}
	}
	
}

public void mousePressed(){
	//translate
	if(width-100>mouseX && !SELECT){
		DRAG = true;
		lastX = mouseX;
		lastY = mouseY;
	}
	//select
	if(width-100>mouseX && SELECT){
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
//claases focus change
public void changeFocus(){
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
public void trans(int vx, int vy){
	translate(vx, vy);
}

//get mouseXY translated
public int getMouseX(){
	return mouseX-VX;
}
public int getMouseY(){
	return mouseY-VY;
}
public int getViewX(int x){
	return x;
}
public int getViewY(int y){
	return y;
}
//clear view  class&method
public void clearView(){
	for(int i=0; i<classes.size(); i++){
		classes.get(i).setView(false);
		for(int j=0; j<classes.get(i).getCode().length; j++){
			classes.get(i).getCode()[j].setView(false);
		}
	}
}
//get view class & method befor select event
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
public void keyPressed(){
	if(key=='2'){
		trans(-VX,-VY);
		VX = 0;
		VY = 0;
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
	if(key=='5'){
		MODE++;
		MODE %= 2;
		if(MODE == 0)
			println("Local size draw");
		else if(MODE == 1)
			println("Stack size draw");
	}
	if(key=='0'){
		pathIndex = (pathIndex+1)%path.length;
		setClass(path[pathIndex]);
		setXY();
	}
}

/////key EVENT---END--------------



//get  code's distance & XY
public void setXY(){
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
		double[][] datas = new double[len][len];
		for(int i=0; i<len; i++){
			datas[i][i] = 0;
			for(int j=i+1; j<len; j++){
				CodeAttribute c1 = viewCodes.get(i);
				CodeAttribute c2 = viewCodes.get(j);
				double dis = (double)c1.edit(c2);
				//if(dis != 0)
					//dis = Math.log(dis);
				//println(dis);
				datas[i][j] = dis;
				datas[j][i] = dis;
				//debug
				println("("+c1.getID()+"."+c1.getName() +", "+c2.getID()+"."+c2.getName()+") = "+dis);
			}
		}
		//get xyMatrix
		int w = (width-100)/2 - 50;
		int h = height/2 -50;
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
		}
	}
}
//get xyMatrix
public double[][] getXY(double[][] array){
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
	//固有�?  ?�を飛�?�?	int k = 0;
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
	//固有�?
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
//ls2: ソート�?//ソート後�?indexを返す
public int[] getIndex(double[] ls1, double[] ls2){
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
    static public void main(String args[]) {
        PApplet.main(new String[] { "--bgcolor=#ECE9D8", "binaryFlow5" });
    }
}
