#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

Expr *numberexpr(int value)
{
  Expr *numberexp = calloc(1, sizeof(Expr));
  numberexp->value = value;
  numberexp->expr_kind = EK_Number;
  return numberexp;
}

Expr *identifierexpr(char value)
{
  Expr *numberexp = calloc(1, sizeof(Expr));
  numberexp->value = value;
  numberexp->expr_kind = EK_Identifier;
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
// primary    = num | ident | "(" expr ")"
Expr *parsePrimary(Token **ptrptr, Token *token_end)
{
  Token *maybe_number = *ptrptr;
  if (maybe_number >= token_end)
  {
    fprintf(stderr, "Expected: number, but got EOF");
    exit(1);
  }
  if (maybe_number->kind == TK_Number)
  {
    *ptrptr += 1;
    return numberexpr(maybe_number->value);
  }
  else if (maybe_number->kind == TK_Identifier)
  {
    *ptrptr += 1;
    return identifierexpr(maybe_number->value);
  }
  else
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
  return parseAssign(ptrptr, token_end);
}
Expr *parseAssign(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  if (token_end == tokens)
  {
    fprintf(stderr, "No token found");
    exit(1);
  }
  Expr *result = parseEquality(&tokens, token_end);
  if (tokens->kind == TK_Assign)
  {
    tokens++;
    Expr *newresult = parseAssign(&tokens, token_end);
    *ptrptr = tokens;
    return binaryExpr(result, newresult, BO_Assign);
  }
  *ptrptr = tokens;
  return result;
}
// program = expr (";" expr)*
Expr *parseProgram(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  if (token_end == tokens)
  {
    fprintf(stderr, "No token found");
    exit(1);
  }
  Expr *result = parseExpr(&tokens, token_end);

  for (; tokens < token_end;)
  {
    Token maybe_operator = *tokens;
    switch (maybe_operator.kind)
    {
    case TK_Semicolon:
    {
      tokens++;
      Expr *numberexp = parseExpr(&tokens, token_end);
      result = binaryExpr(result, numberexp, BO_AndThen);
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
