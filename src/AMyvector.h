/*
* Argus Open Source
* Software to apply Statistical Disclosure Control techniques
* 
* Copyright 2014 Statistics Netherlands
* 
* This program is free software; you can redistribute it and/or 
* modify it under the terms of the European Union Public Licence 
* (EUPL) version 1.1, as published by the European Commission.
* 
* You can find the text of the EUPL v1.1 on
* https://joinup.ec.europa.eu/software/page/eupl/licence-eupl
* 
* This software is distributed on an "AS IS" basis without 
* warranties or conditions of any kind, either express or implied.
*/

#ifndef __MYVECTOR_H
#define __MYVECTOR_H

#include <malloc.h>
#include "AErrors.h"
#include "resource.h"
#include <cstdlib>
#include <vector>

extern void LogPrintf(const char* LogName, const char* LogMessage);
extern void LogPrintf(const char* LogName, std::string LogMessage);
extern void LogPrintf(std::string LogName, std::string LogMessage);

extern std::string to_string(int);

// Definition of class Vector<Type>
// elements can be accessed with [i] where
// i = 1, ..., dim (i.e., starts at i=1)

template <class T> class Vector
{
  private:
        int dim;
        T* body;
  public:
        Vector();
	virtual ~Vector();
	//T& Body() {return *body;};
        int size() {return dim;};
        T& operator [] (int i) {return body[i-1];};
	const T& operator [] (int i) const {return body[i-1];};
        void Init() {body = NULL; dim=0;};
        void Make(int L);
        void Grow(int L);
        void Free();
};

template <class T> Vector<T>::Vector()
{
};

template <class T> Vector<T>::~Vector()
{
};

template <class T> void Vector<T>::Make(int L)
{
    if (L < 0) L = 0;				// Empty Vector in case L is negative
    body = (T*) malloc(L*sizeof(T));            // Is body = malloc(0) safe to use (i.e., when L=0)? 
    if (body==NULL)                             // Is malloc save to use for any type T, even Vector<T>?
    {
        LogPrintf(LogName,"Not enough memory in Vector<T>.Make()\n");
        exit(HITAS_NOTENOUGHMEMORY);
    }
    dim = L;
};

template <class T> void Vector<T>::Grow(int L)
{
    if ((body == NULL))
    {
        body = (T*) malloc(L*sizeof(T));
        dim = L;
    }
    else
    {  
        body = (T*) realloc(body,(dim + L)*sizeof(T)); // realloc can be unsafe when used with complex types T?
        if ((body==NULL) && (dim+L != 0))
	{
            LogPrintf(LogName,"Not enough memory to Grow Vector<T> by ");
            LogPrintf(LogName,to_string(L));
            LogPrintf(LogName," elements\n");
	    exit(HITAS_NOTENOUGHMEMORY);
	}
	dim += L;
    }
};

template <class T> void Vector<T>::Free()
{
  dim = 0;
  free(body);
  body=NULL;
};
#endif
