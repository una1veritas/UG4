//TinyBASICの機能は再現完了。外付けのRAMにプログラムを書き込むことは可能に。1/9
//配列,可変長の変数名宣言が可能になった（名前は英字のみ）
//float型の使用、実行時の出力形式の変更実装　1/23
//次点目標、string型実装　とりあえず文字列だけ変数に格納できるようにする

#ifndef ARDUINO
#include "stdafx.h"
#include <conio.h>
#endif 

// ASCII Characters
#define CR	'\r'
#define NL	'\n'
#define TAB	'\t'
#define BELL	'\b'
#define DEL	'\177'
#define SPACE   ' '
#define CTRLC	0x03
#define CTRLH	0x08
#define CTRLS	0x13
#define CTRLX	0x18


#define MAX_PROGRAMSIZE 32*1024
#define MAX_DIMSIZE 1024
#define MAX_NAMESIZE 32

typedef short unsigned LINENUM;


/*RAM使用準備*/

#include <SPI.h>
#include <SPISRAM.h>
const int SRAM_CS = 10;
SPISRAM Ram(SRAM_CS, SPISRAM::BUS_MBits); // CS pin


/***********************************************************/
// Keyword table and constants - the last character has 0x80 added to it
static unsigned char keywords[] = {
	'L','I','S','T'+0x80,
	'L','O','A','D'+0x80,
	'N','E','W'+0x80,
	'R','U','N'+0x80,
	'S','A','V','E'+0x80,
	'N','E','X','T'+0x80,
	'L','E','T'+0x80,
	'I','F'+0x80,
	'G','O','T','O'+0x80,
	'G','O','S','U','B'+0x80,
	'R','E','T','U','R','N'+0x80,
	'R','E','M'+0x80,
	'F','O','R'+0x80,
	'I','N','P','U','T'+0x80,
	'P','R','I','N','T'+0x80,
	'P','O','K','E'+0x80,
	'S','T','O','P'+0x80,
	'B','Y','E'+0x80,
        'D','I','M'+0x80,
	0
};

#define KW_LIST		0
#define KW_LOAD		1
#define KW_NEW		2
#define KW_RUN		3
#define KW_SAVE		4
#define KW_NEXT		5
#define KW_LET		6
#define KW_IF		7
#define KW_GOTO		8
#define KW_GOSUB	9
#define KW_RETURN	10
#define KW_REM		11
#define KW_FOR		12
#define KW_INPUT	13
#define KW_PRINT	14
#define KW_POKE		15
#define KW_STOP		16
#define KW_BYE		17
#define KW_DIM          18
#define KW_DEFAULT	19

struct stack_for_frame {
	char frame_type;
	unsigned int for_var;
	short int terminal;
	short int step;
	unsigned int current_line;
	unsigned int txtpos;
};

struct stack_gosub_frame {
	char frame_type;
	unsigned int current_line;
	unsigned int txtpos;
};

union Data_t{
  short int typeint;
  float  typefloat;
  unsigned char Byte[4];  

};


static unsigned char func_tab[] = {
	'P','E','E','K'+0x80,
	'A','B','S'+0x80,
	0
};

#define FUNC_PEEK     0
#define FUNC_ABS      1
#define FUNC_UNKNOWN 2

static unsigned char to_tab[] = {
	'T','O'+0x80,
	0
};

static unsigned char step_tab[] = {
	'S','T','E','P'+0x80,
	0
};

static unsigned char relop_tab[] = {
	'>','='+0x80,
	'<','>'+0x80,
	'>'+0x80,
	'='+0x80,
	'<','='+0x80,
	'<'+0x80,
	0
};

#define RELOP_GE		0
#define RELOP_NE		1
#define RELOP_GT		2
#define RELOP_EQ		3
#define RELOP_LE		4
#define RELOP_LT		5
#define RELOP_UNKNOWN	6

#define VAR_SIZE sizeof(short int) // Size of variables in bytes

static unsigned char memory[256];
static unsigned int  txtpos,list_line;
static unsigned char expression_error;
static unsigned char *tempsp;
static unsigned char *stack_limit;
static unsigned int  program_start;
static unsigned int  program_end;
static unsigned char *stack; // Software stack for things that should go on the CPU stack

static unsigned int  variables_table;
static unsigned int  variables_end;

static unsigned int  current_line;

static unsigned char vartype;    //現在の型が何かを示す
#define INTEGER 2
#define FLOAT   4
#define STRING  1


static unsigned int  sp;
static unsigned char *framesp;

#define STACK_GOSUB_FLAG 'G'
#define STACK_FOR_FLAG 'F'
static unsigned char table_index;
static LINENUM linenum;

static const unsigned char okmsg[]		= "OK";
static const unsigned char badlinemsg[]		= "Invalid line number";
static const unsigned char invalidexprmsg[] = "Invalid expression";
static const unsigned char syntaxmsg[] = "Syntax Error";
static const unsigned char badinputmsg[] = "\nBad number";
static const unsigned char badnamemsg[] = "Variable name error";
static const unsigned char nomemmsg[]	= "Not enough memory!";
static const unsigned char initmsg[]	= "TinyBasic in C V0.03.";
static const unsigned char memorymsg[]	= " bytes free.";
static const unsigned char breakmsg[]	= "break!";
static const unsigned char stackstuffedmsg[] = "Stack is stuffed!\n";
static const unsigned char unimplimentedmsg[]	= "Unimplemented";
static const unsigned char backspacemsg[]		= "\b \b";

static int inchar(void);
static void outchar(unsigned char c);
static void line_terminator(void);
static union Data_t expression(void);
static unsigned char breakcheck(void);

/***************************************************************************/
static void ignore_blanks(void)
{
	while((unsigned char)Ram.read(txtpos) == SPACE || (unsigned char)Ram.read(txtpos) == TAB)
		txtpos++;
}

/***************************************************************************/
static void scantable(unsigned char *table)
{
	int i = 0;
	ignore_blanks();
	table_index = 0;
	while(1)
	{
		// Run out of table entries?
		if(table[0] == 0)
                    return;

		// Do we match this character?
		if((unsigned char)Ram.read(txtpos+i) == table[0])
		{
			i++;
			table++;
		}
		else
		{
			// do we match the last character of keywork (with 0x80 added)? If so, return
			if((unsigned char)Ram.read(txtpos+i)+0x80 == table[0])
			{
				txtpos += i+1;  // Advance the pointer to following the keyword
				ignore_blanks();
				return;
			}

			// Forward to the end of this keyword
			while((table[0] & 0x80) == 0)
				table++;

			// Now move on to the first character of the next word, and reset the position index
			table++;
			table_index++;
			i = 0;
		}
	}
}


