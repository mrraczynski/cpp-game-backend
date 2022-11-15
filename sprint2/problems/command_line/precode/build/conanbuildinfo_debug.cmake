
#################
###  BOOST
#################
set(CONAN_BOOST_ROOT_DEBUG "C:/.conan/621dd6/1")
set(CONAN_INCLUDE_DIRS_BOOST_DEBUG "C:/.conan/621dd6/1/include")
set(CONAN_LIB_DIRS_BOOST_DEBUG "C:/.conan/621dd6/1/lib")
set(CONAN_BIN_DIRS_BOOST_DEBUG )
set(CONAN_RES_DIRS_BOOST_DEBUG )
set(CONAN_SRC_DIRS_BOOST_DEBUG )
set(CONAN_BUILD_DIRS_BOOST_DEBUG "C:/.conan/621dd6/1/")
set(CONAN_FRAMEWORK_DIRS_BOOST_DEBUG )
set(CONAN_LIBS_BOOST_DEBUG libboost_contract libboost_coroutine libboost_fiber_numa libboost_fiber libboost_context libboost_graph libboost_iostreams libboost_json libboost_log_setup libboost_log libboost_locale libboost_math_c99 libboost_math_c99f libboost_math_c99l libboost_math_tr1 libboost_math_tr1f libboost_math_tr1l libboost_nowide libboost_program_options libboost_random libboost_regex libboost_stacktrace_noop libboost_stacktrace_windbg libboost_stacktrace_windbg_cached libboost_timer libboost_type_erasure libboost_thread libboost_atomic libboost_chrono libboost_container libboost_date_time libboost_unit_test_framework libboost_prg_exec_monitor libboost_test_exec_monitor libboost_exception libboost_wave libboost_filesystem libboost_wserialization libboost_serialization)
set(CONAN_PKG_LIBS_BOOST_DEBUG libboost_contract libboost_coroutine libboost_fiber_numa libboost_fiber libboost_context libboost_graph libboost_iostreams libboost_json libboost_log_setup libboost_log libboost_locale libboost_math_c99 libboost_math_c99f libboost_math_c99l libboost_math_tr1 libboost_math_tr1f libboost_math_tr1l libboost_nowide libboost_program_options libboost_random libboost_regex libboost_stacktrace_noop libboost_stacktrace_windbg libboost_stacktrace_windbg_cached libboost_timer libboost_type_erasure libboost_thread libboost_atomic libboost_chrono libboost_container libboost_date_time libboost_unit_test_framework libboost_prg_exec_monitor libboost_test_exec_monitor libboost_exception libboost_wave libboost_filesystem libboost_wserialization libboost_serialization)
set(CONAN_SYSTEM_LIBS_BOOST_DEBUG ole32 dbgeng bcrypt)
set(CONAN_FRAMEWORKS_BOOST_DEBUG )
set(CONAN_FRAMEWORKS_FOUND_BOOST_DEBUG "")  # Will be filled later
set(CONAN_DEFINES_BOOST_DEBUG "-DBOOST_STACKTRACE_USE_NOOP"
			"-DBOOST_STACKTRACE_USE_WINDBG"
			"-DBOOST_STACKTRACE_USE_WINDBG_CACHED"
			"-DBOOST_ALL_NO_LIB")
set(CONAN_BUILD_MODULES_PATHS_BOOST_DEBUG )
# COMPILE_DEFINITIONS are equal to CONAN_DEFINES without -D, for targets
set(CONAN_COMPILE_DEFINITIONS_BOOST_DEBUG "BOOST_STACKTRACE_USE_NOOP"
			"BOOST_STACKTRACE_USE_WINDBG"
			"BOOST_STACKTRACE_USE_WINDBG_CACHED"
			"BOOST_ALL_NO_LIB")

set(CONAN_C_FLAGS_BOOST_DEBUG "")
set(CONAN_CXX_FLAGS_BOOST_DEBUG "")
set(CONAN_SHARED_LINKER_FLAGS_BOOST_DEBUG "")
set(CONAN_EXE_LINKER_FLAGS_BOOST_DEBUG "")

# For modern cmake targets we use the list variables (separated with ;)
set(CONAN_C_FLAGS_BOOST_DEBUG_LIST "")
set(CONAN_CXX_FLAGS_BOOST_DEBUG_LIST "")
set(CONAN_SHARED_LINKER_FLAGS_BOOST_DEBUG_LIST "")
set(CONAN_EXE_LINKER_FLAGS_BOOST_DEBUG_LIST "")

