#define	DEBUG_LEVEL_NONE


#include <boost/algorithm/string.hpp>
#include <string>
#include "core/common.h"
#include "core/corePackage.h"
#include "core/pathname.h"
#include "character.h"
#include "symbolTable.h"
#include "numbers.h"
#include "arguments.h"
#include "package.h"
#include "lispDefinitions.h"
#include "hashTableEql.h"
#include "hashTableEq.h"
#include "multipleValues.h"
#include "evaluator.h"
#include "lispStream.h"
#include "str.h"
#include "specialForm.h"
#include "cons.h"
//#include "lisp_ParserExtern.h"
#include "lispReader.h"
#include "readtable.h"
#include "wrappers.h"

namespace core 
{

#define TRAIT_DIGIT			0X000100
#define	TRAIT_ALPHABETIC		0X000200
#define	TRAIT_PACKAGEMARKER		0X000400
#define TRAIT_ALPHADIGIT		0X000800
#define TRAIT_PLUSSIGN			0X001000
#define	TRAIT_MINUSSIGN			0X002000
#define	TRAIT_DOT			0X004000
#define	TRAIT_DECIMALPOINT		0X008000
#define	TRAIT_RATIOMARKER		0X010000
#define	TRAIT_EXPONENTMARKER		0X020000
#define TRAIT_INVALID			0X040000
#define	TRAIT_MASK			0X0FFF00
#define	CHAR_MASK			0X0000FF
#define CHR(x) (static_cast<char>((x)&CHAR_MASK))
#define CHR_MATCH(x,m) (CHR(x)==(m))
#define TRAIT_MATCH_ANY(x,t) (((x)&(t))!=0)
#define CHR_TRAIT_MATCH_ANY(x,t,m) (TRAIT_MATCH_ANY(x,t)&&CHR_MATCH(x,m))
#define TRAIT_MATCH_EQ(x,t)


#if 1
#define TRAP_BAD_CONS(x)
#else
#define TRAP_BAD_CONS(x) {if (af_consP(x)) {LOG(BF("About to try trap bad cons"));string ssss=af_sourceFileInfo(x.as_or_nil<Cons_O>())->fileName();}}
#endif

    /*! Return a uint that combines the character x with its character TRAITs
      See CLHS 2.1.4.2 */
    uint constituentChar(Character_sp ch, uint trait=0)
    {
	brclChar x(ch->asChar());
	if ( trait!=0 ) return (x|trait);
	uint result = 0;
	int readBase = cl::_sym_STARread_baseSTAR->symbolValue().as<Fixnum_O>()->get();
	if ( x >= '0' && x <= '9' ) {
	    uint uix = x-'0';
	    if ( uix < readBase ) {
		result = (TRAIT_DIGIT | x);
		return result;
	    }
	    return (TRAIT_ALPHABETIC | x);
	}
	if ( x >= 'a' && x <= 'z' ) {
	    uint uix = x - 'a' + 10;
	    if ( uix < readBase ) {
		result = (TRAIT_DIGIT | x);
		goto LETTER;
	    }
	    result = (TRAIT_ALPHABETIC | x);
	    goto LETTER;
	}
	if ( x >= 'A' && x <= 'Z' ) {
	    uint uix = x - 'A' + 10;
	    if ( uix < readBase ) {
		result = (TRAIT_DIGIT | x);
		goto LETTER;
	    }
	    result = (TRAIT_ALPHABETIC | x);
	    goto LETTER;
	}
	if ( x == ':' ) return (TRAIT_PACKAGEMARKER | x);
	if ( x == '+' ) return (TRAIT_PLUSSIGN | x);
	if ( x == '-' ) return (TRAIT_MINUSSIGN | x);
	if ( x == '.' ) return (TRAIT_DOT | TRAIT_DECIMALPOINT | x);
	if ( x == '/' ) return (TRAIT_RATIOMARKER | x);
	if ( x > ' ' && x < 127 ) return (TRAIT_ALPHABETIC | x);
	return (TRAIT_INVALID | x);
    LETTER:
	if (    x == 'd' || x == 'D'
	     || x == 'e' || x == 'E'
	     || x == 'f' || x == 'F'
	     || x == 'l' || x == 'L' ) result |= TRAIT_EXPONENTMARKER;
	return result;
    }


#define DOCS_af_nread "nread"
#define LOCK_af_nread 1
#define ARGS_af_nread "(sin &optional (eof-error-p t) eof-value)"
#define DECL_af_nread ""    
    T_mv af_nread(Stream_sp sin, T_sp eof_error_p, T_sp eof_value )
    {_G();
	T_sp result = read_lisp_object(sin,eof_error_p.isTrue(),eof_value,false);
	return(Values(result));
    };




