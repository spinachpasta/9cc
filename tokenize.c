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
        if (strncmp(ptr, "sizeof", 6) == 0 && !is_alnum(ptr[6]))
        {
            Token token = {TK_SizeOf, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i += 6;
            continue;
        }
        if (strncmp(ptr, "return", 6) == 0 && !is_alnum(ptr[6]))
        {
            Token token = {TK_Return, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i += 6;
            continue;
        }
        if (strncmp(ptr, "int", 3) == 0 && !is_alnum(ptr[3]))
        {
            Token token = {TK_Int, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i += 3;
            continue;
        }
        if (strncmp(ptr, "if", 2) == 0 && !is_alnum(ptr[2]))
        {
            Token token = {TK_IF, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i += 2;
            continue;
        }
        if (strncmp(ptr, "while", 5) == 0 && !is_alnum(ptr[5]))
        {
            Token token = {TK_WHILE, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i += 5;
            continue;
        }
        if (strncmp(ptr, "else", 4) == 0 && !is_alnum(ptr[4]))
        {
            Token token = {TK_ELSE, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i += 4;
            continue;
        }
        if (strncmp(ptr, "for", 3) == 0 && !is_alnum(ptr[3]))
        {
            Token token = {TK_FOR, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i += 3;
            continue;
        }
        switch (c)
        {
        case '+':
        {
            /* code */
            Token token = {TK_Plus, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case ',':
        {
            /* code */
            Token token = {TK_Comma, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case ';':
        {
            /* code */
            Token token = {TK_Semicolon, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case '{':
        {
            /* code */
            Token token = {TK_LeftCurlyBrace, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case '}':
        {
            /* code */
            Token token = {TK_RightCurlyBrace, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case '(':
        {
            /* code */
            Token token = {TK_LeftParenthesis, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case ')':
        {
            /* code */
            Token token = {TK_RightParenthesis, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case '[':
        {
            /* code */
            Token token = {TK_LeftSquareBracket, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case ']':
        {
            /* code */
            Token token = {TK_RightSquareBracket, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case '-':
        {
            Token token = {TK_Minus, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case '*':
        {
            /* code */
            Token token = {TK_Mul, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case '&':
        {
            /* code */
            Token token = {TK_And, 0, NULL};
            tokens[token_index] = token;
            token_index++;
            i++;
            break;
        }
        case '/':
        {
            Token token = {TK_Div, 0, NULL};
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
                Token token = {TK_Greater, 0, NULL};
                tokens[token_index] = token;
                token_index++;
                break;
            }
            i++;
            Token token = {TK_GreaterEqual, 0, NULL};
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
                Token token = {TK_Less, 0, NULL};
                tokens[token_index] = token;
                token_index++;
                break;
            }
            i++;
            Token token = {TK_LessEqual, 0, NULL};
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
                Token token = {TK_Assign, 0, NULL};
                tokens[token_index] = token;
                token_index++;
                break;
            }
            i++;
            Token token = {TK_Equal, 0, NULL};
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
            Token token = {TK_NotEqual, 0, NULL};
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
            Token token = {TK_Number, parsednum, NULL};
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
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        {
            char *start = str + i;
            i++;
            for (;;)
            {
                char c1 = str[i];
                if (('a' <= c1 && c1 <= 'z') || ('0' <= c1 && c1 <= '9') || ('A' <= c1 && c1 <= 'Z') || c1 == '_')
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
            Token token = {TK_Identifier, 0, NULL};
            token.identifier_name = name;
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

LVar *insertLVar(TypeAndIdent *typeandident)
{
    char *name = typeandident->identifier;
    Type *type = typeandident->type;
    LVar *newlocal = calloc(1, sizeof(LVar));
    LVar *last = lastLVar();
    newlocal->len = strlen(name);
    newlocal->name = name;
    if (!last)
    {
        newlocal->offset = 8;
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
    newlocal->type = type;
    return newlocal;
}

int round_up(int x, int y)
{
    int a = (x / y) * y;
    if (a == x)
    {
        return a;
    }
    return a + 1;
}

LVar *insertLArray(TypeAndIdent *typeandident, int arr_length)
{
    char *name = typeandident->identifier;
    Type *type = typeandident->type;
    LVar *newlocal = calloc(1, sizeof(LVar));
    LVar *last = lastLVar();
    newlocal->len = strlen(name);
    newlocal->name = name;
    int arr_size = round_up(arr_length * getSize(type), 8);
    if (!last)
    {
        newlocal->offset = arr_size;
    }
    else
    {
        newlocal->offset = last->offset + arr_size; // offset+last size
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
    newlocal->type = type;
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