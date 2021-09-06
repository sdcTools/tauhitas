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

#ifndef HITASCTRL_H
#define	HITASCTRL_H

#include "AMyIO.h"
#include "AMiscFunc.h"
#include "ADefs.h"
#include "IProgressListener.h"
#include "ICallback.h"
#include <iostream>
#include <time.h>

class __declspec(dllexport) HiTaSCtrl
//class HiTaSCtrl
{
private:
    double JJzero;
    double JJzero1;
    double JJzero2;
    double JJInf;
    int    JJmaxcolslp;
    int    JJmaxrowslp;
    int    JJmaxcutspool;
    int    JJmaxcutsiter;
    double JJminviola;
    double JJmaxslack;
    double JJFeasibilityTol;
    double JJOptimalityTol;
    
    bool PPdebug;

    IProgressListener* m_ProgressListener;
    void FireUpperBound(int UB);
    void FireLowerBound(int LB);
    void FireUpdateGroup(int perc);
    void FireUpdateTables(int perc);
        
    void LoadErrorStrings(StringTable& ErrorStrings);
    void freecsp(int ncells, char **names, double *data, double *lpl, double *upl, double *spl, double *lb, double *ub, int *weight, char *states, int *ncard, double *rhs, int *list, signed char *val);
    void CleanUp();
    //void FreeHierVector(Vector<Hierarchy>& VS);
    void FreeHierVector(std::vector<Hierarchy>& VS);
    void CloseSolver(const char* Solver);
    long CheckStart(const char* Solver, const char* ILMFile);   
    
public:
    HiTaSCtrl()
    {
        //m_ProgressListener = NULL;
        // Load from file the strings assigned to error-codes
	//LoadErrorStrings(ErrorStrings);
    
	// Default settings for JJ-variables, in case of Cplex
	JJzero        = 1E-7;
	JJzero1       = 1E-7;
	JJzero2       = 1E-10;
	JJInf         = 2.14E9;
	JJmaxcolslp   = 50000;
	JJmaxrowslp   = 15000;
	JJmaxcutspool = 500000;
	JJmaxcutsiter = 50;
	JJminviola    = 0.0001;
	JJmaxslack    = 0.0001;

        JJFeasibilityTol = 1E-6;
        JJOptimalityTol = 1E-9;
        
        // Default setting for debugmode
	PPdebug = false;        
    }
    
    ~HiTaSCtrl()
    {
        CleanUp();
    }
    
    void SetProgressListener(IProgressListener *ProgressListener);
    void SetCallback(ICallback *jCallback);

    void SetJJconstantsInt(const int ConstName, const long ConstValue);
    void SetJJconstantsDbl(const int ConstName, const double ConstValue);
    long GetJJconstantsInt(const int ConstName);
    double GetJJconstantsDbl(const int ConstName);
    std::string GetErrorString(int ErrorNumber);
    std::string GetVersion();
    void SetDebugMode(bool debug);
    
    long FullJJ(const char* InFileJJ, const char* OutFile, long MaxTime, const char* ILMFile, const char* OutDir, const char* Solver);
    long AHiTaS(const char* ParsFile, const char* FilesFile, long MaxTime, const char* ILMFile, const char* TauOutDir, const char* Solver, bool DoSingleWithSingle, bool DoSingleWithMore, bool DoCountBounds) ;
    
};

#endif	/* HITASCTRL_H */

