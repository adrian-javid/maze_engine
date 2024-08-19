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

#endif
