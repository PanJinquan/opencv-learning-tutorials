# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# STATUS:一般的打印信息，ERROR:错误打印信息
set(NAME "this is CMakeDemo")
message("cmake doing...")
message(STATUS "info : ${NAME}")

# 项目信息
project (CMakeDemo)

include_directories(${PROJECT_SOURCE_DIR}/math/include)


#指定可执行文件的输出目录，输出到bin下面
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#指定库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


#查找库的方法：库文件名libMathFunctions.a(静态库)，表示MathFunctions
#[1]通过FIND_LIBRARY()查找库的方法将库MathFunctions赋值给变量MATH_LIBS
FIND_LIBRARY(MATH_LIBS  MathFunctions ${PROJECT_SOURCE_DIR}/math/lib NO_DEFAULT_PATH)

#[2]也可以link_directories()直接设置库所在目录
#link_directories(${PROJECT_SOURCE_DIR}/math/lib)
#SET(MATH_LIBS MathFunctions)#再将库MathFunctions赋值给变量MATH_LIBS

message(STATUS "find lib : ${MATH_LIBS}")
# 链接到库MATH_LIBS
link_libraries(${MATH_LIBS})
#[3]当然可以直接链接到库文件,多个库请用空格分隔
#link_libraries(${PROJECT_SOURCE_DIR}/math/lib/libMathFunctions.a)


# 指定生成目标
add_executable(Demo main.cc)

# 如果使用了link_libraries链接库，就不需要target_link_libraries链接库
# target_link_libraries(Demo ${MATH_LIBS})# 添加链接库

# 将可执行文件install到bin目录
# install(TARGETS Demo DESTINATION ${CMAKE_SOURCE_DIR}/bin)
