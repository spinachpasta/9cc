#include <stdio.h>
#include <stdlib.h>

int isDigit(char c);

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

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  {
    int parsednum = parseInt(p);
    int parsedlength = intLength(p);
    p += parsedlength;
    printf("  mov rax, %ld\n", parsednum);
  }
  while (*p)
  {
    if (*p == '+')
    {
      p++;
      {
        int parsednum = parseInt(p);
        int parsedlength = intLength(p);
        p += parsedlength;
        printf("  add rax, %ld\n", parsednum);
      }
      continue;
    }

    if (*p == '-')
    {
      p++;
      {
        int parsednum = parseInt(p);
        int parsedlength = intLength(p);
        p += parsedlength;
        printf("  sub rax, %ld\n", parsednum);
      }
      continue;
    }

    fprintf(stderr, "予期しない文字です: '%c'\n", *p);
    return 1;
  }

  printf("  ret\n");
  return 0;
}