    string fix_exponent_char(const char* cur)
    {_G();
	stringstream ss;
	while (*cur)
	{
	    if ( isalpha(*cur))
	    {
		char uc = toupper(*cur);
		switch (uc)
		{
		case 'D':
		case 'E':
		case 'F':
		case 'L':
		case 'S':
		    ss << "E";
		    break;
		default:
		    SIMPLE_ERROR(BF("Illegal exponent character[%c]") % *cur);
		    break;
		}
	    } else ss << (*cur);
	    ++cur;
	}
	return ss.str();
    }




    typedef enum {
	tstart,
	tsyms, tsymf, tsymr, tsymx, tsymy, tsymdot, tsymz,
	tsymk, tsymkw, tsymbad, tsyme, tsymex, tsymp, tsympv,
	tintt, tintp, 
	tratio,
	tfloat0, tfloate, tfloatp } TokenState;

    typedef enum {
	undefined_exp,
	double_float_exp,
	float_exp,
	single_float_exp,
	long_float_exp,
	short_float_exp } FloatExponentType;


    string tokenStr(const vector<uint>& token, uint start=0, uint end=UNDEF_UINT)
    {
	stringstream ss;
	for ( uint i=start, iEnd(end==UNDEF_UINT ? token.size() : end );
	      i<iEnd; ++i ) {
	    ss << CHR(token[i]);
	}
	return ss.str();
    }

	



