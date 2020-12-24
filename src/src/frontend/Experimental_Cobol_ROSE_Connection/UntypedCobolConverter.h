#ifndef UNTYPED_COBOL_CONVERTER_H
#define UNTYPED_COBOL_CONVERTER_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedConverter class functions are used to convert
// SgUntypedNodes to regular SgNodes.
//-----------------------------------------------------------------------------------

#include "sageBuilder.h"
#include "CobolBuilderInterface.h"

#ifdef FORTRAN_JUNK
#include "FASTNodes.hpp"
#include "FortranBuilderInterface.h"
#endif

namespace Cobol {
namespace Untyped {

class UntypedConverter
  {
    public:

      UntypedConverter(CobolBuilderInterface* builder) : pBuilder(builder)
        {
        }

      void  setSourcePositionFrom      ( SgLocatedNode* toNode, SgLocatedNode* fromNode );
      void  setSourcePositionIncluding ( SgLocatedNode* toNode, SgLocatedNode* startNode, SgLocatedNode* endNode );

      void  setSourcePositionUnknown   ( SgLocatedNode* locatedNode );

      SgScopeStatement* initialize_global_scope ( SgSourceFile* file );

      void buildProcedureSupport (SgUntypedFunctionDeclaration* ut_function,
                                  SgProcedureHeaderStatement* procedureDeclaration, SgScopeStatement* scope);

      void convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                        SgLabelSymbol::label_type_enum label_type = SgLabelSymbol::e_start_label_type, SgScopeStatement* label_scope=NULL);

      SgInitializedName* convertSgUntypedInitializedName (SgUntypedInitializedName* ut_name, SgType* sg_type, SgInitializer* sg_init);

      void convertFunctionPrefix (SgUntypedTokenList* prefix_list, SgFunctionDeclaration* function_decl);

      void setDeclarationModifiers (SgDeclarationStatement* decl, SgUntypedTokenList* mod_list);


      SgType*   convertSgUntypedType        (SgUntypedType* ut_type,                SgScopeStatement* scope);
      SgGlobal* convertSgUntypedGlobalScope (SgUntypedGlobalScope* ut_scope,        SgScopeStatement* scope);

      SgModuleStatement*          convertSgUntypedModuleDeclaration        (SgUntypedModuleDeclaration*        ut_decl, SgScopeStatement* scope);
      SgProgramHeaderStatement*   convertSgUntypedProgramHeaderDeclaration (SgUntypedProgramHeaderDeclaration* ut_decl, SgScopeStatement* scope);
      SgProcedureHeaderStatement* convertSgUntypedSubroutineDeclaration    (SgUntypedSubroutineDeclaration*    ut_decl, SgScopeStatement* scope);
      SgProcedureHeaderStatement* convertSgUntypedFunctionDeclaration      (SgUntypedFunctionDeclaration*      ut_decl, SgScopeStatement* scope);
      void                        convertSgUntypedFunctionDeclarationList  (SgUntypedFunctionDeclarationList*  ut_list, SgScopeStatement* scope);

   // Declaration statements
   //
      SgDeclarationStatement* convertSgUntypedNameListDeclaration (SgUntypedNameListDeclaration* ut_decl, SgScopeStatement* scope);
      SgImplicitStatement*    convertSgUntypedImplicitDeclaration (SgUntypedImplicitDeclaration* ut_decl, SgScopeStatement* scope);
      SgVariableDeclaration*  convertSgUntypedVariableDeclaration (SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope);

   // Executable statements
   //
      SgExprStatement* convertSgUntypedAssignmentStatement (SgUntypedAssignmentStatement* ut_stmt, SgExpressionPtrList& children, SgScopeStatement* scope);
      SgStatement*     convertSgUntypedExpressionStatement (SgUntypedExpressionStatement* ut_stmt, SgExpressionPtrList& children, SgScopeStatement* scope);
      SgStatement*     convertSgUntypedOtherStatement      (SgUntypedOtherStatement* ut_stmt, SgScopeStatement* scope);

   // Expressions
   //
      SgExpression* convertSgUntypedExpression      (SgUntypedExpression* ut_expr, SgExpressionPtrList& children, SgScopeStatement* scope);

      SgValueExp*   convertSgUntypedValueExpression (SgUntypedValueExpression* ut_expr);
      SgUnaryOp*    convertSgUntypedUnaryOperator   (SgUntypedUnaryOperator* untyped_operator, SgExpression* expr);
      SgBinaryOp*   convertSgUntypedBinaryOperator  (SgUntypedBinaryOperator* untyped_operator, SgExpression* lhs, SgExpression* rhs);

    private:
      CobolBuilderInterface* pBuilder;  // pointer to controlling builder class; DO NOT delete/free
  };

} // namespace Cobol
} // namespace Untyped

// endif for UNTYPED_COBOL_CONVERTER_H
#endif

