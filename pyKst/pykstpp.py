import numpy as npy
from scipy.weave import inline

def get_arr(arr,socket,handle):
    dtype2ctype = {
        npy.dtype(npy.float64): 'double',
        npy.dtype(npy.float32): 'float',
        npy.dtype(npy.int32): 'int',
        npy.dtype(npy.int16): 'short',
    }
    dt = dtype2ctype.get(arr.dtype)	#I'm leaving this code here in case anyone wants to mess with it.
    assert(dt=='double') # Only use float64 arrays!

    code = \
"""

//
// I don't usually program in python, but when I do, I program in C++
//


QLocalSocket s;
s.connectToServer(QString(socket.c_str()));
s.waitForConnected(300);
s.write("Vector::getBinaryArray("+QByteArray(handle.c_str())+")");
QDataStream ds(&s);
qint64 count;
s.waitForReadyRead(-1);
ds>>count;

PyArray_Dims dims;
dims.len = 1;
dims.ptr = new npy_intp[1];
dims.ptr[0] = (npy_intp)count;
PyArray_Resize(arr_array, &dims, 0, PyArray_ANYORDER);
delete[]dims.ptr;

NpyIter* it=NpyIter_New(arr_array,NPY_ITER_READWRITE,NPY_CORDER,NPY_NO_CASTING,NULL);
char**dataptr = NpyIter_GetDataPtrArray(it);
NpyIter_IterNextFunc *iternext = NpyIter_GetIterNext(it, NULL);

do {
	char* data=*dataptr;	//i.e., byte array
	while(ds.atEnd()) {s.waitForReadyRead(-1);}	//possible optimization: call ds.atEnd() less
	ds>>((double*)data)[0];	//possible optimization: QByteArray switches the endianess twice...
} while(iternext(it));

""" #% (dt)


    support_code = \
"""
#include <QtCore>
#include <QLocalSocket>
"""

    socket=str(socket)
    handle=str(handle)
    inline(code, ['arr','socket','handle'],support_code=support_code,include_dirs=["/usr/include/qt4/QtCore","/usr/include/qt4/QtNetwork","/usr/include/qt4"],libraries=["QtCore","QtNetwork"],runtime_library_dirs=["/usr/lib/"])
    # I'm not sure if the above line can be made portable or not...








def get_matrix(arr,socket,handle):
    dtype2ctype = {
        npy.dtype(npy.float64): 'double',
        npy.dtype(npy.float32): 'float',
        npy.dtype(npy.int32): 'int',
        npy.dtype(npy.int16): 'short',
    }
    dt = dtype2ctype.get(arr.dtype)	#I'm leaving this code here in case anyone wants to mess with it.
    assert(dt=='double') # Only use float64 arrays!

    code = \
"""

//
// I don't usually program in python, but when I do, I program in C++
//


QLocalSocket s;
s.connectToServer(QString(socket.c_str()));
s.waitForConnected(300);
s.write("Matrix::getBinaryArray("+QByteArray(handle.c_str())+")");
QDataStream ds(&s);
qint64 count;
s.waitForReadyRead(-1);
qint32 nX,nY;
double minX,minY,stepX,stepY;
ds>>nX>>nY>>minX>>minY>>stepX>>stepY;

PyArray_Dims dims;
dims.len = 2;
dims.ptr = new npy_intp[2];
dims.ptr[0] = (npy_intp)nX;
dims.ptr[1] = (npy_intp)nY;
PyArray_Resize(arr_array, &dims, 0, PyArray_ANYORDER);
delete[]dims.ptr;

NpyIter* it=NpyIter_New(arr_array,NPY_ITER_READWRITE,NPY_CORDER,NPY_NO_CASTING,NULL);
char**dataptr = NpyIter_GetDataPtrArray(it);
NpyIter_IterNextFunc *iternext = NpyIter_GetIterNext(it, NULL);

do {
	char* data=*dataptr;	//i.e., byte array
	while(ds.atEnd()) {s.waitForReadyRead(-1);}	//possible optimization: call ds.atEnd() less
	ds>>((double*)data)[0];	//possible optimization: QByteArray switches the endianess twice...
} while(iternext(it));

""" #% (dt)


    support_code = \
"""
#include <QtCore>
#include <QString>
#include <QDataStream>
#include <QLocalSocket>
"""

    socket=str(socket)
    handle=str(handle)
    inline(code, ['arr','socket','handle'],support_code=support_code,include_dirs=["/usr/include/qt4/QtCore","/usr/include/qt4/QtNetwork","/usr/include/qt4"],libraries=["QtCore","QtNetwork"],runtime_library_dirs=["/usr/lib/"])
    # I'm not sure if the above line can be made portable or not...









