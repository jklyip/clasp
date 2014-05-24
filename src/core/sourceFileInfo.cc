#define DEBUG_LEVEL_FULL

#include <string.h>
#include "core/foundation.h"
#include "core/fileSystem.h"
#include "core/str.h"
#include "core/lispStream.h"
#include "core/hashTableEq.h"
#include "core/pathname.h"
#include "sourceFileInfo.h"
#include "bundle.h"
#include "wrappers.h"

#define REQUIRE_SOURCE_INFO 0
extern "C" {
    // For debugging the sourceManager
    void dumpSourceInfo(core::T_sp exp) 
    {
	string repExp = _rep_(exp);
	printf("Object: %s\n", repExp.c_str() );
	core::SourceFileInfo_mv sfi = _lisp->sourceManager()->lookupSourceInfo(exp);
	if ( sfi.number_of_values() >= 4 ) {
	    core::SourceFileInfo_sp sf = sfi;
	    core::Fixnum_sp lineno  = sfi.valueGet(1).as<core::Fixnum_O>();
	    core::Fixnum_sp column  = sfi.valueGet(2).as<core::Fixnum_O>();
	    core::Fixnum_sp filePos = sfi.valueGet(3).as<core::Fixnum_O>();
	    printf("     Source file: %s   lineno: %d  column: %d   filePos %d\n",
		   _rep_(sf).c_str(), lineno->get(), column->get(), filePos->get());
	} else {
	    printf("     No source file info found\n");
	}
	if ( core::af_consP(exp) ) {
	    dumpSourceInfo(oCar(exp));
	    dumpSourceInfo(oCdr(exp));
	}
    }
};




namespace core
{


    
#define ARGS_af_sourceFileInfo "(arg)"
#define DECL_af_sourceFileInfo ""
#define DOCS_af_sourceFileInfo "sourceFileInfo"
    SourceFileInfo_sp af_sourceFileInfo(T_sp obj)
    {_G();
	SourceFileInfo_sp result = _Nil<SourceFileInfo_O>();
	if ( obj.nilp() ) {
	    // do nothing
	} else if ( Cons_sp co = obj.asOrNull<Cons_O>() )
	{
	    return _lisp->sourceManager()->lookupSourceInfo(co);
	} else if ( Stream_sp so = obj.asOrNull<Stream_O>() )
	{
	    result = so->sourceFileInfo();
	} else if ( Function_sp fo = obj.asOrNull<Function_O>() )
	{
	    result = fo->sourceFileInfo();
	} else
	{
	    SIMPLE_ERROR(BF("Implement sourceFileInfo for %s") % _rep_(obj));
	}
	if ( result.nilp() )
	{
	    return SourceFileInfo_O::getOrCreate("Anonymous");
	}
	return result;
    };


    
#define ARGS_af_lineNumber "(arg)"
#define DECL_af_lineNumber ""
#define DOCS_af_lineNumber "lineNumber"
    uint af_lineNumber(T_sp obj)
    {_G();
	if ( obj.nilp() )
	{
	    return 0;
	} else if ( Cons_sp co = obj.asOrNull<Cons_O>() )
	{
	    return co->lineNumber();
	} else if ( Stream_sp so = obj.asOrNull<Stream_O>() )
	{
	    return so->lineNumber();
	} else if ( Function_sp fo = obj.asOrNull<Function_O>() )
	{
	    return fo->lineNumber();
	}
	SIMPLE_ERROR(BF("Implement lineNumber for %s") % _rep_(obj));
    };


#define ARGS_af_column "(arg)"
#define DECL_af_column ""
#define DOCS_af_column "column"
    uint af_column(T_sp obj)
    {_G();
	if ( obj.nilp() )
	{
	    return 0;
	} else if ( Cons_sp co = obj.asOrNull<Cons_O>() )
	{
	    return co->column();
	} else if ( Stream_sp so = obj.asOrNull<Stream_O>() )
	{
	    return so->column();
	} else if ( Function_sp fo = obj.asOrNull<Function_O>() )
	{
	    return fo->column();
	}
	SIMPLE_ERROR(BF("Implement column for %s") % _rep_(obj));
    };

    



    
    
#define ARGS_af_walkToFindSourceInfo "(arg)"
#define DECL_af_walkToFindSourceInfo ""
#define DOCS_af_walkToFindSourceInfo "walkToFindSourceInfo"
    SourceFileInfo_mv af_walkToFindSourceInfo(T_sp obj)
    {_G();
	if ( af_consP(obj) ) {
	    SourceFileInfo_mv sfi = _lisp->sourceManager()->lookupSourceInfo(obj);
	    if ( sfi.notnilp() ) {
		return sfi;
	    }
	    Cons_sp cur = obj.as<Cons_O>();
	    for ( ; cur.notnilp(); cur=cCdr(cur) ) {
		SourceFileInfo_mv sfisub = af_walkToFindSourceInfo(oCar(cur));
		if ( sfisub.notnilp() ) return sfisub;
	    }
	}
	return Values(_Nil<SourceFileInfo_O>());
    };



    
    
#define ARGS_af_SourceFileInfoGetOrCreate "(arg)"
#define DECL_af_SourceFileInfoGetOrCreate ""
#define DOCS_af_SourceFileInfoGetOrCreate "SourceFileInfoGetOrCreate"
    T_sp af_SourceFileInfoGetOrCreate(T_sp arg)
    {_G();
	if ( Str_sp sarg = arg.asOrNull<Str_O>() )
	{
	    return SourceFileInfo_O::getOrCreate(sarg->get());
	} else if ( Pathname_sp parg = arg.asOrNull<Pathname_O>() )
	{
	    return SourceFileInfo_O::getOrCreate(parg);
	}
	SIMPLE_ERROR(BF("Illegal argument for source-file-info-get-or-create"));
    }

