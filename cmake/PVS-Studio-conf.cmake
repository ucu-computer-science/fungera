if (ENABLE_PVS_STUDIO)
    message("- UCU.APPS.CS: PVS Studio enabled in CMakeLists.txt")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    include(cmake/extra/PVS-Studio.cmake)
    foreach(TARGET ${ALL_TARGETS})
        pvs_studio_add_target(TARGET ${TARGET}.analyze ALL
                OUTPUT FORMAT errorfile
                ANALYZE ${TARGET}
                LOG target.err)
    endforeach()

else ()
    message(NOTICE "\n- UCU.APPS.CS: Consider enabling PVS-Studio in CMakeLists.txt by 'set(ENABLE_PVS_STUDIO ON)' flag or use Windows PVS Studio GUI.")
endif ()
