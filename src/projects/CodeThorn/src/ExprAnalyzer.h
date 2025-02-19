#ifndef CT_EXPR_ANALYZER_H
#define CT_EXPR_ANALYZER_H

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <limits.h>
#include <string>
#include <unordered_set>
#include "Labeler.h"
#include "EState.h"
#include "VariableIdMapping.h"
#include "AbstractValue.h"
#include "AstTerm.h"
#include "ProgramLocationsReport.h"
#include "TypeSizeMapping.h"
#include "CodeThornOptions.h"

using namespace std;

namespace CodeThorn {

  class CTAnalysis;
  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  class SingleEvalResult {
  public:
    EState estate;
    CodeThorn::BoolLattice result;
    bool isTop() {return result.isTop();}
    bool isTrue() {return result.isTrue();}
    bool isFalse() {return result.isFalse();}
    bool isBot() {return result.isBot();}
  };
  
  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  class SingleEvalResultConstInt {
  public:
    void init(EState estate, AbstractValue result);
    EState estate;
    AbstractValue result;
    AbstractValue value() {return result;}
    bool isConstInt() {return result.isConstInt();}
    bool isTop() {return result.isTop();}
    bool isTrue() {return result.isTrue();}
    bool isFalse() {return result.isFalse();}
    bool isBot() {return result.isBot();}
  };
  
  enum InterpreterMode { IM_DISABLED, IM_ENABLED };
  // ACCESS_ERROR is null pointer dereference is detected. ACCESS_NON_EXISTING if pointer is lattice bottom element.
  enum MemoryAccessBounds {ACCESS_ERROR,ACCESS_DEFINITELY_NP, ACCESS_DEFINITELY_INSIDE_BOUNDS, ACCESS_POTENTIALLY_OUTSIDE_BOUNDS, ACCESS_DEFINITELY_OUTSIDE_BOUNDS, ACCESS_NON_EXISTING};
  
  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  //#define EXPR_VISITOR
  class ExprAnalyzer {
  public:
    enum EvalMode { MODE_ADDRESS, MODE_VALUE, MODE_EMPTY_STATE };
    ExprAnalyzer();
    void setAnalyzer(CTAnalysis* analyzer);
    //SingleEvalResult eval(SgNode* node,EState estate);
    //! compute abstract lvalue
    list<SingleEvalResultConstInt> evaluateLExpression(SgNode* node,EState estate);
    //! Evaluates an expression using AbstractValue and returns a list
    //! of all evaluation-results.  There can be multiple results if
    //! one of the variables was bound to top and branching constructs
    //! are inside the expression.
    list<SingleEvalResultConstInt> evaluateExpression(SgNode* node,EState estate, EvalMode mode=MODE_VALUE);
    //! uses AbstractValue::getVariableIdMapping()
    AbstractValue evaluateExpressionWithEmptyState(SgExpression* expr);
    void setVariableIdMapping(VariableIdMappingExtended* variableIdMapping);
    void setSkipUnknownFunctionCalls(bool skip);
    bool getSkipUnknownFunctionCalls();
    void setSkipArrayAccesses(bool skip);
    bool getSkipArrayAccesses();
    void setIgnoreUndefinedDereference(bool skip);
    bool getIgnoreUndefinedDereference();
    void setIgnoreFunctionPointers(bool skip);
    bool getIgnoreFunctionPointers();
    void setSVCompFunctionSemantics(bool flag);
    bool getSVCompFunctionSemantics();
    // deprecated
    bool stdFunctionSemantics();
    bool getStdFunctionSemantics();
    void setStdFunctionSemantics(bool flag);

    bool getPrintDetectedViolations();
    void setPrintDetectedViolations(bool flag);
    
    // deprecated (superseded by checkMemoryAccessBounds
    bool accessIsWithinArrayBounds(VariableId arrayVarId,int accessIndex);
    // supersedes accessIsWithinArrayBounds
    enum MemoryAccessBounds checkMemoryAccessBounds(AbstractValue address);
    
    // deprecated
    //VariableId resolveToAbsoluteVariableId(AbstractValue abstrValue) const;
    AbstractValue computeAbstractAddress(SgVarRefExp* varRefExp);

