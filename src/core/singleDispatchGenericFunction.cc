//#define DEBUG_SINGLE_DISPATCH 1
/*
    File: singleDispatchGenericFunction.cc
*/

/*
Copyright (c) 2014, Christian E. Schafmeister

CLASP is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

See directory 'clasp/licenses' for full details.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/* -^- */

//#define DEBUG_LEVEL_FULL
#include <clasp/core/foundation.h>
#include <clasp/core/common.h>
#include <clasp/core/environment.h>
#include <clasp/core/lispCallingConvention.h>
#include <clasp/core/symbolTable.h>
#include <clasp/core/hashTable.h>
#include <clasp/core/hashTableEql.h>
#include <clasp/core/primitives.h>
#include <clasp/core/package.h>
#include <clasp/core/array.h>
#include <clasp/core/funcallableInstance.h>
#include <clasp/core/documentation.h>
#include <clasp/core/multipleValues.h>
#include <clasp/core/lambdaListHandler.h>
#include <clasp/core/singleDispatchGenericFunction.h>
#include <clasp/core/singleDispatchMethod.h>
#include <clasp/core/wrappers.h>
#include <clasp/core/sort.h>

namespace core {

CL_DECLARE();
CL_DOCSTRING("ensureSingleDispatchGenericFunction");
CL_DEFUN FuncallableInstance_sp core__ensure_single_dispatch_generic_function(T_sp gfname, LambdaListHandler_sp llhandler, bool autoExport, size_t singleDispatchArgumentIndex) {
  T_sp tgfn;
  if (!cl__fboundp(gfname)) {
    tgfn = _Nil<T_O>();
  } else {
    tgfn = cl__fdefinition(gfname);
  }
  FuncallableInstance_sp gfn;
  if (tgfn.nilp()) {
    // Use CAS to push the new gfname into the list of single dispatch generic functions
    T_sp expected;
    Cons_sp cell = core::Cons_O::create(gfname,_Nil<T_O>());
    do {
      expected = _lisp->_Roots._SingleDispatchGenericFunctions.load();
      cell->rplacd(expected);
    } while (!_lisp->_Roots._SingleDispatchGenericFunctions.compare_exchange_weak(expected,cell));
    if (gfname.consp() && CONS_CAR(gfname) == cl::_sym_setf) {
      Symbol_sp setf_gfname = CONS_CAR(CONS_CDR(gfname));
      if (setf_gfname->fboundp_setf()) {
        SIMPLE_ERROR(BF("The name %s has something bound to its setf function slot but no generic function with that name was found") % _rep_(gfname));
      }
      gfn = FuncallableInstance_O::create_single_dispatch_generic_function(gfname, llhandler,singleDispatchArgumentIndex);
      setf_gfname->setSetfFdefinition(gfn);
      if (autoExport) setf_gfname->exportYourself();
    } else {
      ASSERT(gc::IsA<Symbol_sp>(gfname));
      Symbol_sp gfname_symbol = gc::As_unsafe<Symbol_sp>(gfname);
      if (gfname_symbol->fboundp()) {
        T_sp symFunc = gfname_symbol->symbolFunction();
        SIMPLE_ERROR(BF("The symbol %s has something bound to its function slot but no FuncallableInstance with that name was found") % _rep_(gfname));
      }
      gfn = FuncallableInstance_O::create_single_dispatch_generic_function(gfname, llhandler,singleDispatchArgumentIndex);
      gfname_symbol->setf_symbolFunction(gfn);
      if (autoExport) gfname_symbol->exportYourself();
    }
  } else {
    gfn = gc::As<FuncallableInstance_sp>(tgfn);
  }
  return gfn;
};


CL_LAMBDA("gf gfname receiver-class &key lambda-list-handler declares (docstring \"\") body ");
CL_DECLARE();
CL_DOCSTRING("ensureSingleDispatchMethod creates a method and adds it to the single-dispatch-generic-function");
CL_DEFUN void core__ensure_single_dispatch_method(FuncallableInstance_sp gfunction, T_sp tgfname, Instance_sp receiver_class, LambdaListHandler_sp lambda_list_handler, List_sp declares, T_sp docstring, Function_sp body) {
  //	string docstr = docstring->get();
//  SingleDispatchGenericFunctionClosure_sp gf = gc::As<SingleDispatchGenericFunctionClosure_sp>(gfname->symbolFunction());
  SingleDispatchMethod_sp method = SingleDispatchMethod_O::create(tgfname,
                                                                  receiver_class,
                                                                  lambda_list_handler,
                                                                  declares,
                                                                  docstring,
                                                                  body);
  ASSERT(lambda_list_handler.notnilp());
  LambdaListHandler_sp gf_llh = gc::As<LambdaListHandler_sp>(gfunction->lambdaListHandler());
  if (lambda_list_handler->numberOfRequiredArguments() != gf_llh->numberOfRequiredArguments()) {
    SIMPLE_ERROR(BF("There is a mismatch between the number of required arguments\n"
                    " between the single-dispatch-generic-function %s which expects %d arguments\n"
                    " for methods: %s\n"
                    " and another method with the same name in %s which expects %d arguments\n"
                    " - this is probably due to the way you can overload method names with\n"
                    " different argument signatures in C++ which does not translate well\n"
                    " to Common Lisp.\n"
                    " --> The solution is to give the most recent Common Lisp method you defined\n"
                    " a new name by prefixing it with the class name\n"
                    " eg: getFilename -> PresumedLoc-getFilename") %
                 _rep_(tgfname) % gf_llh->numberOfRequiredArguments() % _rep_(gfunction->callHistory()) % _rep_(receiver_class) % lambda_list_handler->numberOfRequiredArguments());
  }
  gfunction->addSingleDispatchMethod(method);
  if (docstring.notnilp()) {
    core::ext__annotate(method,cl::_sym_documentation,core::_sym_single_dispatch_method, docstring );
  }
};


/*! I should probably get the key for each element first and then sort */
class OrderByClassPrecedence {
private:
public:
  OrderByClassPrecedence() {
  }
  bool operator()(Instance_sp x, Instance_sp y) {
    List_sp yClassPrecedence = y->instanceRef(Instance_O::REF_CLASS_CLASS_PRECEDENCE_LIST);
    if (yClassPrecedence.notnilp()) {
      Cons_sp yCons = gc::As<Cons_sp>(yClassPrecedence);
      return (yCons->memberEq(x).notnilp());
    }
    return false;
  }
};


ComplexVector_T_sp sortDispatchVectorByClassPrecedence(ComplexVector_T_sp dispatchVector)
{
  if (dispatchVector->length()<=2) return dispatchVector;
  gctools::Vec0<Instance_sp> classes;
  classes.resize(dispatchVector->length()/2);
  for ( size_t ii = 0; ii< dispatchVector->length(); ii += 2) {
    classes[ii/2] = gc::As<Instance_sp>((*dispatchVector)[ii]);
  }
  OrderByClassPrecedence orderer;
  sort::quickSortVec0(classes,0,classes.size(),orderer);
  ComplexVector_T_sp sorted = ComplexVector_T_O::make(dispatchVector->length(),_Nil<T_O>(),make_fixnum(0));
  for ( size_t ii = 0; ii<classes.size(); ii++ ) {
    for (size_t jj = 0; jj<dispatchVector->length(); jj+=2) {
      if ((*dispatchVector)[jj] == classes[ii]) {
        sorted->vectorPushExtend(classes[ii]);
        sorted->vectorPushExtend((*dispatchVector)[jj+1]);
        goto FOUND;
      }
    }
    printf("%s:%d Could not find class %s\n", __FILE__, __LINE__, _rep_(classes[ii]).c_str());
    abort();
  FOUND:
    (void)0;
  }
  return sorted;
}


/*! Recursively descend through dispatchClass subclasses and add them to the dispatch vector */
void recursivelySatiate(FuncallableInstance_sp gfun, Instance_sp dispatchClass, SingleDispatchMethod_sp method, ComplexVector_T_sp newDispatchVector)
{
  for ( size_t ii = 0; ii<newDispatchVector->length(); ii+=2) {
    if (dispatchClass == gc::As<Instance_sp>((*newDispatchVector)[ii])) {
      (*newDispatchVector)[ii+1] = method;
      goto FOUND;
    }
  }
#ifdef DEBUG_SINGLE_DISPATCH
  printf("%s:%d      In recursivelySatiate gfun: %s for receiver class %s\n", __FILE__, __LINE__, _rep_(gfun).c_str(), _rep_(dispatchClass).c_str());
#endif
  newDispatchVector->vectorPushExtend(dispatchClass);
  newDispatchVector->vectorPushExtend(method);
 FOUND:
  List_sp directSubClasses = gc::As<List_sp>(dispatchClass->instanceRef(Instance_O::REF_CLASS_DIRECT_SUBCLASSES));
  while (directSubClasses.notnilp()) {
    Instance_sp directSubClass = gc::As<Instance_sp>(oCar(directSubClasses));
    recursivelySatiate(gfun,directSubClass,method,newDispatchVector);
    directSubClasses = oCdr(directSubClasses);
  }
}


#if 0
CL_DEFUN void core__satiateSingleDispatchGenericFunctions()
{
  List_sp singleDispatchGenericFunctions = _lisp->_Roots._SingleDispatchGenericFunctions.load();
  size_t len = cl__length(singleDispatchGenericFunctions);
  SimpleVector_sp functions = SimpleVector_O::make(len);
  size_t index = 0;
  List_sp cur = singleDispatchGenericFunctions;
  while (cur.notnilp()) {
    T_sp tgfunName = oCar(cur);
    FuncallableInstance_sp gfun = gc::As<FuncallableInstance_sp>(cl__fdefinition(tgfunName));
    ComplexVector_T_sp dispatchVector = gc::As<ComplexVector_T_sp>(gfun->_Rack->low_level_rackRef(Instance_O::REF_SINGLE_DISPATCH_SPECIALIZER_DISPATCH_VECTOR));
    ComplexVector_T_sp sortedDispatchVector = sortDispatchVectorByClassPrecedence(dispatchVector);
#ifdef DEBUG_SINGLE_DISPATCH
    printf("%s:%d vvvvvvvvvvvvvvvvvv  %s with %lu entries\n", __FILE__, __LINE__, _rep_(tgfunName).c_str(), dispatchVector->length()/2 );
    printf("%s:%d           dispatchVector\n", __FILE__, __LINE__);
    for ( size_t ii = 0; ii<dispatchVector->length(); ii+= 2) printf("      %s\n", _rep_((*dispatchVector)[ii]).c_str());
    if (dispatchVector->length()>2) {
      printf("%s:%d     sortedDispatchVector\n", __FILE__, __LINE__);
      for ( size_t ii = 0; ii<sortedDispatchVector->length(); ii+= 2) printf("      %s\n", _rep_((*sortedDispatchVector)[ii]).c_str());
    }
#endif
    ComplexVector_T_sp newDispatchVector = ComplexVector_T_O::make(16,_Nil<T_O>(),make_fixnum(0));
    for (size_t ii = 0; ii<sortedDispatchVector->length(); ii += 2) {
      Instance_sp dispatchClass = gc::As<Instance_sp>((*sortedDispatchVector)[ii]);
      SingleDispatchMethod_sp method = gc::As<SingleDispatchMethod_sp>((*sortedDispatchVector)[ii+1]);
      recursivelySatiate(gfun,dispatchClass,method,newDispatchVector);
    }
    gfun->_Rack->low_level_rackSet(Instance_O::REF_SINGLE_DISPATCH_SPECIALIZER_DISPATCH_VECTOR,newDispatchVector);
#ifdef DEBUG_SINGLE_DISPATCH
    printf("%s:%d ^^^^^^^^^^^^^^^^^  satiating %s there are %lu entries\n", __FILE__, __LINE__, _rep_(tgfunName).c_str(), newDispatchVector->length()/2 );
#endif
    cur = oCdr(cur);
  }
}
#endif

// ----------------------------------------------------------------------
//

  SYMBOL_EXPORT_SC_(CorePkg, ensureSingleDispatchGenericFunction);
  SYMBOL_EXPORT_SC_(CorePkg, ensureSingleDispatchMethod);



}; /* core */
