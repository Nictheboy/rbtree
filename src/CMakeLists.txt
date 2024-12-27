SET(CORE_SOURCES
    rbtree.cpp
)

if(NOT TARGET rbtree-nictheboy)
    if(WIN32)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
    add_library(rbtree-nictheboy SHARED ${CORE_SOURCES})
    set_target_properties(rbtree-nictheboy PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_include_directories(rbtree-nictheboy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    install(TARGETS rbtree-nictheboy)
endif()
