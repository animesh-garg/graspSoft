

cmake_minimum_required(VERSION 2.6)
if(COMMAND cmake_policy)
  cmake_policy(VERSION 2.6)
endif(COMMAND cmake_policy)



project(json)


SET(CMAKE_C_FLAGS "-arch x86_64 -arch i386 -DDEBUG -Wno-deprecated")



include_directories(${matvtk_SOURCE_DIR}
)

LINK_DIRECTORIES(
)


SET(LIBJSON_SRC
  arraylist.c
  debug.c
  json_object.c
  json_tokener.c
  json_util.c
  linkhash.c
  printbuf.c
)

add_library(json STATIC ${LIBJSON_SRC})




