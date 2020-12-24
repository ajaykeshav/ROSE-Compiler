
#ifndef _ADA_TO_ROSE
#define _ADA_TO_ROSE 1

// not sure why the policy checker prevents this
//~ #include "sage3basic.h"

#include <map>
#include <vector>
#include <utility>
#include <sstream>

#include "Diagnostics.h"
#include "sageGeneric.h"

#include "a_nodes.h"

namespace Ada_ROSE_Translation
{
static constexpr bool FAIL_ON_ERROR = false;

//
// logging

extern Sawyer::Message::Facility mlog;


//
// entry point to conversion routine

/// converts all nodes reachable through the units in \ref head_nodes to ROSE
/// \param head_nodes  entry point to ASIS
/// \param file        the ROSE root for the translation unit
void convertAsisToROSE(Nodes_Struct& head_nodes, SgSourceFile* file);



//
// node mapping accessors, storage, and retrieval
// of globally visible elements

/// returns a mapping from an Asis Element_ID to an Asis struct
std::map<int, Element_Struct*>& elemMap();

/// returns a mapping from an Asis Unit_ID to an Asis struct
/// \todo currently this is the same as elementMap, but the Asis mapping
///       should be split into different maps to avoid pointer casts
/// \note seems not needed
// std::map<int, Element_Struct*>& unitMap();

template <class KeyType, class SageNode>
using map_t = std::map<KeyType, SageNode>;

struct AdaIdentifier : std::string
{
  typedef std::string base;

  AdaIdentifier()                                = default;
  AdaIdentifier(const AdaIdentifier&)            = default;
  AdaIdentifier(AdaIdentifier&&)                 = default;
  AdaIdentifier& operator=(const AdaIdentifier&) = default;
  AdaIdentifier& operator=(AdaIdentifier&&)      = default;

  AdaIdentifier(const std::string& rep)
  : std::string(boost::to_upper_copy(rep))
  {}

  AdaIdentifier(const char* rep)
  : AdaIdentifier(std::string(rep))
  {}
};

/// returns a mapping from Unit_ID to constructed root node in AST
//~ map_t<int, SgDeclarationStatement*>& asisUnits();

/// returns a mapping from Element_ID to SgInitializedName
map_t<int, SgInitializedName*>& asisVars();

/// returns a mapping from Element_ID to Exception declaration
/// \todo revise with representation of Exceptions
map_t<int, SgInitializedName*>& asisExcps();

/// returns a mapping from Declaration_ID to SgDeclarationStatement
map_t<int, SgDeclarationStatement*>& asisDecls();

/// returns a mapping from Element_ID to ROSE type declaration
map_t<int, SgDeclarationStatement*>& asisTypes();

/// returns a mapping from string to builtin type nodes
map_t<AdaIdentifier, SgType*>& adaTypes();


//
// auxiliary functions and types


/// \brief resolves all goto statements to labels
///        at the end of procedures or functions.
struct LabelAndLoopManager
{
    LabelAndLoopManager() = default;

    /// patch gotos with target (a label statement)
    ///   at the end of a procudure / function.
    ~LabelAndLoopManager();

    /// records a new labeled statement \ref lblstmt with key \ref id.
    void label(Element_ID id, SgLabelStatement& lblstmt);

    /// records a new goto statement \ref gotostmt with label key \ref id.
    void gotojmp(Element_ID id, SgGotoStatement& gotostmt);

    /// returns a mapping from an Element_ID to a loop statement
    map_t<int, SgStatement*>& asisLoops() { return loops; }

  private:
    typedef std::map<Element_ID, SgLabelStatement*>               LabelContainer;
    typedef std::vector<std::pair<SgGotoStatement*, Element_ID> > GotoContainer;
    typedef map_t<int, SgStatement*>                              LoopMap;

    LabelContainer labels;
    GotoContainer  gotos;
    LoopMap        loops;

