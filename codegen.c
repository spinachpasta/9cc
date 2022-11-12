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
