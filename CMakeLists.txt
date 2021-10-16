cmake_minimum_required(VERSION 3.10)

#Project Name
project(mosaico VERSION 1.0)

#Bring the headers, such as libtiles.h into the project
include_directories(include)

# Adding compile Flags
SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Werror -g")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

#Can manually add the sources using the set command as follows:
#set(SOURCES /home/grpp/UFPR/Periodo_Especial/Prog_II/fotomosaico/mosaico.c /home/grpp/UFPR/Periodo_Especial/Prog_II/fotomosaico/libtiles.c /home/grpp/UFPR/Periodo_Especial/Prog_II/fotomosaico/libtiles.h)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "/home/grpp/UFPR/Periodo_Especial/Prog_II/fotomosaico/*.c" "/home/grpp/UFPR/Periodo_Especial/Prog_II/fotomosaico/*.h")

# Add the Executable
add_executable(mosaico ${SOURCES})
target_link_libraries(mosaico m)

