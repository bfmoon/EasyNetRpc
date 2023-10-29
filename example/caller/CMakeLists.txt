
set(SRC_LIST calluserservice.cc ../user.pb.cc)
add_executable(customer ${SRC_LIST})

target_link_libraries(customer  mprpc protobuf)