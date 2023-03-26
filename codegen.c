#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

int labelCounter = 0;

char *registers[] = {
    "rdi",
    "rsi",
    "rdx",
    "rcx",
    "r8",
    "r9",
};

void EvaluateLValueAddressIntoRax(Expr *expr)
{
    if (expr->expr_kind == EK_Identifier)
    {
        LVar *local = findLVar(expr->name);
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", local->offset);
    }
    else
    {
        fprintf(stderr, "not lvalue:expr kind=%d", expr->expr_kind);
        exit(1);
    }
}

void CodegenFunction(Function *func)
{
    printf(".globl %s\n", func->name);
    printf("%s:\n", func->name);
    // prologue
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");
    for (int i = 0; i < func->parameter_length; i++)
    {
        // fprintf(stderr,"%s\n",func->parameter_names[i]);
        LVar *local = findLVar(func->parameter_names[i]);
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", local->offset);
        printf("  mov [rax], %s\n", registers[i]);
    }
    Codegen(func->content);
}

void Codegen(Stmt *stmt)
{
    switch (stmt->stmt_kind)
    {
    case SK_Expr:
    {
        EvaluateExprIntoRax(stmt->expr);
        break;
    }
    case SK_AndThen:
    {
        Codegen(stmt->first_child);
        Codegen(stmt->second_child);
        break;
    }
    case SK_Return:
    {
        EvaluateExprIntoRax(stmt->expr);
        // epilogue
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        break;
    }
    case SK_If:
    {
        EvaluateExprIntoRax(stmt->expr);
        printf("  cmp rax, 0\n");
        printf("  je  .Lelse%d\n", labelCounter);
        Codegen(stmt->second_child);
        printf("  jmp .Lend%d\n", labelCounter);
        printf(".Lelse%d:\n", labelCounter);
        if (stmt->third_child != NULL)
        {
            Codegen(stmt->third_child);
        }
        printf(".Lend%d:\n", labelCounter);
        labelCounter++;
        break;
    }
    case SK_While:
    {
        printf(".Lbegin%d:\n", labelCounter);
        EvaluateExprIntoRax(stmt->expr);
        printf("  cmp rax, 0\n");
        printf("  je  .Lend%d\n", labelCounter);
        Codegen(stmt->second_child);
        printf("  jmp  .Lbegin%d\n", labelCounter);
        printf(".Lend%d:\n", labelCounter);

        labelCounter++;
        break;
    }
    case SK_For:
    {
        if (stmt->expr)
        {
            EvaluateExprIntoRax(stmt->expr);
        }
        printf(".Lbegin%d:\n", labelCounter);
        if (stmt->expr1)
        {
            EvaluateExprIntoRax(stmt->expr1);
        }
        else
        {
            printf("  mov rax, 1\n");
        }
        printf("  cmp rax, 0\n");
        printf("  je  .Lend%d\n", labelCounter);
        Codegen(stmt->second_child);
        if (stmt->expr2)
        {
            EvaluateExprIntoRax(stmt->expr2);
        }
        printf("  jmp  .Lbegin%d\n", labelCounter);
        printf(".Lend%d:\n", labelCounter);

        labelCounter++;
        break;
    }
    default:
        break;
    }
}
void EvaluateExprIntoRax(Expr *expr)
{
    switch (expr->expr_kind)
    {
    case EK_CallFunction:
        for (int i = 5; i >= 0; i--)
        {
            if (expr->argments[i])
            {
                EvaluateExprIntoRax(expr->argments[i]);
                printf("   push rax\n");
            }
        }
        for (int i = 0; i < 6; i++)
        {
            if (expr->argments[i])
            {
                printf("   pop %s\n", registers[i]);
            }
        }
        printf("   call %s\n", expr->name);
        break;
    case EK_Identifier:
        EvaluateLValueAddressIntoRax(expr);
        printf("  mov rax,[rax]\n");
        break;

    case EK_Number:
        printf("  mov rax, %d\n", expr->value);
        break;
    case EK_UnaryOperator:
    {
        switch (expr->op)
        {
        case UO_Deref:
            EvaluateExprIntoRax(expr->first_child);
            printf("    mov rax,[rax]\n");
            break;
        case UO_AddressOf:
            EvaluateLValueAddressIntoRax(expr->first_child);
            break;
        default:
        {
            fprintf(stderr, "Invalid unaryop kind:%d", expr->op);
            exit(1);
            break;
        }
        }
        break;
    }
    case EK_BinaryOperator:
    {
        if (expr->op == BO_Assign)
        {
            EvaluateLValueAddressIntoRax(expr->first_child);
            printf("    push rax\n");
            EvaluateExprIntoRax(expr->second_child);
            printf("    push rax\n");
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], rdi\n");
            break;
        }
        EvaluateExprIntoRax(expr->first_child);
        printf("    push rax\n");
        EvaluateExprIntoRax(expr->second_child);
        printf("    push rax\n");

        printf("    pop rdi\n");
        printf("    pop rax\n");

        switch (expr->op)
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
        case BO_AndThen:
        {
            printf("  mov rax, rdi\n");
            break;
        }
        default:
        {
            fprintf(stderr, "Invalid binaryop kind:%d", expr->op);
            exit(1);
            break;
        }
        }
        break;
    }

    default:
        fprintf(stderr, "Invalid expr kind:%d", expr->expr_kind);
        exit(1);
        break;
    }
}
