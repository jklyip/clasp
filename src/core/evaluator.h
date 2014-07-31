#ifndef	evaluator_H
#define evaluator_H


#include "foundation.h"
#include "ql.h"
#include "evaluator.fwd.h"
#include "executables.h"
#include "activationFrame.h"

namespace cl {
    extern core::Symbol_sp _sym_findClass;
};

namespace core
{


    T_sp af_interpreter_lookup_variable(Symbol_sp sym, T_sp env);
    Function_sp af_interpreter_lookup_function(Symbol_sp sym, T_sp env);
    Function_sp af_interpreter_lookup_macro(Symbol_sp sym, T_sp env);
    Function_sp af_interpreter_lookup_symbol_macro(Symbol_sp sym, T_sp env);



    namespace eval
    {


        /*! Evaluate within env.
          See ecl/src/c/compiler.d:eval-with-env */
        T_mv af_topLevelEvalWithEnv(T_sp form, T_sp env, bool stepping=false, bool compiler_env_p=true, bool execute=true);

	extern Cons_sp evaluateList(Cons_sp args, T_sp environment);
	extern T_mv evaluateListReturnLast(Cons_sp args, T_sp environment);


	extern T_mv sp_progn(Cons_sp code, T_sp env);
	extern T_mv sp_setq(Cons_sp args, T_sp environment);



	extern void defineSpecialOperatorsAndMacros(Package_sp pkg);



	Function_sp lookupFunction(T_sp functionDesignator, T_sp env);


	/*! Evaluate a list of expressions (args) into an ActivationFrame that has
	  enough storage to accept each of the objects that is generated by the list of args */
	extern void evaluateIntoActivationFrame(ActivationFrame_sp af, Cons_sp args, T_sp environment );


/*! See the CLHS for "apply" - all arguments are in args 
  (functionDesignator) can be a Symbol or an Function
*/

	extern T_mv applyClosureToActivationFrame(Closure* closureP, ActivationFrame_sp af);

	extern T_mv applyToActivationFrame(T_sp functionDesignator, ActivationFrame_sp af);


	inline T_mv apply( T_sp fn, Cons_sp argsPLUS )
	{
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
	    int numArgsPassed = 0;
	    int numArgsPlus = af_length(argsPLUS);
	    int nargs = numArgsPassed + numArgsPlus;
	    ValueFrame_sp frame(ValueFrame_O::create_fill_numExtraArgs(numArgsPlus,_Nil<ActivationFrame_O>()));
	    Cons_sp cur = argsPLUS;
	    for ( int i=numArgsPassed; i<nargs; ++i ) {
		frame->operator[](i) = oCar(cur);
		cur=cCdr(cur);
	    }
            return applyToActivationFrame(func,frame);
	}

	inline T_mv apply( T_sp fn, T_sp a0,
                           Cons_sp argsPLUS )
	{
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
	    int numArgsPassed = 1;
	    int numArgsPlus = af_length(argsPLUS);
	    int nargs = numArgsPassed + numArgsPlus;
	    ValueFrame_sp frame(ValueFrame_O::create_fill_numExtraArgs(numArgsPlus,_Nil<ActivationFrame_O>(),a0));
	    Cons_sp cur = argsPLUS;
	    for ( int i=numArgsPassed; i<nargs; ++i ) {
		frame->operator[](i) = oCar(cur);
		cur=cCdr(cur);
	    }
            return applyToActivationFrame(func,frame);
	}

	inline T_mv apply( T_sp fn, T_sp a0, T_sp a1,
                           Cons_sp argsPLUS )
	{
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
	    int numArgsPassed = 2;
	    int numArgsPlus = af_length(argsPLUS);
	    int nargs = numArgsPassed + numArgsPlus;
	    ValueFrame_sp frame(ValueFrame_O::create_fill_numExtraArgs(numArgsPlus,_Nil<ActivationFrame_O>(),a0,a1));
	    Cons_sp cur = argsPLUS;
	    for ( int i=numArgsPassed; i<nargs; ++i ) {
		frame->operator[](i) = oCar(cur);
		cur=cCdr(cur);
	    }
            return applyToActivationFrame(func,frame);
	}

	inline T_mv apply( T_sp fn, T_sp a0, T_sp a1, T_sp a2,
                           Cons_sp argsPLUS )
	{
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
	    int numArgsPassed = 3;
	    int numArgsPlus = af_length(argsPLUS);
	    int nargs = numArgsPassed + numArgsPlus;
	    ValueFrame_sp frame(ValueFrame_O::create_fill_numExtraArgs(numArgsPlus,_Nil<ActivationFrame_O>(),a0,a1,a2));
	    Cons_sp cur = argsPLUS;
	    for ( int i=numArgsPassed; i<nargs; ++i ) {
		frame->operator[](i) = oCar(cur);
		cur=cCdr(cur);
	    }
            return applyToActivationFrame(func,frame);
	}

	inline T_mv apply( T_sp fn, T_sp a0, T_sp a1, T_sp a2, T_sp a3,
                           Cons_sp argsPLUS )
	{
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
	    int numArgsPassed = 4;
	    int numArgsPlus = af_length(argsPLUS);
	    int nargs = numArgsPassed + numArgsPlus;
	    ValueFrame_sp frame(ValueFrame_O::create_fill_numExtraArgs(numArgsPlus,_Nil<ActivationFrame_O>(),a0,a1,a2,a3));
	    Cons_sp cur = argsPLUS;
	    for ( int i=numArgsPassed; i<nargs; ++i ) {
		frame->operator[](i) = oCar(cur);
		cur=cCdr(cur);
	    }
            return applyToActivationFrame(func,frame);
	}