def set_arr(arr,socket,handle):
    dtype2ctype = {
        npy.dtype(npy.float64): 'double',
        npy.dtype(npy.float32): 'float',
        npy.dtype(npy.int32): 'int',
        npy.dtype(npy.int16): 'short',
    }
    dt = dtype2ctype.get(arr.dtype)
    assert(dt=='double') # Only use float64 arrays!

    code = \
"""

//
// I don't usually program in python, but when I do, I program in C++
//

QLocalSocket s;
s.connectToServer(QString(socket.c_str()));
s.waitForConnected(300);
s.write("EditableVector::setBinaryArray("+QByteArray(handle.c_str())+")");
s.waitForReadyRead(-1);
QDataStream ds(&s);
ds<<(qint64)PyArray_SIZE(arr_array);

NpyIter* it=NpyIter_New(arr_array,NPY_ITER_READWRITE,NPY_CORDER,NPY_NO_CASTING,NULL);
char**dataptr = NpyIter_GetDataPtrArray(it);
NpyIter_IterNextFunc *iternext = NpyIter_GetIterNext(it, NULL);

int l=0;
int v=0;
do {
	++v;
	char* data=*dataptr;	//i.e., byte array
	ds<<((double*)data)[0];	//possible optimization: QByteArray switches the endianess twice...
	if(++l==512) {
		while(s.bytesToWrite()) {
			s.waitForBytesWritten(-1);
		}
		l=0;
	}
} while(iternext(it));
while(s.bytesToWrite()) {
	s.waitForBytesWritten(-1);
}

""" #% (dt)


    support_code = \
"""
#include <QtCore>
#include <QString>
#include <QDataStream>
#include <QLocalSocket>
"""

    socket=str(socket)
    handle=str(handle)
    inline(code, ['arr','socket','handle'],support_code=support_code,include_dirs=["/usr/include/qt4/QtCore","/usr/include/qt4/QtNetwork","/usr/include/qt4"],libraries=["QtCore","QtNetwork"],runtime_library_dirs=["/usr/lib/"])
    # I'm not sure if the above line can be made portable or not...




def set_matrix(arr,nX,nY,socket,handle):
    dtype2ctype = {
        npy.dtype(npy.float64): 'double',
        npy.dtype(npy.float32): 'float',
        npy.dtype(npy.int32): 'int',
        npy.dtype(npy.int16): 'short',
    }
    dt = dtype2ctype.get(arr.dtype)
    assert(dt=='double') # Only use float64 arrays!

    code = \
"""

//
// I don't usually program in python, but when I do, I program in C++
//

QLocalSocket s;
s.connectToServer(QString(socket.c_str()));
s.waitForConnected(300);
s.write("EditableMatrix::setBinaryArray("+QByteArray(handle.c_str())+","+QByteArray::number(nX)+","+QByteArray::number(nY)+","+"0.0,0.0,1.0,1.0)");
s.waitForReadyRead(-1);
QDataStream ds(&s);

NpyIter* it=NpyIter_New(arr_array,NPY_ITER_READWRITE,NPY_CORDER,NPY_NO_CASTING,NULL);
char**dataptr = NpyIter_GetDataPtrArray(it);
NpyIter_IterNextFunc *iternext = NpyIter_GetIterNext(it, NULL);

int l=0;
int v=0;
do {
	++v;
	char* data=*dataptr;	//i.e., byte array
	ds<<((double*)data)[0];	//possible optimization: QByteArray switches the endianess twice...
	if(++l==512) {
		while(s.bytesToWrite()) {
			s.waitForBytesWritten(-1);
		}
		l=0;
	}
} while(iternext(it));
while(s.bytesToWrite()) {
	s.waitForBytesWritten(-1);
}

""" #% (dt)


    support_code = \
"""
#include <QtCore>
#include <QString>
#include <QDataStream>
#include <QLocalSocket>
"""

    socket=str(socket)
    handle=str(handle)
    inline(code, ['arr','nX','nY','socket','handle'],support_code=support_code,include_dirs=["/usr/include/qt4/QtCore","/usr/include/qt4/QtNetwork","/usr/include/qt4"],libraries=["QtCore","QtNetwork"],runtime_library_dirs=["/usr/lib/"])
    # I'm not sure if the above line can be made portable or not...
