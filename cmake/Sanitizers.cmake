if (ENABLE_SANITIZERS OR ENABLE_UBSan OR ENABLE_MSAN OR ENABLE_ASAN OR ENABLE_TSan)
    message("- UCU.APPS.CS: Sanitizers enabled. You can disable it in CMakeLists.txt")
    if (CMAKE_C_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(WARNING "- UCU.APPS.CS: Sanitizers for the MSVC are not yet supported")
    elseif (MINGW) #  OR CYGWIN?
        message(WARNING "- UCU.APPS.CS: Sanitizers for the MINGW are not yet supported")
    else ()
        if (ENABLE_UBSan)
            set(SANITIZE_UNDEFINED ON)
        endif ()

        # Only one of Memory, Address, or Thread sanitizers is applicable at the time
        if (CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            if (ENABLE_MSAN)
                message("- UCU.APPS.CS: MSAN Enabled in CMakeLists.txt")
                set(SANITIZE_MEMORY ON)
            elseif (ENABLE_ASAN)
                message("- UCU.APPS.CS: ASAN Enabled in CMakeLists.txt")
                set(SANITIZE_ADDRESS ON)
            elseif (ENABLE_TSan)
                message("- UCU.APPS.CS:  TSAN Enabled in CMakeLists.txt")
                set(SANITIZE_THREAD ON)
            endif ()
        elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            if (ENABLE_MSAN)
                message(WARNING "UCU.APPS.CS: GCC does not have a MSAN (SANITIZE_MEMORY).")
            elseif (ENABLE_ASAN)
                message("- UCU.APPS.CS: ASAN Enabled in CMakeLists.txt")
                set(SANITIZE_ADDRESS ON)
            elseif(ENABLE_TSan)
                message("- UCU.APPS.CS: TSAN Enabled in CMakeLists.txt")
                set(SANITIZE_THREAD ON)
            endif ()
        endif ()

        set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/extra/sanitizers" ${CMAKE_MODULE_PATH})
        find_package(Sanitizers)

        add_sanitizers(${ALL_TARGETS})
    endif ()

endif () # For MSVC

