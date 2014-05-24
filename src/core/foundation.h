#ifndef	FOUNDATION_H //[
#define	FOUNDATION_H

#ifdef CANDO_COMPILE
#define GARBAGE_COLLECTION_INCLUDE "cando/clasp_gc.cc"
#define GC_INTERFACE_HEADER "cando/gc_interface.h"
#else
#define GARBAGE_COLLECTION_INCLUDE "main/clasp_gc.cc"
#define GC_INTERFACE_HEADER "main/gc_interface.h"
#endif

/*! Old way of doing #= and ## used alists which are slow
  Switch to hash-tables to speed things up */
#define USE_SHARP_EQUAL_HASH_TABLES 1

namespace std { class type_info; };

//
// USE_TAGGED_PTR_P0  determines whether a p0 pointer,  the most-derived-pointer is stored
//         in the tagged_ptr.   If you turn it on then tagged_ptr uses twice as much
//         memory
//
//
#define USE_TAGGED_PTR_P0 1




#define VARARGS

#ifdef	USEBOOSTPYTHON
#include "mainBoostPython.h"
#endif
#include "gcInterface.h"
//#include <cstdio>
#include <map>


//! Macro for attribute that causes symbols to be exposed
#define ATTR_WEAK __attribute__((weak))


#if defined(DEBUG_MPS)
#define DEBUG_MPS_MESSAGE(_bf_) printf("%s:%d %s\n", __FILE__, __LINE__, (_bf_).str().c_str())
#define DEBUG_MPS_ALLOCATION(addr,gcobject_addr,size,kind)  brcl_mps_debug_allocation(addr,gcobject_addr,size,kind)
//#define DEBUG_MPS_FIX1_BEFORE(base,smartaddr) brcl_mps_debug_fix1_before(base,smartaddr)
#define DEBUG_MPS_FIX_BEFORE(pbase,px,offset) brcl_mps_debug_fix_before(pbase,px,offset)
#define	DEBUG_MPS_FIX_AFTER(pbase,px) brcl_mps_debug_fix_after(pbase,px)
#define DEBUG_MPS_CONTAINER(ctype,cnt) brcl_mps_debug_container(ctype,#cnt,cnt.size())
#define DEBUG_SCAN_OBJECT(obj) brcl_mps_debug_scan_object(obj)
#else
#define DEBUG_MPS_ALLOCATION(addr,gcobject_addr,size,kind)
//#define DEBUG_MPS_FIX1_BEFORE(base,smartaddr)
#define DEBUG_MPS_MESSAGE(_bf_) 
#define DEBUG_MPS_FIX_BEFORE(pbase,px,offset)
#define	DEBUG_MPS_FIX_AFTER(pbase,px)
#define DEBUG_MPS_CONTAINER(ctype,cnt)
#define DEBUG_SCAN_OBJECT(obj)
#endif




#define brcl_unlikely(x) __builtin_expect((x),0)
#define brcl_likely(x) 	__builtin_expect((x),1)
#define UNLIKELY(x) brcl_unlikely(x)
#define LIKELY(x) brcl_likely(x)


typedef std::size_t class_id;


/*! Configure architecture dependent types */
#include "config.h"



/*! Use old Conditions system baked into C++
  OLD_CONDITIONS = 1
  otherwise OLD_CONDITIONS = 0
*/
#define	OLD_CONDITIONS 0


/*! Check for NULLs being put into data structures throughout the code
  - this is very slow but will trap errors - turn off unless debugging
  set to 1 to turn on checking of NULLS using invokePossibleMultipleValueFunction
  set to 0 if you want to turn it off
*/
#define DEBUG_TRAP_NULLS 0   


/*! Set to 1 to support CLOS_STREAMS
  Undefine if you dont want CLOS_STREAMS
 */
#define CLOS_STREAMS 1



//! Use factory functions in chem
#define INIT_TO_FACTORIES	1

/*! Set the following to 1 if you want to use intrusive smart_ptrs
  and 0 if you want to use boost::shared_ptr */

#define USE_INTRUSIVE_SMART_PTR 	1



// Switching to new AtomTree kinematics
// This breaks a lot of old code and depreciates a lot of other code 
#define	KINEMATICS	1

		//
		// While converting to subclasses for energy components
		// use only a few components at a time
		//
#define	USE_ALL_ENERGY_COMPONENTS	1


//
// (C) 2004 Christian E. Schafmeister
//
//
// Turn on global debugging options here
// Otherwise comment this out and let individual modules
// set their debugging level
//
//

//
//	Turn debugging on or off
//		define DEBUG_LEVEL_FULL at the top of a .cc file
//





//#undef SOURCE_DEBUG
//#define ENABLE_PROFILING 1
//
// Turn on INIT_DEBUG if you want LISP initialization log messages
// - there are lots and startup will be very slow
#undef INIT_DEBUG

// Debug CLOS 0 - no debug 1 - slight debug 2 - verbose debug
#define DEBUG_CLOS 0 // 2


/*! If CLOS is being implemented */
#define	CLOS 1


#define MULTIPLE_VALUES_LIMIT	64

#ifdef SOURCE_DEBUG
 #ifdef DEBUG_LEVEL_FULL_REFCOUNT //[
  #define	DEBUG_ON	1
  #define	CALLSTACK_ON	1
  #define	REFCOUNTDEBUG	1
 #else //][
  #ifdef	DEBUG_LEVEL_FULL //[
   #define	DEBUG_ON	1
   #define	CALLSTACK_ON	1
   #define DEBUG_ASSERTS	1
  #else //][
   #ifdef	DEBUG_LEVEL_SILENT //[
    #define	CALLSTACK_SILENT	1
    //#undef	DEBUG_ON
   #else //][
    //#undef	CALLSTACK_ON
    //#undef	CALLSTACK_SILENT
    //#undef	DEBUG_ON
   #endif //]
  #endif //]
 #endif //]
