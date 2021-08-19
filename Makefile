######################################################################################
# Makefile for building: TauHiTaS
# use "make 32BIT=true" to compile for 32 bit system
######################################################################################
####### Compiler, tools and options
# Environment

MKDIR           = mkdir
RM              = rm -f
CP              = cp -p

32BIT           = true
#32BIT           = false

SWIGDIR         = D:/Peter-Paul/Documents/Thuiswerk/Programmatuur/swigwin-4.0.1

ifeq ($(32BIT), false)  # 64 bit assumed
    BITS        = -m64 -D_LP64
    ARCH        = x86_64
    CND_PLATFORM= MinGW-Windows64
    JAVADIR     = ../../../Java/zulu8.52.0.23-ca-jdk8.0.282-win_x64
    GNUDIR      = C:/Progra~1/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin
else                    # 32 bit assumed
    BITS        = -m32
    ARCH        = x86
    CND_PLATFORM= MinGW-Windows
    JAVADIR     = ../../../Java/zulu8.52.0.23-ca-jdk8.0.282-win_i686
    GNUDIR      = C:/Progra~2/mingw-w64/i686-8.1.0-win32-sjlj-rt_v6-rev0/mingw32/bin
endif

JAVAINC         = -I$(JAVADIR)/include -I$(JAVADIR)/include/win32
CC              = $(GNUDIR)/g++
CXX             = $(GNUDIR)/g++
WINDRES         = $(GNUDIR)/windres
SWIG            = $(SWIGDIR)/swig.exe

# Macros
CND_DLIB_EXT    = dll
CND_CONF        = Debug
CND_DISTDIR     = dist
CND_BUILDDIR    = build

LIBNAME         = TauHitas
JAVAPACKAGE     = tauargus.extern.tauhitas

CSPDIR          = ../CSP/$(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)
CSPLIBS         = -L$(CSPDIR) -lCSPlibCPLEX -lCSPlibXPRESS -lCSPlibSCIP

ifeq ($(32BIT),false)
    CPXDIR      = ../Solvers/Cplex/Cplex125/Windows/64bits
    CPXLIBS     = -L$(CPXDIR) -lcplex125
else
    CPXDIR      = ../Solvers/Cplex/Cplex75
    CPXLIBS     = -L$(CPXDIR)/lib -lcplex75
endif
CPXINC          = -I$(CPXDIR)/include/ilcplex

XPRDIR          = ../Solvers/XPress/XPress_28/$(ARCH)
XPRINC          = -I$(XPRDIR)
XPRLIBS         = -L$(XPRDIR) -lxprl -lxprs

DIRLPS          = ../Solvers/scip-3.1.1
DIRSOPLEX       = ../Solvers/soplex-2.0.1
SCIPINC         = -I$(DIRLPS)/src -I$(DIRSOPLEX)/src
SOPLEXLIB       = soplex-2.0.1.mingw.$(ARCH).gnu.opt
NLPILIB         = nlpi.cppad-3.1.1.mingw.$(ARCH).gnu.opt
SCIPLIB         = scip-3.1.1.mingw.$(ARCH).gnu.opt
OBJSCIPLIB      = objscip-3.1.1.mingw.$(ARCH).gnu.opt
LPISPXLIB       = lpispx-3.1.1.mingw.$(ARCH).gnu.opt
SCIPLIBS        = -L$(DIRLPS)/lib -L$(DIRSOPLEX)/lib -l$(OBJSCIPLIB) -l$(SCIPLIB) -l$(NLPILIB) -l$(LPISPXLIB) -l$(SOPLEXLIB)

# Object Directory
OBJECTDIR       = $(CND_BUILDDIR)/$(CND_CONF)/$(CND_PLATFORM)

# Object Files
OBJECTFILES = \
    $(OBJECTDIR)/src/ALList.o \
    $(OBJECTDIR)/src/AMiscFunc.o \
    $(OBJECTDIR)/src/AMyLoadProb.o \
    $(OBJECTDIR)/src/ATabs.o \
    $(OBJECTDIR)/src/Adefs.o \
    $(OBJECTDIR)/src/Ahier.o \
    $(OBJECTDIR)/src/Amyio.o \
    $(OBJECTDIR)/src/HiTaSCtrl.o \
    $(OBJECTDIR)/src/HiTaSCtrl_wrap.o \
    $(OBJECTDIR)/src/WrapCSP.o

