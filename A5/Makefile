VIMPL:=EXT
OUTPUT:=OUTPUT
BCHECK:=NOBARGINGCHECK

CXX = u++					# compiler
CXXFLAGS = -g -multi -Wall -Wextra -MMD -D"${VIMPL}" -D"${OUTPUT}" \
			-D"${BCHECK}" # compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS = q2tallyVotes${VIMPL}.o q2main.o q2tallyVotes.o q2voter.o q2printer.o# list of object files for question 3 prefixed with "q3"
EXEC = vote

DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"

#############################################################

.PHONY : all clean

all : ${EXEC}					# build all executables

-include VoteImpl

ifeq (${shell if [ "${LOCKVIMPL}" = "${VIMPL}" -a "${OUTPUTTYPE}" = "${OUTPUT}" -a \
			"${BCHECKIMPL}" = "${BCHECK}" ] ; then echo true ; fi },true)
${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed => rebuilt
.PHONY : ${EXEC}
${EXEC} :
	rm -f VoteImpl
	touch q2tallyVotes.h
	${MAKE} ${EXEC} VIMPL="${VIMPL}" OUTPUT="${OUTPUT}" BCHECK="${BCHECK}"
endif

VoteImpl :
	echo "LOCKVIMPL=${VIMPL}\nOUTPUTTYPE=${OUTPUT}\nBCHECKIMPL=${BCHECK}" > VoteImpl
	sleep 1

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d ${OBJECTS} ${EXEC} VoteImpl
