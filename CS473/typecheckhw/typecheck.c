
// #include "symbolTable.h"
#include "typecheck.h"
// #include "ast.h"

extern AstNode * program;

// compares two types and returns the resulting type
// from the comparison

Type* type_equiv(Type *t1, Type *t2){

    printf("type_equiv\n");
    if(t1->kind == t2->kind)
        return t1;

    printf("wtf\n");
    return NULL;

}

int checkType(TypePtr type) {

    switch(type->kind) {

        case INT:
            return 0;
        case ARRAY:
            return 1;
        case VOID:
            return 2;
        case FUNCTION:
            return checkType(type->function);

    }

}

// Typechecks a method and returns 1 on success
int typecheck_method(AstNode *node_){

    printf("root->nKind = METHOD\n");
    if(node_->children[0] == NULL)
        printf("Method has no parameters\n");
    else // typecheck parameters of method
        if(typecheck_Ast_Recursion(node_->children[0]) != 1)
            return 0;

    int return_type = typecheck_Ast_Recursion(node_->children[1]);
    printf("root->nKind = METHOD, return = %d\n", return_type);
    if(checkType(node_->nType) == 0 && return_type == 3)
        return typecheck_Ast_Recursion(node_->sibling);
    else if( checkType(node_->nType) == 2 && return_type == 2)
        return typecheck_Ast_Recursion(node_->sibling);
    else {
        if(return_type > 0)
            return 1;
        else
            return 0;
    }

}

// Typechecks a statement and returns 1 on success
int typecheck_stmt( AstNode *node_){

    if(node_ == NULL)
        return 1;

    int return_type = 0;

    switch(node_->sKind) {

        case RETURN_STMT :
            printf("stmt->sKind = RETURN_STMT\n");
            if(node_->children[0] == NULL) {

                printf("return type VOID\n");
                return 2;

            }
            else
                if(typecheck_expr(node_->children[0]) != NULL) {

                    printf("return type INT\n");
                    return 3;

                }
            return 0;

        case IF_THEN_ELSE_STMT :
            printf("stmt->sKind = IF_THEN_ELSE_STMT\n");

            //for if stmt check expression
            if(typecheck_expr(node_->children[0]) == NULL) {

                printf("Error: not a valid expression in if statement at line %d\n", node_->nLinenumber);
                return 0;

            }
            return_type = 0;
            if(node_->children[1] != NULL)
                return_type = typecheck_stmt(node_->children[1]);
            if(return_type != 0 && node_->children[2] != NULL )
                return_type = typecheck_stmt(node_->children[2]);

            printf("stmt->sKind = IF_STMT, return = %d\n", return_type);
            return return_type;


        case WHILE_STMT :
            printf("stmt->sKind = WHILE_STMT\n");
            if(typecheck_expr(node_->children[0]) == NULL) {

                printf("Error: not a valid expression in while statement at line %d\n", node_->nLinenumber);
                return 0;

            }
            else if(node_->children[1] != NULL)
                return_type = typecheck_stmt(node_->children[1]);

            printf("stmt->sKind = WHILE_STMT, return = %d\n", return_type);
            return return_type;

        case COMPOUND_STMT :
            printf("stmt->sKind = COMPOUND_STMT\n");
            if(node_->children[0] != NULL) {

                return_type = typecheck_stmt(node_->children[0]);
                // node_->children[0]->nType = node_->nType;
                if(return_type > 0) {

                    printf("trying siblings\n");
                    if(typecheck_Ast_Recursion(node_->sibling) == 1) {

                        printf("stmt->sKind = COMPOUND_STMT, return = %d\n", return_type);
                        return return_type;

                    }

                }

            }
            return 0;

        case EXPRESSION_STMT :
            printf("stmt->sKind = EXPRESSION_STMT\n");
            if(typecheck_expr(node_->children[0]) == NULL) {

                printf("idk why!!\n");
                return 0;

            }

            return_type = typecheck_Ast_Recursion(node_->sibling);
            printf("stmt->sKind = EXPRESSION_STMT, return = %d\n", return_type);
            return return_type;

    }

    return 0;

}

// Type checks a given expression and returns its type
//

Type * typecheck_expr (AstNode * expr){

    if(expr == NULL)
        return NULL;

    switch(expr->eKind) {

        case VAR_EXP :
            printf("expr->eKind = VAR_EXP\n");
            return new_type(INT);

        case ARRAY_EXP :
            printf("%s[", expr->nSymbolPtr->id);
            // print_Expression(expr->children[0], 0);
            printf("]");
            break;

        case ASSI_EXP :
            printf("expr->eKind = ASSI_EXP\n");
            return type_equiv(typecheck_expr(expr->children[0]), typecheck_expr(expr->children[1]));

        case ADD_EXP :
            // printf(" + ");
            return type_equiv(typecheck_expr(expr->children[0]), typecheck_expr(expr->children[1]));

        case SUB_EXP :
            // printf(" - ");
            return type_equiv(typecheck_expr(expr->children[0]), typecheck_expr(expr->children[1]));

        case MULT_EXP :
            // printf(" * ");
            return type_equiv(typecheck_expr(expr->children[0]), typecheck_expr(expr->children[1]));

        case DIV_EXP :
            // printf(" / ");
            return type_equiv(typecheck_expr(expr->children[0]), typecheck_expr(expr->children[1]));

        case GT_EXP :
            // printf(" > ");
            return type_equiv(typecheck_expr(expr->children[0]), typecheck_expr(expr->children[1]));

        case LT_EXP :
            // printf(" < ");
            return type_equiv(typecheck_expr(expr->children[0]), typecheck_expr(expr->children[1]));

        case GE_EXP :
            // printf(" >= ");
            return type_equiv(typecheck_expr(expr->children[0]), typecheck_expr(expr->children[1]));

        case LE_EXP :
            // printf(" <= ");
            return type_equiv(typecheck_expr(expr->children[0]), typecheck_expr(expr->children[1]));

        case EQ_EXP :
            // printf("==");
            return type_equiv(typecheck_expr(expr->children[0]), typecheck_expr(expr->children[1]));

        case NE_EXP :
            // printf("!= ");
            return type_equiv(typecheck_expr(expr->children[0]), typecheck_expr(expr->children[1]));

        case CALL_EXP :
            printf("%s(", expr->fname);
            if(expr->children[0] != NULL) {
                // print_Expression(expr->children[0], 0);
                AstNodePtr ptr = expr->children[0]->sibling;
                while(ptr != NULL) {
                    // printf(", ");
                    // print_Expression(ptr, 0);
                    ptr = ptr->sibling;
                }
            }
            // printf(")");
            break;

        case CONST_EXP :
            printf("expr->eKind = CONST_EXP\n");
            return new_type(INT);

        default:
            printf("What is going on!!!!\n");
            break;

    }

    return NULL;

}


int typecheck_Ast_Recursion(AstNodePtr root) {

    if(root == NULL) {

        printf("Sibling is NULL\n");
        return 1;

    }
    switch(root->nKind) {

        case METHOD :
            return typecheck_method(root);
        case FORMALVAR :
            printf("root->nKind = FORMALVAR\n");
            return 1;
        case STMT :
            printf("root->nKind = STMT\n");
            int return_type = typecheck_stmt(root);
            printf("root->nKind = STMT, return = %d\n", return_type);
            return return_type;
        case EXPRESSION :
            printf("root->nKind = EXPRESSION\n");
            typecheck_expr(root);
            return 1;

    }

    return 0;

}

// Starts typechecking the AST  returns 1 on success
//use the global variable program
//
int typecheck(){

    return typecheck_Ast_Recursion(program);

}
