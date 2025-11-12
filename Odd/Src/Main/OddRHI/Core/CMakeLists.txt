Odd_CreateModule(OddRHICore)

target_link_libraries(OddRHICore 
	PUBLIC
		OddCore
		OddPlatformCore)

if (WIN32)
	target_link_libraries(OddRHICore PRIVATE OddD3D12RHI)
endif()