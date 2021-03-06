#ifndef DFG_DEFS_GJVQVKZL
#define DFG_DEFS_GJVQVKZL

/*
dfgDefs.hpp

Defines common macro definitions needed across the library. Since this is expected to be included practically everywhere,
it must be lightweigth, dependecy free and avoid identifier ambiguities.

Note: Due to the nature of the file, includes are to be avoided. 

*/

/*

Preprocessor options
DFG_NO_BOOST: If defined no boost libraries will be included in common headers.
*/

#define DFG_ROOT_NS			dfg
#define DFG_DETAIL_NS		dfDetail
#define DFG_ROOT_NS_BEGIN	namespace DFG_ROOT_NS

#define DFG_SUB_NS_NAME(x)	x
#define DFG_SUB_NS(x)	namespace DFG_SUB_NS_NAME(x)

#define DFG_CLASS_NAME(x)	x

#define DFG_MODULE_NS(MODULE) DFG_ROOT_NS::DFG_SUB_NS_NAME(MODULE)

#define DFG_STATIC_ASSERT(EXPR, MSG) static_assert(EXPR, MSG)

// MSVC specific: Auto detect console apps.
#if defined(_MSC_VER) && defined(_CONSOLE) && !defined(DFG_CONSOLE_APP)
	#define DFG_CONSOLE_APP 1
#endif

// DFG_STRINGIZE makes a string of given argument. If used with #defined value,
// the string is made of the contents of the defined value.
#define DFG_HELPER_STRINGIZE(x)			#x
#define DFG_STRINGIZE(x)				DFG_HELPER_STRINGIZE(x)

// To suppress "unused variable"-warning for example with unused function parameters.
// Usage: DFG_UNUSED(variableName);
// Related matter:
//   http://herbsutter.com/2009/10/18/mailbag-shutting-up-compiler-warnings/
//   http://stackoverflow.com/questions/4851075/universally-compiler-independant-way-of-implementing-an-unused-macro-in-c-c
//   http://stackoverflow.com/questions/7090998/the-unused-macro
namespace DFG_ROOT_NS { namespace DFG_DETAIL_NS
{
	template <class T> inline void unusedItem(const T&) {}
}}

#define DFG_UNUSED(x) DFG_ROOT_NS::DFG_DETAIL_NS::unusedItem(x); // Related code: UNREFERENCED_PARAMETER in winnt.h, Q_UNUSED in QT

// DFG__FUNCTION__: Statically available function name similar to __FUNCTION__ in VC.

#ifdef _MSC_VER
	#define DFG_CURRENT_FUNCTION_NAME			__FUNCTION__
	#define DFG_CURRENT_FUNCTION_SIGNATURE		__FUNCSIG__
	#define DFG_CURRENT_FUNCTION_DECORATED_NAME	__FUNCDNAME__
#else // case: !_MSC_VER
	#ifdef DFG_NO_BOOST
		#define DFG_CURRENT_FUNCTION_SIGNATURE	"<current function signature not implemented for current compiler>"
	#else // case: !DFG_NO_BOOST
		#include <boost/current_function.hpp>
		#define DFG_CURRENT_FUNCTION_SIGNATURE	BOOST_CURRENT_FUNCTION
	#endif

	#define DFG_CURRENT_FUNCTION_NAME			"<current function name not implemented for current compiler>"
	#define DFG_CURRENT_FUNCTION_DECORATED_NAME	"<current function decorated name not implemented for current compiler>"

#endif // _MSC_VER


#ifdef _MSC_VER
	// ---> DFG_TAG_DEPRECATED: mark item deprecated.
	// For example: DFG_TAG_DEPRECATED void someDeprecatedFunc();
	// TODO: Usage example.
	#define DFG_TAG_DEPRECATED	__declspec(deprecated)
	#define DFG_BEGIN_INCLUDE_WITH_DISABLED_WARNINGS \
		__pragma(warning(push, 1)) /* TODO: disable warnings instead of using level 1. */ \
		__pragma(warning(disable:4251 4265 4505 4701 4946)) // Note: 4505 can't be silenced with this (by design), see http://support.microsoft.com/kb/947783
		/*
		4251: "class 'x' needs to have dll-interface to be used by clients of class 'y'"
		4265: "class has virtual functions, but destructor is not virtual"
		4505: "unreferenced local function has been removed"
		4701: "potentially uninitialized local variable 'x' used"
		4946: "reinterpret_cast used between related classes: 'class1' and 'class2'"
		*/
	#define DFG_END_INCLUDE_WITH_DISABLED_WARNINGS __pragma(warning(pop))
#else
	// TODO: Implemented for other compilers
	#define DFG_TAG_DEPRECATED	
	#define DFG_BEGIN_INCLUDE_WITH_DISABLED_WARNINGS
	#define DFG_END_INCLUDE_WITH_DISABLED_WARNINGS
#endif
//

#define DFG_HIDE_COPY_CONSTRUCTOR(CLASS) \
	private: \
	CLASS(const CLASS&);
#define DFG_HIDE_ASSIGNMENT_OPERATOR(CLASS) \
	private: \
	void operator=(const CLASS&);
#define DFG_HIDE_COPY_CONSTRUCTOR_AND_COPY_ASSIGNMENT(CLASS) \
	private: \
	void operator=(const CLASS&); \
	CLASS(const CLASS&);

// Implementation of DFG_COUNTOF()
namespace DFG_ROOT_NS
{
    namespace DFG_DETAIL_NS
    {
        template <class T, unsigned int N> char(*StaticCountOfHelper(const T(&)[N]))[N]; // Return type is pointer to char array of size N.
    }
}

//  DFG_COUNTOF-macro computes the number of elements in a statically-allocated array.
// Related code: _countof (in MSVC)
#define DFG_COUNTOF(x) sizeof(*::DFG_ROOT_NS::DFG_DETAIL_NS::StaticCountOfHelper(x))

// Computes the number of elements excluding the trailing null in a string literal (CSL = C String Literal).
// zero terminated string.
// Example: DFG_COUNTOF_CSL("string") is 6.
// Note: If string contains embedded nulls, the return value differs from what strlen(x) would return.
#define DFG_COUNTOF_CSL(x) (DFG_COUNTOF(x) - 1)
#define DFG_COUNTOF_SZ(x) (DFG_COUNTOF(x) - 1)



#endif // include guard
