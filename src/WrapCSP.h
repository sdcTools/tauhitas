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

#ifndef WRAPCSP_H
#define	WRAPCSP_H
#include "IProgressListener.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
    #define IMPORTFUNC __declspec(dllimport)
#else
    #define IMPORTFUNC __attribute__ ((visibility("default")))
#endif

int    CSPoptimize(const char*, IProgressListener*);
int    CSPloadprob(const char*,int,double*,int,double*,int*,char*,double*,double*,double*,double*,char**,int*,int*,signed char*);
int    CSPfreeprob(const char*);
int    CSPsolution(const char*,int*, int*,char*);
int    CSPrelbounds(const char*,int,int*,double*,double*,char);

void   CSPSetFileNames(const char*, const char*);
void   CSPFreeFileNames(const char*);
void   CSPSetDoubleConstant(const char*, const int, double);
void   CSPSetIntegerConstant(const char*, const int, int);
double CSPGetDoubleConstant(const char*, const int);
int    CSPGetIntegerConstant(const char*, const int);

#ifdef LPCP
namespace CPLEXv{
IMPORTFUNC void   CSPSetFileNames(const char*);
IMPORTFUNC void   CSPFreeFileNames();
IMPORTFUNC void   CSPSetDoubleConstant(const int, double);
IMPORTFUNC void   CSPSetIntegerConstant(const int, int);
IMPORTFUNC double CSPGetDoubleConstant(const int);
IMPORTFUNC int    CSPGetIntegerConstant(const int);
IMPORTFUNC int    CSPoptimize(IProgressListener*);
IMPORTFUNC int    CSPloadprob(int,double*,int,double*,int*,char*,double*,double*,double*,double*,char**,int*,int*,signed char*);
IMPORTFUNC int    CSPfreeprob();
IMPORTFUNC int    CSPsolution(int*, int*,char*);
IMPORTFUNC int    CSPrelbounds(int,int*,double*,double*,char);
}
#endif

#ifdef LPSC
namespace SCIPv{
IMPORTFUNC void   CSPSetFileNames(const char*);
IMPORTFUNC void   CSPFreeFileNames();
IMPORTFUNC void   CSPSetDoubleConstant(const int, double);
IMPORTFUNC void   CSPSetIntegerConstant(const int, int);
IMPORTFUNC double CSPGetDoubleConstant(const int);
IMPORTFUNC int    CSPGetIntegerConstant(const int);
IMPORTFUNC int    CSPoptimize(IProgressListener*);
IMPORTFUNC int    CSPloadprob(int,double*,int,double*,int*,char*,double*,double*,double*,double*,char**,int*,int*,signed char*);
IMPORTFUNC int    CSPfreeprob();
IMPORTFUNC int    CSPsolution(int*, int*,char*);
IMPORTFUNC int    CSPrelbounds(int,int*,double*,double*,char);
}
#endif

#ifdef LPXP
namespace XPRESSv{
IMPORTFUNC void   CSPSetFileNames(const char*);
IMPORTFUNC void   CSPFreeFileNames();
IMPORTFUNC void   CSPSetDoubleConstant(const int, double);
IMPORTFUNC void   CSPSetIntegerConstant(const int, int);
IMPORTFUNC double CSPGetDoubleConstant(const int);
IMPORTFUNC int    CSPGetIntegerConstant(const int);
IMPORTFUNC int    CSPoptimize(IProgressListener*);
IMPORTFUNC int    CSPloadprob(int,double*,int,double*,int*,char*,double*,double*,double*,double*,char**,int*,int*,signed char*);
IMPORTFUNC int    CSPfreeprob();
IMPORTFUNC int    CSPsolution(int*, int*,char*);
IMPORTFUNC int    CSPrelbounds(int,int*,double*,double*,char);
}
#endif

#endif	/* WRAPCSP_H */

