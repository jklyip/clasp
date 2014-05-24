#define	DEBUG_LEVEL_FULL

#include "foundation.h"
#include "candoClass.h"
#include "lisp.h"
#include "effectiveSlotDefinition.h"
#include "evaluator.h"
#include "standardObject.h"
#include "package.h"
#include "lambdaListHandler.h"
#include "wrappers.h"

namespace core {


    void	CandoClass_O::initialize()
    {
	this->Base::initialize();
	this->_CoreObjectClass = _Nil<Class_O>();
    }

    CandoClass_sp CandoClass_O::create(Lisp_sp lisp,Symbol_sp name)
    {_G();
        GC_ALLOCATE(CandoClass_O);
	oclass->_Name = name;
	// Lets create a predefined symbol here for the class name
	oclass->_InstanceClassSymbol = UNDEFINED_SYMBOL;
	_lisp->createPredefinedSymbol(oclass->_InstanceClassSymbol,name);
//    oclass->_InstanceVariableNames = _Nil<Cons_O>();
	IMPLEMENT_MEF(BF("What do I do about superclasses of the CandoClass here? - I should call setInstanceBaseClasses with something or just StandardObject "));
	return oclass;
    }


#if defined(XML_ARCHIVE)
void CandoClass_O::archiveBase(ArchiveP node)
    {_OF();
	IMPLEMENT_MEF(BF("Implement CandoClass_O::archiveBase"));
    }
#endif // defined(XML_ARCHIVE)




    Class_sp CandoClass_O::getCoreObjectClass()
    {_G();
	return this->_CoreObjectClass;
    }

    void	CandoClass_O::setCoreObjectClass(Class_sp mc)
    {_G();
	this->_CoreObjectClass = mc;
    }

    void	CandoClass_O::describe()
    {_G();
	_lisp->print(BF("------------  CandoClass name: %s    instanceClassSymbol: %d") % this->_Name->__repr__() % this->_InstanceClassSymbol );
//    _lisp->print(BF("Instance variables: %s") % this->_InstanceVariableNames->__repr__().c_str() );
	_lisp->print(BF("%s") % this->dumpInfo() );
    }

    string CandoClass_O::dumpInfo()
    {_G();
	stringstream ss;
	ss << this->Base::dumpInfo();
	ss << "CoreObjectClass: " << this->_CoreObjectClass->getPackagedName() << std::endl;
	return ss.str();
    }

    string CandoClass_O::dumpMethods()
    {
	return this->Base::dumpMethods();
    }



    Cons_sp CandoClass_O::find(Symbol_sp sym)
    {_G();
	IMPLEMENT_MEF(BF("Handle find slot by name"));
#if 0
	ASSERTNOTNULL(sym);
	LOG(BF("Looking in CandoClass for slot for symbol: %s") % sym->fullName() );
	CandoClass_O::slotIterator fnd = this->_SlotNames.find(sym);
	return this->_SlotNames.find(sym);
#endif
    }








    void CandoClass_O::setupAccessors(Cons_sp slotNames)
    {_G();
	IMPLEMENT_ME(); // Dont pass the slot names, use the slots already defined
#if 0
	this->_InstanceVariableNames = slotNames;
	while ( slotNames.notnilp() )
	{
	    Symbol_sp slotName = slotNames->ocar().as<Symbol_O>();
	    string setterName = "set_"+slotName->symbolNameAsString();
	    Symbol_sp setterSymbol = _lisp->internKeyword(setterName);
	    SlotSetter_sp setterForm = SlotSetter_O::create(setterSymbol,_lisp);
	    this->addMethod(setterSymbol,setterForm);
	    string getterName = "get_"+slotName->symbolNameAsString();
	    Symbol_sp getterSymbol = _lisp->internKeyword(getterName);
	    SlotGetter_sp getterForm = SlotGetter_O::create(getterSymbol,_lisp);
	    this->addMethod(getterSymbol,getterForm);
	    slotNames = slotNames->cdr();
	}
#endif
    }



	void CandoClass_O::exposeCando(Lisp_sp lisp)
	{
	    class_<CandoClass_O>()
		;
	}
    void CandoClass_O::exposePython(Lisp_sp lisp)
{_G();
    PYTHON_CLASS(CorePkg,CandoClass,"","",_lisp)
		;
	}


    EXPOSE_CLASS(core,CandoClass_O);

};


