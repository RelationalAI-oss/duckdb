# don't export any symbols from the third party stuff
set(CMAKE_C_VISIBILITY_PRESET hidden)

add_subdirectory(hyperloglog)
add_subdirectory(libpg_query)
add_subdirectory(re2)
add_subdirectory(miniz)

if(NOT WIN32 AND NOT LEAN)
  add_subdirectory(imdb)
  add_subdirectory(dbgen)
  add_subdirectory(dsdgen)
  add_subdirectory(sqlsmith)
  add_subdirectory(sqlite)
  add_subdirectory(tpce-tool)
endif()