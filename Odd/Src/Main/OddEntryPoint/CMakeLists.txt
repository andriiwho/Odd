Odd_CreateModule(OddEntryPoint)

target_link_libraries(OddEntryPoint PUBLIC 
	OddCore
	OddRootObj
	OddLuaCore
	OddRHICore
	OddConfig
PRIVATE
	OddPlatformCore
)