# Link Libraries and Options
LDLIBSOPTIONS   = $(CSPLIBS) $(CPXLIBS) $(XPRLIBS) $(SCIPLIBS) $(CND_BUILDDIR)/$(CND_CONF)/$(CND_PLATFORM)/src/Versioninfo.o
# CC Compiler Flags
SFLAGS          = -c++ -I./src -java -package $(JAVAPACKAGE) -outdir $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)
#CXXFLAGS        = -ggdb -DSECBOUNDS $(BITS) -fPIC -malign-double -std=c++11 -Wall
CXXFLAGS        = -ggdb -g -Og -DSECBOUNDS $(BITS) -fPIC -std=c++11 -Wall
LDFLAGS         = $(CXXFLAGS) -Wl,--subsystem,windows -Wl,--kill-at -shared 

.PHONY: all clean

all:
	$(MKDIR) -p $(OBJECTDIR)/src
	$(MKDIR) -p $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)

	$(WINDRES) ./src/Versioninfo.rc $(CND_BUILDDIR)/$(CND_CONF)/$(CND_PLATFORM)/src/Versioninfo.o
	$(SWIG) $(SFLAGS) -o ./src/HiTaSCtrl_wrap.cpp hitasctrl.swg
	$(CXX) -c $(CXXFLAGS) $(CPXINC) $(XPRINC) $(SCIPINC) $(JAVAINC) -o $(OBJECTDIR)/src/ALList.o src/ALList.cpp
	$(CXX) -c $(CXXFLAGS) $(CPXINC) $(XPRINC) $(SCIPINC) $(JAVAINC) -o $(OBJECTDIR)/src/AMiscFunc.o src/AMiscFunc.cpp
	$(CXX) -c $(CXXFLAGS) $(CPXINC) $(XPRINC) $(SCIPINC) $(JAVAINC) -o $(OBJECTDIR)/src/AMyLoadProb.o src/AMyLoadProb.cpp
	$(CXX) -c $(CXXFLAGS) $(CPXINC) $(XPRINC) $(SCIPINC) $(JAVAINC) -o $(OBJECTDIR)/src/ATabs.o src/ATabs.cpp
	$(CXX) -c $(CXXFLAGS) $(CPXINC) $(XPRINC) $(SCIPINC) $(JAVAINC) -o $(OBJECTDIR)/src/Adefs.o src/Adefs.cpp
	$(CXX) -c $(CXXFLAGS) $(CPXINC) $(XPRINC) $(SCIPINC) $(JAVAINC) -o $(OBJECTDIR)/src/Ahier.o src/Ahier.cpp
	$(CXX) -c $(CXXFLAGS) $(CPXINC) $(XPRINC) $(SCIPINC) $(JAVAINC) -o $(OBJECTDIR)/src/Amyio.o src/Amyio.cpp
	$(CXX) -c $(CXXFLAGS) $(CPXINC) $(XPRINC) $(SCIPINC) $(JAVAINC) -o $(OBJECTDIR)/src/HiTaSCtrl.o src/HiTaSCtrl.cpp
	$(CXX) -c $(CXXFLAGS) -Wno-unused-function $(CPXINC) $(XPRINC) $(SCIPINC) $(JAVAINC) -o $(OBJECTDIR)/src/HiTaSCtrl_wrap.o src/HiTaSCtrl_wrap.cpp
	$(CXX) -c $(CXXFLAGS) $(CPXINC) $(XPRINC) $(SCIPINC) $(JAVAINC) -o $(OBJECTDIR)/src/WrapCSP.o src/WrapCSP.cpp
	$(CXX) $(LDFLAGS) -o $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)/libtauhitas.$(CND_DLIB_EXT) $(OBJECTFILES) $(LDLIBSOPTIONS) 

# Copy .dll and .java files to tau-argus development directory
	$(CP) $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)/libtauhitas.$(CND_DLIB_EXT) ../tauargus/$(LIBNAME).dll
	$(CP) $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)/*.java ../tauargus/src/tauargus/extern/tauhitas

clean:
	$(RM) -r $(CND_BUILDDIR)/$(CND_CONF)
	$(RM) $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)/*.$(CND_DLIB_EXT)