/***************************************************************************/
static void pushb(unsigned char b)
{
	sp--;
        Ram.write(sp,b);
}

/***************************************************************************/
static unsigned char popb()
{
	unsigned char b;

        b = (unsigned char)Ram.read(sp);
	sp++;
	return b;
}

/***************************************************************************/
static void printnum(int num)
{
	int digits = 0;

	if(num < 0)
	{
		num = -num;
		outchar('-');
	}

	do {
		pushb(num%10+'0');
		num = num/10;
		digits++;
	}
	while (num > 0);

	while(digits > 0)
	{
		outchar(popb());
		digits--;
	}
}
/*****txtposのところから、行番号を計算して返す(文字⇒行番号へ)************************/
static unsigned short testnum(void)
{
	unsigned short num = 0;
	ignore_blanks();
	unsigned char c;
        c = (unsigned char)Ram.read(txtpos);
	while(c >= '0' &&  c <= '9' )
	{
		// Trap overflows
		if(num >= 0xFFFF/10)
		{
			num = 0xFFFF;
			break;
		}

		num = num *10 + c - '0';
		txtpos++;
                c = (unsigned char)Ram.read(txtpos);
	}
	return	num;
}

/***************文が終わっているかをチェックする********************/
unsigned char check_statement_end(void)
{
	ignore_blanks();
        unsigned char c = (unsigned char)Ram.read(txtpos);
	return (c == NL) || (c == ':');
}

/*******プログラムの使用上のメッセージ表示(RAM外でおｋ)*******************/
static void printmsgNoNL(const unsigned char *msg)
{
	while(*msg)
	{
		outchar(*msg);
		msg++;
	}
}

/****"  "で囲まれた文を文字列として出力***************************/
static unsigned char print_quoted_string(void)
{
	int i=0;
	unsigned char delim = (unsigned char)Ram.read(txtpos);
        
	if(delim != '"' && delim != '\'')    //引用符じゃなかったら終わり
		return 0;
	txtpos++;
        
        // Check we have a closing delimiter
	while((unsigned char)Ram.read(txtpos+i) != delim)
	{
		if((unsigned char)Ram.read(txtpos+i) == NL)  //引用符の閉じがこなかった
			return 0;
		i++;
	}      
	// Print the characters
	while((unsigned char)Ram.read(txtpos) != delim)
	{
		outchar((unsigned char)Ram.read(txtpos));
		txtpos++;
	}
	txtpos++; // Skip over the last delimiter
	ignore_blanks();

	return 1;
}

/*******エラーメッセージ等のメッセージを表示*************/
static void printmsg(const unsigned char *msg)
{
	printmsgNoNL(msg);
    line_terminator();
}

/**********入力をプログラムエンドの２個後ろから書き込む********************************/
unsigned char getln(char prompt)
{
	outchar(prompt);
	txtpos = program_end+sizeof(LINENUM);

	while(1)
	{
		char c = inchar();
		switch(c)
		{
			case CR:
			case NL:
                line_terminator();
				// Terminate all strings with a NL
				Ram.write(txtpos, NL);
                                //txtpos[0] = NL;
				return 1;
			case CTRLC:
				return 0;
			case CTRLH:
				if(txtpos == program_end)
					break;
				txtpos--;
				printmsgNoNL(backspacemsg);
				break;
			default:
				// We need to leave at least one space to allow us to shuffle the line into order
				if(txtpos == sp-2)
					outchar(BELL);
				else
				{
                                        Ram.write(txtpos,c);
					txtpos++;
					outchar(c);
				}
		}
	}
}



/*pointerから2バイトで整数を読み出す*/
static short int get2byte(int pointer){
  union Data_t getdata;
  getdata.Byte[0]=(unsigned char)Ram.read(pointer);
  getdata.Byte[1]=(unsigned char)Ram.read(pointer+1);
  
  return getdata.typeint;
} 
/*pointerから2バイトを書き込む*/
static void set2byte(int pointer, short int value){
  union Data_t setdata;
  setdata.typeint = value;
  
  Ram.write(pointer, setdata.Byte[0]);  //下位ビット
  Ram.write(pointer+1,setdata.Byte[1]); //上位ビット
}


/*pointerから4バイト読み出す*/
static float get4byte(int pointer){
  union Data_t getdata;
  int i;
  getdata.Byte[0]=(unsigned char)Ram.read(pointer);
  getdata.Byte[1]=(unsigned char)Ram.read(pointer+1);
  getdata.Byte[2]=(unsigned char)Ram.read(pointer+2);
  getdata.Byte[3]=(unsigned char)Ram.read(pointer+3);
  return getdata.typefloat;
}

/*pointerから4バイトfloatを書き込む*/
static void set4byte(int pointer,float value){
  union Data_t setdata;
  setdata.typefloat=value;
  int i;
  for(i=0;i<4;i++)
    Ram.write(pointer+i,setdata.Byte[i]);
}



/***************************************************************************/
static unsigned int findline(void)
{
	unsigned int line = program_start;
        short int findnum=0;
        short int high=0;
        short int low=0;
	while(1)
	{
		if(line == program_end)
			return line;
                findnum = get2byte(line);
		if( findnum >= linenum)
			return line;

		// Add the line length onto the current address, to get to the next line;
		line += (unsigned char)Ram.read(line+2);  //lineから3番目には行長が入っている
	}
}

/***************************************************************************/
static void toUppercaseBuffer(void)
{
	unsigned int c = program_end+sizeof(LINENUM);
	unsigned int quote = 0;
        unsigned char tmp;

        tmp = (unsigned char)Ram.read(c);
	while(tmp != NL)
	{
		// Are we in a quoted string?
		if(tmp == quote)
			quote = 0;
		else if(tmp == '"' || tmp == '\'')
			quote = c;
		else if(quote == 0 && tmp >= 'a' && tmp <= 'z'){
			tmp = tmp + 'A' - 'a';
                        Ram.write(c,tmp);  //大文字にしたtmpをcに書き込む
                }
		c++;
                tmp = (unsigned char)Ram.read(c);  
	}
}


