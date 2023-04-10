cmake_minimum_required(VERSION 3.17)
project(SSTT)


set(SRC)
set(INCL)
set(LINKLIBS)
set(COMPILE_OPTION)

set(GRPC_DEFAULT_SSL_ROOTS_FILE_PATH C:/workplace/roots.pem)
set(GOOGLE_APPLICATION_CREDENTIALS C:/workplace/key/eternal-photon-379901-af0f9d61554d.json )

include(${CMAKE_CURRENT_LIST_DIR}/lib/WAV/WAV.cmake)

find_package(google_cloud_cpp_speech REQUIRED)
find_package(Boost 1.66 REQUIRED COMPONENTS program_options)
find_package(Threads)

#add_library(parse_arguments STATIC parse_arguments.cc parse_arguments.h)
#target_compile_features(parse_arguments PUBLIC cxx_std_14)
#target_link_libraries(
#   parse_arguments PUBLIC Boost::program_options
#                           google-cloud-cpp::cloud_speech_protos)
#if (MSVC)
    # The protobuf-generated files have warnings under the default MSVC
    # settings. We are not interested in these warnings, because we cannot fix
    # them.
#    target_compile_options(parse_arguments PUBLIC "/wd4244" "/wd4251")
#endif ()

list(APPEND SRC
	SSTT.cpp
	SSTT.h
	
	test.cpp
  )
list(APPEND INCL
  ${CMAKE_CURRENT_LIST_DIR}/.
  )
list(APPEND LINKLIBS
	#parse_arguments
	google-cloud-cpp::speech
    Boost::program_options
	Threads::Threads
  )
if(MSVC)
  list(APPEND COMPILE_OPTION
    )
else()
  list(APPEND COMPILE_OPTION
    )
endif()

add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME}    PRIVATE    ${INCL}  )
target_link_libraries(${PROJECT_NAME} ${LINKLIBS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTION} )
set_target_properties(${PROJECT_NAME} PROPERTIES  CXX_STANDARD 17 )