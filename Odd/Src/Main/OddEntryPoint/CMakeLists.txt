Odd_CreateModule(OddEntryPoint)

target_link_libraries(OddEntryPoint PUBLIC 
	OddCore
	RootObj
	LuaCore
	OddRHICore
PRIVATE
	OddPlatformCore
)