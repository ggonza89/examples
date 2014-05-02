#include "llvm/Analysis/Passes.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include <cstdio>
#include <map>
#include <string>
#include <vector>
namespace project473 {
extern "C" {
  #include "ast.h"
}
}

using namespace llvm;

extern "C" int initLex(int argc, char** argv);
extern "C" void initSymbolTable();
extern "C" int  yyparse();
extern "C" int typecheck();
extern "C" int gettoken();
extern "C" void print_Ast();
extern "C" void printSymbolTable(int flag);
extern "C" project473::AstNodePtr program;
extern "C" project473::SymbolTableStackEntryPtr symbolStackTop;

static Module *TheModule;
static IRBuilder<> Builder(getGlobalContext());
static std::map<std::string, AllocaInst*> NamedValues;
static std::map<std::string, Function*> Functions;
BasicBlock *ReturnBB=0;
AllocaInst *RetAlloca=0;
void Error(const char *Str) { fprintf(stderr, "Error: %s\n", Str);}
Value *ErrorV(const char *Str) { Error(Str); return 0; }

/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  VarName is an std::string containing the variable name
static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName) {


}

/// CreateEntryBlockArrayAlloca - Create an alloca instruction for an array in the entry block of
/// the function.  VarName is an std::string containing the array name, arraySize is the number of elements in the array
static AllocaInst *CreateEntryBlockArrayAlloca(Function *TheFunction, const std::string &VarName, int arraySize) {

}

///Codegen the Allocas for local variables that correspond to the formal variables of function F.
///function_node is an AST node representing a function. Steps
///1. For every formal variable, get its name from the AST, create a Type object
// and call AllocaInst* alloca_inst = Builder.CreateAlloca(Type, name);
//2. set NamedValues[name] = alloca_inst
//3. For every formal variable of the function, create a store instruction that copies the value
//from the formal variable to the allocated local variable.
void createFormalVarAllocations(Function *F, project473::AstNodePtr function_node) {

	char * fvname;
	project473::AstNodePtr formalVar = function_node->children[0];
	// AllocaInst * alloca_inst;
	while(formalVar) {

		fvname = strdup(formalVar->nSymbolPtr->id);
		std::string Name(fvname);
    std::string LocalName(Name);

		if(formalVar->nSymbolPtr->stype->kind == project473::INT) {


      AllocaInst * alloca_inst = Builder.CreateAlloca(Type::getInt32Ty(getGlobalContext()), 0, LocalName.append(".addr"));
			NamedValues[LocalName] = alloca_inst;

      if(NamedValues[Name] != NULL) {

        printf("HOORAY!!!\n");
        exit(0);

      }
      // Builder.CreateStore(NamedValues[Name], NamedValues[LocalName]);
      printf("Formal Var: %s allocated with value %i\n", Name.c_str(), formalVar->nValue);

		}
    // else if(formalVar->nSymbolPtr->stype->kind == project473::ARRAY) {

    //   PointerType* t = Type::getInt32PtrTy(getGlobalContext());
    //   alloca_inst = Builder.CreateAlloca(t, 0, Name);
      // NamedValues[Name] = alloca_inst;
    //   printf("Formal Var: int %s[] allocated\n", Name.c_str());

    // }

    formalVar = formalVar->sibling;

	}

}

Function* Codegen_Function_Declaration(project473::AstNodePtr declaration_node) {
  char* fname = strdup(declaration_node->nSymbolPtr->id);
  std::string Name(fname);
  std::vector<Type*> formalvars;
  project473::AstNodePtr formalVar = declaration_node->children[0];
  while(formalVar) {
    if(formalVar->nSymbolPtr->stype->kind == project473::INT) {
      formalvars.push_back(Type::getInt32Ty(getGlobalContext()));
      formalVar=formalVar->sibling;
    }
    else if(formalVar->nSymbolPtr->stype->kind == project473::ARRAY) {
      PointerType* t = Type::getInt32PtrTy(getGlobalContext());
      formalvars.push_back(t);
      formalVar = formalVar->sibling;
    }
    else {
      printf("Error, formal variable is not an int, in line: %d", formalVar->nLinenumber);
    }
  }
  project473::Type* functionTypeList = declaration_node->nSymbolPtr->stype->function;
  FunctionType *FT;
  if(functionTypeList->kind==project473::INT) {
     FT = FunctionType::get(Type::getInt32Ty(getGlobalContext()), formalvars, false);
  }
  else if(functionTypeList->kind==project473::VOID) {
    FT = FunctionType::get(Type::getVoidTy(getGlobalContext()), formalvars, false);
  }

  Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule);
  // Set names for all arguments. Reuse formalVar
  formalVar = declaration_node->children[0];
  for (Function::arg_iterator AI = F->arg_begin(); formalVar != NULL; ++AI, formalVar=formalVar->sibling) {
          std::string argName(formalVar->nSymbolPtr->id);
          AI->setName(argName);

  }
  Functions[Name] = F; //add the Function to the map of functions
  return F;
}

