#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FunctionDeclaration *function_declarations;

Type *EvaluateType(Expr *expr)
{
    if (expr->expr_kind == EK_Number)
    {
        Type *integer = calloc(1, sizeof(Type));
        integer->kind = TYPE_INT;
        return integer;
    }
    if (expr->expr_kind == EK_Identifier)
    {
        LVar *local = findLVar(expr->name);
        return local->type;
        // return expr->type;
    }
    if (expr->expr_kind == EK_CallFunction)
    {
        FunctionDeclaration *func = findFunction(expr->name);
        if (func == NULL)
        {
            fprintf(stderr, "function declaration missing:%s\n", expr->name);
            Type *int_type = calloc(1, sizeof(Type));
            int_type->kind = TYPE_INT;
            return int_type;
        }
        return func->return_type;
    }
    if (expr->expr_kind == EK_UnaryOperator)
    {
        if (expr->op == UO_SizeOf)
        {
            Type *int_type = calloc(1, sizeof(Type));
            int_type->kind = TYPE_INT;
            return int_type;
        }
        if (expr->op == UO_AddressOf)
        {
            Type *ptr_to = EvaluateType(expr->first_child);
            Type *ptr_type = calloc(1, sizeof(Type));
            ptr_type->kind = TYPE_PTR;
            ptr_type->ptr_to = ptr_to;
            return ptr_type;
        }
        else if (expr->op == UO_Deref)
        {
            Type *ptr_from = EvaluateType(expr->first_child);
            return ptr_from->ptr_to;
        }
    }
    if (expr->expr_kind == EK_BinaryOperator)
    {

        if (expr->op == BO_Add)
        {
            Type *first = EvaluateType(expr->first_child);
            Type *second = EvaluateType(expr->second_child);
            if (first->kind == TYPE_INT && second->kind == TYPE_INT)
            {
                return first;
            }
            if (first->kind == TYPE_INT && isPointerOrArray(second))
            {
                return second;
            }
            if (isPointerOrArray(first) && second->kind == TYPE_INT)
            {
                return first;
            }
            fprintf(stderr, "invalid type:expected INT+INT, INT + ptr or ptr+INT but got %d and %d", first->kind, second->kind);
            exit(1);
        }
        else if (expr->op == BO_Sub)
        {
            Type *first = EvaluateType(expr->first_child);
            Type *second = EvaluateType(expr->second_child);
            if (first->kind == TYPE_INT && second->kind == TYPE_INT)
            {
                return first;
            }
            if (isPointerOrArray(first) && second->kind == TYPE_INT)
            {
                return first;
            }
            if (isPointerOrArray(first) && isPointerOrArray(second))
            {
                Type *int_type = calloc(1, sizeof(Type));
                int_type->kind = TYPE_INT;
                return int_type;
            }
            fprintf(stderr, "invalid type:expected INT-INT, PTR - INT, PTR-PTR but got %d and %d", first->kind, second->kind);
            exit(1);
        }
        else if (expr->op == BO_Div || expr->op == BO_Mul)
        {
            Type *first = EvaluateType(expr->first_child);
            Type *second = EvaluateType(expr->second_child);
            if (first->kind != TYPE_INT || second->kind != TYPE_INT)
            {
                fprintf(stderr, "invalid type:expected INT*/ INT but got %d and %d", first->kind, second->kind);
                exit(1);
            }
            else
            {
                return first;
            }
        }
        else if (expr->op == BO_Assign)
        {
            Type *first = EvaluateType(expr->first_child);
            Type *second = EvaluateType(expr->second_child);
            if (!typeEqual(first, second))
            {
                fprintf(stderr, "BO_Assign:invalid type:left hand side and right hand side must have the same types but %d and %d", first->kind, second->kind);
                exit(1);
            }
            return first;
        }
        else if (expr->op == BO_Equal || expr->op == BO_Greater || expr->op == BO_GreaterEqual || expr->op == BO_NotEqual)
        {
            Type *first = EvaluateType(expr->first_child);
            Type *second = EvaluateType(expr->second_child);
            if (!typeEqual(first, second))
            {
                fprintf(stderr, "invalid type:left hand side and right hand side must have the same types but %d and %d", first->kind, second->kind);
                exit(1);
            }
            Type *int_type = calloc(1, sizeof(Type));
            int_type->kind = TYPE_INT;
            return int_type;
        }
        else
        {
            fprintf(stderr, "types not implemented:%d", expr->op);
            exit(1);
        }
    }
    return NULL;
}

int isPointerOrArray(Type *t)
{
    return t->kind == TYPE_PTR || t->kind == TYPE_ARRAY;
}

int typeEqual(Type *t1, Type *t2)
{
    if (t1->kind != TYPE_PTR || t2->kind != TYPE_PTR)
    {
        return t1->kind == t2->kind;
    }
    return typeEqual(t1->ptr_to, t2->ptr_to);
}

FunctionDeclaration *findFunction(char *name)
{
    FunctionDeclaration *func = function_declarations;
    if (!func)
    {
        return NULL;
    }
    while (func)
    {
        if (!strcmp(name, func->name))
        {
            return func;
        }
        func = func->next;
    }
    return NULL;
}

FunctionDeclaration *insertFunction(TypeAndIdent *typeandident)
{
    char *name = typeandident->identifier;
    Type *type = typeandident->type;
    FunctionDeclaration *newfunc = calloc(1, sizeof(FunctionDeclaration));
    FunctionDeclaration *last = lastFunction();
    newfunc->name = name;
    newfunc->next = NULL;

    if (!last)
    {
        function_declarations = newfunc;
    }
    else
    {
        last->next = newfunc;
    }
    newfunc->return_type = type;
    return newfunc;
}

FunctionDeclaration *lastFunction()
{
    FunctionDeclaration *func = function_declarations;
    if (!func)
    {
        return NULL;
    }
    while (1)
    {
        if (!func->next)
        {
            return func;
        }
        func = func->next;
    }
}

int getSize(Type *type)
{
    if (type->kind == TYPE_INT)
    {
        return 4;
    }
    if (type->kind == TYPE_PTR)
    {
        return 8;
    }
    if (type->kind == TYPE_ARRAY)
    {
        return getSize(type->ptr_to) * type->array_length;
    }
    fprintf(stderr, "getSize:invalid type:%d", type->kind);
    exit(1);
}