    T_sp interpret_token_or_throw_reader_error(Stream_sp sin, const vector<uint>& token )
    {_G();
	ASSERTF(token.size()>0,BF("The token is empty!"));
	const uint* start = token.data();
	const uint* cur = start;
	const uint* end = token.data()+token.size();
	const uint* package_marker = start;
	const uint* name_marker = start;
	TokenState state = tstart;
	FloatExponentType exponent = undefined_exp;
#define EXPTEST(c,x) {							\
	    if (CHR_TRAIT_MATCH_ANY(c,TRAIT_EXPONENTMARKER,'D')||CHR_TRAIT_MATCH_ANY(c,TRAIT_EXPONENTMARKER,'d')) { exponent = double_float_exp; state = x; goto NEXT;} \
	    if (CHR_TRAIT_MATCH_ANY(c,TRAIT_EXPONENTMARKER,'E')||CHR_TRAIT_MATCH_ANY(c,TRAIT_EXPONENTMARKER,'e')) { exponent =        float_exp; state = x; goto NEXT;} \
	    if (CHR_TRAIT_MATCH_ANY(c,TRAIT_EXPONENTMARKER,'F')||CHR_TRAIT_MATCH_ANY(c,TRAIT_EXPONENTMARKER,'f')) { exponent = single_float_exp; state = x; goto NEXT;} \
	    if (CHR_TRAIT_MATCH_ANY(c,TRAIT_EXPONENTMARKER,'L')||CHR_TRAIT_MATCH_ANY(c,TRAIT_EXPONENTMARKER,'l')) { exponent =   long_float_exp; state = x; goto NEXT;} \
	    if (CHR_TRAIT_MATCH_ANY(c,TRAIT_EXPONENTMARKER,'S')||CHR_TRAIT_MATCH_ANY(c,TRAIT_EXPONENTMARKER,'s')) { exponent =  short_float_exp; state = x; goto NEXT;} \
	}
#define	TEST(c,x) if (c) {state =x; goto NEXT;}
#define TEST_CMD(c,cmd,x) if (c) { cmd; state=x; goto NEXT;}
#define ELSE(x) {state=x;goto NEXT;}
	while ( cur != end )
	{
	    switch (state)
	    {
	    case tstart:
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_PLUSSIGN,'+')||CHR_TRAIT_MATCH_ANY(*cur,TRAIT_MINUSSIGN,'-'),tsyms);
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tintt);
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_DOT|TRAIT_DECIMALPOINT,'.'),tsymdot);
		TEST_CMD(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_PACKAGEMARKER,':'),package_marker=cur,tsymk);
		ELSE(tsymz);
	    case tsyms:
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_DOT|TRAIT_DECIMALPOINT,'.'),tsymf);
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tintt);
		ELSE(tsymz);
	    case tsymf:
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tfloat0);
		ELSE(tsymz);
	    case tintt:
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tintt);
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_DOT|TRAIT_DECIMALPOINT,'.'),tintp);
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_RATIOMARKER,'/'),tsymr);
		EXPTEST(*cur,tsymx);
		ELSE(tsymz);
	    case tintp:
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tfloat0);
		ELSE(tsymz);
	    case tfloat0:
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tfloat0);
		EXPTEST(*cur,tsymx);
		ELSE(tsymz);
	    case tsymr:
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tratio);
		ELSE(tsymz);
	    case tratio:
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tratio);
		ELSE(tsymz);
	    case tsymx:
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tfloate);
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_PLUSSIGN,'+')||CHR_TRAIT_MATCH_ANY(*cur,TRAIT_MINUSSIGN,'-'),tsymy);
		ELSE(tsymz);
	    case tsymy:
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tfloate);
		ELSE(tsymz);
	    case tfloate:
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tfloate);
		ELSE(tsymz);
	    case tsymdot:
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tfloatp);
		ELSE(tsymz);
	    case tfloatp:
		TEST(TRAIT_MATCH_ANY(*cur,TRAIT_DIGIT),tfloatp);
		EXPTEST(*cur,tsymx);
		ELSE(tsymz);
	    case tsymz:
		TEST_CMD(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_PACKAGEMARKER,':'),package_marker=cur,tsyme);
		ELSE(tsymz);
	    case tsyme:
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_PACKAGEMARKER,':'),tsymp);
		name_marker = cur;
		ELSE(tsymex);
	    case tsymex:
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_PACKAGEMARKER,':'),tsymbad);
		ELSE(tsymex);
	    case tsymp:
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_PACKAGEMARKER,':'),tsymbad);
		name_marker = cur;
		ELSE(tsympv);
	    case tsympv:
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_PACKAGEMARKER,':'),tsymbad);
		ELSE(tsympv);
	    case tsymk:
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_PACKAGEMARKER,':'),tsymbad);
		name_marker = cur;
		ELSE(tsymkw);
	    case tsymkw:
		TEST(CHR_TRAIT_MATCH_ANY(*cur,TRAIT_PACKAGEMARKER,':'),tsymbad);
		ELSE(tsymkw);
	    case tsymbad:
		ELSE(tsymbad);
	    default:
		SIMPLE_ERROR(BF("unhandled state[%d] for token assignment") % state );
	    }
	NEXT:
	    ++cur;
	}
	switch (state)
	{
	case tstart:
	    SIMPLE_ERROR(BF("There was no token!!!!"));
	case tsymdot:
	    return _sym_dot;
	case tsyms: case tsymf: case tsymr: case tsymx: case tsymy: case tsymz:
	    // interpret symbols in current package
	{
	    if ( cl::_sym_STARread_suppressSTAR->symbolValue().isTrue() ) return _Nil<T_O>();
	    string symbolName = tokenStr(token,name_marker-token.data());
	    return _lisp->getCurrentPackage()->intern(symbolName);
	}
	break;
	case tsymex: case tsympv:
	{
	    if ( cl::_sym_STARread_suppressSTAR->symbolValue().isTrue() ) return _Nil<T_O>();
	    // interpret good package:name symbols
	    // Get package part
	    stringstream packageSin;
	    cur = start;
	    while (cur!=package_marker)
	    {
		packageSin<<CHR(*cur);
		++cur;
	    }
	    string symbolName = tokenStr(token,name_marker-token.data());
	    LOG(BF("Interpreting token as packageName[%s] and symbol-name[%s]")
		% packageSin.str() % symbolName );
	    string packageName = packageSin.str();
	    Package_sp pkg = _lisp->findPackage(packageName);
	    if ( pkg.nilp() )
	    {
		PACKAGE_ERROR(Str_O::create(packageName));
	    }
	    return pkg->intern(symbolName);
	}
	break;
	case tsymkw:
	{
	    if ( cl::_sym_STARread_suppressSTAR->symbolValue().isTrue() ) return _Nil<T_O>();
	    // interpret good keywords
	    LOG(BF("Token[%s] interpreted as keyword") % name_marker );
	    string keywordName = tokenStr(token,name_marker-token.data());
	    return _lisp->keywordPackage()->intern(keywordName);
	}
	break;
	case tsyme: case tsymp:	case tsymk: case tsymbad:
	    if ( cl::_sym_STARread_suppressSTAR->symbolValue().isTrue() ) return _Nil<T_O>();
	    // interpret failed symbols
	    SIMPLE_ERROR(BF("ReaderError_O::create(sin,_lisp))"));
	    break;
	case tintt: case tintp:
	    // interpret ints
	{
	    int read_base = cl::_sym_STARread_baseSTAR->symbolValue().as<Fixnum_O>()->get();
	    string num = tokenStr(token,start-token.data());
	    mpz_class z(num.c_str(),read_base);
	    return Integer_O::create(z);
	}
	break;
	case tratio:
	{
	    // interpret ratio
	    string ratioStr = tokenStr(token,start-token.data());
	    Ratio_sp r = Ratio_O::create(ratioStr.c_str());
	    return r;
	    break;
	}
	case tfloat0: case tfloate: case tfloatp:
	    // interpret float
	{
	    switch (exponent)
	    {
	    case undefined_exp:
	    case float_exp:
	    {
		char* lastValid = NULL;
		string numstr = fix_exponent_char(tokenStr(token,start-token.data()).c_str());
		double d = ::strtod(numstr.c_str(),&lastValid);
		return DoubleFloat_O::create(d);
	    }
	    case short_float_exp:
	    {
		char* lastValid = NULL;
		string numstr = fix_exponent_char(tokenStr(token,start-token.data()).c_str());
		double d = ::strtod(numstr.c_str(),&lastValid);
		return SingleFloat_O::create(d);
	    }
	    case single_float_exp:
	    {
		char* lastValid = NULL;
		string numstr = fix_exponent_char(tokenStr(token,start-token.data()).c_str());
		double d = ::strtod(numstr.c_str(),&lastValid);
		return SingleFloat_O::create(d);
	    }
	    case double_float_exp:
	    {
		char* lastValid = NULL;
		string numstr = fix_exponent_char(tokenStr(token,start-token.data()).c_str());
		double d = ::strtod(numstr.c_str(),&lastValid);
		return DoubleFloat_O::create(d);
	    }
	    case long_float_exp:
	    {
		char* lastValid = NULL;
		string numstr = fix_exponent_char(tokenStr(token,start-token.data()).c_str());
#ifdef BRCL_LONG_FLOAT
		LongFloat d = ::strtold(numstr.c_str(),&lastValid);
		return LongFloat_O::create(d);
#else
		double d = ::strtod(numstr.c_str(),&lastValid);
		return DoubleFloat_O::create(d);
#endif
	    }
	    }
	    SIMPLE_ERROR(BF("Shouldn't get here - unhandled exponent type"));
	}
	}
	LOG(BF("Bad token[%s]") % tok );
	SIMPLE_ERROR(BF("create<ReaderError_O>(sin));"));
    }


