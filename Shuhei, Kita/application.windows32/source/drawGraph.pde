	/*
	* 操作パネル描画。
	*/ 
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
			if(i==0 && LoopBias==0.0  ||  i==1 && LoopBias==0.5  ||  i==2 && LoopBias==1.0){
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
	/*
	* クラスのもつメソッドの描画。
	* @param cl 可視化対象のクラス
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
	* メソッド間の呼び出し関係描画。
	* @param cl 可視化対象のクラス
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
	* メソッドノード間の呼び出し関係。
	* @param cl 可視化対象のクラス
	*/
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
	/*
	* 命令バイトコードプロットからの呼び出し関係。
	* @param cl 可視化対象のクラス
	*/
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
	/*
	* メソッドノードの描画。
	* @param cl 可視化対象のクラス
	*/
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
	/*
	* メソッドの命令バイトプロット描画。
	*　@param cl 可視化対象のクラス
	*/
	 void drawPlotMnemo(ClassRead cl){
		for(int i=0; i<cl.getCode().length; i++){
			drawPlotMnemo(cl, i);
		}
	}
	/*
	* メソッドの命令バイトプロット描画 (メソッド指定)。
	*　@param cl 可視化対象のクラス
	* @param index メソッドの添字
	*/
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
	/*
	* メソッド名描画。
	*　@param cl 可視化対象のクラス
	*/
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
	/*
	* ファイル名描画。
	*　@param cl 可視化対象のクラス
	*/
	 void drawFileName(ClassRead cl){
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
	* クラスノード名描画。
	*/
	void drawClassName(ClassRead cl){
		textSize(20);
		fill(0,0,0,60);
		textAlign(CENTER, BASELINE);
		text(cl.getName(), cl.getX(), cl.getY());
	}
	/*
	* クラスノード描画。
	*/
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
	/*
	* クラスノード間の呼び出し関係。
	*/
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
		
		
		
