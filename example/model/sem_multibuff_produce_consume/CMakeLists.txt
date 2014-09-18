AUX_SOURCE_DIRECTORY(. SRC)
ADD_EXECUTABLE(sem_multibuff_produce_consume ${SRC})

IF (GLOBAL AND LOCK AND SHM)
	TARGET_LINK_LIBRARIES(sem_multibuff_produce_consume ${GLOBAL} ${LOCK} ${SHM})
	INSTALL(TARGETS sem_multibuff_produce_consume RUNTIME DESTINATION bin)
ELSE (GLOBAL AND LOCK AND SHM)
	IF (NOT GLOBAL)
		MESSAGE("can't find libTimepassGlobal.so")
	ENDIF (NOT GLOBAL)

	IF (NOT LOCK)
		MESSAGE("can't find libTimepassLock.so")
	ENDIF (NOT LOCK)

    IF (NOT SHM)
        MESSAGE("can't find libTimepassShm.so")
    ENDIF(NOT SHM)
ENDIF (GLOBAL AND LOCK AND SHM)
