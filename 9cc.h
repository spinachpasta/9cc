

enum BinaryOperation{
  BO_Add,
  BO_Sub,
  BO_Mul,
  BO_Div,
  BO_Equal,
  BO_Greater,
  BO_GreaterEqual,
  BO_NotEqual,
  BO_AndThen
};

enum ExprKind
{
  EK_Number,
  EK_Operator
};

typedef struct Expr
{
  enum BinaryOperation binary_op;
  enum ExprKind expr_kind;
  int value;
  struct Expr *first_child;
  struct Expr *second_child;
} Expr;

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
};

typedef struct Token
{
  enum TokenKind kind;
  int value;
} Token;

// prototype declaration
int isDigit(char c);
int intLength(char *str);
int parseInt(char *str);
Expr *parseMultiplicative(Token **ptrptr, Token *token_end);
Expr *parseAdditive(Token **ptrptr, Token *token_end);
Expr *parseExpr(Token **ptrptr, Token *token_end);
Expr *parseUnary(Token **ptrptr, Token *token_end);
Expr *parseProgram(Token **ptrptr, Token *token_end);
int tokenize(char *str);

extern Token tokens[1000];