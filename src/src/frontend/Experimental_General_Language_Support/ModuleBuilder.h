#ifndef ROSE_MODULE_BUILDER_H_
#define ROSE_MODULE_BUILDER_H_

#include <map>

#define TEMPLATES 0

class SgProject;
class SgSourceFile;

// Need std=c++11
//
#ifndef nullptr
#define nullptr NULL
#endif

namespace Rose {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build files

/** Build a new binary composite object.
 *
 *  A SgBinaryComposite is the top-level node for binary analysis and holds two lists: a list of file headers representing the
 *  various executable containers that have been parsed, and a list of interpretations that organize thos headers into
 *  compatible units.  For instance, a Microsoft Windows executable file will have two headers (DOS and PE) and two
 *  interpretations (one for the DOS code and data and another for the PE code and data).  If dynamic linking is performed,
 *  then even more headers will be present, but their code and data will likely be inserted into one of the existing
 *  interpretations.   An interpretation (SgAsmInterpretation) is analogous to a process.
 *
 *  ASTs normally have only one SgBinaryComposite, but an AST can have more than one if the AST is used to analyze two or more
 *  binaries.  For instance, if we have two related versions of the i586 ELF "login" program and we want to run some analysis
 *  that compares the two programs, then we probably want two SgBinaryComposite nodes in the AST. Each SgBinaryComposite will
 *  have a SgAsmInterpretation to represent the i586 "login" process.  Doing it this way will prevent headers and code from one
 *  version to be confused with headers and code from the other version.
 *
 *  A SgProject node will be created if one has not been created already. The segments/sections from the file are mapped into
 *  process virtual memory in the SgAsmInterpretation but are not disassembled. */

/*
 *  Read a module file "*.rcmp"
 *  Create a SgFile for the module
 *  Create an entry for the module in the moduleNAmeAstMap
 */

#define  SKIP_SYNTAX_CHECK "-rose:skip_syntax_check"

#if TEMPLATES
template <typename T>
#endif
class ModuleBuilder
  {
#if TEMPLATES
     typedef std::map<std::string, T*> ModuleMapType;
#else
     typedef std::map<std::string, SgJovialCompoolStatement*> ModuleMapType;
#endif

     public:
       ModuleBuilder() : loadingModuleState(false), nestedSgFile(0), currentProject(nullptr)
         {
         }

       bool isRoseModuleFile();

       void setCurrentProject(SgProject*);
       SgProject* getCurrentProject();

       void setLoadingModuleState(bool loading_state) { loadingModuleState = loading_state; }
       bool getLoadingModuleState() { return loadingModuleState; }

#if TEMPLATES
       T*   getModule (const std::string &module_name) { ROSE_ASSERT(false); }
       void addMapping(const std::string &module_name, T* module_stmt);
#else
       SgJovialCompoolStatement* getModule(const std::string &module_name);
       void addMapping(const std::string &module_name, SgJovialCompoolStatement* module_stmt);
       std::string getModuleFileSuffix() {return std::string(".rcmp");}
#endif

       std::string find_file_from_inputDirs(const std::string &name);

       void setInputDirs(SgProject*);
 
     private:
       bool loadingModuleState;
       unsigned int nestedSgFile;
       SgProject* currentProject;

       std::vector<std::string> inputDirs;
       ModuleMapType moduleNameMap;

       SgSourceFile*  createSgSourceFile(const std::string &module_name);
       void           clearMap();
       void           dumpMap();
  };


class ModuleBuilderFactory
  {
    public:
#if TEMPLATES
      static ModuleBuilder<SgJovialCompoolStatement>& get_compool_builder();
#else
      static ModuleBuilder& get_compool_builder()
         {
            static ModuleBuilder compool_builder;
            return compool_builder;
         }
#endif
  };


} // namespace Rose

#endif /* ROSE_MODULE_BUILDER_H_ */
