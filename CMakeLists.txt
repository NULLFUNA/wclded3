CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

PROJECT(quake3e)

SET(DNAME 		"wclded3")
SET(GAMENAME 	"wclstandart")

SET(CMAKE_MODULE_PATH 					${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY      ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set(CMAKE_CACHEFILE_DIR                 ${CMAKE_CURRENT_SOURCE_DIR}/build/cache)
set(CMAKE_PDB_OUTPUT_DIRECTORY          ${CMAKE_CURRENT_SOURCE_DIR}/build/pdb)
AUX_SOURCE_DIRECTORY(code/qcommon 		QCOMMON_SRCS)

# exclude platform-dependent QVM bytecode compilers
list(FILTER QCOMMON_SRCS EXCLUDE REGEX ".*vm_[alx].*.c")
list(FILTER QCOMMON_SRCS EXCLUDE REGEX ".cpp")

# add platform-dependent QVM bytecode compilers
IF(CMAKE_SYSTEM_PROCESSOR MATCHES AMD64|x86|i*86)
	list(APPEND QCOMMON_SRCS code/qcommon/vm_x86.c)
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES aarch64|arm64)
	list(APPEND QCOMMON_SRCS code/qcommon/vm_aarch64.c)
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES arm*)
	list(APPEND QCOMMON_SRCS code/qcommon/vm_armv7l.c)
ELSE()
	TARGET_COMPILE_DEFINITIONS(qcommon_ded PRIVATE -DNO_VM_COMPILED)
ENDIF()

# botlib
AUX_SOURCE_DIRECTORY(code/botlib BOTLIB_SRCS)
ADD_LIBRARY(botlib OBJECT ${BOTLIB_SRCS})
TARGET_COMPILE_DEFINITIONS(botlib PRIVATE BOTLIB)

AUX_SOURCE_DIRECTORY(code/server SERVER_SRCS)
list(REMOVE_ITEM SERVER_SRCS code/server/sv_rankings.c)

# dedicated server
ADD_LIBRARY(qcommon_ded OBJECT ${QCOMMON_SRCS} ${SERVER_SRCS})
TARGET_COMPILE_DEFINITIONS(qcommon_ded PUBLIC DEDICATED)

# common function, window system
IF(UNIX)
	SET (Q3_SRCS
		code/unix/unix_main.c
		code/unix/unix_shared.c
		code/unix/linux_signals.c)

	IF(APPLE)
		SET(EXE_TYPE MACOSX_BUNDLE)
		list(APPEND Q3_SRCS code/unix/quake3_flat.icns)
		set_source_files_properties(code/unix/quake3_flat.icns PROPERTIES
			MACOSX_PACKAGE_LOCATION Resources)
		SET(MACOSX_BUNDLE_ICON_FILE quake3_flat.icns)
	ENDIF()
ELSEIF(WIN32)
	SET(EXE_TYPE WIN32)
	IF(MSVC)
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
		ADD_COMPILE_DEFINITIONS(_CRT_SECURE_NO_WARNINGS) # v3.12+
	ENDIF()

	SET(Q3_SRCS
		code/win32/win_main.c
		code/win32/win_shared.c
		code/win32/win_syscon.c
		code/win32/win_resource.rc)
ENDIF(UNIX)

# dedicated server executable

ADD_EXECUTABLE(${DNAME} ${EXE_TYPE} ${Q3_SRCS})
TARGET_LINK_LIBRARIES(${DNAME} qcommon_ded botlib)
#TARGET_LINK_OPTIONS(${DNAME} PRIVATE -fno-pie) # v3.13+

IF(WIN32)
	TARGET_LINK_LIBRARIES(${DNAME} winmm comctl32 ws2_32)
ELSE()
	TARGET_LINK_LIBRARIES(${DNAME} m ${CMAKE_DL_LIBS})
ENDIF()

# detour wclded3 dynamic library

ADD_LIBRARY(${GAMENAME} SHARED 
	"code/qcommon/q_math.cpp"
	"code/qcommon/q_shared.cpp"

	"code/gamedetour/main.cpp"
)