    LabelAndLoopManager(const LabelAndLoopManager&)            = delete;
    LabelAndLoopManager(LabelAndLoopManager&&)                 = delete;
    LabelAndLoopManager& operator=(const LabelAndLoopManager&) = delete;
    LabelAndLoopManager& operator=(LabelAndLoopManager&&)      = delete;
};




/// The context class for translation from Asis to ROSE
///   containts context that is passed top-down
struct AstContext
{
    AstContext()                             = default;
    AstContext(AstContext&&)                 = default;
    AstContext& operator=(AstContext&&)      = default;
    AstContext(const AstContext&)            = default;
    AstContext& operator=(const AstContext&) = default;

    /// returns the current scope
    SgScopeStatement& scope()  const { return SG_DEREF(the_scope); }

    /// returns the current label manager
    LabelAndLoopManager& labelsAndLoops() const { return SG_DEREF(all_labels_loops); }

    /// returns the source file name
    /// \note the Asis source names do not always match the true source file name
    ///       e.g., loop_exit.adb contains a top level function Compute, and the Asis
    ///             nodes under Compute report Compute.adb as the source file.
    const std::string& sourceFileName() const { return SG_DEREF(unit_file_name); }

    /// sets scope without parent check (no-parent-check)
    ///   e.g., when the parent node is built after the scope \ref s (e.g., if statements)
    /// \note the passed object needs to survive the lifetime of the return AstContext
    AstContext scope_npc(SgScopeStatement& s) const;

    /// sets scope and checks that the parent of \ref s is set properly
    /// \note the passed object needs to survive the lifetime of the return AstContext
    AstContext scope(SgScopeStatement& s) const;

    /// sets a new label manager
    /// \note the passed object needs to survive the lifetime of the return AstContext
    AstContext labelsAndLoops(LabelAndLoopManager& lm) const;

    /// unit file name
    /// \note the passed object needs to survive the lifetime of the return AstContext
    AstContext sourceFileName(std::string& file) const;

  private:
    SgScopeStatement*    the_scope;
    LabelAndLoopManager* all_labels_loops;
    const std::string*   unit_file_name;
};


/// functor to create elements that are added to the current scope
struct ElemCreator
{
    explicit
    ElemCreator(AstContext astctx, bool privateItems = false)
    : ctx(astctx), privateElems(privateItems)
    {}

    void operator()(Element_Struct& elem);

  private:
    AstContext ctx;
    bool       privateElems;

    ElemCreator() = delete;
};

/// attaches the source location information from \ref elem to
///   the AST node \ref n.
void attachSourceLocation(SgLocatedNode& n, Element_Struct& elem, AstContext ctx);

/// logs that an asis element kind \ref kind has been explored
/// \param kind a C-string naming the Asis kind
/// \param primaryHandler true if this is the primary handler
void logKind(const char* kind, bool primaryHandler = true);

/// non-tracing alternative
//~ static inline
//~ void logKind(const char*, bool = false) {}

/// anonymous namespace for auxiliary templates and functions
namespace
{
  /// converts a value of type V to a value of type U via streaming
  /// \tparam  V input value type
  /// \tparam  U return value type
  /// \param   val the value to be converted
  /// \returns \ref val converted to type \ref U
  template <class U, class V>
  inline
  U conv(const V& val)
  {
    U                 res;
    std::stringstream buf;

    buf << val;
    buf >> res;

    return res;
  }

  /// upcasts an object of type Derived to an object of type Base
  /// \note useful mainly in the context of overloaded functions
  template <class Base, class Derived>
  inline
  Base& as(Derived& obj)
  {
    Base& res = obj;

    return res;
  }

  //
  // loggers

#ifndef USE_SIMPLE_STD_LOGGER

  inline
  auto logTrace() -> decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::TRACE])
  {
    return Ada_ROSE_Translation::mlog[Sawyer::Message::TRACE];
  }

  inline
  auto logInfo() -> decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::INFO])
  {
    return Ada_ROSE_Translation::mlog[Sawyer::Message::INFO];
  }

  inline
  auto logWarn() -> decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::WARN])
  {
    return Ada_ROSE_Translation::mlog[Sawyer::Message::WARN];
  }

  inline
  auto logError() -> decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::ERROR])
  {
    return Ada_ROSE_Translation::mlog[Sawyer::Message::ERROR];
  }

  inline
  auto logFatal() -> decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::FATAL])
  {
    return Ada_ROSE_Translation::mlog[Sawyer::Message::FATAL];
  }

