add_subdirectory(Core)
add_subdirectory(Null)

if (WIN32)
	add_subdirectory(D3D12)
endif()