    SourceFileInfo_O::SourceFileInfo_O() : Base(), _PermanentPathName(NULL), _PermanentFileName(NULL) {};

    void SourceFileInfo_O::initialize()
    {_G();
	this->Base::initialize();
    }

    SourceFileInfo_sp SourceFileInfo_O::getOrCreate(const string& str)
    {_G();
	SourceFileInfo_sp spx = _lisp->getSourceFileInfo(str);
	if ( spx.notnilp() ) return spx;
        GC_ALLOCATE(SourceFileInfo_O,spi );
	spi->_pathname = af_pathname(Str_O::create(str));
	_lisp->setSourceFileInfo(str,spi);
	return spi;
    }


    SourceFileInfo_sp SourceFileInfo_O::getOrCreate(Pathname_sp path)
    {_G();
	Str_sp s = af_namestring(path);
	return SourceFileInfo_O::getOrCreate(s->get());
    }


    string SourceFileInfo_O::__repr__() const
    {_G();
	stringstream ss;
	ss << "#<" << this->_instanceClass()->classNameAsString() << " " << _rep_(this->_pathname) << ">";
	return ss.str();
    }



    string SourceFileInfo_O::fileName() const
    {
	Str_sp s = af_fileNamestring(this->_pathname);
	return s->get();
    }


    string SourceFileInfo_O::namestring() const
    {
	Str_sp s = af_namestring(this->_pathname);
	return s->get();
    }


    string SourceFileInfo_O::parentPathName() const
    {
	Str_sp s = af_directoryNamestring(this->_pathname);
	return s->get();
    }


    const char* SourceFileInfo_O::permanentPathName()
    {
	if ( this->_PermanentPathName == NULL )
	{
	    string fn = this->namestring();
	    this->_PermanentPathName = (char*)malloc(fn.size()+1);
	    ::strcpy(this->_PermanentPathName,fn.c_str());
	}
	return this->_PermanentPathName;
    }


    const char* SourceFileInfo_O::permanentFileName()
    {
	if ( this->_PermanentFileName == NULL )
	{
	    string fn = this->fileName();
	    this->_PermanentFileName = (char*)malloc(fn.size()+1);
	    ::strcpy(this->_PermanentFileName,fn.c_str());
	}
	return this->_PermanentFileName;
    }


    EXPOSE_CLASS(core,SourceFileInfo_O);

