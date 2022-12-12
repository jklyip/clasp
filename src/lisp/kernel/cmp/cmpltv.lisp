(defpackage #:cmpltv
  (:use #:cl)
  (:export #:with-constants #:ensure-constant #:add-constant
           #:write-bytecode))

(in-package #:cmpltv)

;;; For this first version, I'm going to track permanency but not do anything
;;; with it - cutting out transients can be later, since I think it will need
;;; more coordination with the compiler.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Initial processing level: Reflects only the necessary recursion
;;; structure, not necessarily the eventual underlying representation.
;;; We collect a sequence of specialized "instructions" that, when executed,
;;; will create and initialize the LTV table.

;;; Next available index for a constant.
(defvar *next-index*)

(defclass instruction () ())
;;; An instruction that allocates or otherwise creates an object.
;;; The object may be fully initialized or may require further initialization.
(defclass creator (instruction)
  (;; T if the object outlasts loading (e.g. is referred to directly in code)
   ;; otherwise NIL
   (%permanency :initform nil :accessor permanency :type boolean)
   (%index :initform (prog1 *next-index* (incf *next-index*))
           :accessor index :type (integer 0))))
;;; A creator for which a prototype value (which the eventual LTV will be
;;; similar to) is available.
(defclass vcreator (creator)
  ((%prototype :initarg :prototype :reader prototype)))
;;; An instruction that performs some action for effect. This can include
;;; initialization as well as arbitrary side effects (as from make-load-form).
(defclass effect (instruction) ())

(defun permanentize (creator) (setf (permanency creator) t) creator)

;;;

;;; TODO: Abbreviate with list/dotted list, but make sure
;;; coalescence is still really possible.
(defclass cons-creator (vcreator) ())

(defclass rplaca-init (effect)
  ((%cons :initarg :cons :reader rplac-cons :type cons-creator)
   (%value :initarg :value :reader rplac-value :type creator)))

(defclass rplacd-init (effect)
  ((%cons :initarg :cons :reader rplac-cons :type cons-creator)
   (%value :initarg :value :reader rplac-value :type creator)))

;;; Abstract. element-type and dimensions are grabbed from the prototype
;;; since they shouldn't really need to be coalesced.
(defclass array-creator (vcreator) ())
;;; T arrays
(defclass general-array-creator (array-creator) ())

;; row-major.
(defclass setf-aref (effect)
  ((%array :initarg :array :reader setf-aref-array :type general-array-creator)
   (%index :initarg :index :reader setf-aref-index :type (integer 0))
   (%value :initarg :value :reader setf-aref-value :type creator)))

;;; Sub-T arrays. Elements are gotten from the value itself.
;;; (Coalescing specialized array elements is probably pointless.)
(defclass specialized-array-creator (array-creator) ())

(defclass hash-table-creator (vcreator) ())

(defclass setf-gethash (effect)
  ((%hash-table :initarg :hash-table :reader setf-gethash-hash-table
                :type hash-table-creator)
   (%key :initarg :key :reader setf-gethash-key :type creator)
   (%value :initarg :value :reader setf-gethash-value :type creator)))

(defclass symbol-creator (vcreator)
  (;; Is there actually a point to trying to coalesce symbol names?
   (%name :initarg :name :reader symbol-creator-name :type creator)
   (%package :initarg :package :reader symbol-creator-package :type creator)))

(defclass package-creator (vcreator)
  (;; Is there actually a point to trying to coalesce package names?
   ;; Also, some symbols (CL, KEYWORD) could probably be dumped without
   ;; a general package reference.
   (%name :initarg :name :reader package-creator-name :type creator)))

(defclass number-creator (vcreator) ())
(defclass sb64-creator (number-creator) ())
(defclass bignum-creator (number-creator) ())
(defclass ratio-creator (number-creator) ())
;; TODO: ratio, float, complex
;; float presents some issues if this is supposed to be a truly portable
;; form. Probably we should use the IEEE formats (binary16, 32, etc.) but
;; it is not technically guaranteed that Lisp implementations use these,
;; and encoding/decoding them portably is a little weird.
;; Maybe should just use float-features.

(defclass character-creator (vcreator) ())

(defclass general-creator (vcreator)
  (;; Reference to a function to call to allocate the object, i.e. a
   ;; function made of the first return value from make-load-form.
   ;; The function is called with no arguments and returns the new value.
   (%function :initarg :function :reader general-creator-function
              :type creator)))

(defclass general-initializer (effect)
  (;; Reference to a function to call to initialize the object, i.e. a
   ;; function made of the second return value from make-load-form.
   ;; The function is called with no arguments and its value is ignored.
   (%function :initarg :function :reader general-initializer-function
              :type creator)))

(defclass singleton-creator (vcreator) ())

(defclass load-time-value-creator (creator)
  (;; Reference to a function to call to evaluate the load form.
   ;; It's called with no arguments and returns the value.
   (%function :initarg :function :reader load-time-value-creator-function
              :type creator)
   ;; Boolean indicating whether the LTV is read-only. Unused for now.
   (%read-only-p :initarg :read-only-p :type boolean
                 :reader load-time-value-creator-read-only-p)
   ;; The original form, for debugging/display
   (%form :initarg :form :reader load-time-value-creator-form)
   ;; If something's referenced directly from load-time-value, it's permanent.
   (%permanency :initform t)))

;;;

;; Look up a value in the instructions.
;; On success returns the creator, otherwise NIL.
;; Could be extended with coalescence relations or made more efficient.
(defun %find-constant (value sequence)
  (find-if (lambda (c)
             (and (typep c 'vcreator)
                  (eql (prototype c) value)))
           sequence))

;;; List of instructions to be executed by the loader.
;;; In reverse.
(defvar *instructions*)

;;; Bound by the client to a function that compiles a lambda expression
;;; relative to an environment, and then returns some object that
;;; cmpltv can treat as a constant.
(defvar *compiler*)

(defmacro with-constants ((&key ((:compiler *compiler*))) &body body)
  (declare (ignore options))
  `(let ((*instructions* nil)
         (*next-index* 0))
     ,@body))

(defun find-constant (value)
  (%find-constant value *instructions*))

(defun add-instruction (instruction)
  (push instruction *instructions*)
  instruction)

(defgeneric add-constant (value))

(defun ensure-constant (value permanentp)
  (let ((creator (or (find-constant value) (add-constant value))))
    (when permanentp (permanentize creator))
    creator))

;;; Given a form, get a constant handle to a function that at load time will
;;; have the effect of evaluating the form in a null lexical environment.
(defun add-form (form)
  ;; PROGN so that (declare ...) expressions for example correctly cause errors.
  (add-constant (funcall *compiler* `(lambda () (progn ,form)) nil)))

(defmethod add-constant ((value cons))
  (let ((cons (add-instruction
               (make-instance 'cons-creator :prototype value))))
    (add-instruction (make-instance 'rplaca-init
                       :cons cons :value (ensure-constant (car value) nil)))
    (add-instruction (make-instance 'rplacd-init
                       :cons cons :value (ensure-constant (cdr value) nil)))
    cons))

(defmethod add-constant ((value array))
  (let ((uaet (array-element-type value)))
    (if (subtypep uaet '(or number character))
        (add-instruction (make-instance 'specialized-array-creator
                           :prototype value))
        (let ((arr (add-instruction
                    (make-instance 'general-array-creator :prototype value))))
          (loop for i below (array-total-size value)
                do (add-instruction
                    (make-instance 'setf-aref
                      :array arr :index i
                      :value (ensure-constant (row-major-aref value i) nil))))
          arr))))

(defmethod add-constant ((value hash-table))
  (let ((ht (add-instruction
             (make-instance 'hash-table-creator :prototype value))))
    (maphash (lambda (k v)
               (add-instruction
                (make-instance 'setf-gethash
                  :hash-table ht
                  :key (ensure-constant k nil) :value (ensure-constant v nil))))
             value)
    ht))

(defmethod add-constant ((value symbol))
  (add-instruction (make-instance 'symbol-creator
                     :prototype value
                     :name (ensure-constant (symbol-name value) nil)
                     :package (ensure-constant (symbol-package value) nil))))

(defmethod add-constant ((value (eql nil)))
  (add-instruction (make-instance 'singleton-creator :prototype value)))
(defmethod add-constant ((value (eql t)))
  (add-instruction (make-instance 'singleton-creator :prototype value)))

(defmethod add-constant ((value package))
  (add-instruction (make-instance 'package-creator
                     :prototype value
                     :name (ensure-constant (package-name value) nil))))

(defmethod add-constant ((value integer))
  (add-instruction
   (etypecase value
     ;; TODO? Could have different opcodes for smaller integers.
     ((signed-byte 64) (make-instance 'sb64-creator :prototype value))
     (integer (make-instance 'bignum-creator :prototype value)))))

(defmethod add-constant ((value character))
  (add-instruction (make-instance 'character-creator :prototype value)))

(defmethod add-constant ((value t))
  (multiple-value-bind (create initialize) (make-load-form value)
    (let ((creator (add-form create)) (initializer (add-form initialize)))
      (prog1 (add-instruction (make-instance 'general-creator
                                :prototype value :function creator))
        (add-instruction (make-instance 'general-initializer
                           :function initializer))))))

(defun add-load-time-value (form read-only-p)
  (add-instruction (make-instance 'load-time-value-creator
                     :function (add-form form)
                     :form form :read-only-p read-only-p)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Bytecode
;;;
;;; "bytecode" is actually a little strong. This "bytecode" consists of a
;;; sequence of "instructions" that must be executed sequentially.
;;; There's no other control flow. There is no data structure involved other
;;; than the array of constants being produced (so e.g. no operand stack).
;;; All multibyte values are big-endian. All indices are one byte, or two
;;; bytes, or etc. powers of two based on how many constants there are. E.g. if
;;; there are 200 constants indices will be one byte, but if there are 300
;;; indices will be two bytes.
;;; Instruction set is copied from Clasp for now. "sind" in the below means an
;;; index that the allocated object will be stored into. This may need some
;;; review later.
;;; Operations are as follows:
(defparameter +ops+
  '((nil 65 sind)
    (t 66 sind)
    (ratio 67) ; TODO
    (complex 68) ; TODO
    (cons 69 sind)
    (rplaca 70 ind1 ind2) ; (setf (car [ind1]) [ind2])
    (rplacd 71 ind1 ind2)
    (make-array 74 sind rank . dims)
    ((setf row-major-aref) 75 arrayind rmindex valueind)
    (make-hash-table 76 sind test count)
    ((setf gethash) 77 htind keyind valueind)
    (make-sb64 78 sind sb64)
    (find-package 79 sind nameind)
    (make-bignum 80 sind size . words) ; size is signed
    (intern 82 sind packageind nameind) ; make-symbol
    (make-character 83 sind ub32) ; ub64 in clasp, i think?
    (make-pathname 85) ; TODO
    (funcall-create 93 sind fnind)
    (funcall-initialize 94 fnind)
    ;; set-ltv-funcall in clasp- redundant
    (make-specialized-array 97 sind rank dims etype . elems)))

;;; STREAM is a ub8 stream.
(defgeneric encode (instruction stream))

;; how many bytes are needed to represent a position?
(defvar *position-bytes*)

;;; Write an n-byte integer to a ub8 stream, big-endian.
(defun write-b (int n stream)
  ;; write-sequence is better for this, but I don't think we can really
  ;; use it without consing or touching memory generally.
  (loop for i from (* (1- n) 8) downto 0 by 8
        for byte = (ldb (byte 8 i) int)
        do (write-byte byte stream)))

(defun write-b64 (word stream) (write-b word 8 stream))
(defun write-b32 (word stream) (write-b word 4 stream))
(defun write-b16 (word stream) (write-b word 2 stream))

(defconstant +magic+ #x8d7498b1) ; randomly chosen bytes.

(defun write-magic (stream) (write-b32 +magic+ stream))

(defparameter *major-version* 0)
(defparameter *minor-version* 0)

(defun write-version (stream)
  (write-b16 *major-version* stream)
  (write-b16 *minor-version* stream))

(defun %write-bytecode (instructions stream)
  (let* (;; lol efficiency
         (insts (nreverse instructions))
         (nobjs (count-if (lambda (i) (typep i 'creator)) insts))
         ;; Next highest power of two bytes, roughly
         (*position-bytes* (ash 1 (1- (ceiling (integer-length nobjs) 8)))))
    (write-magic stream)
    (write-version stream)
    (write-b64 nobjs stream)
    (map nil (lambda (inst) (encode inst stream)) insts)))

(defun write-bytecode (stream)
  (%write-bytecode *instructions* stream))

(defun opcode (mnemonic)
  (let ((inst (assoc mnemonic +ops+ :test #'equal)))
    (if inst
        (second inst)
        (error "unknown mnemonic ~a" mnemonic))))

(defun write-mnemonic (mnemonic stream) (write-byte (opcode mnemonic) stream))

(defun write-index (creator stream)
  (let ((position (index creator)))
    (ecase *position-bytes*
      ((1) (write-byte position stream))
      ((2) (write-b16 position stream))
      ((4) (write-b32 position stream))
      ((8) (write-b64 position stream)))))

(defmethod encode ((inst cons-creator) stream)
  (write-mnemonic 'cons stream)
  (write-index inst stream))

(defmethod encode ((inst rplaca-init) stream)
  (write-mnemonic 'rplaca stream)
  (write-index (rplac-cons inst) stream)
  (write-index (rplac-value inst) stream))

(defmethod encode ((inst rplacd-init) stream)
  (write-mnemonic 'rplacd stream)
  (write-index (rplac-cons inst) stream)
  (write-index (rplac-value inst) stream))

(defun write-dimensions (dimensions stream)
  (let ((rank (length dimensions)))
    (unless (< rank 256)
      (error "Can't dump an array of ~d dimensions" rank))
    (write-byte rank stream))
  ;; Only two bytes for now. Might want different opcodes for larger
  ;; (or smaller?) dimensions.
  (unless (< (reduce #'* dimensions) 65536)
    (error "Can't dump an array with ~d elements" (reduce #'* dimensions)))
  (dolist (dim dimensions)
    (write-b16 dim stream)))

(defmethod encode ((inst general-array-creator) stream)
  (write-mnemonic 'make-array stream)
  (write-index inst stream)
  (let* ((arr (prototype inst))
         (dims (array-dimensions arr)))
    (write-dimensions dims stream)))

(defmethod encode ((inst setf-aref) stream)
  (write-mnemonic '(setf row-major-aref) stream)
  (write-index (setf-aref-array inst) stream)
  (write-b16 (setf-aref-index inst) stream)
  (write-index (setf-aref-value inst) stream))

(defvar +uaet-codes+
  '((t         #b00000000)
    (base-char #b10000000)
    (character #b11000000)))

(defun uaet-code (uaet)
  (or (second (assoc uaet +uaet-codes+))
      (error "Unknown UAET ~a" uaet)))

(defmethod encode ((inst specialized-array-creator) stream)
  (write-mnemonic 'make-specialized-array stream)
  (write-index inst stream)
  (let* ((arr (prototype inst))
         (uaet (array-element-type arr)))
    (write-dimensions (array-dimensions arr) stream)
    (write-byte (uaet-code uaet) stream)
    (ecase uaet
      (base-char
       (dotimes (i (array-total-size arr))
         (write-byte (char-code (row-major-aref arr i)) stream)))
      (character
       (dotimes (i (array-total-size arr))
         ;; FIXME: UTF-8 would be more compact
         (write-b32 (char-code (row-major-aref arr i)) stream))))))

(defmethod encode ((inst hash-table-creator) stream)
  (let* ((ht (prototype inst))
         ;; TODO: Custom hash-table tests.
         ;; NOTE that for non-custom hash table tests, the standard
         ;; guarantees that hash-table-test returns a symbol.
         (testcode (ecase (hash-table-test ht)
                     ((eq) #b00)
                     ((eql) #b01)
                     ((equal) #b10)
                     ((equalp) #b11)))
         ;; For now, only allow counts up to #xffff.
         ;; Since the count is just a hint, bigger hash tables can still
         ;; be dumped okay.
         ;; efficiency NOTE: The size passed to make-hash-table really
         ;; specifies a capacity, so for example if we have an HT with 56
         ;; entries, make a 56-entry similar hash table, and start filling it
         ;; up, it might be rehashed and resized during initialization as it
         ;; reaches the rehash threshold. I am not sure how to deal with this
         ;; in a portable fashion. (we could just invert a provided rehash-size?)
         (count (max (hash-table-count ht) #xffff)))
    (write-mnemonic 'make-hash-table stream)
    (write-byte testcode stream)
    (write-b16 count stream)))

(defmethod encode ((inst setf-gethash) stream)
  (write-mnemonic '(setf gethash) stream)
  (write-index (setf-gethash-hash-table inst) stream)
  (write-index (setf-gethash-key inst) stream)
  (write-index (setf-gethash-value inst) stream))

(defmethod encode ((inst singleton-creator) stream)
  (ecase (prototype inst)
    ((nil) (write-mnemonic 'nil stream))
    ((t) (write-mnemonic 't stream)))
  (write-index inst stream))

(defmethod encode ((inst symbol-creator) stream)
  (write-mnemonic 'intern stream)
  (write-index inst stream)
  (write-index (symbol-creator-package inst) stream)
  (write-index (symbol-creator-name inst) stream))

(defmethod encode ((inst package-creator) stream)
  (write-mnemonic 'find-package stream)
  (write-index inst stream)
  (write-index (package-creator-name inst) stream))

(defmethod encode ((inst character-creator) stream)
  (write-mnemonic 'make-character stream)
  (write-index inst stream)
  (write-b32 (char-code (prototype inst)) stream))

(defmethod encode ((inst sb64-creator) stream)
  (write-mnemonic 'make-sb64 stream)
  (write-index inst stream)
  (write-b64 (prototype inst) stream))

(defmethod encode ((inst bignum-creator) stream)
  ;; uses sign-magnitude representation.
  (write-mnemonic 'make-bignum stream)
  (write-index inst stream)
  (let* ((number (prototype inst))
         (anumber (abs number))
         (nwords (ceiling (integer-length anumber) 64))
         (negp (minusp number)))
    (write-b64 (if negp (- nwords) nwords) stream)
    (loop for i from nwords above 0
          for pos = (* (1- i) 64)
          for word = (ldb (byte 64 pos) anumber)
          do (write-b64 word stream))))

(defmethod encode ((inst general-creator) stream)
  (write-mnemonic 'funcall-create stream)
  (write-index inst stream)
  (write-index (general-creator-function inst) stream))

(defmethod encode ((inst general-initializer) stream)
  (write-mnemonic 'funcall-initialize stream)
  (write-index (general-initializer-function inst) stream))
