// TinyBASIC.cpp : An implementation of TinyBASIC in C
//
// Author : Mike Field - hamster@snap.net.nz
//
// Based on TinyBasic for 68000, by Gordon Brandly
// (see http://members.shaw.ca/gbrandly/68ktinyb.html)
//
// which itself was Derived from Palo Alto Tiny BASIC as 
// published in the May 1976 issue of Dr. Dobb's Journal.  
// 
// 0.03 21/01/2011 : Added INPUT routine 
//                 : Reorganised memory layout
//                 : Expanded all error messages
//                 : Break key added
//                 : Removed the calls to printf (left by debugging)

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
#define KW_DEFAULT	18

struct stack_for_frame {
	char frame_type;
	char for_var;
	short int terminal;
	short int step;
	unsigned char *current_line;
	unsigned char *txtpos;
};

struct stack_gosub_frame {
	char frame_type;
	unsigned char *current_line;
	unsigned char *txtpos;
};

static unsigned char func_tab[] = {
	'P','E','E','K'+0x80,
	'A','B','S'+0x80,
	0
};
#define FUNC_PEEK    0
#define FUNC_ABS	 1
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

static unsigned char memory[100];
static unsigned char *txtpos,*list_line;
static unsigned char expression_error;
static unsigned char *tempsp;
static unsigned char *stack_limit;
static unsigned char *program_start;
static unsigned char *program_end;
static unsigned char *stack; // Software stack for things that should go on the CPU stack
static unsigned char *variables_table;
static unsigned char *current_line;
static unsigned char *sp;
#define STACK_GOSUB_FLAG 'G'
#define STACK_FOR_FLAG 'F'
static unsigned char table_index;
static LINENUM linenum;

static const unsigned char okmsg[]		= "OK";
static const unsigned char badlinemsg[]		= "Invalid line number";
static const unsigned char invalidexprmsg[] = "Invalid expression";
static const unsigned char syntaxmsg[] = "Syntax Error";
static const unsigned char badinputmsg[] = "\nBad number";
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
static short int expression(void);
static unsigned char breakcheck(void);


/***************************************************************************/
static void ignore_blanks(void)
{
	while((unsigned char)Ram.read((int)txtpos) == SPACE || (unsigned char)Ram.read((int)txtpos) == TAB)
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
		if((unsigned char)Ram.read((int)(txtpos+i)) == table[0])
		{
			i++;
			table++;
		}
		else
		{
			// do we match the last character of keywork (with 0x80 added)? If so, return
			if((unsigned char)Ram.read((int)(txtpos+i))+0x80 == table[0])
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
        Ram.write((int)sp,b);
//	*sp = b;
}

/***************************************************************************/
static unsigned char popb()
{
	unsigned char b;

        b = (unsigned char)Ram.read((int)sp);
//	b = *sp;
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
        c = (unsigned char)Ram.read((int)txtpos);
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
                c = (unsigned char)Ram.read((int)txtpos);
	}
	return	num;
}

/***************文が終わっているかをチェックする********************/
unsigned char check_statement_end(void)
{
	ignore_blanks();
        unsigned char c = (unsigned char)Ram.read((int)txtpos);
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
	unsigned char delim = (unsigned char)Ram.read((int)txtpos);
        
	if(delim != '"' && delim != '\'')    //引用符じゃなかったら終わり
		return 0;
	txtpos++;
        
        // Check we have a closing delimiter
	while((unsigned char)Ram.read((int)(txtpos+i)) != delim)
	{
		if((unsigned char)Ram.read((int)(txtpos+i)) == NL)  //引用符の閉じがこなかった
			return 0;
		i++;
	}
        
	// Print the characters
	while((unsigned char)Ram.read((int)txtpos) != delim)
	{
		outchar((unsigned char)Ram.read((int)txtpos));
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
				Ram.write((int)txtpos, NL);
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
                                        Ram.write((int)txtpos,c);
				//	txtpos[0] = c;
					txtpos++;
					outchar(c);
				}
		}
	}
}

/***************************************************************************/
static unsigned char *findline(void)
{
	unsigned char *line = program_start;
        short int findnum=0;
	while(1)
	{
		if(line == program_end)
			return line;
                findnum = (int)Ram.read((int)line) + (int)Ram.read((int)(line+1))*256;
		if( findnum >= linenum)
			return line;

		// Add the line length onto the current address, to get to the next line;
		line += (unsigned char)Ram.read((int)(line+2));  //lineから3番目には行長が入っている
	}
}