    void SourceFileInfo_O::exposeCando(core::Lisp_sp lisp)
    {
	core::class_<SourceFileInfo_O>()
	    .def("source-file-info-pathname",&SourceFileInfo_O::pathname)
	    ;
	SYMBOL_SC_(CorePkg,SourceFileInfoGetOrCreate);
	Defun(SourceFileInfoGetOrCreate);
	SYMBOL_EXPORT_SC_(CorePkg,walkToFindSourceInfo);
	Defun(walkToFindSourceInfo);
    }

    void SourceFileInfo_O::exposePython(core::Lisp_sp lisp)
    {_G();
#ifdef USEBOOSTPYTHON
	PYTHON_CLASS(CorePkg,SourceFileInfo,"","",_lisp)
	    ;
#endif
    }







    EXPOSE_CLASS(core,SourcePosInfo_O);

    void SourcePosInfo_O::exposeCando(core::Lisp_sp lisp)
    {
	core::class_<SourcePosInfo_O>()
	    ;
    }


    void SourcePosInfo_O::exposePython(core::Lisp_sp lisp)
    {_G();
#ifdef USEBOOSTPYTHON
	PYTHON_CLASS(CorePkg,SourcePosInfo,"","",_lisp)
	    ;
#endif
    }






    
#define ARGS_af_lookupSourceFileInfo "(obj)"
#define DECL_af_lookupSourceFileInfo ""
#define DOCS_af_lookupSourceFileInfo "lookupSourceFileInfo"
    SourceFileInfo_mv af_lookupSourceFileInfo(T_sp obj)
    {_G();
	return _lisp->sourceManager()->lookupSourceInfo(obj);
    };



    
    
#define ARGS_af_dumpSourceManager "()"
#define DECL_af_dumpSourceManager ""
#define DOCS_af_dumpSourceManager "dumpSourceManager"
    void af_dumpSourceManager()
    {_G();
	_lisp->print(BF("Source Manager entries: %d\n") % _lisp->sourceManager()->_SourcePosInfo->size());
	for ( int i=0, iEnd(_lisp->sourceManager()->_Files.size()); i<iEnd; ++i ) {
	    _lisp->print(BF("   File[%d] --> %s\n") % i % _lisp->sourceManager()->_Files[i]->namestring());
	}
    };


    EXPOSE_CLASS(core,SourceManager_O);

    void SourceManager_O::exposeCando(core::Lisp_sp lisp)
    {
	core::class_<SourceManager_O>()
	    ;

	SYMBOL_EXPORT_SC_(CorePkg,lookupSourceFileInfo);
	Defun(lookupSourceFileInfo);
	Defun(dumpSourceManager);
    }


    void SourceManager_O::exposePython(core::Lisp_sp lisp)
    {_G();
#ifdef USEBOOSTPYTHON
	PYTHON_CLASS(CorePkg,SourceManager,"","",_lisp)
	    ;
#endif
    }


    void SourceManager_O::initialize()
    {
        this->Base::initialize();
        this->_SourcePosInfo = HashTableEq_O::create_default();
    }


    void SourceManager_O::registerSourceInfo(T_sp key,
					     SourceFileInfo_sp sourceFile,
					     uint lineno,
					     uint column,
					     uint filePos,
					     Function_sp expander)
    {_G();
        SourcePosInfo_sp sfi = this->_SourcePosInfo->gethash(sourceFile,_Nil<SourcePosInfo_O>()).as<SourcePosInfo_O>();
	uint fileId;
	if ( sfi.nilp() ) {
	    fileId = this->_Files.size();
	    this->_Files.push_back(sourceFile);
	    SourcePosInfo_sp finfo = SourcePosInfo_O::create(fileId,UNDEF_UINT,UNDEF_UINT,UNDEF_UINT,expander);
	    this->_SourcePosInfo->setf_gethash(sourceFile, finfo);
	} else {
	    fileId = sfi->_FileId;
	}
	SourcePosInfo_sp info = SourcePosInfo_O::create(fileId,lineno,column,filePos);
        this->_SourcePosInfo->setf_gethash(key,info);
//	this->_SourcePosInfo[key] = info;
    }