/***************************************************************************/
void printline()
{
	LINENUM line_num;

        line_num = get2byte(list_line);
        list_line += sizeof(LINENUM) + sizeof(char);  //行番号、行長データ読み飛ばし

	// Output the line         
	printnum(line_num);
	outchar(' ');

        unsigned char c;
        c = (unsigned char)Ram.read((int)list_line);  //list_lineの先頭文字がc
        
        while(c != NL){
                outchar(c);
		list_line++;
                c = (unsigned char)Ram.read((int)list_line);
        }
        
        list_line++;
        line_terminator();
}

/************直値、変数、関数、括弧の対応を計算***************************************/
static union Data_t expr4(void)
{
	union Data_t a;
        unsigned char c;  //txtposの文字をそんままもらってくる    
        short int integer=0;  //整数部
        float dec=0;          //小数部
        unsigned char intflag=1;  //整数部を書き込む必要があるかないか 
//単純な数式の処理
        unsigned char temp;
        
        
       c = (unsigned char)Ram.read(txtpos);
       if(c=='0'){  //最上位の桁が0の時
         txtpos++;
         goto decimal;  //小数部の計算へ
       }
       
       if(c >= '1'&& c <= '9'){  //とにかく数字がきたよ
         while(c >= '0' && c <= '9'){
           integer = integer*10 + c - '0';
           txtpos++;
           c = (unsigned char)Ram.read(txtpos);
         }
         goto decimal;  //小数部の計算へ
       }

//文字であるかどうか確認
	if(c >= 'A' && c <= 'Z')    //変数or関数である時
	{
		// Is it a function with a single parameter
		int temp = txtpos;
                scantable(func_tab);  //関数なのかチェック
		if(table_index == FUNC_UNKNOWN){
		        txtpos = temp;
                	goto variables;
                }

		unsigned char f = table_index;
                c = (unsigned char)Ram.read(txtpos);  //関数読み終えた後の括弧
                
		if(c != '(')
			goto expr4_error;
		txtpos++;
                if(vartype==FLOAT)
                    temp=FLOAT;
		a = expression();
                if(temp==FLOAT)
                    vartype=FLOAT;
                    
                c = (unsigned char)Ram.read(txtpos);  //閉じ括弧があるか
		if(c != ')')
				goto expr4_error;
                txtpos++;
		switch(f)
		{
			case FUNC_PEEK:
                                if(vartype==INTEGER)
				  a.typeint = (short int)Ram.read(a.typeint);  //Ram上のa番目の１バイトを返す
                                else if(vartype==FLOAT)
                                  a.typefloat = (short int)Ram.read((int)a.typefloat);
                                goto success;
			case FUNC_ABS:
                                if(vartype==INTEGER)
         				if(a.typeint < 0)
					    a.typeint = -a.typeint;
                                if(vartype==FLOAT)
                                        if(a.typefloat < 0)
                                           a.typefloat = -a.typefloat;
				goto success;
		}

	}
        
	if(c == '(')  //いきなり開き括弧
	{
		txtpos++;
		a = expression();
                c = (unsigned char)Ram.read(txtpos);  //閉じ括弧チェック
		if(c != ')')
			goto expr4_error;

		txtpos++;
		goto success;
	}

decimal:{
              c=(unsigned char)Ram.read(txtpos);
              if(c=='.'){  //小数点以下の存在
                  float i = 0.1;  //桁
                  txtpos++;
                  c=(unsigned char)Ram.read(txtpos);
                  while(c>='0' && c<= '9'){
                    dec = dec + (c-'0')*i;
                    i *= 0.1;
                    txtpos++;
                    c=(unsigned char)Ram.read(txtpos); 
                  }  
               }
               
               if(vartype==INTEGER)
                   a.typeint=integer;
               else if(vartype==FLOAT)
                   a.typefloat = dec + integer;
               goto success;               
          }

variables:{
                unsigned char datatype;  //変数の型情報
                unsigned char temp=INTEGER;
                if(vartype==FLOAT)
                    temp=FLOAT;
                int var = findvar(&datatype);  //配列計算したらexpressionの都合上、vartypeがintegerになってしまうので注意
                                              //糞コード・・・                
                if(temp==FLOAT)
                    vartype=FLOAT;
		// Is it a variable reference (single alpha)
		if((var != variables_end) && (var != 0) )  //変数があった時
		{
                      switch (datatype){
                          case INTEGER:  //変数がint型
                               if(vartype==INTEGER){
                                 a.typeint = get2byte(var);                                 
                               }else if(vartype==FLOAT){
                                 a.typefloat = get2byte(var);                               
                               }
                               break;                                                 
                          case FLOAT:
                               if(vartype==INTEGER){
                                 a.typeint = get4byte(var);                                 
                               }else if(vartype==FLOAT){
                                 a.typefloat = get4byte(var);                               
                               }
                               break;
                          default:
                               goto expr4_error;                     
                      }                   
                      while(((unsigned char)Ram.read(txtpos) <= 'Z') && ((unsigned char)Ram.read(txtpos) >= 'A'))
          		  txtpos++;  //変数名の長さ分加算しねーとあかん
		      goto success;
		}
                goto expr4_error;
            }
            
expr4_error:
	expression_error = 1;
success:
	ignore_blanks();
	return a;
}

/********掛け算、割り算****************************/
static union Data_t expr3(void)
{
	union Data_t a,b;
        unsigned char c;      

	a = expr4();
	while(1)
	{       ignore_blanks();
                c = (unsigned char)Ram.read(txtpos);
		if(c == '*')
		{
			txtpos++;
			b = expr4();
                        if(vartype==INTEGER)
			    a.typeint *= b.typeint;
                        else if(vartype==FLOAT)
                            a.typefloat *= b.typefloat;
		}
		else if(c == '/')
		{
			txtpos++;
			b = expr4();
                        if(vartype==INTEGER){
			    if(b.typeint != 0)
			        	a.typeint /= b.typeint;
			    else
			        	expression_error = 1;
                        }else if(vartype==FLOAT){
			    if(b.typefloat != 0)
			        	a.typefloat /= b.typefloat;
			    else
			        	expression_error = 1;
                        }
		}
		else
			return a;  //乗算除算の処理をやり終えた。
	}
}

/************加算、減算の処理**************************/
static union Data_t expr2(void)
{
	union Data_t a,b;
        unsigned char c;
        c = (unsigned char)Ram.read(txtpos);
        
