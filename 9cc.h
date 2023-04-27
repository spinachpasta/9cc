

enum Operation
{
  BO_Add,
  BO_Sub,
  BO_Mul,
  BO_Div,
  BO_Equal,
  BO_Greater,
  BO_GreaterEqual,
  BO_NotEqual,
  BO_Assign,
  UO_AddressOf,
  UO_Deref,
  UO_SizeOf,
};

enum ExprKind
{
  EK_Number,
  EK_BinaryOperator,
  EK_UnaryOperator,
  EK_Identifier,
  EK_CallFunction,
};
enum TypeKind
{
  TYPE_INT,
  TYPE_PTR,
  TYPE_ARRAY,
};
typedef struct Type
{
  enum TypeKind kind;
  struct Type *ptr_to;
  int array_length;
} Type;

typedef struct Expr
{
  enum Operation op;
  enum ExprKind expr_kind;
  int value;
  struct Expr *first_child;
  struct Expr *second_child;
  struct Expr *argments[6];
  char *name;
} Expr;

enum StmtKind
{
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
struct LVar
{
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
  Type *type;
};

typedef struct FunctionDeclaration
{
  struct FunctionDeclaration *next;
  char *name;
  Type *return_type;
} FunctionDeclaration;


typedef struct TypeAndIdent
{
  Type *type;
  char *identifier;
} TypeAndIdent;

extern LVar *locals;
extern FunctionDeclaration *function_declarations;

enum TokenKind
{
  TK_Number,
  TK_Minus,
  TK_Plus,
  TK_Mul,
  TK_Div,
  TK_And,
  TK_LeftParenthesis,
  TK_RightParenthesis,
  TK_LeftCurlyBrace,
  TK_RightCurlyBrace,
  TK_LeftSquareBracket,
  TK_RightSquareBracket,
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
  TK_Int,
  TK_SizeOf,
};

typedef struct Token
{
  enum TokenKind kind;
  int value;
  char *identifier_name;
} Token;

typedef struct Function
{
  /* data */
  Stmt *content;
  char *name;
  int parameter_length;
  char *parameter_names[6];
} Function;

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
Stmt *parseFor(Token **ptrptr, Token *token_end);
Stmt *parseStmt(Token **ptrptr, Token *token_end);
void consumeOrDie(Token **ptrptr, Token *token_end, enum TokenKind kind);
Function *parseFunction(Token **ptrptr, Token *token_end);
void CodegenFunction(Function *func);
TypeAndIdent *parseTypeAndIdentifier(Token **ptrptr, Token *token_end);

void Codegen(Stmt *stmt);

int tokenize(char *str);
LVar *findLVar(char *name);
LVar *insertLVar(TypeAndIdent *typeandident);
LVar *insertLArray(TypeAndIdent *typeandident);

LVar *lastLVar();
int is_alnum(char c);

Type* EvaluateType(Expr *expr);

void EvaluateExprIntoRax(Expr *expr);

FunctionDeclaration *findFunction(char *name);
FunctionDeclaration *insertFunction(TypeAndIdent *typeandident);
FunctionDeclaration *lastFunction();
int typeEqual(Type *t1, Type *t2);

int getSize(Type *type);

int round_up(int x, int y);

int isPointerOrArray(Type *t);

extern Token tokens[1000];