#define	DEBUG_LEVEL_FULL

#include "core/common.h"
#include "core/environment.h"
#include "core/ql.h"
#include "designators.h"
#include "package.h"
#include "symbolTable.h"
#include "fileSystem.h"
#include "lispStream.h"
#include "character.h"
#include "str.h"
#include "core/wrappers.h"
namespace core
{

    namespace coerce
    {	
	
	Function_sp functionDesignator(T_sp obj)
	{_G();
	    if ( obj.nilp() ) {
		SIMPLE_ERROR(BF("You tried to treat nil as a function designator"));
	    }
	    if ( af_symbolp(obj) )
	    {
		Symbol_sp sym = obj.as<Symbol_O>();
		if (!sym->fboundp()) 
		    SIMPLE_ERROR(BF("Function value for %s is unbound") % _rep_(sym));
		return sym->symbolFunction();
	    }
	    return obj.as<Function_O>();
	}


	Path_sp pathDesignator(T_sp obj)
	{_G();
	    if ( af_strP(obj) )
	    {
		return Path_O::create(obj.as<Str_O>()->get());
	    } else if (obj.isA<Path_O>())
	    {
		return obj.as<Path_O>();
	    }
	    SIMPLE_ERROR(BF("Illegal path designator[%s]") % obj);
	}
	


	Package_sp packageDesignator(T_sp obj)
	{_G();
	    if ( obj.nilp() )
	    {
		return _lisp->getCurrentPackage();
	    } else if (af_packageP(obj) )
	    {
		return obj.as<Package_O>();
	    }
	    Str_sp packageName = stringDesignator(obj);
	    Package_sp pkg = _lisp->findPackage(packageName->get());
	    if ( pkg.nilp() ) {
		SIMPLE_ERROR(BF("Could not find package %s") % packageName->get() );
	    }
	    return pkg;
	}

	Cons_sp listOfPackageDesignators(T_sp obj)
	{_G();
	    if ( obj.nilp() ) return _Nil<Cons_O>();
	    if ( af_consP(obj) )
	    {
		ql::list res(_lisp);
		for ( Cons_sp cur = obj.as_or_nil<Cons_O>(); cur.notnilp(); cur = cCdr(cur) )
		{
		    Package_sp pkg = packageDesignator(oCar(cur));
		    res << pkg;
		}
		return res.cons();
	    }
	    return Cons_O::create(packageDesignator(obj));
	}



	Cons_sp listOfSymbols(T_sp syms)
	{_G();
	    if ( syms.nilp() ) return _Nil<Cons_O>();
	    Cons_sp symbols;
	    if ( af_symbolp(syms) )
	    {
		symbols = Cons_O::create(syms);
	    } else
	    {
		symbols = syms.as_or_nil<Cons_O>();
	    }
	    return symbols;
	}


	Str_sp stringDesignator(T_sp obj)
	{_G();
	    if ( obj.nilp() )
	    {
		return af_symbolName(_Nil<Symbol_O>());
	    } else if ( Str_sp str = obj.asOrNull<Str_O>() ) {
		return str;
	    } else if ( Symbol_sp sym = obj.asOrNull<Symbol_O>() )
	    {
		return af_symbolName(sym);
	    } else if ( af_characterP(obj) )
	    {
		stringstream ss;
		ss << obj.as<Character_O>()->asChar();
		return Str_O::create(ss.str());
	    }
	    SIMPLE_ERROR(BF("Illegal string designator[%s] of class[%s]") % _rep_(obj) % _rep_(lisp_instance_class(obj)) );
	}

	Cons_sp listOfStringDesignators(T_sp obj)
	{_G();
	    if ( obj.nilp() ) return _Nil<Cons_O>();
	    if ( af_consP(obj) )
	    {
		Cons_sp first = Cons_O::create(_Nil<T_O>());
		Cons_sp cur = first;
		for ( Cons_sp ic = obj.as_or_nil<Cons_O>(); ic.notnilp(); ic = cCdr(ic) )
		{
		    Cons_sp one = Cons_O::create(stringDesignator(oCar(ic)));
		    cur->setCdr(one);
		    cur = one;
		}
		return cCdr(first);
	    } else
	    {
		return Cons_O::create(stringDesignator(obj));
	    }
	    SIMPLE_ERROR(BF("Illegal list of string designators[%s]") % _rep_(obj) );
	}




	Stream_sp inputStreamDesignator(T_sp obj)
	{_G();
	    if ( obj.nilp() )
	    {
		return cl::_sym_STARstandard_inputSTAR->symbolValue().as<Stream_O>();
	    } else if (obj == _lisp->_true() )
	    {
		return cl::_sym_STARterminal_ioSTAR->symbolValue().as<Stream_O>();
	    } else if ( cl_streamp(obj) )
	    {
		return obj.as<Stream_O>();
	    }
	    SIMPLE_ERROR(BF("Cannot convert object[%s] into a Stream") % _rep_(obj));
	}


	Stream_sp outputStreamDesignator(T_sp obj)
	{_G();
	    if ( obj.nilp() )
	    {
		return cl::_sym_STARstandard_outputSTAR->symbolValue().as<Stream_O>();
	    } else if (obj == _lisp->_true() )
	    {
		return cl::_sym_STARterminal_ioSTAR->symbolValue().as<Stream_O>();
	    } else if ( cl_streamp(obj) )
	    {
		return obj.as<Stream_O>();
	    }
	    SIMPLE_ERROR(BF("Cannot convert object[%s] into a Stream") % _rep_(obj));
	}








    }; /* desig */    



    void initialize_designators()
    {_G();
	af_def(CorePkg,"pathDesignator",&coerce::pathDesignator);
	af_def(CorePkg,"coerce-to-package",&coerce::packageDesignator);
    }
}; /* core */