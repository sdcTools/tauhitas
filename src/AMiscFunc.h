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

#ifndef __MISCFUNC_H
#define __MISCFUNC_H
#include "AMyvector.h"
#include "AHier.h"
#include "ATabs.h"

/*
#ifdef XPRESS_13
	#include "XPTauHiTaSCtl.h"
	typedef CXPTauHiTaSCtrl TauHiTaSCtrl;
#else
	#include "CPTauHiTaSCtl.h"
	typedef CCPTauHiTaSCtrl TauHiTaSCtrl;
#endif
*/

#define __min(a,b)  (((a) < (b)) ? (a) : (b))
#define __max(a,b)  (((a) > (b)) ? (a) : (b))

//#define NEW_FILE 91
#define NEW_TAB  92
#define OLD_TAB  93
#define INFWEIGHT 999   // TEMPORARY!!!!!!!!

// Protection levels to be used in "tricks" (Singletons etc.)
#define TRICK_LPL		0.00
#define TRICK_SPL		0.00
#define TRICK_UPL_PERC          0.0001
#define TRICK_UPL		__max(MyRound(TRICK_UPL_PERC * CellInfo.value, DECIMALS), pow(10.0,-DECIMALS))

extern bool PPDEBUG;
extern StringTable ErrorStrings;

typedef struct{int position; int count; double value;} celinfo;
typedef std::map< std::string, std::vector<celinfo>* > CountInfo;
typedef std::map< std::string, int> TotCountInfo;

//int FillTable(JJTable& Tab, Vector< Vector<int> > SGTab, Table& BTab, std::vector<double>& bounds, bool DoCosts, double& MaxCost);  // PWOF-23-02-2009 Changed MaxCost from int to double
int FillTable(JJTable& Tab, std::vector< std::vector<int> > SGTab, Table& BTab, std::vector<double>& bounds, bool DoCosts, double& MaxCost);  // PWOF-23-02-2009 Changed MaxCost from int to double
int Suppress(const char* Solver, JJTable& Tab, int Rdim, bool DoCosts, double& MaxCost, int Hierarch,  int &ObjVal);
int Update(Table& BTab, JJTable& Tab, int RetCode, std::vector<double>& bounds);

int TestNewTable(std::string& TableList, JJTable& Tab, Table& BTab);
int ReadOldStats(std::string TableList, JJTable& Tab);

double MyRound(double x, int dec);
double seconds();
#endif
