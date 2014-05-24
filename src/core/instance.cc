#define	DEBUG_LEVEL_FULL

#include "foundation.h"
#include "object.h"
#include "lisp.h"
#include "symbolTable.h"
#include "serialize.h"
#include "evaluator.h"
#include "standardClass.h"
#include "genericFunction.h"
#include "instance.h"
#include "wrappers.h"

namespace core
{


#define ARGS_clos_setFuncallableInstanceFunction "(instance func)"
#define DECL_clos_setFuncallableInstanceFunction ""
#define DOCS_clos_setFuncallableInstanceFunction "setFuncallableInstanceFunction"
    T_sp clos_setFuncallableInstanceFunction(T_sp obj, T_sp func)
    {_G();
	if ( Instance_sp iobj = obj.asOrNull<Instance_O>() )
	{
	    return iobj->setFuncallableInstanceFunction(func);
	}
	SIMPLE_ERROR(BF("You can only setFuncallableInstanceFunction on instances - you tried to set it on a: %s")% _rep_(obj));
    };


#define ARGS_af_instanceClassSet "(instance func)"
#define DECL_af_instanceClassSet ""
#define DOCS_af_instanceClassSet "instanceClassSet"
    T_sp af_instanceClassSet(T_sp obj, Class_sp mc)
    {_G();
	if ( Instance_sp iobj = obj.asOrNull<Instance_O>() )
	{
	    return iobj->instanceClassSet(mc);
	} else if ( Class_sp cobj = obj.asOrNull<Class_O>() )
	{
	    return cobj->instanceClassSet(mc);
	}
	SIMPLE_ERROR(BF("You can only instanceClassSet on Instance_O or Class_O - you tried to set it on a: %s")% _rep_(mc));
    };


    void Instance_O::initializeSlots(int numberOfSlots)
    {
        this->_Slots.resize(numberOfSlots,_Unbound<T_O>());
    }

    T_sp Instance_O::oinstancep() const
    {
	return Fixnum_O::create((LongLongInt)(this->_Slots.size()));
    }

    T_sp Instance_O::oinstancepSTAR() const
    {
	return Fixnum_O::create((LongLongInt)(this->_Slots.size()));
    }


    /*! See ECL>>instance.d>>si_allocate_instance */
    T_sp Instance_O::allocateInstance(T_sp theClass, int numberOfSlots )
    {_G();
        Class_sp cl = theClass.as<Class_O>();
        if ( !cl->hasAllocator() ) {
            IMPLEMENT_MEF(BF("Handle no allocator class: %s slots: %d") % _rep_(theClass) % numberOfSlots );
        }
        core::AllocatorFunctor* allocatorP = (cl->getAllocator());
        T_sp obj = allocatorP->allocate();
	if ( obj.nilp()) 
	{
	    printf("%s:%d allocateInstance returning nil!!!\n", __FILE__, __LINE__);
	}
	obj->instanceClassSet(theClass.as<Class_O>());
	obj->initializeSlots(numberOfSlots);
	return(obj);
    }

    /*! See ECL>>instance.d>>si_allocate_raw_instance */
    T_sp Instance_O::allocateRawInstance(T_sp orig, T_sp theClass, int numberOfSlots)
    {_G();
	T_sp output = Instance_O::allocateInstance(theClass,numberOfSlots);
	if ( orig.nilp() )
	{
	    orig = output;
	} else
	{
	    orig->instanceClassSet(theClass.as<Class_O>());
	    orig->adoptSlots(output);
	}
	return(orig);
    }


    void Instance_O::archiveBase(ArchiveP node)
    {
	if (node->saving()) {
	    if ( this->_isgf || this->_Entry!=NULL ) {
		SIMPLE_ERROR(BF("You cannot archive FUNCALLABLE instances or generic-functions"));
	    }
	    SYMBOL_EXPORT_SC_(KeywordPkg,iclass);
//	    Symbol_sp className = this->_Class->name();
//	    node->attribute(kw::_sym_iclass,className);
	    for ( int i(0); i<this->_Slots.size(); ++i ) {
		node->pushVector(this->_Slots[i]);
	    }
	} else {
	    this->_isgf = false;
	    this->_Entry = NULL;
#if 1
	    Symbol_sp className = node->getKind();
//	    node->attribute(kw::_sym_iclass,className);
	    Class_sp cl = eval::funcall(cl::_sym_findClass,className,_lisp->_true()).as<Class_O>();
	    this->_Class = cl;
	    this->initializeSlots(node->vectorSize());
#endif
	    int idx(0);
	    if (node->vectorSize() != this->_Slots.size() ) {
		SIMPLE_ERROR(BF("While loading archive class %s mismatch in number of slots - expected %d - loaded %d slots") % _rep_(this->_Class) % this->_Slots.size() % node->vectorSize());
	    }
	    node->mapVector( [this, &idx] (T_sp value) {
		    this->_Slots[idx++] = value;
		} );
	    this->instanceSigSet();
	}
    }

