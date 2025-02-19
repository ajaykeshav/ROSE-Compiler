
#ifndef CTX_TRANSFER_H
#define CTX_TRANSFER_H 1

/// \author Peter Pirkelbauer

#include <iterator>
#include <algorithm>

#include <sage3basic.h>
#include <sageGeneric.h>
#include <Labeler.h>

#include "CtxLattice.h"

namespace CodeThorn
{

//
// auxiliary functions

namespace
{
  inline
  bool isCallContextModifier(Labeler& labeler, Label lbl)
  {
    return (  labeler.isFunctionCallLabel(lbl)
           || labeler.isFunctionCallReturnLabel(lbl)
           );
  }

  //
  // auxiliary functors

  /// calls transfer on the DfItem (data-flow object).
  /// \tparam DfItem either a data-flow node (source) or a data-flow edge
  template <class DfItem>
  struct SubTransfer
  {
      SubTransfer(DFTransferFunctions& comptrans, DfItem dfitem)
      : tffun(comptrans), item(dfitem)
      {}

      template <class Key>
      void operator()(std::pair<const Key, Lattice*>& p) const
      {
        tffun.transfer(item, SG_DEREF(p.second));
      }

    private:
      DFTransferFunctions& tffun;
      DfItem               item;
  };

  template <class TfObject>
  inline
  SubTransfer<TfObject>
  subTransfer(DFTransferFunctions& comptrans, TfObject tfobj)
  {
    return SubTransfer<TfObject>(comptrans, tfobj);
  }
}


template <class CallContext>
struct CtxAnalysis;

//
// implements call context transfers on top of underlying components

namespace
{
  template <class CallContext>
  CtxLattice<CallContext>&
  mkCtxLattice(CtxAnalysis<CallContext>& ctxanalysis, const CtxLattice<CallContext>& init)
  {
    return SG_DEREF(cloneLattice(ctxanalysis.factory(), init));
  }

  template <class CallContext>
  CtxLattice<CallContext>&
  mkCtxLattice(CtxAnalysis<CallContext>& ctxanalysis, DFTransferFunctions& component)
  {
    CtxLattice<CallContext>& elem = SG_DEREF(ctxanalysis.factory().create());
    Lattice*                 sub  = elem.componentFactory().create();

    ROSE_ASSERT(elem.isBot() && sub->isBot());
    component.initializeExtremalValue(*sub);
    elem[CallContext()] = sub;
    ROSE_ASSERT(!elem.isBot() && !sub->isBot());
    return elem;
  }
}

/// Transfer functions for context-sensitive dataflow analysis.
///
/// \tparam  CallContext an implementation of a CallContext
template <class CallContext>
struct CtxTransfer : DFTransferFunctions
{
    typedef DFTransferFunctions     base;
    typedef CtxLattice<CallContext> ctx_lattice_t;

    /// Initializes a new transfer object.
    /// \param comptrans   the component transfer functions (e.g., for reaching
    ///                    definitions).
    /// \param ctxanalysis the context sensitive analysis class.
    /// \param init        a prototype lattice to initialize extremal values
    CtxTransfer(DFTransferFunctions& comptrans, CtxAnalysis<CallContext>& ctxanalysis, const ctx_lattice_t& init)
    : base(), component(comptrans), analysis(ctxanalysis), initialElement(&mkCtxLattice(analysis, init))
    {}

    /// Initializes a new transfer object.
    /// \param comptrans   the component transfer functions (e.g., for reaching
    ///                    definitions).
    /// \param ctxanalysis the context sensitive analysis class.
    CtxTransfer(DFTransferFunctions& comptrans, CtxAnalysis<CallContext>& ctxanalysis)
    : base(), component(comptrans), analysis(ctxanalysis), initialElement(&mkCtxLattice(analysis, component))
    {}
    
    ~CtxTransfer()
    {
      delete initialElement;
    }


    /// sets its and its' component's abstraction layer
    void setProgramAbstractionLayer(ProgramAbstractionLayer* pal) ROSE_OVERRIDE
    {
      ROSE_ASSERT(pal);

      base::setProgramAbstractionLayer(pal);
      component.setProgramAbstractionLayer(pal);
    }