///Generate code for expressions
Value* Codegen_Expression(project473::AstNodePtr expression) {

  if(expression == NULL)
    return NULL;

  if(expression->eKind==project473::ASSI_EXP) {

    // printf("ASSI_EXP\n");
    Value * varValue = Codegen_Expression(expression->children[0]);
    printf(" = ");
    Value * variable = Codegen_Expression(expression->children[1]);
    printf(";\n");
    if(expression->children[1]->eKind == project473::VAR_EXP)
      variable = Builder.CreateLoad(variable, "var");

    Builder.CreateStore(variable, varValue);
    return varValue;

  }
  else if(expression->eKind==project473::VAR_EXP) {

    printf("VAR_EXP\n");
    printf("%s", expression->nSymbolPtr->id);
    char * varName = strdup(expression->nSymbolPtr->id);
    // sprintf(varName, "%s", expression->nSymbolPtr->id);
    std::string Name(varName);
    AllocaInst* pointerToLocal = NamedValues[Name];
    if(pointerToLocal == NULL) {

      // expression->nSymbolPtr
      llvm::GlobalVariable* gVar = TheModule->getNamedGlobal(Name);
      return gVar;

    }

    return pointerToLocal;


  }
  else if(expression->eKind==project473::CONST_EXP) {

    printf("%i", expression->nValue);
    printf("CONST_EXP\n");
    return Builder.getInt32(expression->nValue);

  }
  else if(expression->eKind==project473::GT_EXP) {

    // printf("GT_EXP\n");
    Value * lhsValue = Codegen_Expression(expression->children[0]);
    // printf(" < ");
    Value * rhsValue = Codegen_Expression(expression->children[1]);
    Value * lhs, *rhs;
    if(expression->children[0]->eKind == project473::CONST_EXP)
      lhs = lhsValue;
    else
      lhs = Builder.CreateLoad(lhsValue, "lhs");
    if(expression->children[1]->eKind == project473::CONST_EXP)
      rhs = rhsValue;
    else
      rhs = Builder.CreateLoad(rhsValue, "rhs");
    return Builder.CreateICmpUGT(lhs, rhs, "cmp");

  }
  else if(expression->eKind==project473::LT_EXP) {

    // printf("LT_EXP\n");
    Value * lhsValue = Codegen_Expression(expression->children[0]);
    Value * rhsValue = Codegen_Expression(expression->children[1]);
    Value * lhs, *rhs;
    if(expression->children[0]->eKind == project473::CONST_EXP)
      lhs = lhsValue;
    else
      lhs = Builder.CreateLoad(lhsValue, "lhs");
    if(expression->children[1]->eKind == project473::CONST_EXP)
      rhs = rhsValue;
    else
      rhs = Builder.CreateLoad(rhsValue, "rhs");
    return Builder.CreateICmpULT(lhs, rhs, "cmp");

  }
  else if(expression->eKind==project473::GE_EXP) {

    // printf("GE_EXP\n");
    Value * lhsValue = Codegen_Expression(expression->children[0]);
    Value * rhsValue = Codegen_Expression(expression->children[1]);
    Value * lhs, *rhs;
    if(expression->children[0]->eKind == project473::CONST_EXP)
      lhs = lhsValue;
    else
      lhs = Builder.CreateLoad(lhsValue, "lhs");
    if(expression->children[1]->eKind == project473::CONST_EXP)
      rhs = rhsValue;
    else
      rhs = Builder.CreateLoad(rhsValue, "rhs");
    return Builder.CreateICmpUGE(lhs, rhs, "cmp");

  }
  else if(expression->eKind==project473::LE_EXP) {

    // printf("LE_EXP\n");
    Value * lhsValue = Codegen_Expression(expression->children[0]);
    Value * rhsValue = Codegen_Expression(expression->children[1]);
    Value * lhs, *rhs;
    if(expression->children[0]->eKind == project473::CONST_EXP)
      lhs = lhsValue;
    else
      lhs = Builder.CreateLoad(lhsValue, "lhs");
    if(expression->children[1]->eKind == project473::CONST_EXP)
      rhs = rhsValue;
    else
      rhs = Builder.CreateLoad(rhsValue, "rhs");
    return Builder.CreateICmpULE(lhs, rhs, "cmp");

  }
  else if(expression->eKind==project473::EQ_EXP) {

    // printf("EQ_EXP\n");
    Value * lhsValue = Codegen_Expression(expression->children[0]);
    // printf(" == ");
    Value * rhsValue = Codegen_Expression(expression->children[1]);
    Value * lhs, *rhs;
    if(expression->children[0]->eKind == project473::CONST_EXP)
      lhs = lhsValue;
    else
      lhs = Builder.CreateLoad(lhsValue, "lhs");
    if(expression->children[1]->eKind == project473::CONST_EXP)
      rhs = rhsValue;
    else
      rhs = Builder.CreateLoad(rhsValue, "rhs");
    return Builder.CreateICmpEQ(lhs, rhs, "cmp");

  }
  else if(expression->eKind==project473::NE_EXP) {

    // printf("NE_EXP\n");
    Value * lhsValue = Codegen_Expression(expression->children[0]);
    Value * rhsValue = Codegen_Expression(expression->children[1]);
    Value * lhs, *rhs;
    if(expression->children[0]->eKind == project473::CONST_EXP)
      lhs = lhsValue;
    else
      lhs = Builder.CreateLoad(lhsValue, "lhs");
    if(expression->children[1]->eKind == project473::CONST_EXP)
      rhs = rhsValue;
    else
      rhs = Builder.CreateLoad(rhsValue, "rhs");

    return Builder.CreateICmpNE(lhs, rhs, "cmp");

  }
  else if(expression->eKind==project473::ADD_EXP) {

    Value * firstChild = Codegen_Expression(expression->children[0]);
    // printf(" + ");
    Value * secondChild = Codegen_Expression(expression->children[1]);
    Value * first;
    Value * second;
    if(expression->children[0]->eKind == project473::CONST_EXP)
      first = firstChild;
    else
      first = Builder.CreateLoad(firstChild, "firstoperand");
    if(expression->children[1]->eKind == project473::CONST_EXP)
      second = secondChild;
    else
      second = Builder.CreateLoad(secondChild, "secondoperand");

    Value * sum = Builder.CreateAdd(first, second, "result", false, false);

    return sum;

  }
  else if(expression->eKind==project473::SUB_EXP) {

    printf("SUB_EXP\n");
    Value * firstChild = Codegen_Expression(expression->children[0]);
    Value * secondChild = Codegen_Expression(expression->children[1]);
    Value * first;
    Value * second;
    if(expression->children[0]->eKind == project473::CONST_EXP)
      first = firstChild;
    else
      first = Builder.CreateLoad(firstChild, "firstoperand");
    if(expression->children[1]->eKind == project473::CONST_EXP)
      second = secondChild;
    else
      second = Builder.CreateLoad(secondChild, "secondoperand");

    Value * sum = Builder.CreateSub(first, second, "result", false, false);

    printf("HELLO!!\n");
    return sum;

  }
  else if(expression->eKind==project473::MULT_EXP) {

    Value * firstChild = Codegen_Expression(expression->children[0]);
    Value * secondChild = Codegen_Expression(expression->children[1]);
    Value * first;
    Value * second;
    if(expression->children[0]->eKind == project473::CONST_EXP)
      first = firstChild;
    else
      first = Builder.CreateLoad(firstChild, "firstoperand");
    if(expression->children[1]->eKind == project473::CONST_EXP)
      second = secondChild;
    else
      second = Builder.CreateLoad(secondChild, "secondoperand");

    Value * sum = Builder.CreateMul(first, second, "result", false, false);

    printf("MULT_EXP\n");
    return sum;

  }
  else if(expression->eKind==project473::DIV_EXP) {

    Value * first = Codegen_Expression(expression->children[0]);
    Value * second = Codegen_Expression(expression->children[1]);

    if(expression->children[0]->eKind != project473::CONST_EXP)
      first = Builder.CreateLoad(first, "firstoperand");

    if(expression->children[1]->eKind != project473::CONST_EXP)
      second = Builder.CreateLoad(second, "secondoperand");

    Value * sum = Builder.CreateUDiv(first, second, "result", false);

    return sum;

  }
  else if(expression->eKind==project473::ARRAY_EXP) {

    printf("ARRAY_EXP\n");

    Value * indexValue = Codegen_Expression(expression->children[0]);

    if(expression->children[0]->eKind == project473::VAR_EXP) {

      printf("COME ON!!!\n");
      indexValue = Builder.CreateLoad(indexValue, "indexValue");

    }

    printf("WTF!\n");
    Value * zeroValue = Builder.getInt32(0);
    Value* Args[] = {zeroValue, indexValue};
    char * arrName = strdup(expression->nSymbolPtr->id);
    std::string Name(arrName);
    Value * arr_name = NamedValues[Name];
    if(arr_name == NULL) {

      printf("Global Array Variable\n");
      llvm::GlobalVariable* gVar = TheModule->getNamedGlobal(Name);
      arr_name = gVar;

    }
    printf("Local Array Variable\n");

    Value* pointerElement = Builder.CreateInBoundsGEP(arr_name, Args, "arrayidx");

    // printf("\n");

    return pointerElement;

  }
  else if(expression->eKind==project473::CALL_EXP) {

    printf("CALL_EXP\n");
    char * fName = strdup(expression->fname);
    std::string FunctionName(fName);
    Function *CalleeF = Functions[FunctionName];
    printf("%s\n", fName);
    std::vector<Value *> ArgsV;
    Value * arg = Codegen_Expression(expression->children[0]);
    while(arg != NULL) {

      // printf("Arg 1!\n");

      if(expression->children[0]->eKind == project473::VAR_EXP || expression->children[0]->eKind == project473::ARRAY_EXP)
        arg = Builder.CreateLoad(arg, "arg");

      ArgsV.push_back(arg);
      arg = Codegen_Expression(expression->sibling);

    }

    printf("Make Call\n");

    return Builder.CreateCall(CalleeF, ArgsV, "call");

  }
  return NULL;
}