    //前の項を評価 
	if(c == '-' || c == '+')
		a.typefloat = 0;
	else
		a = expr3();

        c = (unsigned char)Ram.read(txtpos);
	while(1)
	{
		if(c == '-')
		{
			txtpos++;
			b = expr3();
                        if(vartype==INTEGER)
      			      a.typeint -= b.typeint;
                        else if(vartype==FLOAT)
                              a.typefloat -= b.typefloat;
		}
		else if(c == '+')
		{
			txtpos++;
			b = expr3();
                       if(vartype==INTEGER)
      			      a.typeint += b.typeint;
                        else if(vartype==FLOAT)
                              a.typefloat += b.typefloat;
		}
		else
			return a;
               c = (unsigned char)Ram.read(txtpos);   //次の文字へ
	}
}

/***************************************************************************/
static union Data_t expression(void)
{
	union Data_t a,b;
        union Data_t Bool;
        Bool.typeint=1;
	a = expr2();
	// Check if we have an error
	if(expression_error)	return a;

	scantable(relop_tab);
	if(table_index == RELOP_UNKNOWN)
		return a;

	switch(table_index)
	{
	case RELOP_GE:
		b = expr2();
		if(vartype==INTEGER){
                    if(a.typeint>=b.typeint)
                         Bool.typeint=1;
		}else if(vartype==FLOAT){
                    if(a.typefloat>=b.typefloat)
                         Bool.typeint=1;
                }
                break;
	case RELOP_NE:
		b = expr2();
		if(vartype==INTEGER){
                    if(a.typeint != b.typeint)
                         Bool.typeint=1;
		}else if(vartype==FLOAT){
                    if(a.typefloat != b.typefloat)
                         Bool.typeint=1;
                }
		break;
	case RELOP_GT:
		b = expr2();
		if(vartype==INTEGER){
                    if(a.typeint > b.typeint)
                         Bool.typeint=1;
		}else if(vartype == FLOAT){
                    if(a.typefloat > b.typefloat)
                         Bool.typeint=1;
                }
		break;
	case RELOP_EQ:
		b = expr2();
		if(vartype==INTEGER){
                    if(a.typeint == b.typeint)
                         Bool.typeint=1;
		}else if(vartype == FLOAT){
                    if(a.typefloat == b.typefloat)
                         Bool.typeint=1;
                }
		break;
		b = expr2();
		if(vartype==INTEGER){
                    if(a.typeint <= b.typeint)
                         Bool.typeint=1;
		}else if(vartype == FLOAT){
                    if(a.typefloat <= b.typefloat)
                         Bool.typeint=1;
                }
	case RELOP_LT:
		b = expr2();
		if(vartype==INTEGER){
                    if(a.typeint >= b.typeint)
                         Bool.typeint=1;
		}else if(vartype == FLOAT){
                    if(a.typefloat >= b.typefloat)
                         Bool.typeint=1;
                }
	}
	return Bool;
}

/*STRING型を表示する*/
static void printstring(unsigned int top){
  unsigned int endoftxt = top + get2byte(top);
  unsigned int pos=top+3;
  while((unsigned char)Ram.read(pos) != NL){
          pos++;
  }
  pos++;  
  while(pos != endoftxt){
    outchar((unsigned char)Ram.read(pos));
    pos++;
  }
  line_terminator();
}


/*"がくるまでtopから始まる変数に文字列を書き込む*/
static void setstring(unsigned int top){
  unsigned int from;
  unsigned int dest;

  unsigned int datasize=3;
  unsigned char c = (unsigned char)Ram.read(top+datasize);
  outchar(c);
  while(c != NL){
    datasize++;
    c = (unsigned char)Ram.read(top+datasize);
    outchar(c);    
  }
  datasize++;
  /*datasizeは変数名+NL+3状態。*/
  from=top+get2byte(top);
  dest=top+datasize;  //NLの次（次の変数の頭）

//とりあえず一回削除  
  while(from !=variables_end){
    c = (unsigned char)Ram.read(from);
    Ram.write(dest,c);
    dest++;
    from++;
  }
  variables_end = dest;
  
  unsigned int stringlen=0;  //入力文字列の長さ
  unsigned char temp=txtpos;
  c=(unsigned char)Ram.read(temp);
  
  while(c != '"'){  //引用符がくるまで
    stringlen++;
    temp++;
    c=(unsigned char)Ram.read(temp);    
  }
  //tempが引用符まできたらカウント終了
  from = variables_end;
  dest = variables_end+stringlen;

  while(from > (top+datasize)){
    c=(unsigned char)Ram.read(from);    
    Ram.write(dest,c);
    from--;  //fromはtop+datasizeで終わる
    dest--;
  }
  
  int i=0;
  c=(unsigned char)Ram.read(txtpos);

  while(c != '"'){  //引用符がくるまで
      Ram.write(top+datasize+i,c);
      i++;
      txtpos++;
      c=(unsigned char)Ram.read(txtpos);      
  }
  txtpos++;  //引用符の次に移動
  
  set2byte(top,datasize+stringlen);
  variables_end += stringlen;    
}

/*txtposからアルファベットを読み取り、変数名として解釈し変数を作成*/
/*txtposはアルファベットの終端の次、値は変数が格納されている場所が返される(pos)
 追記1/23
 　変数名の前に型情報を付加。typeには作成したデータの型情報が書き込まれる
*/
static unsigned int makevar(unsigned char *type){
  unsigned char c;
  unsigned char size=0;
  unsigned int pos;
  
  while(MAX_NAMESIZE>size){
      c = (unsigned char)Ram.read(txtpos);
      if(c >= 'A' && c <= 'Z'){
        Ram.write(variables_end + size + 3, c);
        size++;
        txtpos++;      
      }else{    //アルファベット以外が来た
        Ram.write(variables_end + size + 3, NL);
        size++;  //NULLまで含めた文字列長
        pos = variables_end + size + 3;  //変数のところ(NLの次)
        if(c=='$'){
            Ram.write(variables_end+2,STRING);  //型情報はSTRING型          
            size +=3;
            txtpos++;         
            *type = STRING;
            set2byte(variables_end,(short int)size);
            Serial.println(size);
            return variables_end;  //変数名の先頭
        }else if(c=='%'){
            Ram.write(variables_end+2,FLOAT);  //型情報はFLOAT型
            size +=7;
            txtpos++;
            set4byte(pos,0);
            *type = FLOAT;    
        }else{
           Ram.write(variables_end+2,INTEGER);  //型情報はINT型
           size +=5;
           set2byte(pos,2);
           *type = INTEGER;        
        }
        set2byte(variables_end,(short int)size);
        return pos;
      }
  }
  printmsg(badnamemsg);
  return variables_end;
}