/***************************************************************************/
static void toUppercaseBuffer(void)
{
	unsigned char *c = program_end+sizeof(LINENUM);
	unsigned char quote = 0;
        unsigned char tmp;

        tmp = (unsigned char)Ram.read((int)c);
	while(tmp != NL)
	{
		// Are we in a quoted string?
		if(tmp == quote)
			quote = 0;
		else if(tmp == '"' || tmp == '\'')
			quote = *c;
		else if(quote == 0 && tmp >= 'a' && tmp <= 'z'){
			tmp = tmp + 'A' - 'a';
                        Ram.write((int)c,tmp);  //大文字にしたtmpをcに書き込む
                }
		c++;
                tmp = (unsigned char)Ram.read((int)c);  
	}
}


/***********１月９日　こっから******/
/***************************************************************************/
void printline()
{
	LINENUM line_num;
	
        line_num = (int)Ram.read((int)list_line) + (int)Ram.read((int)(list_line+1))*256;
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
/*        
	line_num = *((LINENUM *)(list_line));
    list_line += sizeof(LINENUM) + sizeof(char);

	// Output the line 
	printnum(line_num);
	outchar(' ');
	while(*list_line != NL)
    {
		outchar(*list_line);
		list_line++;
	}
	list_line++;
	line_terminator();
*/
}

/************直値、変数、関数、括弧の対応を計算***************************************/
static short int expr4(void)
{
	short int a = 0;
        unsigned char c;  //txtposの文字をそんままもらってくる
        c = (unsigned char)Ram.read((int)txtpos);

//単純な数式の処理
	if(c == '0')
	{
		txtpos++;
		a = 0;
		goto success;
	}

	if(c >= '1' && c <= '9')  //１桁以上のとき
	{
		do 	{
			a = a*10 + c - '0';
			txtpos++;
                        c = (unsigned char)Ram.read((int)txtpos);
		} while(c >= '0' && c <= '9');
			goto success;
	}

//文字であるかどうか確認
	// Is it a function or variable reference?
	if(c >= 'A' && c <= 'Z')    //変数or関数である時
	{ 
		// Is it a variable reference (single alpha)
		if((unsigned char)Ram.read((int)(txtpos+1)) < 'A' || (unsigned char)Ram.read((int)(txtpos+1)) > 'Z')  //一文字の時、つまり変数の時
		{
                        int variable = (int)variables_table + c - 'A';  //変数のRAM上の場所を指すためのvariable
                        
			a = (int)Ram.read(variable) + (int)Ram.read((variable+1))*256;
			txtpos++;
			goto success;
		}

		// Is it a function with a single parameter
		scantable(func_tab);  //関数なのかチェック
		if(table_index == FUNC_UNKNOWN)
			goto expr4_error;

		unsigned char f = table_index;
                c = (unsigned char)Ram.read((int)txtpos);  //関数読み終えた後の括弧
                
		if(c != '(')
			goto expr4_error;

		txtpos++;
		a = expression();
                
                c = (unsigned char)Ram.read((int)txtpos);  //閉じ括弧があるか
		if(*txtpos != ')')
				goto expr4_error;
		
                txtpos++;
		switch(f)
		{
			case FUNC_PEEK:
				//a =  memory[a];
                                /*石井くん神↓*/
				a = (int)Ram.read(a);  //Ram上のa番目の１バイトを返す
                                goto success;
			case FUNC_ABS:
				if(a < 0)
					a = -a;
				goto success;
		}
	}
        
	if(c == '(')  //いきなり開き括弧
	{
		txtpos++;
		a = expression();
                c = (unsigned char)Ram.read((int)txtpos);  //閉じ括弧チェック
		if(c != ')')
			goto expr4_error;

		txtpos++;
		goto success;
	}

expr4_error:
	expression_error = 1;
success:
	ignore_blanks();
	return a;
}

/********掛け算、割り算****************************/
static short int expr3(void)
{
	short int a,b;
        unsigned char c;      

	a = expr4();

        c = (unsigned char)Ram.read((int)txtpos);
	while(1)
	{
		if(c == '*')
		{
			txtpos++;
			b = expr4();
			a *= b;
		}
		else if(c == '/')
		{
			txtpos++;
			b = expr4();
			if(b != 0)
				a /= b;
			else
				expression_error = 1;
		}
		else
			return a;  //乗算除算の処理をやり終えた。
	}
}

/************加算、減算の処理**************************/
static short int expr2(void)
{
	short int a,b;
        unsigned char c;
        c = (unsigned char)Ram.read((int)txtpos);
        
    //前の項を評価 
	if(c == '-' || c == '+')
		a = 0;
	else
		a = expr3();

        c = (unsigned char)Ram.read((int)txtpos);
	while(1)
	{
		if(c == '-')
		{
			txtpos++;
			b = expr3();
			a -= b;
		}
		else if(c == '+')
		{
			txtpos++;
			b = expr3();
			a += b;
		}
		else
			return a;

               c = (unsigned char)Ram.read((int)txtpos);   //次の文字へ
	}
}

