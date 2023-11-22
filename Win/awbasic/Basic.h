
/* expression.cpp */
int EvalExpression(const unsigned char* expr, unsigned int* len, struct BasicCode*, struct TypedValue* vr);
int DoEvalExpression(const unsigned char** expr, int call_nest, struct BasicCode*, struct TypedValue* result);
int GetOperator(const unsigned char** expr, int* level, int peek_mode, unsigned char *);
int GetValue(const unsigned char** expr, struct BasicCode*, struct TypedValue* result);
int DoOperator(struct TypedValue* x, struct TypedValue* y, unsigned char op, struct TypedValue* result);
void SkipWhiteSpace(const unsigned char** expr);
int EvalParen(const unsigned char** expr, struct BasicCode*, struct TypedValue*);
int ParseStringLiteral(const unsigned char* expr, struct TypedValue* v, unsigned int* len);
int DoStringOperator(struct TypedValue* x, struct TypedValue* y, unsigned char op, struct TypedValue* result);
int DoFloatOperator(struct TypedValue* x, struct TypedValue* y, unsigned char op, struct TypedValue* result);
int DoIntOperator(struct TypedValue* x, struct TypedValue* y, unsigned char op, struct TypedValue* result);
int DoDoubleOperator(struct TypedValue* x, struct TypedValue* y, unsigned char op, struct TypedValue* result);
int ParseNumberFull(const unsigned char* expr, struct TypedValue* v, unsigned int* len, bool force_double);
int ParseNumber(const unsigned char* expr, struct TypedValue* v, unsigned int* len, bool force_double);
bool uisdigit(unsigned char c);

/* interpret.cpp */
int Interpret(struct BasicCode *code);
bool IsEosToken(unsigned char token);
int ExecuteStatement(struct BasicCode *code);
int DoRestore(struct BasicCode *code);

/* basic main.cpp */
struct BasicCode* BasicCreateProcess(struct IoConsole* primary_con, IoConsole* debug_con);
void BasicSetDefaultState(struct BasicCode* p);
int BasicRun(struct BasicCode* code);
bool BasicLoadFile(struct BasicCode* process, char* filespec);
bool LoadAlreadyTokenized(unsigned char* source, int size, struct BasicCode* code);
bool BasicSaveFile(struct BasicCode* code, char* filespec);
void BasicClear(struct BasicCode* p);
void BasicDestroyProcess(struct BasicCode* p);

/* variables.cpp */
void SetVariable(union VariableValue* storage, union VariableValue newvalue, unsigned char type);
unsigned int CountArrayDims(struct BasicCode* code, const unsigned char* expr);
int CalcIndexValues(struct BasicCode*, const unsigned char* expr, unsigned int* numdims, unsigned int** indexvals, unsigned int* len);
int SimpleParseVariable(unsigned char deftab[], const unsigned char* expr, unsigned char* type, unsigned short* label, unsigned int* len);
int ParseVariable(struct BasicCode*, const unsigned char* expr, union VariableValue** value_ptr, unsigned char* type, unsigned int* len);
int CreateArray(struct Variable vartab[], unsigned short label, unsigned char type, unsigned int* ap);
short IntVal(struct TypedValue* tv);
float FloatVal(struct TypedValue* tv);
double DoubleVal(struct TypedValue* tv);
bool IsNumber(struct TypedValue* tv);
bool IsNumberT(unsigned char type);
void CopyVariableValue(VariableValue* d, VariableValue* s);
void ConvertType(struct TypedValue* v, unsigned char newtype);
bool IsInt(struct TypedValue* tv);

/* stack.cpp */
bool PushStack(struct Stack* stack, struct StackFrame* frame);
bool PopStack(struct Stack* stack, StackFrame** rp);
bool PeekStack(struct Stack* stack, StackFrame** rp);
bool SearchForStack(struct Stack* stack, VariableValue* var, unsigned int* frame_index);
void UnwindStack(struct Stack* stack, unsigned int frame_index);

