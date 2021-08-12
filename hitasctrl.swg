// %module tauhitas
// directors="1" needed for being able to use the director feature
%module(directors="1") tauhitas

%include <windows.i>
%include <typemaps.i>
%include <arrays_java.i>
%include <various.i>
%include <std_string.i>

// Defines not covered by the above ".i" files of SWIG
#define FAR                     __far
#define STDMETHOD(method)       HRESULT STDMETHODCALLTYPE method
#define STDMETHODIMP            HRESULT STDMETHODCALLTYPE
#define STDMETHODCALLTYPE       __stdcall

// Include this block in the generated C++ wrapper code (hitasctrl_wrap.cpp)
%{
#include "IProgressListener.h"
#include "ICallback.h"
#include "hitasctrl.h"
%}

%feature("director") IProgressListener;
%feature("director") ICallback;

// Turn on director wrapping IProgressListener. This makes it possible to extend this class in Java
// Generate wrapper code for the classes in these header files, so they can be called from Java
%include "IProgressListener.h"
%include "ICallback.h"
%include "hitasctrl.h"