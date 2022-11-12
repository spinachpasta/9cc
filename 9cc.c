#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"


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
    case BO_Div:
    {
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    }
    case BO_Equal:
    {
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    }
    case BO_NotEqual:
    {
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    }
    case BO_Greater:
    {
      printf("  cmp rax, rdi\n");
      printf("  setg al\n");
      printf("  movzb rax, al\n");
      break;
    }
    case BO_GreaterEqual:
    {
      printf("  cmp rax, rdi\n");
      printf("  setge al\n");
      printf("  movzb rax, al\n");
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

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Expr *parseRelational(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  if (token_end == tokens)
  {
    fprintf(stderr, "No token found");
    exit(1);
  }
  Expr *result = parseAdditive(&tokens, token_end);

  for (; tokens < token_end;)
  {
    Token maybe_relational = *tokens;
    switch (maybe_relational.kind)
    {
    case TK_Greater:
    {
      tokens++;
      Expr *numberexp = parseAdditive(&tokens, token_end);
      result = binaryExpr(result, numberexp, BO_Greater);
      break;
    }
    case TK_GreaterEqual:
    {
      tokens++;
      Expr *numberexp = parseAdditive(&tokens, token_end);
      result = binaryExpr(result, numberexp, BO_GreaterEqual);
      break;
    }
    case TK_Less:
    {
      tokens++;
      Expr *numberexp = parseAdditive(&tokens, token_end);
      // swap children of operator node
      result = binaryExpr(numberexp, result, BO_Greater);
      break;
    }
    case TK_LessEqual:
    {
      tokens++;
      Expr *numberexp = parseAdditive(&tokens, token_end);
      // swap children of operator node
      result = binaryExpr(numberexp, result, BO_GreaterEqual);
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
// equality   = relational ("==" relational | "!=" relational)*
Expr *parseEquality(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  if (token_end == tokens)
  {
    fprintf(stderr, "No token found");
    exit(1);
  }
  Expr *result = parseRelational(&tokens, token_end);

  for (; tokens < token_end;)
  {
    Token maybe_relational = *tokens;
    switch (maybe_relational.kind)
    {
    case TK_Equal:
    {
      tokens++;
      Expr *numberexp = parseRelational(&tokens, token_end);
      result = binaryExpr(result, numberexp, BO_Equal);
      break;
    }
    case TK_NotEqual:
    {
      tokens++;
      Expr *numberexp = parseRelational(&tokens, token_end);
      result = binaryExpr(result, numberexp, BO_NotEqual);
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
    Token *maybe_leftparenthesis = maybe_number;
    if (maybe_leftparenthesis->kind == TK_LeftParenthesis)
    {
      *ptrptr += 1;
      Expr *expr = parseExpr(ptrptr, token_end);
      Token *maybe_rightparenthesis = *ptrptr;
      if (maybe_rightparenthesis->kind != TK_RightParenthesis)
      {
        fprintf(stderr, "Expected: right parenthesis. Token Kind:%d", maybe_rightparenthesis->kind);
        exit(1);
      }
      *ptrptr += 1;
      return expr;
    }
    fprintf(stderr, "Expected: number. Token Kind:%d", maybe_number->kind);
    exit(1);
  }
  *ptrptr += 1;
  return numberexpr(maybe_number->value);
}
// unary   = ("+" | "-")? primary
Expr *parseUnary(Token **ptrptr, Token *token_end)
{
  Token *maybe_unary = *ptrptr;
  if (maybe_unary >= token_end)
  {
    fprintf(stderr, "Expected: number, but got EOF");
    exit(1);
  }
  if (maybe_unary->kind == TK_Plus)
  {
    *ptrptr += 1;
    return parsePrimary(ptrptr, token_end);
  }
  if (maybe_unary->kind == TK_Minus)
  {
    *ptrptr += 1;
    return binaryExpr(numberexpr(0), parsePrimary(ptrptr, token_end), BO_Sub);
  }
  return parsePrimary(ptrptr, token_end);
}

// expr       = equality
Expr *parseExpr(Token **ptrptr, Token *token_end)
{
  return parseEquality(ptrptr, token_end);
}

// mul = unary ("*" unary | "/" unary)*
Expr *parseMultiplicative(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  if (token_end == tokens)
  {
    fprintf(stderr, "No token found");
    exit(1);
  }
  Expr *result = parseUnary(&tokens, token_end);

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
      Expr *numberexp = parseUnary(&tokens, token_end);
      result = binaryExpr(result, numberexp, BO_Mul);
      // ptr++;
      break;
    }
    case TK_Div:
    {
      tokens++;
      Expr *numberexp = parseUnary(&tokens, token_end);
      result = binaryExpr(result, numberexp, BO_Div);
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

// additive = mul ("+" mul | "-" mul)*
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
    case '(':
    {
      /* code */
      Token token = {TK_LeftParenthesis, 0};
      tokens[token_index] = token;
      token_index++;
      i++;
      break;
    }
    case ')':
    {
      /* code */
      Token token = {TK_RightParenthesis, 0};
      tokens[token_index] = token;
      token_index++;
      i++;
      break;
    }
    case '-':
    {
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
    case '/':
    {
      Token token = {TK_Div, 0};
      tokens[token_index] = token;
      token_index++;
      i++;
      break;
    }
    case '>':
    {
      i++;
      char c = str[i];
      if (c != '=')
      {
        Token token = {TK_Greater, 0};
        tokens[token_index] = token;
        token_index++;
        break;
      }
      i++;
      Token token = {TK_GreaterEqual, 0};
      tokens[token_index] = token;
      token_index++;
      break;
    }
    case '<':
    {
      i++;
      char c = str[i];
      if (c != '=')
      {
        Token token = {TK_Less, 0};
        tokens[token_index] = token;
        token_index++;
        break;
      }
      i++;
      Token token = {TK_LessEqual, 0};
      tokens[token_index] = token;
      token_index++;
      break;
    }

    //==
    case '=':
    {
      i++;
      char c = str[i];
      if (c != '=')
      {
        fprintf(stderr, "%s: unknown token =%c(%d)\n", __FUNCTION__, c, c);
        return -1;
      }
      i++;
      Token token = {TK_Equal, 0};
      tokens[token_index] = token;
      token_index++;
      break;
    }
    case '!':
    {
      i++;
      char c = str[i];
      if (c != '=')
      {
        fprintf(stderr, "%s: unknown token !%c(%d)\n", __FUNCTION__, c, c);
        return -1;
      }
      i++;
      Token token = {TK_NotEqual, 0};
      tokens[token_index] = token;
      token_index++;
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