# Apple Frameworks
conan_find_apple_frameworks(CONAN_FRAMEWORKS_FOUND_BOOST_DEBUG "${CONAN_FRAMEWORKS_BOOST_DEBUG}" "_BOOST" "_DEBUG")
# Append to aggregated values variable
set(CONAN_LIBS_BOOST_DEBUG ${CONAN_PKG_LIBS_BOOST_DEBUG} ${CONAN_SYSTEM_LIBS_BOOST_DEBUG} ${CONAN_FRAMEWORKS_FOUND_BOOST_DEBUG})


#################
###  ZLIB
#################
set(CONAN_ZLIB_ROOT_DEBUG "C:/Users/mrrac/.conan/data/zlib/1.2.13/_/_/package/164640aad040835ac89882393a96d89200694f04")
set(CONAN_INCLUDE_DIRS_ZLIB_DEBUG "C:/Users/mrrac/.conan/data/zlib/1.2.13/_/_/package/164640aad040835ac89882393a96d89200694f04/include")
set(CONAN_LIB_DIRS_ZLIB_DEBUG "C:/Users/mrrac/.conan/data/zlib/1.2.13/_/_/package/164640aad040835ac89882393a96d89200694f04/lib")
set(CONAN_BIN_DIRS_ZLIB_DEBUG )
set(CONAN_RES_DIRS_ZLIB_DEBUG )
set(CONAN_SRC_DIRS_ZLIB_DEBUG )
set(CONAN_BUILD_DIRS_ZLIB_DEBUG "C:/Users/mrrac/.conan/data/zlib/1.2.13/_/_/package/164640aad040835ac89882393a96d89200694f04/")
set(CONAN_FRAMEWORK_DIRS_ZLIB_DEBUG )
set(CONAN_LIBS_ZLIB_DEBUG zlib)
set(CONAN_PKG_LIBS_ZLIB_DEBUG zlib)
set(CONAN_SYSTEM_LIBS_ZLIB_DEBUG )
set(CONAN_FRAMEWORKS_ZLIB_DEBUG )
set(CONAN_FRAMEWORKS_FOUND_ZLIB_DEBUG "")  # Will be filled later
set(CONAN_DEFINES_ZLIB_DEBUG )
set(CONAN_BUILD_MODULES_PATHS_ZLIB_DEBUG )
# COMPILE_DEFINITIONS are equal to CONAN_DEFINES without -D, for targets
set(CONAN_COMPILE_DEFINITIONS_ZLIB_DEBUG )

set(CONAN_C_FLAGS_ZLIB_DEBUG "")
set(CONAN_CXX_FLAGS_ZLIB_DEBUG "")
set(CONAN_SHARED_LINKER_FLAGS_ZLIB_DEBUG "")
set(CONAN_EXE_LINKER_FLAGS_ZLIB_DEBUG "")

# For modern cmake targets we use the list variables (separated with ;)
set(CONAN_C_FLAGS_ZLIB_DEBUG_LIST "")
set(CONAN_CXX_FLAGS_ZLIB_DEBUG_LIST "")
set(CONAN_SHARED_LINKER_FLAGS_ZLIB_DEBUG_LIST "")
set(CONAN_EXE_LINKER_FLAGS_ZLIB_DEBUG_LIST "")

# Apple Frameworks
conan_find_apple_frameworks(CONAN_FRAMEWORKS_FOUND_ZLIB_DEBUG "${CONAN_FRAMEWORKS_ZLIB_DEBUG}" "_ZLIB" "_DEBUG")
# Append to aggregated values variable
set(CONAN_LIBS_ZLIB_DEBUG ${CONAN_PKG_LIBS_ZLIB_DEBUG} ${CONAN_SYSTEM_LIBS_ZLIB_DEBUG} ${CONAN_FRAMEWORKS_FOUND_ZLIB_DEBUG})


