add_subdirectory(Core)

add_library(Odd INTERFACE)
target_link_libraries(Odd INTERFACE 
	OddCore
	RootObj
	LuaCore)