#else /* USE_SIMPLE_STD_LOGGER */

  inline
  std::ostream& logTrace()
  {
    return std::cerr << "[TRACE] ";
  }

  inline
  std::ostream logInfo()
  {
    return std::cerr << "[INFO] ";
  }

  inline
  std::ostream& logWarn()
  {
    return std::cerr << "[WARN] ";
  }

  inline
  std::ostream& logError()
  {
    return std::cerr << "[ERROR] ";
  }

  inline
  std::ostream& logFatal()
  {
    return std::cerr << "[FATAL] ";
  }

  void logInit() {}
#endif /* USE_SIMPLE_STD_LOGGER */

  /// records a node (value) \ref val with key \ref key in map \ref m.
  /// \pre key is not in the map yet
  template <class KeyT, class DclT, class ValT>
  inline
  void
  recordNode(map_t<KeyT, DclT*>& m, KeyT key, ValT& val)
  {
    ROSE_ASSERT(m.find(key) == m.end());

    m[key] = &val;
  }

  /// records the first mapping that appears in the translation
  /// secondary mappings are ignored, but do not trigger an error.
  /// \note use for non-defining/defining decls and
  ///       other nodes that do not have a single defining mapping.
  template <class KeyT, class DclT, class ValT>
  inline
  void
  recordNonUniqueNode(map_t<KeyT, DclT*>& m, KeyT key, ValT& val)
  {
    if (m.find(key) != m.end()) return;

    recordNode(m, key, val);
  }

  /// retrieves a node from map \ref m with key \ref key.
  template <class KeyT, class DclT>
  inline
  DclT&
  lookupNode(const map_t<KeyT, DclT*>& m, KeyT key)
  {
    typename map_t<KeyT, DclT*>::const_iterator pos = m.find(key);

    ROSE_ASSERT(pos != m.end());
    return *(pos->second);
  }

  /// retrieves a node from map \ref m with key \ref key, under the
  ///   assumption that it is of real type TgtT.
  template <class TgtT, class KeyT, class DclT>
  inline
  TgtT&
  lookupNodeAs(const map_t<KeyT, DclT*>& m, KeyT key)
  {
    DclT& node = lookupNode(m, key);

    return SG_ASSERT_TYPE(TgtT, node);
  }

  /// retrieves a node from map \ref m with key \ref key if key exists.
  /// returns nullptr otherwise.
  template <class KeyT, class DclT>
  inline
  DclT*
  findNode(const map_t<KeyT, DclT*>& m, KeyT key)
  {
    typename map_t<KeyT, DclT*>::const_iterator pos = m.find(key);

    if (pos == m.end())
      return nullptr;

    return pos->second;
  }

  /// \private
  /// base case when a declaration is not in the map
  template <class KeyT, class DclT>
  inline
  DclT*
  findFirst(const map_t<KeyT, DclT*>&)
  {
    return nullptr;
  }

  /// tries one or more keys to find a declaration from map \ref m
  /// returns nullptr if none of the keys can be found.
  template <class KeyT, class DclT, class Key0T, class... KeysT>
  inline
  DclT*
  findFirst(const map_t<KeyT, DclT*>& m, Key0T key0, KeysT... keys)
  {
    DclT* dcl = findNode(m, key0);

    return dcl ? dcl : findFirst(m, keys...);
  }


  //
  // retrieve from ASIS map

  /// retrieves data from the big Asis map
  /// returns a nullptr if the element is not in the map.
  template <class ElemT>
  inline
  ElemT*
  retrieveAsOpt(std::map<int, Element_Struct*>& map, int key)
  {
    //~ logInfo() << "key: " << key << std::endl;
    std::map<int, Element_Struct*>::iterator pos = map.find(key);

    if (pos != map.end())
      return reinterpret_cast<ElemT*>((*pos).second);

    return nullptr;
  }

  /// retrieves data from the big Asis map
  template <class ElemT>
  inline
  ElemT&
  retrieveAs(std::map<int, Element_Struct*>& map, int key)
  {
    return SG_DEREF(retrieveAsOpt<ElemT>(map, key));
  }

  /// A range abstraction for a contiguous sequence
  template <class T>
  struct Range : std::pair<T, T>
  {
    Range(T lhs, T rhs)
    : std::pair<T, T>(lhs, rhs)
    {}

    bool empty() const { return this->first == this->second; }
    int  size()  const { return this->second - this->first; }
  };

  /// A range of Asis Units
  struct UnitIdRange : Range<Unit_ID_Ptr>
  {
    typedef Unit_Struct value_type;

    UnitIdRange(Unit_ID_Ptr lhs, Unit_ID_Ptr rhs)
    : Range<Unit_ID_Ptr>(lhs, rhs)
    {}
  };

  /// A range of Asis Elements
  struct ElemIdRange : Range<Element_ID_Ptr>
  {
    typedef Element_Struct value_type;

    ElemIdRange(Element_ID_Ptr lhs, Element_ID_Ptr rhs)
    : Range<Element_ID_Ptr>(lhs, rhs)
    {}
  };

  /// Type mapping for range element types
  template <class T>
  struct range_types {};

  template <>
  struct range_types<Element_ID_List>
  {
    typedef ElemIdRange type;
  };

  template <>
  struct range_types<Unit_ID_Array_Struct>
  {
    typedef UnitIdRange type;
  };


  /// traverses an Asis linked list and calls \ref functor
  ///   for each element in the range [\ref first, \ref limit).
  ///   e.g., functor(*first)
  /// \returns a copy of the functor
  template <class P, class FnT>
  inline
  FnT traverse(P* first, P* limit, FnT functor)
  {
    while (first != limit)
    {
      functor(*first);

      first = first->Next;
    }

    return functor;
  }


  /// traverses a list of pointers to Elements (or Units) in the range [\ref first, \ref limit),
  ///   looks up the associated Asis struct, and passes it as argument to \ref func.
  ///   e.g., func(*retrieveAs<Element_struct>(map, *first))
  /// \tparam ElemT    the type of the element (Unit or Element)
  /// \tparam PtrT     pointer to elements
  /// \tparam AsisMapT the map type
  /// \tparam FnT      the functor
  /// \returns a copy of \ref func after all elements have been traversed
  /// \todo split the map into two, one for elements, one for units, in order
  ///       to eliminate the need for casting.
  template <class ElemT, class PtrT, class AsisMapT, class FnT>
  inline
  FnT
  traverseIDs(PtrT first, PtrT limit, AsisMapT& map, FnT func)
  {
    while (first != limit)
    {
      ElemT* el = retrieveAsOpt<ElemT>(map, *first);

      if (el)
      {
        func(*el);
      }
      else
      {
        logWarn() << "asis-element of type " << typeid(ElemT).name()
                  << " not available -- asismap[" << *first << "]=nullptr"
                  << std::endl;
      }

      //~ func(retrieveAs<ElemT>(map, *first));
      ++first;
    }

    return func;
  }

  /// traverses all IDs in the range \ref range and calls functor with the associated
  ///   struct of each element.
  /// \returns a copy of \ref func after all elements have been traversed
  template <class Range, class AsisMapT, class FnT>
  inline
  FnT
  traverseIDs(Range range, AsisMapT& map, FnT functor)
  {
    return traverseIDs<typename Range::value_type>(range.first, range.second, map, functor);
  }

  /// creates a range for a contiguous sequence of IDs
  template <class Lst>
  inline
  typename range_types<Lst>::type
  idRange(Lst lst)
  {
    typedef typename range_types<Lst>::type RangeType;

    return RangeType(lst.IDs, lst.IDs + lst.Length);
  }


  /// \brief tests whether the link in an Asis node is valid (set)
  ///        or incomplete.
  /// \note  function should become obsolete eventually.
  inline
  bool isInvalidId(int id) { return id == -1; }

  inline
  bool isValidId(int id) { return !isInvalidId(id); }

} // anonymous


}

#endif /* _ADA_TO_ROSE */
