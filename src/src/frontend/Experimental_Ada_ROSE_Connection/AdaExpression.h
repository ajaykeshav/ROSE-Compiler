

#ifndef _ADA_EXPRESSION
#define _ADA_EXPRESSION 1

/// Contains functions and classes for converting Ada expressions
///   from Asis to ROSE.


#include "Ada_to_ROSE.h"
#include "a_nodes.h"

namespace Ada_ROSE_Translation
{
  /// returns the ROSE representation of the Asis expression \ref elem
  SgExpression&
  getExpr(Element_Struct& elem, AstContext ctx);

  /// returns the ROSE representation of the Asis Element_ID \ref el
  SgExpression&
  getExprID(Element_ID el, AstContext ctx);

  /// returns the ROSE representation of a valid Asis Element_ID \ref el.
  ///   if el is not a valid exprression, an SgNullExpression is returned
  SgExpression&
  getExprID_opt(Element_ID el, AstContext ctx);

  /// returns a range expression for the element \ref id.
  /// \pre id identifies a Discrete_Range definition
  SgExpression&
  getDiscreteRangeID(Element_ID id, AstContext ctx);

  /// returns an expression for the Asis definition
  /// \pre id identifies a Asis definition
  SgExpression&
  getDefinitionExprID(Element_ID id, AstContext ctx);


  /// creates a sequence of SgExpressions from a sequence of Asis elements
  ///   (eiter expression or definition).
  struct ExprSeqCreator
  {
      explicit
      ExprSeqCreator(AstContext astctx)
      : ctx(astctx), elems()
      {}

      ExprSeqCreator(ExprSeqCreator&&)                 = default;
      ExprSeqCreator& operator=(ExprSeqCreator&&)      = default;

      // \todo the following copying functions should be removed post C++17
      // @{
      ExprSeqCreator(const ExprSeqCreator&)            = default;
      ExprSeqCreator& operator=(const ExprSeqCreator&) = default;
      // @}

      void operator()(Element_Struct& el);

      /// result read-out
      operator std::vector<SgExpression*> () &&
      {
        return std::move(elems);
      }

    private:
      AstContext                 ctx;
      std::vector<SgExpression*> elems;

      ExprSeqCreator() = delete;
  };

  /// creates an SgExprListExp object from a sequence of Asis expressions
  struct ArgListCreator
  {
      explicit
      ArgListCreator(AstContext astctx)
      : ctx(astctx), args()
      {}

      ArgListCreator(ArgListCreator&&)                 = default;
      ArgListCreator& operator=(ArgListCreator&&)      = default;

      // \todo the following copying functions should be removed post C++17
      // @{
      ArgListCreator(const ArgListCreator&)            = default;
      ArgListCreator& operator=(const ArgListCreator&) = default;
      // @}

      void operator()(Element_Struct& elem);

      /// result read-out
      operator SgExprListExp& ()
      {
        namespace sb = SageBuilder;

        return SG_DEREF(sb::buildExprListExp(args));
      }

      operator std::vector<SgExpression*> () &&
      {
        return std::move(args);
      }

    private:
      AstContext                 ctx;
      std::vector<SgExpression*> args;

      ArgListCreator() = delete;
  };

  /// creates a sequence of SgRangeExp objects from a Discrete_Range sequence
  struct RangeListCreator
  {
      explicit
      RangeListCreator(AstContext astctx)
      : ctx(astctx), lst()
      {}

      RangeListCreator(RangeListCreator&&)                 = default;
      RangeListCreator& operator=(RangeListCreator&&)      = default;

      // \todo the following copying functions should be removed post C++17
      // @{
      RangeListCreator(const RangeListCreator&)            = default;
      RangeListCreator& operator=(const RangeListCreator&) = default;
      // @}

      void operator()(Element_Struct& elem);

      /// result read-out
      operator SgRangeExpPtrList () &&
      {
        return std::move(lst);
      }

    private:
      AstContext        ctx;
      SgRangeExpPtrList lst;

      RangeListCreator() = delete;
  };
}


#endif /* _ADA_EXPRESSION */