	inline T_mv apply( T_sp fn, T_sp a0, T_sp a1, T_sp a2, T_sp a3, T_sp a4,
                           Cons_sp argsPLUS )
	{
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
	    int numArgsPassed = 5;
	    int numArgsPlus = af_length(argsPLUS);
	    int nargs = numArgsPassed + numArgsPlus;
	    ValueFrame_sp frame(ValueFrame_O::create_fill_numExtraArgs(numArgsPlus,_Nil<ActivationFrame_O>(),a0,a1,a2,a3,a4));
	    Cons_sp cur = argsPLUS;
	    for ( int i=numArgsPassed; i<nargs; ++i ) {
		frame->operator[](i) = oCar(cur);
		cur=cCdr(cur);
	    }
            return applyToActivationFrame(func,frame);
	}

	inline T_mv apply( T_sp fn, T_sp a0, T_sp a1, T_sp a2, T_sp a3, T_sp a4, T_sp a5,
                           Cons_sp argsPLUS )
	{
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
	    int numArgsPassed = 6;
	    int numArgsPlus = af_length(argsPLUS);
	    int nargs = numArgsPassed + numArgsPlus;
	    ValueFrame_sp frame(ValueFrame_O::create_fill_numExtraArgs(numArgsPlus,_Nil<ActivationFrame_O>(),a0,a1,a2,a3,a4,a5));
	    Cons_sp cur = argsPLUS;
	    for ( int i=numArgsPassed; i<nargs; ++i ) {
		frame->operator[](i) = oCar(cur);
		cur=cCdr(cur);
	    }
            return applyToActivationFrame(func,frame);
	}

	inline T_mv apply( T_sp fn, T_sp a0, T_sp a1, T_sp a2, T_sp a3, T_sp a4, T_sp a5, T_sp a6, Cons_sp argsPLUS )
	{
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
	    int numArgsPassed = 7;
	    int numArgsPlus = af_length(argsPLUS);
	    int nargs = numArgsPassed + numArgsPlus;
	    ValueFrame_sp frame(ValueFrame_O::create_fill_numExtraArgs(numArgsPlus,_Nil<ActivationFrame_O>(),a0,a1,a2,a3,a4,a5,a6));
	    Cons_sp cur = argsPLUS;
	    for ( int i=numArgsPassed; i<nargs; ++i ) {
		frame->operator[](i) = oCar(cur);
		cur=cCdr(cur);
	    }
            return applyToActivationFrame(func,frame);
	}

	inline T_mv apply( T_sp fn, T_sp a0, T_sp a1, T_sp a2, T_sp a3, T_sp a4, T_sp a5, T_sp a6, T_sp a7, Cons_sp argsPLUS )
	{
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
	    int numArgsPassed = 8;
	    int numArgsPlus = af_length(argsPLUS);
	    int nargs = numArgsPassed + numArgsPlus;
	    ValueFrame_sp frame(ValueFrame_O::create_fill_numExtraArgs(numArgsPlus,_Nil<ActivationFrame_O>(),a0,a1,a2,a3,a4,a5,a6,a7));
	    Cons_sp cur = argsPLUS;
	    for ( int i=numArgsPassed; i<nargs; ++i ) {
		frame->operator[](i) = oCar(cur);
		cur=cCdr(cur);
	    }
            return applyToActivationFrame(func,frame);
	}


#define USE_ARRAY0


	inline T_mv funcall(T_sp fn) {
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
            Functoid* ft = func->closure;
            T_mv result;
            (*ft)(&result,0,NULL,NULL,NULL);
            return result;
        }

	template <class ARG0>
	inline T_mv funcall(T_sp fn, ARG0 arg0) {
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
            Functoid* ft = func->closure;
            T_mv result;
            (*ft)(&result,1,arg0.asTPtr(),NULL,NULL);
            return result;
        }

	template <class ARG0, class ARG1>
	inline T_mv funcall(T_sp fn, ARG0 arg0, ARG1 arg1) {
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
            if ( !func.pointerp() ) {                                                                                                                        
                // While booting, cl::_sym_findClass will apply'd before it is bound to a symbol                                                             
                if ( fn == cl::_sym_findClass ) {                                                                                                            
                    return(af_findClass(arg0,true,_Nil<Environment_O>()));                                                    
                }                                                                                                                                            
                SIMPLE_ERROR(BF("Could not find function %s") % _rep_(fn) );                                                                                 
            }                                                                                                                                                

            Functoid* ft = func->closure;
            T_mv result;
            (*ft)(&result,2,arg0.asTPtr(),arg1.asTPtr(),NULL);
            return result;
        }

	template <class ARG0, class ARG1, class ARG2, class...ARGS>
	inline T_mv funcall(T_sp fn, ARG0 arg0, ARG1 arg1, ARG2 arg2, ARGS...args) {
	    Function_sp func = lookupFunction(fn,_Nil<Environment_O>());
            Functoid* ft = func->closure;
            T_mv result;
            size_t vnargs = sizeof...(ARGS);
            (*ft)(&result,3+vnargs,arg0.asTPtr(),arg1.asTPtr(),arg2.asTPtr(),args.asTPtr()...);
            return result;
        }
    };
};



#endif
