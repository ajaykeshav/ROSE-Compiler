#ifndef RTEDSYM_H
#define RTEDSYM_H

#include "rose.h"

enum SourceFileType
{
  ftUnknown = 0,
  ftHeader  = 1,
  ftCbase   = 2,
  ftClang   = 4  | ftCbase,  // C
  ftCxx     = 8  | ftCbase,  // C++
  ftUPC     = 16 | ftCbase   // UPC
};

struct RtedSymbols
{
  SgFunctionSymbol*   roseCreateArray;
  SgFunctionSymbol*   roseAllocMem;
  SgFunctionSymbol*   roseAccessArray;
  SgFunctionSymbol*   roseCheckpoint;
  SgFunctionSymbol*   roseFunctionCall;
  SgFunctionSymbol*   roseAssertFunctionSignature;
  SgFunctionSymbol*   roseConfirmFunctionSignature;
  SgFunctionSymbol*   roseFreeMemory;
  SgFunctionSymbol*   roseReallocateMemory;
  SgFunctionSymbol*   roseConvertIntToString;
  SgFunctionSymbol*   roseCreateVariable;
  SgFunctionSymbol*   roseCreateObject;
  SgFunctionSymbol*   roseInitVariable;
  SgFunctionSymbol*   roseMovePointer;
  SgFunctionSymbol*   roseAccessVariable;
  SgFunctionSymbol*   roseEnterScope;
  SgFunctionSymbol*   roseExitScope;
  SgFunctionSymbol*   roseIOFunctionCall;
  SgFunctionSymbol*   roseRegisterTypeCall;
  SgFunctionSymbol*   roseCheckIfThisNULL;
  SgFunctionSymbol*   roseAddr;
  SgFunctionSymbol*   roseAddrSh;
  SgFunctionSymbol*   roseClose;

  SgFunctionSymbol*   roseReportViolation;

  // symbols for UPC
  SgFunctionSymbol*   roseUpcExitWorkzone;
  SgFunctionSymbol*   roseUpcEnterWorkzone;
  SgFunctionSymbol*   roseUpcAllInitialize;
  SgFunctionSymbol*   roseUpcBeginExclusive;
  SgFunctionSymbol*   roseUpcEndExclusive;
  SgFunctionSymbol*   roseUpcEnterSharedPtr;
  SgFunctionSymbol*   roseUpcExitSharedPtr;

  SgFunctionSymbol*   roseCxxTransientPtr;
  SgFunctionSymbol*   roseCTransientPtr;
  SgFunctionSymbol*   roseCheckTransientPtr;
  SgFunctionSymbol*   roseCheckForMemoryLeak;

  SgEnumDeclaration*  roseAllocKind;

  SgClassType*        roseTypeDesc;
  SgClassType*        roseAddressDesc;
  SgClassType*        roseSourceInfo;
  SgClassType*        roseScopeGuard;
  SgTypedefType*      size_t_member;

  RtedSymbols()
  : roseCreateArray(NULL),
    roseAllocMem(NULL),
    roseAccessArray(NULL),
    roseCheckpoint(NULL),
    roseFunctionCall(NULL),
    roseAssertFunctionSignature(NULL),
    roseConfirmFunctionSignature(NULL),
    roseFreeMemory(NULL),
    roseReallocateMemory(NULL),
    roseConvertIntToString(NULL),
    roseCreateVariable(NULL),
    roseCreateObject(NULL),
    roseInitVariable(NULL),
    roseMovePointer(NULL),
    roseAccessVariable(NULL),
    roseEnterScope(NULL),
    roseExitScope(NULL),
    roseIOFunctionCall(NULL),
    roseRegisterTypeCall(NULL),
    roseCheckIfThisNULL(NULL),
    roseAddr(NULL),
    roseAddrSh(NULL),
    roseClose(NULL),

    roseReportViolation(NULL),

    roseUpcExitWorkzone(NULL),
    roseUpcEnterWorkzone(NULL),
    roseUpcAllInitialize(NULL),
    roseUpcBeginExclusive(NULL),
    roseUpcEndExclusive(NULL),
    roseUpcEnterSharedPtr(NULL),
    roseUpcExitSharedPtr(NULL),

    roseCxxTransientPtr(NULL),
    roseCTransientPtr(NULL),
    roseCheckTransientPtr(NULL),

    roseAllocKind(NULL),

    roseTypeDesc(NULL),
    roseAddressDesc(NULL),
    roseSourceInfo(NULL),
    roseScopeGuard(NULL),
    size_t_member(NULL)
  {}

  void initialize(SgGlobal& n, SourceFileType sft);

  static const std::string prefix;
};

#endif
