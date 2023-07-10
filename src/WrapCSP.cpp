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

// Makes choice of Solver-dll invisible to user
// Additional Solver available? Add a line to each wrapper function.
// Currently only CPLEX, SCIP and XPRESS available
#include <string.h>
#include "WrapCSP.h"
#include "resource.h"

void CSPSetFileNames(const char* Solver, const char* dir)
{
#ifdef LPCP
    if (strcmp(Solver,"CPLEX")==0) CPLEXv::CSPSetFileNames(dir);
#endif
#ifdef LPSC
    if (strcmp(Solver,"SCIP")==0) SCIPv::CSPSetFileNames(dir);
#endif
#ifdef LPXP
    if (strcmp(Solver,"XPRESS")==0) XPRESSv::CSPSetFileNames(dir);
#endif
}

void CSPFreeFileNames(const char* Solver)
{
#ifdef LPCP
    if (strcmp(Solver,"CPLEX")==0) CPLEXv::CSPFreeFileNames();
#endif
#ifdef LPSC
    if (strcmp(Solver,"SCIP")==0) SCIPv::CSPFreeFileNames();
#endif
#ifdef LPXP
    if (strcmp(Solver,"XPRESS")==0) XPRESSv::CSPFreeFileNames();
#endif
}

void   CSPSetDoubleConstant(const char* Solver, const int VarNumber, double VarValue)
{
#ifdef LPCP
    if (strcmp(Solver,"CPLEX")==0) CPLEXv::CSPSetDoubleConstant(VarNumber, VarValue);
#endif
#ifdef LPSC
    if (strcmp(Solver,"SCIP")==0) SCIPv::CSPSetDoubleConstant(VarNumber, VarValue);
#endif
#ifdef LPXP
    if (strcmp(Solver,"XPRESS")==0) XPRESSv::CSPSetDoubleConstant(VarNumber, VarValue);
#endif
}

void   CSPSetIntegerConstant(const char* Solver, const int VarNumber, int VarValue)
{
#ifdef LPCP
    if (strcmp(Solver,"CPLEX")==0) CPLEXv::CSPSetIntegerConstant(VarNumber, VarValue);
#endif
#ifdef LPSC
    if (strcmp(Solver,"SCIP")==0) SCIPv::CSPSetIntegerConstant(VarNumber, VarValue);
#endif
#ifdef LPXP
    if (strcmp(Solver,"XPRESS")==0) XPRESSv::CSPSetIntegerConstant(VarNumber, VarValue);
#endif
}

double CSPGetDoubleConstant(const char* Solver, const int VarNumber)
{
    double retval = -1;
#ifdef LPCP
    if (strcmp(Solver,"CPLEX")==0)  retval = CPLEXv::CSPGetDoubleConstant(VarNumber);
#endif
#ifdef LPSC
    if (strcmp(Solver,"SCIP")==0)   retval = SCIPv::CSPGetDoubleConstant(VarNumber);
#endif
#ifdef LPXP
    if (strcmp(Solver,"XPRESS")==0) retval = XPRESSv::CSPGetDoubleConstant(VarNumber);
#endif
    return retval;
}
int    CSPGetIntegerConstant(const char* Solver, const int VarNumber)
{
    int retval = -1;
#ifdef LPCP
    if (strcmp(Solver,"CPLEX")==0)  retval = CPLEXv::CSPGetIntegerConstant(VarNumber);
#endif
#ifdef LPSC
    if (strcmp(Solver,"SCIP")==0)   retval = SCIPv::CSPGetIntegerConstant(VarNumber);
#endif
#ifdef LPXP
    if (strcmp(Solver,"XPRESS")==0) retval = XPRESSv::CSPGetIntegerConstant(VarNumber);
#endif
    return retval;
}

int    CSPoptimize(const char* Solver, IProgressListener* ProgressListener)
{
    int retval = -1;
#ifdef LPCP
    if (strcmp(Solver,"CPLEX")==0)  retval = CPLEXv::CSPoptimize(ProgressListener);
#endif
#ifdef LPSC
    if (strcmp(Solver,"SCIP")==0)   retval = SCIPv::CSPoptimize(ProgressListener);
#endif
#ifdef LPXP
    if (strcmp(Solver,"XPRESS")==0) retval = XPRESSv::CSPoptimize(ProgressListener);
#endif
    return retval;
}

int    CSPloadprob(const char* Solver, int nsums_,double *rhs_,int ncells_,double *data_,int  *weight_,char *status_,double *lpl_,double *upl_,double *lb_,double *ub_,char **names_,int  *nlist_,int  *listcell_,signed char *listcoef_)
{
    int retval = -1;
#ifdef LPCP
    if (strcmp(Solver,"CPLEX")==0)
        retval = CPLEXv::CSPloadprob(nsums_,rhs_,ncells_,data_,weight_,status_,lpl_,upl_,lb_,ub_,names_,nlist_,listcell_,listcoef_);
#endif
#ifdef LPSC
    if (strcmp(Solver,"SCIP")==0) 
        retval = SCIPv::CSPloadprob(nsums_,rhs_,ncells_,data_,weight_,status_,lpl_,upl_,lb_,ub_,names_,nlist_,listcell_,listcoef_);
#endif
#ifdef LPXP
    if (strcmp(Solver,"XPRESS")==0) 
        retval = XPRESSv::CSPloadprob(nsums_,rhs_,ncells_,data_,weight_,status_,lpl_,upl_,lb_,ub_,names_,nlist_,listcell_,listcoef_);
#endif
    if (retval){
        throw(HITAS_JJNOTLOADED);
    }
    return retval;
}

int    CSPfreeprob(const char* Solver)
{
    int retval = -1;
#ifdef LPCP
    if (strcmp(Solver,"CPLEX")==0)  retval = CPLEXv::CSPfreeprob();
#endif
#ifdef LPSC
    if (strcmp(Solver,"SCIP")==0)   retval = SCIPv::CSPfreeprob();    
#endif
#ifdef LPXP
    if (strcmp(Solver,"XPRESS")==0) retval = XPRESSv::CSPfreeprob();  
#endif
    return retval;
}

int    CSPsolution(const char* Solver, int *lowerb_,int *upperb_,char *status_)
{
    int retval = -1;
#ifdef LPCP
    if (strcmp(Solver,"CPLEX")==0)  retval = CPLEXv::CSPsolution(lowerb_,upperb_,status_);
#endif
#ifdef LPSC
    if (strcmp(Solver,"SCIP")==0)   retval = SCIPv::CSPsolution(lowerb_,upperb_,status_);
#endif
#ifdef LPXP
    if (strcmp(Solver,"XPRESS")==0) retval = XPRESSv::CSPsolution(lowerb_,upperb_,status_);    
#endif
    return retval;
}

int    CSPrelbounds(const char* Solver, int nlist,int *list,double *ub,double *lb,char type)
{
    int retval = -1;
#ifdef LPCP
    if (strcmp(Solver,"CPLEX")==0)  retval = CPLEXv::CSPrelbounds(nlist,list,ub,lb,type);
#endif
#ifdef LPSC
    if (strcmp(Solver,"SCIP")==0)   retval = SCIPv::CSPrelbounds(nlist,list,ub,lb,type);
#endif
#ifdef LPXP
    if (strcmp(Solver,"XPRESS")==0) retval = XPRESSv::CSPrelbounds(nlist,list,ub,lb,type);    
#endif
    return retval;
}
