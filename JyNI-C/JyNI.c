/*
 * Copyright of JyNI:
 * Copyright (c) 2013 Stefan Richthofer.  All rights reserved.
 *
 *
 * Copyright of Python and Jython:
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010,
 * 2011, 2012, 2013 Python Software Foundation.  All rights reserved.
 *
 *
 * This file is part of JyNI.
 *
 * JyNI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * JyNI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with JyNI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */


/*
 * JyNI.c
 *
 *  Created on: 14.03.2013, 00:49:46
 *	  Author: Stefan Richthofer
 */
//PyObject_MALLOC
#include <JyNI.h>
#include <JySync.h>
//#include <JyNI_JyNI.h>
//#include "frameobject.h"
#include "importdl.h"
//#include <dlfcn.h>
//_Py_CheckInterval
const char* excPrefix = "exceptions.";

TypeMapEntry builtinTypes[builtinTypeCount];

#define builtinExceptionCount 50
PyTypeObject* builtinExceptions[builtinExceptionCount];

/*
 * Class:	 JyNI_JyNI
 * Method:	loadModule
 * Signature: (Ljava/lang/String;Ljava/lang/String;)LJyNI/JyNIModule;
 */
//JNIEXPORT jobject JNICALL Java_JyNI_JyNI_loadModule(JNIEnv *env, jclass class, jstring moduleName, jstring modulePath)
jobject JyNI_loadModule(JNIEnv *env, jclass class, jstring moduleName, jstring modulePath)
{
//	puts("64 bit check:");
//	printf("%i\n", sizeof(int));
	//printf("%i\n", SIZEOF_SIZE_T);
	//printf("%i\n", SIZEOF_INT);

	jdbg puts("we will later load the following module at this place:");
	const char* utf_string;

	jboolean isCopy;

	utf_string = (*env)->GetStringUTFChars(env, moduleName, &isCopy);
	//"+1" for 0-termination:
	char mName[strlen(utf_string)+1];
	strcpy(mName, utf_string);
	(*env)->ReleaseStringUTFChars(env, moduleName, utf_string);
	utf_string = (*env)->GetStringUTFChars(env, modulePath, &isCopy);
	//"+1" for 0-termination:
	char mPath[strlen(utf_string)+1];
	strcpy(mPath, utf_string);
	(*env)->ReleaseStringUTFChars(env, moduleName, utf_string);
	//PyExc_ImportError
	jdbg puts("Module:");
	jdbg puts(mName);
	jdbg printf("%i\n", strlen(mName));
	jdbg puts("");
	jdbg puts("Path:");
	jdbg puts(mPath);
	jdbg printf("%i\n", strlen(mPath));
	FILE *fp;
	fp = fopen(mPath, "r" PY_STDIOTEXTMODE);
	if (fp == NULL)
		//PyErr_SetFromErrno(PyExc_IOError);
		puts("some error happened opening the file");
	jdbg puts("start loading");
	//PyObject* er = _PyImport_LoadDynamicModule(mName, mPath, fp);
	jobject er = _PyImport_LoadDynamicModuleJy(mName, mPath, fp);
	jdbg puts("loading done");
	if (fclose(fp))
		puts("Some error occured on file close");
	return er;
}

/*
 * Class:	 JyNI_JyNI
 * Method:	clearPyCPeer
 * Signature: (JJ)V
 */
void JyNI_clearPyCPeer(JNIEnv *env, jclass class, jlong objectHandle, jlong refHandle)
{
	if ((PyObject*) objectHandle != NULL) Py_DECREF((PyObject*) objectHandle);
	if ((jweak) refHandle != NULL) (*env)->DeleteWeakGlobalRef(env, (jweak) refHandle);
	else {
		JyObject* jy = AS_JY((PyObject*) objectHandle);
		(*env)->DeleteWeakGlobalRef(env, (jweak) jy->jy);
	}
	//maybe check for ref-count == 0 here and free non-gc objects...
}

/*
 * Class:     JyNI_JyNI
 * Method:    callPyCPeer
 * Signature: (JLorg/python/core/PyObject;Lorg/python/core/PyObject;)Lorg/python/core/PyObject;
 */
jobject JyNI_callPyCPeer(JNIEnv *env, jclass class, jlong peerHandle, jobject args, jobject kw)
{
	//puts("JyNI_callPyCPeer called");
	//note: here should be done sync
	//(maybe sync-idea is obsolete anyway)
	PyObject* peer = (PyObject*) peerHandle;
	//printf("handle Address: %u\n", peerHandle);
	//puts(peer->ob_type->tp_name);
//	PyObject* j1 = JyNI_PyObject_FromJythonPyObject(args);
//	PyObject* j2 = JyNI_PyObject_FromJythonPyObject(kw);
//	PyObject* er = peer->ob_type->tp_call(peer, j1, j2);
//	//puts("xxx");
//	return NULL;
	return JyNI_JythonPyObject_FromPyObject(
			peer->ob_type->tp_call(peer,
			JyNI_PyObject_FromJythonPyObject(args),
			JyNI_PyObject_FromJythonPyObject(kw)
		));
			//JyNI_PyObject_FromJythonPyObject(args),
			//JyNI_PyObject_FromJythonPyObject(kw));
	//return NULL;
}

/*
 * Class:     JyNI_JyNI
 * Method:    getAttrString
 * Signature: (JLjava/lang/String;)Lorg/python/core/PyObject;
 */
jobject JyNI_getAttrString(JNIEnv *env, jclass class, jlong handle, jstring name)
{
	//PyString_InternFromString
	if (handle == 0) return NULL;
	//puts("JyNI_getAttrString");
	cstr_from_jstring(cName, name);
//	puts("get:");
//	puts(cName);
	PyObject* attr = PyObject_GetAttrString((PyObject*) handle, cName);
//	puts("attr obtained");
//	puts(PyString_AS_STRING(attr));
	return JyNI_JythonPyObject_FromPyObject(attr);
}

/*
 * Class:     JyNI_JyNI
 * Method:    setAttrString
 * Signature: (JLjava/lang/String;Lorg/python/core/PyObject;)I
 */
jint JyNI_setAttrString(JNIEnv *env, jclass class, jlong handle, jstring name, jobject value)
{
	if (handle == 0) return NULL;
	cstr_from_jstring(cName, name);
	return PyObject_SetAttrString((PyObject*) handle, cName, JyNI_PyObject_FromJythonPyObject(value));
}

/*
 * Class:     JyNI_JyNI
 * Method:    repr
 * Signature: (J)Lorg/python/core/PyObject;
 */
jobject JyNI_repr(JNIEnv *env, jclass class, jlong handle)
{
	return JyNI_JythonPyObject_FromPyObject(PyObject_Repr((PyObject*) handle));
}

/*
 * Class:     JyNI_JyNI
 * Method:    PyObjectAsString
 * Signature: (J)Ljava/lang/String;
 */
jstring JyNI_PyObjectAsString(JNIEnv *env, jclass class, jlong handle)
{
	return JyNI_jstring_FromPyStringObject(env, (PyStringObject*) PyObject_Str((PyObject*) handle));
}

/*
 * Class:     JyNI_JyNI
 * Method:    PyObjectAsPyString
 * Signature: (J)Lorg/python/core/PyString;
 */
jobject JyNI_PyObjectAsPyString(JNIEnv *env, jclass class, jlong handle)
{
	return JyNI_JythonPyObject_FromPyObject(PyObject_Str((PyObject*) handle));
}

//PySys_GetObject creates new object
/*Builtin types not relevant for Jython (?):
EncodingMapType,
wrappertype,
_PyExc_BaseException, _PyExc_, _PyExc_UnicodeEncodeError,
_PyExc_UnicodeDecodeError, _PyExc_UnicodeTranslateError
cmpwrapper_type,
sortwrapper_type,
Long_InfoType,
FloatInfoType,
PyCapsule_Type, PyCObject_Type,
_PyWeakref_RefType,
_PyWeakref_ProxyType,
_PyWeakref_CallableProxyType,
_struct_sequence_template
PyGetSetDescr_Type;
PyMemberDescr_Type;
PyCFunction_Type;
PyWrapperDescr_Type;
PyDictIterKey_Type;
PyDictIterValue_Type;
PyDictIterItem_Type;
PyDictKeys_Type;
PyDictItems_Type;
PyDictValues_Type;
PyReversed_Type;
(PyTypeObject*) PyExc_;
(PyTypeObject*) PyExc_UnicodeEncodeError;
(PyTypeObject*) PyExc_UnicodeDecodeError;
(PyTypeObject*) PyExc_UnicodeTranslateError;

//maybe we can later support this via PyArray:
PyByteArrayIter_Type;
*/

