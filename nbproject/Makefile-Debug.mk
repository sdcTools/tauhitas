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
	${OBJECTDIR}/src/WrapCSP.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../CSP/dist/Debug/MinGW-Windows -L../Cplex/lib -L../XPress -L../JJSource/InstallSCIP/scip-3.0.1/lib -L../JJSource/InstallSCIP/Clp-1.15.3/lib -lCSPlibCPLEX -lcplex75 -lCSPlibXPRESS -lxprl -lxprs -lCSPlibSCIP -lscip -llpiclp -lnlpi.cppad -lobjscip -lClp -lCoinUtils

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtauhitas.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtauhitas.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtauhitas.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -Wl,--kill-at -shared

${OBJECTDIR}/src/ALList.o: src/ALList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -DSECBOUNDS -I../Cplex/include/ilcplex -I../XPress -I../JJSource/InstallSCIP/scip-3.0.1/src -I/C/Program\ Files/Java/jdk1.7.0_17/include -I/C/Program\ Files/Java/jdk1.7.0_17/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ALList.o src/ALList.cpp

${OBJECTDIR}/src/AMiscFunc.o: src/AMiscFunc.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -DSECBOUNDS -I../Cplex/include/ilcplex -I../XPress -I../JJSource/InstallSCIP/scip-3.0.1/src -I/C/Program\ Files/Java/jdk1.7.0_17/include -I/C/Program\ Files/Java/jdk1.7.0_17/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AMiscFunc.o src/AMiscFunc.cpp

${OBJECTDIR}/src/AMyLoadProb.o: src/AMyLoadProb.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -DSECBOUNDS -I../Cplex/include/ilcplex -I../XPress -I../JJSource/InstallSCIP/scip-3.0.1/src -I/C/Program\ Files/Java/jdk1.7.0_17/include -I/C/Program\ Files/Java/jdk1.7.0_17/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AMyLoadProb.o src/AMyLoadProb.cpp

${OBJECTDIR}/src/ATabs.o: src/ATabs.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -DSECBOUNDS -I../Cplex/include/ilcplex -I../XPress -I../JJSource/InstallSCIP/scip-3.0.1/src -I/C/Program\ Files/Java/jdk1.7.0_17/include -I/C/Program\ Files/Java/jdk1.7.0_17/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ATabs.o src/ATabs.cpp

${OBJECTDIR}/src/Adefs.o: src/Adefs.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -DSECBOUNDS -I../Cplex/include/ilcplex -I../XPress -I../JJSource/InstallSCIP/scip-3.0.1/src -I/C/Program\ Files/Java/jdk1.7.0_17/include -I/C/Program\ Files/Java/jdk1.7.0_17/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Adefs.o src/Adefs.cpp

${OBJECTDIR}/src/Ahier.o: src/Ahier.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -DSECBOUNDS -I../Cplex/include/ilcplex -I../XPress -I../JJSource/InstallSCIP/scip-3.0.1/src -I/C/Program\ Files/Java/jdk1.7.0_17/include -I/C/Program\ Files/Java/jdk1.7.0_17/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Ahier.o src/Ahier.cpp

${OBJECTDIR}/src/Amyio.o: src/Amyio.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -DSECBOUNDS -I../Cplex/include/ilcplex -I../XPress -I../JJSource/InstallSCIP/scip-3.0.1/src -I/C/Program\ Files/Java/jdk1.7.0_17/include -I/C/Program\ Files/Java/jdk1.7.0_17/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Amyio.o src/Amyio.cpp

${OBJECTDIR}/src/HiTaSCtrl.o: src/HiTaSCtrl.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -DSECBOUNDS -I../Cplex/include/ilcplex -I../XPress -I../JJSource/InstallSCIP/scip-3.0.1/src -I/C/Program\ Files/Java/jdk1.7.0_17/include -I/C/Program\ Files/Java/jdk1.7.0_17/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/HiTaSCtrl.o src/HiTaSCtrl.cpp

${OBJECTDIR}/src/WrapCSP.o: src/WrapCSP.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -DSECBOUNDS -I../Cplex/include/ilcplex -I../XPress -I../JJSource/InstallSCIP/scip-3.0.1/src -I/C/Program\ Files/Java/jdk1.7.0_17/include -I/C/Program\ Files/Java/jdk1.7.0_17/include/win32  -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/WrapCSP.o src/WrapCSP.cpp

.NO_PARALLEL:hitasctrl_wrap.cpp hitasctrl_wrap.h
hitasctrl_wrap.cpp hitasctrl_wrap.h: tauhitas.swg 
	@echo Performing Custom Build Step
	D:\pwof\Documents\swig\swigwin-2.0.9\swig.exe -c++ -I.\src -java -package tauargus.extern.tauhitas -outdir ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM} -o src/hitasctrl_wrap.cpp tauhitas.swg

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libtauhitas.${CND_DLIB_EXT}
	${RM} hitasctrl_wrap.cpp hitasctrl_wrap.h

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
