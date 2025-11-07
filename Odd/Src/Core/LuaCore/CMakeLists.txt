Odd_CreateModule(LuaCore)

target_include_directories(LuaCore PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Include
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Include/lua
)

if (WIN32)
	target_link_directories(LuaCore PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Bin/Win64
	)
else()
	message(FATAL_ERROR "Only Win64 is supported at the moment.")
endif()

target_link_libraries(LuaCore 
	PUBLIC 
		OddCore
		RootObj
	PRIVATE
		luajit
		lua51)

macro(Odd_CopyLuaLibrary targetName)
	if (WIN32)
		add_custom_command(TARGET ${targetName} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Bin/Win64/lua51.dll"
			$<TARGET_FILE_DIR:${targetName}>)
	endif()
endmacro()