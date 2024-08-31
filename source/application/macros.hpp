#ifndef Application_Macros_hpp
#define Application_Macros_hpp

#ifndef FORCE_INLINE
	#if defined(__GNUC__)
		#define FORCE_INLINE [[gnu::always_inline]]
	#elif defined(__clang__)
		#define FORCE_INLINE [[clang::always_inline]]
	#elif defined(_MSC_VER)
		#define FORCE_INLINE [[msvc::forceinline]]
	#else
		#define FORCE_INLINE [[]]
	#endif
#else
	#error Macro `FORCE_INLINE` is already defined.
#endif

#ifndef SUPPRESS_WARNINGS
	#if defined (__GNUC__)
		#define SUPPRESS_WARNINGS(section) \
		_Pragma("GCC diagnostic push") \
		_Pragma("GCC diagnostic ignored \"-Wall\"") \
		_Pragma("GCC diagnostic ignored \"-Wextra\"") \
		_Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
		_Pragma("GCC diagnostic ignored \"-Wold-style-cast\"") \
		_Pragma("GCC diagnostic ignored \"-Wzero-as-null-pointer-constant\"") \
		section \
		_Pragma("GCC diagnostic pop")
	#else
		#define SUPPRESS_WARNINGS(section) section
	#endif
#else
	#error Macro `SUPPRESS_WARNINGS` is already defined.
#endif

#ifndef SUPPRESS_DEPRECATION_WARNINGS
	#if defined (__GNUC__)
		#define SUPPRESS_DEPRECATION_WARNINGS(section) \
		_Pragma("GCC diagnostic push") \
		_Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"") \
		section \
		_Pragma("GCC diagnostic pop")
	#else
		#define SUPPRESS_DEPRECATION_WARNINGS(section) section
	#endif
#else
	#error Macro `SUPPRESS_DEPRECATION_WARNINGS` is already defined.
#endif

#endif
