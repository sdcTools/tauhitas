#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/ALList.o \
	${OBJECTDIR}/src/AMiscFunc.o \
	${OBJECTDIR}/src/AMyLoadProb.o \
	${OBJECTDIR}/src/ATabs.o \
	${OBJECTDIR}/src/Adefs.o \
	${OBJECTDIR}/src/Ahier.o \
	${OBJECTDIR}/src/Amyio.o \
	${OBJECTDIR}/src/HiTaSCtrl.o \
	${OBJECTDIR}/src/HiTaSCtrl_wrap.o \
	${OBJECTDIR}/src/WrapCSP.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32 -ggdb
CXXFLAGS=-m32 -ggdb

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../CSP/dist/Debug/MinGW-Windows -L../Solvers/Cplex/Cplex75/lib -L../Solvers/XPress -L../Solvers/scip-3.1.1/lib -L../Solvers/soplex-2.0.1/lib -lCSPlibCPLEX -lcplex75 -lCSPlibXPRESS -lxprl -lxprs -lCSPlibSCIP -lscip -llpispx -lnlpi.cppad -lobjscip -lsoplex ${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/src/Versioninfo.o

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtauhitas.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtauhitas.${CND_DLIB_EXT}: ${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/src/Versioninfo.o

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtauhitas.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtauhitas.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -Wl,--kill-at -shared

${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/src/Versioninfo.o: /C/Users/pwof/Documents/Argus/tauhitas/src/Versioninfo.rc
	${MKDIR} -p ${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/src
	@echo Performing Custom Build Step
	windres ./src/Versioninfo.rc ${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/src/Versioninfo.o

.NO_PARALLEL:./src/HiTaSCtrl_wrap.cpp ./src/HiTaSCtrl_wrap.h
./src/HiTaSCtrl_wrap.cpp ./src/HiTaSCtrl_wrap.h: hitasctrl.swg
	${MKDIR} -p ./src ./src
	@echo Performing Custom Build Step
	C:/swigwin-3.0.10/swig.exe -c++ -I.\src -java -package tauargus.extern.tauhitas -outdir ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM} -o ./src/HiTaSCtrl_wrap.cpp hitasctrl.swg

${OBJECTDIR}/src/ALList.o: src/ALList.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DSECBOUNDS -I../Solvers/Cplex/Cplex75/include/ilcplex -I../Solvers/XPress -I../Solvers/scip-3.1.1/src -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include/win32  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ALList.o src/ALList.cpp

${OBJECTDIR}/src/AMiscFunc.o: src/AMiscFunc.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DSECBOUNDS -I../Solvers/Cplex/Cplex75/include/ilcplex -I../Solvers/XPress -I../Solvers/scip-3.1.1/src -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include/win32  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AMiscFunc.o src/AMiscFunc.cpp

${OBJECTDIR}/src/AMyLoadProb.o: src/AMyLoadProb.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DSECBOUNDS -I../Solvers/Cplex/Cplex75/include/ilcplex -I../Solvers/XPress -I../Solvers/scip-3.1.1/src -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include/win32  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AMyLoadProb.o src/AMyLoadProb.cpp

${OBJECTDIR}/src/ATabs.o: src/ATabs.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DSECBOUNDS -I../Solvers/Cplex/Cplex75/include/ilcplex -I../Solvers/XPress -I../Solvers/scip-3.1.1/src -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include/win32  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ATabs.o src/ATabs.cpp

${OBJECTDIR}/src/Adefs.o: src/Adefs.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DSECBOUNDS -I../Solvers/Cplex/Cplex75/include/ilcplex -I../Solvers/XPress -I../Solvers/scip-3.1.1/src -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include/win32  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Adefs.o src/Adefs.cpp

${OBJECTDIR}/src/Ahier.o: src/Ahier.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DSECBOUNDS -I../Solvers/Cplex/Cplex75/include/ilcplex -I../Solvers/XPress -I../Solvers/scip-3.1.1/src -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include/win32  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Ahier.o src/Ahier.cpp

${OBJECTDIR}/src/Amyio.o: src/Amyio.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DSECBOUNDS -I../Solvers/Cplex/Cplex75/include/ilcplex -I../Solvers/XPress -I../Solvers/scip-3.1.1/src -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include/win32  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Amyio.o src/Amyio.cpp

${OBJECTDIR}/src/HiTaSCtrl.o: src/HiTaSCtrl.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DSECBOUNDS -I../Solvers/Cplex/Cplex75/include/ilcplex -I../Solvers/XPress -I../Solvers/scip-3.1.1/src -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include/win32  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/HiTaSCtrl.o src/HiTaSCtrl.cpp

${OBJECTDIR}/src/HiTaSCtrl_wrap.o: src/HiTaSCtrl_wrap.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DSECBOUNDS -I../Solvers/Cplex/Cplex75/include/ilcplex -I../Solvers/XPress -I../Solvers/scip-3.1.1/src -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include/win32  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/HiTaSCtrl_wrap.o src/HiTaSCtrl_wrap.cpp

${OBJECTDIR}/src/WrapCSP.o: src/WrapCSP.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DSECBOUNDS -I../Solvers/Cplex/Cplex75/include/ilcplex -I../Solvers/XPress -I../Solvers/scip-3.1.1/src -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include -I/C/Program\ Files\ \(x86\)/Java/jdk1.7.0_80/include/win32  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/WrapCSP.o src/WrapCSP.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/src/Versioninfo.o
	${RM} ./src/HiTaSCtrl_wrap.cpp ./src/HiTaSCtrl_wrap.h

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
