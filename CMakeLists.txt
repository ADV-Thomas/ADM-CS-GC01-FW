cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)
project(adm-cs-fp-fm01 C ASM)

set(FW_LIB ${CMAKE_SOURCE_DIR}/fw_lib)

include(${FW_LIB}/build/fw_app_build.cmake)

check_options()


include(${FW_LIB}/build/c2000_build.cmake)

set_include_directories()
set_link_directories()

# Generate firmware version and timestamp
version()

# Register CAN database object generator
acgu(ADM_CS_FP 16 ${CMAKE_SOURCE_DIR}/db/ADM_CS_fP.kcd)
acgu_different_module_db_all_cons(User ADM_PC_BP25 16 ${CMAKE_SOURCE_DIR}/db/ADM_PC_BP25.kcd)
acgu_different_module_db_all_cons(User ADM_PC_VG11_FM01 16 ${CMAKE_SOURCE_DIR}/db/ADM_PC_VG11.kcd)
acgu_different_module_db_all_cons(User ADM_PC_VG11_FM02 16 ${CMAKE_SOURCE_DIR}/db/ADM_PC_VG11.kcd)



# We create an intermediate library with the databases, otherwise the dependency list is too
# large and compilation fails
add_library(
    databases.lib
    ${CMAKE_BINARY_DIR}/adm_pc_vg11_fm01_database.c
    ${CMAKE_BINARY_DIR}/adm_pc_vg11_fm01_db.c
    ${CMAKE_BINARY_DIR}/adm_pc_vg11_fm02_database.c
    ${CMAKE_BINARY_DIR}/adm_pc_vg11_fm02_db.c
    ${CMAKE_BINARY_DIR}/adm_pc_bp25_database.c
    ${CMAKE_BINARY_DIR}/adm_pc_bp25_db.c
    ${CMAKE_BINARY_DIR}/adm_cs_fp_database.c
    ${CMAKE_BINARY_DIR}/adm_cs_fp_db.c
)




add_library(
    icons.lib
    app/SSD1322_OLED_lib/Icons/icon_about.c
    app/SSD1322_OLED_lib/Icons/icon_fault.c
    app/SSD1322_OLED_lib/Icons/icon_superset.c
    app/SSD1322_OLED_lib/Icons/icon_module.c
    app/SSD1322_OLED_lib/Icons/icon_none.c
    app/SSD1322_OLED_lib/Icons/icon_set.c
    app/SSD1322_OLED_lib/Icons/icon_stack.c
    app/SSD1322_OLED_lib/Icons/icon_version.c

    app/SSD1322_OLED_lib/Icons/icon_boost.c
    app/SSD1322_OLED_lib/Icons/icon_buck.c
    app/SSD1322_OLED_lib/Icons/icon_inverter.c
    app/SSD1322_OLED_lib/Icons/icon_neutral.c
    app/SSD1322_OLED_lib/Icons/icon_pwm.c
    app/SSD1322_OLED_lib/Icons/icon_rectifier.c


    app/SSD1322_OLED_lib/Icons/icon_3_ph_gen_1.c
)




add_executable(
    ${CMAKE_PROJECT_NAME}.out
    app/adm_cs_fp_db.c

    app/dev/db/adm_pc_bp25_db.c
    app/dev/db/adm_pc_vg11_fm01_db.c
    app/dev/db/adm_pc_vg11_fm02_db.c

    app/dev/ctl/bp25_ctl.c
    app/dev/ctl/vg11_fm01_ctl.c
    app/dev/ctl/vg11_fm02_ctl.c

    app/SSD1322_OLED_lib/SSD1322_HW_Driver.c
    app/SSD1322_OLED_lib/SSD1322_API.c
    app/SSD1322_OLED_lib/SSD1322_GFX.c

    app/display/view.c
    app/display/state_machine.c
    app/display/key.c
    app/hapi_rev0.c
    app/hapi_rev1.c
    app/hapi.c
    app/main.c
    app/task.c
    app/tlo.c
    app/adc.c
    app/ctl.c
    app/db.c
    #app/wcs.c
    app/dev_ctl.c
    app/superset_ctl.c
    ${FW_LIB}/code/src/drv/ecap.c
    #${FW_LIB}/code/src/lib/fan_ctl.c
    ${FW_LIB}/code/src/net/spi.c
)


target_link_libraries(${CMAKE_PROJECT_NAME}.out databases.lib  icons.lib ) 

set_library_files()
set_linker_file()

add_dependencies(${CMAKE_PROJECT_NAME}.out acgu_adm_cs_fp)
add_dependencies(${CMAKE_PROJECT_NAME}.out acgu_adm_pc_bp25)
add_dependencies(${CMAKE_PROJECT_NAME}.out acgu_adm_pc_vg11_fm01)
add_dependencies(${CMAKE_PROJECT_NAME}.out acgu_adm_pc_vg11_fm02)


set_compiler_flags()

add_hex_output()
add_asm_output()