inline void initBuiltinTypes()
{
	int i;
	for (i = 0; i < builtinTypeCount; ++i)
	{
		builtinTypes[i].py_type = NULL;
		builtinTypes[i].jy_class = NULL;
		builtinTypes[i].flags = 0;
		builtinTypes[i].sync = NULL;
	}

	builtinTypes[0].py_type = &PyType_Type;							builtinTypes[0].jy_class = pyTypeClass;				builtinTypes[0].flags = 0;
	builtinTypes[1].py_type = &PyNotImplemented_Type; //(_Py_NotImplementedStruct.ob_type);
																	builtinTypes[1].jy_class = pyNotImplementedClass;	builtinTypes[1].flags = 0;
	builtinTypes[2].py_type = &PyNone_Type;//(_Py_NoneStruct.ob_type);
																	builtinTypes[2].jy_class = pyNoneClass;				builtinTypes[2].flags = 0;
	builtinTypes[3].py_type = &PyFile_Type;							builtinTypes[3].jy_class = pyFileClass;				builtinTypes[3].flags = JY_TRUNCATE_FLAG_MASK;
	builtinTypes[4].py_type = &PyModule_Type;						builtinTypes[4].jy_class = pyModuleClass;			builtinTypes[4].flags = JY_TRUNCATE_FLAG_MASK;
	/*builtinTypes[5].py_type = &PyCell_Type;							builtinTypes[5].jy_class = pyCellClass;				builtinTypes[5].flags = 0;*/
	builtinTypes[6].py_type = &PyClass_Type;						builtinTypes[6].jy_class = pyClassClass;			builtinTypes[6].flags = 0;
	/*builtinTypes[7].py_type = &PyInstance_Type;						builtinTypes[7].jy_class = pyInstanceClass;		builtinTypes[7].flags = 0;
	builtinTypes[8].py_type = &PyMethod_Type;						builtinTypes[8].jy_class = pyMethodClass;			builtinTypes[8].flags = 0;
	builtinTypes[9].py_type = &PyFunction_Type;						builtinTypes[9].jy_class = pyFunctionClass;			builtinTypes[9].flags = 0;
	builtinTypes[10].py_type = &PyClassMethod_Type;					builtinTypes[10].jy_class = pyClassMethodClass;		builtinTypes[10].flags = 0;
	builtinTypes[11].py_type = &PyStaticMethod_Type;				builtinTypes[11].jy_class = pyStaticMethodClass;	builtinTypes[11].flags = 0;
	builtinTypes[12].py_type = &PyMethodDescr_Type;					builtinTypes[12].jy_class = pyMethodDescrClass;		builtinTypes[12].flags = 0;
	builtinTypes[13].py_type = &PyClassMethodDescr_Type;			builtinTypes[13].jy_class = pyClassMethodDescrClass;builtinTypes[13].flags = 0;
	builtinTypes[14].py_type = &PyDictProxy_Type;					builtinTypes[14].jy_class = pyDictProxyClass;		builtinTypes[14].flags = 0;
	builtinTypes[15].py_type = &PyProperty_Type;					builtinTypes[15].jy_class = pyPropertyClass;		builtinTypes[15].flags = 0;
	builtinTypes[16].py_type = &PyBool_Type;						builtinTypes[16].jy_class = pyBooleanClass;			builtinTypes[16].flags = 0;*/
	builtinTypes[17].py_type = &PyFloat_Type;						builtinTypes[17].jy_class = pyFloatClass;			builtinTypes[17].flags = JySYNC_ON_INIT_FLAGS;
	builtinTypes[17].sync = malloc(sizeof(SyncFunctions));
	builtinTypes[17].sync->jyInit = (jyInitSync) JySync_Init_JyFloat_From_PyFloat;
	builtinTypes[17].sync->pyInit = (pyInitSync) JySync_Init_PyFloat_From_JyFloat;
	builtinTypes[18].py_type = &PyInt_Type;							builtinTypes[18].jy_class = pyIntClass;				builtinTypes[18].flags = JySYNC_ON_INIT_FLAGS;
	builtinTypes[18].sync = malloc(sizeof(SyncFunctions));
	builtinTypes[18].sync->jyInit = (jyInitSync) JySync_Init_JyInt_From_PyInt;
	builtinTypes[18].sync->pyInit = (pyInitSync) JySync_Init_PyInt_From_JyInt;
	//for computational efficiency we mirror PyLong, although it could also be wrappered
	//later we are going to offer both options by a configuration-parameter in JyNI.
	builtinTypes[19].py_type = &PyLong_Type;						builtinTypes[19].jy_class = pyLongClass;			builtinTypes[19].flags = JySYNC_ON_INIT_FLAGS;
	builtinTypes[19].sync = malloc(sizeof(SyncFunctions));
	builtinTypes[19].sync->jyInit = (jyInitSync) JySync_Init_JyLong_From_PyLong;
	builtinTypes[19].sync->pyInit = (pyInitSync) JySync_Init_PyLong_From_JyLong;
	builtinTypes[20].py_type = &PyComplex_Type;						builtinTypes[20].jy_class = pyComplexClass;			builtinTypes[20].flags = JySYNC_ON_INIT_FLAGS;
	builtinTypes[20].sync = malloc(sizeof(SyncFunctions));
	builtinTypes[20].sync->jyInit = (jyInitSync) JySync_Init_JyComplex_From_PyComplex;
	builtinTypes[20].sync->pyInit = (pyInitSync) JySync_Init_PyComplex_From_JyComplex;
	builtinTypes[21].py_type = &PyUnicode_Type;						builtinTypes[21].jy_class = pyUnicodeClass;			builtinTypes[21].flags = JySYNC_ON_INIT_FLAGS;
	builtinTypes[21].sync = malloc(sizeof(SyncFunctions));
	builtinTypes[21].sync->jyInit = (jyInitSync) JySync_Init_JyUnicode_From_PyUnicode;
	builtinTypes[21].sync->pyInit = (pyInitSync) JySync_Init_PyUnicode_From_JyUnicode;
	builtinTypes[22].py_type = &PyString_Type;						builtinTypes[22].jy_class = pyStringClass;			builtinTypes[22].flags = JySYNC_ON_INIT_FLAGS;
	builtinTypes[22].sync = malloc(sizeof(SyncFunctions));
	builtinTypes[22].sync->jyInit = (jyInitSync) JySync_Init_JyString_From_PyString;
	builtinTypes[22].sync->pyInit = (pyInitSync) JySync_Init_PyString_From_JyString;
	/*builtinTypes[23].py_type = &PyBaseString_Type;					builtinTypes[23].jy_class = pyBaseStringClass;		builtinTypes[23].flags = 0;
	builtinTypes[24].py_type = &PySeqIter_Type;						builtinTypes[24].jy_class = pySequenceIterClass;	builtinTypes[24].flags = 0;
	builtinTypes[25].py_type = &PyRange_Type;						builtinTypes[25].jy_class = pyXRangeClass;			builtinTypes[25].flags = 0;
	builtinTypes[26].py_type = &Pyrangeiter_Type; //jython uses PySequenceIter. Map this to PySeqIter_Typ
																	builtinTypes[26].jy_class = pySequenceIterClass;	builtinTypes[26].flags = 0;*/
	builtinTypes[27].py_type = &PyTuple_Type;						builtinTypes[27].jy_class = pyTupleClass;			builtinTypes[27].flags = JySYNC_ON_INIT_FLAGS;
	builtinTypes[27].sync = malloc(sizeof(SyncFunctions));
	builtinTypes[27].sync->jyInit = (jyInitSync) JySync_Init_JyTuple_From_PyTuple;
	builtinTypes[27].sync->pyInit = (pyInitSync) JySync_Init_PyTuple_From_JyTuple;
/*	builtinTypes[28].py_type = &PyTupleIter_Type; //jython uses PyFastSequenceIter. Map this to PySeqIter_Type
																	builtinTypes[28].jy_class = pyFastSequenceIterClass;builtinTypes[28].flags = 0;*/
	builtinTypes[29].py_type = &PyList_Type;						builtinTypes[29].jy_class = pyListClass;			builtinTypes[29].flags = JySYNC_ON_INIT_FLAGS;
	builtinTypes[29].sync = malloc(sizeof(SyncFunctions));
	builtinTypes[29].sync->jyInit = (jyInitSync) JySync_Init_JyList_From_PyList;
	builtinTypes[29].sync->pyInit = (pyInitSync) JySync_Init_PyList_From_JyList;
	/*builtinTypes[30].py_type = &PyListIter_Type; //jython uses PyFastSequenceIter. Map this to PySeqIter_Type
																	builtinTypes[30].jy_class = pyFastSequenceIterClass;builtinTypes[30].flags = 0;
	builtinTypes[31].py_type = &PyListRevIter_Type; //jython uses PyReversedIterator.
																	builtinTypes[31].jy_class = pyReversedIteratorClass;builtinTypes[31].flags = 0;*/
	builtinTypes[32].py_type = &PyDict_Type;						builtinTypes[32].jy_class = pyDictClass;			builtinTypes[32].flags = JY_TRUNCATE_FLAG_MASK;
	/*builtinTypes[33].py_type = &PySet_Type;							builtinTypes[33].jy_class = pySetClass;				builtinTypes[33].flags = 0;
	builtinTypes[34].py_type = &PySetIter_Type; //jython uses inline subclass of PyIterator. Map this to PySeqIter_Type for now
																	builtinTypes[34].jy_class = pySequenceIterClass;	builtinTypes[34].flags = 0;
	builtinTypes[35].py_type = &PyFrozenSet_Type;					builtinTypes[35].jy_class = pyFrozenSetClass;		builtinTypes[35].flags = 0;
	builtinTypes[36].py_type = &PyEnum_Type;						builtinTypes[36].jy_class = pyEnumerationClass;		builtinTypes[36].flags = 0;*/
	builtinTypes[37].py_type = &PySlice_Type;						builtinTypes[37].jy_class = pySliceClass;			builtinTypes[37].flags = JY_TRUNCATE_FLAG_MASK;
	builtinTypes[38].py_type = &PyEllipsis_Type;					builtinTypes[38].jy_class = pyEllipsisClass;		builtinTypes[38].flags = 0;
	/*builtinTypes[39].py_type = &PyGen_Type;							builtinTypes[39].jy_class = pyGeneratorClass;		builtinTypes[39].flags = 0;
	builtinTypes[40].py_type = &PyCode_Type;						builtinTypes[40].jy_class = pyCodeClass;			builtinTypes[40].flags = 0;
	builtinTypes[41].py_type = &PyCallIter_Type;					builtinTypes[41].jy_class = pyCallIterClass;		builtinTypes[41].flags = 0;
	builtinTypes[42].py_type = &PyFrame_Type;						builtinTypes[42].jy_class = pyFrameClass;			builtinTypes[42].flags = 0;
	builtinTypes[43].py_type = &PySuper_Type;						builtinTypes[43].jy_class = pySuperClass;			builtinTypes[43].flags = 0;*/
	builtinTypes[44].py_type = (PyTypeObject*) PyExc_BaseException;	builtinTypes[44].jy_class = pyBaseExceptionClass;	builtinTypes[44].flags = JY_TRUNCATE_FLAG_MASK;
	builtinTypes[45].py_type = &PyTraceBack_Type;					builtinTypes[45].jy_class = pyTracebackClass;		builtinTypes[45].flags = JY_TRUNCATE_FLAG_MASK;
	/*builtinTypes[46].py_type = &PyByteArray_Type;					builtinTypes[46].jy_class = pyByteArrayClass;		builtinTypes[46].flags = 0;
	builtinTypes[47].py_type = &PyBuffer_Type;						builtinTypes[47].jy_class = pyBufferClass;			builtinTypes[47].flags = 0;
	builtinTypes[48].py_type = &PyMemoryView_Type;					builtinTypes[48].jy_class = pyMemoryViewClass;		builtinTypes[48].flags = 0;
	builtinTypes[49].py_type = &PyBaseObject_Type;					builtinTypes[49].jy_class = pyObjectClass;			builtinTypes[49].flags = 0;*/
}

inline void initBuiltinExceptions()
{
	builtinExceptions[0] = (PyTypeObject*) PyExc_BaseException;
	builtinExceptions[1] = (PyTypeObject*) PyExc_Exception;
	builtinExceptions[2] = (PyTypeObject*) PyExc_StandardError;
	builtinExceptions[3] = (PyTypeObject*) PyExc_TypeError;
	builtinExceptions[4] = (PyTypeObject*) PyExc_StopIteration;
	builtinExceptions[5] = (PyTypeObject*) PyExc_GeneratorExit;
	builtinExceptions[6] = NULL;//(PyTypeObject*) PyExc_SystemExit;
	builtinExceptions[7] = (PyTypeObject*) PyExc_KeyboardInterrupt;
	builtinExceptions[8] = (PyTypeObject*) PyExc_ImportError;
	builtinExceptions[9] = NULL;//(PyTypeObject*) PyExc_EnvironmentError;
	builtinExceptions[10] = NULL;//(PyTypeObject*) PyExc_IOError;
	builtinExceptions[11] = NULL;//(PyTypeObject*) PyExc_OSError;
#ifdef MS_WINDOWS
	builtinExceptions[12] = (PyTypeObject*) PyExc_WindowsError;
#endif
#ifdef __VMS
	builtinExceptions[13] = (PyTypeObject*) PyExc_VMSError;
#endif
	builtinExceptions[14] = (PyTypeObject*) PyExc_EOFError;
	builtinExceptions[15] = (PyTypeObject*) PyExc_RuntimeError;
	builtinExceptions[16] = (PyTypeObject*) PyExc_NotImplementedError;
	builtinExceptions[17] = (PyTypeObject*) PyExc_NameError;
	builtinExceptions[18] = (PyTypeObject*) PyExc_UnboundLocalError;
	builtinExceptions[19] = (PyTypeObject*) PyExc_AttributeError;
	builtinExceptions[20] = NULL;//(PyTypeObject*) PyExc_SyntaxError;
	builtinExceptions[21] = NULL;//(PyTypeObject*) PyExc_IndentationError;
	builtinExceptions[22] = NULL;//(PyTypeObject*) PyExc_TabError;
	builtinExceptions[23] = (PyTypeObject*) PyExc_LookupError;
	builtinExceptions[24] = (PyTypeObject*) PyExc_IndexError;
	builtinExceptions[25] = NULL;//(PyTypeObject*) PyExc_KeyError;
	builtinExceptions[26] = (PyTypeObject*) PyExc_ValueError;
	builtinExceptions[27] = NULL;//(PyTypeObject*) PyExc_UnicodeError;
#ifdef Py_USING_UNICODE
	builtinExceptions[28] = NULL;//(PyTypeObject*) PyExc_UnicodeEncodeError;
	builtinExceptions[29] = NULL;//(PyTypeObject*) PyExc_UnicodeDecodeError;
	builtinExceptions[30] = NULL;//(PyTypeObject*) PyExc_UnicodeTranslateError;
#endif
	builtinExceptions[31] = (PyTypeObject*) PyExc_AssertionError;
	builtinExceptions[32] = (PyTypeObject*) PyExc_ArithmeticError;
	builtinExceptions[33] = (PyTypeObject*) PyExc_FloatingPointError;
	builtinExceptions[34] = (PyTypeObject*) PyExc_OverflowError;
	builtinExceptions[35] = (PyTypeObject*) PyExc_ZeroDivisionError;
	builtinExceptions[36] = (PyTypeObject*) PyExc_SystemError;
	builtinExceptions[37] = (PyTypeObject*) PyExc_ReferenceError;
	builtinExceptions[38] = (PyTypeObject*) PyExc_MemoryError;
	builtinExceptions[39] = (PyTypeObject*) PyExc_BufferError;
	builtinExceptions[40] = (PyTypeObject*) PyExc_Warning;
	builtinExceptions[41] = (PyTypeObject*) PyExc_UserWarning;
	builtinExceptions[42] = (PyTypeObject*) PyExc_DeprecationWarning;
	builtinExceptions[43] = (PyTypeObject*) PyExc_PendingDeprecationWarning;
	builtinExceptions[44] = (PyTypeObject*) PyExc_SyntaxWarning;
	builtinExceptions[45] = (PyTypeObject*) PyExc_RuntimeWarning;
	builtinExceptions[46] = (PyTypeObject*) PyExc_FutureWarning;
	builtinExceptions[47] = (PyTypeObject*) PyExc_ImportWarning;
	builtinExceptions[48] = (PyTypeObject*) PyExc_UnicodeWarning;
	builtinExceptions[49] = (PyTypeObject*) PyExc_BytesWarning;
}

inline jboolean JyNI_IsBuiltinPyType(PyTypeObject* type)
{
	int i;
	for (i = 0; i < builtinTypeCount; ++i)
	{
		if (builtinTypes[i].py_type == type) return JNI_TRUE;
	}
	return JNI_FALSE;
}

inline jclass JyNI_JythonClassFromPyType(PyTypeObject* type)
{
	int i;
	for (i = 0; i < builtinTypeCount; ++i)
	{
		if (builtinTypes[i].py_type == type) return builtinTypes[i].jy_class;
	}
	return NULL;
}

inline TypeMapEntry* JyNI_JythonTypeEntry_FromPyType(PyTypeObject* type)
{
	int i;
	for (i = 0; i < builtinTypeCount; ++i)
	{
		if (builtinTypes[i].py_type == type) return &(builtinTypes[i]);
	}
	return NULL;
}

inline TypeMapEntry* JyNI_JythonTypeEntry_FromJythonPyClass(jclass jythonPyClass)
{
	if (jythonPyClass == NULL) return NULL;
	//if (jythonPyObject == JyNone) return Py_None;
	//if (jythonPyObject == JyNotImplemented) return Py_NotImplemented;
	env(NULL);
	int i;
	for (i = 0; i < builtinTypeCount; ++i)
	{
		if (builtinTypes[i].jy_class != NULL && (*env)->CallBooleanMethod(env, jythonPyClass, classEquals, builtinTypes[i].jy_class))
			return &(builtinTypes[i]);
	}
	return NULL;
}

inline TypeMapEntry* JyNI_JythonTypeEntry_FromName(char* name)
{
	if (name == NULL) return NULL;
	int i;
	for (i = 0; i < builtinTypeCount; ++i)
	{
		if (builtinTypes[i].py_type != NULL && strcmp(builtinTypes[i].py_type->tp_name, name) == 0)
			return &(builtinTypes[i]);
	}
	return NULL;
}

inline TypeMapEntry* JyNI_JythonTypeEntry_FromJStringName(jstring name)
{
	if (name == NULL) return NULL;
	env(NULL);
	cstr_from_jstring(cName, name);
	return JyNI_JythonTypeEntry_FromName(cName);
}

inline TypeMapEntry* JyNI_JythonTypeEntry_FromJythonPyType(jobject jythonPyType)
{
	env(NULL);
	return JyNI_JythonTypeEntry_FromJStringName((*env)->CallObjectMethod(env, jythonPyType, pyTypeGetName));
}

inline jobject JyNI_JythonException_FromPyException(PyObject* exc)
{
	env(NULL);
	return (*env)->CallStaticObjectMethod(env, JyNIClass, JyNIExceptionByName, (*env)->NewStringUTF(env, ((PyTypeObject*) exc)->tp_name));
}

inline PyTypeObject* JyNI_PyException_FromJythonException(jobject exc)
{
	env(NULL);
	jboolean isCopy;
	jobject name = (*env)->CallObjectMethod(env, exc, pyTypeGetName);
	char* utf_string = (*env)->GetStringUTFChars(env, name, &isCopy);
	//char mName[strlen(excPrefix)+strlen(utf_string)+1];
	char mName[strlen(utf_string)+12];
	strcpy(mName, excPrefix);
	strcat(mName, utf_string);
	(*env)->ReleaseStringUTFChars(env, name, utf_string);
	int i;
	for (i = 0; i < builtinExceptionCount; ++i)
	{
		if (builtinExceptions[i] != NULL && strcmp(builtinExceptions[i]->tp_name, mName) == 0) return builtinExceptions[i];
	}
	return NULL;
}

inline void JyNI_SyncJy2Py(JyObject* jy, PyObject* op)
{
	//todo: take care of the other flags
	SyncFunctions* sync = (SyncFunctions*) JyNI_GetJyAttribute(jy, JyAttributeSyncFunctions);
	if (sync != NULL && sync->jy2py != NULL) sync->jy2py(jy->jy, op);
}

