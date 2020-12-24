#ifndef ATERM_TO_UNTYPED_COBOL_TRAVERSAL_H
#define ATERM_TO_UNTYPED_COBOL_TRAVERSAL_H

#include "ATermToUntypedTraversal.h"
#include "Cobol_to_ROSE_translation.h"

namespace ATermSupport {

class ATermToUntypedCobolTraversal : public ATermToUntypedTraversal
{
 public:
   ATermToUntypedCobolTraversal(SgSourceFile* source);
   virtual ~ATermToUntypedCobolTraversal();

 public:
// cobol-source-program
ATbool traverse_CobolSourceProgram (ATerm term);

ATbool traverse_Name           (ATerm term, std::string & name);
ATbool traverse_OptProgramSpec (ATerm term, Cobol_ROSE_Translation::ProgramSpec & prog_spec_enum);
ATbool traverse_OptEndProgram  (ATerm term, std::string & name);

#if 0
// 1.2.3 MAIN PROGRAM MODULES
ATbool traverse_MainProgramModule(ATerm term, SgUntypedGlobalScope* global_scope);
ATbool traverse_ProgramBody(ATerm term, SgUntypedFunctionScope** function_scope);
ATbool traverse_NonNestedSubroutineList(ATerm term, SgUntypedScope* scope);
ATbool traverse_SubroutineDefinitionList(ATerm term, SgUntypedFunctionDeclarationList*);

ATbool traverse_DeclarationList(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 2.1 DATA DECLARATIONS
ATbool traverse_DataDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 2.1.1 ITEM DECLARATION
ATbool traverse_ItemDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_ItemTypeDescription(ATerm term, SgUntypedType** type);

// 2.1.1.1 INTEGER TYPE DESCRIPTIONS
ATbool traverse_IntegerItemDescription(ATerm term, SgUntypedType** type);
ATbool traverse_OptIntegerSize(ATerm term, bool* has_size /*TODO - return type */);

// 2.1.1.2 FLOATING TYPE DESCRIPTIONS

ATbool traverse_OptRoundOrTruncate(ATerm term, bool* has_round_or_truncate /*TODO - return type */);

// 2.1.5 ALLOCATION OF DATA OBJECTS
ATbool traverse_OptAllocationSpecifier(ATerm term, bool* has_spec, std::string & spec_string);

// 4.0 STATEMENTS
ATbool traverse_Statement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_StatementList(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_SimpleStatement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_LabelList(ATerm term, std::vector<std::string> & labels);

// 4.1 ASSIGNMENT STATEMENTS
ATbool traverse_AssignmentStatement(ATerm term, std::vector<std::string> & labels, SgUntypedStatementList* stmt_list);

// 5.0 FORMULAS
ATbool traverse_Formula(ATerm term, SgUntypedExpression** expr);

// 5.1 NUMERIC FORMULAS
ATbool traverse_NumericFormula(ATerm term, SgUntypedExpression** expr);

// 5.1.1 INTEGER FORMULAS
ATbool traverse_IntegerFormula (ATerm term, SgUntypedExpression** expr);
ATbool traverse_IntegerPrimary (ATerm term, SgUntypedExpression** expr);
ATbool traverse_OptSign        (ATerm term, Cobol_ROSE_Translation::ExpressionKind & op_enum);
ATbool traverse_IntegerTerm    (ATerm term, SgUntypedExpression** expr);
ATbool traverse_IntegerFactor  (ATerm term, SgUntypedExpression** expr);

// 6.1 VARIABLE AND BLOCK REFERENCES
ATbool traverse_Variable(ATerm term, SgUntypedExpression** var);
ATbool traverse_VariableList(ATerm term, std::vector<SgUntypedExpression*> & vars);

// 8.2.3 OPERATORS
ATbool traverse_MultiplyDivideOrMod(ATerm term, Cobol_ROSE_Translation::ExpressionKind & op_enum, std::string & op_name);
#endif

}; // class ATermToUntypedCobolTraversal
}  // namespace Cobol

#endif
