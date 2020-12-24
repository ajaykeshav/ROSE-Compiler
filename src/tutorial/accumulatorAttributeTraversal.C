// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"
#include <iostream>
#include <string>

// Build an accumulator attribute, fancy name for what is essentially a global variable :-).
class AccumulatorAttribute
{
public:
    int forLoopCounter;
    Rose_STL_Container<SgInitializedName*> globalVariables;
    // Specific constructors are optional
    AccumulatorAttribute () { forLoopCounter = 0;}
    AccumulatorAttribute ( const AccumulatorAttribute & X ) {}
    AccumulatorAttribute & operator= ( const AccumulatorAttribute & X ) { return *this; }
    // Member function to determine the smallest array variable in the source AST
    int determineLeastSizeAmongArrayVars(SgNode*);
    void buildListOfGlobalVariables(SgSourceFile *);
};

class visitorTraversal : public AstSimpleProcessing
{
public:
    static AccumulatorAttribute accumulatorAttribute;
    virtual void visit(SgNode* n);
};

void AccumulatorAttribute::buildListOfGlobalVariables(SgSourceFile *file)
{
    ROSE_ASSERT(file != NULL);
    SgGlobal *globalScope = file->get_globalScope();
    ROSE_ASSERT(globalScope != NULL);
    SgDeclarationStatementPtrList declStmt = globalScope->get_declarations();
    Rose_STL_Container<SgDeclarationStatement*>::iterator i = declStmt.begin();
    while(i != declStmt.end()) {
        SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration(*i);
        if(variableDeclaration != NULL){
            Rose_STL_Container<SgInitializedName*> &variableList = variableDeclaration->get_variables();
            Rose_STL_Container<SgInitializedName*>::iterator var = variableList.begin();
            while(var != variableList.end()){
                visitorTraversal::accumulatorAttribute.globalVariables.push_back(*var);
                var++;
            }
            SageInterface::removeStatement(*i);
        }
        i++;
    }
}

int AccumulatorAttribute::determineLeastSizeAmongArrayVars(SgNode* n)
{
    int least_size = INT32_MAX;
    Rose_STL_Container<SgNode*> arrayAccess = NodeQuery::querySubTree(n,V_SgPntrArrRefExp);
    for(Rose_STL_Container<SgNode*>::iterator it = arrayAccess.begin(); it != arrayAccess.end(); ++it){
        SgBinaryOp *fullExp = isSgBinaryOp(*it);
        ROSE_ASSERT(fullExp != NULL);
        SgExpression *arrVarExp = fullExp->get_lhs_operand();
        SgType *arrayVarType = arrVarExp->get_type();
        SgArrayType *arrayVar = isSgArrayType(arrayVarType);
        if(arrayVar != NULL){
            int sz = arrayVar->get_number_of_elements();
            if(sz < least_size)
                least_size = sz;
        }
    }
    return least_size;
}

// declaration required for static data member
AccumulatorAttribute visitorTraversal::accumulatorAttribute;

void visitorTraversal::visit(SgNode* n)
{
    if(isSgFunctionDefinition(n) != NULL)
    {
        SgFunctionDefinition *func = isSgFunctionDefinition(n);
        SgFunctionDeclaration *func_declaration = func->get_declaration();
        ROSE_ASSERT(func_declaration != NULL);
        func_declaration->set_name(SgName("main"));
        SgFunctionType *func_type = func_declaration->get_type();
        SgType *integerTy = new SgTypeInt();
        ROSE_ASSERT(integerTy != NULL);
        func_type->set_orig_return_type(integerTy);
        func_declaration->set_type(func_type);
        func->set_declaration(func_declaration);

        SgBasicBlock *func_body = func->get_body();
        ROSE_ASSERT(func_body != NULL);

        Sg_File_Info *sourceLocation = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
        ROSE_ASSERT(sourceLocation != NULL);

        /* insert global variables declarations here to make bring them to local scope */
        for(Rose_STL_Container<SgInitializedName*>::iterator var = accumulatorAttribute.globalVariables.begin(); var != accumulatorAttribute.globalVariables.end(); ++var){
            SgType *type = (*var)->get_type();
            ROSE_ASSERT(type != NULL);

            SgName name = (*var)->get_name();
            SgVariableDeclaration *variableDeclaration = SageBuilder::buildVariableDeclaration(name, type);
            SageInterface::prependStatement(variableDeclaration,func_body);
        }

        /* insert return 0 at the end */
        SgExpression *retVal = SageBuilder::buildIntVal(0);
        SgReturnStmt *retStmt = SageBuilder::buildReturnStmt(retVal);
        SgStatement *lastStmt = SageInterface::getLastStatement(func_body);
        SageInterface::insertStatementAfter(lastStmt,retStmt);
    }
    if (isSgForStatement(n) != NULL)
    {
        SgForStatement *forStmt = isSgForStatement(n);
        SgExpression *loopCondExpr = forStmt->get_test_expr();
        ROSE_ASSERT(loopCondExpr != NULL);
        SgBinaryOp *op = isSgBinaryOp(loopCondExpr);
        ROSE_ASSERT(op != NULL);
        SgExpression *boundVal = op->get_rhs_operand();
        SgIntVal *varType = isSgIntVal(boundVal);
        if(varType == nullptr){
            /* set the upper bound to the least possible value */
            int upperBound = accumulatorAttribute.determineLeastSizeAmongArrayVars(n);
            SgExpression *upperBoundExp = SageBuilder::buildIntVal(upperBound);
            op->set_rhs_operand(upperBoundExp);
        }
        accumulatorAttribute.forLoopCounter++;
    }
}

int
main ( int argc, char* argv[] )
{
    // Initialize and check compatibility. See Rose::initialize
    ROSE_INITIALIZE;

    SgProject* project = frontend(argc,argv);
    ROSE_ASSERT (project != NULL);

    // Build the traversal object
    visitorTraversal exampleTraversal;

    // Build list of global variables
    const SgFilePtrList &fileList = project->get_fileList();
    SgFilePtrList::const_iterator file = fileList.begin();
    SgSourceFile *f = isSgSourceFile(*file);
    visitorTraversal::accumulatorAttribute.buildListOfGlobalVariables(f);
    SgGlobal *globalScope = f->get_globalScope();
    ROSE_ASSERT(globalScope != NULL);

    // Call the traversal starting at the project node of the AST
    // can be specified to be preorder or postorder).
    exampleTraversal.traverseInputFiles(project,preorder);

    AstTests::runAllTests(project);

    return backend(project);
}