/* functions.cpp */
int ExecuteBuiltinFunction(struct BasicCode*, const unsigned char* expr, struct TypedValue* result, unsigned int* len);
bool IsBuiltinFunction(const unsigned char token);

/* WinMain.cpp */
void WinTrsDelay(void);
bool WinGetFileName(char* fname, int size_fname, int mode);
struct IoConsole* IoWinWinOpen();
bool WinLprintMessage();

/* WinTrsKeyboard.cpp */
char WinInkey(IoConsole*);
unsigned char WinTrsPeekKeyboard(IoConsole*, unsigned short Addr);
void WinOnKeyDown(unsigned int vk, unsigned int nRepCnt, unsigned int nFlags);
void WinOnChar(unsigned int c, unsigned int nRepCnt, unsigned int nFlags);
void WinOnKeyUp(unsigned int vk, unsigned int nRepCnt, unsigned int nFlags);

/* debug.cpp */
void TraceOn(struct BasicCode* code);
void DebugShowTrace(struct BasicCode* code);
void DebugTimeSlice(struct BasicCode* code);
struct IoConsole* DebugAttachConsole(struct BasicCode* bc, struct IoConsole*);
char* LookupErrorString(unsigned char e);
void DebugErrorDump(struct BasicCode* code, struct IoConsole* io, int err);
void DebugShowUnknownTokens(struct BasicCode* code, struct IoConsole* io);
void DebugShowList(struct BasicCode* code, struct IoConsole* io);
bool DebugIsToken(unsigned char);

/* iowincon.cpp */
void IoWinConPutstr(char *);
void IoWinConPutc(char c);
struct IoConsole* IoWinConOpen(void);

/* io.cpp */
void io_putc(struct IoConsole* io, char c);
void io_putstr(struct IoConsole* io, char* s);
char io_inkey(struct IoConsole* io);
int io_chk_brk(struct IoConsole* io);
void io_close(struct IoConsole* io);
int io_printf1i(struct IoConsole *io, const char *format, int arg1);
void io_printusingnum(struct IoConsole *io, char* string, double value);
int io_printf2i(struct IoConsole *io, const char *format,  int arg1, int arg2);
char io_getc(struct IoConsole* io);
void io_cls(struct IoConsole* io);
void io_set_c(struct IoConsole* io, unsigned short loc, unsigned char c);
unsigned char io_peek(struct IoConsole* io, short loc);
bool io_setreset(struct IoConsole* io, short x, short y, bool isset);
short io_point(struct IoConsole* io, short x, short y);
void io_set_cursor(struct IoConsole* io, short loc);
short io_get_cursor(struct IoConsole* io);
char *io_gets(struct IoConsole* io, char *buffer );
void io_draw_range(struct IoConsole* io, int x, int y);

/* tokens.cpp */
void TokenizeLine(unsigned char* dest, char* src, int *destlen, int *sourcelen);
bool Tokenize(char* source, struct BasicCode* code);
char* TokenToAscii(unsigned char findtoken);

/* iofile.cpp */
struct IoConsole* IoFileWriteOpen(char* filespec);
struct IoConsole* IoNullOpen(void);


#define TRUE 1
#define FALSE 0

#define MAX_VARIABLES 128
#define MAX_IMAGESIZE 128*1024
#define MAX_STACKSIZE 64

struct LineNumber {
	unsigned char* position;
	unsigned long value;
};

#define VT_INT		1
#define VT_FLOAT	2
#define VT_DOUBLE	4
#define VT_STRING	8
#define	VT_ARRAY	16

union VariableValue {
		short intval;
		char* string;
		unsigned int* array;
		float floatval;
		double doubleval;
};

struct TypedValue {
	unsigned char type_flags;
	union VariableValue value;
};

struct Variable {
	struct TypedValue value;
	unsigned short label;
};

struct ForFrame {
	VariableValue* variable;
	VariableValue to_val;
	VariableValue step_val;
	unsigned char* start_pc;
	unsigned char type_flags;
	unsigned char* current_line_start_pc;
	unsigned char* current_statement_start_pc;
};

