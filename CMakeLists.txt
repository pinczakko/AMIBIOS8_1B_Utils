cmake_minimum_required (VERSION 2.6)
project (amibios8_utils)

set(C_FLAGS_DEBUG "-g3 -O0 -Wall")
set(C_FLAGS_RELEASE "-O1")

set(CMAKE_C_FLAGS_RELEASE ${C_FLAGS_RELEASE})

set(CMAKE_C_FLAGS_DEBUG ${C_FLAGS_DEBUG})

set(CMAKE_CONFIGURATION_TYPES Debug Release)

include_directories( ${PROJECT_SOURCE_DIR} )

if (MINGW)
	message(status: " ** MINGW detected.. **")
	#set(CMAKE_C_FLAGS_RELEASE "-municode ${CMAKE_C_FLAGS_RELEASE}")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

	#set(CMAKE_C_FLAGS_DEBUG "-municode ${CMAKE_C_FLAGS_DEBUG}")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

	##set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -Wl,-Bstatic -lwinpthread ${CMAKE_EXE_LINKER_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -Wl,-Bstatic ${CMAKE_EXE_LINKER_FLAGS}")
else()
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
endif()

set(SOURCES1 ami_1B_splitter.c ami_1B_lib.c)
set(SOURCES2 ami_1B_combiner.c ami_1B_lib.c)

add_executable(ami_1b_splitter ${SOURCES1})
add_executable(ami_1b_combiner ${SOURCES2})

