#ifndef DFABSTRACTSOLVER_H
#define DFABSTRACTSOLVER_H

#include "Labeler.h"

namespace CodeThorn 
{
  
struct Lattice;  

/// Defines the abstract solver interface 
class DFAbstractSolver 
{
 public:
  /// runs the solver
  virtual void runSolver() = 0;
  
  /// computes the preinfo at @ref lab / @ref inInfo  
  virtual void computeCombinedPreInfo(Label lab, Lattice& inInfo) = 0;
  
  /// currently only internally used, so not a member of the public interface
  //~ virtual void computePostInfo(Label lab, Lattice& inInfo) = 0;

  /// optional debugging/tracing support
  virtual void setTrace(bool trace) { _traceFlag=trace;}
  virtual bool getTrace() { return _traceFlag; }
 private:
  bool _traceFlag;
};

} // namespace CodeThorn

#endif /* DFABSTRACTSOLVER_H */
