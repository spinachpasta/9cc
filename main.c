#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

Token tokens[1000];

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

    Token *ptr = tokens;
    Token *token_end = tokens + token_length;
    Stmt *expr = parseProgram(&ptr, token_end);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    // prologue
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");
    Codegen(expr);
    return 0;
}
