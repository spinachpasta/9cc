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

Expr *callfunctionexpr(char *name, Expr **arguments)
{
  Expr *expr = calloc(1, sizeof(Expr));
  expr->name = name;
  expr->expr_kind = EK_CallFunction;
  for (int i = 0; i < 6; i++)
  {
    expr->argments[i] = arguments[i];
  }
  return expr;
}

Expr *identifierexpr(char *name)
{
  Expr *numberexp = calloc(1, sizeof(Expr));
  numberexp->name = name;
  numberexp->expr_kind = EK_Identifier;
  return numberexp;
}

Expr *unaryExpr(Expr *child, enum Operation unaryop)
{
  Expr *newexp = calloc(1, sizeof(Expr));
  newexp->first_child = child;
  newexp->expr_kind = EK_UnaryOperator;
  newexp->op = unaryop;
  return newexp;
}

Expr *binaryExpr(Expr *first_child, Expr *second_child, enum Operation binaryop)
{
  Expr *newexp = calloc(1, sizeof(Expr));
  newexp->first_child = first_child;
  newexp->expr_kind = EK_BinaryOperator;
  newexp->op = binaryop;
  newexp->second_child = second_child;
  return newexp;
}

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
  if (maybe_number->kind == TK_Number)
  {
    *ptrptr += 1;
    return numberexpr(maybe_number->value);
  }
  else if (maybe_number->kind == TK_Identifier)
  {
    *ptrptr += 1;
    Token *maybe_leftparenthesis = *ptrptr;
    if (maybe_leftparenthesis->kind == TK_LeftParenthesis)
    {
      *ptrptr += 1;
      Token *maybe_rightparenthesis = *ptrptr;
      Expr *arguments[] = {NULL, NULL, NULL, NULL, NULL, NULL};
      if (maybe_rightparenthesis->kind != TK_RightParenthesis)
      {
        arguments[0] = parseExpr(ptrptr, token_end);
        for (int i = 1; i < 6; i++)
        {
          Token *maybe_comma = *ptrptr;
          if (maybe_comma->kind == TK_Comma)
          {
            *ptrptr += 1;
            arguments[i] = parseExpr(ptrptr, token_end);
          }
          else
          {
            break;
          }
        }
        maybe_rightparenthesis = *ptrptr;
        if (maybe_rightparenthesis->kind != TK_RightParenthesis)
        {
          fprintf(stderr, "Expected: right parenthesis. Token Kind:%d", maybe_rightparenthesis->kind);
          exit(1);
        }
      }
      *ptrptr += 1;
      return callfunctionexpr(maybe_number->identifier_name, arguments);
    }
    else
    {
      return identifierexpr(maybe_number->identifier_name);
    }
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
  if (maybe_unary->kind == TK_Mul)
  {
    *ptrptr += 1;
    return unaryExpr(parsePrimary(ptrptr, token_end), UO_Deref);
  }
  if (maybe_unary->kind == TK_And)
  {
    *ptrptr += 1;
    return unaryExpr(parsePrimary(ptrptr, token_end), UO_AddressOf);
  }
  return parsePrimary(ptrptr, token_end);
}

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

Expr *parseOptionalExprAndToken(Token **ptrptr, Token *token_end, enum TokenKind target)
{
  Token *tokens = *ptrptr;
  if (tokens->kind == target)
  {
    tokens++;
    *ptrptr = tokens;
    return NULL;
  }
  Expr *expr = parseExpr(&tokens, token_end);
  if (tokens->kind == target)
  {
    tokens++;
    *ptrptr = tokens;
    return expr;
  }
  fprintf(stderr, "expected TokenKind#%d after optional expression but did not find one", target);
  exit(-1);
}

Stmt *parseFor(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  tokens++;

  if (tokens->kind == TK_LeftParenthesis)
  {
    tokens++;
  }
  else
  {
    fprintf(stderr, "expected left parenthesis got %d\n", tokens->kind);
    exit(1);
  }
  Expr *exprs[3] = {NULL, NULL, NULL};
  exprs[0] = parseOptionalExprAndToken(&tokens, token_end, TK_Semicolon);
  exprs[1] = parseOptionalExprAndToken(&tokens, token_end, TK_Semicolon);
  exprs[2] = parseOptionalExprAndToken(&tokens, token_end, TK_RightParenthesis);

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->stmt_kind = SK_For;
  stmt->first_child = NULL;
  stmt->second_child = NULL;
  stmt->third_child = NULL;
  stmt->expr = exprs[0];
  stmt->expr1 = exprs[1];
  stmt->expr2 = exprs[2];

  Stmt *statement = parseStmt(&tokens, token_end);
  stmt->second_child = statement;

  *ptrptr = tokens;

  return stmt;
}

