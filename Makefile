######################################################################################
# Makefile for building: TauHiTaS
# use "make 32BIT=true" to compile for 32 bit system
# use "make SOLVER=foo" to compile only for selected LP-solves
#	    where foo is a comma separated list
#	    Allowed solvers: CP=Cplex, XP=Xpress, SC=SCIP
#	    So e.g., "make SOLVER=CP,SC" would only compile for Cplex and SCIP
######################################################################################

####### Compiler, tools and options
# Environment

MKDIR           = mkdir
RM              = rm -f
CP              = cp -p

32BIT           = true  # is the default
#32BIT           = false

#SWIGDIR         = D:/Peter-Paul/Documents/Thuiswerk/Programmatuur/swigwin-4.0.2
SWIGDIR         = D:/Peter-Paul/Documents/Thuiswerk/Programmatuur/swigwin-4.5.0

ifeq ($(32BIT), false)  # 64 bit assumed
    BITS        = -m64 -D_LP64
    ARCH        = x86_64
    CND_PLATFORM= MinGW-Windows64
    #JAVADIR     = ../../../Java/zulu8.52.0.23-ca-jdk8.0.282-win_x64
    JAVADIR     = ../../../Java/bellsoft-jdk21.0.12.1+1-windows-amd64/jdk-21.0.12.1
    GNUDIR      = C:/Progra~1/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin
else                    # 32 bit assumed
    BITS        = -m32
    ARCH        = x86
    CND_PLATFORM= MinGW-Windows
    #JAVADIR     = ../../../Java/zulu8.52.0.23-ca-jdk8.0.282-win_i686
    JAVADIR     = ../../../Java/bellsoft-jdk21.0.12.1+1-windows-i586/jdk-21.0.12.1
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
CSPLIBS         = -L$(CSPDIR)

# Solvers
SOLVER = CP,XP,SC# default is all three
comma:=,
null:=
space:= $(null) #
ifeq (,$(SOLVER)) # if not specified, use defaults
    USEDSOLVERS = CP XP SC
else
    USEDSOLVERS = $(subst $(comma), $(space), $(SOLVER))
endif

# CPLEX
ifeq ($(32BIT),false)
    CPXDIR      = ../Solvers/Cplex/Cplex125/Windows/64bits
    CPXLIBS     = -L$(CPXDIR) -lcplex125
else
#    CPXDIR      = ../Solvers/Cplex/Cplex75
#    CPXLIBS     = -L$(CPXDIR)/lib -lcplex75
    CPXDIR      = ../Solvers/Cplex/Cplex125/Windows/32bits
    CPXLIBS     = -L$(CPXDIR) -lcplex125
endif
CPXINC          = -I$(CPXDIR)/include/ilcplex

#XPRESS
#XPRDIR          = ../Solvers/XPress/XPress_28/$(ARCH)
XPRDIR          = ../Solvers/XPress/XPress_19
XPRINC          = -I$(XPRDIR)
XPRLIBS         = -L$(XPRDIR) -lxprl -lxprs

#SCIP
DIRLPS          = ../Solvers/scip-3.1.1
DIRSOPLEX       = ../Solvers/soplex-2.0.1
SCIPINC         = -I$(DIRLPS)/src -I$(DIRSOPLEX)/src
SOPLEXLIB       = soplex-2.0.1.mingw.$(ARCH).gnu.opt
NLPILIB         = nlpi.cppad-3.1.1.mingw.$(ARCH).gnu.opt
SCIPLIB         = scip-3.1.1.mingw.$(ARCH).gnu.opt
OBJSCIPLIB      = objscip-3.1.1.mingw.$(ARCH).gnu.opt
LPISPXLIB       = lpispx-3.1.1.mingw.$(ARCH).gnu.opt
SCIPLIBS        = -L$(DIRLPS)/lib -L$(DIRSOPLEX)/lib -l$(OBJSCIPLIB) -l$(SCIPLIB) -l$(NLPILIB) -l$(LPISPXLIB) -l$(SOPLEXLIB)

ifneq (,$(findstring CP,$(USEDSOLVERS)))
	CSPLIBS += -lCSPlibCPLEX
	SOLVERSINC += $(CPXINC)
	SOLVERSLIBS += $(CPXLIBS)
	ADDCXX += -DLPCP -DBUILD_CPXSTATIC
endif
ifneq (,$(findstring XP,$(USEDSOLVERS)))
	CSPLIBS += -lCSPlibXPRESS
	SOLVERSINC += $(XPRINC)
	SOLVERSLIBS += $(XPRLIBS)
	ADDCXX += -DLPXP
endif
ifneq (,$(findstring SC,$(USEDSOLVERS)))
	CSPLIBS += -lCSPlibSCIP
	SOLVERSINC += $(SCIPINC)
	SOLVERSLIBS += $(SCIPLIBS)
	ADDCXX += -DLPSC
endif

# Object Directory
OBJECTDIR       = $(CND_BUILDDIR)/$(CND_CONF)/$(CND_PLATFORM)
SOURCES		= $(wildcard src/*.cpp)
OBJECTS		= $(patsubst src/%.cpp, $(OBJECTDIR)/src/%.o, $(SOURCES))
VERSIONOBJ	= $(OBJECTDIR)/src/Versioninfo.o

# Object Files
# OBJECTFILES = \
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
LDLIBSOPTIONS   = $(CSPLIBS) $(SOLVERSLIBS) $(CND_BUILDDIR)/$(CND_CONF)/$(CND_PLATFORM)/src/Versioninfo.o
# CC Compiler Flags
SFLAGS          = -c++ -I./src -java -package $(JAVAPACKAGE) -outdir $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)
#CXXFLAGS        = -ggdb -DSECBOUNDS $(BITS) -fPIC -malign-double -std=c++11 -Wall
CXXFLAGS        = -g -O2 -DSECBOUNDS $(BITS) $(ADDCXX) -fPIC -std=c++11 -Wno-unused-function -Wall -fno-strict-aliasing
#CXXFLAGS        = -ggdb -O0 -DSECBOUNDS $(BITS) -fPIC -std=c++11 -Wall -fno-strict-aliasing
LDFLAGS         = $(CXXFLAGS) -Wl,--subsystem,windows -Wl,--kill-at -shared 

.PHONY: all clean

all:	setup $(VERSIONOBJ) $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)/libtauhitas.$(CND_DLIB_EXT) $(OBJECTFILES) $(LDLIBSOPTIONS) 
# Copy .dll and .java files to tau-argus development directory
	$(CP) $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)/libtauhitas.$(CND_DLIB_EXT) ../tauargus/$(LIBNAME).dll
	$(CP) $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)/*.java ../tauargus/src/tauargus/extern/tauhitas

setup:
	$(MKDIR) -p $(OBJECTDIR)/src
	$(MKDIR) -p $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)
	$(SWIG) $(SFLAGS) -o ./src/HiTaSCtrl_wrap.cpp hitasctrl.swg
	
$(VERSIONOBJ): ./src/Versioninfo.rc
	$(WINDRES) $< $@
	
$(OBJECTDIR)/src/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $(SOLVERSINC) $(JAVAINC) $< -o $@

clean:
	$(RM) -r $(CND_BUILDDIR)/$(CND_CONF)
	$(RM) $(SRCDIR)/*_wrap.*
	$(RM) $(CND_DISTDIR)/$(CND_CONF)/$(CND_PLATFORM)/*.$(CND_DLIB_EXT)