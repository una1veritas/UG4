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
#include <string.h>
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
	unsigned char frame_type;
	unsigned char for_var;
	short int terminal;
	short int step;
	unsigned char *current_line;
	unsigned char *txtpos;
};

struct stack_gosub_frame {
	unsigned char frame_type;
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

static unsigned char memory[200];
static unsigned char program[300];  //プログラム用メモリー
static unsigned char Buffer[64];  //文格納用BUFFER
static unsigned char tmpBuf[64];  //作業用BUFFER

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


/******行の情報を持つ構造体*************/ 
struct infomation{
    unsigned short num;  //先頭２バイトは行番号
    unsigned char len;  //次点１バイトは行の長さ() 
    char txt;  //
  };
/**************************************/

/************行データそのものの共用体line_t***********************/
union line_t {
  char state;  //文の先頭からcharで読み出せるよnullまでコピればいい感じにできるよ
  struct infomation info;  //行の情報
};
/*****************************************************************/



/***************************************************************************/
static void ignore_blanks(void)
{
	while(*txtpos == SPACE || *txtpos == TAB)
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
		if(txtpos[i] == table[0])
		{
			i++;
			table++;
		}
		else
		{
			// do we match the last character of keywork (with 0x80 added)? If so, return
			if(txtpos[i]+0x80 == table[0])
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
	*sp = b;
}

/***************************************************************************/
static unsigned char popb()
{
	unsigned char b;
	b = *sp;
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
/***************************************************************************/
static unsigned short testnum(void)
{
	unsigned short num = 0;
	ignore_blanks();
	
	while(*txtpos>= '0' && *txtpos <= '9' )
	{
		// Trap overflows
		if(num >= 0xFFFF/10)
		{
			num = 0xFFFF;
			break;
		}

		num = num *10 + *txtpos - '0';
		txtpos++;
	}
	return	num;
}

/***************************************************************************/
unsigned char check_statement_end(void)
{
	ignore_blanks();
	return (*txtpos == NL) || (*txtpos == ':');
}

/***************************************************************************/
static void printmsgNoNL(const unsigned char *msg)
{
	while(*msg)
	{
		outchar(*msg);
		msg++;
	}
}

/***************************************************************************/
static unsigned char print_quoted_string(void)
{
	int i=0;
	unsigned char delim = *txtpos;
	if(delim != '"' && delim != '\'')
		return 0;
	txtpos++;

	// Check we have a closing delimiter
	while(txtpos[i] != delim)
	{
		if(txtpos[i] == NL)
			return 0;
		i++;
	}

	// Print the characters
	while(*txtpos != delim)
	{
		outchar(*txtpos);
		txtpos++;
	}
	txtpos++; // Skip over the last delimiter
	ignore_blanks();

	return 1;
}

/***************************************************************************/
static void printmsg(const unsigned char *msg)
{
	printmsgNoNL(msg);
    line_terminator();
}

/**********作業用バッファのtmpbufに文字列としてそのまま書き込む********************/
unsigned char getln(char prompt)
{
	outchar(prompt);
	txtpos = tmpBuf;

	while(1)
	{
		char c = inchar();
		switch(c)
		{
			case CR:
			case NL:
                line_terminator();
				// Terminate all strings with a NL
				txtpos[0] = NL;
				return 1;
			case CTRLC:
				return 0;
			case CTRLH:
				if(txtpos == tmpBuf)
					break;
				txtpos--;
				printmsgNoNL(backspacemsg);
				break;
			default:
				// We need to leave at least one space to allow us to shuffle the line into order
				if(txtpos == (tmpBuf+128))
					outchar(BELL);
				else
				{
					txtpos[0] = c;
					txtpos++;
					outchar(c);
				}
		}
	}
}


/**********num以上の行番号をもつところの行の先頭を返す********************************/
static unsigned char *findline(short int num)
{
	unsigned char *pos = program_start;
        union line_t *posline;
	while(1)
	{
                posline = (union line_t *)pos;
		if(pos == program_end){
 //                       Serial.println('a');
  			return pos;
                }

		if(posline->info.num >= num)
			return pos;
		// Add the line length onto the current address, to get to the next line;
		pos = pos + posline->info.len;  //次の行にシフト
	}
}


void printBuffer(){
  int i=0;
  while(Buffer[i] != NL){
     outchar(Buffer[i]);
     i++;
  }
  Serial.print('\n');
}

/****行の挿入を行う関数、行情報はnewline、文はBUFFERに入っている
挿入が未完成。末尾からの追加、行の削除はできている。
********/
void insert_line(union line_t newline){
  
    unsigned char *start = findline(newline.info.num);
    union line_t *linepos;
    
    linepos = (union line_t *)start;
    
//    Serial.println('S');
//    printnum((int)linepos->info.num);
//   Serial.print('\n');
    
    unsigned char *from;
    unsigned char *dest;
    int tomove;
    
    /********行番号が同じならとりあえず削除してから************/
    if((linepos->info.num == newline.info.num) && (start != program_end)){
        from = start + linepos->info.len;
        dest = start;
        tomove = program_end - from;
        
        while(tomove>0){
            *dest = *from;
            from++;
            dest++;
            tomove--;        
        }
        program_end = dest;
    }
    
    if(newline.info.len == 4)  //文字が何もかかれていなければそのままreturn
        return;

 //   printBuffer();    
    from = program_end;
    dest = program_end + newline.info.len;
    tomove = from - start;
    
    program_end = dest;
    
    while(tomove > 0){
        dest--;
        from--;
        *dest = *from;
        tomove--;
    }
    
    linepos->info.num = newline.info.num;
    linepos->info.len = newline.info.len;
    strcpy(&(linepos->info.txt),(char *)Buffer);

    char *test = &(linepos->info.txt);

    while(*test != NL){
        outchar(*test);
        test++;
    }
    Serial.print('\n');
    
}





/*******作業用BUFFERのTMPBUFFERを大文字に書き換える*****************************************************************/
static void toUppercaseBuffer(void)
{
	unsigned char *c = tmpBuf;
	unsigned char quote = 0;

	while(*c != NL)
	{
		// Are we in a quoted string?
		if(*c == quote)
			quote = 0;
		else if(*c == '"' || *c == '\'')
			quote = *c;
		else if(quote == 0 && *c >= 'a' && *c <= 'z')
			*c = *c + 'A' - 'a';
		c++;
	}
}

/*****list_lineから一行文表示する**************************/
void printline()
{
     union line_t *posline;
     posline = (union line_t *)list_line;
     
     printnum(posline->info.num);
     outchar(' ');
     list_line=list_line + sizeof(short int) + sizeof(unsigned char);
     while(*list_line != NL)
        {
		outchar(*list_line);
		list_line++;
	}
	list_line++;  //次の行の先頭にくる
	line_terminator();     
/*        union line_t *posline;
  	LINENUM line_num;
	
	posline = (union line_t *)list_line;

	// Output the line 
	printnum(posline->info.num);
	outchar(' ');
        list_line = &((unsigned char)(posline->info.txt));  //poslineの文の先頭にポインタ持ってくる
	while(*list_line != NL)
        {
		outchar(*list_line);
		list_line++;
	}
	list_line++;  //次の行の先頭にくる
	line_terminator();
*/
}

/***************************************************************************/
static short int expr4(void)
{
	short int a = 0;

//単純な数式の処理
	if(*txtpos == '0')
	{
		txtpos++;
		a = 0;
		goto success;
	}

	if(*txtpos >= '1' && *txtpos <= '9')
	{
		do 	{
			a = a*10 + *txtpos - '0';
			txtpos++;
		} while(*txtpos >= '0' && *txtpos <= '9');
			goto success;
	}

//文字であるかどうか確認
	// Is it a function or variable reference?
	if(txtpos[0] >= 'A' && txtpos[0] <= 'Z')
	{
		// Is it a variable reference (single alpha)
		if(txtpos[1] < 'A' || txtpos[1] > 'Z')
		{
			a = ((short int *)variables_table)[*txtpos - 'A'];
			txtpos++;
			goto success;
		}

		// Is it a function with a single parameter
		scantable(func_tab);
		if(table_index == FUNC_UNKNOWN)
			goto expr4_error;

		unsigned char f = table_index;

		if(*txtpos != '(')
			goto expr4_error;

		txtpos++;
		a = expression();
		if(*txtpos != ')')
				goto expr4_error;
		txtpos++;
		switch(f)
		{
			case FUNC_PEEK:
				//a =  memory[a];
                                /*石井くん神↓*/
				a = *(short int*)(memory + a);
                                goto success;
			case FUNC_ABS:
				if(a < 0)
					a = -a;
				goto success;
		}
	}

	if(*txtpos == '(')
	{
		txtpos++;
		a = expression();
		if(*txtpos != ')')
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

/***************************************************************************/
static short int expr3(void)
{
	short int a,b;

	a = expr4();
	while(1)
	{
		if(*txtpos == '*')
		{
			txtpos++;
			b = expr4();
			a *= b;
		}
		else if(*txtpos == '/')
		{
			txtpos++;
			b = expr4();
			if(b != 0)
				a /= b;
			else
				expression_error = 1;
		}
		else
			return a;
	}
}

/***************************************************************************/
static short int expr2(void)
{
	short int a,b;

	if(*txtpos == '-' || *txtpos == '+')
		a = 0;
	else
		a = expr3();

	while(1)
	{
		if(*txtpos == '-')
		{
			txtpos++;
			b = expr3();
			a -= b;
		}
		else if(*txtpos == '+')
		{
			txtpos++;
			b = expr3();
			a += b;
		}
		else
			return a;
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
	unsigned char *start;
	unsigned char *newEnd;
	unsigned char linelen;
        union line_t newline;  //行の入力用

        union line_t *exec;  //計算用
        	
	variables_table = memory;  //変数領域
	program_start = program;   //プログラム先頭アドレス  
	program_end = program_start;


//プログラムエンドがどこなのか試しに出力
        printnum((int)program_end);
        Serial.print('\n');


	sp = memory+sizeof(memory);  // Needed for printnum
	printmsg(initmsg);
//	printnum(sp-program_end);
	printmsg(memorymsg);

warmstart:
	// this signifies that it is running in 'direct' mode.
	current_line = 0;
	sp = memory+sizeof(memory);  
	printmsg(okmsg);

prompt:
	while(!getln('>'))  //tmpbufに只今文章がそんまま入ってます
		line_terminator();
	toUppercaseBuffer();    //tmpbufを大文字にしました  

/*****入力はtmpBufに書き込まれている*******/
        txtpos = tmpBuf;  //入力文の先頭ポインタ
        newline.info.num = testnum();  //行番号ゲッチュ    
	ignore_blanks();  //空白読み飛ばし

/***Bufferにステート部にコピりながら行長を図る***/
	linelen = 0;
	while(txtpos[linelen] != NL){
                Buffer[linelen]=txtpos[linelen];
                linelen++;
        }       

        Buffer[linelen] = NL;  //文末をNULLにする
        
	linelen++; // Include the NL in the line length
	linelen += sizeof(unsigned short)+sizeof(char); // Add space for the line number and line length
        newline.info.len = linelen;
        
	if(newline.info.num == 0)
		goto direct;
	if(newline.info.num == 0xFFFF)
		goto badline;
	// Find the length of what is left, including the (yet-to-be-populated) line header
        insert_line(newline);

//プログラムエンドがどこなのか試しに出力
        printnum((int)program_end);
        Serial.print('\n');

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
           unsigned char tmp = *txtpos;
		   if(*txtpos != NL)
				*txtpos = '^';
           list_line = current_line;
           printline();
           *txtpos = tmp;
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
	while(*txtpos == ':')
		txtpos++;
	ignore_blanks();
	if(*txtpos == NL)
		goto execnextline;
	goto interperateAtTxtpos;

direct: 
	txtpos = Buffer;  //Bufferに文が入っている
	if(*txtpos == NL)  //何も書いてなかったら何もせず入力へ
		goto prompt;

interperateAtTxtpos:
        if(breakcheck())
        {
          printmsg(breakmsg);
          goto warmstart;
        }
        
	scantable(keywords);
	ignore_blanks();

	switch(table_index)
	{
		case KW_LIST:
			goto list;
		case KW_LOAD:
			goto unimplemented; /////////////////
		case KW_NEW:
			if(txtpos[0] != NL)
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
			if(expression_error || *txtpos == NL)
				goto invalidexpr;
			if(val != 0)
				goto interperateAtTxtpos;
			goto execnextline;
			}
		case KW_GOTO:
			expression_error = 0;
			linenum = expression();
			if(expression_error || *txtpos != NL)
				goto invalidexpr;
			current_line = findline(linenum);
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
			if(txtpos[0] != NL)
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
        exec = (union line_t *)current_line;
	current_line = current_line + exec->info.len;	 
execline:
  	if(current_line == program_end) // Out of lines to run
		goto warmstart;

        txtpos = current_line;  //現在の行から実行していく
        txtpos = txtpos + sizeof(short int)+sizeof(unsigned char);
	goto interperateAtTxtpos;

input:
	{
		unsigned char isneg=0;
		unsigned char *temptxtpos;
		short int *var;
		ignore_blanks();
		if(*txtpos < 'A' || *txtpos > 'Z')
			goto syntaxerror;
		var = ((short int *)variables_table)+*txtpos-'A';
		txtpos++;
		if(!check_statement_end())
			goto syntaxerror;
again:
		temptxtpos = txtpos;
		if(!getln('?'))
			goto warmstart;

		// Go to where the buffer is read
		txtpos = tmpBuf;
		if(*txtpos == '-')
		{
			isneg = 1;
			txtpos++;
		}

		*var = 0;
		do 	{
			*var = *var*10 + *txtpos - '0';
			txtpos++;
		} while(*txtpos >= '0' && *txtpos <= '9');
		ignore_blanks();
		if(*txtpos != NL)
		{
			printmsg(badinputmsg);
			goto again;
		}
	
		if(isneg)
			*var = -*var;

		goto run_next_statement;
	}
forloop:
	{
		unsigned char var;
		short int initial, step, terminal;

		if(*txtpos < 'A' || *txtpos > 'Z')
			goto syntaxerror;
		var = *txtpos;
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


		if(!expression_error && *txtpos == NL)
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
		current_line = findline(linenum);
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
		short int value;
		short int *var;

		if(*txtpos < 'A' || *txtpos > 'Z')
			goto syntaxerror;
		var = (short int *)variables_table + *txtpos - 'A';
		txtpos++;

		ignore_blanks();

		if (*txtpos != '=')
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
		*var = value;
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
                
                /*石井くん神↓*/
                *(short int*)(memory + address) = value;
                //printf("Poke %p value %i\n",address, (char)value);
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
	if(txtpos[0] != NL)
		goto syntaxerror;

	// Find the line
	list_line = findline(linenum);
	while(list_line != program_end)
          printline();
	goto warmstart;

print:
	// If we have an empty list then just put out a NL
	if(*txtpos == ':' )
	{
        line_terminator();
		txtpos++;
		goto run_next_statement;
	}
	if(*txtpos == NL)
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
		else if(*txtpos == '"' || *txtpos == '\'')
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
		if(*txtpos == ',')
			txtpos++;	// Skip the comma and move onto the next
		else if(txtpos[0] == ';' && (txtpos[1] == NL || txtpos[1] == ':'))
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
  	Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
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