    // record detected errors in programs
    ProgramLocationsReport getProgramLocationsReport(enum AnalysisSelector analysisSelector);

    // deprecated (use getProgramLocationsReport instead)
    ProgramLocationsReport getViolatingLocations(enum AnalysisSelector analysisSelector);

    // record detected errors in programs
    void recordDefinitiveViolatingLocation(enum AnalysisSelector analysisSelector, Label lab);
    void recordPotentialViolatingLocation(enum AnalysisSelector analysisSelector, Label lab);
    std::string analysisSelectorToString(AnalysisSelector sel);
    
    // deprecated
    void recordDefinitiveNullPointerDereferenceLocation(Label lab);
    void recordPotentialNullPointerDereferenceLocation(Label lab);
    void recordDefinitiveOutOfBoundsAccessLocation(Label lab);
    void recordPotentialOutOfBoundsAccessLocation(Label lab);
    void recordDefinitiveUninitializedAccessLocation(Label lab);
    void recordPotentialUninitializedAccessLocation(Label lab);

    bool definitiveErrorDetected();
    bool potentialErrorDetected();

    void setOptionOutputWarnings(bool flag);
    bool getOptionOutputWarnings();

    //! returns true if node is a VarRefExp and sets varId=id, otherwise false and varId=0.
    bool checkIfVariableAndDetermineVarId(SgNode* node,VariableId& varId); // only used by CTAnalysis

    list<SingleEvalResultConstInt> evalFunctionCallArguments(SgFunctionCallExp* funCall, EState estate);
    list<SingleEvalResultConstInt> evalFunctionCall(SgFunctionCallExp* node, EState estate);
    bool isLValueOp(SgNode* node);
    // requires StructureAccessLookup to be initialized.
    bool isMemberVariable(CodeThorn::VariableId varId);
    // checks if value is a null pointer. If it is 0 it records a null pointer violation at provided label.
    // returns true if execution may continue, false if execution definitely does not continue.
    bool checkAndRecordNullPointer(AbstractValue value, Label label);

    enum InterpreterMode getInterpreterMode();
    void setInterpreterMode(enum InterpreterMode);
    std::string getInterpreterModeFileName();
    void setInterpreterModeFileName(std::string);

    // reserves memory location and sets as value 'undef'
    void reserveMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc);
    // reserves and initializes memory location with newValue
    void initializeMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue);
    AbstractValue readFromMemoryLocation(Label lab, const PState* pstate, AbstractValue memLoc);
    void writeToMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue);

    AbstractValue readFromReferenceMemoryLocation(Label lab, const PState* pstate, AbstractValue memLoc);
    void writeToReferenceMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue);
    
    // memory must already be reserved (hence, this function is redundant if reserves is used before)
    void writeUndefToMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc);
    void writeUndefToMemoryLocation(PState* pstate, AbstractValue memLoc);
    //! This function turn a single result into a one-elment list with
    //! this one result.
    static list<SingleEvalResultConstInt> listify(SingleEvalResultConstInt res);

    // utilify functions
    int getMemoryRegionNumElements(CodeThorn::AbstractValue ptrToRegion);
    int getMemoryRegionElementSize(CodeThorn::AbstractValue);

    static void initDiagnostics();
  protected:
    static Sawyer::Message::Facility logger;
    AbstractValue abstractValueFromSgValueExp(SgValueExp* valueExp, EvalMode mode);
    
   
    // evaluation state
#ifdef EXPR_VISITOR
    SingleEvalResultConstInt res;
