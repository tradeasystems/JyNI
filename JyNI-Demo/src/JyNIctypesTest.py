'''
 * Copyright of JyNI:
 * Copyright (c) 2013, 2014, 2015 Stefan Richthofer.  All rights reserved.
 *
 *
 * Copyright of Python and Jython:
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010,
 * 2011, 2012, 2013, 2014, 2015 Python Software Foundation.  All rights reserved.
 *
 *
 * This file is part of JyNI.
 *
 * JyNI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * JyNI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with JyNI.  If not, see <http://www.gnu.org/licenses/>.


Created on 03.10.2015

@author: Stefan Richthofer
'''

import sys

#Include native ctypes:
#sys.path.append('/usr/lib/python2.7/lib-dynload')
sys.path.insert(0, '/home/stefan/eclipseWorkspace/ctypes')
sys.path.insert(0, '../../Lib')
#sys.path.remove('/home/stefan/eclipseWorkspace/JyNI/Lib')
#print sys.path

#from ctypes import cdll
#from _ctypes import _SimpleCData
import ctypes
#import struct
#from java.lang import System

#print "py_object0..."

#class py_object0(_SimpleCData):
#	pass

# print "import of ctypes successful"
# print ""
#print ctypes.sizeof(ctypes.c_long)
#print struct.calcsize("l")#ctypes.c_long._type_)
#print ctypes.c_long._type_
#print py_object0

#libc = ctypes.cdll.LoadLibrary("libc.so.6")
#libc = ctypes.CDLL('libc.so.6')
#print libc
#print libc.time
# print libc.strlen
# print libc.strlen("abcdef")
# print "----------"
#printf = libc.printf
#printf("%i second-timestamp\n", System.currentTimeMillis()/1000)
#System.out.println('Timestamp: '+str(libc.time(0)))
#print libc.time(0)
#import _ctypes_test
#import ctypes.test
#from ctypes import *
# class cell(Structure):
# 	pass
# 
# print "----------"
#cell._fields_ = [("name", c_char_p), ("next", POINTER(cell))]
#cell.test = "blah"
#print cell.test
#print cell._fields_
# c1 = cell()
# # print cell
# # print c1
# c1.name = "foo"
# c2 = cell()
# c2.name = "bar"
# #pt = pointer(c1)
# c1.next = pointer(c2)
# c2.next = pointer(c1)
# p = c1
# print c1
# print type(cell.__dict__)
# print type(c1.__dict__)
# print type(pt.__dict__)
# print type(pt[0].__dict__)
#print c1.__dict__
# print pt
# print pt[0]
#print pt[0].name
#print c2
#print "---"
# print p.name
# p = p.next[0]
# print p.name
# p = p.next[0]
# print p.name
# for i in range(8):
# 	print p
# 	print p.name,
# 	p = p.next[0]

#ctypes.test.main(ctypes.test)
print "=========="
#print libc.time(0)