///Generates code for the statements.
//Calls Codegen_Expression
Value* Codegen_Statement(project473::AstNodePtr statement, Function * TheFunction) {

    if(statement == NULL) {

      // printf("NULL returned\n");
      return NULL;

    }

    if(statement->sKind==project473::EXPRESSION_STMT) {

      // printf("EXPRESSION_STMT\n");
      if(statement->sibling == NULL)
        return Codegen_Expression(statement->children[0]);
      Codegen_Expression(statement->children[0]);


    }
    else if(statement->sKind==project473::RETURN_STMT) {

      // printf("RETURN_STMT\n");
      // if(statement->nKind == project473::METHOD) {

        if(statement->children[0] == NULL)
          return Builder.CreateRetVoid();

        Value * ret = Codegen_Expression(statement->children[0]);

        if(statement->children[0]->eKind == project473::VAR_EXP || statement->children[0]->eKind == project473::ARRAY_EXP)
          ret = Builder.CreateLoad(ret, "ret");

        return Builder.CreateRet(ret);

      // }


    }
    else if(statement->sKind==project473::IF_THEN_ELSE_STMT) {

      // printf("IF_THEN_ELSE_STMT\n");
      // printf("if(");
      Value * condition = Codegen_Expression(statement->children[0]);
      // printf(")\n");
      // Function * TheFunction = Builder.GetInsertBlock()->getParent();

      // if(TheFunction == NULL)
        // printf("WTH!!!\n");

      BasicBlock * ThenBB = BasicBlock::Create(getGlobalContext(), "if.then", TheFunction);
      BasicBlock * ElseBB = BasicBlock::Create(getGlobalContext(), "if.e");
      BasicBlock * MergeBB = BasicBlock::Create(getGlobalContext(), "if.end");
      if(statement->children[2] != NULL) {

        Builder.CreateCondBr(condition, ThenBB, ElseBB);
        Builder.SetInsertPoint(ThenBB);
        Value *ThenV = Codegen_Statement(statement->children[1], TheFunction); //generate code for the then part
        Builder.CreateBr(MergeBB); //after generating code, create a branch to the Merge basic block
        // if(statement->children[2] != NULL) {

          // printf("else\n");
        ThenBB = Builder.GetInsertBlock();
        TheFunction->getBasicBlockList().push_back(ElseBB); //add the Else basic block to the list of basic blocks of the function
        Builder.SetInsertPoint(ElseBB);
        Value *ElseV = Codegen_Statement(statement->children[2], TheFunction);
        Builder.CreateBr(MergeBB); //after generating code, create a branch to the Merge basic block
        TheFunction->getBasicBlockList().push_back(MergeBB);//add the Merge basic block to the list of basic blocks of the function
      // Builder.CreateBr(MergeBB);
        Builder.SetInsertPoint(MergeBB);

      }
      else {

        Builder.CreateCondBr(condition, ThenBB, MergeBB);
        // Builder.CreateBr(ThenBB);
        Builder.SetInsertPoint(ThenBB);
        Value * ThenV = Codegen_Statement(statement->children[1], TheFunction);
        Builder.CreateBr(MergeBB);
        // printf("wtf!!\n");
        TheFunction->getBasicBlockList().push_back(MergeBB);//add the Merge basic block to the list of basic blocks of the function
        // Builder.CreateBr(MergeBB);
        Builder.SetInsertPoint(MergeBB);

      }

    }
    else if(statement->sKind==project473::COMPOUND_STMT) {

      // printf("COMPOUND_STMT\n");
      if(statement->children[0] != NULL)
        Codegen_Statement(statement->children[0], TheFunction);
      // printf("wtf!!\n");

    }
    else if(statement->sKind==project473::WHILE_STMT) {

      // printf("WHILE_STMT\n");

      Function * TheFunction = Builder.GetInsertBlock()->getParent();
      BasicBlock * ConditionBB = BasicBlock::Create(getGlobalContext(), "while.condition", TheFunction);
      BasicBlock * WhileBB = BasicBlock::Create(getGlobalContext(), "while.block");
      BasicBlock * WhileEndBB = BasicBlock::Create(getGlobalContext(), "while.end");
      Builder.CreateBr(ConditionBB);
      Builder.SetInsertPoint(ConditionBB);
      Value * condition = Codegen_Expression(statement->children[0]);
      Builder.CreateCondBr(condition, WhileBB, WhileEndBB);
      Builder.SetInsertPoint(WhileBB);
      Value * WhileValue = Codegen_Statement(statement->children[1], TheFunction);
      TheFunction->getBasicBlockList().push_back(WhileBB);
      Builder.CreateBr(ConditionBB);
      TheFunction->getBasicBlockList().push_back(WhileEndBB);
      Builder.SetInsertPoint(WhileEndBB);

    }

    // printf("statement->sibling\n");

    return Codegen_Statement(statement->sibling, TheFunction);

}