    Instance_O::Instance_O() :
	Function_O(), _isgf(ECL_NOT_FUNCALLABLE), _Class(_Nil<Class_O>()), _Entry(NULL), _Sig(_Nil<T_O>()) {};

    Instance_O::~Instance_O()
    {
    }



    T_sp Instance_O::instanceSigSet()
    {
#if DEBUG_CLOS>=2
	printf("\nMLOG instanceSigSet of Instance %p  CLASS_OF --> %s\n", (void*)(this), this->_CLASS_OF()->__repr__().c_str() );
#endif
        T_sp classSlots(_Nil<T_O>());
#if 0
        // This is the old code
	T_sp classOf = this->_ C L A S S_OF();
	if ( Instance_sp instClassOf = classOf.asOrNull<Instance_O>() )
	{
	    classSlots = instClassOf->_C L A S S_SLOTS();
	} else if (Class_sp mc = classOf.asOrNull<Class_O>() )
	{
	    classSlots = mc->slots();
	} else
	{
	    SIMPLE_ERROR(BF("illegal class-of"));
	}
#else
        Class_sp mc = this->_instanceClass();
        classSlots = mc->slots();
#endif
	this->_Sig = classSlots;
	return(( classSlots));
    }

    T_sp Instance_O::instanceSig() const
    {
#if DEBUG_CLOS >=2
	stringstream ssig;
	if ( this->_Sig )
	{
	    ssig << this->_Sig->__repr__();
	} else
	{
	    ssig << "UNDEFINED ";
	}
	printf("\nMLOG INSTANCE-SIG of Instance %p \n", (void*)(this) );
#endif
	return(( this->_Sig));
    }


    EXPOSE_CLASS(core,Instance_O);

    void Instance_O::exposeCando(core::Lisp_sp lisp)
    {
	core::class_<Instance_O>()
	    .def("copyInstance",&Instance_O::copyInstance) // TODO: Change this to Instance_O::copyInstance
	    ;

	af_def(CorePkg,"allocateRawInstance",&Instance_O::allocateRawInstance);
	SYMBOL_EXPORT_SC_(ClosPkg,setFuncallableInstanceFunction);
	ClosDefun(setFuncallableInstanceFunction);
	SYMBOL_EXPORT_SC_(CorePkg,instanceClassSet);
	Defun(instanceClassSet);
    }

    void Instance_O::exposePython(core::Lisp_sp lisp)
    {_G();
#ifdef USEBOOSTPYTHON
	PYTHON_CLASS(CorePkg,Instance,"","",_lisp)
	    ;
#endif
    }


    T_sp Instance_O::instanceClassSet(Class_sp mc)
    {_G();
	this->_Class = mc;
	return(this->sharedThis<Instance_O>());
    }


    T_sp Instance_O::instanceRef(int idx) const
    {
#if DEBUG_CLOS >=2
	printf("\nMLOG INSTANCE-REF[%d] of Instance %p --->%s\n", idx, (void*)(this), this->_Slots[idx]->__repr__().c_str() );
#endif
	return(( this->_Slots[idx]));
    }
    T_sp Instance_O::instanceSet(int idx, T_sp val)
    {
#if DEBUG_CLOS>=2
	printf("\nMLOG SI-INSTANCE-SET[%d] of Instance %p to val: %s\n", idx, (void*)(this), val->__repr__().c_str() );
#endif
	this->_Slots[idx] = val;
	return(( val));
    }



    string Instance_O::__repr__() const
    {
	stringstream ss;
	ss << "#S(";
	if ( this->_Class.nilp() ) 
	{
	    ss << "<CLASS-IS_NIL> ";
	} else if ( Class_sp mc = this->_Class.asOrNull<Class_O>() )
	{
	    ss << _rep_(mc) << " ";
	} else
	{
	    ss << "<ADD SUPPORT FOR INSTANCE _CLASS=" << _rep_(this->_Class)<< " >";
	}
	{
	    ss << " #slots[" << this->_Slots.size() << "]" << std::endl;
#if 1
	    for ( int i(0); i<this->_Slots.size(); ++i)
	    {
		T_sp obj = this->_Slots[i];
		ss << "        :slot" << i << " ";
		if ( obj )
		{
		    stringstream sslot;
		    if ( af_consP(obj) )
		    {
			sslot << "CONS...";
			ss << sslot.str() << std::endl;
		    } else if ( Instance_sp inst = obj.asOrNull<Instance_O>() )
		    {
			sslot << "INSTANCE..." ;
			ss << sslot.str() << std::endl;
		    } else
		    {
			sslot << _rep_(obj);
			if (sslot.str().size() > 80 )
			{
			    ss << sslot.str().substr(0,80) << "...";
			} else
			{
			    ss << sslot.str();
			}
			ss << " " << std::endl;
		    }
		} else
		{
		    ss << "UNDEFINED " << std::endl;
		}
	    }
#endif
	}
	ss << ")" << std::endl;
	return(( ss.str()));
    }
	    


