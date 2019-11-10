message(STATUS "Find lib ...")
find_path(
    LIBDB_CXX_INCLUDE_DIR
    db_cxx.h
    /usr/include/
    /usr/local/include/
    ./lib/
    )

find_library(
    LIBDB_CXX_LIBRARIES NAMES db_cxx
    PATHS /usr/lib/ /usr/local/lib/ ./lib/
    )