/*unsigned int findvar()
  txtposから文字列をよみとり、対応する値が入っている場所を返す。
  配列の呼び出しにも対応
  みつけた⇒txtposは変数名(もしくは配列名())の後ろ、値が入っているところを返す
　ない　　⇒txtposは変数名の先頭(変化なし) 、variables_endを返す
  ()内が無効な式　⇒txtposは変数名の中、0を返す
  追記1/23
  　変数を見つける+型情報をtypeに書き込む
*/
static unsigned int findvar(unsigned char *type){
  
  unsigned int top=variables_table;  //topは参照している変数データの先頭
  unsigned int pos = top+3;          //posは参照している変数データの変数名
  unsigned int temp = txtpos;
  union Data_t index;
  index.typeint=0;
  unsigned char c;
  unsigned char name;
  unsigned char typetemp;
  
  while(1){
      if(top == variables_end)
            return top;
      while(1){
        c = (unsigned char)Ram.read(temp);  //txtposの文字列
        name = (unsigned char)Ram.read(pos);
        if((c < 'A') | (c > 'Z')){  //照合完了
           txtpos = temp;
           pos++;  //データ部へ移動
           typetemp = (unsigned char)Ram.read(top+2);  //型情報ゲット         
           c = (unsigned char)Ram.read(txtpos);           
           if(c=='('){  //配列だったら・・・・
             txtpos++;
             expression_error = 0;
             vartype=INTEGER;  //⇒vertypeをintに変更してるので注意！
             index = expression();  //txtposから添字の処理
             *type=typetemp;  //ここでようやく型を決定
             if(expression_error)
                 goto error;
             c = (unsigned char)Ram.read(txtpos);  //次の文字
             pos += *type * index.typeint; //対象データ部へ移動(型情報×添字)
             if((pos < (top + get2byte(top))) && c == ')'){  //閉じ括弧でindex範囲内にあるか
                 txtpos++;     //txtposは')'の後ろ
                 return pos;  //配列の添字の場所の値を返す
             }else
                 goto error;
           }
           *type=typetemp;
           if(*type==STRING)
               return top;  //文字列型は変数データの先頭を返す
           return pos;           
        }if(c >= 'A' && c <= 'Z' && c==name){  //１文字一致
          temp++;
          pos ++;
        }else{  //１文字も一致なかったら
          top += get2byte(top);  //topに現在変数のサイズを加算⇒次の変数の先頭へ
          pos = top +3;
          temp = txtpos;
          break;
        }
      }
    }   
error:
        return 0;
}


/*txtposから変数名を読み込んで配列を宣言（宣言のみ）*/
/*txtposは()まで読んだ先、返り値は配列宣言できた(1)か否(0)か*/
char makedim(){
  unsigned char c;
  short int size = 0;
  short int index=0;
  unsigned char datatype;
  while(MAX_NAMESIZE > size){
    c = (unsigned char)Ram.read(txtpos);
    if(c >= 'A' && c <= 'Z'){  //アルファベットが来る限り
       Ram.write(variables_end + size + 3, c);
       size++;
       txtpos++;
    }else if(c=='('){  //アルファベット以外の文字、(がきた
      Ram.write(variables_end + size + 3, NL);  //NLを書き込む
      size++;    //NL含む文字数
      txtpos++;  //括弧の中へ
      index = testnum(); //添字
      c = (unsigned char)Ram.read(txtpos); 
      if(c != ')')
        break;
      txtpos++;
      c = (unsigned char)Ram.read(txtpos);  //次の型情報
      if(c=='%'){
           txtpos++;
           Ram.write(variables_end+2,FLOAT);  //型情報はFLOAT型
           datatype = FLOAT;
       }else{
          Ram.write(variables_end+2,INTEGER);  //型情報はINT型
          datatype = INTEGER;        
       }
       size = size + (index+1) * datatype+3;  
       set2byte(variables_end,size);
       variables_end += size;       
       return 1;
   }else
       break; 
  }
  return 0;
}


