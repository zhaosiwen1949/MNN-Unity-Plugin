cmake_minimum_required(VERSION 3.31)
project(mnn_plugin)

set(CMAKE_CXX_STANDARD 17)

#mnn
set (MNN_SOURCE_ROOT "${CMAKE_SOURCE_DIR}/submodules/MNN")
#set (MNN_INSTALL_ROOT "${MNN_SOURCE_ROOT}/project/android/build_64")

message(INFO "MNN_SOURCE_ROOT: ${MNN_SOURCE_ROOT}")
#message(INFO "MNN_INSTALL_ROOT: ${MNN_INSTALL_ROOT}")

include_directories("${MNN_SOURCE_ROOT}/include/")

#diffusion
include_directories("${MNN_SOURCE_ROOT}/transformers/diffusion/engine/include/")

#llm
include_directories("${MNN_SOURCE_ROOT}/transformers/llm/engine/include")
#include_directories("${CMAKE_SOURCE_DIR}/include")

#audio
include_directories("${MNN_SOURCE_ROOT}/tools/audio/include")

# Add library paths
set(LIB_PATH "${CMAKE_SOURCE_DIR}/third_party/MNN/lib")

# Link the shared library
add_library(mnn_lib SHARED IMPORTED)

# 设置导入库的实际文件位置
set_target_properties(mnn_lib PROPERTIES
        IMPORTED_LOCATION "${LIB_PATH}/MNN.dll"
        IMPORTED_IMPLIB  "${LIB_PATH}/MNN.lib"
)


add_library(mnn_plugin SHARED mnn_plugin.cpp)

target_include_directories(mnn_plugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(mnn_plugin
        PRIVATE mnn_lib
)

add_executable(mnn_test mnn_test.cpp)

target_include_directories(mnn_test
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(mnn_test
        PRIVATE mnn_lib
)

# Windows平台特定：复制DLL到输出目录
if(WIN32)
    # 方法1：直接复制特定DLL
    add_custom_command(TARGET mnn_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/third_party/MNN/lib/MNN.dll"
            $<TARGET_FILE_DIR:mnn_test>
            COMMENT "Copying MNN.dll to executable directory"
    )

    add_custom_command(TARGET mnn_plugin POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/third_party/MNN/lib/MNN.dll"
            $<TARGET_FILE_DIR:mnn_plugin>
            COMMENT "Copying MNN.dll to executable directory"
    )

    # 方法2：如果有多个DLL，复制整个目录
    # add_custom_command(TARGET mnn_project POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_directory
    #         "${PROJECT_SOURCE_DIR}/third_party/MNN/lib"
    #         $<TARGET_FILE_DIR:mnn_project>
    #     COMMENT "Copying MNN libraries to executable directory"
    # )
endif()
