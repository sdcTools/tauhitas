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

#ifndef __CPTAUHITAS_MAIN
#define __CPTAUHITAS_MAIN

#include <string>
#include <vector>
#include <map>
#include "WrapCSP.h"

double MAX_TIME;
extern double ZERO;
extern double ZERO_1;
extern double ZERO_2;
extern double INF;
extern long MAX_COLS_LP;
extern long MAX_ROWS_LP;
extern long MAX_CUTS_POOL;
extern long MAX_CUTS_ITER;
extern double MIN_VIOLA;
extern double MAX_SLACK;
extern double FEAS_TOL;
extern double OPT_TOL;
bool PPDEBUG;

double JJTime;
int MINCOUNT;
short DISTANCE;
//Vector<int[5]> D;
std::vector< std::vector<int> > D;
double MAXDISTANCE;
double LOWERMARG;
double UPPERMARG;
double MINTABVAL;
double MAXTABVAL;
int APRIORI;
double APRIORILB;
double APRIORIUB;
int LINKED;
int DECIMALS;

bool DOSINGLETONS;
bool DOSINGLEWITHSINGLE;
bool DOSINGLEWITHMORE;
bool DOCOUNTBOUNDS;
bool DOLIFTUP;

double MAXWEIGHT;

const char* OUTDIR;

FILE* LogFile;

std::string LogName;

#ifdef LPCP
namespace CPLEXv{
IMPORTFUNC CPXENVptr Env;
IMPORTFUNC void   CSPSetFileNames(const char*);
IMPORTFUNC void   CSPFreeFileNames();
IMPORTFUNC void   CSPSetDoubleConstant(const int, double);
IMPORTFUNC void   CSPSetIntegerConstant(const int, int);
IMPORTFUNC double CSPGetDoubleConstant(const int);
IMPORTFUNC int    CSPGetIntegerConstant(const int);
IMPORTFUNC int    CSPoptimize();
IMPORTFUNC int    CSPloadprob(int,double*,int,double*,int*,char*,double*,double*,double*,double*,char**,int*,int*,signed char*);
IMPORTFUNC int    CSPfreeprob(void);
IMPORTFUNC int    CSPsolution(int*, int*,char*);
IMPORTFUNC int    CSPrelbounds(int,int*,double*,double*,char);
}
#endif

#ifdef LPSC
namespace SCIPv{
SCIP   *_scip;
IMPORTFUNC SCIP_LPI **Env;
IMPORTFUNC void   CSPSetFileNames(const char*);
IMPORTFUNC void   CSPFreeFileNames();
IMPORTFUNC void   CSPSetDoubleConstant(const int, double);
IMPORTFUNC void   CSPSetIntegerConstant(const int, int);
IMPORTFUNC double CSPGetDoubleConstant(const int);
IMPORTFUNC int    CSPGetIntegerConstant(const int);
IMPORTFUNC int    CSPoptimize();
IMPORTFUNC int    CSPloadprob(int,double*,int,double*,int*,char*,double*,double*,double*,double*,char**,int*,int*,signed char*);
IMPORTFUNC int    CSPfreeprob(void);
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
IMPORTFUNC int    CSPloadprob(int,double*,int,double*,int*,char*,double*,double*,double*,double*,char**,int*,int*,signed char*);
IMPORTFUNC int    CSPoptimize();
IMPORTFUNC int    CSPfreeprob();
IMPORTFUNC int    CSPsolution(int*,int*,char*);
IMPORTFUNC int    CSPrelbounds(int,int*,double*,double*,char);
}  //namespace SCIPv end
#endif
#endif
