Odd_CreateModule(OddPlatformCore)

target_link_libraries(OddPlatformCore
	PUBLIC
		OddCore
		OddRootObj)

if (WIN32 OR UNIX OR APPLE OR ANDROID)
	target_link_libraries(OddPlatformCore PRIVATE OddPlatformSDL)
endif()