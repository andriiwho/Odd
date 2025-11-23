# Odd uses modular system.
# To create a module there are useful macros that can be used.
# - Odd_CreateModule creates a module.
# - Odd_CreateExecutable creates an executable.
#
# All modules are static libraries by default.

# Option to enable/disable automatic PCH generation
option(ODD_ENABLE_PCH "Enable automatic precompiled header generation for modules" ON)

# Function to scan source files and generate a precompiled header
function(Odd_GeneratePrecompiledHeader target_name)
	if(NOT ODD_ENABLE_PCH)
		return()
	endif()
	
	# Get all source files from the target
	get_target_property(SOURCE_FILES ${target_name} SOURCES)
	
	# Check if we got any source files
	if(NOT SOURCE_FILES OR SOURCE_FILES STREQUAL "SOURCE_FILES-NOTFOUND")
		message(STATUS "[PCH] Skipped ${target_name} (no source files found)")
		return()
	endif()
	
	# List of common standard library headers to check for
	set(STD_HEADERS
		"<algorithm>"
		"<array>"
		"<atomic>"
		"<barrier>"
		"<bit>"
		"<bitset>"
		"<cassert>"
		"<cctype>"
		"<cerrno>"
		"<cfloat>"
		"<charconv>"
		"<chrono>"
		"<cinttypes>"
		"<climits>"
		"<cmath>"
		"<codecvt>"
		"<compare>"
		"<complex>"
		"<concepts>"
		"<condition_variable>"
		"<coroutine>"
		"<cstdarg>"
		"<cstddef>"
		"<cstdint>"
		"<cstdio>"
		"<cstdlib>"
		"<cstring>"
		"<ctime>"
		"<deque>"
		"<exception>"
		"<execution>"
		"<expected>"
		"<filesystem>"
		"<flat_map>"
		"<flat_set>"
		"<format>"
		"<forward_list>"
		"<fstream>"
		"<functional>"
		"<future>"
		"<generator>"
		"<initializer_list>"
		"<iomanip>"
		"<ios>"
		"<iosfwd>"
		"<iostream>"
		"<istream>"
		"<iterator>"
		"<latch>"
		"<limits>"
		"<list>"
		"<locale>"
		"<map>"
		"<mdspan>"
		"<memory>"
		"<memory_resource>"
		"<mutex>"
		"<new>"
		"<numbers>"
		"<numeric>"
		"<optional>"
		"<ostream>"
		"<print>"
		"<queue>"
		"<random>"
		"<ranges>"
		"<ratio>"
		"<regex>"
		"<scoped_allocator>"
		"<semaphore>"
		"<set>"
		"<shared_mutex>"
		"<source_location>"
		"<span>"
		"<spanstream>"
		"<sstream>"
		"<stack>"
		"<stacktrace>"
		"<stdexcept>"
		"<stdfloat>"
		"<stop_token>"
		"<streambuf>"
		"<string>"
		"<string_view>"
		"<syncstream>"
		"<system_error>"
		"<thread>"
		"<tuple>"
		"<type_traits>"
		"<typeindex>"
		"<typeinfo>"
		"<unordered_map>"
		"<unordered_set>"
		"<utility>"
		"<valarray>"
		"<variant>"
		"<vector>"
		"<version>"
	)
	
	# Add spdlog and fmt headers
	set(THIRD_PARTY_HEADERS
		"<spdlog/spdlog.h>"
		"<spdlog/logger.h>"
		"<spdlog/sinks/basic_file_sink.h>"
		"<spdlog/sinks/stdout_color_sinks.h>"
		"<spdlog/async.h>"
		"<spdlog/fmt/fmt.h>"
		"<spdlog/fmt/ostr.h>"
		"<fmt/core.h>"
		"<fmt/format.h>"
		"<fmt/ranges.h>"
		"<fmt/chrono.h>"
	)
	
	# Add OddCore headers (internal framework headers that should be precompiled)
	set(ODDCORE_HEADERS
		"\"OddCore.h\""
		"\"Macros.h\""
		"\"Types.h\""
		"\"OddMalloc.h\""
		"\"OddWaysToCrash.h\""
		"\"Helpers.h\""
		"\"Logging.h\""
		"\"OddConfig.h\""
		"\"RootObj.h\""
		"\"WeakObjPtr.h\""
		"\"SharedPtr.h\""
	)
	
	# Create a set to track which headers are found
	set(FOUND_HEADERS)
	
	# Get the source directory for the target to resolve relative paths
	get_target_property(TARGET_SOURCE_DIR ${target_name} SOURCE_DIR)
	
	# Scan all source files
	foreach(SOURCE_FILE ${SOURCE_FILES})
		# Only scan .cpp and .h files
		get_filename_component(FILE_EXT ${SOURCE_FILE} EXT)
		if(FILE_EXT STREQUAL ".cpp" OR FILE_EXT STREQUAL ".h")
			# Resolve the full path if it's relative
			if(NOT IS_ABSOLUTE ${SOURCE_FILE})
				set(SOURCE_FILE "${TARGET_SOURCE_DIR}/${SOURCE_FILE}")
			endif()
			
			# Read the file
			if(EXISTS ${SOURCE_FILE})
				file(READ ${SOURCE_FILE} FILE_CONTENTS)
				
				# Check for each standard library header
				foreach(HEADER ${STD_HEADERS})
					# Escape special regex characters
					string(REPLACE "<" "\\<" HEADER_REGEX ${HEADER})
					string(REPLACE ">" "\\>" HEADER_REGEX ${HEADER_REGEX})
					
					# Check if the header is included
					if(FILE_CONTENTS MATCHES "#include[ \t]+${HEADER_REGEX}")
						list(APPEND FOUND_HEADERS ${HEADER})
					endif()
				endforeach()
				
				# Check for third-party headers
				foreach(HEADER ${THIRD_PARTY_HEADERS})
					string(REPLACE "<" "\\<" HEADER_REGEX ${HEADER})
					string(REPLACE ">" "\\>" HEADER_REGEX ${HEADER_REGEX})
					
					if(FILE_CONTENTS MATCHES "#include[ \t]+${HEADER_REGEX}")
						list(APPEND FOUND_HEADERS ${HEADER})
					endif()
				endforeach()
				
				# Check for OddCore headers
				foreach(HEADER ${ODDCORE_HEADERS})
					string(REPLACE "\"" "\\\"" HEADER_REGEX ${HEADER})
					
					if(FILE_CONTENTS MATCHES "#include[ \t]+${HEADER_REGEX}")
						list(APPEND FOUND_HEADERS ${HEADER})
					endif()
				endforeach()
			endif()
		endif()
	endforeach()
	
	# Remove duplicates
	if(FOUND_HEADERS)
		list(REMOVE_DUPLICATES FOUND_HEADERS)
		
		# Sort the headers for consistency
		list(SORT FOUND_HEADERS)
		
		# Generate PCH file path
		set(PCH_DIR "${CMAKE_BINARY_DIR}/PCH/${target_name}")
		set(PCH_FILE "${PCH_DIR}/${target_name}_pch.h")
		
		# Create PCH directory
		file(MAKE_DIRECTORY ${PCH_DIR})
		
		# Generate PCH content
		set(PCH_CONTENT "// Auto-generated precompiled header for ${target_name}\n")
		set(PCH_CONTENT "${PCH_CONTENT}// Generated by Odd_GeneratePrecompiledHeader\n")
		set(PCH_CONTENT "${PCH_CONTENT}// Do not edit manually - this file is regenerated on each CMake configure\n\n")
		set(PCH_CONTENT "${PCH_CONTENT}#pragma once\n\n")
		
		# Separate standard library and third-party headers
		set(STD_FOUND_LIST)
		set(THIRD_PARTY_FOUND_LIST)
		set(ODDCORE_FOUND_LIST)
		
		foreach(HEADER ${FOUND_HEADERS})
			list(FIND THIRD_PARTY_HEADERS ${HEADER} IS_THIRD_PARTY)
			list(FIND ODDCORE_HEADERS ${HEADER} IS_ODDCORE)
			if(IS_THIRD_PARTY GREATER -1)
				list(APPEND THIRD_PARTY_FOUND_LIST ${HEADER})
			elseif(IS_ODDCORE GREATER -1)
				list(APPEND ODDCORE_FOUND_LIST ${HEADER})
			else()
				list(APPEND STD_FOUND_LIST ${HEADER})
			endif()
		endforeach()
		
		# Add standard library headers
		if(STD_FOUND_LIST)
			set(PCH_CONTENT "${PCH_CONTENT}// Standard Library Headers\n")
			foreach(HEADER ${STD_FOUND_LIST})
				set(PCH_CONTENT "${PCH_CONTENT}#include ${HEADER}\n")
			endforeach()
			set(PCH_CONTENT "${PCH_CONTENT}\n")
		endif()
		
		# Add third-party headers
		if(THIRD_PARTY_FOUND_LIST)
			set(PCH_CONTENT "${PCH_CONTENT}// Third-Party Library Headers\n")
			foreach(HEADER ${THIRD_PARTY_FOUND_LIST})
				set(PCH_CONTENT "${PCH_CONTENT}#include ${HEADER}\n")
			endforeach()
			set(PCH_CONTENT "${PCH_CONTENT}\n")
		endif()
		
		# Add OddCore headers
		if(ODDCORE_FOUND_LIST)
			set(PCH_CONTENT "${PCH_CONTENT}// OddCore Framework Headers\n")
			foreach(HEADER ${ODDCORE_FOUND_LIST})
				set(PCH_CONTENT "${PCH_CONTENT}#include ${HEADER}\n")
			endforeach()
		endif()
		
		# Count headers for status message
		list(LENGTH FOUND_HEADERS HEADER_COUNT)
		
		# Write PCH file only if content changed (to avoid unnecessary rebuilds)
		set(WRITE_PCH TRUE)
		if(EXISTS ${PCH_FILE})
			file(READ ${PCH_FILE} EXISTING_CONTENT)
			if(EXISTING_CONTENT STREQUAL PCH_CONTENT)
				set(WRITE_PCH FALSE)
			endif()
		endif()
		
		if(WRITE_PCH)
			file(WRITE ${PCH_FILE} ${PCH_CONTENT})
			message(STATUS "[PCH] Generated for ${target_name} (${HEADER_COUNT} headers)")
		else()
			message(STATUS "[PCH] Up-to-date for ${target_name} (${HEADER_COUNT} headers)")
		endif()
		
		# Apply PCH to target
		target_precompile_headers(${target_name} PRIVATE ${PCH_FILE})
		
	else()
		message(STATUS "[PCH] Skipped ${target_name} (no common headers found)")
	endif()
endfunction()

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
	target_compile_definitions(${module_name} PRIVATE ODD_MODULE_${module_name}=1 FMT_UNICODE=0)
	
	# Generate and apply precompiled header
	Odd_GeneratePrecompiledHeader(${module_name})
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
	target_compile_definitions(${exe_name} PRIVATE ODD_MODULE_${exe_name}=1 FMT_UNICODE=0)
	
	# Generate and apply precompiled header
	Odd_GeneratePrecompiledHeader(${exe_name})
endmacro()

# This should be run when configuring the project.
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/Binaries)
	make_directory(${CMAKE_SOURCE_DIR}/Binaries)
	file(WRITE ${CMAKE_SOURCE_DIR}/Binaries/.gitignore "*")
endif()

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/BuildTree/.gitignore)
	file(WRITE ${CMAKE_SOURCE_DIR}/BuildTree/.gitignore "*")
endif()