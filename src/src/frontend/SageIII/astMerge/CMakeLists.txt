add_library(astMerge OBJECT
  fixupTraversal.C merge.C link-function.C link-variable.C link-class.C link-namespace.C delete.C share.C load.C)
add_dependencies(astMerge rosetta_generated)


########### install files ###############

install(FILES
  merge.h fixupTraversal.h link.h delete.h share.h load.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
