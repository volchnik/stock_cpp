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
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Genetics/Generation.o \
	${OBJECTDIR}/Helpers.o \
	${OBJECTDIR}/Operator/Operator.o \
	${OBJECTDIR}/Operator/OperatorAdd.o \
	${OBJECTDIR}/Operator/OperatorBinary.o \
	${OBJECTDIR}/Operator/OperatorDivide.o \
	${OBJECTDIR}/Operator/OperatorEma.o \
	${OBJECTDIR}/Operator/OperatorMultiply.o \
	${OBJECTDIR}/Operator/OperatorNumber.o \
	${OBJECTDIR}/Operator/OperatorSeries.o \
	${OBJECTDIR}/Operator/OperatorSma.o \
	${OBJECTDIR}/Operator/OperatorSubtract.o \
	${OBJECTDIR}/Operator/OperatorUnary.o \
	${OBJECTDIR}/Series.o \
	${OBJECTDIR}/SeriesSample.o \
	${OBJECTDIR}/Strategy.o \
	${OBJECTDIR}/Trader.o \
	${OBJECTDIR}/TraderOperation.o \
	${OBJECTDIR}/main.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++11
CXXFLAGS=-std=c++11

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stock_analyze

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stock_analyze: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stock_analyze ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/Genetics/Generation.o: Genetics/Generation.cpp 
	${MKDIR} -p ${OBJECTDIR}/Genetics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Genetics/Generation.o Genetics/Generation.cpp

${OBJECTDIR}/Helpers.o: Helpers.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Helpers.o Helpers.cpp

${OBJECTDIR}/Operator/Operator.o: Operator/Operator.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/Operator.o Operator/Operator.cpp

${OBJECTDIR}/Operator/OperatorAdd.o: Operator/OperatorAdd.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorAdd.o Operator/OperatorAdd.cpp

${OBJECTDIR}/Operator/OperatorBinary.o: Operator/OperatorBinary.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorBinary.o Operator/OperatorBinary.cpp

${OBJECTDIR}/Operator/OperatorDivide.o: Operator/OperatorDivide.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorDivide.o Operator/OperatorDivide.cpp

${OBJECTDIR}/Operator/OperatorEma.o: Operator/OperatorEma.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorEma.o Operator/OperatorEma.cpp

${OBJECTDIR}/Operator/OperatorMultiply.o: Operator/OperatorMultiply.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorMultiply.o Operator/OperatorMultiply.cpp

${OBJECTDIR}/Operator/OperatorNumber.o: Operator/OperatorNumber.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorNumber.o Operator/OperatorNumber.cpp

${OBJECTDIR}/Operator/OperatorSeries.o: Operator/OperatorSeries.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorSeries.o Operator/OperatorSeries.cpp

${OBJECTDIR}/Operator/OperatorSma.o: Operator/OperatorSma.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorSma.o Operator/OperatorSma.cpp

${OBJECTDIR}/Operator/OperatorSubtract.o: Operator/OperatorSubtract.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorSubtract.o Operator/OperatorSubtract.cpp

${OBJECTDIR}/Operator/OperatorUnary.o: Operator/OperatorUnary.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorUnary.o Operator/OperatorUnary.cpp

${OBJECTDIR}/Series.o: Series.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Series.o Series.cpp

${OBJECTDIR}/SeriesSample.o: SeriesSample.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SeriesSample.o SeriesSample.cpp

${OBJECTDIR}/Strategy.o: Strategy.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Strategy.o Strategy.cpp

${OBJECTDIR}/Trader.o: Trader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Trader.o Trader.cpp

${OBJECTDIR}/TraderOperation.o: TraderOperation.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TraderOperation.o TraderOperation.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-conf ${TESTFILES}
${TESTDIR}/TestFiles/f1: ${TESTDIR}/SeriesTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc}   -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS} 