#endif

// Empty macros to scrape symbol and keyword symbol declarations from
// the source code
#define SYMBOL_EXPORT_SC_(p,x)
#define SYMBOL_SC_(p,x)

    /*! Use this here in the header to declare an extern function if you want */
#define	EXTERN_FN(x) extern T_sp fn_##x(Function_sp exec, Cons_sp args, Environment_sp env, Lisp_sp lisp);

/*! Use this used to bind the C++ function fn_##x that will have the name (x) in Lisp (with "_" converted to "-") */
#define DEFUN(pkg,x) defun(pkg, #x, &fn_##x, ARGS_fn_##x, DECL_fn_##x, DOCS_fn_##x, LOCK_fn_##x,  _lisp); 

#define Defun(x) core::af_def(CurrentPkg,#x,&af_##x, ARGS_af_##x, DECL_af_##x, DOCS_af_##x); 
#define Defun_maker(pkg,x) core::af_def(pkg,"make-" #x,&(x ## _O::make), ARGS_##x##_O_make, DECL_##x##_O_make, DOCS_##x##_O_make);

#define ClDefun(x) core::af_def(ClPkg,#x,&cl_##x, ARGS_cl_##x, DECL_cl_##x, DOCS_cl_##x); 
#define ClosDefun(x) core::af_def(ClosPkg,#x,&clos_##x, ARGS_clos_##x, DECL_clos_##x, DOCS_clos_##x); 
#define CoreDefun(x) core::af_def(CorePkg,#x,&core_##x, ARGS_core_##x, DECL_core_##x, DOCS_core_##x); 


/*! Use this used to bind the C++ function fn_##x that will have the name (x) in Lisp (with "_" converted to "-") */
#define DEFUN_EXPORT(pkg,x) defun(pkg, #x, &fn_##x, ARGS_fn_##x, DECL_fn_##x, DOCS_fn_##x, LOCK_fn_##x, _lisp); 

/*! Use this used to bind the C++ function fn_##x that will have the name (x) in Lisp (with "_" converted to "-") */
#define DEFUN_NAME(pkg,x,lispname) defun(pkg, lispname, &fn_##x, ARGS_fn_##x, DECL_fn_##x, DOCS_fn_##x, LOCK_fn_##x,  _lisp); 

/*! Use this used to bind the C++ function fn_##x that will have the name (x) in Lisp (with "_" converted to "-") */
#define DEFUN_NAME_EXPORT(pkg,x,lispname) defun(pkg, lispname, &fn_##x, ARGS_fn_##x, DECL_fn_##x, DOCS_fn_##x, LOCK_fn_##x, _lisp); 