Stmt *parseStmt(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  if (token_end == tokens)
  {
    fprintf(stderr, "No token found");
    exit(1);
  }
  int is_return = 0;
  int is_if = 0;
  int is_while = 0;

  if (tokens->kind == TK_Int)
  {
    TypeAndIdent *typeident_local = parseTypeAndIdentifier(&tokens, token_end);
    if (!findLVar(typeident_local->identifier))
    {
      insertLVar(typeident_local->identifier);
    }
    consumeOrDie(&tokens, token_end, TK_Semicolon);
    // Stmt *result = parseStmt(&tokens, token_end);
    // *ptrptr = tokens;
    // return result;
    Stmt *stmt = malloc(sizeof(Stmt));
    stmt->stmt_kind = SK_Expr;
    stmt->first_child = NULL;
    stmt->second_child = NULL;
    stmt->third_child = NULL;
    stmt->expr = numberexpr(42);
    *ptrptr = tokens;
    return stmt;
  }

  if (tokens->kind == TK_LeftCurlyBrace)
  {
    tokens++;

    Stmt *result = parseStmt(&tokens, token_end);
    for (;;)
    {
      if (tokens->kind == TK_RightCurlyBrace)
      {
        tokens++;
        break;
      }
      Stmt *stmt = parseStmt(&tokens, token_end);
      Stmt *andthen = calloc(1, sizeof(Stmt));
      andthen->first_child = result;
      andthen->stmt_kind = SK_AndThen;
      andthen->second_child = stmt;
      result = andthen;
    }
    *ptrptr = tokens;
    return result;
  }

  if (tokens->kind == TK_Return)
  {
    tokens++;
    is_return = 1;
  }
  if (tokens->kind == TK_IF)
  {
    tokens++;
    is_if = 1;
    if (tokens->kind == TK_LeftParenthesis)
    {
      tokens++;
    }
    else
    {
      fprintf(stderr, "expected right parenthesis got %d\n", tokens->kind);
      exit(1);
    }
  }
  if (tokens->kind == TK_WHILE)
  {
    tokens++;
    is_while = 1;
    if (tokens->kind == TK_LeftParenthesis)
    {
      tokens++;
    }
    else
    {
      fprintf(stderr, "expected right parenthesis got %d\n", tokens->kind);
      exit(1);
    }
  }
  if (tokens->kind == TK_FOR)
  {
    Stmt *stmt = parseFor(&tokens, token_end);
    *ptrptr = tokens;
    return stmt;
  }
  Expr *expr = parseExpr(&tokens, token_end);
  {
    Token maybe_operator = *tokens;
    if (is_if || is_while)
    {
      if (maybe_operator.kind == TK_RightParenthesis)
      {
        tokens++;
      }
      else
      {
        fprintf(stderr, "expected right parenthesis got %d\n", maybe_operator.kind);
        exit(1);
      }
    }
    else
    {
      if (maybe_operator.kind == TK_Semicolon)
      {
        tokens++;
      }
      else
      {
        fprintf(stderr, "no semicolon after expr. kind=%d\n", maybe_operator.kind);
        exit(1);
      }
    }
  }
  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->stmt_kind = SK_Expr;
  stmt->first_child = NULL;
  stmt->second_child = NULL;
  stmt->third_child = NULL;
  stmt->expr = expr;
  if (is_if)
  {
    stmt->stmt_kind = SK_If;
    {
      Stmt *statement = parseStmt(&tokens, token_end);
      stmt->second_child = statement;
    }
    Token maybe_else = *(tokens);
    if (maybe_else.kind == TK_ELSE)
    {
      tokens++;
      Stmt *statement1 = parseStmt(&tokens, token_end);
      stmt->third_child = statement1;
    }
  }
  if (is_while)
  {
    stmt->stmt_kind = SK_While;
    Stmt *statement = parseStmt(&tokens, token_end);
    stmt->second_child = statement;
  }
  if (is_return)
  {
    stmt->stmt_kind = SK_Return;
  }
  *ptrptr = tokens;
  return stmt;
}
Stmt *parseFunctionContent(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  if (token_end == tokens)
  {
    fprintf(stderr, "No token found");
    exit(1);
  }
  consumeOrDie(&tokens, token_end, TK_LeftCurlyBrace);
  Stmt *result = parseStmt(&tokens, token_end);

  for (; tokens < token_end;)
  {
    if (tokens->kind == TK_RightCurlyBrace)
    {
      tokens += 1;
      break;
    }
    Stmt *statement = parseStmt(&tokens, token_end);
    Stmt *andthen = calloc(1, sizeof(Stmt));
    andthen->first_child = result;
    andthen->stmt_kind = SK_AndThen;
    andthen->second_child = statement;
    result = andthen;
  }
  *ptrptr = tokens;
  return result;
}

