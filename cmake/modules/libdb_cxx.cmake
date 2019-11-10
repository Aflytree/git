message("Find lib ...")
FIND_PATH(
    LIBDB_CXX_INCLUDE_DIR
    db_cxx.h
    /usr/include/
    /usr/local/include/
    )

FIND_LIBRARY(
    LIBDB_CXX_LIBRARIES NAMES db_cxx
    PATHS /usr/lib/ /usr/local/lib/ ./lib/
    )