inline PyObject* JyNI_InitPyObject(TypeMapEntry* tme, jobject src)
{
	PyObject* dest;
	if (tme->flags & SYNC_ON_JY_INIT_FLAG_MASK)
	{
		//puts("InitPyObject sync-construct");
		if (tme->sync != NULL && tme->sync->pyInit != NULL)
			dest = tme->sync->pyInit(src);
	} else
	{
		//dest = PyObject_GC_New(tme->py_type);
		//puts("InitPyObject by GC New");
		dest = _JyObject_GC_New(tme->py_type, tme);
		//printf("PyObject-size: %u\n", (jlong) sizeof(PyObject));
		PyObject_GC_Track(dest);
		if (dest && tme->sync && tme->sync->jy2py)
			tme->sync->jy2py(src, dest);
		//else puts("no sync needed");
	}
	if (dest)
	{
		//printf("dest at %u\n", (jlong) dest);
		JyObject* jy = AS_JY(dest);
//		printf("jy at %u\n", (jlong) jy);
//		printf("JyObject-size: %u\n", (jlong) sizeof(JyObject));
//		printf("GC_Head-size: %u\n", (jlong) sizeof(PyGC_Head));
//		puts("try access jy:");
//		printf("jy-Flags: %u\n", (int) jy->flags);
//		puts("try access dest:");
//		printf("dest-type: %u\n", (int) dest->ob_type);
//		puts("type-name:");
//		puts(dest->ob_type->tp_name);
//		printf("dest-check for module: %u\n", (int) PyModule_Check(dest));
		//PyBaseObject_Type
		if (jy->flags & SYNC_NEEDED_MASK)
			JyNI_AddJyAttribute(jy, JyAttributeSyncFunctions, tme->sync);//, char flags)
		env(NULL);
		jy->jy = (*env)->NewGlobalRef(env, src);
		jy->flags |= JY_INITIALIZED_FLAG_MASK;
		//printf("dest-check for module2: %u\n", (int) PyModule_Check(dest));
	}
	//printf("dest-check for module3: %u\n", (int) PyModule_Check(dest));
	return dest;
}

/*
 * Does expect a "local" ref!
 * If the ref is stored somehow, it is made global
 * automatically.
 * (Todo: Check what happens if a global ref is made global again?)
 */
inline PyObject* JyNI_PyObject_FromJythonPyObject(jobject jythonPyObject)
{
	return _JyNI_PyObject_FromJythonPyObject(jythonPyObject, JNI_TRUE, JNI_TRUE, JNI_TRUE);
}

inline PyObject* _JyNI_PyObject_FromJythonPyObject(jobject jythonPyObject, jboolean lookupNative, jboolean checkCPeer, jboolean checkForType)
{
	if (jythonPyObject == NULL) return NULL;
	//puts("Transform jython jobject to PyObject*...");
	if (jythonPyObject == JyNone) return Py_None;
	if (jythonPyObject == JyNotImplemented) return Py_NotImplemented;
	if (jythonPyObject == JyEllipsis) return Py_Ellipsis;
	env(NULL);
	//todo: What about exceptions?
	if (checkForType && (*env)->IsInstanceOf(env, jythonPyObject, pyTypeClass))
	{
		return (PyObject*) JyNI_PyTypeObject_FromJythonPyTypeObject(jythonPyObject);
	}
	if (checkCPeer && (*env)->IsInstanceOf(env, jythonPyObject, pyCPeerClass))
	{
		//puts("object is a PyCPeer");
		return (PyObject*) (*env)->GetLongField(env, jythonPyObject, pyCPeerObjectHandle);
	}
	if (lookupNative)
	{
		PyObject* handle = (PyObject*) (*env)->CallStaticLongMethod(env, JyNIClass, JyNILookupNativeHandle, jythonPyObject);
		//printf("handle obtained: %u\n", handle);
		if (handle)
		{
			//don't forget to sync if necessary:
			JyObject* jy = AS_JY(handle);
			if (jy->flags & SYNC_ON_JY_TO_PY_FLAG_MASK)
				JyNI_SyncJy2Py(jy, handle);
			return handle;
		}
	}

	//puts("no handle exists yet");
	//initialize PyObject*...
	//find tme:
	jstring tpName = (*env)->CallObjectMethod(env,
			(*env)->CallObjectMethod(env, jythonPyObject, pyObjectGetType),
			pyTypeGetName);
	//puts("tp name obtained:");
	cstr_from_jstring(cName, tpName);
	//puts(cName);
	TypeMapEntry* tme = JyNI_JythonTypeEntry_FromName(cName);
	PyObject* er = JyNI_InitPyObject(tme, jythonPyObject);
	//puts("handle initialized");
	return er;
	//PyObject* result = JyNI_NewPyObject_FromJythonPyObject(jythonPyObject);
	//Py_INCREF(result);
	//return result;
}

//inline void JyNI_SetUpJyObject(JyObject* jyObject)
//{
//	if (((PyObject*) jyObject) == Py_None) return;
//	jclass jyType = JyNI_JythonClassFromPyType(((PyObject*) jyObject)->ob_type);
//	if (jyType == NULL) return;// pyObject;
//	else
//	{
//		env();
//		jobject jyObj = (*env)->CallStaticObjectMethod(env, JyNIClass, JyNIConstructDefaultObject, jyType);
//		(*env)->CallStaticObjectMethod(env, JyNIClass, JyNISetNativeHandle, jyObj, (jlong) jyObject);
//		jyObject->jy = (*env)->NewGlobalRef(env, jyObj);
//	}
//}

//inline PyTypeObject* JyNI_PyTypeFromJythonPyObject(jobject jythonPyObject)
//{
//	env(NULL);
//	int i;
//	for (i = 0; i < builtinTypeCount; ++i)
//	{
//		//if (builtinTypes[i] == type) return JNI_TRUE;
//		if (builtinTypes[i].py_type != 0 && (*env)->IsInstanceOf(env, jythonPyObject, builtinTypes[i].jy_class))
//			return builtinTypes[i].py_type;
//	}
//	puts("no type found, returning NULL");
//	return NULL;
//}

/*PyObject* PyObject_FromJythonJyObject(jobject jythonJyObject)
{
	//PyObject jyHead = {PyObject_HEAD_INIT(PyType_FromJythonObject(jythonObject))};
	//JyObject result = {jyHead, jythonObject};
	//#define PyObject_GC_New(type, typeobj)	 ( (type *) _PyObject_GC_New(typeobj) )
	JNIEnv *env;
	if ((*java)->GetEnv(java, (void **)&env, JNI_VERSION_1_2)) {
		return NULL; // JNI version not supported
	}
	jlong handle = (*env)->GetLongField(env, jythonJyObject, jyObjectNativeHandleField);
	if (handle == -1)
	{
		jobject jyPy = (*env)->GetObjectField(env, jythonJyObject, jyObjectObjectField);
		JyObject* result = (JyObject*) PyObject_FromJythonPyObject(jyPy);
		result->jy = jyPy;
		result->jyJy = jythonJyObject;
		(*env)->SetLongField(env, jythonJyObject, jyObjectNativeHandleField, (jlong) result);
		return (PyObject*) result;
	} else
	{
		return (PyObject*) handle;//(PyObject*) &result;
	}
}*/

inline void JyNI_SyncPy2Jy(PyObject* op, JyObject* jy)
{
	//todo: take care of the other flags
	SyncFunctions* sync = (SyncFunctions*) JyNI_GetJyAttribute(jy, JyAttributeSyncFunctions);
	if (sync != NULL && sync->py2jy != NULL) sync->py2jy(op, jy->jy);
}

inline jobject JyNI_InitJythonPyObject(TypeMapEntry* tme, PyObject* src, JyObject* srcJy)
{
	jobject dest;
	if (tme->flags & SYNC_ON_JY_INIT_FLAG_MASK)
	{
		if (tme->sync && tme->sync->jyInit)
			dest = tme->sync->jyInit(src);
	} else
	{
		env(NULL);
		jmethodID cm = (*env)->GetMethodID(env, tme->jy_class, "<init>", "()V");
		if (cm)
		{
			dest = (*env)->NewObject(env, tme->jy_class, cm);
			if (tme->sync && tme->sync->py2jy)
				tme->sync->py2jy(src, dest);
		} else
			dest = NULL;
	}
	if (dest && (srcJy->flags & SYNC_NEEDED_MASK))
		JyNI_AddJyAttribute(srcJy, JyAttributeSyncFunctions, tme->sync);//, char flags)
	env(NULL);
	srcJy->jy = (*env)->NewWeakGlobalRef(env, dest);
	srcJy->flags |= JY_INITIALIZED_FLAG_MASK;
	return srcJy->jy;
}

inline jobject JyNI_JythonPyObject_FromPyObject(PyObject* op)
{
	if (op == NULL) return NULL;
	if (op == Py_None) return JyNone;
	if (op == Py_NotImplemented) return JyNotImplemented;
	if (op == Py_Ellipsis) return JyEllipsis;
	if (PyExceptionClass_Check(op))
	{
		//puts("convert exception...");
		jobject er = JyNI_JythonException_FromPyException(op);
		if (er != NULL) return er;
	}
	if (PyType_Check(op))
	{
		jobject er = JyNI_JythonPyTypeObject_FromPyTypeObject((PyTypeObject*) op);
		if (er != NULL) return er;
	}
	JyObject* jy = AS_JY(op);
	//if (JyNI_IsJyObject(op))
	if (JyObject_IS_INITIALIZED(jy))
	{
		if (jy->flags & SYNC_ON_PY_TO_JY_FLAG_MASK)
			JyNI_SyncPy2Jy(op, jy);
		return jy->jy;
	} else
	{
		TypeMapEntry* tme;
		if (jy->jy != NULL) tme = (TypeMapEntry*) jy->jy;
		else tme = JyNI_JythonTypeEntry_FromPyType(Py_TYPE(op));

		if (tme != NULL)
			return JyNI_InitJythonPyObject(tme, op, jy);
		else
		{
			//setup and return PyCPeer in this case...
			env(NULL);
			//The following lookup is not neccesary, because if there already was a PyCPeer,
			//JyObject_IS_INITIALIZED would have evaluated true.
			//jobject er = (*env)->CallStaticObjectMethod(env, JyNIClass, JyNILookupCPeerHandle, (jlong) op);
			//if (er != NULL) return er;
//			puts("creating PyCPeer for Type:");
//			puts(Py_TYPE(op)->tp_name);
//			printf("op-address: %u\n", (jlong) op);
			//PyCPeer has to be created...
			Py_INCREF(op);
			//first obtain type:
			jobject opType = JyNI_JythonPyTypeObject_FromPyTypeObject(Py_TYPE(op));
//			puts("opType-address:");
//			printf("%u\n", (jlong) opType);
			jobject er = (*env)->NewObject(env, pyCPeerClass, pyCPeerConstructor, (jlong) op, opType);
			jy->flags |= JY_INITIALIZED_FLAG_MASK;
			jy->flags |= JY_CPEER_FLAG_MASK;
			jy->jy = (*env)->NewWeakGlobalRef(env, er);
			//(*env)->SetLongField(env, er, pyCPeerRefHandle, (jlong) ref);
			return jy->jy;
		}
	}
}

inline jobject _JyNI_JythonPyTypeObject_FromPyTypeObject(PyTypeObject* type, jclass cls)
{
	//if (type == NULL) return NULL;
	env(NULL);
	if (cls != NULL)
	{
		return (*env)->CallStaticObjectMethod(env, JyNIClass, JyNIGetPyType, cls);
	} else {
		//setup and return PyCPeerType in this case...
		jobject er = (*env)->CallStaticObjectMethod(env, JyNIClass, JyNILookupCPeerHandle, (jlong) type);
		if (er != NULL) return er;
		//No JyObjects available for types!
		//JyObject* jy = AS_JY_NO_GC(type); //since we currently don't init types properly, GC-check would not work with PyTypeObjects
		//if (JyObject_IS_INITIALIZED(jy))
		//	return jy->jy; //no sync-attempt for types
		else {
			Py_INCREF(type);
			jobject er = (*env)->NewObject(env, pyCPeerTypeClass, pyCPeerTypeConstructor, (jlong) type);
			jweak ref = (*env)->NewWeakGlobalRef(env, er);
			(*env)->SetLongField(env, er, pyCPeerTypeRefHandle, (jlong) ref);
			return ref;
		}
	}
}

inline jobject JyNI_JythonPyTypeObject_FromPyTypeObject(PyTypeObject* type)
{
	if (type == NULL) return NULL;
	//jclass cls = JyNI_JythonClassFromPyType((PyTypeObject*) type);
	return _JyNI_JythonPyTypeObject_FromPyTypeObject(type, JyNI_JythonClassFromPyType((PyTypeObject*) type));
}

//No support for derived types for now...
//Only builtin types or native declared types work.
inline PyTypeObject* JyNI_PyTypeObject_FromJythonPyTypeObject(jobject jythonPyTypeObject)
{
	env(NULL);
	if ((*env)->IsInstanceOf(env, jythonPyTypeObject, pyCPeerTypeClass))
	{
		return (PyTypeObject*) (*env)->GetLongField(env, jythonPyTypeObject, pyCPeerTypeObjectHandle);
	}

	//jstring name = (*env)->CallStaticObjectMethod(env, pyTypeClass, pyTypeGetName, jythonPyTypeObject);
	jstring name = (*env)->CallObjectMethod(env, jythonPyTypeObject, pyTypeGetName);
//	jboolean isCopy;
//
//	char* utf_string = (*env)->GetStringUTFChars(env, name, &isCopy);
//	//"+1" for 0-termination:
//	char mName[strlen(utf_string)+1];
//	strcpy(mName, utf_string);
//	(*env)->ReleaseStringUTFChars(env, name, utf_string);
	cstr_from_jstring(mName, name);
	int i;
	for (i = 0; i < builtinTypeCount; ++i)
	{
		if (builtinTypes[i].py_type != NULL && strcmp(builtinTypes[i].py_type->tp_name, mName) == 0) return builtinTypes[i].py_type;
	}
	return NULL;
}

/*returns local ref!*/
inline jstring JyNI_jstring_FromPyStringObject(JNIEnv *env, PyStringObject* op)
{
	JyObject* jy = AS_JY_NO_GC(op);
	if (jy->flags & JY_INITIALIZED_FLAG_MASK)
	{
		return (*env)->CallObjectMethod(env, JyNI_JythonPyObject_FromPyObject((PyObject*) op), pyStringAsString);
	} else
	{
		return (*env)->NewStringUTF(env, PyString_AS_STRING(op));
	}
}