TypeAndIdent *parseTypeAndIdentifier(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  TypeAndIdent *typeandident = calloc(1, sizeof(TypeAndIdent));
  Type *previoustype = calloc(1, sizeof(Type));
  previoustype->kind = TYPE_INT;
  consumeOrDie(&tokens, token_end, TK_Int);

  for (int i = 0;; i++)
  {
    if (tokens->kind == TK_Mul)
    {
      {
        Type *type = calloc(1, sizeof(Type));
        type->kind = TYPE_PTR;
        type->ptr_to = previoustype;
        previoustype = type;
      }
      tokens++;
    }
    else
    {
      break;
    }
  }
  if (tokens->kind == TK_Identifier)
  {
    typeandident->identifier = tokens->identifier_name;
    typeandident->type = *previoustype;
    tokens += 1;
    *ptrptr = tokens;
    return typeandident;
  }
  else
  {
    fprintf(stderr, "identifier expected after type\n");
    exit(1);
  }
}

Function *parseFunction(Token **ptrptr, Token *token_end)
{
  Token *tokens = *ptrptr;
  if (token_end == tokens)
  {
    fprintf(stderr, "No token found");
    exit(1);
  }

  TypeAndIdent *typeident_ret = parseTypeAndIdentifier(&tokens, token_end);

  Function *func = malloc(sizeof(Function));
  char *function_name = typeident_ret->identifier;
  func->name = function_name;

  consumeOrDie(&tokens, token_end, TK_LeftParenthesis);
  // parameter
  if (tokens->kind != TK_Int)
  {
    func->parameter_length = 0;
  }
  else
  {
    TypeAndIdent *typeident = parseTypeAndIdentifier(&tokens, token_end);
    fprintf(stderr, "first param\n");
    func->parameter_length = 1;
    func->parameter_names[0] = typeident->identifier;
    if (!findLVar(typeident->identifier))
    {
      insertLVar(typeident->identifier);
    }
    fprintf(stderr, "first param done\n");

    for (int i = 1; i < 6; i++)
    {
      if (tokens->kind == TK_Comma)
      {
        tokens += 1;
        TypeAndIdent *typeident = parseTypeAndIdentifier(&tokens, token_end);

        func->parameter_length = i + 1;
        func->parameter_names[i] = typeident->identifier;
        if (!findLVar(typeident->identifier))
        {
          insertLVar(typeident->identifier);
        }
        fprintf(stderr, "%d-th param done\n", i + 1);
      }
      else
      {
        break;
      }
    }
  }
  consumeOrDie(&tokens, token_end, TK_RightParenthesis);
  if (tokens->kind != TK_LeftCurlyBrace)
  {
    fprintf(stderr, "Expected { but got: %d", tokens->kind);
    exit(1);
  }
  Stmt *content = parseFunctionContent(&tokens, token_end);
  // consumeOrDie(&tokens, token_end, TK_RightCurlyBrace);
  *ptrptr = tokens;
  func->content = content;
  return func;
}

void consumeOrDie(Token **ptrptr, Token *token_end, enum TokenKind kind)
{
  Token *tokens = *ptrptr;
  if (tokens->kind != kind)
  {
    fprintf(stderr, "Expected %d but got: %d", kind, tokens->kind);
    exit(1);
  }
  tokens += 1;
  *ptrptr = tokens;
}

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