struct GosubFrame {
	unsigned char* return_pc;
	unsigned char* current_line_start_pc;
	unsigned char* current_statement_start_pc;
};

struct StackFrame {
	unsigned char type;		// token of stack type:  TOK_FOR, TOK_GOSUB
	union {
		struct ForFrame for_frame;
		struct GosubFrame gosub_frame;
	};
};
	
struct Stack {
	struct StackFrame entrys[MAX_STACKSIZE];
	short sp;
};

/* 
 * The IOConsole can support windows of arbitrary size, as well as
 * un windowed consoles (like RS232 or Line Printer).  For a streaming
 * device like RS232, set io_width & io_height to zero.  
 */

struct IoConsole {
	/* TTY driver functions  */
	char (*io_inkey)(IoConsole*);
	void (*io_putc)(IoConsole*, char);
	char (*io_getc)(IoConsole*);
	int (*io_chk_brk)(IoConsole*);
	void (*io_close)(IoConsole*);
	unsigned short io_width;	// in cells
	unsigned short io_height;

	/* In TTY mode: cursor is the char position on the current line */
	/* In windowed mode: cursor is the cursor position as a count of chars from start of window */
	/* (the position where the next putc is going to go)*/
	unsigned short cursor; 

	/* Windowed driver functions */
	void (*io_draw_range)(IoConsole*, unsigned char*, int, int);
	unsigned char (*io_peek_keyboard)(IoConsole*, unsigned short);
	
	/* Windowed values (required when width & height are non-zero */

	bool cursor_visable;
	unsigned char* video_mem;
};

#define RM_IDLE			0
#define RM_RUNNING		1
#define RM_SINGLESTEP	2

#define TRACE_NONE		0
#define TRACE_LINE		1
#define TRACE_STMT		2

struct BasicCode {
	unsigned char		image[MAX_IMAGESIZE];
	struct Variable		variables[MAX_VARIABLES];
	struct Stack		stack;
	unsigned char*		pc;	
	unsigned char*		current_line_start_pc;		// line number currently executing
	unsigned char*		current_statement_start_pc;	// Statement currently executing
	unsigned char		deftab[26];					// Defaults for DEFXXX A-Z
	unsigned char*		readptr;					// READ / DATA. 
	unsigned char*		err_stmt;					// current_statement_start_pc snapshot on error
	unsigned short		onerror;					// ON ERROR GOTO onerror
	unsigned short		last_erl;					// ON ERROR:  ERL
	int					last_err;					// ON ERROR: ERR
	int					tron;						// TRACE ON?
	unsigned int		last_tron_line;
	IoConsole*			debug_con;					// If non NULL, points to open debug console
	IoConsole*			std_con;					// This code's default IO console
	IoConsole*			print_con;					// if non zero, an LPRINT console is open
	unsigned char		run_mode;
};


/*
 * Microsoft TRS80 L2 Basic Tokens
 */

#define	TOK_END		0x80
#define TOK_LET 	0x8C
#define TOK_IF		0x8F
#define TOK_ELSE	0x95
#define TOK_THEN	0xCA
#define TOK_PRINT	0xB2
#define TOK_NOT		0xCB
#define TOK_AND		0xD2
#define TOK_OR		0xD3
#define TOK_GT		0xD4
#define TOK_LT		0xD6
#define TOK_EQ		0xD5
#define TOK_MUL		0xCF		
#define TOK_DIV		0xD0
#define TOK_ADD		0xCD
#define TOK_SUB		0xCE
#define TOK_EXP		0xD1
#define TOK_REM		0x93
#define TOK_APOS	0xFB
#define TOK_GOTO	0x8D	
#define TOK_FOR		0x81
#define TOK_TO		0xBD
#define TOK_STEP	0xCC
#define TOK_NEXT	0x87
#define TOK_GOSUB	0x91
#define TOK_RETURN	0x92
#define TOK_DIM		0x8A
#define TOK_INPUT	0x89
#define TOK_CLEAR	0xb8
#define TOK_CLS		0x84
#define TOK_DEFSTR	0x98
#define TOK_DEFINT	0x99
#define TOK_DEFSGN	0x9A
#define TOK_DEFDBL	0x9B
#define TOK_DATA	0x88
#define TOK_READ	0x8B
#define TOK_RESTORE	0x90
#define TOK_RANDOM	0x86
#define TOK_ON		0xA1
#define TOK_ERROR	0x9E
#define TOK_RESUME	0x9F
#define TOK_POKE	0xB1
#define TOK_SET		0x83
#define TOK_RESET	0x82
#define TOK_STOP	0x94
#define TOK_RUN		0x8E
#define TOK_LPRINT	0xAF
#define TOK_OUT		0xA0
#define TOK_LINE	0x9C