/*returns local ref!*/
inline jstring JyNI_interned_jstring_FromPyStringObject(JNIEnv *env, PyStringObject* op)
{
	JyObject* jy = AS_JY_NO_GC(op);
	if (jy->flags & JY_INITIALIZED_FLAG_MASK)
	{
		jstring er = (*env)->CallObjectMethod(env, JyNI_JythonPyObject_FromPyObject((PyObject*) op), pyStringAsString);
		if (JyNI_HasJyAttribute(jy, JyAttributeStringInterned))
		{
			return er;
		} else
		{
			return (*env)->CallObjectMethod(env, er, stringIntern);
		}
	} else
	{
		return (*env)->CallObjectMethod(env, (*env)->NewStringUTF(env, PyString_AS_STRING(op)), stringIntern);
	}
}

/* JY_DELEGATE indicates, when Functions like repr
 * or GetAttr should be delegated to Jython.
 * At least in Truncate-Case, this is necessary and
 * at least in CPeer-Case, it is impossible.
 */
//Only delegate if truncated:
//#define JY_DELEGATE(flags) (flags & JY_TRUNCATE_FLAG_MASK)

//Delegate in every case but CPeer-case:

/*
 * JY_DELEGATE:
 * if initialized & !CPeer: true
 * if !initialized & existsClass: true
 */
#define JY_DELEGATE(v, flags) ((flags & JY_INITIALIZED_FLAG_MASK) ? \
			!(flags & JY_CPEER_FLAG_MASK) : \
			(JyNI_JythonClassFromPyType(Py_TYPE(v)) != NULL))

/*returns local ref!*/
inline jobject JyNI_GetJythonDelegate(PyObject* v)
{


	if (!PyType_Check(v)) // && !PyExc_Check(v)
	{
		JyObject* jy = AS_JY(v);
		//JY_INITIALIZED_FLAG_MASK JY_CPEER_FLAG_MASK
		if (JY_DELEGATE(v, jy->flags))
		{
			env(NULL);
			//return JyNI_PyObject_FromJythonPyObject((*env)->CallObjectMethod(env, JyNI_JythonPyObject_FromPyObject(v), pyObject__repr__));
			JyNI_JythonPyObject_FromPyObject(v);
		}
	} else {
	//		jobject cPeer = (*env)->CallStaticObjectMethod(env, JyNIClass, JyNILookupCPeerHandle, (jlong) v);
	//		if (cPeer == NULL) ...

		jclass cls = JyNI_JythonClassFromPyType((PyTypeObject*) v);
		if (cls != NULL) //delegate
		{
			return _JyNI_JythonPyTypeObject_FromPyTypeObject((PyTypeObject*) v, cls);
			//env(NULL);
			//return JyNI_PyObject_FromJythonPyObject((*env)->CallObjectMethod(env, jyV, pyObject__repr__));
		}
		//todo... care for exception-types
	}
	return NULL;
}

//inline PyObject* JyNI_PyObject_FromJythonPyObjectAndHandle(jobject jythonPyObject, jlong handle)
//{
//	if (jythonPyObject == JyNone) return Py_None;
//	if (jythonPyObject == JyNotImplemented) return Py_NotImplemented;
//	if (((PyObject*) handle) == NULL)
//	{
//		PyObject* result = JyNI_NewPyObject_FromJythonPyObject(jythonPyObject);
//		Py_INCREF(result);
//		return result;
//	} else
//	{
//		return (PyObject*) handle;
//	}
//}

//inline PyObject* JyNI_NewPyObject_FromJythonPyObject(jobject jythonPyObject)
//{
//	//PyObject jyHead = {PyObject_HEAD_INIT(PyType_FromJythonObject(jythonObject))};
//	//JyObject result = {jyHead, jythonObject};
//	//#define PyObject_GC_New(type, typeobj)	 ( (type *) _PyObject_GC_New(typeobj) )
//	env(NULL);
//	PyTypeObject* tp = JyNI_PyTypeFromJythonPyObject(jythonPyObject);
//	JyObject* result;
//	if (tp->tp_itemsize == 0)
//	{
//		result = JyNI_JyObject_GC_New(tp);
//		result->jy = (*env)->NewGlobalRef(env, jythonPyObject);
//	} else
//	{
//		result = (JyObject*) JyNI_JyObject_GC_NewVar(tp, 0);
//		((JyVarObject*) result)->jy = (*env)->NewGlobalRef(env, jythonPyObject);
//	}
//	(*env)->CallStaticVoidMethod(env, JyNIClass, JyNISetNativeHandle, jythonPyObject, (jlong) result);
//	PyObject_GC_Track(result);
//	return (PyObject*) result;//(PyObject*) &result;
//}

inline jint JyNI_GetDLOpenFlags()
{
	env(0x00001 | 0x00100); //RTLD_LAZY | RTLD_GLOBAL
	return (*env)->CallStaticIntMethod(env, JyNIClass, JyNIGetDLOpenFlags);
}

//Like PyErr_SetString but without detour for exception.
inline void JyNI_JyErr_SetString(jobject exception, const char *string)
{
	env();
	(*env)->CallStaticVoidMethod(env, JyNIClass, JyNIPyErr_SetString, exception, (*env)->NewStringUTF(env, string));
}

inline void JyNI_JyErr_SetObject(jobject exception, PyObject *value)
{
	//Py_XINCREF(exception);
	//Py_XINCREF(value);
	//PyErr_Restore(exception, value, (PyObject *)NULL);
	env();
	(*env)->CallStaticVoidMethod(env, JyNIClass, JyNIPyErr_SetObject, exception, JyNI_JythonPyObject_FromPyObject(value));
}

inline void JyNI_JyErr_SetExceptionObject(jobject exception)
{
	//Py_XINCREF(exception);
	//Py_XINCREF(value);
	//PyErr_Restore(exception, value, (PyObject *)NULL);
	env();
	jobject tstate = (*env)->CallStaticObjectMethod(env, pyPyClass, pyPyGetThreadState);
	(*env)->SetObjectField(env, tstate, pyThreadStateExceptionField, exception);
}

inline int JyNI_JyErr_ExceptionMatches(jobject exc)
{
	env(0);
	return (*env)->CallStaticBooleanMethod(env, JyNIClass, JyNIPyErr_ExceptionMatches, exc);

    //return PyErr_GivenExceptionMatches(PyErr_Occurred(), exc);
}

inline PyObject* JyNI_JyErr_Format(jobject exception, const char *format, ...)
{
	va_list vargs;
	PyObject* string;

#ifdef HAVE_STDARG_PROTOTYPES
	va_start(vargs, format);
#else
	va_start(vargs);
#endif

	string = PyString_FromFormatV(format, vargs);
	JyNI_JyErr_SetObject(exception, string);
	Py_XDECREF(string);
	va_end(vargs);
	return NULL;
}

inline void JyNI_CleanUp_JyObject(JyObject* obj)
{
	//if (FROM_JY(obj) == Py_None) return;
	//if (obj == NULL) return;
	//if (FROM_JY(obj) == NULL) return;
	if (obj->attr) JyNI_ClearJyAttributes(obj);
	if (obj->jy)
	{
		env();
		(*env)->CallStaticVoidMethod(env, JyNIClass, JyNIClearNativeHandle, obj->jy);
		if (obj->flags & JY_CPEER_FLAG_MASK)
			(*env)->DeleteWeakGlobalRef(env, obj->jy);
		else
			(*env)->DeleteGlobalRef(env, obj->jy);
	}
}

void JyNI_Del(void * obj)
{
	//assume no gc, maybe change this later:
	JyObject* jy = AS_JY_NO_GC(obj);
	JyNI_CleanUp_JyObject(jy);
	PyObject_RawFree(jy);
}

//inline char* PyLongToJavaSideString(PyObject* pl)
//{
//	if (pl == NULL) return "plNULL";
//	JyObject* jy = AS_JY_NO_GC(pl);
//	if (jy->jy == NULL) return ("jyNULL");
//	env(NULL);
//	jstring er = (*env)->CallObjectMethod(env, jy->jy, pyObjectAsString);
//	//cstr_from_jstring(er2, er);
//	return "hier2";
//}

//singletons:
JavaVM* java;
jobject JyNone;
jobject JyNotImplemented;
jobject JyEllipsis;
//PyUnicodeObject* unicode_empty;
PyObject* PyTrue;
PyObject* PyFalse;

jclass classClass;
jmethodID classEquals;

jclass JyNIClass;
jmethodID JyNISetNativeHandle;
jmethodID JyNILookupNativeHandle;
jmethodID JyNILookupCPeerHandle;
jmethodID JyNIClearNativeHandle;
//jmethodID JyNIConstructDefaultObject;
jmethodID JyNIGetDLOpenFlags;
jmethodID JyNIGetJyObjectByName;
jmethodID JyNIGetPyObjectByName;
jmethodID JyNIGetPyType;
jmethodID JyNI_getNativeAvailableKeysAndValues;
jmethodID JyNIExceptionByName;
jmethodID JyNIPyErr_Restore;
jmethodID JyNIPyErr_Clear;
jmethodID JyNIPyErr_Occurred;
jmethodID JyNIPyErr_ExceptionMatches;
jmethodID JyNIPyErr_SetObject;
jmethodID JyNIPyErr_SetString;
jmethodID JyNIPyErr_SetNone;
jmethodID JyNIPyErr_NoMemory;
jmethodID JyNIPyErr_Fetch;
jmethodID JyNIPyErr_WriteUnraisable;
jmethodID JyNIGetDLVerbose;
jmethodID JyNI_PyImport_FindExtension;
jmethodID JyNIGetPyDictionary_Next;
jmethodID JyNIPyImport_GetModuleDict;
jmethodID JyNIPyImport_AddModule;
jmethodID JyNIJyNI_GetModule;
jmethodID JyNISlice_compare;
jmethodID JyNIPrintPyLong;
jmethodID JyNILookupNativeHandles;

jclass JyNIDictNextResultClass;
jfieldID JyNIDictNextResultKeyField;
jfieldID JyNIDictNextResultValueField;
jfieldID JyNIDictNextResultNewIndexField;
jfieldID JyNIDictNextResultKeyHandleField;
jfieldID JyNIDictNextResultValueHandleField;

jclass JyListClass;
jmethodID JyListFromBackendHandleConstructor;
jmethodID JyListInstallToPyList;

jclass pyCPeerClass;
jmethodID pyCPeerConstructor;
jfieldID pyCPeerObjectHandle;
//jfieldID pyCPeerRefHandle;

jclass pyCPeerTypeClass;
jmethodID pyCPeerTypeConstructor;
jfieldID pyCPeerTypeObjectHandle;
jfieldID pyCPeerTypeRefHandle;

jclass stringClass;
jmethodID stringFromBytesAndCharsetNameConstructor;
jmethodID stringToUpperCase;
jmethodID stringGetBytesUsingCharset;
jmethodID stringIntern;

jclass bigIntClass;
jmethodID bigIntFromStringConstructor;
jmethodID bigIntFromStringRadixConstructor;
jmethodID bigIntegerFromByteArrayConstructor;
jmethodID bigIntegerFromSignByteArrayConstructor;
jmethodID bigIntToByteArray;
jmethodID bigIntSignum;
jmethodID bigIntToStringRadix;

jclass pyPyClass;
jmethodID pyPyGetThreadState;
jmethodID pyPyGetFrame;
jmethodID pyPyIsSubClass;
jmethodID pyPyWarning;
jmethodID pyPyNewString;
jmethodID pyPyNewUnicode;
jmethodID pyPyNewIntFromInt;
jmethodID pyPyNewIntFromLong;
jmethodID pyPyNewLongFromBigInt;
jmethodID pyPyNewLongFromInt;
jmethodID pyPyNewLongFromLong;
jmethodID pyPyNewLongFromString;
jmethodID pyPyNewFloatFromDouble;
jmethodID pyPyWarningStck;
jmethodID pyPyExplicitWarning;
jfieldID pyPyImportError;
jfieldID pyPySystemError;
jfieldID pyPyUnicodeError;
jfieldID pyPyUnicodeEncodeError;
jfieldID pyPyUnicodeDecodeError;
jfieldID pyPyUnicodeTranslateError;
jfieldID pyPyUnicodeWarning;
jmethodID pyPyUnicodeErrorFactory;
jmethodID pyPyUnicodeEncodeErrorFactory;
jmethodID pyPyUnicodeDecodeErrorFactory;
jmethodID pyPyUnicodeTranslateErrorFactory;
jmethodID pyPyRaiseUnicodeWarning;

jclass pyObjectClass;
jmethodID pyObjectGetType;
jmethodID pyObjectAsString;
jfieldID pyObjectType;
jmethodID pyObject__getattr__;
jmethodID pyObject__findattr__;
jmethodID pyObject__setattr__;
jmethodID pyObject__repr__;

jclass pyThreadStateClass;
jfieldID pyThreadStateExceptionField;
jfieldID pyThreadStateRecursionDepth;
jmethodID pyThreadStateEnterRecursiveCall;
jmethodID pyThreadStateLeaveRecursiveCall;

jclass pyInstanceClass;
jfieldID pyInstanceInstclassField;
jmethodID pyInstanceIsSequenceType;

jclass pyFrameClass;

jclass pyTracebackClass;
jmethodID pyTracebackByTracebackFrameConstructor;

jclass pyExceptionClass;
jfieldID pyExceptionTypeField;
jfieldID pyExceptionValueField;
jfieldID pyExceptionTracebackField;
jmethodID pyExceptionFullConstructor;
jmethodID pyExceptionTypeValueConstructor;
jmethodID pyExceptionNormalize;
jmethodID pyExceptionIsExceptionClass;
jmethodID pyExceptionIsExceptionInstance;

jclass pyBooleanClass;
jmethodID pyBooleanConstructor;

jclass pyArrayClass;
jmethodID pyArrayGetTypecode;

jclass pyTupleClass;
jmethodID pyTupleConstructor;
jmethodID pyTupleByPyObjectArrayBooleanConstructor;
jmethodID pyTupleSize;
jmethodID pyTuplePyGet;
//jmethodID pyTupleGetArray;

jclass pyListClass;
jmethodID pyListConstructor;
jmethodID pyListByPyObjectArrayConstructor;
jmethodID pyListFromList;
jmethodID pyListSize;
jmethodID pyListPyGet;
jmethodID pyListPySet;
jmethodID pyListAppend;
jmethodID pyListInsert;
jmethodID pyListSort;
jmethodID pyListReverse;
jmethodID pyListGetArray;

