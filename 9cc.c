#include <stdio.h>
#include <stdlib.h>

enum BinaryOperation
{
  BO_Add,
  BO_Sub,
  BO_Mul
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
  TK_Mul
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

Token tokens[1000];

void EvaluateExprIntoRax(Expr *expr)
{
  switch (expr->expr_kind)
  {
  case EK_Number:
    printf("  mov rax, %d\n", expr->value);
    break;
  case EK_Operator:
    EvaluateExprIntoRax(expr->first_child);
    printf("    push rax\n");
    EvaluateExprIntoRax(expr->second_child);
    printf("    push rax\n");

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (expr->binary_op)
    {
    case BO_Add:
    {
      printf("    add rax,rdi\n");
      break;
    }
    case BO_Sub:
    {
      printf("    sub rax,rdi\n");
      break;
    }
    case BO_Mul:
    {
      printf("    imul rax,rdi\n");
      break;
    }
    default:
    {
      fprintf(stderr, "Invalid binaryop kind:%d", expr->binary_op);
      exit(1);
      break;
    }
    }
    break;

  default:
    fprintf(stderr, "Invalid expr kind:%d", expr->expr_kind);
    exit(1);
    break;
  }
}

Expr *numberexpr(int value)
{
  Expr *numberexp = calloc(1, sizeof(Expr));
  numberexp->value = value;
  numberexp->expr_kind = EK_Number;
  return numberexp;
}

Expr *binaryExpr(Expr *first_child, Expr *second_child, enum BinaryOperation binaryop)
{
  Expr *newexp = calloc(1, sizeof(Expr));
  newexp->first_child = first_child;
  newexp->expr_kind = EK_Operator;
  newexp->binary_op = binaryop;
  newexp->second_child = second_child;
  return newexp;
}

Expr *parsePrimary(Token **ptrptr, Token *token_end)
{
  Token *maybe_number = *ptrptr;
  if (maybe_number >= token_end)
  {
    fprintf(stderr, "Expected: number, but got EOF");
    exit(1);
  }
  if (maybe_number->kind != TK_Number)
  {
    fprintf(stderr, "Expected: number. Token Kind:%d", maybe_number->kind);
    exit(1);
  }
  *ptrptr += 1;
  return numberexpr(maybe_number->value);
}

Expr *parseExpr(Token **ptrptr, Token *token_end)
{
  return parseAdditive(ptrptr, token_end);
}
Expr *parseMultiplicative(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  if (token_end == tokens)
  {
    fprintf(stderr, "No token found");
    exit(1);
  }
  Expr *result = parsePrimary(&tokens, token_end);

  for (; tokens < token_end;)
  {
    Token maybe_operator = *tokens;
    switch (maybe_operator.kind)
    {
    case TK_Number:
    {
      fprintf(stderr, "Expected operator got Number");
      exit(1);
    }
    case TK_Mul:
    {
      tokens++;
      Expr *numberexp = parsePrimary(&tokens, token_end);
      result = binaryExpr(result, numberexp, BO_Mul);
      // ptr++;
      break;
    }
    default:
      *ptrptr = tokens;
      return result;
      break;
    }
  }
  *ptrptr = tokens;
  return result;
}

Expr *parseAdditive(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  if (token_end == tokens)
  {
    fprintf(stderr, "No token found");
    exit(1);
  }
  Expr *result = parseMultiplicative(&tokens, token_end);

  for (; tokens < token_end;)
  {
    Token maybe_operator = *tokens;
    switch (maybe_operator.kind)
    {
    case TK_Number:
    {
      fprintf(stderr, "Expected operator got Number");
      exit(1);
    }
    case TK_Minus:
    {
      tokens++;
      Expr *numberexp = parseMultiplicative(&tokens, token_end);
      result = binaryExpr(result, numberexp, BO_Sub);
      // ptr++;
      break;
    }
    case TK_Plus:
    {
      tokens++;
      Expr *numberexp = parseMultiplicative(&tokens, token_end);
      result = binaryExpr(result, numberexp, BO_Add);
      // ptr++;
      break;
    }
    default:
      *ptrptr = tokens;
      return result;
      break;
    }
  }
  *ptrptr = tokens;
  return result;
}

int tokenize(char *str)
{
  int token_index = 0;
  for (int i = 0; str[i];)
  {
    char c = str[i];
    switch (c)
    {
    case '+':
    {
      /* code */
      Token token = {TK_Plus, 0};
      tokens[token_index] = token;
      token_index++;
      i++;
      break;
    }
    case '-':
    {
      /* code */
      Token token = {TK_Minus, 0};
      tokens[token_index] = token;
      token_index++;
      i++;
      break;
    }
    case '*':
    {
      /* code */
      Token token = {TK_Mul, 0};
      tokens[token_index] = token;
      token_index++;
      i++;
      break;
    }
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
      int parsednum = parseInt(&str[i]);
      int parsedlength = intLength(&str[i]);
      i += parsedlength;
      Token token = {TK_Number, parsednum};
      tokens[token_index] = token;
      token_index++;
      break;
    }
    case ' ':
    {
      i++;
      break;
    }
    default:
    {
      fprintf(stderr, "%s: unknown character %c(%d)\n", __FUNCTION__, c, c);
      return -1;
    }
    }
  }
  return token_index;
}

int parseInt(char *str)
{
  int result = 0;
  while (1)
  {
    if (!isDigit(*str))
    {
      break;
    }
    else
    {
      int digit = *str - '0';
      result = result * 10 + digit;
    }
    str++;
  }
  return result;
}
int intLength(char *str)
{
  int length = 0;
  while (1)
  {
    if (!isDigit(*str))
    {
      break;
    }
    length++;
    str++;
  }
  return length;
}
int isDigit(char c)
{
  return '0' <= c && c <= '9';
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  char *p = argv[1];

  int token_length = tokenize(p);

  if (token_length == 0)
  {
    fprintf(stderr, "No token found");
    return 1;
  }

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  Token *ptr = tokens;
  Token *token_end = tokens + token_length;
  Expr *expr = parseExpr(&ptr, token_end);
  EvaluateExprIntoRax(expr);
  printf("  ret\n");
  return 0;
}
