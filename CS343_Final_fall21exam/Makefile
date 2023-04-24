# ***********************
# DO NOT CHANGE THIS CODE
# ***********************

MIMPL:=EXT

CXX = u++					# compiler
CXXFLAGS = -g -Wall -Wextra -multi -MMD -D${MIMPL} # compiler flags
MAKEFLAGS = --no-print-directory
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS1 = BarrierTime.o
EXEC1 = barrier

OBJECTS2 = BarrierTimeAdmin.o
EXEC2 = barrieradm

OBJECTS = ${OBJECTS1} ${OBJECTS1} ${OBJECTS2}	# all object files
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC1} ${EXEC2}		# all executables

#############################################################

.PHONY : all clean

all : ${EXECS}					# build all executables

-include MonitorImpl

ifeq (${MONITORMIMPL},${MIMPL})			# same implementation type as last time ?
${EXEC1} : ${OBJECTS1}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed => rebuilt
.PHONY : ${EXEC1}
${EXEC1} :
	rm -f MonitorImpl
	touch BarrierTime.cc
	${MAKE} ${EXEC1} MIMPL="${MIMPL}"
endif

MonitorImpl :
	echo "MONITORMIMPL=${MIMPL}" > MonitorImpl
	sleep 1

${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d ${OBJECTS} ${EXECS} MonitorImpl