jclass pyDictClass;
jmethodID pyDictConstructor;
jmethodID pyDictByPyObjectArrayConstructor;
jmethodID pyDictGet_PyObject;
jmethodID pyDictGet_PyObjectWithDefault;
jmethodID pyDict__setitem__;
jmethodID pyDict__delitem__;
jmethodID pyDictClear;
jmethodID pyDictSize;

jclass pyIntClass;
jmethodID pyIntConstructor;
jmethodID pyIntAsInt;
jmethodID pyIntAsLong;

jclass pyLongClass;
jmethodID pyLongByBigIntConstructor;
jmethodID pyLongByLongConstructor;
jmethodID pyLongAsLong;
jmethodID pyLongGetLong;
jmethodID pyLongGetValue;
jmethodID pyLongBit_length;
//jmethodID pyLongToString;

jclass pyUnicodeClass;
jmethodID pyUnicodeByJStringConstructor;
jmethodID pyUnicodeAsString;

jclass pyStringClass;
jmethodID pyStringByJStringConstructor;
jmethodID pyStringAsString;

jclass pyFloatClass;
jmethodID pyFloatByDoubleConstructor;
jmethodID pyFloatAsDouble;
jfieldID pyFloatTypeField;

jclass pyComplexClass;
jmethodID pyComplexBy2DoubleConstructor;
jfieldID pyComplexRealField;
jfieldID pyComplexImagField;

jclass pySequenceClass;
jmethodID pySequenceLen;
jmethodID pySequenceGetItem;

jclass pySequenceListClass;
jmethodID pySequenceListSize;
jmethodID pySequenceListPyGet;

jclass pyTypeClass;
jmethodID pyTypeGetName;
jmethodID pyTypeIsSubType;

jclass pyCodecsClass;
jmethodID pyCodecsDecode;
jmethodID pyCodecsEncode;
jmethodID pyCodecsGetDefaultEncoding;
jmethodID pyCodecsLookup;
jmethodID pyCodecsRegister;
jmethodID pyCodecsLookupError;
jmethodID pyCodecsRegisterError;

//jclass jyObjectClass;
//jfieldID jyObjectNativeHandleField;
//jfieldID jyObjectObjectField;

jclass pyFileClass;
jmethodID pyFileWrite;

jclass pyNotImplementedClass;
jclass pyNoneClass;

jclass pyModuleClass;
jmethodID pyModuleByStringConstructor;
jmethodID pyModuleGetDict;
jmethodID pyModule__setattr__;
jmethodID pyModule__delattr__;

jclass pyCellClass;
jclass pyClassClass;
jclass pyMethodClass;
jclass pyFunctionClass;
jclass pyClassMethodClass;
jclass pyStaticMethodClass;
jclass pyMethodDescrClass;
jclass pyClassMethodDescrClass;
jclass pyDictProxyClass;
jclass pyPropertyClass;
jclass pyBaseStringClass;
jclass pyXRangeClass;
jclass pySequenceIterClass;
jclass pyFastSequenceIterClass;
jclass pyReversedIteratorClass;
jclass pySetClass;
jclass pyFrozenSetClass;
jclass pyEnumerationClass;

jclass pySliceClass;
jmethodID pySliceFromStartStopStepConstructor;
jmethodID pySliceToString;
jmethodID pySliceGetStart;
jmethodID pySliceGetStop;
jmethodID pySliceGetStep;
jmethodID pySliceIndicesEx;

jclass pyEllipsisClass;
jclass pyGeneratorClass;
jclass pyCodeClass;
jclass pyCallIterClass;
jclass pySuperClass;
jclass pyBaseExceptionClass;
jclass pyByteArrayClass;
jclass pyBufferClass;
jclass pyMemoryViewClass;

inline jint initJNI(JNIEnv *env)
{
	jclass classClassLocal = (*env)->FindClass(env, "java/lang/Class");
	if (classClassLocal == NULL) { return JNI_ERR;}
	classClass = (jclass) (*env)->NewWeakGlobalRef(env, classClassLocal);
	(*env)->DeleteLocalRef(env, classClassLocal);
	classEquals = (*env)->GetMethodID(env, classClass, "equals", "(Ljava/lang/Object;)Z");

	jclass stringClassLocal = (*env)->FindClass(env, "java/lang/String");
	if (stringClassLocal == NULL) { return JNI_ERR;}
	stringClass = (jclass) (*env)->NewWeakGlobalRef(env, stringClassLocal);
	(*env)->DeleteLocalRef(env, stringClassLocal);
	stringFromBytesAndCharsetNameConstructor = (*env)->GetMethodID(env, stringClass, "<init>", "([BLjava/lang/String;)V");
	stringToUpperCase = (*env)->GetMethodID(env, stringClass, "toUpperCase", "()Ljava/lang/String;");
	stringGetBytesUsingCharset = (*env)->GetMethodID(env, stringClass, "getBytes", "(Ljava/lang/String;)[B");
	stringIntern = (*env)->GetMethodID(env, stringClass, "intern", "()Ljava/lang/String;");

	jclass bigIntClassLocal = (*env)->FindClass(env, "java/math/BigInteger");
	if (bigIntClassLocal == NULL) { return JNI_ERR;}
	bigIntClass = (jclass) (*env)->NewWeakGlobalRef(env, bigIntClassLocal);
	(*env)->DeleteLocalRef(env, bigIntClassLocal);
	bigIntegerFromByteArrayConstructor = (*env)->GetMethodID(env, bigIntClass, "<init>", "([B)V");
	bigIntegerFromSignByteArrayConstructor = (*env)->GetMethodID(env, bigIntClass, "<init>", "(I[B)V");
	bigIntFromStringConstructor = (*env)->GetMethodID(env, bigIntClass, "<init>", "(Ljava/lang/String;)V");
	bigIntFromStringRadixConstructor = (*env)->GetMethodID(env, bigIntClass, "<init>", "(Ljava/lang/String;I)V");
	bigIntToByteArray = (*env)->GetMethodID(env, bigIntClass, "toByteArray", "()[B");
	bigIntSignum = (*env)->GetMethodID(env, bigIntClass, "signum", "()I");
	bigIntToStringRadix = (*env)->GetMethodID(env, bigIntClass, "toString", "(I)Ljava/lang/String;");

	return JNI_VERSION_1_2;
}

inline jint initJyNI(JNIEnv *env)
{
	/*jclass jyObjectClassLocal = (*env)->FindClass(env, "JyNI/JyObject");
	if (jyObjectClassLocal == NULL) { return JNI_ERR;}
	jyObjectClass = (jclass) (*env)->NewWeakGlobalRef(env, jyObjectClassLocal);
	(*env)->DeleteLocalRef(env, jyObjectClassLocal);
	jyObjectNativeHandleField = (*env)->GetFieldID(env, jyObjectClass, "nativeHandle", "I");
	jyObjectObjectField = (*env)->GetFieldID(env, jyObjectClass, "object", "Lorg/python/core/PyObject;");*/

	jclass JyNIClassLocal = (*env)->FindClass(env, "JyNI/JyNI");
	JyNIClass = (jclass) (*env)->NewWeakGlobalRef(env, JyNIClassLocal);
	(*env)->DeleteLocalRef(env, JyNIClassLocal);
	JyNISetNativeHandle = (*env)->GetStaticMethodID(env, JyNIClass, "setNativeHandle", "(Lorg/python/core/PyObject;J)V");
	JyNILookupNativeHandle = (*env)->GetStaticMethodID(env, JyNIClass, "lookupNativeHandle", "(Lorg/python/core/PyObject;)J");
	JyNIClearNativeHandle = (*env)->GetStaticMethodID(env, JyNIClass, "clearNativeHandle", "(Lorg/python/core/PyObject;)V");
	JyNILookupCPeerHandle = (*env)->GetStaticMethodID(env, JyNIClass, "lookupCPeerHandle", "(J)Lorg/python/core/PyObject;");
	//JyNIConstructDefaultObject = (*env)->GetStaticMethodID(env, JyNIClass, "constructDefaultObject", "(Ljava/lang/Class;)Lorg/python/core/PyObject;");
	JyNIGetDLOpenFlags = (*env)->GetStaticMethodID(env, JyNIClass, "getDLOpenFlags", "()I");
	JyNIGetDLVerbose = (*env)->GetStaticMethodID(env, JyNIClass, "getDLVerbose", "()I");
	JyNIGetJyObjectByName = (*env)->GetStaticMethodID(env, JyNIClass, "getJyObjectByName", "(Ljava/lang/String;)J");
	JyNIGetPyObjectByName = (*env)->GetStaticMethodID(env, JyNIClass, "getPyObjectByName", "(Ljava/lang/String;)Lorg/python/core/PyObject;");
	JyNI_PyImport_FindExtension = (*env)->GetStaticMethodID(env, JyNIClass, "_PyImport_FindExtension", "(Ljava/lang/String;Ljava/lang/String;)Lorg/python/core/PyObject;");
	JyNI_getNativeAvailableKeysAndValues = (*env)->GetStaticMethodID(env, JyNIClass, "getNativeAvailableKeysAndValues", "(Lorg/python/core/PyDictionary;)[J");
	JyNIGetPyDictionary_Next = (*env)->GetStaticMethodID(env, JyNIClass, "getPyDictionary_Next", "(Lorg/python/core/PyDictionary;I)LJyNI/JyNIDictNextResult;");
	JyNIPyImport_GetModuleDict = (*env)->GetStaticMethodID(env, JyNIClass, "PyImport_GetModuleDict", "()Lorg/python/core/PyObject;");
	JyNIPyImport_AddModule = (*env)->GetStaticMethodID(env, JyNIClass, "PyImport_AddModule", "(Ljava/lang/String;)Lorg/python/core/PyObject;");
	JyNIJyNI_GetModule = (*env)->GetStaticMethodID(env, JyNIClass, "JyNI_GetModule", "(Ljava/lang/String;)Lorg/python/core/PyObject;");
	JyNISlice_compare = (*env)->GetStaticMethodID(env, JyNIClass, "slice_compare", "(Lorg/python/core/PySlice;Lorg/python/core/PySlice;)I");
	JyNIPrintPyLong = (*env)->GetStaticMethodID(env, JyNIClass, "printPyLong", "(Lorg/python/core/PyObject;)V");
	JyNILookupNativeHandles = (*env)->GetStaticMethodID(env, JyNIClass, "lookupNativeHandles", "(Lorg/python/core/PyList;)[J");

	//Error stuff:
	JyNIPyErr_Restore = (*env)->GetStaticMethodID(env, JyNIClass, "PyErr_Restore", "(Lorg/python/core/PyObject;Lorg/python/core/PyObject;Lorg/python/core/PyObject;)V");
	JyNIPyErr_Clear = (*env)->GetStaticMethodID(env, JyNIClass, "PyErr_Clear", "()V");
	JyNIPyErr_Occurred = (*env)->GetStaticMethodID(env, JyNIClass, "PyErr_Occurred", "()Lorg/python/core/PyObject;");
	JyNIPyErr_ExceptionMatches = (*env)->GetStaticMethodID(env, JyNIClass, "PyErr_ExceptionMatches", "(Lorg/python/core/PyObject;)Z");
	JyNIPyErr_SetObject = (*env)->GetStaticMethodID(env, JyNIClass, "PyErr_SetObject", "(Lorg/python/core/PyObject;Lorg/python/core/PyObject;)V");
	JyNIPyErr_SetString = (*env)->GetStaticMethodID(env, JyNIClass, "PyErr_SetString", "(Lorg/python/core/PyObject;Ljava/lang/String;)V");
	JyNIPyErr_SetNone = (*env)->GetStaticMethodID(env, JyNIClass, "PyErr_SetNone", "(Lorg/python/core/PyObject;)V");
	JyNIPyErr_NoMemory = (*env)->GetStaticMethodID(env, JyNIClass, "PyErr_NoMemory", "()Lorg/python/core/PyObject;");
	JyNIPyErr_Fetch = (*env)->GetStaticMethodID(env, JyNIClass, "PyErr_Fetch", "()Lorg/python/core/PyException;");
	JyNIPyErr_WriteUnraisable = (*env)->GetStaticMethodID(env, JyNIClass, "PyErr_WriteUnraisable", "(Lorg/python/core/PyObject;)V");
	JyNIExceptionByName = (*env)->GetStaticMethodID(env, JyNIClass, "exceptionByName", "(Ljava/lang/String;)Lorg/python/core/PyObject;");
	JyNIGetPyType = (*env)->GetStaticMethodID(env, JyNIClass, "getPyType", "(Ljava/lang/Class;)Lorg/python/core/PyType;");

	jclass JyNIDictNextResultClassLocal = (*env)->FindClass(env, "JyNI/JyNIDictNextResult");
	JyNIDictNextResultClass = (jclass) (*env)->NewWeakGlobalRef(env, JyNIDictNextResultClassLocal);
	(*env)->DeleteLocalRef(env, JyNIDictNextResultClassLocal);
	JyNIDictNextResultKeyField = (*env)->GetFieldID(env, JyNIDictNextResultClass, "key", "Lorg/python/core/PyObject;");
	JyNIDictNextResultValueField = (*env)->GetFieldID(env, JyNIDictNextResultClass, "value", "Lorg/python/core/PyObject;");
	JyNIDictNextResultNewIndexField = (*env)->GetFieldID(env, JyNIDictNextResultClass, "newIndex", "I");
	JyNIDictNextResultKeyHandleField = (*env)->GetFieldID(env, JyNIDictNextResultClass, "keyHandle", "J");
	JyNIDictNextResultValueHandleField = (*env)->GetFieldID(env, JyNIDictNextResultClass, "valueHandle", "J");

	jclass JyListClassLocal = (*env)->FindClass(env, "JyNI/JyList");
	JyListClass = (jclass) (*env)->NewWeakGlobalRef(env, JyListClassLocal);
	(*env)->DeleteLocalRef(env, JyListClassLocal);
	JyListFromBackendHandleConstructor = (*env)->GetMethodID(env, JyListClass, "<init>", "(J)V");
	JyListInstallToPyList = (*env)->GetMethodID(env, JyListClass, "installToPyList", "(Lorg/python/core/PyList;)V");

	//Peer stuff:
	jclass pyCPeerClassLocal = (*env)->FindClass(env, "JyNI/PyCPeer");
	pyCPeerClass = (jclass) (*env)->NewWeakGlobalRef(env, pyCPeerClassLocal);
	(*env)->DeleteLocalRef(env, pyCPeerClassLocal);
	pyCPeerConstructor = (*env)->GetMethodID(env, pyCPeerClass, "<init>", "(JLorg/python/core/PyType;)V");
	pyCPeerObjectHandle = (*env)->GetFieldID(env, pyCPeerClass, "objectHandle", "J");
	//pyCPeerRefHandle = (*env)->GetFieldID(env, pyCPeerClass, "refHandle", "J");
	jclass pyCPeerTypeClassLocal = (*env)->FindClass(env, "JyNI/PyCPeerType");
	pyCPeerTypeClass = (jclass) (*env)->NewWeakGlobalRef(env, pyCPeerTypeClassLocal);
	(*env)->DeleteLocalRef(env, pyCPeerTypeClassLocal);
	pyCPeerTypeConstructor = (*env)->GetMethodID(env, pyCPeerTypeClass, "<init>", "(J)V");
	pyCPeerTypeObjectHandle = (*env)->GetFieldID(env, pyCPeerTypeClass, "objectHandle", "J");
	pyCPeerTypeRefHandle = (*env)->GetFieldID(env, pyCPeerTypeClass, "refHandle", "J");

	return JNI_VERSION_1_2;
}

