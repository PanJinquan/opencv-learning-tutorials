cmake_minimum_required(VERSION 3.5)
# 常用的变量：
# . 表示当前目录
# ${PROJECT_SOURCE_DIR}：工程的根目录  

# 指定头文件目录,PROJECT_SOURCE_DIR为工程的根目录  
include_directories(${PROJECT_SOURCE_DIR}/include)



#指定可执行文件的输出目录，输出到bin下面  
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#指定库文件输出路径  
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


# 将指定的源文件生成链接文件myhello
# add_library(myhello myhello.cpp)
# 更加便捷的方法是使用aux_source_directory(<dir> <variable>)
# 查找当前目录下的所有源文件，并将名称保存到 DIR_SRCS 变量中
aux_source_directory(${PROJECT_SOURCE_DIR}/myhello DIR_SRCS)

add_library(my_lib ${DIR_SRCS})
#set_target_properties(my_lib PROPERTIES OUTPUT_NAME "my_lib")
