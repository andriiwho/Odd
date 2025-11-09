Odd_CreateModule(D3D12RHI)

set(AGILITY_SDK_DIR "${ODD_THIRD_PARTY_DIR}/AgilitySDK" CACHE STRING "" FORCE)

target_include_directories(D3D12RHI PRIVATE ${AGILITY_SDK_DIR}/Include)
target_include_directories(D3D12RHI PRIVATE ${AGILITY_SDK_DIR}/Include/d3dx12)
target_link_libraries(D3D12RHI PRIVATE OddRHICore)
target_sources(D3D12RHI 
	PRIVATE 
		${AGILITY_SDK_DIR}/Src/d3dx12/d3dx12_property_format_table.cpp)

macro(Odd_CopyD3D12Agility targetName)	
	set(D3D12_LIBS 
		${AGILITY_SDK_DIR}/Bin/Win64/D3D12Core.dll 
		${AGILITY_SDK_DIR}/Bin/Win64/d3d12SDKLayers.dll)

	foreach(lib ${D3D12_LIBS})
	get_filename_component(libName ${lib} NAME)
	add_custom_command(TARGET ${targetName} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${targetName}>/D3D12)
	add_custom_command(TARGET ${targetName} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${lib}" $<TARGET_FILE_DIR:${targetName}>/D3D12/)
	endforeach()
endmacro()