find_package(PkgConfig REQUIRED)
pkg_check_modules(PJPROJECT REQUIRED libpjproject)

# PJPROJECT genera su archivo .pc con prefijo POSIX (/mingw64). Cuando CMake
# se ejecuta como programa nativo de Windows desde MSYS2, esa ruta no existe
# para CMake aunque sí exista dentro del shell. Convertimos el prefijo al que
# contiene al compilador MinGW (por ejemplo D:/a/_temp/msys64/mingw64).
set(_pjsip_include_dirs ${PJPROJECT_INCLUDE_DIRS})
set(_pjsip_library_dirs ${PJPROJECT_STATIC_LIBRARY_DIRS})

if(MINGW)
    get_filename_component(_pjsip_compiler_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
    get_filename_component(_pjsip_mingw_prefix "${_pjsip_compiler_bin}" DIRECTORY)

    set(_pjsip_normalized_includes)
    foreach(_path IN LISTS _pjsip_include_dirs)
        if(_path MATCHES "^/mingw64(/.*)?$")
            string(REGEX REPLACE "^/mingw64" "${_pjsip_mingw_prefix}" _path "${_path}")
        endif()
        list(APPEND _pjsip_normalized_includes "${_path}")
    endforeach()
    set(_pjsip_include_dirs ${_pjsip_normalized_includes})

    set(_pjsip_normalized_library_dirs)
    foreach(_path IN LISTS _pjsip_library_dirs)
        if(_path MATCHES "^/mingw64(/.*)?$")
            string(REGEX REPLACE "^/mingw64" "${_pjsip_mingw_prefix}" _path "${_path}")
        endif()
        list(APPEND _pjsip_normalized_library_dirs "${_path}")
    endforeach()
    set(_pjsip_library_dirs ${_pjsip_normalized_library_dirs})
endif()

add_library(PJSIP::PJSUA2 INTERFACE IMPORTED)
set_target_properties(PJSIP::PJSUA2 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_pjsip_include_dirs}"
    INTERFACE_LINK_DIRECTORIES "${_pjsip_library_dirs}"
    INTERFACE_LINK_LIBRARIES "${PJPROJECT_STATIC_LIBRARIES}"
    INTERFACE_COMPILE_OPTIONS "${PJPROJECT_STATIC_CFLAGS_OTHER}"
    INTERFACE_LINK_OPTIONS "${PJPROJECT_STATIC_LDFLAGS_OTHER}"
)
