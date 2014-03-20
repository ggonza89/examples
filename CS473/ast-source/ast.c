#include "symbolTable.h"
#include "ast.h"


extern SymbolTableStackEntryPtr symbolStackTop;

extern int scopeDepth;

//creates a new expression node
AstNodePtr  new_ExprNode(ExpKind kind)
{
    return NULL;
}

//creates a new statement node
AstNodePtr new_StmtNode(StmtKind kind)
{
    return NULL;
}

AstNodePtr new_Node(NodeKind kind) {

    AstNodePtr newNode = (AstNodePtr)malloc(sizeof(AstNode));
    if(newNode == NULL)
        return NULL;
    newNode->nKind = kind;
    return newNode;

}

//creates a new type node for entry into symbol table
Type* new_type(TypeKind kind) {

    Type * newType = (Type*)malloc(sizeof(Type));
    if(newType == NULL)
        return NULL;
    newType->kind = kind;
    return newType;

}







