

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
};

typedef struct Expr
{
  enum BinaryOperation binary_op;
  enum ExprKind expr_kind;
  int value;
  struct Expr *first_child;
  struct Expr *second_child;
  char* name;
} Expr;

enum StmtKind{
  SK_AndThen,
  SK_Expr
};

typedef struct Stmt
{
  enum StmtKind stmt_kind;
  
  struct Expr *expr;

  struct Stmt *first_child;
  struct Stmt *second_child;
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
  TK_Equal,
  TK_NotEqual,
  TK_Greater,
  TK_GreaterEqual,
  TK_Less,
  TK_LessEqual,
  TK_Semicolon,
  TK_Assign,
  TK_Identifier,
};

typedef struct Token
{
  enum TokenKind kind;
  int value;
  char* name;
} Token;

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
void Codegen(Stmt* stmt);

int tokenize(char *str);
LVar *findLVar(char *name);
LVar *insertLVar(char *name);
LVar *lastLVar();


void EvaluateExprIntoRax(Expr *expr);

extern Token tokens[1000];