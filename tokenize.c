#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

LVar *locals;

int tokenize(char *str)
{
    int token_index = 0;
    for (int i = 0; str[i];)
    {
        char c = str[i];
        char *ptr = str + i;
        if (strncmp(ptr, "return", 6) == 0 && !is_alnum(ptr[6]))
        {
            Token token = {TK_Return, 0};
            tokens[token_index] = token;
            token_index++;
            i += 6;
            continue;
        }
        if (strncmp(ptr, "if", 2) == 0 && !is_alnum(ptr[2]))
        {
            Token token = {TK_IF, 0};
            tokens[token_index] = token;
            token_index++;
            i += 2;
            continue;
        }
        if (strncmp(ptr, "while", 5) == 0 && !is_alnum(ptr[5]))
        {
            Token token = {TK_WHILE, 0};
            tokens[token_index] = token;
            token_index++;
            i += 5;
            continue;
        }
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
        case ';':
        {
            /* code */
            Token token = {TK_Semicolon, 0};
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
                Token token = {TK_Assign, 0};
                tokens[token_index] = token;
                token_index++;
                break;
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
        case 'a':
        case 'b':
        case 'c':
        {
            char *start = str + i;
            i++;
            for (;;)
            {
                char c1 = str[i];
                if ('a' <= c1 && c1 <= 'z')
                {
                    i++;
                }
                else
                {
                    break;
                }
            }
            char *end = str + i;

            int length = end - start;
            char *name = malloc(length + 1);
            memcpy(name, start, length);
            name[length] = '\0';
            if (!findLVar(name))
            {
                insertLVar(name);
            }
            Token token = {TK_Identifier, 0};
            token.name = name;
            tokens[token_index] = token;
            token_index++;

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
int is_alnum(char c)
{
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
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

LVar *findLVar(char *name)
{
    LVar *local = locals;
    if (!local)
    {
        return NULL;
    }
    while (local)
    {
        if (!strcmp(name, local->name))
        {
            return local;
        }
        local = local->next;
    }
    return NULL;
}

LVar *insertLVar(char *name)
{
    LVar *newlocal = calloc(1, sizeof(LVar));
    LVar *last = lastLVar();
    newlocal->len = strlen(name);
    newlocal->name = name;
    if (!last)
    {
        newlocal->offset = 0;
    }
    else
    {
        newlocal->offset = last->offset + 8; // offset+last size
    }
    newlocal->next = NULL;

    if (!last)
    {
        locals = newlocal;
    }
    else
    {
        last->next = newlocal;
    }
    return newlocal;
}

LVar *lastLVar()
{
    LVar *local = locals;
    if (!local)
    {
        return NULL;
    }
    while (1)
    {
        if (!local->next)
        {
            return local;
        }
        local = local->next;
    }
}