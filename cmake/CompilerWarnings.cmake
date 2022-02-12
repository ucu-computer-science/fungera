set(MSVC_WARNINGS /W4)
set(GCC_CLANG_WARNINGS -Wextra)

if (WARNINGS_AS_ERRORS)
    message("- UCU.APPS.CS: 'Warnings as errors' enabled in CMakeLists.txt")

    set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)
    set(GCC_CLANG_WARNINGS ${GCC_CLANG_WARNINGS} -Wall -pedantic -Werror -Werror=vla)
else ()
    set(GCC_CLANG_WARNINGS ${GCC_CLANG_WARNINGS} -Werror=vla)
endif ()

if (MSVC)
    add_compile_options(${MSVC_WARNINGS})
else ()
    add_compile_options(${GCC_CLANG_WARNINGS})
endif ()