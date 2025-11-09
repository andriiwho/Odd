Odd_CreateExecutable(Even)
target_link_libraries(Even 
	PRIVATE 
		Odd
)

Odd_CopyLuaLibrary(Even)

if (WIN32)
	Odd_CopyD3D12Agility(Even)
endif()