    void setPointerAnalysis(PointerAnalysisInterface* pai) ROSE_OVERRIDE
    {
      ROSE_ASSERT(pai);

      base::setPointerAnalysis(pai);
      component.setPointerAnalysis(pai);
    }

    /// actual transfer function.
    /// \details computes call context changes for call and return labels
    /// \note the method does not override
    void transfer(Label lbl, ctx_lattice_t& lat);

    /// handles transfers on edges
    /// \note CURRENTLY ONLY CALLED from post-info processing
    void transfer(Label lbl, Lattice& element) ROSE_OVERRIDE
    {
      ctx_lattice_t& lat = dynamic_cast<ctx_lattice_t&>(element);

      this->transfer(lbl, lat);
      std::for_each(lat.begin(), lat.end(), subTransfer(component, lbl));
    }

    /// this method is invoked from the solver/worklist algorithm and
    ///   calls the component's transfer function for each context.
    ///   It also calls the concrete transfer function (above) to
    ///   handle context changes.
    void transfer(Edge edge, Lattice& element) ROSE_OVERRIDE;

    /// initializes extremal values
    void initializeExtremalValue(Lattice& element) ROSE_OVERRIDE
    {
      ctx_lattice_t& lat = dynamic_cast<ctx_lattice_t&>(element);

      //~ lat.combine(const_cast<ctx_lattice_t&>(*initialElement));
      lat.combine(*initialElement);
    }

    /// overrides behavior from base function for cases where
    ///   root = nullptr, indicating a nested analysis that must not
    ///   reinitialize global state.
    ctx_lattice_t* initializeGlobalVariables(SgProject* root) ROSE_OVERRIDE
    {
      if (!root) return cloneLattice(analysis.factory(), *initialElement);

      ctx_lattice_t& elem = dynamic_cast<ctx_lattice_t&>(SG_DEREF(base::initializeGlobalVariables(root)));
      
      ROSE_ASSERT(elem.size() <= 1);
      
      // \todo remove workaround when CFG has return nodes for function calls
      //       in global variable initialization.
      if (elem.size())
      {
        // replace initial element with lattice from global variable analysis
        delete initialElement;
      
        initialElement = &mkCtxLattice(analysis, elem);
      }

      //~ dbgPrintCtx(std::cerr, SG_DEREF(analysis.getLabeler()), elem);
      return &elem;
    }
    
    DFTransferFunctions& componentTransfer() { return component; }

  private:
    DFTransferFunctions&      component;       ///< The component transfer
    CtxAnalysis<CallContext>& analysis;        ///< The analysis objects
    //~ const ctx_lattice_t&      initialElement;  ///< prototype lattice for extremal value
    ctx_lattice_t*            initialElement;  ///< prototype lattice for extremal value 
    
    CtxTransfer() = delete;
};


//
// implementation

template <class CallContext>
void CtxTransfer<CallContext>::transfer(Label lbl, ctx_lattice_t& lat)
{
  CodeThorn::Labeler& labeler = SG_DEREF(getLabeler());

  // we are only interested in Call context modifications
  if (!isCallContextModifier(labeler, lbl)) return;

  ctx_lattice_t       tmp(lat.componentFactory());

  // mv lattices to tmp, then fill lat
  tmp.swap(lat);

  if (labeler.isFunctionCallLabel(lbl))
  {
    allCallInvoke(tmp, lat, analysis, labeler, lbl);
  }
  else
  {
    allCallReturn(tmp, lat, analysis, labeler, lbl);
  }

  // Sub-lattices in tmp WILL BE DELETED !!!
}


template <class CallContext>
void CtxTransfer<CallContext>::transfer(Edge edge, Lattice& element)
{
  ctx_lattice_t& lat = dynamic_cast<ctx_lattice_t&>(element);

  this->transfer(edge.source(), lat);  
  std::for_each(lat.begin(), lat.end(), subTransfer(component, edge));

  //~ std::cerr << "# tr'd " << getLabeler()->getNode(edge.source())->unparseToString();
  //~ lat.toStream(std::cerr, NULL);
  //~ std::cerr << std::endl;
}


} // namespace CodeThorn

#endif /* CTX_TRANSFER_H */
