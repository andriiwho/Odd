Odd_CreateModule(OddCore)

target_include_directories(OddCore 
	PUBLIC 
		${ODD_THIRD_PARTY_DIR}/SpdLog/Include)

target_link_libraries(OddCore PUBLIC yaml-cpp::yaml-cpp)