/*! Define a PAIR of accessor functions, the getter and setter */
#define DEFACCESSORS(x) \
    af_def(CurrentPkg,#x,&af_##x,ARGS_af_##x,DOCS_af_##x,LOCK_af_##x); \
    _lisp->add_accessor_pair(_sym_##x,_sym_setf_##x);

/*! Define a PAIR of accessor functions, the getter and setter */
#define DEFACCESSORS_EXPORT(x) \
    af_def(CurrentPkg,#x,&af_##x,ARGS_af_##x,DECL_af_##x,DOCS_af_##x,LOCK_af_##x); \
    _lisp->add_accessor_pair(_sym_##x,_sym_setf_##x);


#define EXTERN_GENERIC(x) extern T_sp gf_##x(Function_sp exec, Cons_sp args, Environment_sp env, Lisp_sp lisp);
/*! Use this in initializeCandoPrimitives to define a function
  This is a little more complicated than it needs to be to try and avoid unused variable warnings */
#define DEFGENERIC(pkg,x) defgeneric(pkg,#x,&gf_##x,ARGS_gf_##x,DOCS_gf_##x,_lisp); 



/*! Use this in initializeCandoPrimitives to attach methods to the generic function */
#define DEFMETHOD(x,id) defmethod(_sym_##x,md_##x##id,ARGS_md_##x##id,DECL_md_##x##id, DOCS_md_##x##id,_lisp);


	//
	// Define this if you want to debug energy evaluation
	//
#define	TURN_ENERGY_FUNCTION_DEBUG_ON	1


//
// For Production code set PRODUCTION_CODE to 1
// this will remove some tests
// that are in the non-debugging code for catching
// very rare conditions but slow things down
//

#define	PRODUCTION_CODE	1

/*!
 * Log all steps of score evaluation
 * This really slows things down
 */
#define	DEBUG_SCORE_EVALUATION	0

/*!
 * Save a text description of a alchemist state within every AlchemistState object
 * This slows things down
 */
#ifdef	DEBUG_ON
#define	SAVE_STATE_DESCRIPTION 1
#else
#define	SAVE_STATE_DESCRIPTION 0
#endif

//
//#define	DEBUG_LEVEL_FULL_REFCOUNT
//
//
// To just debug object creation/destruction define REFCOUNTDEBUG
//#define	REFCOUNTDEBUG


//
// Turn on DEBUG_SIGNALS to log commands signal/catchSignal/propagate calls
// even when no other debugging is going on
#define	DEBUG_SIGNALS

/*!
 * In Archive object data can either be stored as separate strings
 * or it can be stored as in a MultiStringBuffer
 *
 * Set to 1 if you want to use a MultiStringBuffer 
 * Set to 0 if you don't
 */ 
#define	ARCHIVE_MULTI_STRING_BUFFER	1

#include <cstddef>


typedef	unsigned int uint;
typedef int Fixnum; // a fixnum that can be represented within a tagged pointer
typedef Fixnum cl_fixnum;
typedef size_t _Index; // 

struct size_t_pair {
    size_t start;
    size_t end;
    size_t length;
};


#if defined(OLD_SERIALIZE)
#include "serializerNode.fwd.h"
#endif


namespace core
{
    extern bool _ClassesAreInitialized;
    typedef	uint handleType;
    const uint handleNumberFlag = 1 << 31;
    const uint handleNumberMask = handleNumberFlag - 1;
    const uint MaxHandle = handleNumberFlag - 1;
    const uint IllegalHandle = MaxHandle;
    const handleType EmptyStringHandle = 0;
    const handleType UniqueIdHandle = 1;
}



/*! Associate a namespace name with a Package.
  This is scraped out of the code by "registerClasses.py"
*/

#define	NAMESPACE_PACKAGE_ASSOCIATION(x,y,z) \
    static const std::string y = z;	     \
    namespace x 				\
{						\
    static const std::string CurrentPkg = z; 	\
}



#define	UndefinedUnsignedInt UINT_MAX
#define UNDEF_UINT	UndefinedUnsignedInt




//
// Define exceptions
//



/*!\defgroup CompileTimeDebugging Compile time debugging*/
/*@{*/
/*!
 * The macro SOURCE_DEBUG must be defined for any source level debugging
 * to take place.
 *
 * To control the compile time debugging level in the code
 * define the macros: DEBUG_LEVEL_FULL, DEBUG_LEVEL_SILENT, and DEBUG_LEVEL_NONE
 * on the FIRST LINE of a .cc file.  In the foundation.h file it will be expanded
 * into DEBUG_ON, CALLSTACK_ON and CALLSTACK_SILENT defines that will be interpreted by
 * exceptions.h to turn on/off VP0 macros and _FUNCTION_TRACE and _BLOCK_TRACE messages.
 *
 * 	- DEBUG_LEVEL_FULL
 * 		- Turns on debugging messages (LOG) and callstack tracing to the
 * 		output file.
 * 	- DEBUG_LEVEL_SILENT
 * 		- No debugging output (LOG and call traces) will be sent to the
 * 			debug output file but a call stack
 * 			will be maintained for backtracing
 * 			in the event of a crash.
 * 	- DEBUG_LEVEL_NONE
 * 		- All runtime debugging will be disabled
 *
 */
/*@}*/





//
//
//	For debugging memory
//
//
#ifdef	CDEBUG_FULL
#define	REFCOUNTDEBUG
#endif


#if 0
#ifdef	_DEBUG
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <cstdio>
#include <crtdbg.h>
#define	REFCOUNTDEBUG
#endif
#endif

		//
		// Trap failed BOOST_ASSERT invocations
		// so that they call my code and throw a proper exception
		//
#define	BOOST_ENABLE_ASSERT_HANDLER	1




//
//
// If we are using boost::python then include a few extra files
//
//


#include <string>
#include <vector>
#include <queue>
#include <list>
#include <map>
#include <set>
#include <ostream>
#include <boost/format.hpp>
//using namespace std;
using string = std::string;
using type_info = std::type_info;
template <typename X, typename Y>
using map = std::map<X,Y>;
template <typename X>
using vector = std::vector<X>;
using stringstream = std::stringstream;
template <typename X, typename Y>
using pair = std::pair<X,Y>;
template <typename X>
using list = std::list<X>;
template <typename X, typename Y>
using multimap = std::multimap<X,Y>;
template <typename X>
using set = std::set<X>;
template <typename X>
using deque = std::deque<X>;


#if defined(USE_REFCOUNT)
namespace boost
{
    template <class T> void intrusive_ptr_add_ref(T* p);
    template <class T> void intrusive_ptr_release(T* p);
};
#endif

/* --------------------------------------------------
   --------------------------------------------------
   
   Memory Pool System stuff

   --------------------------------------------------
   --------------------------------------------------
*/
/*! Usage
  STATIC_ROOT_FRAME_BEGIN(StaticFrame) {
  ...
  }
  STATIC_ROOT_FRAME_BEGIN(StaticFrame,staticFrame);
*/
#define STATIC_ROOT_FRAME_BEGIN(X) struct X : public gctools::HeapRoot
#define STATIC_ROOT_FRAME_END(X,N) ; static X* N = NULL; if (!N) N = new X();


namespace core
{
    class T_O;
    class Pointer_O;

    /* AMS pool classes */
    class Symbol_O;
    class Null_O;

};
void dbg_hook(const char* errorString);




namespace core {

    bool internal_isTrue(const void* T_spPtr);

};





/*! Class registration code - each registered class gets a unique number associated with it */

#include <boost/operators.hpp>


namespace gctools {
    /*! Inheriting from this class indicates that the derived class
      includes smart_ptr's but is only ever instantiated on the stack.
      This means the conservative garbage collector will see the smart_ptr's
      when it scans the stack.
    */
    class StackBoundClass {};


    /*! Inheriting from this class indicates that the derived class
      contains absolutely no smart_ptrs or weak_ptrs either directly
      or indirectly - DON'T PUT ANY POINTERS IN THE DERIVED CLASS */
    class GCIgnoreClass {};
};


namespace reg {

    struct null_type : public gctools::GCIgnoreClass {};


    class_id const unknown_class = (std::numeric_limits<class_id>::max)();

    class type_id
        : public boost::less_than_comparable<type_id>
        , public gctools::GCIgnoreClass 
    {
    public:
        type_id()
            : id(&typeid(null_type))
        {}

        type_id(std::type_info const& id)
            : id(&id)
        {}

        bool operator!=(type_id const& other) const
        {
            return *id != *other.id;
        }

        bool operator==(type_id const& other) const
        {
            return *id == *other.id;
        }

        bool operator<(type_id const& other) const
        {
            return id->before(*other.id);
        }

        char const* name() const
        {
            return id->name();
        }

        std::type_info const* get_type_info() const { return this->id;};
    private:
        std::type_info const* id;
    };


    class_id allocate_class_id(type_id const& cls);
    template <class T>
    struct registered_class : gctools::GCIgnoreClass
    {
        static class_id const id;
    };
    template <class T>
    class_id const registered_class<T>::id = allocate_class_id( typeid(T));
    template <class T>
    struct registered_class<T const>
        : registered_class<T>
    {};




};

#define BF boost::format



void lisp_errorUnexpectedType(class_id expectedTyp, class_id givenTyp, core::T_O* objP);
void lisp_errorDereferencedNil();
void lisp_errorDereferencedUnbound();


#include "gctools/memoryManagement.h"



extern void brcl_mps_debug_allocation(void* base, void* objAddr, int size, int kind);
extern void brcl_mps_debug_fix1_before(void* base, void* smartAddr);
extern void brcl_mps_debug_fix_before(void* pbase, void* px, int offset);
extern void brcl_mps_debug_fix_after(void* pbase, void* px);
extern void brcl_mps_debug_container(const char* ctype,const char* name, int size);
extern void brcl_mps_debug_scan_object(gctools::GCObject*  obj);






#define	DEFINE_O_SMART_POINTERS(zclass) \
	class zclass##_O;\
	typedef mem::smart_ptr<zclass##_O> zclass##_sp; /* Stack pointers */  \
	typedef mem::smart_ptr<zclass##_O> zclass##_hp; /* Heap pointers */ \
	typedef mem::smart_ptr</* TODO: const */ zclass##_O> zclass##_scp; \
	typedef mem::smart_ptr<zclass##_O> zclass##_rp; \
	typedef mem::smart_ptr<zclass##_O> const& zclass##_cp; \
	typedef mem::weak_smart_ptr<zclass##_O> zclass##_wp; \
	typedef mem::multiple_values<zclass##_O> zclass##_mv;

	// This ensures that smart pointers are only declared once
	// for each compilation
#define	SMART(zclass) \
    DEFINE_O_SMART_POINTERS(zclass)

#define FORWARD(zclass) \
    DEFINE_O_SMART_POINTERS(zclass)



namespace core
{
    typedef mem::smart_ptr<T_O>			T_sp;
    typedef mem::smart_ptr</* TODO: use const */ T_O>		T_scp;
    typedef mem::weak_smart_ptr<T_O>		T_wp;

    class Cons_O;
    typedef mem::smart_ptr<Cons_O>	Cons_sp;

    class Class_O;
    typedef mem::smart_ptr<Class_O>	Class_sp;

    class VectorObjects_O;
    typedef mem::smart_ptr<VectorObjects_O> 	VectorObjects_sp;

    class Symbol_O;
    typedef mem::smart_ptr<Symbol_O>		Symbol_sp;

    class Pointer_O;
    typedef mem::smart_ptr<Pointer_O>   Pointer_sp;

    void lisp_error_simple(const char* functionName, const char* fileName, int lineNumber, const boost::format& fmt);

};








class _RootDummyClass;



namespace core
{
    void	lisp_throwLispError(const string& str);
    void	lisp_throwLispError(const boost::format& fmt);

};








namespace core {
    class MultipleValues;
    MultipleValues* lisp_multipleValues();
};

#include "multipleValues.h"






namespace core {

    typedef	double	real;	// For now real is double, may be float in the future

    typedef	vector<string>	    VectorStrings;
    typedef	vector<int>	    VectorInts;
    typedef	vector<double>	    VectorDoubles;




//
//
// prototypes for functions defined in foundation.cc

    class StringStack : public gctools::GCIgnoreClass {
    private:
	vector<string> parts;
    public:
	void	clear() {this->parts.clear();};
	void	push(const string& s) {this->parts.push_back(s);};
	void	pop();
	string	all(const string& separator) ;
    };


/*! Escape all white space (spaces, cr, tab) */
    string	escapeWhiteSpace(const string& inp);
/*! Unescape all whitespace (spaces/cr/tab) */
    string unEscapeWhiteSpace(const string& inp);

    string	trimWhiteSpace(const string& inp);
    string	stripCharacters(const string& inp, const string& strip);

    vector<string>split(const string& str, const string& delim = " \t" );
    void tokenize(const string& str,
		  vector<string>& tokens,
		  const string& delimiters = " ");
    void queueSplitString(const string& str,
			  std::queue<string>& tokens,
			  const string& delimiters = " ");

    string	stringUpper(const string& s);
    string	stringUpper(const char* s);

    string	concatenateVectorStrings( VectorStrings strs);
    void	appendVectorStrings( VectorStrings& app, VectorStrings strs);



    bool	wildcmp(	string	sWild,
				string	sRegular );


/*
 * Documentation macros
 */

#define	DOC_METHOD()
#define	DOC_FUNCTION()



    typedef	struct NullTerminatedEnumAssocationStruct : public gctools::GCIgnoreClass	{
	string	_Key;
	int	_Enum;
    } NullTerminatedEnumAssociation;




    typedef	unsigned int	AtomFlags;	// Minimum 32 bits



#ifndef MAX
# define        MAX( x, y )     ( (x) < (y) ? (y) : (x) )
#endif //MAX
#ifndef MIN
# define        MIN( x, y )     ( (x) > (y) ? (y) : (x) )
#endif //MIN

    typedef	unsigned int	AtomHandle;// Implement the AtomHandle as an index into
    // a vector of atoms that maintains a constant
    // order even when the Residue is copied.

    typedef	vector<AtomHandle>	VectorAtomHandle;

    // Used to define atom handles to the connect atoms
    // in the previous and next residues
#define	ATOMHANDLE_PREV0	-11
#define	ATOMHANDLE_PREV1	-12
#define	ATOMHANDLE_PREV2	-13
#define	ATOMHANDLE_NEXT0	-21
#define	ATOMHANDLE_NEXT1	-22
#define	ATOMHANDLE_NEXT2	-23

#ifdef	WIN32
//#define	assert(x)   {if (!(x)) abort(); }
// # p r a g m a warning( disable : 4290 )
#endif


    class ActivationFrame_O;
    typedef mem::smart_ptr<ActivationFrame_O> ActivationFrame_sp;
    typedef mem::weak_smart_ptr<ActivationFrame_O> ActivationFrame_wp;
//    typedef const ActivationFrame_sp* const_ActivationFrame_spP;
//    typedef const ActivationFrame_sp& const_ActivationFrame_spREF;
    typedef ActivationFrame_sp& ActivationFrame_spREF;


};

/*! A type for an array of arguments */
    typedef	core::T_sp* 	ArgArray;


namespace core {
    class Instance_O;

//    typedef T_mv (*ActivationFrameFunctionPtr)(ActivationFrame_sp);
    typedef T_mv (*ArgArrayGenericFunctionPtr)(const Instance_O& gf, int nargs, ArgArray argArray);

    class Lisp_O;
    typedef Lisp_O* Lisp_sp;
    class Function_O;
    typedef mem::smart_ptr<Function_O>	Function_sp;
    class Str_O;
    typedef mem::smart_ptr<Str_O> Str_sp;
    class Fixnum_O;
    typedef mem::smart_ptr<Fixnum_O> Fixnum_sp;

    class LambdaListHandler_O;
    typedef mem::smart_ptr<LambdaListHandler_O>	LambdaListHandler_sp;
    class Environment_O;
    typedef mem::smart_ptr<Environment_O>	Environment_sp;
    class Symbol_O;
    typedef mem::smart_ptr<Symbol_O> Symbol_sp;
    typedef void (*ExposeCandoFunction)(Lisp_sp);
    typedef void (*ExposePythonFunction)(Lisp_sp);
    typedef T_mv (*SpecialFormCallback)(Cons_sp,Environment_sp);
    typedef void (*MakePackageCallback)(string const& packageName,Lisp_sp);
    typedef void (*ExportSymbolCallback)(Symbol_sp symbol,Lisp_sp);

    class Package_O;
    typedef mem::smart_ptr<Package_O> Package_sp;

    /* A few symbols associated with error handling that everything needs */
    extern Symbol_sp _sym_error;
    extern Symbol_sp _sym_setThrowPosition;
    extern Symbol_sp _sym_makeCondition;
    extern Symbol_sp _sym_simpleError;
/*! Search for multiple occurances of a string and replace it
 * \param str The string that is modified
 * \param search The string to search for
 * \param replace The string to replace with
 */
    string searchAndReplaceString(const string& str, const string& search, const string& replace,Lisp_sp lisp);

}

namespace kw
{
    extern core::Symbol_sp _sym_formatControl;
    extern core::Symbol_sp _sym_formatArguments;
};


// Can I get rid of this?
#define IS_SYMBOL_DEFINED(x) (x)
#define IS_SYMBOL_UNDEFINED(x) (!x)
#define UNDEFINED_SYMBOL	(_Unbound<core::Symbol_O>())
//#define UNDEFINED_SYMBOL _global_undefined_symbol
//extern core::Symbol_sp _global_undefined_symbol;


namespace core {

    class AllocatorFunctor
    {
    public:
        GC_RESULT onHeapScanGCRoots(GC_SCAN_ARGS_PROTOTYPE);
        // Some AllocatorFunctors don't actually allocate anything - 
        // classes that don't have default allocators
        virtual bool allocates() const { return true;};
        /*! If this is the allocator for a primary CxxAdapter class then return true, */
        virtual int duplicationLevel() const { return 0;};
        virtual AllocatorFunctor* duplicateForClassName(core::Symbol_sp className) {printf("Subclass must implement AllocatorFunctor::duplicateForClassName\n"); exit(1);};
        virtual void describe() const = 0;
        virtual core::T_sp allocate() = 0;
    };


};


//
//
//

//
// These macros are scraped by an external program
// and used to create "initClasses_inc.h" which includes
// calls to initialize anything you want before the program
// runs.
//
//		__INITIALIZE_PYTHON and __INITIALIZE_PYTHON_AFTER 
//		initialize the Python interface after all of the classes have been registered
//
//		__INITIALIZE and __INITIALIZE_AFTER
//		initialize other stuff after all the Python interfaces have been registered
#define	__INITIALIZE_PYTHON(x)	// Do nothing
#define	__INITIALIZE_PYTHON_AFTER(x,y)	// Do nothing
#define	__INITIALIZE(p,x)	// Do nothing
#define	__INITIALIZE_AFTER(x,y)	// Do nothing

namespace core
{


/*! Create the class hierarchy
 */
    extern	void registerAllClasses();
// extern void throwIfClassesNotInitialized();

/*! Expose c++ to CandoScript
 */
    extern	void exposeClassesAndDefineNils(Lisp_sp);

/*! Expose c++ to Python
 */
    extern	void initializePythonScript();





// ---------------------------------------------------
//  Object oriented programming stuff
//  ------


    class Cons_O;
    typedef	mem::smart_ptr<Cons_O>	Cons_sp;

    class Class_O;
    typedef	mem::smart_ptr<Class_O>	Class_sp;

    class Function_O;

    class Symbol_O;
    typedef	mem::smart_ptr<Symbol_O>	Symbol_sp;

    class SymbolToEnumConverter_O;
    typedef mem::smart_ptr<SymbolToEnumConverter_O>	SymbolToEnumConverter_sp;


}





namespace core 
{

#define	_NEW_(x) (new x)




    class DebugStream;


    class Functoid;


/* Callbacks that initialize the Lisp environment have this structure*/

    typedef void(*InitializationCallback)(Lisp_sp);


    void errorFormatted(boost::format fmt);
    void errorFormatted(const char* errorString);
    void errorFormatted(const string& msg);


//
//  Lisp class access functions for when we only have a forward
//  definition for the Lisp_O class and for any other object
//
    string _rep_(T_sp obj);
    /*! Convert underscores to "-" and "STAR" to "*" and "AMP" to "&"
      to convert a C++ name to a lisp symbol */
    string  lispify_symbol_name(string const& name);
    Symbol_sp lispify_intern_keyword(string const& name);
    Symbol_sp lispify_intern(string const& name, string const& packageName);
    Symbol_sp lispify_intern_export(string const& name, string const& packageName);
    Symbol_sp lisp_upcase_intern(string const& name, string const& packageName);
    Symbol_sp lisp_upcase_intern_export(string const& name, string const& packageName);
    Lisp_sp lisp_lisp(Lisp_sp lisp);
    Lisp_sp lisp_fromObject(T_sp obj);
    string lisp_currentPackageName();
    string lisp_classNameAsString(Class_sp c);
    void lisp_throwUnexpectedType(T_sp offendingObject, Symbol_sp expectedTypeId );
    core::T_sp lisp_true();
    core::T_sp lisp_false();
    T_sp lisp_ArgArrayToCons(int nargs, ArgArray args);
    bool lisp_fixnumP(core::T_sp obj);
    Fixnum lisp_asFixnum(core::T_sp obj);

    bool lisp_characterP(core::T_sp obj);
    bool	lisp_BuiltInClassesInitialized(Lisp_sp lisp);
    bool lisp_BuiltInClassesInitialized(Lisp_sp lisp);
    bool lisp_NilsCreated(Lisp_sp lisp);
    string symbol_symbolName(Symbol_sp);
    string symbol_repr(Symbol_sp);
    Symbol_sp lisp_symbolNil();
    Class_sp lisp_boot_findClassBySymbolOrNil(Symbol_sp sym);
    void	lisp_exposeClass(Lisp_sp lisp, const string& className, ExposeCandoFunction exposeCandoFunction, ExposePythonFunction exposePythonFunction);
    void	lisp_addClass( Symbol_sp classSymbol, AllocatorFunctor* cb, Symbol_sp baseClassSymbol1, Symbol_sp baseClassSymbol2=UNDEFINED_SYMBOL,Symbol_sp baseClassSymbol3=UNDEFINED_SYMBOL );
    void	lisp_addClass( Symbol_sp classSymbol );
    void	lisp_addClassAndInitialize( Symbol_sp classSymbol, AllocatorFunctor* cb, Symbol_sp baseClassSymbol1, Symbol_sp baseClassSymbol2=UNDEFINED_SYMBOL,Symbol_sp baseClassSymbol3=UNDEFINED_SYMBOL );
    void	lisp_throwIfBuiltInClassesNotInitialized(Lisp_sp lisp);
    string	lisp_classNameFromClassSymbol(Lisp_sp lisp, Symbol_sp classSymbol );
    Class_sp lisp_classFromClassSymbol(Symbol_sp classSymbol );
    Class_sp lisp_instance_class(T_sp obj);
    Class_sp lisp_static_class(T_sp obj);
    void lisp_processSignal(int signo);
    Function_sp lisp_symbolFunction(Symbol_sp sym);
    string lisp_symbolNameAsString(Symbol_sp sym);
    T_sp lisp_createStr(const string& str);
    T_sp lisp_createFixnum(int num);
    T_sp lisp_createList(T_sp a1);
    T_sp lisp_createList(T_sp a1, T_sp a2);
    T_sp lisp_createList(T_sp a1, T_sp a2, T_sp a3);
    T_sp lisp_createList(T_sp a1, T_sp a2, T_sp a3, T_sp a4);
    T_sp lisp_createList(T_sp a1, T_sp a2, T_sp a3, T_sp a4, T_sp a5);
    T_sp lisp_createList(T_sp a1, T_sp a2, T_sp a3, T_sp a4, T_sp a5, T_sp a6);
    T_sp lisp_createList(T_sp a1, T_sp a2, T_sp a3, T_sp a4, T_sp a5, T_sp a6, T_sp a7);
    T_sp lisp_createList(T_sp a1, T_sp a2, T_sp a3, T_sp a4, T_sp a5, T_sp a6, T_sp a7, T_sp a8);
    void lisp_error_condition(const char* functionName, const char* fileName, int lineNumber, T_sp baseCondition, T_sp initializers);
    void lisp_error(T_sp datum, T_sp arguments);

    void lisp_setGlobalInt(Lisp_sp lisp, const string& package, const string& n, uint val );
//    Symbol_sp lisp_allocate_packageless_sid(string const& n);
    Symbol_sp lisp_getClassSymbolForClassName(Lisp_sp lisp, const string& n);
//    bool lisp_subClassOrder(Lisp_sp lisp, Symbol_sp baseClassSymbol, Symbol_sp classSymbol );
    string lisp_convertCNameToLispName(string const& cname, bool convertUnderscoreToDash=true);
    T_sp lisp_apply(T_sp funcDesig, ActivationFrame_sp args );
    Cons_sp lisp_parse_arguments(Lisp_sp lisp, const string& packageName, const string& args);
    Cons_sp lisp_parse_declares(Lisp_sp lisp, const string& packageName, const string& declarestring);
    LambdaListHandler_sp lisp_function_lambda_list_handler(Lisp_sp lisp, Cons_sp lambda_list, Cons_sp declares );

    void lisp_defun_lispify_name(const string& packageName, const string& name,
				 Functoid*, const string& arguments="", const string& declarestring="",
				 const string& docstring="", int locked=1, bool autoExport=true, int number_of_required_arguments=0 );
    void lisp_defmacro(const string& packageName, const string& name,
		       Functoid*, const string& arguments="", const string& declarestring="",
		       const string& docstring="", bool autoExport=true);
    void lisp_defun(const string& packageName, const string& name,
		    Functoid*, const string& arguments="", const string& declarestring="",
		    const string& docstring="", int locked=1, bool autoExport=true, int number_of_required_arguments=0 );
    void lisp_defgeneric( const string& packageName, const string& name,
			 Functoid*, const string& arguments="", const string& docstring="", bool autoExport=true  );
    void lisp_defmethod(Lisp_sp lisp, Symbol_sp gfSymbol, Functoid* func, const string& arguments, const string& docstring);

    void	lisp_defineSingleDispatchMethod(const string& name, Symbol_sp classSymbol,
						Functoid*,
                                                int TemplateDispatchOn=0,
                                                const string& lambda_list="",
						const string& declares="",
						const string& docstring="",
						bool autoExport=true,
						int number_of_required_arguments = -1);


    void	lisp_defsetfSingleDispatchMethod(Lisp_sp lisp, const string& name, Symbol_sp classSymbol,
						 Functoid*, const string& arguments="", const string& declares="", const string& docstring="", bool autoExport=true);

    void	lisp_defsetf(Lisp_sp lisp, const string& name, Symbol_sp classSymbol,
			     Functoid*, const string& arguments="", const string& docstring="", bool autoExport=true);

    core::T_sp lisp_hiddenBinderLookup(Lisp_sp lisp, Symbol_sp sym);

//
// You define what is being sent to the debug log using these constants 
#define	DEBUG_CPP_FUNCTION	0x0001
#define	DEBUG_CPP_BLOCK		0x0002
#define	DEBUG_SCRIPT		0x0003
#define	DEBUG_SHOUT		0x0004
#define DEBUG_TOPLEVEL		0x0005
#define	DEBUG_LOG		0x0006
#define DEBUG_EXCEPTION 	0x0007
#define	DEBUG_LINE_COL		0x0009

#define	DEBUG_INTERP_LISP_FUNCTION	0x00000100
#define	DEBUG_INTERP_LISP_LET		0x00000200
#define	DEBUG_INTERP_LISP_LET_STAR	0x00000300
#define	DEBUG_INTERP_LISP_BLOCK		0x00000400
#define	DEBUG_INTERP_LISP_TAGBODY	0x00000500
#define	DEBUG_INTERP_LISP_LAMBDA	0x00000600
#define	DEBUG_INTERP_LISP_FLET		0x00000700
#define	DEBUG_INTERP_LISP_LABELS	0x00000800
#define DEBUG_INTERP_LISP_CALL		0x00000900
#define DEBUG_INTERP_LISP_PROGN		0x00000A00
#define DEBUG_INTERP_LISP_CATCH	        0x00000B00
#define DEBUG_INTERP_LISP_UNWIND_PROTECT 0x00000C00

#define	DEBUG_COMPILED_LISP_FUNCTION	0x00010000
#define	DEBUG_COMPILED_LISP_LET		0x00020000
#define	DEBUG_COMPILED_LISP_LET_STAR	0x00030000
#define	DEBUG_COMPILED_LISP_BLOCK	0x00040000
#define	DEBUG_COMPILED_LISP_TAGBODY	0x00050000
#define DEBUG_COMPILED_LISP_LAMBDA	0x00060000
#define DEBUG_COMPILED_LISP_FLET	0x00070000
#define DEBUG_COMPILED_LISP_LABELS	0x00080000
#define DEBUG_COMPILED_LISP_CALL	0x00090000
#define DEBUG_COMPILED_LISP_PROGN	0x000A0000
#define DEBUG_COMPILED_LISP_CATCH	0x000B0000
#define DEBUG_COMPILED_LISP_UNWIND_PROTECT 0x000C0000


    bool lisp_debugIsOn(const char* fileName,uint debugFlag = DEBUG_CPP_FUNCTION);
    void lisp_debugLogWrite( const char* fileName, const char* funcName, uint lineNumber, uint column, const boost::format& message, uint debugFlags=DEBUG_CPP_FUNCTION );


    DebugStream* lisp_debugLog();
    T_sp lisp_ocar(Lisp_sp lisp, Cons_sp args);
    T_sp lisp_ocadr(Lisp_sp lisp, Cons_sp args);
    T_sp lisp_ocaddr(Lisp_sp lisp, Cons_sp args);
    Symbol_sp lisp_internKeyword(const string& name);
    Symbol_sp lisp_intern(const string& name);
    Symbol_sp lisp_intern(const string& pkgName, const string& symName);
    string symbol_fullName(Symbol_sp s);
    void lisp_logException(const char* file, const char* fn, int line, const char* structure, T_sp condition );
    bool lisp_isGlobalInitializationAllowed(Lisp_sp lisp);
    void lisp_installGlobalInitializationCallback(Lisp_sp lisp, InitializationCallback initGlobals);
    Symbol_sp lisp_predefinedSymbol(Lisp_sp lisp, Symbol_sp symId);
//    void lisp_hiddenBinderExtend(Lisp_sp lisp, Symbol_sp sym, T_sp obj);
    void lisp_extendSymbolToEnumConverter(SymbolToEnumConverter_sp conv, Symbol_sp const& name, Symbol_sp const& archiveName, int value );
    int lisp_lookupEnumForSymbol( Symbol_sp predefSymId, T_sp symbol);
    core::Symbol_sp lisp_lookupSymbolForEnum(Symbol_sp predefSymId, int enumVal);





    class Functoid 
    {
    protected:
	string	_Name;
    public:
    public:
	virtual string describe() const {return "Functoid - subclass must implement describe()";};
	virtual T_mv activate(ActivationFrame_sp closedOverFrame, int nargs, ArgArray argArray) {printf("Subclass of Functoid must implement 'activate'\n"); exit(1);};

	void dump() const
	{
	    printf( "Functoid - %s\n", this->_Name.c_str() );
	}



	T_mv funcall(int n_args, va_list vl) {
	    //IMPLEMENT_ME();
	    return Values0<T_O>();
	}
	    



	Functoid( const string& name) : _Name(name) {};
	virtual ~Functoid() {};
    };


    class AFFunctoid : public Functoid
    {
    public:
	virtual string describe() const {return "AFFunctoid";};
	virtual T_mv activate(ActivationFrame_sp closedOverEnv, int nargs, ArgArray argArray) {printf("Subclass of Functoid must implement 'activate'\n"); exit(1);};

	AFFunctoid(const string& name) : Functoid(name) {};
	virtual ~AFFunctoid() {};
    };



}





#include <boost/random.hpp>


namespace core
{


    typedef unsigned char ubyte;




/* Callbacks that return a boolean value have this structure */
typedef bool(*BoolReturnCallback)(Lisp_sp);

/* Callbacks that return an integer value have this structure */
typedef	int(*IntReturnCallback)(Lisp_sp);

/* Callbacks that return an object have this structure */
typedef T_sp(*ObjectReturnCallback)(Lisp_sp);

/*
 * These callback types are used to redirect screen output depending if we are
 * on a console or using WxWidgets
 */

typedef void (*PrintvWriteCallback)( const char* outputBuffer );
typedef void (*PrintvWriteCharCallback)( char outputChar );
typedef	void (*PrintvFlushCallback)();


const char* trimSourceFilePathName(const char* fullPathName);





#define	IN_PLUG_PREFIX	'-'
#define	OUT_PLUG_PREFIX	'+'

};




namespace llvm_interface
{

    typedef void (*llvmAddSymbolCallbackType)(const core::Symbol_sp& sym);
    extern llvmAddSymbolCallbackType addSymbol;

}

#ifdef	USEBOOSTPYTHON
#include <cctype>
#include "mainBoostPython.h"
#endif



#define TRY_BOOST_FORMAT_STRING(__fmt,__fmt_str) 			\
	string __fmt_str;					\
	try {  __fmt_str = __fmt.str(); } 			\
	catch (const std::exception &exc)			\
	{										\
	lisp_throwLispError(BF("Error - could not get string from boost::format --> %s") % exc.what() ); \
	}


#include "exceptions.h"





#include "clasp_gmpxx.h"


extern "C"
{
#define 	_LISP _lisp
    extern core::Lisp_sp _lisp;
};






namespace reg {
    extern std::vector<core::Symbol_sp,gctools::root_allocator<core::Symbol_sp> > globalClassSymbolsVector;

    inline void lisp_associateClassIdWithClassSymbol(class_id cid, core::Symbol_sp sym)
    {
        if ( cid >= globalClassSymbolsVector.size() ) {
            globalClassSymbolsVector.resize(cid+1,core::lisp_symbolNil());
        }
        globalClassSymbolsVector[cid] = sym;
    }

    template <class T>
    void lisp_registerClassSymbol(core::Symbol_sp sym)
    {
        class_id cid = registered_class<T>::id;
        lisp_associateClassIdWithClassSymbol(cid,sym);
    }

    inline core::Symbol_sp lisp_classSymbolFromClassId(class_id cid)
    {
        core::Symbol_sp sym = globalClassSymbolsVector[cid];
        if (sym.nilp()) {
            return _Unbound<core::Symbol_O>();
        }
        return sym;
    }

    template <class T>
    core::Symbol_sp lisp_classSymbol() {
        class_id cid = registered_class<T>::id;
        core::Symbol_sp sym = globalClassSymbolsVector[cid];
        if (sym.nilp()) {
            string typen = typeid(T).name();
            sym = core::lisp_intern(typen);
            // Fall through - intern the name and return the symbol
        }
        return sym;
    }
};







#define EXECUTABLE_NAME "brcl"
#define KERNEL_NAME "kernel"



#include "boost/filesystem.hpp"

namespace boost_filesystem = boost::filesystem;


namespace core
{
    void initialize_foundation();
}


#define brcl_disable_interrupts()
#define brcl_enable_interrupts()


#define unlikely_if(x) if(UNLIKELY(x))
#define BRCL_T (_lisp->_true())
#define BRCL_NIL (_Nil<core::T_O>())

namespace core {
extern int _global_signalTrap;
};
#define SET_SIGNAL(s) {core::_global_signalTrap = s;}
#define POLL_SIGNALS() if (core::_global_signalTrap) core::lisp_processSignal(core::_global_signalTrap);

#endif //]