#endif
    // evaluation functions
    list<SingleEvalResultConstInt> evalConditionalExpr(SgConditionalExp* node, EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evaluateShortCircuitOperators(SgNode* node,EState estate, EvalMode mode=MODE_VALUE);

    list<SingleEvalResultConstInt> evalEqualOp(SgEqualityOp* node,
                                               SingleEvalResultConstInt lhsResult, 
                                               SingleEvalResultConstInt rhsResult,
                                               EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalNotEqualOp(SgNotEqualOp* node,
                                                  SingleEvalResultConstInt lhsResult, 
                                                  SingleEvalResultConstInt rhsResult,
                                                  EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalAndOp(SgAndOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalOrOp(SgOrOp* node,
                                            SingleEvalResultConstInt lhsResult, 
                                            SingleEvalResultConstInt rhsResult,
                                            EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalAddOp(SgAddOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalSubOp(SgSubtractOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalMulOp(SgMultiplyOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalDivOp(SgDivideOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalModOp(SgModOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseAndOp(SgBitAndOp* node,
                                                    SingleEvalResultConstInt lhsResult, 
                                                    SingleEvalResultConstInt rhsResult,
                                                    EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseOrOp(SgBitOrOp* node,
                                                   SingleEvalResultConstInt lhsResult, 
                                                   SingleEvalResultConstInt rhsResult,
                                                   EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseXorOp(SgBitXorOp* node,
                                                    SingleEvalResultConstInt lhsResult, 
                                                    SingleEvalResultConstInt rhsResult,
                                                    EState estate, EvalMode mode=MODE_VALUE);
    
    list<SingleEvalResultConstInt> evalGreaterOrEqualOp(SgGreaterOrEqualOp* node,
                                                        SingleEvalResultConstInt lhsResult, 
                                                        SingleEvalResultConstInt rhsResult,
                                                        EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalGreaterThanOp(SgGreaterThanOp* node,
                                                     SingleEvalResultConstInt lhsResult, 
                                                     SingleEvalResultConstInt rhsResult,
                                                     EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalLessOrEqualOp(SgLessOrEqualOp* node,
                                                     SingleEvalResultConstInt lhsResult, 
                                                     SingleEvalResultConstInt rhsResult,
                                                     EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalLessThanOp(SgLessThanOp* node,
                                                  SingleEvalResultConstInt lhsResult, 
                                                  SingleEvalResultConstInt rhsResult,
                                                  EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseShiftLeftOp(SgLshiftOp* node,
                                                          SingleEvalResultConstInt lhsResult, 
                                                          SingleEvalResultConstInt rhsResult,
                                                          EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseShiftRightOp(SgRshiftOp* node,
                                                           SingleEvalResultConstInt lhsResult, 
                                                           SingleEvalResultConstInt rhsResult,
                                                           EState estate, EvalMode mode=MODE_VALUE);

    list<SingleEvalResultConstInt> evalAssignOp(SgAssignOp* node,
                                                SingleEvalResultConstInt lhsResult,
                                                SingleEvalResultConstInt rhsResult,
                                                EState estate, EvalMode mode);


    list<SingleEvalResultConstInt> evalArrayReferenceOp(SgPntrArrRefExp* node,
                                                        SingleEvalResultConstInt lhsResult, 
                                                        SingleEvalResultConstInt rhsResult,
                                                        EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalCommaOp(SgCommaOpExp* node, 
                                                        SingleEvalResultConstInt lhsResult, 
                                                        SingleEvalResultConstInt rhsResult,
                                                        EState estate, EvalMode mode=MODE_VALUE);

    list<SingleEvalResultConstInt> evalNotOp(SgNotOp* node, 
                                             SingleEvalResultConstInt operandResult, 
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalUnaryMinusOp(SgMinusOp* node, 
                                                    SingleEvalResultConstInt operandResult, 
                                                    EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalArrowOp(SgArrowExp* node,
                                               SingleEvalResultConstInt lhsResult, 
                                               SingleEvalResultConstInt rhsResult,
                                               EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalDotOp(SgDotExp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalCastOp(SgCastExp* node, 
                                              SingleEvalResultConstInt operandResult, 
                                              EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalDereferenceOp(SgPointerDerefExp* node, 
                                                     SingleEvalResultConstInt operandResult, 
                                                     EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> semanticEvalDereferenceOp(SingleEvalResultConstInt operandResult, 
                                                             EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalAddressOfOp(SgAddressOfOp* node, 
                                                   SingleEvalResultConstInt operandResult, 
                                                   EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalPreComputationOp(EState estate, AbstractValue address, AbstractValue change);
    list<SingleEvalResultConstInt> evalPreIncrementOp(SgPlusPlusOp* node, 
						      SingleEvalResultConstInt operandResult, 
						      EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalPostIncrementOp(SgPlusPlusOp* node, 
						       SingleEvalResultConstInt operandResult, 
						       EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalPostComputationOp(EState estate, AbstractValue address, AbstractValue change);
    list<SingleEvalResultConstInt> evalPreDecrementOp(SgMinusMinusOp* node, 
						      SingleEvalResultConstInt operandResult, 
						      EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalPostDecrementOp(SgMinusMinusOp* node, 
						       SingleEvalResultConstInt operandResult, 
						       EState estate, EvalMode mode=MODE_VALUE);

    // dispatch function
    list<SingleEvalResultConstInt> evalMinusMinusOp(SgMinusMinusOp* node, 
                                                    SingleEvalResultConstInt operandResult, 
                                                    EState estate, EvalMode mode=MODE_VALUE);
    // dispatch function
    list<SingleEvalResultConstInt> evalPlusPlusOp(SgPlusPlusOp* node, 
                                                  SingleEvalResultConstInt operandResult, 
                                                  EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalBitwiseComplementOp(SgBitComplementOp* node, 
                                                           SingleEvalResultConstInt operandResult, 
                                                           EState estate, EvalMode mode=MODE_VALUE);
    
    // special case of sizeof operator (operates on types and types of expressions)
    list<SingleEvalResultConstInt> evalSizeofOp(SgSizeOfOp* node, 
                                                EState estate, EvalMode mode=MODE_VALUE);

    list<SingleEvalResultConstInt> evalLValuePntrArrRefExp(SgPntrArrRefExp* node, EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalLValueVarRefExp(SgVarRefExp* node, EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalLValuePointerDerefExp(SgPointerDerefExp* node, EState estate);
    // handles DotExp and ArrowExp
    list<SingleEvalResultConstInt> evalLValueExp(SgNode* node, EState estate, EvalMode mode=MODE_VALUE);

    list<SingleEvalResultConstInt> evalRValueVarRefExp(SgVarRefExp* node, EState estate, EvalMode mode=MODE_VALUE);
    list<SingleEvalResultConstInt> evalValueExp(SgValueExp* node, EState estate, EvalMode mode);
    list<SingleEvalResultConstInt> evalFunctionRefExp(SgFunctionRefExp* node, EState estate, EvalMode mode=MODE_VALUE);
    // supported system functions
    list<SingleEvalResultConstInt> evalFunctionCallMalloc(SgFunctionCallExp* funCall, EState estate);
    list<SingleEvalResultConstInt> evalFunctionCallFree(SgFunctionCallExp* funCall, EState estate);
    list<SingleEvalResultConstInt> evalFunctionCallMemCpy(SgFunctionCallExp* funCall, EState estate);
    list<SingleEvalResultConstInt> evalFunctionCallStrLen(SgFunctionCallExp* funCall, EState estate);

    // supported functions to be executed (interpreter mode)
    list<SingleEvalResultConstInt> execFunctionCallPrintf(SgFunctionCallExp* funCall, EState estate);
    list<SingleEvalResultConstInt> execFunctionCallScanf(SgFunctionCallExp* funCall, EState estate);
    std::string sourceLocationAndNodeToString(Label lab);
  private:
    void printLoggerWarning(EState& estate);
    void initViolatingLocations();
    VariableIdMappingExtended* _variableIdMapping=nullptr;
    std::vector<ProgramLocationsReport> _violatingLocations;
    // Options
    bool _skipSelectedFunctionCalls=false;
    bool _skipArrayAccesses=false;
    bool _stdFunctionSemantics=true;
    bool _svCompFunctionSemantics=false;
    bool _ignoreUndefinedDereference=false;
    bool _ignoreFunctionPointers=false;
    CTAnalysis* _analyzer=nullptr;
    bool _printDetectedViolations=false;
    enum InterpreterMode _interpreterMode=IM_DISABLED;
    std::string _interpreterModeFileName;
    bool _optionOutputWarnings=false;
  public:
  };

  
} // end of namespace CodeThorn

#endif
