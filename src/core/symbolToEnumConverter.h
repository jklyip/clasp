#ifndef	SymbolToEnumConverter_H //[
#define SymbolToEnumConverter_H



#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include "foundation.h"
#include "object.h"


namespace core {

SMART(Symbol);

SMART(SymbolToEnumConverter );
class SymbolToEnumConverter_O : public T_O
{
    LISP_BASE1(T_O);
    LISP_CLASS(core,CorePkg,SymbolToEnumConverter_O,"SymbolToEnumConverter");
public:
    static SymbolToEnumConverter_sp create(const string& whatDoesEnumRepresent);
    static SymbolToEnumConverter_sp create(const string& whatDoesEnumRepresent, string const& packageName, NullTerminatedEnumAssociation  assoc[]);
public: // virtual functions inherited from Object
	void	initialize();

private: // instance variables
		/*! Store what the the enumIndex values represent
		 * Used when errors are thrown
		 */
    string		_WhatTheEnumsRepresent;
    HashTableEql_sp     _EnumToSymbol;
    HashTableEq_sp  	_ArchiveSymbolToEnum;
    HashTableEql_sp     _EnumToArchiveSymbol;
    HashTableEq_sp	_SymbolToEnum;

public:
	void	setWhatTheEnumsRepresent(const string& represent);

	/*!Associate the symbol with the enum
	  If the symbol is nil then create a new one with the archiveString and return it */
	Symbol_sp addSymbolEnumPair(Symbol_cp sym, Symbol_cp archiveSymbol, int enumIndex);
	int	enumIndexForSymbol(Symbol_sp sym);
		/*! Return the enum associated with the symbol
		 */
	template <typename T>
	    T enumForSymbol(Symbol_sp sym)
	    {
		int index = this->enumIndexForSymbol(sym);
		return ((T)(index));
	    };


	Symbol_sp symbolForEnumIndex(int index);
	template <typename EnumType>
	    Symbol_sp symbolForEnum(EnumType ev)
	    {
		return symbolForEnumIndex((int)(ev));
	    }

		/*! Dump all legal enum values and symbols
		 */
	string legalEnumValuesAndSymbols();
	void throwIfUnrecognizedEnumIndex(int ei);
	template <typename EnumType>
	    void throwIfUnrecognizedEnum(EnumType ei)
	    {
		this->throwIfUnrecognizedEnumIndex((int)(ei));
	    };
	bool recognizesEnumIndex(int ei);
	bool recognizesSymbol(Symbol_sp ei);
	template <typename EnumType>
	    bool recognizesEnum(EnumType val)
	    {
		return this->recognizesEnumIndex((int)(val));
	    }

    string __repr__() const;


	DEFAULT_CTOR_DTOR(SymbolToEnumConverter_O);


};


};
TRANSLATE(core::SymbolToEnumConverter_O);



#define ENUM_TRANSLATOR(enumType,converterSymbol) \
namespace translate {	\
    template <> struct to_object<enumType>				\
    {									\
	typedef	enumType	GivenType;				\
	static core::T_sp convert(const GivenType& val)			\
	{_G();								\
	    core::SymbolToEnumConverter_sp converter =			\
		converterSymbol->symbolValue().as<core::SymbolToEnumConverter_O>(); \
	    return converter->symbolForEnum(val);		\
	}								\
    };									\
									\
    template <>								\
    struct from_object<enumType,std::true_type>                         \
    {									\
	typedef	enumType 	DeclareType;				\
	DeclareType _v;							\
	from_object(T_P o) {                                            \
	    core::SymbolToEnumConverter_sp converter = converterSymbol->symbolValue().as<core::SymbolToEnumConverter_O>(); \
	    _v = converter->enumForSymbol<enumType>(o.as<core::Symbol_O>()); \
	}								\
    };									\
									\
									\
};	



#define DECLARE_SYMBOL_TO_ENUM_CONVERTER(assocArray,description,package,converterSymbol) { \
	core::SymbolToEnumConverter_sp enumConverter =			\
	    core::SymbolToEnumConverter_O::create(description,package,assocArray); \
	(converterSymbol)->defparameter(enumConverter);			\
    }






#endif //]