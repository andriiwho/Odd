Odd_CreateModule(LuaCore)

target_include_directories(LuaCore PUBLIC 
	${ODD_THIRD_PARTY_DIR}/LuaJIT/Include
	${ODD_THIRD_PARTY_DIR}/Sol2/Include
)

if (WIN32)
	target_link_directories(LuaCore PUBLIC
		${ODD_THIRD_PARTY_DIR}/LuaJIT/Bin/Win64
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

set(LUA_BIN_DIR ${ODD_THIRD_PARTY_DIR}/LuaJIT/Bin CACHE STRING "" FORCE)
macro(Odd_CopyLuaLibrary targetName)
	if (WIN32)
		add_custom_command(TARGET ${targetName} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${LUA_BIN_DIR}/Win64/lua51.dll"
			$<TARGET_FILE_DIR:${targetName}>)
	endif()
endmacro()