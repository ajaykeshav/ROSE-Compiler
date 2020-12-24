#include <string>

/* Use the same header file to declare these variables as is used to reference them so that they will be globally available (and not local). */
#include "rose_paths.h"

/* These paths will be absolute or relative depending on how the configure script is called (called with an absolute or relative path). */
const std::string ROSE_GFORTRAN_PATH          = "/usr/bin/gfortran";
const std::string ROSE_AUTOMAKE_TOP_SRCDIR    = "/home/meltdown1995/roseRoot/rose-compiler/src";
const std::string ROSE_AUTOMAKE_TOP_BUILDDIR  = "/home/meltdown1995/roseRoot/rose-compiler/build";
const std::string ROSE_AUTOMAKE_PREFIX        = "/home/meltdown1995/roseRoot/rose-compiler//install";
const std::string ROSE_AUTOMAKE_DATADIR       = "/home/meltdown1995/roseRoot/rose-compiler//install/share";
const std::string ROSE_AUTOMAKE_BINDIR        = "/home/meltdown1995/roseRoot/rose-compiler//install/bin";
const std::string ROSE_AUTOMAKE_INCLUDEDIR    = "/home/meltdown1995/roseRoot/rose-compiler//install/include";
const std::string ROSE_AUTOMAKE_INFODIR       = "/home/meltdown1995/roseRoot/rose-compiler//install/share/info";
const std::string ROSE_AUTOMAKE_LIBDIR        = "/home/meltdown1995/roseRoot/rose-compiler//install/lib";
const std::string ROSE_AUTOMAKE_LIBEXECDIR    = "/home/meltdown1995/roseRoot/rose-compiler//install/libexec";
const std::string ROSE_AUTOMAKE_LOCALSTATEDIR = "/home/meltdown1995/roseRoot/rose-compiler//install/var";
const std::string ROSE_AUTOMAKE_MANDIR        = "/home/meltdown1995/roseRoot/rose-compiler//install/share/man";

/* This will always be an absolute path, while paths above are dependent on how the configure script is called (called with an absolute or relative path). */
const std::string ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR = "/home/meltdown1995/roseRoot/rose-compiler/src";

/* Additional interesting data to provide. */
const std::string ROSE_CONFIGURE_DATE     = "Wednesday December 23 22:42:41 2020";
const std::string ROSE_AUTOMAKE_BUILD_OS  = "linux-gnu";
const std::string ROSE_AUTOMAKE_BUILD_CPU = "x86_64";
const std::string ROSE_OFP_VERSION_STRING = "20200819-JDK8";

const unsigned long ROSE_NUMERIC_VERSION = 11013ul;
