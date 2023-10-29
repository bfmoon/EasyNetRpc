#当前目录下所有文件
aux_source_directory(. SRC_LIST)
#生成库文件
add_library(mprpc ${SRC_LIST})

target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)