//generates the code for the body of the function. Steps
//1. Generate alloca instructions for the local variables
//2. Iterate over list of statements and call Codegen_Statement for each of them
Value* Codegen_Function_Body(project473::AstNodePtr function_node) {

  project473::AstNodePtr formalVar = function_node->children[0];
  project473::AstNodePtr function_body = function_node->children[1];

  std::map<std::string, Value *> FormalVars;

  // while(formalVar) {

  //   printf("%s -> %i\n", formalVar->nSymbolPtr->id, formalVar->nValue);
  //   std::string fVar(strdup(formalVar->nSymbolPtr->id));
  //   FormalVars[fVar] = NamedValues[fVar];

  //   formalVar = formalVar->sibling;

  // }

  for(int i = 0; i < MAXHASHSIZE; i++) {

    project473::ElementPtr localVars = function_body->nSymbolTabPtr->hashTable[i];
    AllocaInst * alloca_inst;
    char * localName;
    while(localVars) {

      localName = strdup(localVars->id);
      std::string Name(localName);

      if(FormalVars[Name] != NULL) {

        // Builder.CreateStore(variable, NamedValues[Name]);

      }
      else {

        if(localVars->stype->kind == project473::INT) {

          AllocaInst * alloca_inst = Builder.CreateAlloca(Type::getInt32Ty(getGlobalContext()), 0, localName);
          NamedValues[Name] = alloca_inst;
          printf("Local Var: int %s;\n", Name.c_str());

        }
        else if(localVars->stype->kind == project473::ARRAY) {

          int arraySize = localVars->stype->dimension;
          ArrayType* arrType = ArrayType::get(Type::getInt32Ty(getGlobalContext()), arraySize);
          alloca_inst = Builder.CreateAlloca(arrType, 0, localName);
          NamedValues[Name] = alloca_inst;
          printf("Local Var: int %s[];\n", Name.c_str());

        }

      }

      localVars = localVars->next;

    }

  }

  project473::AstNodePtr statements = function_body;
  while(statements != NULL) {

    // printf("WTF!!!\n");
    Function * TheFunction = Builder.GetInsertBlock()->getParent();
    Codegen_Statement(statements, TheFunction);
    // printf("Why!!\n");
    statements = statements->sibling;
    if(statements == NULL)
      break;
      // return NULL;

  }

  return NULL;

}