#if 0
    (defun read-list (char &optional (stream *standard-input*)
		      &key allow-consing-dot)
     (let ((*consing-dot-allowed* allow-consing-dot)
	   c stack values list)
      (loop
       (setq c (peek-char t stream t nil t))
       (when (char= char c)           ; found the closing parenthesis.           
	(when (eq (first stack) *consing-dot*)
	 (error "Nothing appears after . in list."))
	(read-char stream t nil t)
	(setq list (if (eq (second stack) *consing-dot*)
                        (nreconc (cddr stack) (first stack))
			    (nreverse stack)))
	(return))
       (when (setq values (multiple-value-list (lisp-object? stream t nil t)))
	(if (eq (second stack) *consing-dot*)
	    (error "More than one object follows . in list.")
		(push (car values) stack))))
      list))
#endif



    Cons_sp read_list(Stream_sp sin, char end_char, bool allow_consing_dot)
    {_G();
	uint start_lineNumber=UNDEF_UINT;
	uint start_column=UNDEF_UINT;
	uint start_filePos = UNDEF_UINT;
	bool got_dotted = false;
	T_sp dotted_object = _Nil<T_O>();
	Cons_sp first = Cons_O::create(_Nil<T_O>(),_Nil<Cons_O>());
	Cons_sp cur = first;
	start_lineNumber = sin->lineNumber();
	start_column = sin->column();
	start_filePos = sin->tell();
	while (1)
	{
	    Character_sp cp = af_peekChar(_lisp->_true(),sin,_lisp->_true(),_Nil<Character_O>(),_lisp->_true()).as<Character_O>();
	    LOG(BF("read_list ---> peeked char[%s]") % _rep_(cp) );
	    if ( cp->asChar() == end_char )
	    {
		af_readChar(sin,_lisp->_true(),_Nil<T_O>(),_lisp->_true());
		if ( dotted_object.notnilp() )
		{
		    cur->setOCdr(dotted_object);
		    Cons_sp result = cCdr(first);
		    TRAP_BAD_CONS(result);
		    if ( result.nilp() ) return(Values(_Nil<Cons_O>()));
		    return(Values(result));
		}
		Cons_sp otherResult = cCdr(first);
		TRAP_BAD_CONS(otherResult);
		if ( otherResult.nilp() ) return(Values(_Nil<Cons_O>()));
		_lisp->sourceManager()->registerSourceInfo(otherResult,
							   sin->sourceFileInfo(),
							   start_lineNumber,
							   start_column,
							   start_filePos);
		return(otherResult);
	    }
	    int ivalues;
	    T_sp obj;
	    T_mv mv = lisp_object_query(sin,true,_Nil<T_O>(),true);
	    ivalues = mv.number_of_values();
	    if ( ivalues > 0 )
	    {
		obj = mv;
		LOG(BF("lisp_object_query: %s") % _rep_(obj) );
		// Could get ending lineNumber and column here
		if ( obj == _sym_dot )
		{
		    if ( allow_consing_dot )
		    {	
			got_dotted = true;
			Character_sp cdotp = af_peekChar(_lisp->_true(),sin,_lisp->_true(),_Nil<Character_O>(),_lisp->_true()).as<Character_O>();
			if ( cdotp->asChar() == end_char )
			{
			    SIMPLE_ERROR(BF("Nothing after consing dot"));
			}
			dotted_object = read_lisp_object(sin,true,_Nil<T_O>(),true);
		    } else
		    {
			SIMPLE_ERROR(BF("Illegal consing dot"));
		    }
		} else
		{
		    if ( got_dotted )
		    {
			SIMPLE_ERROR(BF("More than one object after consing dot"));
		    }
		    Cons_sp one = Cons_O::create(obj,_Nil<Cons_O>());
		    _lisp->sourceManager()->registerSourceInfo(one,af_sourceFileInfo(sin),start_lineNumber,start_column,start_filePos);
		    LOG(BF("One = %s\n") % _rep_(one) );
		    LOG(BF("one->sourceFileInfo()=%s") % _rep_(af_sourceFileInfo(one)) );
		    LOG(BF("one->sourceFileInfo()->fileName()=%s") % af_sourceFileInfo(one)->fileName());
		    LOG(BF("one->sourceFileInfo()->fileName().c_str() = %s") % af_sourceFileInfo(one)->fileName().c_str());
		    TRAP_BAD_CONS(one);
		    cur->setCdr(one);
		    cur = one;
		}
	    }
	}
    }


    
    /*! Used when USE_SHARP_EQUAL_HASH_TABLES is not defined */
    SYMBOL_SC_(CorePkg,STARsharp_equal_alistSTAR);
    SYMBOL_SC_(CorePkg,STARsharp_sharp_alistSTAR);
    /*! Used when USE_SHARP_EQUAL_HASH_TABLES is on */
    SYMBOL_SC_(CorePkg,STARsharp_equal_final_tableSTAR);
    SYMBOL_SC_(CorePkg,STARsharp_equal_temp_tableSTAR);
    SYMBOL_SC_(CorePkg,STARsharp_equal_repl_tableSTAR);

    T_sp read_lisp_object(Stream_sp sin, bool eofErrorP, T_sp eofValue, bool recursiveP)
    {_G();
	T_sp result = _Nil<T_O>();
	if ( recursiveP )
	{
	    while (1)
	    {
		T_mv mv = lisp_object_query(sin,eofErrorP,eofValue,recursiveP);
		int ivalues = mv.number_of_values();
		if ( ivalues> 0 )
		{
		    result = mv;
		    if (cl::_sym_STARread_suppressSTAR->symbolValue().isTrue() )
		    {
			LOG(BF("read_suppress == true"));
			result = _Nil<T_O>();
			break;
		    }
		    break;
		}
	    }
	} else
	{
#ifndef USE_SHARP_EQUAL_HASH_TABLES
	    DynamicScopeManager scope(_sym_STARsharp_equal_alistSTAR,_Nil<Cons_O>());
#else
	    DynamicScopeManager scope(_sym_STARsharp_equal_final_tableSTAR,HashTableEql_O::create(40,Fixnum_O::create(4000),0.8));
	    scope.pushSpecialVariableAndSet(_sym_STARsharp_equal_temp_tableSTAR,HashTableEql_O::create(40,Fixnum_O::create(4000),0.8));
	    scope.pushSpecialVariableAndSet(_sym_STARsharp_equal_repl_tableSTAR,HashTableEq_O::create(40,Fixnum_O::create(4000),0.8));
#endif
	    result = read_lisp_object(sin,eofErrorP,eofValue,true);
	}
	if ( result.nilp() ) return(Values(_Nil<T_O>()));
	return(result);
    }
	    

	    



    /*!
      Read a character from the stream and based on what it is continue to process the
      stream until a complete symbol/number of macro is processed.
      Return the result in a MultipleValues object - if it is empty then nothing was read */
    T_mv lisp_object_query(Stream_sp sin, bool eofErrorP, T_sp eofValue, bool recursiveP)
    {_G();
#if 1	
	static int monitorReaderStep = 0;
	if ( (monitorReaderStep % 1000 ) == 0 && af_member(_sym_monitorReader,_sym_STARdebugMonitorSTAR->symbolValue(),_Nil<T_O>()).notnilp()) {
	    printf("%s:%d:%s stream %s -> pos = %lld\n",__FILE__,__LINE__,__FUNCTION__, _rep_(sin->pathname()).c_str(),sin->tell());
	}
	++monitorReaderStep;
#endif
	string chars;
	vector<uint> token;
	ReadTable_sp readTable = _lisp->getCurrentReadTable();
	Character_sp x, y, z, X, Y, Z;
	/* See the CLHS 2.2 Reader Algorithm  - continue has the effect of jumping to step 1 */
    step1:
	LOG(BF("step1"));
	x = af_readChar(sin,_Nil<T_O>(),_Unbound<Character_O>(),_lisp->_true()).as<Character_O>();
	if ( x.unboundp() )
	{
	    if ( eofErrorP ) STREAM_ERROR(sin);
	    return Values(eofValue);
	}
	LOG(BF("Reading character x[%s]") % x->asChar() );
	Symbol_sp x1_syntax_type = readTable->syntax_type(x);
//    step2:
	if ( x1_syntax_type == kw::_sym_invalid_character )
	{
	    LOG(BF("step2 - invalid-character[%c]") % x->asChar() );
	    SIMPLE_ERROR(BF("ReaderError_O::create(sin,_lisp)"));
	}
//    step3:
	if ( x1_syntax_type == kw::_sym_whitespace_character)
	{
	    LOG(BF("step3 - whitespace character[%c/%d]") % x->asChar() % x->asChar() );
	    goto step1;
	}
//    step4:
	if ( (x1_syntax_type == kw::_sym_terminating_macro_character)
	     || (x1_syntax_type == kw::_sym_non_terminating_macro_character ))
	{_BLOCK_TRACEF(BF("Processing macro character x[%s]") % x->asChar() );
	    LOG(BF("step4 - terminating-macro-character or non-terminating-macro-character char[%c]") % x->asChar());
	    T_sp reader_macro;
	    {MULTIPLE_VALUES_CONTEXT();
		reader_macro = readTable->get_macro_character(x);
	    }
	    ASSERT(reader_macro.notnilp());
	    return eval::funcall(reader_macro,sin,x);
	}
//    step5:
	if ( x1_syntax_type == kw::_sym_single_escape_character)
	{
	    LOG(BF("step5 - single-escape-character char[%c]") % x->asChar());
	    LOG(BF("Handling single escape"));
	    y = af_readChar(sin,_lisp->_true(),_Nil<T_O>(),_lisp->_true()).as<Character_O>();
	    token.clear();
	    token.push_back(constituentChar(y,TRAIT_ALPHABETIC));
	    LOG(BF("Read y[%s]") % y->asChar() );
	    goto step8;
	}
//    step6:
	if ( x1_syntax_type == kw::_sym_multiple_escape_character )
	{
	    LOG(BF("step6 - multiple-escape-character char[%c]") % x->asChar());
	    LOG(BF("Handling multiple escape - clearing token"));
	    token.clear();
	    goto step9;
	}
//    step7:
	if ( readTable->syntax_type(x) == kw::_sym_constituent_character )
	{
	    LOG(BF("step7 - constituent-character char[%c]") % x->asChar());
	    LOG(BF("Handling constituent character"));
	    token.clear();
	    X = readTable->convert_case(x);
	    LOG(BF("Converted case X[%s]") % X->asChar());
	    token.push_back(constituentChar(X));
	}
    step8:
	LOG(BF("step8"));
	{
	    y = af_readChar(sin,BRCL_NIL,BRCL_NIL,BRCL_T).as<Character_O>();
	    if ( y.nilp() )
	    {
		LOG(BF("Hit eof"));
		goto step10;
	    }
	    LOG(BF("Step8: Read y[%s]") % y->asChar() );
	    Symbol_sp y8_syntax_type = readTable->syntax_type(y);
	    LOG(BF("y8_syntax_type=%s") % _rep_(y8_syntax_type) );
	    if ( (y8_syntax_type == kw::_sym_constituent_character)
		 || (y8_syntax_type == kw::_sym_non_terminating_macro_character) )
	    {
		Y = readTable->convert_case(y);
		token.push_back(constituentChar(Y));
		goto step8;
	    }
	    if ( y8_syntax_type == kw::_sym_single_escape_character)
	    {
		z = af_readChar(sin,BRCL_T,BRCL_NIL,BRCL_T).as<Character_O>();
		token.push_back(constituentChar(z,TRAIT_ALPHABETIC));
		LOG(BF("Single escape read z[%s] accumulated token[%s]")
		    % z->asChar() % tokenStr(token) );
		goto step8;
	    }
	    if ( y8_syntax_type == kw::_sym_multiple_escape_character) goto step9;
	    if ( y8_syntax_type == kw::_sym_invalid_character) SIMPLE_ERROR(BF("ReaderError_O::create()"));
	    if ( y8_syntax_type == kw::_sym_terminating_macro_character )
	    {
		LOG(BF("UNREADING char y[%s]") % y->asChar() );
		sin->unread_char(y->asChar());
		goto step10;
	    }
	    if ( y8_syntax_type == kw::_sym_whitespace_character )
	    {
		LOG(BF("y is whitespace"));
		if (_sym_STARpreserve_whitespace_pSTAR->symbolValue().isTrue() )
		{
		    LOG(BF("unreading y[%s]") % y->asChar() );
		    sin->unread_char(y->asChar());
		}
		goto step10;
	    }
	}
    step9:
	LOG(BF("step9"));
	{
	    y = af_readChar(sin,BRCL_T,BRCL_NIL,BRCL_T).as<Character_O>();
	    Symbol_sp y9_syntax_type = readTable->syntax_type(y);
	    LOG(BF("Step9: Read y[%s] y9_syntax_type[%s]") % y->asChar() % _rep_(y9_syntax_type) );
	    if ( (y9_syntax_type == kw::_sym_constituent_character)
		 || (y9_syntax_type == kw::_sym_non_terminating_macro_character)
		 || (y9_syntax_type == kw::_sym_terminating_macro_character)
		 || (y9_syntax_type == kw::_sym_whitespace_character) )
	    {
		token.push_back(constituentChar(y,TRAIT_ALPHABETIC));
		LOG(BF("token[%s]") % tokenStr(token));
		goto step9;
	    }
	    LOG(BF("About to test y9_syntax_type[%s] single_escape[%s] are equal? ==> %d") % _rep_(y9_syntax_type) % _rep_(kw::_sym_single_escape_character)
		% (y9_syntax_type == kw::_sym_single_escape_character) );
	    if ( y9_syntax_type == kw::_sym_single_escape_character )
	    {
		LOG(BF("Handling single_escape_character"));
		z = af_readChar(sin,BRCL_NIL,_Unbound<Character_O>(),BRCL_T).as<Character_O>();
		if ( z.unboundp() ) {
		    STREAM_ERROR(sin);
		}
		token.push_back(constituentChar(z,TRAIT_ALPHABETIC));
		LOG(BF("Read z[%s] accumulated token[%s]") % z->asChar() % tokenStr(token));
		goto step9;
	    }
	    if ( y9_syntax_type == kw::_sym_multiple_escape_character )
	    {
		LOG(BF("Handling multiple_escape_character"));
		goto step8;
	    }
	    if ( y9_syntax_type == kw::_sym_invalid_character )
	    {
		SIMPLE_ERROR(BF("ReaderError_O::create()"));
	    }
	    SIMPLE_ERROR(BF("Should never get here"));
	}
    step10:
	LOG(BF("step10"));
	// At this point convert the string in tokenSin.str() into an object
	// Lets just return it as an annotated string
	// Throw a ReaderError if the token is not valid syntax
	if ( cl::_sym_STARread_suppressSTAR->symbolValue().isTrue() ) return(Values(_Nil<T_O>()));
	T_sp object = interpret_token_or_throw_reader_error(sin,token);
	if ( object == cl::_sym_nil || object.nilp() ) object = _Nil<T_O>();
	TRAP_BAD_CONS(object);
	return(Values(object));
    }

















    void exposeCore_lisp_reader()
    {_G();
	SYMBOL_SC_(CorePkg,nread);
	Defun(nread);
	
	// functions for reader

    }





};



