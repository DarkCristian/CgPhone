find_package(PkgConfig REQUIRED)
pkg_check_modules(PJPROJECT REQUIRED IMPORTED_TARGET libpjproject)
add_library(PJSIP::PJSUA2 INTERFACE IMPORTED)
target_link_libraries(PJSIP::PJSUA2 INTERFACE PkgConfig::PJPROJECT)

