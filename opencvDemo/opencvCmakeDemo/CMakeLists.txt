cmake_minimum_required(VERSION 3.5)
#add_definitions(-std=c++11)
# 参考资料：
# http://www.hahack.com/codes/cmake/
# https://blog.csdn.net/weicao1990/article/details/72844995

project(CMakeDemo)
SET( CMAKE_CXX_FLAGS "-std=c++11 -O3")
#SET(OpenCV_DIR	D:/my_opencv/opencv/build)# WIN
find_package( OpenCV 3.4.5 REQUIRED )

# 指定头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories( ${OpenCV_INCLUDE_DIRS} )


# 指定可执行文件的输出目录，输出到bin下面
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#指定库文件输出路径
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 将src子工程加入到主工程，里面必须含有CMakeLists.txt文件
add_subdirectory(src)

#指定可执行文件的输出目录，输出到bin下面
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#指定库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(DEBBUG Debug.cpp)

set(MY_LIB my_lib)
# 在指定目录下查找库，并保存在LIBPATH变量中:
# find_library (<VAR> name1 [path1 path2 ...])
#find_library(MY_LIB my_lib ${PROJECT_SOURCE_DIR}/lib/Debug)

# 构成可执行文件
add_executable(CMAKE_DEMO ${DEBBUG} main.cpp)

# 链接的库文件
# link_libraries( ${LIB_MY})

# 添加链接库：将子模myhello链接到Demo中
#target_link_libraries (Demo ${LIB_MY_LIB})
target_link_libraries (CMAKE_DEMO ${MY_LIB} ${OpenCV_LIBS} )