${TESTDIR}/SeriesTests.o: SeriesTests.cpp 
	${MKDIR} -p ${TESTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/SeriesTests.o SeriesTests.cpp


${OBJECTDIR}/Genetics/Generation_nomain.o: ${OBJECTDIR}/Genetics/Generation.o Genetics/Generation.cpp 
	${MKDIR} -p ${OBJECTDIR}/Genetics
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Genetics/Generation.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Genetics/Generation_nomain.o Genetics/Generation.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Genetics/Generation.o ${OBJECTDIR}/Genetics/Generation_nomain.o;\
	fi

${OBJECTDIR}/Helpers_nomain.o: ${OBJECTDIR}/Helpers.o Helpers.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Helpers.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Helpers_nomain.o Helpers.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Helpers.o ${OBJECTDIR}/Helpers_nomain.o;\
	fi

${OBJECTDIR}/Operator/Operator_nomain.o: ${OBJECTDIR}/Operator/Operator.o Operator/Operator.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Operator/Operator.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/Operator_nomain.o Operator/Operator.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Operator/Operator.o ${OBJECTDIR}/Operator/Operator_nomain.o;\
	fi

${OBJECTDIR}/Operator/OperatorAdd_nomain.o: ${OBJECTDIR}/Operator/OperatorAdd.o Operator/OperatorAdd.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Operator/OperatorAdd.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorAdd_nomain.o Operator/OperatorAdd.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Operator/OperatorAdd.o ${OBJECTDIR}/Operator/OperatorAdd_nomain.o;\
	fi

${OBJECTDIR}/Operator/OperatorBinary_nomain.o: ${OBJECTDIR}/Operator/OperatorBinary.o Operator/OperatorBinary.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Operator/OperatorBinary.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorBinary_nomain.o Operator/OperatorBinary.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Operator/OperatorBinary.o ${OBJECTDIR}/Operator/OperatorBinary_nomain.o;\
	fi

${OBJECTDIR}/Operator/OperatorDivide_nomain.o: ${OBJECTDIR}/Operator/OperatorDivide.o Operator/OperatorDivide.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Operator/OperatorDivide.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorDivide_nomain.o Operator/OperatorDivide.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Operator/OperatorDivide.o ${OBJECTDIR}/Operator/OperatorDivide_nomain.o;\
	fi

${OBJECTDIR}/Operator/OperatorEma_nomain.o: ${OBJECTDIR}/Operator/OperatorEma.o Operator/OperatorEma.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Operator/OperatorEma.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorEma_nomain.o Operator/OperatorEma.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Operator/OperatorEma.o ${OBJECTDIR}/Operator/OperatorEma_nomain.o;\
	fi

${OBJECTDIR}/Operator/OperatorMultiply_nomain.o: ${OBJECTDIR}/Operator/OperatorMultiply.o Operator/OperatorMultiply.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Operator/OperatorMultiply.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorMultiply_nomain.o Operator/OperatorMultiply.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Operator/OperatorMultiply.o ${OBJECTDIR}/Operator/OperatorMultiply_nomain.o;\
	fi

${OBJECTDIR}/Operator/OperatorNumber_nomain.o: ${OBJECTDIR}/Operator/OperatorNumber.o Operator/OperatorNumber.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Operator/OperatorNumber.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorNumber_nomain.o Operator/OperatorNumber.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Operator/OperatorNumber.o ${OBJECTDIR}/Operator/OperatorNumber_nomain.o;\
	fi

${OBJECTDIR}/Operator/OperatorSeries_nomain.o: ${OBJECTDIR}/Operator/OperatorSeries.o Operator/OperatorSeries.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Operator/OperatorSeries.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorSeries_nomain.o Operator/OperatorSeries.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Operator/OperatorSeries.o ${OBJECTDIR}/Operator/OperatorSeries_nomain.o;\
	fi

${OBJECTDIR}/Operator/OperatorSma_nomain.o: ${OBJECTDIR}/Operator/OperatorSma.o Operator/OperatorSma.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Operator/OperatorSma.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorSma_nomain.o Operator/OperatorSma.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Operator/OperatorSma.o ${OBJECTDIR}/Operator/OperatorSma_nomain.o;\
	fi

${OBJECTDIR}/Operator/OperatorSubtract_nomain.o: ${OBJECTDIR}/Operator/OperatorSubtract.o Operator/OperatorSubtract.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Operator/OperatorSubtract.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorSubtract_nomain.o Operator/OperatorSubtract.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Operator/OperatorSubtract.o ${OBJECTDIR}/Operator/OperatorSubtract_nomain.o;\
	fi

${OBJECTDIR}/Operator/OperatorUnary_nomain.o: ${OBJECTDIR}/Operator/OperatorUnary.o Operator/OperatorUnary.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operator
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Operator/OperatorUnary.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operator/OperatorUnary_nomain.o Operator/OperatorUnary.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Operator/OperatorUnary.o ${OBJECTDIR}/Operator/OperatorUnary_nomain.o;\
	fi

${OBJECTDIR}/Series_nomain.o: ${OBJECTDIR}/Series.o Series.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Series.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Series_nomain.o Series.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Series.o ${OBJECTDIR}/Series_nomain.o;\
	fi

${OBJECTDIR}/SeriesSample_nomain.o: ${OBJECTDIR}/SeriesSample.o SeriesSample.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/SeriesSample.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SeriesSample_nomain.o SeriesSample.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/SeriesSample.o ${OBJECTDIR}/SeriesSample_nomain.o;\
	fi

${OBJECTDIR}/Strategy_nomain.o: ${OBJECTDIR}/Strategy.o Strategy.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Strategy.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Strategy_nomain.o Strategy.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Strategy.o ${OBJECTDIR}/Strategy_nomain.o;\
	fi

${OBJECTDIR}/Trader_nomain.o: ${OBJECTDIR}/Trader.o Trader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Trader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Trader_nomain.o Trader.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Trader.o ${OBJECTDIR}/Trader_nomain.o;\
	fi

${OBJECTDIR}/TraderOperation_nomain.o: ${OBJECTDIR}/TraderOperation.o TraderOperation.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/TraderOperation.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TraderOperation_nomain.o TraderOperation.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/TraderOperation.o ${OBJECTDIR}/TraderOperation_nomain.o;\
	fi

${OBJECTDIR}/main_nomain.o: ${OBJECTDIR}/main.o main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/main.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main_nomain.o main.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/main.o ${OBJECTDIR}/main_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/stock_analyze

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
