#ifndef	_core_primitives_H
#define _core_primitives_H

#include "core/object.h"
#include "corePackage.fwd.h"
#include "core/lispVector.h"
#include "core/character.fwd.h"
#include "wrappers.h"


namespace core
{

    extern Cons_mv af_separatePairList(Cons_sp listOfPairs);

    extern Symbol_mv af_functionBlockName(T_sp functionName);

    extern void af_ensure_single_dispatch_generic_function(Symbol_sp gfname, LambdaListHandler_sp llh);

    extern T_mv af_read_delimited_list(Character_sp chr, T_sp input_stream_designator, T_sp recursive_p);

    T_sp af_read(T_sp input_stream_designator, T_sp eof_error_p=_Nil<T_O>(), T_sp eof_value=_Nil<T_O>(), T_sp recursive_p=_Nil<T_O>());

    extern void af_ensure_single_dispatch_method(Symbol_sp gfname, Class_sp receiver_class, LambdaListHandler_sp lambda_list_handler, Cons_sp declares, Str_sp docstring, CompiledBody_sp body );


    EXTERN_FN(read);
    EXTERN_FN(read_delimited_list);
    EXTERN_FN(convert_to_list_of_classes);

    EXTERN_GENERIC(make_instance);
    EXTERN_GENERIC(ensure_class_using_class);
    EXTERN_GENERIC(reinitialize_instance);


    T_sp af_type_of(T_sp x);
    T_mv af_notany(T_sp predicate, Cons_sp sequences);
    T_mv af_every(T_sp predicate, Cons_sp sequences);

    List_mv af_mapcar(T_sp func_desig, Cons_sp lists);

    T_sp af_append(Cons_sp lists);

    
    Stream_mv af_open(T_sp filespec, Symbol_sp direction, T_sp element_type, T_sp if_exists, T_sp if_does_not_exist, T_sp external_format );

    Symbol_mv af_gensym(T_sp x);







    class SequenceStepper
    {
    public:
	virtual bool advance() = 0;
	virtual T_sp element() const = 0;
	virtual ~SequenceStepper() {};
        virtual GC_RESULT onHeapScanGCRoots(GC_SCAN_ARGS_PROTOTYPE) = 0;
    };




    class VectorStepper : public SequenceStepper
    {
    private:
	Vector_sp	_Domain;
	int		_Index;
    public:
	VectorStepper(Vector_sp domain) : _Domain(domain), _Index(0) {}; 
	virtual bool advance() { this->_Index++; return (this->_Index >= af_length(this->_Domain));};
	virtual T_sp element() const
	{
	    if (this->_Index<af_length(this->_Domain))
	    {
		return this->_Domain->elt(this->_Index);
	    } else
	    {
		return _Nil<T_O>();
	    }
	};
        virtual GC_RESULT onHeapScanGCRoots(GC_SCAN_ARGS_PROTOTYPE)
#ifndef USE_MPS
        { return GC_RES_OK; }
#endif
        ;
    };





    class ConsStepper : public SequenceStepper
    {
    private:
	Cons_sp	_Cur;
    public:
	ConsStepper(Cons_sp first) : _Cur(first) {};
        virtual GC_RESULT onHeapScanGCRoots(GC_SCAN_ARGS_PROTOTYPE)
#ifndef USE_MPS
        { return GC_RES_OK; }
#endif
            ;
	virtual bool advance() { this->_Cur = cCdr(this->_Cur); return this->_Cur.nilp(); };
	virtual T_sp element() const { return oCar(this->_Cur);};
    };







    /*! A class that generates lists of elements drawn from a list of sequences.
      Given (a1 a2 a3) (b1 b2 b3) (c1 c2 c3)
      Will successively generate (a1 b1 c1) (a2 b2 c2) (a3 b3 c3) */
    class ListOfSequenceSteppers : public gctools::StackRoot
    {
	friend class ListOfListSteppers;
    private:
	vector<SequenceStepper*>	_Steppers;
	bool				_AtEnd;
    public:
	ListOfSequenceSteppers() {};
	ListOfSequenceSteppers(Cons_sp sequences);
	virtual ~ListOfSequenceSteppers();
	bool atEnd() const { return this->_AtEnd;};
//	Cons_sp makeListFromCurrentSteppers() const;
	void fillValueFrameUsingCurrentSteppers(ActivationFrame_sp frame) const;
	/* Advance all of the steppers - return false if the end is hit otherwise true */
	bool advanceSteppers();
	int size() { return this->_Steppers.size();};

        virtual GC_RESULT onStackScanGCRoots(GC_SCAN_ARGS_PROTOTYPE)
        {
#ifdef USE_MPS
            for ( auto& it : this->_Steppers ) {
                GC_RESULT res = it->onHeapScanGCRoots(GC_SCAN_ARGS_PASS);
                if ( res!=GC_RES_OK ) return res;
            };
#endif
            return GC_RES_OK;
        }

    };


    /*! Return the SourceFileInfo for the obj - if obj is nil then return 
      one for anonymous */

    /*! Expose the primitives to cando */
    void	initialize_primitives();
    /*! Expose the primitives to python */
    void	initializePythonPrimitives(Lisp_sp lisp);







};
#endif /* _core_primitives_H */