#################
###  BZIP2
#################
set(CONAN_BZIP2_ROOT_DEBUG "C:/Users/mrrac/.conan/data/bzip2/1.0.8/_/_/package/d74f69022acf854c1ce6f27efa07ccbb6504e4b1")
set(CONAN_INCLUDE_DIRS_BZIP2_DEBUG "C:/Users/mrrac/.conan/data/bzip2/1.0.8/_/_/package/d74f69022acf854c1ce6f27efa07ccbb6504e4b1/include")
set(CONAN_LIB_DIRS_BZIP2_DEBUG "C:/Users/mrrac/.conan/data/bzip2/1.0.8/_/_/package/d74f69022acf854c1ce6f27efa07ccbb6504e4b1/lib")
set(CONAN_BIN_DIRS_BZIP2_DEBUG "C:/Users/mrrac/.conan/data/bzip2/1.0.8/_/_/package/d74f69022acf854c1ce6f27efa07ccbb6504e4b1/bin")
set(CONAN_RES_DIRS_BZIP2_DEBUG )
set(CONAN_SRC_DIRS_BZIP2_DEBUG )
set(CONAN_BUILD_DIRS_BZIP2_DEBUG "C:/Users/mrrac/.conan/data/bzip2/1.0.8/_/_/package/d74f69022acf854c1ce6f27efa07ccbb6504e4b1/")
set(CONAN_FRAMEWORK_DIRS_BZIP2_DEBUG )
set(CONAN_LIBS_BZIP2_DEBUG bz2)
set(CONAN_PKG_LIBS_BZIP2_DEBUG bz2)
set(CONAN_SYSTEM_LIBS_BZIP2_DEBUG )
set(CONAN_FRAMEWORKS_BZIP2_DEBUG )
set(CONAN_FRAMEWORKS_FOUND_BZIP2_DEBUG "")  # Will be filled later
set(CONAN_DEFINES_BZIP2_DEBUG )
set(CONAN_BUILD_MODULES_PATHS_BZIP2_DEBUG )
# COMPILE_DEFINITIONS are equal to CONAN_DEFINES without -D, for targets
set(CONAN_COMPILE_DEFINITIONS_BZIP2_DEBUG )

set(CONAN_C_FLAGS_BZIP2_DEBUG "")
set(CONAN_CXX_FLAGS_BZIP2_DEBUG "")
set(CONAN_SHARED_LINKER_FLAGS_BZIP2_DEBUG "")
set(CONAN_EXE_LINKER_FLAGS_BZIP2_DEBUG "")

# For modern cmake targets we use the list variables (separated with ;)
set(CONAN_C_FLAGS_BZIP2_DEBUG_LIST "")
set(CONAN_CXX_FLAGS_BZIP2_DEBUG_LIST "")
set(CONAN_SHARED_LINKER_FLAGS_BZIP2_DEBUG_LIST "")
set(CONAN_EXE_LINKER_FLAGS_BZIP2_DEBUG_LIST "")

# Apple Frameworks
conan_find_apple_frameworks(CONAN_FRAMEWORKS_FOUND_BZIP2_DEBUG "${CONAN_FRAMEWORKS_BZIP2_DEBUG}" "_BZIP2" "_DEBUG")
# Append to aggregated values variable
set(CONAN_LIBS_BZIP2_DEBUG ${CONAN_PKG_LIBS_BZIP2_DEBUG} ${CONAN_SYSTEM_LIBS_BZIP2_DEBUG} ${CONAN_FRAMEWORKS_FOUND_BZIP2_DEBUG})


### Definition of global aggregated variables ###

set(CONAN_DEPENDENCIES_DEBUG boost zlib bzip2)

set(CONAN_INCLUDE_DIRS_DEBUG "C:/.conan/621dd6/1/include"
			"C:/Users/mrrac/.conan/data/zlib/1.2.13/_/_/package/164640aad040835ac89882393a96d89200694f04/include"
			"C:/Users/mrrac/.conan/data/bzip2/1.0.8/_/_/package/d74f69022acf854c1ce6f27efa07ccbb6504e4b1/include" ${CONAN_INCLUDE_DIRS_DEBUG})
set(CONAN_LIB_DIRS_DEBUG "C:/.conan/621dd6/1/lib"
			"C:/Users/mrrac/.conan/data/zlib/1.2.13/_/_/package/164640aad040835ac89882393a96d89200694f04/lib"
			"C:/Users/mrrac/.conan/data/bzip2/1.0.8/_/_/package/d74f69022acf854c1ce6f27efa07ccbb6504e4b1/lib" ${CONAN_LIB_DIRS_DEBUG})
