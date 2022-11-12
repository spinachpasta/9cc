#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

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
