

enum BinaryOperation{
  BO_Add,
  BO_Sub,
  BO_Mul,
  BO_Div,
  BO_Equal,
  BO_Greater,
  BO_GreaterEqual,
  BO_NotEqual,
  BO_AndThen,
  BO_Assign
};

enum ExprKind
{
  EK_Number,
  EK_Operator,
  EK_Identifier,
  EK_CallFunction,
};

typedef struct Expr
{
  enum BinaryOperation binary_op;
  enum ExprKind expr_kind;
  int value;
  struct Expr *first_child;
  struct Expr *second_child;
  struct Expr *argments[6];
  char* name;
} Expr;

enum StmtKind{
  SK_AndThen,
  SK_Expr,
  SK_Return,
  SK_If,
  SK_While,
  SK_For,
  SK_Block,
};

typedef struct Stmt
{
  enum StmtKind stmt_kind;
  
  struct Expr *expr;
  struct Expr *expr1;
  struct Expr *expr2;

  struct Stmt *first_child;
  struct Stmt *second_child;

  struct Stmt *third_child;
} Stmt;

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
  LVar *next; // 次の変数かNULL
  char* name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
};

extern LVar *locals;



enum TokenKind
{
  TK_Number,
  TK_Minus,
  TK_Plus,
  TK_Mul,
  TK_Div,
  TK_LeftParenthesis,
  TK_RightParenthesis,
  TK_LeftCurlyBrace,
  TK_RightCurlyBrace,
  TK_Equal,
  TK_NotEqual,
  TK_Greater,
  TK_GreaterEqual,
  TK_Less,
  TK_LessEqual,
  TK_Semicolon,
  TK_Assign,
  TK_Identifier,
  TK_Return,
  TK_IF,
  TK_WHILE,
  TK_ELSE,
  TK_FOR,
  TK_Comma,
  TK_DefineFunction,
};

typedef struct Token
{
  enum TokenKind kind;
  int value;
  char* identifier_name;
} Token;

typedef struct Function
{
  /* data */
  Stmt* content;
  char* name;
}Function;


// prototype declaration
int isDigit(char c);
int intLength(char *str);
int parseInt(char *str);
Expr *parseMultiplicative(Token **ptrptr, Token *token_end);
Expr *parseAdditive(Token **ptrptr, Token *token_end);
Expr *parseExpr(Token **ptrptr, Token *token_end);
Expr *parseUnary(Token **ptrptr, Token *token_end);
Stmt *parseProgram(Token **ptrptr, Token *token_end);
Expr *parseAssign(Token **ptrptr, Token *token_end);
Stmt *parseFor(Token **ptrptr,Token *token_end);
Stmt *parseStmt(Token **ptrptr, Token *token_end);
void consumeOrDie(Token **ptrptr, Token *token_end, enum TokenKind kind);
Function *parseFunction(Token **ptrptr, Token *token_end);
void CodegenFunction(Function* func);

void Codegen(Stmt* stmt);

int tokenize(char *str);
LVar *findLVar(char *name);
LVar *insertLVar(char *name);
LVar *lastLVar();
int is_alnum(char c);


void EvaluateExprIntoRax(Expr *expr);

extern Token tokens[1000];