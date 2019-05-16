# CMakeDemo2说明

## create_lib
create_lib项目指导生成一个库：MathFunctions
> CMakeLists.txt
```
cmake_minimum_required(VERSION 3.5)
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 指定生成 MathFunctions 链接库
add_library (MathFunctions ${DIR_LIB_SRCS})

# 指定 MathFunctions 库的安装路径
install (TARGETS MathFunctions DESTINATION ${CMAKE_SOURCE_DIR}/install/lib)
install (FILES MathFunctions.h DESTINATION ${CMAKE_SOURCE_DIR}/install/include)
```
- bash:
```
cd create_lib 
mkdir build 
cd build 
cmake .. 
make 
make install
```
- 运行后，会在create_lib下生成一个install目录，里面含有lib文件（libMathFunctions.a）和include文件（MathFunctions.h）,复制install目录到project目录下，名重新命名为“math”

## project
project指导如果使用这个库MathFunctions
- bash:
```
cd project 
mkdir build 
cd build 
cmake .. 
make
make install
```
> 指定可执行文件的输出目录，输出到bin下面 <br/>
> set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) <br>
> 指定库文件输出路径 <br>
> set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib) <br>
> 这时在project/bin生成一个可执行文件：Demo <br>

```
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
```