set(SDL_BUILD_TESTS OFF CACHE BOOL "")
set(SDL_BUILD_EXAMPLES OFF CACHE BOOL "")
add_subdirectory(SDL)
add_subdirectory(spdlog)

set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "")
set(YAML_CPP_DISABLE_UNINSTALL OFF CACHE BOOL "")
set(YAML_USE_SYSTEM_GTEST OFF CACHE BOOL "")
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory(yaml-cpp)

if (WIN32)
	add_subdirectory(D3D12MemoryAllocator)
endif()

if (MSVC)
	set(THIRD_PARTY_LIBRARIES 
		D3D12MemoryAllocator
		SDL_uclibc
		SDL3_test
		SDL3-shared
		spdlog
		yaml-cpp)
	foreach(lib ${THIRD_PARTY_LIBRARIES})
		Odd_ModuleFolder(${lib} "ThirdParty")
	endforeach()
endif()