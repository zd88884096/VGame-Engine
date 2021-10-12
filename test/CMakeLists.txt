include_directories("utility")
include_directories("../third_party/googletest/googletest/include")
include_directories("../third_party/googletest/googlemock/include")
link_directories("../third_party/googletest/build/lib")
add_subdirectory("unit_test")

if(MSVC)
	add_definitions(/arch:SSE)
	add_definitions(/arch:AVX2)
endif()



