OPT:= -nodebug -O2 
SIMPL:=CBEGIN
TYPE:=int
SENTINEL:=-1
BIMPL:=NOBUSY
BCHECK:=NOBARGINGCHECK

CXX = u++					# compiler
CXXFLAGS = -g -multi ${OPT} -Wall -Wextra -MMD -D"${SIMPL}" -DTYPE="${TYPE}" \
		-D"${BIMPL}" -DSENTINEL="${SENTINEL}" -D"${BCHECK}" # compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS1 = q2.o# object files forming 1st executable with prefix "q2"
EXEC1 = mergesort				# 1st executable name

OBJECTS2 = q3.o q3pc.o# object files forming 2nd executable with prefix "q3"
EXEC2 = buffer					# 2nd executable name

OBJECTS = ${OBJECTS1} ${OBJECTS2}		# all object files
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2}			# all executables

#############################################################

.PHONY : all clean

all : ${EXECS}					# build all executables

-include MergeImpl

ifeq (${shell if [ "${IMPLTYPE}" = "${TYPE}" -a "${IMPLSIMPL}" = "${SIMPL}" ] ; \
		then echo true ; fi },true)
${EXEC1} : ${OBJECTS1}
	${CXX} ${CXXFLAGS} $^ -o $@
else
.PHONY : ${EXEC1}
${EXEC1} :
	rm -f MergeImpl
	touch q2mergesort.h
	${MAKE} ${EXEC1} SIMPL="${SIMPL}" TYPE="${TYPE}"
endif

MergeImpl :
	echo "IMPLSIMPL=${SIMPL}\nIMPLTYPE=${TYPE}" > MergeImpl
	sleep 1

-include BufImpl

ifeq (${shell if [ "${BUFIMPL}" = "${BIMPL}" -a "${SENTIMPL}" = "${SENTINEL}" -a \
			"${BCHECKIMPL}" = "${BCHECK}" ] ; then echo true ; fi },true)
${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed => rebuilt
.PHONY : ${EXEC2}
${EXEC2} :
	rm -f BufImpl
	touch q3buffer.h
	sleep 1
	${MAKE} ${EXEC2} BIMPL="${BIMPL}" SENTINEL="${SENTINEL}" BCHECK="${BCHECK}"
endif

BufImpl :
	echo "BUFIMPL=${BIMPL}\nSENTIMPL=${SENTINEL}\nBCHECKIMPL=${BCHECK}" > BufImpl
	sleep 1

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXECS} MergeImpl BufImpl
