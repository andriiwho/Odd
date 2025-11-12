Odd_CreateModule(OddPlatformCore)

target_link_libraries(OddPlatformCore
	PUBLIC
		OddCore
		OddRootObj)

if (WIN32)
	target_link_libraries(OddPlatformCore PRIVATE OddPlatformWindows)
endif()