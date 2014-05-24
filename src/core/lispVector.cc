#define	DEBUG_LEVEL_FULL

#include "core/common.h"
#include "core/environment.h"
#include "symbolTable.h"
#include "core/lispVector.h"
#include "core/str.h"
#include "core/evaluator.h"
#include "core/strWithFillPtr.h"
#include "core/vectorObjects.h"
#include "core/vectorObjectsWithFillPtr.h"
#include "core/character.h"
#include "core/wrappers.h"
namespace core
{

// ----------------------------------------------------------------------
//




#define DOCS_af_make_vector "make_vector See si_make_vector in ecl>>array.d"
#define ARGS_af_make_vector "(element-type dimension adjustable fill-pointer displaced-to displaced-index-offset &optional initial-element initial-contents)"
#define DECL_af_make_vector ""    
    SYMBOL_EXPORT_SC_(ClPkg,subtypep);
    Vector_sp af_make_vector(Symbol_sp element_type,
			int dimension,
			bool adjustable,
			T_sp fill_pointer,
			T_sp displaced_to,
			T_sp displaced_index_offset,
			T_sp initial_element,
			Sequence_sp initialContents )
    {_G();
	ASSERTF(displaced_to.nilp(),BF("Add support for make-vector :displaced-to"));
	ASSERTF(displaced_index_offset.nilp() || displaced_index_offset.as<Fixnum_O>()->get()==0,BF("Add support for make-vector non-zero :displaced-index-offset "));
	if ( element_type == cl::_sym_bit )
	{
	    IMPLEMENT_MEF(BF("Handle bitvectors"));
	} else if ( element_type == cl::_sym_BaseChar_O
		    || element_type == cl::_sym_Character_O
		    || element_type == cl::_sym_StandardChar_O
		    || element_type == cl::_sym_ExtendedChar_O )
	{
	    // Currently any kind of Character vector is a Str or subclass
	    // TODO: Maybe use other types of strings - unicode?
	    char c = ' ';
	    if ( Character_sp cc = initial_element.asOrNull<Character_O>() )
	    {
		if ( cc.notnilp() ) c = cc->asChar();
	    }
	    if ( fill_pointer.notnilp() )
	    {
		int ifp = 0;
		if ( fill_pointer == BRCL_T ) ifp = dimension;
		else ifp = MIN(dimension,abs(fill_pointer.as<Fixnum_O>()->get()));
		return StrWithFillPtr_O::create(c,dimension,ifp,adjustable,initialContents);
	    }
	    return(Str_O::create(' ',dimension,initialContents));
	} else
	{
	    if ( fill_pointer.notnilp() )
	    {
		int ifp = 0;
		if ( fill_pointer == BRCL_T ) ifp = dimension;
		else ifp = fill_pointer.as<Fixnum_O>()->get();
		return VectorObjectsWithFillPtr_O::make(initial_element,initialContents,dimension,ifp,adjustable);
	    } else
	    {
		return VectorObjects_O::make(initial_element,initialContents,dimension,adjustable);
	    }
	}
	SIMPLE_ERROR(BF("Handle make-vector :element-type %s") % _rep_(element_type) );
    };



    
    
#define ARGS_af_adjustVector "(array dimensions initial-element initial-contents)"
#define DECL_af_adjustVector ""
#define DOCS_af_adjustVector "adjustVector"
    T_sp af_adjustVector(T_sp array, int new_dimensions, T_sp initial_element, Cons_sp initial_contents )
    {_G();
	if ( VectorObjects_sp vo = array.asOrNull<VectorObjects_O>() )
	{
	    vo->adjust(initial_element, initial_contents, new_dimensions );
	    return vo;
	}
	IMPLEMENT_MEF(BF("Implement adjustVector for: %s")% _rep_(array) );
    };







    void Vector_O::initialize()
    {_OF();
	this->Bases::Base1::Base::initialize();
	this->Bases::Base1::initialize();
	this->Bases::Base2::initialize();
    }


    void Vector_O::archiveBase(::core::ArchiveP node)
    {
	// Do nothing
    }


    int Vector_O::arrayDimension(int axisNumber) const
    {
	ASSERTF(axisNumber==0,BF("Illegal axis number %d for Vector") % axisNumber);
	return this->dimension();
    }

    Cons_sp Vector_O::arrayDimensions() const
    {
	return Cons_O::create(Fixnum_O::create(this->dimension()),_Nil<Cons_O>());
    }



    Sequence_sp Vector_O::reverse()
    {_OF();
	int thisLength = this->length();
	Vector_sp newVec = eval::funcall(_sym_make_vector,this->_instanceClass()->className(),Fixnum_O::create(thisLength)).as<Sequence_O>().as<Vector_O>();
	for ( int i=0; i<thisLength; i++ )
	{
	    int ri = thisLength - i;
	    newVec->setf_elt(ri,this->elt(i));
	}
	return newVec;
    }

    /*! Return the reversed vector destructively modifying the current vector */
// Swaps at +
// Length = 6   halfLength = 3
//  0 1 2 3 4 5
//  + + + 
//  5 4 3 2 1 0
// Length = 5   halfLength = 2
//  0 1 2 3 4
//  + + 
//  4 3 2 1 0 
    Sequence_sp Vector_O::nreverse()
    {_OF();
	int thisLength = this->length();
	int halfLength = thisLength/2;  // 5/2 = 2  0 
	T_sp temp;
	for ( int i=0; i<halfLength; i++ )
	{
	    int ri = thisLength-i;
	    this->swapElements(i,ri);
	}
	return this->sharedThis<Sequence_O>();
    }


    
    
#define ARGS_cl_vectorPush "(newElement vector)"
#define DECL_cl_vectorPush ""
#define DOCS_cl_vectorPush "vectorPush"
    Fixnum_sp cl_vectorPush(T_sp newElement, Vector_sp vec)
    {_G();
        return vec->vectorPush(newElement);
    };



    
    
#define ARGS_cl_vectorPushExtend "(newElement vector &optional (exension 16))"
#define DECL_cl_vectorPushExtend ""
#define DOCS_cl_vectorPushExtend "vectorPushExtend"
    Fixnum_sp cl_vectorPushExtend(T_sp newElement, Vector_sp vec, int extension)
    {_G();
	return vec->vectorPushExtend(newElement,extension);
    }




    EXPOSE_CLASS(core,Vector_O);

    void Vector_O::exposeCando(::core::Lisp_sp lisp)
    {_G();
	::core::class_<Vector_O>()
	      .def("adjustableArrayP",&Vector_O::adjustableArrayP)
//	.initArgs("(self)")
	;
	SYMBOL_SC_(CorePkg,make_vector);
	Defun(make_vector);
	SYMBOL_EXPORT_SC_(CorePkg,adjustVector);
	Defun(adjustVector);
        SYMBOL_EXPORT_SC_(ClPkg,vectorPush);
        ClDefun(vectorPush);
        SYMBOL_EXPORT_SC_(ClPkg,vectorPushExtend);
        ClDefun(vectorPushExtend);
    }

    void Vector_O::exposePython(::core::Lisp_sp lisp)
    {
//	PYTHON_CLASS_2BASES(Pkg(),Vector,"","",_LISP)
#ifdef USEBOOSTPYTHON
    boost::python::class_< Vector_O,
			   mem::smart_ptr< Vector_O >,
	    boost::python::bases< Vector_O::Bases::Base1, Vector_O::Bases::Base2>, 
	boost::noncopyable > ( "Vector_O", boost::python::no_init )
//	.initArgs("(self)")
	;
#endif
    }
    

}; /* core */