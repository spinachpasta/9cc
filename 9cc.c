#include <stdio.h>
#include <stdlib.h>

enum TokenKind
{
  Number,
  Minus,
  Plus
};

typedef struct Token
{
  enum TokenKind kind;
  int value;
} Token;

int isDigit(char c);
int intLength(char *str);

Token tokens[1000];

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
      Token token = {Plus, 0};
      tokens[token_index] = token;
      token_index++;
      i++;
      break;
    }
    case '-':
    {
      /* code */
      Token token = {Minus, 0};
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
      Token token = {Number, parsednum};
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
  {
    Token token = tokens[0];
    if (token.kind != Number)
    {
      fprintf(stderr, "The first token is not number. The first token shall be a number");
      return 1;
    }
    printf("  mov rax, %d\n", token.value);
  }
  for (int i = 1; i < token_length;)
  {
    Token maybe_operator = tokens[i];
    switch (maybe_operator.kind)
    {
    case Number:
    {
      fprintf(stderr, "Expected operator got Number");
      return 1;
    }
    case Plus:
    {
      i++;
      if (i >= token_length)
      {
        fprintf(stderr, "Expected: number, but got EOF");
        return 1;
      }

      Token maybe_number = tokens[i];
      if (maybe_number.kind != Number)
      {
        fprintf(stderr, "Expected: number. Token Kind:%d", maybe_number.kind);
        return 1;
      }
      printf("  add rax, %d\n", maybe_number.value);
      i++;

      break;
    }
    case Minus:
    {
      i++;
      if (i >= token_length)
      {
        fprintf(stderr, "Expected: number, but got EOF");
        return 1;
      }
      Token maybe_number = tokens[i];
      if (maybe_number.kind != Number)
      {
        fprintf(stderr, "Expected: number. Token Kind:%d", maybe_number.kind);
        return 1;
      }
      printf("  sub rax, %d\n", maybe_number.value);
      i++;
      break;
    }
    default:
      fprintf(stderr, "Invalid token:%d", maybe_operator.kind);
      return 1;
      break;
    }
  }
  printf("  ret\n");
  return 0;
}