//generates code for the function, verifies the code for the function. Steps:
//1. Create the entry block, place the insert point for the builder in the entry block
//2. Call CreateFormalVarAllocations
//3. Call Codegen_Function_Body
//4. Insert 'return' basic block if needed
Function* Codegen_Function(project473::AstNodePtr function_node) {

  char * fname = strdup(function_node->nSymbolPtr->id);
  std::string functionName(fname);
  Function * TheFunction = TheModule->getFunction(functionName);
  BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", TheFunction);
  Builder.SetInsertPoint(BB);

  createFormalVarAllocations(TheFunction, function_node);
  // Codegen_Function_Body(function_node);

  return TheFunction;

}

void initializeFunctions(project473::AstNodePtr program) {
    project473::AstNodePtr currentFun = program;
    while(currentFun != NULL) {
       Function *TheFunction = Codegen_Function_Declaration(currentFun);
       currentFun=currentFun->sibling;
    }
}

void codegen() {
  InitializeNativeTarget();
  LLVMContext &Context = getGlobalContext();
  // Make the module, which holds all the code.
  TheModule = new Module("filename", Context);
  //Codegen the global variables
  project473::ElementPtr currentGlobal = symbolStackTop->symbolTablePtr->queue;
  char* currentGlobalName;

  while(currentGlobal != NULL) {

    currentGlobalName = strdup(currentGlobal->id);
    std::string globalVarName(currentGlobalName);

    if (currentGlobal->stype->kind==project473::INT) {

      TheModule->getOrInsertGlobal(globalVarName, Type::getInt32Ty(getGlobalContext()));
      llvm::GlobalVariable* gVar = TheModule->getNamedGlobal(globalVarName);
      gVar->setLinkage(llvm::GlobalValue::CommonLinkage);
      gVar->setInitializer(Builder.getInt32(0));
      printf("Global Var: int %s;\n", currentGlobalName);

    }
    else if(currentGlobal->stype->kind==project473::ARRAY) {

      PointerType* t = Type::getInt32PtrTy(getGlobalContext());
      TheModule->getOrInsertGlobal(globalVarName, t);
      llvm::GlobalVariable* gVar = TheModule->getNamedGlobal(globalVarName);
      gVar->setLinkage(llvm::GlobalValue::CommonLinkage);
      gVar->setInitializer(Builder.getInt32(0));
      printf("Global Var: int %s[%i];\n", globalVarName.c_str(), currentGlobal->stype->dimension);

    }

    currentGlobal = currentGlobal->queue_next;

  }

  initializeFunctions(program);
  project473::AstNodePtr currentFunction = program;

  while(currentFunction != NULL) {
    Function* theFunction = Codegen_Function(currentFunction);
    NamedValues.clear();
    currentFunction = currentFunction->sibling;
  }
  // Print out all of the generated code.
  std::string ErrInfo;
  llvm::raw_ostream* filestream = new llvm::raw_fd_ostream("/home/ggonza20/codegen/out.s",ErrInfo,sys::fs::F_RW);
  TheModule->print(*filestream,0);
}

int main(int argc, char** argv) {
  initLex(argc,argv);
    initSymbolTable();
    print_Ast();
    yyparse();
    int typ = typecheck();
    printf("\ntypecheck returned: %d\n", typ);
    codegen();
 return 0;
}
