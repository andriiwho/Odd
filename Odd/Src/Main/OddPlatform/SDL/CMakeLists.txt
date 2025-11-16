Odd_CreateModule(OddPlatformSDL)

target_link_libraries(OddPlatformSDL
	PUBLIC
		OddPlatformCore
		OddCore
		OddRootObj
		SDL3-shared)