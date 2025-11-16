Odd_CreateExecutable(Even)
target_link_libraries(Even 
	PRIVATE 
		Odd
)

if (WIN32)
	Odd_CopyD3D12Agility(Even)
endif()