inline jint initJythonSite(JNIEnv *env)
{
	jclass pyPyClassLocal = (*env)->FindClass(env, "org/python/core/Py");
	if (pyPyClassLocal == NULL) { return JNI_ERR;}
	pyPyClass = (jclass) (*env)->NewWeakGlobalRef(env, pyPyClassLocal);
	(*env)->DeleteLocalRef(env, pyPyClassLocal);
	pyPyGetThreadState = (*env)->GetStaticMethodID(env, pyPyClass, "getThreadState", "()Lorg/python/core/ThreadState;");
	pyPyGetFrame = (*env)->GetStaticMethodID(env, pyPyClass, "getFrame", "()Lorg/python/core/PyFrame;");
	pyPyIsSubClass = (*env)->GetStaticMethodID(env, pyPyClass, "isSubClass", "(Lorg/python/core/PyObject;Lorg/python/core/PyObject;)Z");
	pyPyWarning = (*env)->GetStaticMethodID(env, pyPyClass, "warning", "(Lorg/python/core/PyObject;Ljava/lang/String;)V");
	pyPyWarningStck = (*env)->GetStaticMethodID(env, pyPyClass, "warning", "(Lorg/python/core/PyObject;Ljava/lang/String;I)V");
	pyPyExplicitWarning = (*env)->GetStaticMethodID(env, pyPyClass, "warning", "(Lorg/python/core/PyObject;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;Lorg/python/core/PyObject;)V");
	pyPyImportError = (*env)->GetStaticFieldID(env, pyPyClass, "ImportError", "Lorg/python/core/PyObject;");
	pyPySystemError = (*env)->GetStaticFieldID(env, pyPyClass, "SystemError", "Lorg/python/core/PyObject;");
	pyPyNewString = (*env)->GetStaticMethodID(env, pyPyClass, "newString", "(Ljava/lang/String;)Lorg/python/core/PyString;");
	pyPyNewUnicode = (*env)->GetStaticMethodID(env, pyPyClass, "newUnicode", "(Ljava/lang/String;)Lorg/python/core/PyUnicode;");
	pyPyNewIntFromInt = (*env)->GetStaticMethodID(env, pyPyClass, "newInteger", "(I)Lorg/python/core/PyInteger;");
	pyPyNewIntFromLong = (*env)->GetStaticMethodID(env, pyPyClass, "newInteger", "(J)Lorg/python/core/PyObject;");
	pyPyNewLongFromBigInt = (*env)->GetStaticMethodID(env, pyPyClass, "newLong", "(Ljava/math/BigInteger;)Lorg/python/core/PyLong;");
	pyPyNewLongFromInt = (*env)->GetStaticMethodID(env, pyPyClass, "newLong", "(I)Lorg/python/core/PyLong;");
	pyPyNewLongFromLong = (*env)->GetStaticMethodID(env, pyPyClass, "newLong", "(J)Lorg/python/core/PyLong;");
	pyPyNewLongFromString = (*env)->GetStaticMethodID(env, pyPyClass, "newLong", "(Ljava/lang/String;)Lorg/python/core/PyLong;");
	pyPyNewFloatFromDouble = (*env)->GetStaticMethodID(env, pyPyClass, "newFloat", "(D)Lorg/python/core/PyFloat;");
	pyPyUnicodeError = (*env)->GetStaticFieldID(env, pyPyClass, "UnicodeError", "Lorg/python/core/PyObject;");
	pyPyUnicodeEncodeError = (*env)->GetStaticFieldID(env, pyPyClass, "UnicodeEncodeError", "Lorg/python/core/PyObject;");
	pyPyUnicodeDecodeError = (*env)->GetStaticFieldID(env, pyPyClass, "UnicodeDecodeError", "Lorg/python/core/PyObject;");
	pyPyUnicodeTranslateError = (*env)->GetStaticFieldID(env, pyPyClass, "UnicodeTranslateError", "Lorg/python/core/PyObject;");
	pyPyUnicodeWarning = (*env)->GetStaticFieldID(env, pyPyClass, "UnicodeWarning", "Lorg/python/core/PyObject;");
	pyPyUnicodeErrorFactory = (*env)->GetStaticMethodID(env, pyPyClass, "UnicodeError", "(Ljava/lang/String;)Lorg/python/core/PyException;");
	pyPyUnicodeEncodeErrorFactory = (*env)->GetStaticMethodID(env, pyPyClass, "UnicodeEncodeError", "(Ljava/lang/String;Ljava/lang/String;IILjava/lang/String;)Lorg/python/core/PyException;");
	pyPyUnicodeDecodeErrorFactory = (*env)->GetStaticMethodID(env, pyPyClass, "UnicodeDecodeError", "(Ljava/lang/String;Ljava/lang/String;IILjava/lang/String;)Lorg/python/core/PyException;");
	pyPyUnicodeTranslateErrorFactory = (*env)->GetStaticMethodID(env, pyPyClass, "UnicodeTranslateError", "(Ljava/lang/String;IILjava/lang/String;)Lorg/python/core/PyException;");
	pyPyRaiseUnicodeWarning = (*env)->GetStaticMethodID(env, pyPyClass, "UnicodeWarning", "(Ljava/lang/String;)V");

	jclass pyTracebackClassLocal = (*env)->FindClass(env, "org/python/core/PyTraceback");
	if (pyTracebackClassLocal == NULL) { return JNI_ERR;}
	pyTracebackClass = (jclass) (*env)->NewWeakGlobalRef(env, pyTracebackClassLocal);
	(*env)->DeleteLocalRef(env, pyTracebackClassLocal);
	pyTracebackByTracebackFrameConstructor = (*env)->GetMethodID(env, pyTracebackClass, "<init>", "(Lorg/python/core/PyTraceback;Lorg/python/core/PyFrame;)V");

	jclass pyExceptionClassLocal = (*env)->FindClass(env, "org/python/core/PyException");
	if (pyExceptionClassLocal == NULL) { return JNI_ERR;}
	pyExceptionClass = (jclass) (*env)->NewWeakGlobalRef(env, pyExceptionClassLocal);
	(*env)->DeleteLocalRef(env, pyExceptionClassLocal);
	pyExceptionTypeField = (*env)->GetFieldID(env, pyExceptionClass, "type", "Lorg/python/core/PyObject;");
	pyExceptionValueField = (*env)->GetFieldID(env, pyExceptionClass, "value", "Lorg/python/core/PyObject;");
	pyExceptionTracebackField = (*env)->GetFieldID(env, pyExceptionClass, "traceback", "Lorg/python/core/PyTraceback;");
	pyExceptionFullConstructor = (*env)->GetMethodID(env, pyExceptionClass, "<init>", "(Lorg/python/core/PyObject;Lorg/python/core/PyObject;Lorg/python/core/PyTraceback;)V");
	pyExceptionTypeValueConstructor = (*env)->GetMethodID(env, pyExceptionClass, "<init>", "(Lorg/python/core/PyObject;Lorg/python/core/PyObject;)V");
	pyExceptionNormalize = (*env)->GetMethodID(env, pyExceptionClass, "normalize", "()V");
	pyExceptionIsExceptionClass = (*env)->GetStaticMethodID(env, pyExceptionClass, "isExceptionClass", "(Lorg/python/core/PyObject;)Z");
	pyExceptionIsExceptionInstance = (*env)->GetStaticMethodID(env, pyExceptionClass, "isExceptionInstance", "(Lorg/python/core/PyObject;)Z");

	jclass pyThreadStateClassLocal = (*env)->FindClass(env, "org/python/core/ThreadState");
	if (pyThreadStateClassLocal == NULL) { return JNI_ERR;}
	pyThreadStateClass = (jclass) (*env)->NewWeakGlobalRef(env, pyThreadStateClassLocal);
	(*env)->DeleteLocalRef(env, pyThreadStateClassLocal);
	pyThreadStateExceptionField = (*env)->GetFieldID(env, pyThreadStateClass, "exception", "Lorg/python/core/PyException;");
	pyThreadStateRecursionDepth = (*env)->GetFieldID(env, pyThreadStateClass, "recursion_depth", "I");
	pyThreadStateEnterRecursiveCall = (*env)->GetMethodID(env, pyThreadStateClass, "enterRecursiveCall", "(Ljava/lang/String;)V");
	pyThreadStateLeaveRecursiveCall = (*env)->GetMethodID(env, pyThreadStateClass, "leaveRecursiveCall", "()V");
	return JNI_VERSION_1_2;
}

