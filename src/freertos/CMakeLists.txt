include(FetchContent)

FetchContent_Declare(freertos_kernel
        GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
        GIT_TAG        V11.2.0
)

add_library(freertos_config INTERFACE)

target_include_directories(freertos_config SYSTEM INTERFACE include)
target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)

set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)
set(FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)

FetchContent_MakeAvailable(freertos_kernel)

add_library(freertos OBJECT
        impl.c
        rtos_setup.c
        startup.c
        sys.c
        timer_freertos.c
)

target_include_directories(freertos PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        $<TARGET_PROPERTY:core,INCLUDE_DIRECTORIES>
)
target_link_libraries(freertos PRIVATE core freertos_config freertos_kernel)
target_compile_options(freertos PUBLIC
        -Wall
        -gdwarf-4
)

add_executable(ptpd2_freertos ptpd.c)
target_link_libraries(ptpd2_freertos PUBLIC core unix)
target_compile_features(ptpd2_freertos PUBLIC c_std_99)

#ptpd_add_subdirectory_if(TESTING unit_tests)
