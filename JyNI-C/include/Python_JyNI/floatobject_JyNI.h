/* This File is based on floatobject.h from CPython 2.7.3 release.
 * It has been modified to suite JyNI needs.
 *
 * Copyright of the original file:
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010,
 * 2011, 2012, 2013 Python Software Foundation.  All rights reserved.
 *
 * Copyright of JyNI:
 * Copyright (c) 2013 Stefan Richthofer.  All rights reserved.
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


/* Float object interface */

/*
PyFloatObject represents a (double precision) floating point number.
*/

#ifndef Py_FLOATOBJECT_H
#define Py_FLOATOBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    PyObject_HEAD
    double ob_fval;
} PyFloatObject;

PyAPI_DATA(PyTypeObject) PyFloat_Type;

#define PyFloat_Check(op) PyObject_TypeCheck(op, &PyFloat_Type)
#define PyFloat_CheckExact(op) (Py_TYPE(op) == &PyFloat_Type)

/* The str() precision PyFloat_STR_PRECISION is chosen so that in most cases,
   the rounding noise created by various operations is suppressed, while
   giving plenty of precision for practical use. */

#define PyFloat_STR_PRECISION 12

#ifdef Py_NAN
#define Py_RETURN_NAN return PyFloat_FromDouble(Py_NAN)
#endif

#define Py_RETURN_INF(sign) do					\
	if (copysign(1., sign) == 1.) {				\
		return PyFloat_FromDouble(Py_HUGE_VAL);	\
	} else {						\
		return PyFloat_FromDouble(-Py_HUGE_VAL);	\
	} while(0)

PyAPI_FUNC(double) PyFloat_GetMax(void);
PyAPI_FUNC(double) PyFloat_GetMin(void);
PyAPI_FUNC(PyObject *) PyFloat_GetInfo(void);

/* Return Python float from string PyObject.  Second argument ignored on
   input, and, if non-NULL, NULL is stored into *junk (this tried to serve a
   purpose once but can't be made to work as intended). */
PyAPI_FUNC(PyObject *) PyFloat_FromString(PyObject*, char** junk);

/* Return Python float from C double. */
PyAPI_FUNC(PyObject *) PyFloat_FromDouble(double);

/* Extract C double from Python float.  The macro version trades safety for
   speed. */
PyAPI_FUNC(double) PyFloat_AsDouble(PyObject *);
#define PyFloat_AS_DOUBLE(op) (((PyFloatObject *)(op))->ob_fval)

/* Write repr(v) into the char buffer argument, followed by null byte.  The
   buffer must be "big enough"; >= 100 is very safe.
   PyFloat_AsReprString(buf, x) strives to print enough digits so that
   PyFloat_FromString(buf) then reproduces x exactly. */
PyAPI_FUNC(void) PyFloat_AsReprString(char*, PyFloatObject *v);

/* Write str(v) into the char buffer argument, followed by null byte.  The
   buffer must be "big enough"; >= 100 is very safe.  Note that it's
   unusual to be able to get back the float you started with from
   PyFloat_AsString's result -- use PyFloat_AsReprString() if you want to
   preserve precision across conversions. */
PyAPI_FUNC(void) PyFloat_AsString(char*, PyFloatObject *v);

/* _PyFloat_{Pack,Unpack}{4,8}
 *
 * The struct and pickle (at least) modules need an efficient platform-
 * independent way to store floating-point values as byte strings.
 * The Pack routines produce a string from a C double, and the Unpack
 * routines produce a C double from such a string.  The suffix (4 or 8)
 * specifies the number of bytes in the string.
 *
 * On platforms that appear to use (see _PyFloat_Init()) IEEE-754 formats
 * these functions work by copying bits.  On other platforms, the formats the
 * 4- byte format is identical to the IEEE-754 single precision format, and
 * the 8-byte format to the IEEE-754 double precision format, although the
 * packing of INFs and NaNs (if such things exist on the platform) isn't
 * handled correctly, and attempting to unpack a string containing an IEEE
 * INF or NaN will raise an exception.
 *
 * On non-IEEE platforms with more precision, or larger dynamic range, than
 * 754 supports, not all values can be packed; on non-IEEE platforms with less
 * precision, or smaller dynamic range, not all values can be unpacked.  What
 * happens in such cases is partly accidental (alas).
 */

/* The pack routines write 4 or 8 bytes, starting at p.  le is a bool
 * argument, true if you want the string in little-endian format (exponent
 * last, at p+3 or p+7), false if you want big-endian format (exponent
 * first, at p).
 * Return value:  0 if all is OK, -1 if error (and an exception is
 * set, most likely OverflowError).
 * There are two problems on non-IEEE platforms:
 * 1):  What this does is undefined if x is a NaN or infinity.
 * 2):  -0.0 and +0.0 produce the same string.
 */
PyAPI_FUNC(int) _PyFloat_Pack4(double x, unsigned char *p, int le);
PyAPI_FUNC(int) _PyFloat_Pack8(double x, unsigned char *p, int le);

/* Used to get the important decimal digits of a double */
PyAPI_FUNC(int) _PyFloat_Digits(char *buf, double v, int *signum);
PyAPI_FUNC(void) _PyFloat_DigitsInit(void);

/* The unpack routines read 4 or 8 bytes, starting at p.  le is a bool
 * argument, true if the string is in little-endian format (exponent
 * last, at p+3 or p+7), false if big-endian (exponent first, at p).
 * Return value:  The unpacked double.  On error, this is -1.0 and
 * PyErr_Occurred() is true (and an exception is set, most likely
 * OverflowError).  Note that on a non-IEEE platform this will refuse
 * to unpack a string that represents a NaN or infinity.
 */
PyAPI_FUNC(double) _PyFloat_Unpack4(const unsigned char *p, int le);
PyAPI_FUNC(double) _PyFloat_Unpack8(const unsigned char *p, int le);

/* free list api */
PyAPI_FUNC(int) PyFloat_ClearFreeList(void);

/* Format the object based on the format_spec, as defined in PEP 3101
   (Advanced String Formatting). */
PyAPI_FUNC(PyObject *) _PyFloat_FormatAdvanced(PyObject *obj,
					       char *format_spec,
					       Py_ssize_t format_spec_len);

/* Round a C double x to the closest multiple of 10**-ndigits.  Returns a
   Python float on success, or NULL (with an appropriate exception set) on
   failure.  Used in builtin_round in bltinmodule.c. */
PyAPI_FUNC(PyObject *) _Py_double_round(double x, int ndigits);



#ifdef __cplusplus
}
#endif
#endif /* !Py_FLOATOBJECT_H */