( LET ( ( ::landing-pad-block4997 ( COMPILER::IRC-BASIC-BLOCK-CREATE "try.landing-pad" ) )
       ( ::cont-block4998 ( COMPILER::IRC-BASIC-BLOCK-CREATE "try-cont" ) ) )
  ( SETF-METADATA COMPILER::TAGBODY-ENV :NORMAL-CLAUSE-TYPES ( QUOTE (COMPILER::TYPEID-CORE-GO  ) ) )
  ( COMPILER::WITH-UNWIND-DEST ( ::landing-pad-block4997 ::cont-block4998 )
    ( MAPL ( FUNCTION ( LAMBDA ( COMPILER::CUR )
			( LET* ( ( COMPILER::TAG-BEGIN ( CAR COMPILER::CUR ) )
				( COMPILER::TAG-END ( CADR COMPILER::CUR ) )
				 ( COMPILER::SECTION-BLOCK ( CADR COMPILER::TAG-BEGIN ) )
				 ( COMPILER::SECTION-NEXT-BLOCK ( CADR COMPILER::TAG-END ) )
				 ( COMPILER::SECTION ( COMPILER::EXTRACT-SECTION ( CADDR COMPILER::TAG-BEGIN ) ( CADDR COMPILER::TAG-END ) ) ) )
			  ( COMPILER::IRC-BRANCH-IF-NO-TERMINATOR-INST COMPILER::SECTION-BLOCK )
			  ( COMPILER::IRC-BEGIN-BLOCK COMPILER::SECTION-BLOCK )
			  ( COMPILER::CODEGEN-PROGN COMPILER::RESULT COMPILER::SECTION COMPILER::TAGBODY-ENV )
			  ( WHEN COMPILER::SECTION-NEXT-BLOCK ( COMPILER::IRC-BRANCH-IF-NO-TERMINATOR-INST COMPILER::SECTION-NEXT-BLOCK ) ) )
			) )
	   COMPILER::ENUMERATED-TAG-BLOCKS )
    )
  ( PROGN
    ( COMPILER::IRC-BRANCH-TO-AND-BEGIN-BLOCK ( COMPILER::IRC-BASIC-BLOCK-CREATE "try.normal-cleanup" ) )
    ( COMPILER::CODEGEN-NIL COMPILER::RESULT COMPILER::ENV ) )
  ( COMPILER::IRC-BRANCH-IF-NO-TERMINATOR-INST ::cont-block4998 )
  ( COMPILER::IRC-BEGIN-BLOCK ::landing-pad-block4997 )
  ( LET* ( ( ::clause-types5001 ( COMPILER::TRY.IDENTIFY-ALL-UNIQUE-CLAUSE-TYPES COMPILER::TAGBODY-ENV ) )
	  ( ::landingpad5000 ( COMPILER::IRC-CREATE-LANDING-PAD ( LENGTH ::clause-types5001 ) "" ) ) )
    ( COMPILER::TRY.ADD-LANDING-PAD-CLAUSES ::landingpad5000 ::clause-types5001 )
    ( COMPILER::IRC-SET-CLEANUP ::landingpad5000 T )
    ( MULTIPLE-VALUE-BIND ( ::exn.slot5003 ::ehselector.slot5002 )
	( COMPILER::IRC-PRESERVE-EXCEPTION-INFO COMPILER::TAGBODY-ENV ::landingpad5000 )
      ( PROGN ( COMPILER::IRC-BRANCH-TO-AND-BEGIN-BLOCK ( COMPILER::IRC-BASIC-BLOCK-CREATE "try.eh-cleanup" ) ) ( COMPILER::CODEGEN-NIL COMPILER::RESULT COMPILER::ENV ) )
      ( LET ((::dispatch-TYPEID-CORE-GO-5014 ( COMPILER::IRC-BASIC-BLOCK-CREATE "dispatch-TYPEID-CORE-GO-5014" )  ))
	( SETF-METADATA COMPILER::TAGBODY-ENV :EXCEPTION-HANDLER-RESUME-BLOCK ::dispatch-TYPEID-CORE-GO-5014 )
	( COMPILER::IRC-BR ::dispatch-TYPEID-CORE-GO-5014 )
	( MAPLIST ( FUNCTION ( LAMBDA ( X Y )
			       ( COMPILER::TRY.ONE-DISPATCHER-AND-HANDLER ( CAR X )
									  ( IF ( CADR X )
									       ( CADR X )
									       ( BACKQUOTE-APPEND
										 ( LIST ( QUOTE LOOKUP-METADATA ) )
										 ( LIST ( BACKQUOTE-APPEND ( LIST ( QUOTE GET-PARENT-ENVIRONMENT ) ) ( LIST COMPILER::ENV ) ( QUOTE nil ) ) )
										 ( LIST ( QUOTE :EXCEPTION-HANDLER-RESUME-BLOCK ) )
										 ( QUOTE nil ) ) )
									  ( CAR Y )
									  COMPILER::SUCCESSFUL-CATCH-BLOCK COMPILER::EXN.SLOT COMPILER::EHSELECTOR.SLOT COMPILER::ENV ) ) )
		  (::dispatch-TYPEID-CORE-GO-5014  )
		  (( ( COMPILER::TYPEID-CORE-GO COMPILER::EXCEPTION-PTR )
		     ( LET* ( ( COMPILER::GO-INDEX ( COMPILER::IRC-CALL COMPILER::ENV "tagbodyGoIndexElseRethrow" COMPILER::EXCEPTION-PTR ) )
			     ( COMPILER::DEFAULT-BLOCK ( COMPILER::IRC-BASIC-BLOCK-CREATE "switch-default" ) )
			      ( COMPILER::SW ( COMPILER::IRC-SWITCH COMPILER::GO-INDEX COMPILER::DEFAULT-BLOCK ( LENGTH COMPILER::ENUMERATED-TAG-BLOCKS ) ) )
			      )
		       ( MAPC ( FUNCTION ( LAMBDA ( COMPILER::ONE ) ( LLVM-SYS:ADD-CASE COMPILER::SW ( COMPILER::JIT-CONSTANT-I32 ( CAR COMPILER::ONE ) ) ( CADR COMPILER::ONE ) ) ) )
			      COMPILER::ENUMERATED-TAG-BLOCKS )
		       ( COMPILER::IRC-BEGIN-BLOCK COMPILER::DEFAULT-BLOCK )
		       ( COMPILER::IRC-CALL COMPILER::TAGBODY-ENV "throwIllegalSwitchValue" COMPILER::GO-INDEX
					    ( COMPILER::JIT-CONSTANT-I32 ( LENGTH COMPILER::ENUMERATED-TAG-BLOCKS ) ) ) ) )  ) ) ) )
    ( COMPILER::IRC-BRANCH-IF-NO-TERMINATOR-INST ::cont-block4998 )
    ( COMPILER::IRC-BEGIN-BLOCK ::cont-block4998 ) ) )