    T_sp Instance_O::copyInstance() const
    {
	Instance_sp inew = Instance_O::allocateInstance(this->_Class).as<Instance_O>();
	inew->_isgf = this->_isgf;
	inew->_Slots = this->_Slots;
	inew->_Entry = this->_Entry;
	inew->_Sig = this->_Sig;
	return inew;
    }


    void Instance_O::reshapeInstance(int delta)
    {_G();
	int size = this->_Slots.size() + delta;
	this->_Slots.resize(size,_Unbound<T_O>());
    }
/*
  memcpy(aux->instance.slots, x->instance.slots,
  (delta < 0 ? aux->instance.length : x->instance.length) *
  sizeof(cl_object));
  x->instance = aux->instance;
*/

    SYMBOL_SC_(ClosPkg,standardOptimizedReaderMethod);
    SYMBOL_SC_(ClosPkg,standardOptimizedWriterMethod);

    
    
    

    T_sp Instance_O::setFuncallableInstanceFunction(T_sp functionOrT)
    {_G();
	if (this->_isgf == ECL_USER_DISPATCH)
	{
	    this->reshapeInstance(-1);
	    this->_isgf = ECL_NOT_FUNCALLABLE;
	}
	SYMBOL_EXPORT_SC_(ClPkg,standardGenericFunction);
	SYMBOL_SC_(ClosPkg,standardOptimizedReaderFunction);
	SYMBOL_SC_(ClosPkg,standardOptimizedWriterFunction);
	if (functionOrT == _lisp->_true() )
	{
	    this->_isgf = ECL_STANDARD_DISPATCH;
	    this->_Entry = &generic_function_dispatch;
	} else if (functionOrT == cl::_sym_standardGenericFunction)
	{
	    this->_isgf = ECL_RESTRICTED_DISPATCH;
	    this->_Entry = &generic_function_dispatch;
	} else if (functionOrT.nilp()) {
	    this->_isgf = ECL_NOT_FUNCALLABLE;
	    this->_Entry = notFuncallableDispatch;
	} else if (functionOrT == clos::_sym_standardOptimizedReaderMethod )
	{
	    /* WARNING: We assume that f(a,...) behaves as f(a,b) */
	    this->_isgf = ECL_READER_DISPATCH;
	    // TODO: Switch to using slotReaderDispatch like ECL for improved performace
//	    this->_Entry = &slotReaderDispatch;
	    this->_Entry = &generic_function_dispatch;
	} else if (functionOrT == clos::_sym_standardOptimizedWriterMethod )
	{
	    /* WARNING: We assume that f(a,...) behaves as f(a,b) */
	    this->_isgf = ECL_WRITER_DISPATCH;
	    // TODO: Switch to using slotWriterDispatch like ECL for improved performace
//	    this->_Entry = &slotWriterDispatch;
	    this->_Entry = &generic_function_dispatch;
	} else if (!af_functionP(functionOrT)) 
	{
	    TYPE_ERROR(functionOrT,cl::_sym_function);
	    //SIMPLE_ERROR(BF("Wrong type argument: %s") % functionOrT->__repr__());
	} else
	{
	    this->reshapeInstance(+1);
	    this->_Slots[this->_Slots.size() - 1] = functionOrT;
	    this->_isgf = ECL_USER_DISPATCH;
	    // TODO: Switch to using userFunctionDispatch like ECL for improved performace
//	    this->_Entry = &userFunctionDispatch;
	    this->_Entry = &generic_function_dispatch;
	}
	return(( this->sharedThis<Instance_O>()));
    }


    bool Instance_O::genericFunctionP() const
    {
	return(this->_isgf);
    }


    bool Instance_O::equalp(T_sp obj) const
    {
        if ( this->eq(obj) ) return true;
        if ( Instance_sp iobj = obj.asOrNull<Instance_O>() ) {
            if ( this->_Class != iobj->_Class ) return false;
            for ( int i(0), iEnd(this->_Slots.size()); i<iEnd; ++i ) {
                if ( !af_equalp(this->_Slots[i],iobj->_Slots[i]) ) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }



    T_mv Instance_O::INVOKE(int nargs, ArgArray args)
    {
	ASSERT(this->_Entry!=NULL);
	LispCompiledFunctionIHF _frame(_lisp->invocationHistoryStack(),this->asSmartPtr());
	return(( (this->_Entry)(*this,nargs,args)));
    }





    void Instance_O::describe()
    {
        printf("#<Instance_O>\n");
        printf("isgf %d\n", this->_isgf);
        printf("_Class: %s\n", _rep_(this->_Class).c_str());
        for (int i(0); i<this->_Slots.size(); ++i ) {
            printf("_Slots[%d]: %s\n", i, _rep_(this->_Slots[i]).c_str());
        }
    }

    



};