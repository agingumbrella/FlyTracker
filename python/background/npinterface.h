#ifndef NPINTERFACE_H_
#define NPINTERFACE_H_

#include <numpy/ndarrayobject.h>
#include <opencv2/core/core.hpp>

using namespace cv;

static PyObject* opencv_error = 0;

class PyAllowThreads 
{
public:
	PyAllowThreads() : _state(PyEval_SaveThread()) {}
	~PyAllowThreads()
	{
		PyEval_RestoreThread(_state);
	}
private:
	PyThreadState* _state;
};

class PyEnsureGIL
{
public:
	PyEnsureGIL() : _state(PyGILState_Ensure()) {}
	~PyEnsureGIL() 
	{
		PyGILState_Release(_state);
	}
private:
	PyGILState_STATE _state;
};

class NumpyAllocator : public MatAllocator
{
public:
    NumpyAllocator() {}
    ~NumpyAllocator() {}

    void allocate(int dims, const int* sizes, int type, int*& refcount,
                  uchar*& datastart, uchar*& data, size_t* step);
    void deallocate(int* refcount, uchar*, uchar*);
};


#define ERRWRAP2(expr) \
try \
{ \
    PyAllowThreads allowThreads; \
    expr; \
} \
catch (const cv::Exception &e) \
{ \
    PyErr_SetString(opencv_error, e.what()); \
    return 0; \
}

static size_t REFCOUNT_OFFSET = (size_t)&(((PyObject*)0)->ob_refcnt) +
    (0x12345678 != *(const size_t*)"\x78\x56\x34\x12\0\0\0\0\0")*sizeof(int);

NumpyAllocator g_numpyAllocator;

enum { ARG_NONE = 0, ARG_MAT = 1, ARG_SCALAR = 2 };

static int failmsg(const char* fmt, ...);

static inline PyObject* pyObjectFromRefcount(const int* refcount)
{
    return (PyObject*)((size_t)refcount - REFCOUNT_OFFSET);
}

static inline int* refcountFromPyObject(const PyObject* obj)
{
    return (int*)((size_t)obj + REFCOUNT_OFFSET);
}

static int pyopencv_to(const PyObject* o, Mat& m, const char* name = "<unknown>", bool allowND = true);
static PyObject* pyopencv_from(const cv::Mat& m);

#endif