set(CONAN_BIN_DIRS_DEBUG "C:/Users/mrrac/.conan/data/bzip2/1.0.8/_/_/package/d74f69022acf854c1ce6f27efa07ccbb6504e4b1/bin" ${CONAN_BIN_DIRS_DEBUG})
set(CONAN_RES_DIRS_DEBUG  ${CONAN_RES_DIRS_DEBUG})
set(CONAN_FRAMEWORK_DIRS_DEBUG  ${CONAN_FRAMEWORK_DIRS_DEBUG})
set(CONAN_LIBS_DEBUG libboost_contract libboost_coroutine libboost_fiber_numa libboost_fiber libboost_context libboost_graph libboost_iostreams libboost_json libboost_log_setup libboost_log libboost_locale libboost_math_c99 libboost_math_c99f libboost_math_c99l libboost_math_tr1 libboost_math_tr1f libboost_math_tr1l libboost_nowide libboost_program_options libboost_random libboost_regex libboost_stacktrace_noop libboost_stacktrace_windbg libboost_stacktrace_windbg_cached libboost_timer libboost_type_erasure libboost_thread libboost_atomic libboost_chrono libboost_container libboost_date_time libboost_unit_test_framework libboost_prg_exec_monitor libboost_test_exec_monitor libboost_exception libboost_wave libboost_filesystem libboost_wserialization libboost_serialization zlib bz2 ${CONAN_LIBS_DEBUG})
set(CONAN_PKG_LIBS_DEBUG libboost_contract libboost_coroutine libboost_fiber_numa libboost_fiber libboost_context libboost_graph libboost_iostreams libboost_json libboost_log_setup libboost_log libboost_locale libboost_math_c99 libboost_math_c99f libboost_math_c99l libboost_math_tr1 libboost_math_tr1f libboost_math_tr1l libboost_nowide libboost_program_options libboost_random libboost_regex libboost_stacktrace_noop libboost_stacktrace_windbg libboost_stacktrace_windbg_cached libboost_timer libboost_type_erasure libboost_thread libboost_atomic libboost_chrono libboost_container libboost_date_time libboost_unit_test_framework libboost_prg_exec_monitor libboost_test_exec_monitor libboost_exception libboost_wave libboost_filesystem libboost_wserialization libboost_serialization zlib bz2 ${CONAN_PKG_LIBS_DEBUG})
set(CONAN_SYSTEM_LIBS_DEBUG ole32 dbgeng bcrypt ${CONAN_SYSTEM_LIBS_DEBUG})
set(CONAN_FRAMEWORKS_DEBUG  ${CONAN_FRAMEWORKS_DEBUG})
set(CONAN_FRAMEWORKS_FOUND_DEBUG "")  # Will be filled later
set(CONAN_DEFINES_DEBUG "-DBOOST_STACKTRACE_USE_NOOP"
			"-DBOOST_STACKTRACE_USE_WINDBG"
			"-DBOOST_STACKTRACE_USE_WINDBG_CACHED"
			"-DBOOST_ALL_NO_LIB" ${CONAN_DEFINES_DEBUG})
set(CONAN_BUILD_MODULES_PATHS_DEBUG  ${CONAN_BUILD_MODULES_PATHS_DEBUG})
set(CONAN_CMAKE_MODULE_PATH_DEBUG "C:/.conan/621dd6/1/"
			"C:/Users/mrrac/.conan/data/zlib/1.2.13/_/_/package/164640aad040835ac89882393a96d89200694f04/"
			"C:/Users/mrrac/.conan/data/bzip2/1.0.8/_/_/package/d74f69022acf854c1ce6f27efa07ccbb6504e4b1/" ${CONAN_CMAKE_MODULE_PATH_DEBUG})

set(CONAN_CXX_FLAGS_DEBUG " ${CONAN_CXX_FLAGS_DEBUG}")
set(CONAN_SHARED_LINKER_FLAGS_DEBUG " ${CONAN_SHARED_LINKER_FLAGS_DEBUG}")
set(CONAN_EXE_LINKER_FLAGS_DEBUG " ${CONAN_EXE_LINKER_FLAGS_DEBUG}")
set(CONAN_C_FLAGS_DEBUG " ${CONAN_C_FLAGS_DEBUG}")

# Apple Frameworks
conan_find_apple_frameworks(CONAN_FRAMEWORKS_FOUND_DEBUG "${CONAN_FRAMEWORKS_DEBUG}" "" "_DEBUG")
# Append to aggregated values variable: Use CONAN_LIBS instead of CONAN_PKG_LIBS to include user appended vars
set(CONAN_LIBS_DEBUG ${CONAN_LIBS_DEBUG} ${CONAN_SYSTEM_LIBS_DEBUG} ${CONAN_FRAMEWORKS_FOUND_DEBUG})
