AUX_SOURCE_DIRECTORY(. SRC)
ADD_EXECUTABLE(mutex_multibuff_produce_consume ${SRC})

IF (GLOBAL AND LOCK AND TIME)
	TARGET_LINK_LIBRARIES(mutex_multibuff_produce_consume ${GLOBAL} ${LOCK} ${TIME})
	INSTALL(TARGETS mutex_multibuff_produce_consume RUNTIME DESTINATION bin)
ELSE (GLOBAL AND LOCK AND TIME)
	IF (NOT GLOBAL)
		MESSAGE("can't find libTimepassGlobal.so")
	ENDIF (NOT GLOBAL)

	IF (NOT LOCK)
		MESSAGE("can't find libTimepassLock.so")
	ENDIF (NOT LOCK)

    IF (NOT TIME)
        MESSAGE("can't find libTimePassTime.so")
    ENDIF (NOT TIME)
ENDIF (GLOBAL AND LOCK AND TIME)