/*
 * Bultin Function Tokens
 */
#define TOK_ASC		0xF6
#define TOK_CHR		0xF7
#define TOK_FRE		0xDA	
#define TOK_INKEY	0xC9
#define TOK_LEN		0xF3
#define TOK_LEFT	0xF8
#define TOK_MID		0xFA	// Disk basic?
#define TOK_RIGHT	0xF9	
#define TOK_STR		0xF4
#define TOK_STRING	0xC4
#define TOK_VAL		0xF5
#define TOK_INSTR	0xC5


#define TOK_ABS		0xD9
#define TOK_ATN		0xE4
#define TOK_CDBL	0xF1
#define TOK_CINT	0xEF
#define TOK_COS		0xE1
#define TOK_CSNG	0xF0
#define TOK_FEXP	0xE0
#define TOK_FIX		0xF2
#define TOK_INT		0xD8
#define TOK_LOG		0xDF
#define TOK_RND		0xDE
#define TOK_SGN		0xD7
#define TOK_SIN		0xE2
#define TOK_SQR		0xDD
#define TOK_TAN		0xE3

#define TOK_ERR		0xC3
#define TOK_ERL		0xC2
#define TOK_MEM		0xC8
#define TOK_TAB		0xBC
#define TOK_USING	0xBF
#define TOK_PEEK	0xE5
#define TOK_POINT	0xC6
#define TOK_POS		0xDC


/*
 * This Interpreters internal operators codes that must not overlap with Tokens 
 */

#define OPR_LE		1
#define OPR_NE		2
#define OPR_GE		3

/*
 * Error Codes - Specific to this interpreter implementation
 */

#define ERR_INTERNAL		0xFF		// A condition that shouldn't occur did
#define ERR_OKAY			0xFE		// normal return
#define ERR_UNDEFINED_TOKEN	0xFD		// A token that we don't handle
#define ERR_NORMAL_END		0xFC		// normal, but END encountered.  Terminate execution
#define ERR_UNDEFINED_OP	0xFB		// An expression operator that we don't handle
#define ERR_MISSING_PARN	0xFA
#define ERR_NORMAL_EOL		0xF9		// end of line (no more statments on this line)
#define ERR_NORMAL_CONT		0xF8		// keep executing on next token
#define ERR_STACK_UNDER		0xF7		// noting on stack to pop
#define ERR_BREAK			0xF6		// a break key pressed while running
#define ERR_WINDOWCLOSED	0xF5		// User closed the window we are running in

/*
 * Error Codes - Match Microsoft TRS80 L2 Basic
 */

#define ERR_SYNTAX			0x02
#define ERR_DIV_ZERO		0x14
#define ERR_MISSING_LN		0x0E
#define ERR_OUTOFMEM		0x0C
#define ERR_STRINGTOLONG	0x1C
#define ERR_TM				0x18	// Type Mismatch (string / numeric operation mismatch)
#define ERR_OS				0x1A	// out of string space
#define ERR_RG				0x04	// Return without Gosub
#define ERR_NF				0x00	// Next without For
#define ERR_FC				0x08	// Invalid parameter passed to function/array (e.g neg matrix dim or squr root)
#define ERR_DD				0x12	// Attempted to redimension an array
#define ERR_BS				0x10	// Array subscript out of bounds
#define ERR_OD				0x06	// Out of Data (READ)

