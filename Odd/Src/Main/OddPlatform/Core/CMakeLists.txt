Odd_CreateModule(OddPlatformCore)

target_link_libraries(OddPlatformCore
	PUBLIC
		OddCore
		RootObj)

if (WIN32)
	target_link_libraries(OddPlatformCore PRIVATE OddPlatformWindows)
endif()