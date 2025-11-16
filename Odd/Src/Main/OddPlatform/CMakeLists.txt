add_subdirectory(Core)

if (WIN32 OR UNIX OR APPLE OR ANDROID)
	add_subdirectory(SDL)
endif()