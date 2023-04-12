# See README for more details

find_package(google_cloud_cpp_speech REQUIRED)
find_package(Boost 1.66 REQUIRED COMPONENTS program_options)
find_package(Threads)

list(APPEND SRC
	${CMAKE_CURRENT_LIST_DIR}/SSTT.cpp
	${CMAKE_CURRENT_LIST_DIR}/SSTT.h

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