/***************************************************************************/
static short int expression(void)
{
	short int a,b;

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
		if(a >= b) return 1;
		break;
	case RELOP_NE:
		b = expr2();
		if(a != b) return 1;
		break;
	case RELOP_GT:
		b = expr2();
		if(a > b) return 1;
		break;
	case RELOP_EQ:
		b = expr2();
		if(a == b) return 1;
		break;
	case RELOP_LE:
		b = expr2();
		if(a <= b) return 1;
		break;
	case RELOP_LT:
		b = expr2();
		if(a < b) return 1;
		break;
	}
	return 0;
}

/***************************************************************************/
void loop()
{
    Serial.println('a');
    Serial.flush();
    SPI.begin();    //SPIを使用開始
    Ram.begin();  //myRAM(cs(10)ピンのMYRAM使用開始)
  
	unsigned char *start;
	unsigned char *newEnd;
	unsigned char linelen;
        unsigned char c;	
        short int findnum;    
	variables_table = (unsigned char *)(64*1024 + 1); //変数領域
	program_start = (unsigned char *)1;      //プログラム領域先頭
	program_end = program_start;
	sp =  (unsigned char *)(63*1024);  // Needed for printnum
	printmsg(initmsg);
	printnum(sp-program_end);
	printmsg(memorymsg);

warmstart:
	// this signifies that it is running in 'direct' mode.
	current_line = 0;
	sp =  (unsigned char *)(63*1024);;  
	printmsg(okmsg);

prompt:
        //getlnしたら、program_endの2バイト先から入力を書き込む
	while(!getln('>'))
		line_terminator();
	toUppercaseBuffer();

	txtpos = program_end+sizeof(unsigned short);  //入力したところに戻る。
        
        c = (unsigned char)Ram.read((int)txtpos);
	// Find the end of the freshly entered line
	while(c != NL){      //入力文末まで移動
		txtpos++;
                c = (unsigned char)Ram.read((int)txtpos);
        }
        
        
        //現在txtposは入力した文の文末にきている
	// Move it to the end of program_memory

        //プログラム領域の末尾(sp)に後ろから入力文をコピー
	{
		unsigned char *dest;
		dest = sp-1;
		while(1)
		{       c = (unsigned char)Ram.read((int)txtpos);
                        Ram.write((int)dest,c);
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
        while((unsigned char)Ram.read((int)(txtpos+linelen)) != NL)  //ステートメントの長さを測る
              linelen++;
        linelen++;  //文末NULL文字分
        linelen += sizeof(unsigned short)+sizeof(char);  //行番号、行長分

	// Now we have the number, add the line header.
	txtpos -= 3;  //入力文から３バイト前に移動（行番号、行長、入力文と続くようにする)
        
        Ram.write((int)txtpos,(linenum % 256));  //行番号下位ビット
        Ram.write((int)(txtpos+1),(linenum / 256));  //行番号上位ビット      
        Ram.write((int)(txtpos+2),linelen);

//	*((unsigned short *)txtpos) = linenum;
//	txtpos[sizeof(LINENUM)] = linelen;


	// Merge it into the rest of the program
	start = findline();
        findnum = (int)Ram.read((int)start) + (int)Ram.read((int)(start+1))*256;
              //みつけた場所の行番号
        
	//同じ行番号があったため、とりあえず削除
	if(start != program_end && findnum == linenum)
	{
		unsigned char *dest, *from;
		unsigned tomove;

		from = start + (int)Ram.read((int)(start+2));  //削除対象の行長をstartに加算
		dest = start;

		tomove = program_end - from;
		while( tomove > 0)
		{
                        c = Ram.read((int)from);
                        Ram.write((int)dest,c);
//			*dest = *from;
			from++;
			dest++;
			tomove--;
		}	
		program_end = dest;
	}

	if((unsigned char)Ram.read((int)(txtpos+3)) == NL) //入力されたプログラムになにも書かれていなかった
		goto prompt;



	// Make room for the new line, either all in one hit or lots of little shuffles
	while(linelen > 0)
	{	
		unsigned int tomove;
		unsigned char *from,*dest;
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
		//	*dest = *from;
			tomove--;
		}

		// Copy over the bytes into the new space
		for(tomove = 0; tomove < space_to_make; tomove++)
		{
                        c = Ram.read((int)txtpos);
                        Ram.write((int)start,c);
		//	*start = *txtpos;
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
syntaxerror:
	printmsg(syntaxmsg);
	if(current_line != (void *)0)
	{
           unsigned char tmp = (unsigned char)Ram.read((int)txtpos);
		   if((unsigned char)Ram.read((int)txtpos) != NL)
                           Ram.write((int)txtpos,'^');    //エラーが出ている箇所を書き換える
			// txtpos = '^';
           list_line = current_line;    
           printline();    //エラーでた箇所書き換えたまま、一行表示
           Ram.write((int)txtpos,tmp);  //エラーでた箇所を元に戻す
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
	while((unsigned char)Ram.read((int)txtpos) == ':')
		txtpos++;
	ignore_blanks();
	if((unsigned char)Ram.read((int)txtpos) == NL)
		goto execnextline;
	goto interperateAtTxtpos;

direct: 
	txtpos = program_end+sizeof(LINENUM);
	if((unsigned char)Ram.read((int)txtpos) == NL)  //ただの改行オンリー
		goto prompt;

interperateAtTxtpos:
        if(breakcheck())
        {
          printmsg(breakmsg);
          goto warmstart;
        }

	scantable(keywords);
	ignore_blanks();
        c = (unsigned char)Ram.read((int)txtpos);
        
	switch(table_index)
	{
		case KW_LIST:
			goto list;
		case KW_LOAD:
			goto unimplemented; /////////////////
		case KW_NEW:
			if( c != NL)
				goto syntaxerror;
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
			val = expression();
			if(expression_error || c == NL)
				goto invalidexpr;
			if(val != 0)
				goto interperateAtTxtpos;
			goto execnextline;
			}
		case KW_GOTO:
			expression_error = 0;
			linenum = expression();
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
		case KW_DEFAULT:
			goto assignment;
		default:
			break;
	}
	
execnextline:
	if(current_line == (void *)0)		// Processing direct commands?
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
		unsigned char *temptxtpos;
		short int var;
                short int value=0;
		ignore_blanks();
                c = (unsigned char)Ram.read((int)txtpos);
		if(c < 'A' || c > 'Z')
			goto syntaxerror;
		var = (int)variables_table + c-'A';    //verは変数の場所
		txtpos++;
		if(!check_statement_end())
			goto syntaxerror;
again:
		temptxtpos = txtpos;
		if(!getln('?'))
			goto warmstart;

		txtpos = program_end+sizeof(LINENUM);    //getlnしたところの先頭、入力の頭

//                c = (unsigned char)Ram.read((int)txtpos);
//		if(c == '-')  //単項演算子
//		{
//			isneg = 1;
//			txtpos++;
//		}

/*変数名を検索する方法、マイナスを判断する方法を考える必要有*/
                
                c = (unsigned char)Ram.read((int)txtpos);
		do {
			value = value*10 + c - '0';
			txtpos++;
                        c = (unsigned char)Ram.read((int)txtpos);
		} while(c >= '0' && c <= '9');
		ignore_blanks();
                c = (unsigned char)Ram.read((int)txtpos);
		if(c != NL)
		{
			printmsg(badinputmsg);
			goto again;
		}
	
//		if(isneg)
//			value = -value;

    //型宣言してバイト数増やすときは型情報からwhile文の回数を変更できるようにする
                int bytesize = 2;
                int digit = 0;
                int tmp=value;
                while(bytesize > digit){                     
                       Ram.write(var+digit,value % 256);  //valueの下位ビット
                       value = value / 256;
                       digit++;               
                }
		goto run_next_statement;
	}
forloop:
	{
		unsigned char var;
		short int initial, step, terminal;
 
                c = (unsigned char)Ram.read((int)txtpos);
		if(c < 'A' || c > 'Z')
			goto syntaxerror;
		var = c;
		txtpos++;
		
		scantable(relop_tab);
		if(table_index != RELOP_EQ)
			goto syntaxerror;

		expression_error = 0;
		initial = expression();
		if(expression_error)
			goto invalidexpr;
	
		scantable(to_tab);
		if(table_index != 0)
			goto syntaxerror;
	
		terminal = expression();
		if(expression_error)
			goto invalidexpr;
	
		scantable(step_tab);
		if(table_index == 0)
		{
			step = expression();
			if(expression_error)
				goto invalidexpr;
		}
		else
			step = 1;
		if(!check_statement_end())
			goto syntaxerror;

                c = (unsigned char)Ram.read((int)txtpos);
		if(!expression_error && c == NL)
		{
			struct stack_for_frame *f;
			if(sp + sizeof(struct stack_for_frame) < stack_limit)
				goto nomem;

			sp -= sizeof(struct stack_for_frame);
			f = (struct stack_for_frame *)sp;
			((short int *)variables_table)[var-'A'] = initial;
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
	linenum = expression();
	if(expression_error)
		goto invalidexpr;
	if(!expression_error && *txtpos == NL)
	{
		struct stack_gosub_frame *f;
		if(sp + sizeof(struct stack_gosub_frame) < stack_limit)
			goto nomem;

		sp -= sizeof(struct stack_gosub_frame);
		f = (struct stack_gosub_frame *)sp;
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
	if(*txtpos < 'A' || *txtpos > 'Z')
		goto syntaxerror;
	txtpos++;
	if(!check_statement_end())
		goto syntaxerror;
	
gosub_return:
	// Now walk up the stack frames and find the frame we want, if present
	tempsp = sp;
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
					sp += sizeof(struct stack_gosub_frame);
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
					if(txtpos[-1] == f->for_var)
					{
						short int *varaddr = ((short int *)variables_table) + txtpos[-1] - 'A'; 
						*varaddr = *varaddr + f->step;
						// Use a different test depending on the sign of the step increment
						if((f->step > 0 && *varaddr <= f->terminal) || (f->step < 0 && *varaddr >= f->terminal))
						{
							// We have to loop so don't pop the stack
							txtpos = f->txtpos;
							current_line = f->current_line;
							goto run_next_statement;
						}
						// We've run to the end of the loop. drop out of the loop, popping the stack
						sp = tempsp + sizeof(struct stack_for_frame);
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
		int value;
		short int var;
                c = (unsigned char)Ram.read((int)(txtpos));
		if(c < 'A' || c > 'Z')
			goto syntaxerror;
		var = (int)variables_table + c - 'A';
		txtpos++;

		ignore_blanks();
                c = (unsigned char)Ram.read((int)(txtpos));
		if (c != '=')
			goto syntaxerror;
		txtpos++;
		ignore_blanks();
		expression_error = 0;
		value = expression();
		if(expression_error)
			goto invalidexpr;
		// Check that we are at the end of the statement
		if(!check_statement_end())
			goto syntaxerror;
                               int bytesize = 2;
                int digit = 0;
                int tmp=value;
                while(bytesize > digit){                     
                       Ram.write(var+digit,value % 256);  //valueの下位ビット
                       value = value / 256;
                       digit++;               
                } 
                 

	}
	goto run_next_statement;
poke:
	{
		short int value;
		short int address;

		// Work out where to put it
		expression_error = 0;
		address = expression();
		if(expression_error){
		        Serial.print("invalid1");
                	goto invalidexpr;
                }
		// check for a comma
		ignore_blanks();
		if (*txtpos != ','){
		        Serial.print("syntax1");
			goto syntaxerror;
                }
		txtpos++;
		ignore_blanks();
		// Now get the value to assign
		expression_error = 0;
		value = expression();
		if(expression_error){
		        Serial.print("invalid2");                        
			goto invalidexpr;
                }
//addres番目にvalueをいれるだけ
                //memory[address] = value;
                *(short int*)(memory + address) = value;
                //printf("Poke %p value %i\n",address, (unsigned char)value);
		// Check that we are at the end of the statement
		if(!check_statement_end()){
		        Serial.print("syntax2");
			goto syntaxerror;
                }
	}
	goto run_next_statement;

list:
	linenum = testnum(); // Retuns 0 if no line found.

	// Should be EOL
	if((unsigned char)Ram.read((int)txtpos) != NL)
		goto syntaxerror;

	// Find the line
	list_line = findline();
	while(list_line != program_end)
          printline();
	goto warmstart;

print:
        c = (unsigned char)Ram.read((int)txtpos);       
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
		else if((unsigned char)Ram.read((int)txtpos) == '"' || (unsigned char)Ram.read((int)txtpos) == '\'')
			goto syntaxerror;
		else
		{
			short int e;
			expression_error = 0;
			e = expression();
			if(expression_error)
				goto invalidexpr;
			printnum(e);
		}

		// At this point we have three options, a comma or a new line
		if((unsigned char)Ram.read((int)txtpos) == ',')
			txtpos++;	// Skip the comma and move onto the next
		else if((unsigned char)Ram.read((int)txtpos) == ';' && ((unsigned char)Ram.read((int)(txtpos+1)) == NL ||(unsigned char)Ram.read((int)(txtpos+1)) == ':'))
		{
			txtpos++; // This has to be the end of the print - no newline
			break;
		}
		else if(check_statement_end())
		{
			line_terminator();	// The end of the print statement
			break;
		}
		else
			goto syntaxerror;	
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
