AUX_SOURCE_DIRECTORY(. GLOBAL_SRC)

FILE(GLOB GLOBAL_INCLUDE ./*.h)

ADD_LIBRARY(global_shared SHARED ${GLOBAL_SRC})
ADD_LIBRARY(global_static STATIC ${GLOBAL_SRC})

INSTALL(TARGETS global_shared LIBRARY DESTINATION lib)
INSTALL(TARGETS global_static ARCHIVE DESTINATION lib)
INSTALL(FILES ${GLOBAL_INCLUDE} DESTINATION include/global)

SET_TARGET_PROPERTIES(global_shared PROPERTIES OUTPUT_NAME "TimepassGlobal")
SET_TARGET_PROPERTIES(global_static PROPERTIES OUTPUT_NAME "TimepassGlobal")