    void SourceManager_O::registerSourceInfoFromStream(T_sp obj, Stream_sp stream)
    {_G();
	SourceFileInfo_sp sfi  = stream->sourceFileInfo();
	uint lineNumber = stream->lineNumber();
	uint column = stream->column();
	uint filePos = stream->tell();
	this->registerSourceInfo(obj,sfi,lineNumber,column,filePos);
    }

    bool SourceManager_O::searchForSourceInfoAndDuplicateIt(T_sp orig_obj, T_sp new_obj)
    {
	SourceFileInfo_mv info = _lisp->sourceManager()->lookupSourceInfo(orig_obj);
	if ( info.number_of_values() >= 4) {
	    SourceFileInfo_sp sfi = info;
	    uint lineno = info.valueGet(1).as<Fixnum_O>()->get();
	    uint column = info.valueGet(2).as<Fixnum_O>()->get();
	    uint filePos = info.valueGet(3).as<Fixnum_O>()->get();
	    this->registerSourceInfo(new_obj,sfi,lineno,column,filePos);
	    return true;
	} else {
	    if ( af_consP(orig_obj) ) {
		Cons_sp orig_cons = orig_obj.as<Cons_O>();
		for ( ; orig_cons.notnilp(); orig_cons=cCdr(orig_cons) ) {
		    if ( this->searchForSourceInfoAndDuplicateIt(oCar(orig_cons),new_obj) ) return true;
		}
	    }
	}
	return false;
    }
	


    void SourceManager_O::duplicateSourceInfo(T_sp orig_obj, T_sp new_obj)
    {_G();
	SourceFileInfo_mv info = _lisp->sourceManager()->lookupSourceInfo(orig_obj);
	if ( info.number_of_values() >= 4) {
	    SourceFileInfo_sp sfi = info;
	    uint lineno = info.valueGet(1).as<Fixnum_O>()->get();
	    uint column = info.valueGet(2).as<Fixnum_O>()->get();
	    uint filePos = info.valueGet(3).as<Fixnum_O>()->get();
	    this->registerSourceInfo(new_obj,sfi,lineno,column,filePos);
	} else {
	    this->searchForSourceInfoAndDuplicateIt(orig_obj, new_obj);
#if 0
#if REQUIRE_SOURCE_INFO
	    printf("Dumping source info for orig_obj\n");
	    dumpSourceInfo(orig_obj);
	    IMPLEMENT_MEF(BF("There was no source info to duplicate"));
#endif
#endif
	}
    }

    void SourceManager_O::duplicateSourceInfoForMacroExpansion(T_sp orig_obj, Function_sp expander, T_sp new_obj)
    {_G();
	SourceFileInfo_mv info = _lisp->sourceManager()->lookupSourceInfo(orig_obj);
	if ( info.number_of_values() >= 4) {
	    SourceFileInfo_sp sfi = info;
	    uint lineno = info.valueGet(1).as<Fixnum_O>()->get();
	    uint column = info.valueGet(2).as<Fixnum_O>()->get();
	    uint filePos = info.valueGet(3).as<Fixnum_O>()->get();
	    this->registerSourceInfo(new_obj,sfi,lineno,column,filePos,expander);
	} else {
	    this->searchForSourceInfoAndDuplicateIt(orig_obj, new_obj);
#if 0
#if REQUIRE_SOURCE_INFO
	    printf("Dumping source info for orig_obj\n");
	    dumpSourceInfo(orig_obj);
	    IMPLEMENT_MEF(BF("There was no source info to duplicate"));
#endif
#endif
	}
    }


    SourceFileInfo_mv SourceManager_O::lookupSourceInfo(T_sp key)
    {
        SourcePosInfo_sp it = this->_SourcePosInfo->gethash(key,_Nil<SourcePosInfo_O>()).as<SourcePosInfo_O>();
	if (it.notnilp()) {
	    SourceFileInfo_sp sfi = this->_Files[it->_FileId];
	    return Values(sfi,Fixnum_O::create(it->_LineNumber),
			  Fixnum_O::create(it->_Column),
			  Fixnum_O::create(it->_FilePos));
	}
	return Values(_Nil<SourceFileInfo_O>());
    }




};