/***************************************************************************/
void loop()
{
    Serial.flush();
    SPI.begin();    //SPIを使用開始
    Ram.begin();  //myRAM(cs(10)ピンのMYRAM使用開始)
  
	unsigned int start;
	unsigned int newEnd;
	unsigned short int linelen;
        unsigned char c;
        short int findnum;    

	variables_table = MAX_PROGRAMSIZE; //変数領域
        variables_end = variables_table;
        unsigned int var;  //変数を指すポインタ
        
        union Data_t data;	

        vartype=INTEGER;  //デフォルトはshortint型

	program_start = 1;      //プログラム領域先頭
	program_end = program_start;
	sp =  variables_table-1;  // Needed for printnum
        framesp = memory+sizeof(memory);  //for gosubframe用ポインタ


	printmsg(initmsg);
	Serial.print(sp-program_end);
	printmsg(memorymsg);

warmstart:
	// this signifies that it is running in 'direct' mode.
	current_line = 0;
	sp = MAX_PROGRAMSIZE-1;
        variables_end = variables_table;        
	printmsg(okmsg);

prompt:
//        Serial.println(variables_end);
        //getlnしたら、program_endの2バイト先から入力を書き込む
	while(!getln('>'))
		line_terminator();
	toUppercaseBuffer();

	txtpos = program_end+sizeof(unsigned short);  //入力したところに戻る。
        
        c = (unsigned char)Ram.read(txtpos);
	// Find the end of the freshly entered line
	while(c != NL){      //入力文末まで移動
		txtpos++;
                c = (unsigned char)Ram.read(txtpos);
        }
        
        
        //現在txtposは入力した文の文末にきている
	// Move it to the end of program_memory

        //プログラム領域の末尾(sp)に後ろから入力文をコピー
	{
		unsigned int dest;
		dest = sp-1;
		while(1)
		{       c = (unsigned char)Ram.read(txtpos);
                        Ram.write(dest,c);
			if(txtpos == program_end+sizeof(unsigned short))
				break;
			dest--;
			txtpos--;
		}
		txtpos = dest;  //txtposはプログラム領域の末尾(sp)からコピーされた入力文の先頭
	}

	// Now see if we have a line number
	linenum = testnum();  //行番号を獲得

	ignore_blanks();
	if(linenum == 0)
		goto direct;

	if(linenum == 0xFFFF)
		goto badline;

        linelen=0;
        while((unsigned char)Ram.read(txtpos+linelen) != NL)  //ステートメントの長さを測る
              linelen++;
        linelen++;  //文末NULL文字分
        linelen += sizeof(unsigned short)+sizeof(char);  //行番号、行長分

	// Now we have the number, add the line header.
	txtpos -= 3;  //入力文から３バイト前に移動（行番号、行長、入力文と続くようにする)
        
        set2byte(txtpos,linenum);
//        Ram.write(txtpos,(char)linenum);  //行番号下位ビット
//        Ram.write(txtpos+1,(char)(linenum >>8));  //行番号上位ビット      
        Ram.write(txtpos+2,linelen);

	start = findline();
//        findnum = (int)Ram.read((int)start) + (int)Ram.read((int)(start+1))*256;
        findnum=get2byte(start);
              //みつけた場所の行番号
        
	//同じ行番号があったため、とりあえず削除
	if(start != program_end && findnum == linenum)
	{
		unsigned int dest, from;
		unsigned tomove;

		from = start + (int)Ram.read((int)(start+2));  //削除対象の行長をstartに加算
		dest = start;

		tomove = program_end - from;
		while( tomove > 0)
		{
                        c = Ram.read((int)from);
                        Ram.write((int)dest,c);
			from++;
			dest++;
			tomove--;
		}	
		program_end = dest;
	}

	if((unsigned char)Ram.read(txtpos+3) == NL) //入力されたプログラムになにも書かれていなかった
		goto prompt;



	// Make room for the new line, either all in one hit or lots of little shuffles
	while(linelen > 0)
	{	
		unsigned int tomove;
		unsigned int from,dest;
		unsigned int space_to_make;

		space_to_make = txtpos - program_end;

		if(space_to_make > linelen)
			space_to_make = linelen;
		newEnd = program_end+space_to_make;  //新しく入力される行の長さを加算したものが新しいprogram_end
		tomove = program_end - start;        //操作回数


		//プログラムを後ろから挿入歌書までnewEndからコピーしていく
		from = program_end;
		dest = newEnd;
		while(tomove > 0)
		{
			from--;
			dest--;
                        c = Ram.read((int)from);
                        Ram.write((int)dest,c);
			tomove--;
		}

		// Copy over the bytes into the new space
		for(tomove = 0; tomove < space_to_make; tomove++)
		{
                        c = Ram.read(txtpos);
                        Ram.write(start,c);
			txtpos++;
			start++;
			linelen--;
		}
		program_end = newEnd;
	}
	goto prompt;

unimplemented:
	printmsg(unimplimentedmsg);
	goto prompt;

badline:	
	printmsg(badlinemsg);
	goto prompt;
invalidexpr:
	printmsg(invalidexprmsg);
	goto prompt;
badnameerror:
        printmsg(badnamemsg);
        goto prompt;

syntaxerror:
	printmsg(syntaxmsg);
	if(current_line != 0)
	{
           unsigned char tmp = (unsigned char)Ram.read(txtpos);
		   if((unsigned char)Ram.read(txtpos) != NL)
                           Ram.write(txtpos,'^');    //エラーが出ている箇所を書き換える
			// txtpos = '^';
           list_line = current_line;    
           printline();    //エラーでた箇所書き換えたまま、一行表示
           Ram.write(txtpos,tmp);  //エラーでた箇所を元に戻す
	}
    line_terminator();
	goto prompt;

stackstuffed:	
	printmsg(stackstuffedmsg);
	goto warmstart;
nomem:	
	printmsg(nomemmsg);
	goto warmstart;

run_next_statement:
	while((unsigned char)Ram.read(txtpos) == ':')
		txtpos++;
	ignore_blanks();
	if((unsigned char)Ram.read(txtpos) == NL)
		goto execnextline;
	goto interperateAtTxtpos;

direct: 
	txtpos = program_end+sizeof(LINENUM);
	if((unsigned char)Ram.read(txtpos) == NL)  //ただの改行オンリー
		goto prompt;

interperateAtTxtpos:
        if(breakcheck())
        {
          printmsg(breakmsg);
          goto warmstart;
        }

	scantable(keywords);
	ignore_blanks();
        c = (unsigned char)Ram.read(txtpos);
        
	switch(table_index)
	{
		case KW_LIST:
			goto list;
		case KW_LOAD:
			goto unimplemented; /////////////////
		case KW_NEW:
			if( c != NL)
				goto syntaxerror;
                        variables_end = variables_table;
			program_end = program_start;
			goto prompt;
		case KW_RUN:
			current_line = program_start;
			goto execline;
		case KW_SAVE:
			goto unimplemented; //////////////////////
		case KW_NEXT:
			goto next;
		case KW_LET:
			goto assignment;
		case KW_IF:
			{
			short int val;
			expression_error = 0;
                        vartype=INTEGER;
			data = expression();
                        val=data.typeint;
                        c = (unsigned char)Ram.read(txtpos);
			if(expression_error || c == NL)
				goto invalidexpr;
			if(val != 0)
				goto interperateAtTxtpos;
			goto execnextline;
			}
		case KW_GOTO:
			expression_error = 0;
                        vartype=INTEGER;
                        data = expression(); 
			linenum = data.typeint;
                        c = (unsigned char)Ram.read(txtpos);
			if(expression_error || c != NL)
				goto invalidexpr;
			current_line = findline();
			goto execline;
		case KW_GOSUB:
			goto gosub;
		case KW_RETURN:
			goto gosub_return; 
		case KW_REM:	
			goto execnextline;	// Ignore line completely
		case KW_FOR:
			goto forloop; 
		case KW_INPUT:
			goto input; 
		case KW_PRINT:
			goto print;
		case KW_POKE:
			goto poke;
		case KW_STOP:
			// This is the easy way to end - set the current line to the end of program attempt to run it
			if(c != NL)
				goto syntaxerror;
			current_line = program_end;
			goto execline;
		case KW_BYE:
			// Leave the basic interperater
			return;
                case KW_DIM:
                        goto dim;
		case KW_DEFAULT:
			goto assignment;
		default:
			break;
	}

execnextline:
	if(current_line == 0)		// Processing direct commands?
		goto prompt;
	current_line +=	 (unsigned char)Ram.read((int)(current_line+2));  //行の長さを加算

execline:
  	if(current_line == program_end) // Out of lines to run
		goto warmstart;
	txtpos = current_line+sizeof(LINENUM)+sizeof(char);    //txtposはステート部へ
	goto interperateAtTxtpos;

input:
	{
		unsigned char isneg=0;
		unsigned int temptxtpos;

                short int integer=0;
                float decimal=0;
                
		ignore_blanks();
                
                var = findvar(&vartype);  //txtposは変数名の次にきている  vartypeには型情報
                if(var == variables_end){  //そんな変数名は無い
                    var = makevar(&vartype);  //変数作成⇒英字をすべて変数名とみなす
                    if(var == variables_end)  //makevar()を施してもダメ
                        goto badnameerror;
                    variables_end += get2byte(variables_end);  //変数末尾更新                 
                }
		if(!check_statement_end())
			goto syntaxerror;
again:
		temptxtpos = txtpos;
		if(!getln('?'))
			goto warmstart;

		txtpos = program_end+sizeof(LINENUM);    //getlnしたところの先頭、入力の頭

                c = (unsigned char)Ram.read(txtpos);
		if(c == '-')  //単項演算子
		{
			isneg = 1;
			txtpos++;
		}

/*変数名を検索する方法、マイナスを判断する方法を考える必要有*/
                
                c = (unsigned char)Ram.read(txtpos);
                if(c=='0'){  //最上位が0
                  txtpos++;  //1文字読み飛ばす⇒小数点．が来ないとおかしい
                }else if(c >= '1' && c<='9'){
                    while(c>='0' && c <='9'){
                       integer = integer*10 + c - '0';
                       txtpos++;
                       c = (unsigned char)Ram.read(txtpos);  
                    }
                }
                
                if(c == '.'){  //小数点以下の処理
                    txtpos++;
                    float i=0.1;
                    c = (unsigned char)Ram.read(txtpos);
                       while(c>='0' && c <='9'){
                           decimal = decimal + (c - '0') * i;
                           txtpos++;
                           c = (unsigned char)Ram.read(txtpos);
                           i *= 0.1;  
                    }
                  }
      
		ignore_blanks();
                c = (unsigned char)Ram.read(txtpos);

		if(c != NL)
		{
			printmsg(badinputmsg);
			goto again;
		}

		if(isneg){
                    decimal=-decimal;
                    integer=-integer;
                }
                
		txtpos = temptxtpos;

                if(vartype==INTEGER)
                    set2byte(var,integer);
                else if(vartype==FLOAT)
                    set4byte(var,decimal+integer);
                    
		goto run_next_statement;
	}
forloop:
	{
		short int initial, step, terminal;
                unsigned char datatype;
                vartype=INTEGER;
                var = findvar(&datatype);  //txtposは変数名の次にきている
                if(var == variables_end){  //そんな変数名は無い
                    var = makevar(&datatype);  //変数作成⇒英字をすべて変数名とみなす
                    if(var == variables_end)  //makevar()を施してもダメ
                        goto syntaxerror;
                    if(datatype != INTEGER)  //カウンタ変数はint型のみ
                        goto syntaxerror;
                    variables_end += get2byte(variables_end);  //変数末尾更新                 
                }
                
		scantable(relop_tab);
		if(table_index != RELOP_EQ)
			goto syntaxerror;

		expression_error = 0;

                vartype=INTEGER;
                data = expression();
		initial = data.typeint;
		if(expression_error)
			goto invalidexpr;

		scantable(to_tab);
		if(table_index != 0)
			goto syntaxerror;

                vartype = INTEGER;
                data = expression();
		terminal = data.typeint;
		if(expression_error)
			goto invalidexpr;

		scantable(step_tab);
		if(table_index == 0)
		{
                        data=expression();
			step = data.typeint;
			if(expression_error)
				goto invalidexpr;
		}
		else
			step = 1;
		if(!check_statement_end())
			goto syntaxerror;

		{
			struct stack_for_frame *f;
			if(framesp + sizeof(struct stack_for_frame) < stack_limit)
				goto nomem;
			framesp -= sizeof(struct stack_for_frame);
			f = (struct stack_for_frame *)framesp;
                        set2byte(var,initial);  //初期値設定。代入                                                                          
			f->frame_type = STACK_FOR_FLAG;
			f->for_var = var;
			f->terminal = terminal;
			f->step     = step;
			f->txtpos   = txtpos;
			f->current_line = current_line;
			goto run_next_statement;
		}
	}
	goto syntaxerror;

gosub:
	expression_error = 0;
        vartype=INTEGER;
        data=expression();
	linenum = data.typeint;
	if(expression_error)
		goto invalidexpr;
        c = (unsigned char)Ram.read(txtpos);
	if(!expression_error && c == NL)
	{
		struct stack_gosub_frame *f;
		if(framesp + sizeof(struct stack_gosub_frame) < stack_limit)
			goto nomem;

		framesp -= sizeof(struct stack_gosub_frame);
		f = (struct stack_gosub_frame *)framesp;
		f->frame_type = STACK_GOSUB_FLAG;
		f->txtpos = txtpos;
		f->current_line = current_line;
		current_line = findline();
		goto execline;
	}
	goto syntaxerror;

next:
	// Fnd the variable name
	ignore_blanks();
        var = findvar(&vartype);        
	if(var == variables_end){
        	goto syntaxerror;
        }
	if(!check_statement_end()){        
        	goto syntaxerror;
        }
gosub_return:
	tempsp = framesp;
	while(tempsp < memory+sizeof(memory)-1)
	{
		switch(tempsp[0])
		{
			case STACK_GOSUB_FLAG:
				if(table_index == KW_RETURN)
				{
					struct stack_gosub_frame *f = (struct stack_gosub_frame *)tempsp;
					current_line	= f->current_line;
					txtpos			= f->txtpos;
					framesp += sizeof(struct stack_gosub_frame);
					goto run_next_statement;
				}
				// This is not the loop you are looking for... so Walk back up the stack
				tempsp += sizeof(struct stack_gosub_frame);
				break;
			case STACK_FOR_FLAG:
				// Flag, Var, Final, Step
				if(table_index == KW_NEXT)
				{
					struct stack_for_frame *f = (struct stack_for_frame *)tempsp;
					// Is the the variable we are looking for?
                                        
					if(var == f->for_var)
					{       short int varaddr = get2byte(var)+f->step;
                                                set2byte(var,varaddr);
						// Use a different test depending on the sign of the step increment
						if((f->step > 0 && varaddr <= f->terminal) || (f->step < 0 && varaddr >= f->terminal))
						{
							// We have to loop so don't pop the stack
							txtpos = f->txtpos;
							current_line = f->current_line;
							goto run_next_statement;
						}
						// We've run to the end of the loop. drop out of the loop, popping the stack
						framesp = tempsp + sizeof(struct stack_for_frame);
						goto run_next_statement;
					}
				}
				// This is not the loop you are looking for... so Walk back up the stack
				tempsp += sizeof(struct stack_for_frame);
				break;
			default:
				goto stackstuffed;
		}
	}
	// Didn't find the variable we've been looking for
	goto syntaxerror;

assignment:
	{              
                var = findvar(&vartype);  //変数名に対応する値が入っている場所を指す
                if(var == variables_end){  //そんな変数名は無い
                    var = makevar(&vartype);  //変数作成⇒英字をすべて変数名とみなす               
                    if((var == variables_end) && (vartype != STRING)){  //makevar()を施してもダメ
                        goto syntaxerror;
                    }
                    variables_end += get2byte(variables_end);  //変数末尾更新                 
                }
                if(var == 0)//配列入力してミスった
                    goto syntaxerror;                                    
		ignore_blanks();
                c = (unsigned char)Ram.read(txtpos);
		if (c != '='){

                	goto syntaxerror;
		}
                txtpos++;
		ignore_blanks();
                if(vartype==STRING){
                   if((unsigned char)Ram.read(txtpos)=='"'){
                       txtpos++;                    
                       setstring(var);
                   }else{
                     goto syntaxerror;
                   }
                   goto run_next_statement; 
                }
                
		expression_error = 0;
		data = expression();
  
		if(expression_error)
			goto invalidexpr;
		// Check that we are at the end of the statement
		if(!check_statement_end())
			goto syntaxerror;
                if(vartype==INTEGER)
                    set2byte(var,data.typeint);
                else if(vartype==FLOAT)
                    set4byte(var,data.typefloat);
                    
	}
	goto run_next_statement;
dim:
      {
        short int value;
        var = findvar(&vartype);  //まずは書いた変数名が使用できるかを確認する  
        if(var != variables_end)
              goto badnameerror;  //使用できなかった
        /*この時点でfindvarによるtxtposの移動はない*/

        if(!makedim())
            goto badnameerror;          
        if(!check_statement_end())
	      goto syntaxerror;    
      }
      goto run_next_statement;
poke:
	{
		short int value;
		short int address;

		// Work out where to put it
		expression_error = 0;
                vartype=INTEGER;
                data = expression();
		address = data.typeint;

		if(expression_error)
                	goto invalidexpr;
		ignore_blanks();
                c = Ram.read(txtpos);
		if (c != ',')
			goto syntaxerror;
                txtpos++;
		ignore_blanks();
		// Now get the value to assign
		expression_error = 0;
                vartype=INTEGER;
                data=expression();
		value = data.typeint;
		if(expression_error)
		     goto invalidexpr;
                
                Ram.write(address,(char)value);
		if(!check_statement_end())
			goto syntaxerror;
	}
	goto run_next_statement;

list:
	linenum = testnum(); // Retuns 0 if no line found.

	// Should be EOL
	if((unsigned char)Ram.read(txtpos) != NL)
		goto syntaxerror;

	// Find the line
	list_line = findline();
	while(list_line != program_end)
          printline();
	goto warmstart;

print:
        c = (unsigned char)Ram.read(txtpos);       
	// If we have an empty list then just put out a NL
	if(c == ':' )
	{
        line_terminator();
		txtpos++;
		goto run_next_statement;
	}
	if(c == NL)
	{
		goto execnextline;
	}

	while(1)
	{
		ignore_blanks();
		if(print_quoted_string())
		{
			;
		}
		else if((unsigned char)Ram.read(txtpos) == '"' || (unsigned char)Ram.read(txtpos) == '\'')
			goto syntaxerror;
		else
		{       if(c=='%'){
                              vartype=FLOAT;
                              txtpos++;
                              ignore_blanks();
                              data=expression();
			      if(expression_error)
				  goto invalidexpr;
                              Serial.println(data.typefloat,6);
                        }else if(c=='$'){
                              txtpos++;
                              ignore_blanks();
                              var = findvar(&vartype);
                              Serial.println(var);
                              if(vartype==STRING)
                                  printstring(var);
                              else
                                  goto invalidexpr;                             
                        }else{
                            vartype=INTEGER;
			    expression_error = 0;
			    data = expression();
  			    if(expression_error)
  				  goto invalidexpr;
			    printnum(data.typeint);
                        }
		}

		// At this point we have three options, a comma or a new line
		if((unsigned char)Ram.read(txtpos) == ',')
			txtpos++;	// Skip the comma and move onto the next
		else if((unsigned char)Ram.read(txtpos) == ';' && ((unsigned char)Ram.read(txtpos+1) == NL ||(unsigned char)Ram.read(txtpos+1) == ':'))
		{
			txtpos++; // This has to be the end of the print - no newline
			break;
		}
		else if(check_statement_end())
		{
			line_terminator();	// The end of the print statement
			break;
		}
		else{
			goto syntaxerror;
                }	
	}
	goto run_next_statement;
}

/***************************************************************************/
static void line_terminator(void)
{
  	outchar(NL);
	outchar(CR);
}


/***********************************************************/
static unsigned char breakcheck(void)
{
#ifdef ARDUINO
  if(Serial.available())
    return Serial.read() == CTRLC;
  return 0;
#else
  if(kbhit())
    return getch() == CTRLC;
   else
     return 0;
#endif
}
/***********************************************************/
static int inchar()
{
#ifdef ARDUINO
  while(1)
  {
    if(Serial.available())
      return Serial.read();
  }
#else
	return getch();
#endif
}

/***********************************************************/
static void outchar(unsigned char c)
{
#ifdef ARDUINO
  Serial.write(c);
#else
	putch(c);
#endif
}

#ifdef ARDUINO
/***********************************************************/
void setup()
{
  Serial.begin(9600);  
}
#endif

#ifndef ARDUINO
//***********************************************************/
int main()
{
 while(1)
  loop();
}
/***********************************************************/
#endif
