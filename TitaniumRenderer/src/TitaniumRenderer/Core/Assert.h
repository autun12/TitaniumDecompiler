#pragma once

#include "TitaniumRenderer/Core/Base.h"
#include "TitaniumRenderer/Core/Log.h"
#include <filesystem>

#ifdef TD_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define TD_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { TD##type##ERROR(msg, __VA_ARGS__); TD_DEBUGBREAK(); } }
	#define TD_INTERNAL_ASSERT_WITH_MSG(type, check, ...) TD_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define TD_INTERNAL_ASSERT_NO_MSG(type, check) TD_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", TD_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define TD_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define TD_INTERNAL_ASSERT_GET_MACRO(...) TD_EXPAND_MACRO( TD_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, TD_INTERNAL_ASSERT_WITH_MSG, TD_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define TD_ASSERT(...) TD_EXPAND_MACRO( TD_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define TD_CORE_ASSERT(...) TD_EXPAND_MACRO( TD_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define TD_ASSERT(...)
	#define TD_CORE_ASSERT(...)
#endif