inline jint initJythonObjects(JNIEnv *env)
{
	jclass pyNoneClassLocal = (*env)->FindClass(env, "org/python/core/PyNone");
	if (pyNoneClassLocal == NULL) { return JNI_ERR;}
	pyNoneClass = (jclass) (*env)->NewWeakGlobalRef(env, pyNoneClassLocal);
	//jclass pyNoneClass = (*env)->FindClass(env, "org/python/core/PyNone");
	if (pyNoneClass == NULL) { return JNI_ERR;}
	(*env)->DeleteLocalRef(env, pyNoneClassLocal);
	//(*env)->DeleteLocalRef(env, pyNoneConstructor);

	jclass pyObjectClassLocal = (*env)->FindClass(env, "org/python/core/PyObject");
	if (pyObjectClassLocal == NULL) { return JNI_ERR;}
	pyObjectClass = (jclass) (*env)->NewWeakGlobalRef(env, pyObjectClassLocal);
	(*env)->DeleteLocalRef(env, pyObjectClassLocal);
	pyObjectGetType = (*env)->GetMethodID(env, pyObjectClass, "getType", "()Lorg/python/core/PyType;");
	pyObjectAsString = (*env)->GetMethodID(env, pyObjectClass, "asString", "()Ljava/lang/String;");
	pyObjectType = (*env)->GetStaticFieldID(env, pyObjectClass, "TYPE", "Lorg/python/core/PyType;");
	pyObject__getattr__ = (*env)->GetMethodID(env, pyObjectClass, "__getattr__", "(Ljava/lang/String;)Lorg/python/core/PyObject;");
	pyObject__findattr__ = (*env)->GetMethodID(env, pyObjectClass, "__findattr__", "(Ljava/lang/String;)Lorg/python/core/PyObject;");
	pyObject__setattr__ = (*env)->GetMethodID(env, pyObjectClass, "__setattr__", "(Ljava/lang/String;Lorg/python/core/PyObject;)V");
	pyObject__repr__ = (*env)->GetMethodID(env, pyObjectClass, "__repr__", "()Lorg/python/core/PyString;");

	jclass pyInstanceClassLocal = (*env)->FindClass(env, "org/python/core/PyInstance");
	if (pyInstanceClassLocal == NULL) { return JNI_ERR;}
	pyInstanceClass = (jclass) (*env)->NewWeakGlobalRef(env, pyInstanceClassLocal);
	(*env)->DeleteLocalRef(env, pyInstanceClassLocal);
	pyInstanceInstclassField = (*env)->GetFieldID(env, pyInstanceClass, "instclass", "Lorg/python/core/PyClass;");
	pyInstanceIsSequenceType = (*env)->GetMethodID(env, pyInstanceClass, "isSequenceType", "()Z");

	jclass pyFrameClassLocal = (*env)->FindClass(env, "org/python/core/PyFrame");
	if (pyFrameClassLocal == NULL) { return JNI_ERR;}
	pyFrameClass = (jclass) (*env)->NewWeakGlobalRef(env, pyFrameClassLocal);
	(*env)->DeleteLocalRef(env, pyFrameClassLocal);

	jclass pyBooleanClassLocal = (*env)->FindClass(env, "org/python/core/PyBoolean");
	if (pyBooleanClassLocal == NULL) { return JNI_ERR;}
	pyBooleanClass = (jclass) (*env)->NewWeakGlobalRef(env, pyBooleanClassLocal);
	(*env)->DeleteLocalRef(env, pyBooleanClassLocal);
	pyBooleanConstructor = (*env)->GetMethodID(env, pyBooleanClass, "<init>", "(Z)V");

	jclass pyIntClassLocal = (*env)->FindClass(env, "org/python/core/PyInteger");
	if (pyIntClassLocal == NULL) { return JNI_ERR;}
	pyIntClass = (jclass) (*env)->NewWeakGlobalRef(env, pyIntClassLocal);
	(*env)->DeleteLocalRef(env, pyIntClassLocal);
	pyIntConstructor = (*env)->GetMethodID(env, pyIntClass, "<init>", "(I)V");
	pyIntAsInt = (*env)->GetMethodID(env, pyIntClass, "asInt", "()I");
	pyIntAsLong = (*env)->GetMethodID(env, pyIntClass, "asLong", "()J");

	jclass pyLongClassLocal = (*env)->FindClass(env, "org/python/core/PyLong");
	if (pyLongClassLocal == NULL) { return JNI_ERR;}
	pyLongClass = (jclass) (*env)->NewWeakGlobalRef(env, pyLongClassLocal);
	(*env)->DeleteLocalRef(env, pyLongClassLocal);
	pyLongByLongConstructor = (*env)->GetMethodID(env, pyLongClass, "<init>", "(J)V");
	pyLongByBigIntConstructor = (*env)->GetMethodID(env, pyLongClass, "<init>", "(Ljava/math/BigInteger;)V");
	pyLongAsLong = (*env)->GetMethodID(env, pyLongClass, "asLong", "()J");
	pyLongGetLong = (*env)->GetMethodID(env, pyLongClass, "getLong", "(JJ)J");
	pyLongGetValue = (*env)->GetMethodID(env, pyLongClass, "getValue", "()Ljava/math/BigInteger;");
	pyLongBit_length = (*env)->GetMethodID(env, pyLongClass, "bit_length", "()I");
	//pyLongToString = (*env)->GetMethodID(env, pyLongClass, "toString", "()Ljava/lang/String;");

	jclass pyUnicodeClassLocal = (*env)->FindClass(env, "org/python/core/PyUnicode");
	if (pyUnicodeClassLocal == NULL) { return JNI_ERR;}
	pyUnicodeClass = (jclass) (*env)->NewWeakGlobalRef(env, pyUnicodeClassLocal);
	(*env)->DeleteLocalRef(env, pyUnicodeClassLocal);
	pyUnicodeByJStringConstructor = (*env)->GetMethodID(env, pyUnicodeClass, "<init>", "(Ljava/lang/String;)V");
	pyUnicodeAsString = (*env)->GetMethodID(env, pyUnicodeClass, "asString", "()Ljava/lang/String;");

	jclass pyStringClassLocal = (*env)->FindClass(env, "org/python/core/PyString");
	if (pyStringClassLocal == NULL) { return JNI_ERR;}
	pyStringClass = (jclass) (*env)->NewWeakGlobalRef(env, pyStringClassLocal);
	(*env)->DeleteLocalRef(env, pyStringClassLocal);
	pyStringByJStringConstructor = (*env)->GetMethodID(env, pyStringClass, "<init>", "(Ljava/lang/String;)V");
	pyStringAsString = (*env)->GetMethodID(env, pyStringClass, "asString", "()Ljava/lang/String;");

	jclass pyFloatClassLocal = (*env)->FindClass(env, "org/python/core/PyFloat");
	if (pyFloatClassLocal == NULL) { return JNI_ERR;}
	pyFloatClass = (jclass) (*env)->NewWeakGlobalRef(env, pyFloatClassLocal);
	(*env)->DeleteLocalRef(env, pyFloatClassLocal);
	pyFloatByDoubleConstructor = (*env)->GetMethodID(env, pyFloatClass, "<init>", "(D)V");
	pyFloatAsDouble = (*env)->GetMethodID(env, pyFloatClass, "asDouble", "()D");
	pyFloatTypeField = (*env)->GetStaticFieldID(env, pyFloatClass, "TYPE", "Lorg/python/core/PyType;");

	jclass pyComplexClassLocal = (*env)->FindClass(env, "org/python/core/PyComplex");
	if (pyComplexClassLocal == NULL) { return JNI_ERR;}
	pyComplexClass = (jclass) (*env)->NewWeakGlobalRef(env, pyComplexClassLocal);
	(*env)->DeleteLocalRef(env, pyComplexClassLocal);
	pyComplexBy2DoubleConstructor = (*env)->GetMethodID(env, pyComplexClass, "<init>", "(DD)V");
	//jfieldID pyComplexRealField = (*env)->GetFieldID(env, pyComplexClass, "real", "D");
	//jfieldID pyComplexImagField = (*env)->GetFieldID(env, pyComplexClass, "imag", "D");

	jclass pyArrayClassLocal = (*env)->FindClass(env, "org/python/core/PyArray");
	if (pyArrayClassLocal == NULL) { return JNI_ERR;}
	pyArrayClass = (jclass) (*env)->NewWeakGlobalRef(env, pyArrayClassLocal);
	(*env)->DeleteLocalRef(env, pyArrayClassLocal);
	pyArrayGetTypecode = (*env)->GetMethodID(env, pyArrayClass, "getTypecode", "()Ljava/lang/String;");

	jclass pyTupleClassLocal = (*env)->FindClass(env, "org/python/core/PyTuple");
	if (pyTupleClassLocal == NULL) { return JNI_ERR;}
	pyTupleClass = (jclass) (*env)->NewWeakGlobalRef(env, pyTupleClassLocal);
	(*env)->DeleteLocalRef(env, pyTupleClassLocal);
	pyTupleConstructor = (*env)->GetMethodID(env, pyTupleClass, "<init>", "()V");
	pyTupleByPyObjectArrayBooleanConstructor = (*env)->GetMethodID(env, pyTupleClass, "<init>", "([Lorg/python/core/PyObject;Z)V");
	pyTupleSize = (*env)->GetMethodID(env, pyTupleClass, "size", "()I");
	pyTuplePyGet = (*env)->GetMethodID(env, pyTupleClass, "pyget", "(I)Lorg/python/core/PyObject;");
	//pyTupleGetArray = (*env)->GetMethodID(env, pyTupleClass, "getArray", "()[Lorg/python/core/PyObject;");

	jclass pyListClassLocal = (*env)->FindClass(env, "org/python/core/PyList");
	if (pyListClassLocal == NULL) { return JNI_ERR;}
	pyListClass = (jclass) (*env)->NewWeakGlobalRef(env, pyListClassLocal);
	(*env)->DeleteLocalRef(env, pyListClassLocal);
	pyListConstructor = (*env)->GetMethodID(env, pyListClass, "<init>", "()V");
	pyListByPyObjectArrayConstructor = (*env)->GetMethodID(env, pyListClass, "<init>", "([Lorg/python/core/PyObject;)V");
	pyListFromList = (*env)->GetStaticMethodID(env, pyListClass, "fromList", "(Ljava/util/List;)Lorg/python/core/PyList;");
	pyListSize = (*env)->GetMethodID(env, pyListClass, "size", "()I");
	pyListPyGet = (*env)->GetMethodID(env, pyListClass, "pyget", "(I)Lorg/python/core/PyObject;");
	pyListPySet = (*env)->GetMethodID(env, pyListClass, "pyset", "(ILorg/python/core/PyObject;)V");
	pyListAppend = (*env)->GetMethodID(env, pyListClass, "append", "(Lorg/python/core/PyObject;)V");
	pyListInsert = (*env)->GetMethodID(env, pyListClass, "insert", "(ILorg/python/core/PyObject;)V");
	pyListSort = (*env)->GetMethodID(env, pyListClass, "sort", "()V");
	pyListReverse = (*env)->GetMethodID(env, pyListClass, "reverse", "()V");
	pyListGetArray = (*env)->GetMethodID(env, pyListClass, "getArray", "()[Lorg/python/core/PyObject;");


	jclass pyDictClassLocal = (*env)->FindClass(env, "org/python/core/PyDictionary");
	if (pyDictClassLocal == NULL) { return JNI_ERR;}
	pyDictClass = (jclass) (*env)->NewWeakGlobalRef(env, pyDictClassLocal);
	(*env)->DeleteLocalRef(env, pyDictClassLocal);
	pyDictConstructor = (*env)->GetMethodID(env, pyDictClass, "<init>", "()V");
	pyDictByPyObjectArrayConstructor = (*env)->GetMethodID(env, pyDictClass, "<init>", "([Lorg/python/core/PyObject;)V");
	pyDictGet_PyObject = (*env)->GetMethodID(env, pyDictClass, "get", "(Lorg/python/core/PyObject;)Lorg/python/core/PyObject;");
	pyDictGet_PyObjectWithDefault = (*env)->GetMethodID(env, pyDictClass, "get", "(Lorg/python/core/PyObject;Lorg/python/core/PyObject;)Lorg/python/core/PyObject;");
	pyDict__setitem__ = (*env)->GetMethodID(env, pyDictClass, "__setitem__", "(Lorg/python/core/PyObject;Lorg/python/core/PyObject;)V");
	pyDict__delitem__ = (*env)->GetMethodID(env, pyDictClass, "__delitem__", "(Lorg/python/core/PyObject;)V");
	pyDictClear = (*env)->GetMethodID(env, pyDictClass, "clear", "()V");
	pyDictSize = (*env)->GetMethodID(env, pyDictClass, "size", "()I");

	jclass pySequenceClassLocal = (*env)->FindClass(env, "org/python/core/PySequence");
	if (pySequenceClassLocal == NULL) { return JNI_ERR;}
	pySequenceClass = (jclass) (*env)->NewWeakGlobalRef(env, pySequenceClassLocal);
	(*env)->DeleteLocalRef(env, pySequenceClassLocal);
	pySequenceLen = (*env)->GetMethodID(env, pySequenceClass, "__len__", "()I");
	pySequenceGetItem = (*env)->GetMethodID(env, pySequenceClass, "__finditem__", "(I)Lorg/python/core/PyObject;");

	jclass pySequenceListClassLocal = (*env)->FindClass(env, "org/python/core/PySequenceList");
	if (pySequenceListClassLocal == NULL) { return JNI_ERR;}
	pySequenceListClass = (jclass) (*env)->NewWeakGlobalRef(env, pySequenceListClassLocal);
	(*env)->DeleteLocalRef(env, pySequenceListClassLocal);
	pySequenceListSize = (*env)->GetMethodID(env, pySequenceListClass, "size", "()I");
	pySequenceListPyGet = (*env)->GetMethodID(env, pySequenceListClass, "pyget", "(I)Lorg/python/core/PyObject;");

	jclass pyTypeClassLocal = (*env)->FindClass(env, "org/python/core/PyType");
	if (pyTypeClassLocal == NULL) { return JNI_ERR;}
	pyTypeClass = (jclass) (*env)->NewWeakGlobalRef(env, pyTypeClassLocal);
	(*env)->DeleteLocalRef(env, pyTypeClassLocal);
	pyTypeGetName = (*env)->GetMethodID(env, pyTypeClass, "getName", "()Ljava/lang/String;");
	pyTypeIsSubType = (*env)->GetMethodID(env, pyTypeClass, "isSubType", "(Lorg/python/core/PyType;)Z");

	jclass pyCodecsClassLocal = (*env)->FindClass(env, "org/python/core/codecs");
	if (pyCodecsClassLocal == NULL) { return JNI_ERR;}
	pyCodecsClass = (jclass) (*env)->NewWeakGlobalRef(env, pyCodecsClassLocal);
	(*env)->DeleteLocalRef(env, pyCodecsClassLocal);
	pyCodecsDecode = (*env)->GetStaticMethodID(env, pyCodecsClass, "decode", "(Lorg/python/core/PyString;Ljava/lang/String;Ljava/lang/String;)Lorg/python/core/PyObject;");
	pyCodecsEncode = (*env)->GetStaticMethodID(env, pyCodecsClass, "encode", "(Lorg/python/core/PyString;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
	pyCodecsGetDefaultEncoding = (*env)->GetStaticMethodID(env, pyCodecsClass, "getDefaultEncoding", "()Ljava/lang/String;");
	pyCodecsLookup = (*env)->GetStaticMethodID(env, pyCodecsClass, "lookup", "(Ljava/lang/String;)Lorg/python/core/PyTuple;");
	pyCodecsRegister = (*env)->GetStaticMethodID(env, pyCodecsClass, "register", "(Lorg/python/core/PyObject;)V");
	pyCodecsLookupError = (*env)->GetStaticMethodID(env, pyCodecsClass, "lookup_error", "(Ljava/lang/String;)Lorg/python/core/PyObject;");
	pyCodecsRegisterError = (*env)->GetStaticMethodID(env, pyCodecsClass, "register_error", "(Ljava/lang/String;Lorg/python/core/PyObject;)V");

	jclass pyNotImplementedClassLocal = (*env)->FindClass(env, "org/python/core/PyNotImplemented");
	if (pyNotImplementedClassLocal == NULL) { return JNI_ERR;}
	pyNotImplementedClass = (jclass) (*env)->NewWeakGlobalRef(env, pyNotImplementedClassLocal);
	(*env)->DeleteLocalRef(env, pyNotImplementedClassLocal);

	jclass pyFileClassLocal = (*env)->FindClass(env, "org/python/core/PyFile");
	if (pyFileClassLocal == NULL) { return JNI_ERR;}
	pyFileClass = (jclass) (*env)->NewWeakGlobalRef(env, pyFileClassLocal);
	(*env)->DeleteLocalRef(env, pyFileClassLocal);
	pyFileWrite = (*env)->GetMethodID(env, pyFileClass, "write", "(Ljava/lang/String;)V");

	jclass pyModuleClassLocal = (*env)->FindClass(env, "org/python/core/PyModule");
	if (pyModuleClassLocal == NULL) { return JNI_ERR;}
	pyModuleClass = (jclass) (*env)->NewWeakGlobalRef(env, pyModuleClassLocal);
	(*env)->DeleteLocalRef(env, pyModuleClassLocal);
	pyModuleByStringConstructor = (*env)->GetMethodID(env, pyModuleClass, "<init>", "(Ljava/lang/String;)V");
	pyModuleGetDict = (*env)->GetMethodID(env, pyModuleClass, "getDict", "()Lorg/python/core/PyObject;");
	pyModule__setattr__ = (*env)->GetMethodID(env, pyModuleClass, "__setattr__", "(Ljava/lang/String;Lorg/python/core/PyObject;)V");
	pyModule__delattr__ = (*env)->GetMethodID(env, pyModuleClass, "__delattr__", "(Ljava/lang/String;)V");

	jclass pyCellClassLocal = (*env)->FindClass(env, "org/python/core/PyCell");
	if (pyCellClassLocal == NULL) { return JNI_ERR;}
	pyCellClass = (jclass) (*env)->NewWeakGlobalRef(env, pyCellClassLocal);
	(*env)->DeleteLocalRef(env, pyCellClassLocal);

	jclass pyClassClassLocal = (*env)->FindClass(env, "org/python/core/PyClass");
	if (pyClassClassLocal == NULL) { return JNI_ERR;}
	pyClassClass = (jclass) (*env)->NewWeakGlobalRef(env, pyClassClassLocal);
	(*env)->DeleteLocalRef(env, pyClassClassLocal);

	jclass pyMethodClassLocal = (*env)->FindClass(env, "org/python/core/PyMethod");
	if (pyMethodClassLocal == NULL) { return JNI_ERR;}
	pyMethodClass = (jclass) (*env)->NewWeakGlobalRef(env, pyMethodClassLocal);
	(*env)->DeleteLocalRef(env, pyMethodClassLocal);

	jclass pyFunctionClassLocal = (*env)->FindClass(env, "org/python/core/PyFunction");
	if (pyFunctionClassLocal == NULL) { return JNI_ERR;}
	pyFunctionClass = (jclass) (*env)->NewWeakGlobalRef(env, pyFunctionClassLocal);
	(*env)->DeleteLocalRef(env, pyFunctionClassLocal);

	jclass pyClassMethodClassLocal = (*env)->FindClass(env, "org/python/core/PyClassMethod");
	if (pyClassMethodClassLocal == NULL) { return JNI_ERR;}
	pyClassMethodClass = (jclass) (*env)->NewWeakGlobalRef(env, pyClassMethodClassLocal);
	(*env)->DeleteLocalRef(env, pyClassMethodClassLocal);

	jclass pyStaticMethodClassLocal = (*env)->FindClass(env, "org/python/core/PyStaticMethod");
	if (pyStaticMethodClassLocal == NULL) { return JNI_ERR;}
	pyStaticMethodClass = (jclass) (*env)->NewWeakGlobalRef(env, pyStaticMethodClassLocal);
	(*env)->DeleteLocalRef(env, pyStaticMethodClassLocal);

	jclass pyMethodDescrClassLocal = (*env)->FindClass(env, "org/python/core/PyMethodDescr");
	if (pyMethodDescrClassLocal == NULL) { return JNI_ERR;}
	pyMethodDescrClass = (jclass) (*env)->NewWeakGlobalRef(env, pyMethodDescrClassLocal);
	(*env)->DeleteLocalRef(env, pyMethodDescrClassLocal);

	jclass pyClassMethodDescrClassLocal = (*env)->FindClass(env, "org/python/core/PyClassMethodDescr");
	if (pyClassMethodDescrClassLocal == NULL) { return JNI_ERR;}
	pyClassMethodDescrClass = (jclass) (*env)->NewWeakGlobalRef(env, pyClassMethodDescrClassLocal);
	(*env)->DeleteLocalRef(env, pyClassMethodDescrClassLocal);

	jclass pyDictProxyClassLocal = (*env)->FindClass(env, "org/python/core/PyDictProxy");
	if (pyDictProxyClassLocal == NULL) { return JNI_ERR;}
	pyDictProxyClass = (jclass) (*env)->NewWeakGlobalRef(env, pyDictProxyClassLocal);
	(*env)->DeleteLocalRef(env, pyDictProxyClassLocal);

	jclass pyPropertyClassLocal = (*env)->FindClass(env, "org/python/core/PyProperty");
	if (pyPropertyClassLocal == NULL) { return JNI_ERR;}
	pyPropertyClass = (jclass) (*env)->NewWeakGlobalRef(env, pyPropertyClassLocal);
	(*env)->DeleteLocalRef(env, pyPropertyClassLocal);

	jclass pyBaseStringClassLocal = (*env)->FindClass(env, "org/python/core/PyBaseString");
	if (pyBaseStringClassLocal == NULL) { return JNI_ERR;}
	pyBaseStringClass = (jclass) (*env)->NewWeakGlobalRef(env, pyBaseStringClassLocal);
	(*env)->DeleteLocalRef(env, pyBaseStringClassLocal);

	jclass pyXRangeClassLocal = (*env)->FindClass(env, "org/python/core/PyXRange");
	if (pyXRangeClassLocal == NULL) { return JNI_ERR;}
	pyXRangeClass = (jclass) (*env)->NewWeakGlobalRef(env, pyXRangeClassLocal);
	(*env)->DeleteLocalRef(env, pyXRangeClassLocal);

	jclass pySequenceIterClassLocal = (*env)->FindClass(env, "org/python/core/PySequenceIter");
	if (pySequenceIterClassLocal == NULL) { return JNI_ERR;}
	pySequenceIterClass = (jclass) (*env)->NewWeakGlobalRef(env, pySequenceIterClassLocal);
	(*env)->DeleteLocalRef(env, pySequenceIterClassLocal);

	jclass pyFastSequenceIterClassLocal = (*env)->FindClass(env, "org/python/core/PyFastSequenceIter");
	if (pyFastSequenceIterClassLocal == NULL) { return JNI_ERR;}
	pyFastSequenceIterClass = (jclass) (*env)->NewWeakGlobalRef(env, pyFastSequenceIterClassLocal);
	(*env)->DeleteLocalRef(env, pyFastSequenceIterClassLocal);

	jclass pyReversedIteratorClassLocal = (*env)->FindClass(env, "org/python/core/PyReversedIterator");
	if (pyReversedIteratorClassLocal == NULL) { return JNI_ERR;}
	pyReversedIteratorClass = (jclass) (*env)->NewWeakGlobalRef(env, pyReversedIteratorClassLocal);
	(*env)->DeleteLocalRef(env, pyReversedIteratorClassLocal);

	jclass pySetClassLocal = (*env)->FindClass(env, "org/python/core/PySet");
	if (pySetClassLocal == NULL) { return JNI_ERR;}
	pySetClass = (jclass) (*env)->NewWeakGlobalRef(env, pySetClassLocal);
	(*env)->DeleteLocalRef(env, pySetClassLocal);

	jclass pyFrozenSetClassLocal = (*env)->FindClass(env, "org/python/core/PyFrozenSet");
	if (pyFrozenSetClassLocal == NULL) { return JNI_ERR;}
	pyFrozenSetClass = (jclass) (*env)->NewWeakGlobalRef(env, pyFrozenSetClassLocal);
	(*env)->DeleteLocalRef(env, pyFrozenSetClassLocal);

	jclass pyEnumerationClassLocal = (*env)->FindClass(env, "org/python/core/PyEnumerate");
	if (pyEnumerationClassLocal == NULL) { return JNI_ERR;}
	pyEnumerationClass = (jclass) (*env)->NewWeakGlobalRef(env, pyEnumerationClassLocal);
	(*env)->DeleteLocalRef(env, pyEnumerationClassLocal);

	jclass pySliceClassLocal = (*env)->FindClass(env, "org/python/core/PySlice");
	if (pySliceClassLocal == NULL) { return JNI_ERR;}
	pySliceClass = (jclass) (*env)->NewWeakGlobalRef(env, pySliceClassLocal);
	(*env)->DeleteLocalRef(env, pySliceClassLocal);
	pySliceFromStartStopStepConstructor = (*env)->GetMethodID(env, pySliceClass, "<init>", "(Lorg/python/core/PyObject;Lorg/python/core/PyObject;Lorg/python/core/PyObject;)V");
	pySliceToString = (*env)->GetMethodID(env, pySliceClass, "toString", "()Ljava/lang/String;");
	pySliceGetStart = (*env)->GetMethodID(env, pySliceClass, "getStart", "()Lorg/python/core/PyObject;");
	pySliceGetStop = (*env)->GetMethodID(env, pySliceClass, "getStop", "()Lorg/python/core/PyObject;");
	pySliceGetStep = (*env)->GetMethodID(env, pySliceClass, "getStep", "()Lorg/python/core/PyObject;");
	pySliceIndicesEx = (*env)->GetMethodID(env, pySliceClass, "indicesEx", "(I)[I");

	jclass pyEllipsisClassLocal = (*env)->FindClass(env, "org/python/core/PyEllipsis");
	if (pyEllipsisClassLocal == NULL) { return JNI_ERR;}
	pyEllipsisClass = (jclass) (*env)->NewWeakGlobalRef(env, pyEllipsisClassLocal);
	(*env)->DeleteLocalRef(env, pyEllipsisClassLocal);

	jclass pyGeneratorClassLocal = (*env)->FindClass(env, "org/python/core/PyGenerator");
	if (pyGeneratorClassLocal == NULL) { return JNI_ERR;}
	pyGeneratorClass = (jclass) (*env)->NewWeakGlobalRef(env, pyGeneratorClassLocal);
	(*env)->DeleteLocalRef(env, pyGeneratorClassLocal);

	jclass pyCodeClassLocal = (*env)->FindClass(env, "org/python/core/PyCode");
	if (pyCodeClassLocal == NULL) { return JNI_ERR;}
	pyCodeClass = (jclass) (*env)->NewWeakGlobalRef(env, pyCodeClassLocal);
	(*env)->DeleteLocalRef(env, pyCodeClassLocal);

	jclass pyCallIterClassLocal = (*env)->FindClass(env, "org/python/core/PyCallIter");
	if (pyCallIterClassLocal == NULL) { return JNI_ERR;}
	pyCallIterClass = (jclass) (*env)->NewWeakGlobalRef(env, pyCallIterClassLocal);
	(*env)->DeleteLocalRef(env, pyCallIterClassLocal);

	jclass pySuperClassLocal = (*env)->FindClass(env, "org/python/core/PySuper");
	if (pySuperClassLocal == NULL) { return JNI_ERR;}
	pySuperClass = (jclass) (*env)->NewWeakGlobalRef(env, pySuperClassLocal);
	(*env)->DeleteLocalRef(env, pySuperClassLocal);

	jclass pyBaseExceptionClassLocal = (*env)->FindClass(env, "org/python/core/PyBaseException");
	if (pyBaseExceptionClassLocal == NULL) { return JNI_ERR;}
	pyBaseExceptionClass = (jclass) (*env)->NewWeakGlobalRef(env, pyBaseExceptionClassLocal);
	(*env)->DeleteLocalRef(env, pyBaseExceptionClassLocal);

	jclass pyByteArrayClassLocal = (*env)->FindClass(env, "org/python/core/PyByteArray");
	if (pyByteArrayClassLocal == NULL) { return JNI_ERR;}
	pyByteArrayClass = (jclass) (*env)->NewWeakGlobalRef(env, pyByteArrayClassLocal);
	(*env)->DeleteLocalRef(env, pyByteArrayClassLocal);

	jclass pyBufferClassLocal = (*env)->FindClass(env, "org/python/core/PyBuffer");
	if (pyBufferClassLocal == NULL) { return JNI_ERR;}
	pyBufferClass = (jclass) (*env)->NewWeakGlobalRef(env, pyBufferClassLocal);
	(*env)->DeleteLocalRef(env, pyBufferClassLocal);

	jclass pyMemoryViewClassLocal = (*env)->FindClass(env, "org/python/core/PyMemoryView");
	if (pyMemoryViewClassLocal == NULL) { return JNI_ERR;}
	pyMemoryViewClass = (jclass) (*env)->NewWeakGlobalRef(env, pyMemoryViewClassLocal);
	(*env)->DeleteLocalRef(env, pyMemoryViewClassLocal);

	return JNI_VERSION_1_2;
}

inline jint initSingletons(JNIEnv *env)
{
	//PyTrue = JyNI_PyObject_FromJythonPyObject((*env)->NewWeakGlobalRef(env, (*env)->NewObject(env, pyBooleanClass, pyBooleanConstructor, JNI_TRUE)));
	//PyFalse = JyNI_PyObject_FromJythonPyObject((*env)->NewWeakGlobalRef(env, (*env)->NewObject(env, pyBooleanClass, pyBooleanConstructor, JNI_FALSE)));

	/*jmethodID pyUnicodeEmptyConstructor = (*env)->GetMethodID(env, pyUnicodeClass, "<init>", "()V");
	jobject unicode_emptyLocal = (*env)->NewObject(env, pyUnicodeClass, pyUnicodeEmptyConstructor);
	unicode_empty = JyNI_PyObject_FromJythonPyObject((*env)->NewWeakGlobalRef(env, unicode_emptyLocal));
	(*env)->DeleteLocalRef(env, unicode_emptyLocal);
	*/
	//jmethodID pyNoneConstructor = (*env)->GetMethodID(env, pyNoneClass, "<init>", "()V");
	//if (pyNoneConstructor == NULL) { return JNI_ERR;}
	//JyNone = JyNI_PyObject_FromJythonPyObject((*env)->NewWeakGlobalRef(env, (*env)->NewObject(env, pyNoneClass, pyNoneConstructor)));
	jfieldID jyNone = (*env)->GetStaticFieldID(env, pyPyClass, "None", "Lorg/python/core/PyObject;");
	JyNone = (*env)->NewGlobalRef(env, (*env)->GetStaticObjectField(env, pyPyClass, jyNone));
	jfieldID jyNotImplemented = (*env)->GetStaticFieldID(env, pyPyClass, "NotImplemented", "Lorg/python/core/PyObject;");
	JyNotImplemented = (*env)->NewGlobalRef(env, (*env)->GetStaticObjectField(env, pyPyClass, jyNotImplemented));
	jfieldID jyEllipsis = (*env)->GetStaticFieldID(env, pyPyClass, "Ellipsis", "Lorg/python/core/PyObject;");
	JyEllipsis = (*env)->NewGlobalRef(env, (*env)->GetStaticObjectField(env, pyPyClass, jyEllipsis));
	return JNI_VERSION_1_2;
}

//static PyStringObject *characters2[UCHAR_MAX + 1];

//JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
jint JyNI_init(JavaVM *jvm)
{
	jdbg puts("JyNI_init");

	//cout << "Hier OnLoad7!" << endl;
	java = jvm; // cache the JavaVM pointer
	JNIEnv *env;
//	c++
//	if (jvm->GetEnv(java, (void **)&env, JNI_VERSION_1_2)) {
//		return JNI_ERR; // JNI version not supported
//	}

	if ((*jvm)->GetEnv(jvm, (void **)&env, JNI_VERSION_1_2)) {
		return JNI_ERR; // JNI version not supported
	}
	//Py_Py3kWarningFlag
	if (initJNI(env) == JNI_ERR) return JNI_ERR;
	if (initJyNI(env) == JNI_ERR) return JNI_ERR;
	if (initJythonSite(env) == JNI_ERR) return JNI_ERR;
	if (initJythonObjects(env) == JNI_ERR) return JNI_ERR;
	initBuiltinTypes();
	initBuiltinExceptions();
	if (initSingletons(env) == JNI_ERR) return JNI_ERR;


	//puts("characters-info:");
	//characters[UCHAR_MAX + 1]
	//characters2[0] = NULL;
	//printf("%u\n", characters2[0]);
	//dlopen("/home/stefan/eclipseWorkspace/JyNI/JyNI-C/Debug/libJyNI.so", RTLD_NOLOAD | RTLD_GLOBAL);
	//initErrorsFromJython(env);

	//init native objects where necessary:
	_PyInt_Init();

	return JNI_VERSION_1_2;
}

void JyNI_unload(JavaVM *jvm)
{
	puts("JyNI_unload");
	PyString_Fini();
	PyInt_Fini();
	PyTuple_Fini();
	PyDict_Fini();
	PyCFunction_Fini();


	int i;
	for (i = 0; i < builtinTypeCount; ++i)
	{
		if (builtinTypes[i].sync != NULL) free(builtinTypes[i].sync);
	}

	env();
	(*env)->DeleteGlobalRef(env, JyNone);
	(*env)->DeleteGlobalRef(env, JyNotImplemented);
	(*env)->DeleteGlobalRef(env, JyEllipsis);
}