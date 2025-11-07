# Odd uses modular system.
# To create a module there are useful macros that can be used.
# - Odd_CreateModule creates a module.
# - Odd_CreateExecutable creates an executable.
#
# All modules are static libraries by default.

macro(Odd_CreateModule module_name)
	add_library(${module_name} STATIC)
	file(GLOB_RECURSE SOURCE_FILES 
		${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h 
		${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h 
		${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp)
	target_sources(${module_name} PRIVATE ${SOURCE_FILES})
	target_include_directories(${module_name} 
		PUBLIC 
			${CMAKE_CURRENT_SOURCE_DIR}/Public 
		PRIVATE 
			${CMAKE_CURRENT_SOURCE_DIR}/Private)
	target_compile_features(${module_name} PUBLIC cxx_std_23)
	target_compile_definitions(${module_name} PRIVATE ODD_MODULE_${module_name}=1)
endmacro()

macro(Odd_CreateExecutable exe_name)
	add_executable(${exe_name})
	file(GLOB_RECURSE SOURCE_FILES 
		${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h 
		${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h 
		${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp)
	target_sources(${exe_name} PRIVATE ${SOURCE_FILES})
	target_include_directories(${exe_name} 
		PUBLIC 
			${CMAKE_CURRENT_SOURCE_DIR}/Public 
		PRIVATE 
			${CMAKE_CURRENT_SOURCE_DIR}/Private)
	target_compile_features(${exe_name} PUBLIC cxx_std_23)
	target_compile_definitions(${exe_name} PRIVATE ODD_MODULE_${exe_name}=1)
endmacro()