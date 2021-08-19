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

#include "cplex.h"
#include "xprs.h"
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include "objscip/objscip.h"
#include "HiTaSCtrl.h"
#include "AErrors.h"
#include "Amain.h"
#include "ICallback.h"
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <cstdio>

#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
    #define IMPORTFUNC __declspec(dllimport)
#else
    #define IMPORTFUNC __attribute__ ((visibility("default")))
#endif

IMPORTFUNC int CSPdefinestoptime(int (*)(void));
StringTable ErrorStrings;

ICallback* m_Callback = NULL;

int MyStopTime()
{
    return m_Callback->SetStopTime();
}

///////////////////////////////////////////////////////////////////////
// Private functions
///////////////////////////////////////////////////////////////////////
void HiTaSCtrl::FireUpperBound(int UB)
{
    if (m_ProgressListener != NULL) {
        m_ProgressListener->UpdateUB(UB);
    }
}

void HiTaSCtrl::FireLowerBound(int LB)
{
    if (m_ProgressListener != NULL) {
        m_ProgressListener->UpdateLB(LB);
    }
}

void HiTaSCtrl::FireUpdateGroup(int perc)
{
    if (m_ProgressListener != NULL) {
        m_ProgressListener->UpdateGroups(perc);
    }
}

void HiTaSCtrl::FireUpdateTables(int perc)
{
    if (m_ProgressListener != NULL) {
        m_ProgressListener->UpdateTables(perc);
    }
}

void HiTaSCtrl::CloseSolver(const char* Solver)
{
    if (strcmp(Solver,"XPRESS")==0)
        XPRSfree();

    if (strcmp(Solver,"CPLEX")==0)
	CPXcloseCPLEX(&CPLEXv::Env);
                
    if (strcmp(Solver,"SCIP")==0)
        SCIPfree(&SCIPv::_scip);
}

// Check for license (if necessary) and start solver
// Available solvers: CPLEX, XPRESS, SCIP
long HiTaSCtrl::CheckStart(const char* Solver, const char* ILMFile)
{
    bool KnownSolver = false;
    FILE *logfile = OpenFile(LogName.c_str(),"w");
    
    fprintf(logfile,"-------------------------------------------------------------------------------\n");
    fprintf(logfile,"libTauHiTaS version is %s\n\n", VERSIONNUMBER);
    fprintf(logfile,"Using %s\n", Solver);
    
    fclose(logfile);
        
    if (strcmp(Solver,"CPLEX")==0)
    {
        if ((strcmp(ILMFile,"")!=0) && (ILMFile != NULL))
        {
            char* ILOGInfo;
            ILOGInfo = (char*) malloc((19 + strlen(ILMFile))*sizeof(char));
            strcpy(ILOGInfo, "ILOG_LICENSE_FILE=");
            strcat(ILOGInfo, ILMFile);
            CPXputenv(ILOGInfo);
            free(ILOGInfo);
        }
        int tmpstatus;
        CPLEXv::Env=CPXopenCPLEX(&tmpstatus);
        if (CPLEXv::Env == NULL)
        {
            WriteErrorToLog(LogName,JJ_NOCPLEXLICENCE);
            return(JJ_NOCPLEXLICENCE);
        }
        // At this point all should be OK
        logfile = OpenFile(LogName.c_str(),"a");
        fprintf(logfile,"CPlex version is %s\n\n",CPXversion(CPLEXv::Env));
        fclose(logfile);
        KnownSolver = true;
    }
    
    if (strcmp(Solver,"XPRESS")==0)
    {
        int ierr;
        char slicmsg[256] = "";
        std::string ferror;
        FILE *ErrorFile;
     
        ferror = PrepFile("XPerror.log");
        
        ierr = XPRSinit(NULL);

        if (ierr != 0)
        {
            XPRSgetlicerrmsg(slicmsg,256);
            ErrorFile = OpenFile(ferror.c_str(),"a");
            fprintf(ErrorFile,"%s\n",slicmsg);
            fclose(ErrorFile);
            WriteErrorToLog(LogName,JJ_NOXPDONGLE);
            return(JJ_NOXPDONGLE);
        }
        
        // At this point all should be OK    
        char version[16];
        XPRSgetversion(version);
        logfile = OpenFile(LogName.c_str(),"a");
        fprintf(logfile,"XPress version is %s\n\n",version);
        fclose(logfile);
        KnownSolver = true;        
    }
    
    if (strcmp(Solver,"SCIP")==0) 
    {
        SCIPcreate(&SCIPv::_scip);
        
        logfile = OpenFile(LogName.c_str(),"a");
        fprintf(logfile,"SCIP version is %lf\n",SCIPversion());
        fprintf(logfile,"\tusing %s\n\n",SCIPlpiGetSolverName());
        fclose(logfile);
        KnownSolver = true;        
    }
    
    if (KnownSolver)
    {
        char buffer[256];
        time_t timestamp;
        struct tm * timestring;
    
        logfile = OpenFile(LogName.c_str(),"a");
        //time(&timestamp);
        timestamp = time(NULL);
        timestring = localtime(&timestamp);
        strftime(buffer,256,"%H:%M:%S, %B %d, %Y",timestring);
        fprintf(logfile,"Start at %s\n",buffer);
        fprintf(logfile,"-------------------------------------------------------------------------------\n\n");
        fclose(logfile);
        return 0;
    }
    else    // At this point no known solver passed
    {
        logfile = OpenFile(LogName.c_str(),"a");
        fprintf(logfile,"Unknown (or not yet implemented) solver specified: %s\n",Solver);
        fprintf(logfile,"Currently only CPLEX, XPRESS, SCIP implemented for Modular and Optimal.\n\n");
        fclose(logfile);
        return(HITAS_UNKNOWNSOLVER);
    }
}

// Defines strings to be displayed for error-codes #defined in resource.h
// Strings are defined in file ErrorStrings.txt, located in directory where jar/exe is run
// Numbers in ErrorStrings.txt and in resource.h should match!!!
void HiTaSCtrl::LoadErrorStrings(StringTable& ErrorStrings)
{
    std::fstream StringInput;
    std::size_t index1, index2;
    std::string line, text;
    int constval;
    
    StringInput.open("ErrorStrings.txt",std::fstream::in);
    // Line in ErrorStrings.txt is of format
    // HITAS_FILENOTFOUND = 5000 = HiTaS: can not open file (not found)
    while(std::getline(StringInput,line))
    {
        index1 = line.find_first_of("=");
        std::istringstream(line.substr(index1+2)) >> constval;
        index2 = line.find_first_of("=",index1+1);
        text = line.substr(index2+2);
        ErrorStrings[constval] = text;
    }
            
    StringInput.close();
}

void HiTaSCtrl::freecsp(int ncells, char **names, double *data, double *lpl, double *upl, double *spl, double *lb, double *ub, int *weight, char *states, int *ncard, double *rhs, int *list, signed char *val)
{
    for(int i=0;i<ncells;i++)
        free(names[i]);
    free(names);
    free(data);
    free(lpl);
    free(upl);
    free(spl);
    free(lb);
    free(ub);
    free(weight);
    free(states);
    free(ncard);
    free(rhs);
    free(list);
    free(val);
}

void HiTaSCtrl::CleanUp()
{
    ErrorStrings.clear();
}

//void HiTaSCtrl::FreeHierVector(Vector<Hierarchy>& VS)
void HiTaSCtrl::FreeHierVector(std::vector<Hierarchy>& VS)
{
    for (size_t i=1;i<=VS.size();i++)
    {
        //FreeHierarchy(VS[i]);
        FreeHierarchy(VS[i-1]);
	//VS[i].Free();
        VS[i-1].Free();
	//free(VS[i].name);
        free(VS[i-1].name);
    }
    //VS.Free();
    VS.clear();
}

///////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////
void HiTaSCtrl::SetCallback(ICallback* jCallback)
{
    m_Callback = jCallback;
}

void HiTaSCtrl::SetProgressListener(IProgressListener* ProgressListener)
{
    m_ProgressListener = ProgressListener;
}

void HiTaSCtrl::SetJJconstantsInt(const int ConstName, const long ConstValue)
{
    FILE *LogFile;
    switch (ConstName){
        case JJMAXCOLSLP:
            JJmaxcolslp = ConstValue;
            break;
        case JJMAXROWSLP:
            JJmaxrowslp = ConstValue;
            break;
        case JJMAXCUTSPOOL:
            JJmaxcutspool = ConstValue;
            break;
        case JJMAXCUTSITER:
            JJmaxcutsiter = ConstValue;
            break;
        default:
            LogFile = OpenFile(LogName.c_str(),"a");
            fprintf(LogFile,"Unknown constant %d\n",ConstName);
            fclose(LogFile);
            break;            
    }
}

void HiTaSCtrl::SetJJconstantsDbl(const int ConstName, const double ConstValue)
{
    FILE *LogFile;
    switch (ConstName){
        case JJZERO:
            JJzero = ConstValue;
            break;
        case JJZERO1:
            JJzero1 = ConstValue;
            break;
        case JJZERO2:
            JJzero2 = ConstValue;
            break;
        case JJINF:
            JJInf = ConstValue;
            break;
        case JJMINVIOLA:
            JJminviola = ConstValue;
            break;
        case JJMAXSLACK:
            JJmaxslack = ConstValue;
            break;
        case JJFEASTOL:
            JJFeasibilityTol = ConstValue;
            break;
        case JJOPTTOL:
            JJOptimalityTol = ConstValue;
            break;
        default:
            LogFile = OpenFile(LogName.c_str(),"a");
            fprintf(LogFile,"Unknown constant %d\n",ConstName);
            fclose(LogFile);
            break;            
    }
}

long HiTaSCtrl::GetJJconstantsInt(const int ConstName)
{
    FILE *LogFile;
    switch (ConstName){
        case JJMAXCOLSLP:
            return JJmaxcolslp;
        case JJMAXROWSLP:
            return JJmaxrowslp;
        case JJMAXCUTSPOOL:
            return JJmaxcutspool;
        case JJMAXCUTSITER:
            return JJmaxcutsiter;
        default:
            LogFile = OpenFile(LogName.c_str(),"a");
            fprintf(LogFile,"Unknown constant %d\n",ConstName);
            fclose(LogFile);
            return -9;            
    }
}

double HiTaSCtrl::GetJJconstantsDbl(const int ConstName)
{
    FILE *LogFile;
    switch (ConstName){
        case JJZERO:
            return JJzero;
        case JJZERO1:
            return JJzero1;
        case JJZERO2:
            return JJzero2;
        case JJINF:
            return JJInf;
        case JJMINVIOLA:
            return JJminviola;
        case JJMAXSLACK:
            return JJmaxslack;
        case JJFEASTOL:
            return JJFeasibilityTol;
        case JJOPTTOL:
            return JJOptimalityTol;
        default:
            LogFile = OpenFile(LogName.c_str(),"a");
            fprintf(LogFile,"Unknown constant %d\n",ConstName);
            fclose(LogFile);
            return -9;            
    }
}

std::string HiTaSCtrl::GetErrorString(int ErrorNumber)
{
    return ErrorStrings[ErrorNumber];
}    

std::string HiTaSCtrl::GetVersion(){
    return VERSIONNUMBER;
}

void HiTaSCtrl::SetDebugMode(bool debug)
{
    PPdebug = debug;
}

// Apply Optimal a.k.a. FullJJ
long HiTaSCtrl::FullJJ(const char* InFileJJ, const char* OutFile, long MaxTime, const char* ILMFile, const char* OutDir, const char* Solver)
{
        long res;
        std::string tmpname;

        FILE *JJIn, *JJUit;
        int dum, INumVar, INumCons;
        int i,j, Icell;
        int terms;
        int lcost, ucost;
        double d_,lpl_,upl_,spl_,lb_,ub_;
        int w_;
        char s_;

        char **names;
        double *data, *lpl, *upl, *spl, *lb, *ub;
        int *weight;
        char *states;
	int *ncard, *list;
	double *rhs;
	signed char *val;
        int v_;
        
        setlocale(LC_NUMERIC,"english"); // Ensure that decimal _point_ should be read
        
        PPdebug ? PPDEBUG = true : PPDEBUG = false;
        
        OUTDIR = OutDir;
        LogName = PrepFile("FullJJ.log"); // LogName is global variable

        // Check for availability of license
        // Possibilities: Solver = "CPLEX", "XPRESS", "SCIP"
        // And write info to logfile FullJJ.log
        res = CheckStart(Solver,ILMFile);

        if (res != 0) return (res);
        
        // Append OUTDIR to default filenames in JJ-code
        CSPSetFileNames(Solver, OUTDIR);
        CSPdefinestoptime(MyStopTime);
        MAX_TIME=MaxTime*60.0; // MaxTime in minutes, MAX_TIME in seconds
        
        // Set JJ-constants
        CSPSetDoubleConstant(Solver,JJMAXTIME,MAX_TIME);
        CSPSetDoubleConstant(Solver,JJZERO,JJzero);
        CSPSetDoubleConstant(Solver,JJINF,JJInf);
	CSPSetIntegerConstant(Solver,JJMAXCOLSLP,JJmaxcolslp);
	CSPSetIntegerConstant(Solver,JJMAXROWSLP,JJmaxrowslp);
	CSPSetIntegerConstant(Solver,JJMAXCUTSITER,JJmaxcutsiter);
	CSPSetIntegerConstant(Solver,JJMAXCUTSPOOL,JJmaxcutspool);
	CSPSetDoubleConstant(Solver,JJMINVIOLA,JJminviola);
	CSPSetDoubleConstant(Solver,JJMAXSLACK,JJmaxslack);
        
        CSPSetDoubleConstant(Solver,JJFEASTOL,JJFeasibilityTol);
        CSPSetDoubleConstant(Solver,JJOPTTOL,JJOptimalityTol);
        //Used in PCSP by Rafa, but not in CSP by Salome
	//ZERO_1 = JJzero1;
	//ZERO_2 = JJzero2;
	//FEAS_TOL = JJFeasibilityTol;
	//OPT_TOL = JJOptimalityTol;

	LogFile = OpenFile(LogName.c_str(),"a");
	fprintf(LogFile,"FullJJ started\n\n");
	fprintf(LogFile,"MAX_TIME (secs) = %e\n",CSPGetDoubleConstant(Solver,JJMAXTIME));
	fprintf(LogFile,"ZERO            = %e\n",CSPGetDoubleConstant(Solver,JJZERO));
	fprintf(LogFile,"INF             = %e\n",CSPGetDoubleConstant(Solver,JJINF));
	fprintf(LogFile,"MAX_COLS_LP     = %d\n",CSPGetIntegerConstant(Solver,JJMAXCOLSLP));
	fprintf(LogFile,"MAX_ROWS_LP     = %d\n",CSPGetIntegerConstant(Solver,JJMAXROWSLP));
	fprintf(LogFile,"MAX_CUTS_POOL   = %d\n",CSPGetIntegerConstant(Solver,JJMAXCUTSPOOL));
	fprintf(LogFile,"MAX_CUTS_ITER   = %d\n",CSPGetIntegerConstant(Solver,JJMAXCUTSITER));
	fprintf(LogFile,"MIN_VIOLA       = %e\n",CSPGetDoubleConstant(Solver,JJMINVIOLA));
	fprintf(LogFile,"MAX_SLACK       = %e\n",CSPGetDoubleConstant(Solver,JJMAXSLACK));
	fprintf(LogFile,"FEASIBILITYTOL  = %e\n",CSPGetDoubleConstant(Solver,JJFEASTOL));
	fprintf(LogFile,"OPTIMALITYTOL   = %e\n",CSPGetDoubleConstant(Solver,JJOPTTOL));
        if (PPdebug) fprintf(LogFile,"PPdebug         = true\n");
        fclose(LogFile);
        
        try
        {
            JJIn = OpenFile(InFileJJ,"r");
            JJUit = OpenFile(OutFile,"w");
            LogFile = OpenFile(LogName.c_str(),"a");
            fprintf(LogFile,"InputFile       = %s\n",InFileJJ);
            fprintf(LogFile,"OutputFile      = %s\n\n",OutFile);
            fclose(LogFile);
        }
        catch(int code)
        {
            WriteErrorToLog(LogName,code);
            CSPFreeFileNames(Solver);
            CloseSolver(Solver);
            LogFile = OpenFile(LogName.c_str(),"a");
	    fprintf(LogFile,"Memory freed\n");
            fclose(LogFile);
            //fcloseall();
            return(code);
        }
                
        fscanf(JJIn,"%d\n",&dum); 
        if (dum) // File should start with 0
	{
            WriteErrorToLog(LogName,HITAS_WRONGHITASCALL); 
            //fcloseall();
            return (HITAS_WRONGHITASCALL);
	}

	LogFile = OpenFile(LogName.c_str(),"a");
	fprintf(LogFile,"Start reading cells\n");
	fclose(LogFile);

        fscanf(JJIn,"%d\n",&INumVar);

        names    = (char **) malloc( INumVar*sizeof(char *) );
        data     = (double *)malloc( INumVar*sizeof(double) );
        lpl      = (double *)malloc( INumVar*sizeof(double) );
        upl      = (double *)malloc( INumVar*sizeof(double) );
        spl      = (double *)malloc( INumVar*sizeof(double) );
        lb       = (double *)malloc( INumVar*sizeof(double) );
        ub       = (double *)malloc( INumVar*sizeof(double) );
        weight   = (int *)   malloc( INumVar*sizeof(int) );
        states   = (char *)  malloc( INumVar*sizeof(char) );

        for (i=0;i<INumVar;i++)
        {
            fscanf(JJIn,"%d %lf %d %c %lf %lf %lf %lf %lf\n",&dum,&d_,&w_,&s_,&lb_,&ub_,&lpl_,&upl_,&spl_);
            tmpname.clear();
            tmpname.append("x");
            tmpname.append(to_string(dum));
            names[i] = (char *)malloc( (tmpname.size()+1) * sizeof(char) );
            strcpy(names[i],tmpname.c_str());
            data[i] = d_;
            weight[i] = w_;
            states[i] = s_;
            lb[i] = lb_;
            ub[i] = ub_;
            lpl[i] = lpl_;
            upl[i] = upl_;
            spl[i] = spl_;
        }

	LogFile = OpenFile(LogName.c_str(),"a");
	fprintf(LogFile,"Start reading constraints\n");
	fclose(LogFile);
        
	fscanf(JJIn,"%d\n",&INumCons);

	ncard = (int *)   malloc( INumCons*sizeof(int) );
	rhs   = (double *)malloc( INumCons*sizeof(double) );

        list = NULL;
        val  = NULL;

        int t = 0;
        int listsize = 0;
        for (i=0;i<INumCons;i++)
        {
	    fscanf(JJIn,"%lf %d :", &d_, &terms);
            //int size = 0;
            //if (list) size = _msize(list)/sizeof(int);
            listsize += terms;

            //list  = (int *)        realloc(list, (size + terms)*sizeof(int) );
            //val   = (signed char *)realloc(val,  (size + terms)*sizeof(signed char));
            list  = (int *)        realloc(list, listsize*sizeof(int) );
            val   = (signed char *)realloc(val,  listsize*sizeof(signed char));
				
            rhs[i] = d_;
            ncard[i] = terms;
                
            for (j=0;j<terms;j++)
            {
                fscanf(JJIn," %d (%d)",&Icell,&v_);
                list[t] = Icell;
                val[t] = v_;
                t++;
            }
        }

        fclose(JJIn);
        LogFile = OpenFile(LogName.c_str(),"a");
        fprintf(LogFile,"Read Input File\n");
	fclose(LogFile);
        	
	try
	{
            CSPloadprob(Solver,INumCons,rhs,INumVar,data,weight,states,lpl,upl,lb,ub,names,ncard,list,val);
	}
	catch(int code)
	{
            WriteErrorToLog(LogName,code);
            CSPfreeprob(Solver);
            freecsp(INumVar, names, data, lpl, upl, spl, lb, ub, weight, states, ncard, rhs, list, val);
            CSPFreeFileNames(Solver);
            CloseSolver(Solver);
            
            LogFile = OpenFile(LogName.c_str(),"a");
	    fprintf(LogFile,"Memory freed\n");
            fclose(LogFile);
            //fcloseall();
            return(code);
	}
        
        JJTime = seconds(); // Start-time for optimization
        LogPrintf(LogName,"Start Optimizing\n");
	try
	{
            CSPoptimize(Solver,m_ProgressListener); // m_ProgressListener needed for firing LB and UB from inside of JJ-code
	}
	catch(int code)
	{
            WriteErrorToLog(LogName,code);
            CSPfreeprob(Solver);
	    freecsp(INumVar, names, data, lpl, upl, spl, lb, ub, weight, states, ncard, rhs, list, val);
	    CSPFreeFileNames(Solver);
            CloseSolver(Solver);
                
            LogFile = OpenFile(LogName.c_str(),"a");
	    fprintf(LogFile,"Memory freed\n");
            fclose(LogFile);
            //fcloseall();
            return(code);
	}

        JJTime = seconds() - JJTime; // t0 is now time needed to optimize
        
        LogPrintf(LogName,"Get solution\n");
	try
	{
            CSPsolution(Solver,&lcost,&ucost,&(*states));
	}
	catch(int code)
	{
            WriteErrorToLog(LogName,code);
            CSPfreeprob(Solver);
            freecsp(INumVar, names, data, lpl, upl, spl, lb, ub, weight, states, ncard, rhs, list, val);
            CSPFreeFileNames(Solver);
            CloseSolver(Solver);
                
            LogFile = OpenFile(LogName.c_str(),"a");
	    fprintf(LogFile,"Memory freed\n");
            fclose(LogFile);
            //fcloseall();

            return(code);
	}

        LogPrintf(LogName,"Start writing solution to OutputFile\n");
                
        for (i=0;i<INumVar;i++)
            if (states[i] == 'm')	// Only ID-number of secondary suppressed cells
                fprintf(JJUit,"%5d\n",i);
        
	FireLowerBound(lcost);
	FireUpperBound(ucost);
			
	LogFile = OpenFile(LogName.c_str(),"a");
        fprintf(LogFile,"Objectvalues: %12d and %12d\n",lcost,ucost);
        fprintf(LogFile,"Time needed for optimization: %12.3lf seconds\n",JJTime);
        fclose(LogFile);

	fclose(JJUit);

        CSPfreeprob(Solver);
        freecsp(INumVar, names, data, lpl, upl, spl, lb, ub, weight, states, ncard, rhs, list, val);
        CSPFreeFileNames(Solver);
        CloseSolver(Solver);
        
   	if ((ucost >= CSPGetDoubleConstant(Solver,JJINF) - 0.1) && (lcost >= CSPGetDoubleConstant(Solver,JJINF) - 0.1))
	{
            WriteErrorToLog(LogName,LB_EQ_UB_EQ_INF);
            return(LB_EQ_UB_EQ_INF);
	}
        
	LogFile = OpenFile(LogName.c_str(),"a");
        fprintf(LogFile,"Memory freed\n");
        fclose(LogFile);

        if (ucost-lcost<0.1)
            return 0;       // Optimal integer solution
        else
            return 1;       // Sub-optimal integer solution
}

long HiTaSCtrl::AHiTaS(const char* ParsFile, const char* FilesFile, long MaxTime, const char* ILMFile, const char* TauOutDir, const char* Solver, bool DoSingleWithSingle, bool DoSingleWithMore, bool DoCountBounds)
{
        FILE *Info, *TabUit, *Uit, *XUit, *JJUit, *BTUit; 
        
        std::string OutName, TrickName, BTName, BTTabsName, XOutName;
        std::string TableList, TableName, TmpTableName, FileName, BTabFileName, TmpName;
        
        int JJaant=0, Totaant=0, aantBT=0;
        int res=0, i, k, tel, M, ObjVal, ReturnCode=0, realdim;
        int MAXLINE=256;

        bool DaCapo=true;               // Boolean used to control backtracking
        int newtab;                     // Integer used to control backtracking

        double MaxCost; 
        double TotalTime, JJTotalTime=0;

        char line[MAXLINE];
        
        StringMap TmpStringMap;

        std::vector<double> MaxBounds; // Vector containing maximum lpl and upl per table
        MaxBounds.assign(2,0);         // intialized to 0 
        
        std::vector<StringMap> BogusLists;
        std::vector< std::vector<std::string> > VarNames;
        
        std::map<std::string,int> TmpSImap;
        std::map<std::string, std::string> ListsOfTables;
        
        ExInCodeLijst AllCodes;  // AllCodes contains codelists of spanning variables as defined in hierarchies
        ExInCodeLijst CodeLijst; // CodeLijst contains all codelists of spanning variables at lowest level. Only these codes are allowed in microdatafile
        
        //Vector<int> Vol;
        std::vector<int> Vol;
        //Vector<int> BaseDims;            // Total number of categories (including Level 0) per dimension 
        std::vector<int> BaseDims;            // Total number of categories (including Level 0) per dimension 
        //Vector< Vector< Vector<int> > > SubGTabs;
        std::vector< std::vector< std::vector<int> > > SubGTabs;
        //Vector< Hierarchy > VarStruc;    // Vector with hierarchical structures
        std::vector< Hierarchy > VarStruc;    // Vector with hierarchical structures
        Hierarchy DumHier;
        DumHier.Init();
        
        Table BTab;                      // Base table
        JJTable Tab;                     // Table in format to feed to JJ-routines

        LList SubGs;

        OUTDIR = TauOutDir;
        // Append OUTDIR to default filenames...
        LogName = PrepFile("HiTaS.log"); // LogName is global variable 
        OutName = PrepFile("JJUit.dat"); // rest is used locally
        TrickName = PrepFile("Tricks.dat");
        BTName = PrepFile("BT.dat");
        BTTabsName = PrepFile("BTTabs.dat");
        XOutName = PrepFile("XUit.dat");

        CSPSetFileNames(Solver, OUTDIR); // Append OUTDIR to filenames used in JJ-code
        
        DOSINGLEWITHSINGLE = DoSingleWithSingle;
        DOSINGLEWITHMORE = DoSingleWithMore;
        DOSINGLETONS = DOSINGLEWITHSINGLE || DOSINGLEWITHMORE;
        DOCOUNTBOUNDS = DoCountBounds;
        
        PPdebug ? PPDEBUG = true : PPDEBUG = false;
        
        setlocale(LC_NUMERIC,"english"); // Ensure that decimal _point_ should be read

        // Check for availability of license and start solver
        // Possibilities: Solver = "CPLEX", "XPRESS", "SCIP"
        // And write info to log file
        res = CheckStart(Solver,ILMFile);
        if (res != 0) return (res);
        
        TotalTime = seconds();
        //Infile = OpenFile(ParsFile,"r");

        MAX_TIME=MaxTime*60.0; // MaxTime in minutes, MAX_TIME in seconds

        // Set JJ-constants
        CSPSetDoubleConstant(Solver,JJMAXTIME,MAX_TIME);
        CSPSetDoubleConstant(Solver,JJZERO,JJzero);
	CSPSetDoubleConstant(Solver,JJINF,JJInf);
	CSPSetIntegerConstant(Solver,JJMAXCOLSLP,JJmaxcolslp);
	CSPSetIntegerConstant(Solver,JJMAXROWSLP,JJmaxrowslp);
	CSPSetIntegerConstant(Solver,JJMAXCUTSITER,JJmaxcutsiter);
	CSPSetIntegerConstant(Solver,JJMAXCUTSPOOL,JJmaxcutspool);
	CSPSetDoubleConstant(Solver,JJMINVIOLA,JJminviola);
	CSPSetDoubleConstant(Solver,JJMAXSLACK,JJmaxslack);
        CSPSetDoubleConstant(Solver,JJFEASTOL,JJFeasibilityTol);
        CSPSetDoubleConstant(Solver,JJOPTTOL,JJOptimalityTol);

 	LogFile = OpenFile(LogName.c_str(),"a");
	fprintf(LogFile,"Modular started\n\n");
	fprintf(LogFile,"MAX_TIME (secs) = %e\n",CSPGetDoubleConstant(Solver,JJMAXTIME));
	fprintf(LogFile,"ZERO            = %e\n",CSPGetDoubleConstant(Solver,JJZERO));
	fprintf(LogFile,"INF             = %e\n",CSPGetDoubleConstant(Solver,JJINF));
	fprintf(LogFile,"MAX_COLS_LP     = %d\n",CSPGetIntegerConstant(Solver,JJMAXCOLSLP));
	fprintf(LogFile,"MAX_ROWS_LP     = %d\n",CSPGetIntegerConstant(Solver,JJMAXROWSLP));
	fprintf(LogFile,"MAX_CUTS_POOL   = %d\n",CSPGetIntegerConstant(Solver,JJMAXCUTSPOOL));
	fprintf(LogFile,"MAX_CUTS_ITER   = %d\n",CSPGetIntegerConstant(Solver,JJMAXCUTSITER));
	fprintf(LogFile,"MIN_VIOLA       = %e\n",CSPGetDoubleConstant(Solver,JJMINVIOLA));
	fprintf(LogFile,"MAX_SLACK       = %e\n",CSPGetDoubleConstant(Solver,JJMAXSLACK));
	fprintf(LogFile,"FEASIBILITYTOL  = %e\n",CSPGetDoubleConstant(Solver,JJFEASTOL));
	fprintf(LogFile,"OPTIMALITYTOL   = %e\n",CSPGetDoubleConstant(Solver,JJOPTTOL));
        DOCOUNTBOUNDS ? fprintf(LogFile,"DOCOUNTBOUNDS   = true\n") : fprintf(LogFile,"DOCOUNTBOUNDS   = false\n"); 
        if (!DOSINGLETONS) fprintf(LogFile,"DOSINGLETONS    = false\n"); 
        else 
        {
            fprintf(LogFile,"DOSINGLETONS    = true\n");
            fprintf(LogFile,"   SINGLESINGLE = ");
            fprintf(LogFile,(DOSINGLEWITHSINGLE ? "true\n" : "false\n"));
            fprintf(LogFile,"   SINGLEMORE   = ");
            fprintf(LogFile,(DOSINGLEWITHMORE ? "true\n" : "false\n"));
        }
        if (PPDEBUG) fprintf(LogFile,"PPDEBUG         = true\n"); 
        fclose(LogFile);

        ReadConstants(ParsFile);          // Read some additional constants

        Info = OpenFile(FilesFile,"r");
        fscanf(Info, "%d\n", &M);        // M = number of variables
        PrintConstants(LogName.c_str(),M);      // Write constants to log-file

        // Can initialize now M is known
        //BaseDims.Make(M);                
        BaseDims.assign(M,0);                
        //VarStruc.Make(M);               
        VarStruc.assign(M,DumHier);               
        CodeLijst.assign(M,TmpSImap);
        AllCodes.assign(M,TmpSImap);
        BogusLists.assign(M,TmpStringMap);
        //VarNames is vector< vector< string> >
        VarNames.assign(M,std::vector<std::string>(0));

        // Start reading hierarchies and write info on hierarchies to hierinfo.dat
        TmpName = PrepFile("hierinfo.dat");
        Uit = OpenFile(TmpName.c_str(),"w");
        
        int Hierarchical = 1;        // Number of levels in a hierarchy
        
        for (i=1;i<=M;i++)           // Read hierarchy per variable
        {
        fgets(line,MAXLINE,Info);
        FileName = strtok(line,"\n");
        
        ReadCodeNames(FileName.c_str(), VarNames[i-1]); // VarNames has external code in correct order 
        
        //ReadHierarch(VarStruc[i], BaseDims[i], FileName.c_str(), true, BogusLists[i-1]);
        ReadHierarch(VarStruc[i-1], BaseDims[i-1], FileName.c_str(), true, BogusLists[i-1]);
        // of Var[i] (return), total number of codes (return), from FileName, removing bogussen

        fprintf(Uit,"      name    pos  level  depth\n");
        //fprintf(Uit,"%10s %6d %6d %6d\n",VarStruc[i].name, VarStruc[i].position,VarStruc[i].Level(),VarStruc[i].Depth());
        fprintf(Uit,"%10s %6d %6d %6d\n",VarStruc[i-1].name, VarStruc[i-1].position,VarStruc[i-1].Level(),VarStruc[i-1].Depth());
   
        //Hierarchical *= VarStruc[i].Depth();
        Hierarchical *= VarStruc[i-1].Depth();
        
        //PrintHierarch(VarStruc[i],*Uit);
        PrintHierarch(VarStruc[i-1],*Uit);

        //AllCodes[i-1][VarStruc[i].name] = VarStruc[i].position;
        AllCodes[i-1][VarStruc[i-1].name] = VarStruc[i-1].position;
        
        //MakeCodeList(VarStruc[i], CodeLijst[i-1], AllCodes[i-1]);
        MakeCodeList(VarStruc[i-1], CodeLijst[i-1], AllCodes[i-1]);
        // AllCodes[i-1] has all codes of variable i, external code -> internal code
        // (excluding BogusCodes)
        // CodeLijst[i-1] has all codes (that can appear in the data) of variable i, external code -> internal code
        }
        fclose(Uit);
 
        fgets(line,MAXLINE,Info);
        BTabFileName = strtok(line,"\n");
        fclose(Info);

        JJUit=OpenFile(OutName.c_str(),"w"); // removes "old" JJUit.dat file
        fclose(JJUit);

        if (PPDEBUG) 
        {
            JJUit = OpenFile(TrickName.c_str(),"w"); // removes "old" tricks.dat file
            fclose(JJUit);
        }

        if ((Hierarchical == 1) && (LINKED == 0)) // Non-Hierarchical Table
        {
            LogPrintf(LogName,"Non-Hierarchical Table\n");
	
            BTab.Make(BaseDims);              // BTab will only contain value, status, safety ranges and weight
	
            res = BTab.ReadData(BTabFileName.c_str());    // Read Base Table
    
            if(res) {
                FreeHierVector(VarStruc); 
                return(res);
            } // return error
	
            ReadBTInfo(BTab,M,BTName.c_str(),"buzs");     // Read backtrack information if available
    
            // Read value and determine maximum protection levels of primary unsafe cells
            //MaxBounds.resize(2);            // Currently only lpl and upl
            //MaxBounds[0] = MaxBounds[1] = 0; // Initialize to 0
    
            // Change BTab to JJ-format
            //Vector< Vector<int> > IndexTab;
            std::vector< std::vector<int> > IndexTab;

            FireUpdateGroup(100);
            FireUpdateTables(1);

            DefineNonHierStruc(VarStruc, IndexTab); // IndexTab contains coordinates of table cells

            realdim=FillTable(Tab, IndexTab, BTab, MaxBounds, false, MaxCost); // DoCost = false
                
            if (Tab.Skip) // If Tab.Skip=true at this place, complete table has to be skipped => should not happen!
            {
                Tab.Free(); 
                BTab.Free(); 
                return(HITAS_SKIP_ERROR);
            }

            //Not needed for std::vector
            //for (i=1;i<=M;i++)
            //    IndexTab[i].Free();
            //IndexTab.Free();
            
            PPDEBUG ? JJUit=OpenFile(OutName.c_str(),"a") : JJUit=OpenFile(OutName.c_str(),"w");

            Tab.PrintData(*JJUit);
            fclose(JJUit);

            // Apply protection
            try
            {
                MaxCost = 0.0;
                ReturnCode = Suppress(Solver,Tab,realdim,false,MaxCost,Hierarchical,ObjVal); 
            }
            catch(int code)
            {
                WriteErrorToLog(LogName,code);
                CloseSolver(Solver);
                return(code);
            }
            JJTotalTime += JJTime;

            PPDEBUG ? JJUit=OpenFile(OutName.c_str(),"a") : JJUit=OpenFile(OutName.c_str(),"w");
            Tab.PrintData(*JJUit);
            fclose(JJUit);

            LogPrintf(LogName,"ObjectValue: ");
            LogPrintf(LogName,to_string(ObjVal));
            LogPrintf(LogName,"\n");

            Update(BTab, Tab, ReturnCode, MaxBounds); // Update BTab with secondaries

            Tab.Free();

            FireUpdateTables(100);

            // Write results

            Info = OpenFile(FilesFile,"r");
            fscanf(Info,"%d ",&M);
            for (i=1;i<=M+1;i++)           // Hierarchy files and BTab file
                fgets(line,MAXLINE,Info);
            fgets(line,MAXLINE,Info);
            FileName = strtok(line,"\n");
            fclose(Info);
                
            TabUit=OpenFile(FileName.c_str(),"w");
            LogPrintf(LogName,"Going to write to ");
            LogPrintf(LogName,FileName.c_str());

            BTab.APrintTabel(*TabUit,"bm");  //Only coordinates of secondaries

            fclose(TabUit);
            BTab.Free();
        }
        else					 // Hierarchical Table
        {
            while (DaCapo)                     // DaCaop = true as long as backtracking is needed
            {                                  // Initialize problem
                DaCapo=false;              // Start presuming no backtracking

                if (PPDEBUG)
                {	
                    LogPrintf(LogName,"Track Number ");
                    LogPrintf(LogName,to_string(aantBT));
                    LogPrintf(LogName,"\n");
                }
   
                BTab.Make(BaseDims);          // BTab will only contain value, status, safety ranges and weight

                res = BTab.ReadData(BTabFileName.c_str());    // Read Base Table information
                
                if (PPDEBUG) BTab.PrintData();
                        
                if(res) {
                    FreeHierVector(VarStruc); 
                    return(res);
                } // return error

                ReadBTInfo(BTab,M,BTName.c_str(),"buzs");     // Read backtracking information if available
                        
                if (PPDEBUG) LogPrintf(LogName,"\nStarting processing tables\n\n");

                res = DefineSubGroups(VarStruc,SubGs);       // Define sub-groups of tables

                if (PPDEBUG) 
                {
                    LogPrintf(LogName,"Number of SubGroups: ");
                    LogPrintf(LogName,to_string(SubGs.size()));
                    LogPrintf(LogName,"\n");
                }

                if(res) return(res); // return error

                if (PPDEBUG){
                    SubGs.Printf();
                    LogPrintf(LogName,"Group 1\n");
                }

                for (i=1;i<=SubGs.size();i++)          // Grand total (0,0,0,...,0) does not need to be considered (assumed not to be primary unsafe)
                {         
                    if (PPDEBUG)
                    {
                        if ((i>1) && (SubGs[i]->Gnr != SubGs[i-1]->Gnr))
                        {
                            LogPrintf(LogName,"Group ");
                            LogPrintf(LogName,to_string(SubGs[i]->Gnr));
                            LogPrintf(LogName,"\n");
                        }
                    }

                    //DefineSubGTabs(VarStruc, *SubGs[i], SubGTabs); // SubGTabs[j] has coordinates (no values) of j-th table of subgroup i
                    DefineSubGTabs(VarStruc, *SubGs[i], SubGTabs); // SubGTabs[j-1] has coordinates (no values) of j-th table of subgroup i

                    if (PPDEBUG) 
                    {
                        TmpName = PrepFile("GroupInfo.txt");
                        XUit = OpenFile(TmpName.c_str(),"a");
                        LogFile = OpenFile(LogName.c_str(),"a");
                        LogPrintf(LogName," sub (");
                        fprintf(XUit," sub(");
                        for (unsigned int uk=0;uk<SubGs[i]->Gname.size();uk++)
                        {
                            LogPrintf(LogName,to_string(SubGs[i]->Gname[uk]));
                            fprintf(XUit,"%d",SubGs[i]->Gname[uk]);
                        }
                        LogPrintf(LogName,")\n");
                        fprintf(XUit,")\n");
                        fclose(XUit);
                    }

                    FireUpdateGroup((int) 100*i/SubGs.size());
                    FireUpdateTables(1);

                    int aant=0;
     
                    char tmpchar[3];

                    TmpTableName = "";
                    for (unsigned int uj=0;uj<SubGs[i]->Gname.size();uj++)
                    {
                        //_itoa(SubGs[i]->Gname[uj],tmpchar,10);
                        sprintf(tmpchar,"%d",SubGs[i]->Gname[uj]);
                        TmpTableName = TmpTableName + tmpchar;
                    }

                    for (size_t j=1;j<=SubGTabs.size();j++) // Loop over all tables in SubGroep i
                    { 	   
                        if (PPDEBUG)
                        {
                            XUit = OpenFile(LogName.c_str(),"a");
                            //PrintSubG(*XUit,SubGs[i],SubGTabs[j]);
                            PrintSubG(*XUit,SubGs[i],SubGTabs[j-1]);
                            fclose(XUit);
                        }
                                        
                        MaxBounds.resize(2);          // Currently only lpl and upl
                        MaxBounds[0] = MaxBounds[1] = 0.0001; // Initialize to epsilon
		
                        if (PPDEBUG)			// Write info about "dirty tricks" in debug-mode
                        {
                            JJUit=OpenFile(TrickName.c_str(),"a");
                            for (unsigned int uk=0;uk<SubGs[i]->Gname.size();uk++) fprintf(JJUit,"%d",SubGs[i]->Gname[uk]);
                            fprintf(JJUit,"\n");
                            fclose(JJUit);
                        }

                        
                        //if (DISTANCE!=0)
                        //    realdim = FillTable(Tab, SubGTabs[j], BTab, MaxBounds, false, MaxCost);
                        //else
                        //    realdim = FillTable(Tab, SubGTabs[j], BTab, MaxBounds, true, MaxCost);

                        //realdim = FillTable(Tab, SubGTabs[j], BTab, MaxBounds, DISTANCE==0, MaxCost);
                        realdim = FillTable(Tab, SubGTabs[j-1], BTab, MaxBounds, DISTANCE==0, MaxCost);

                        if (PPDEBUG) LogPrintf(LogName,".");

                        if (realdim == -11)    // Table is to be skipped, make a note about that in LogFile
                        {
                            ReturnCode = -11;  // Give ReturnCode a temporary nonsense value
                            if (PPDEBUG)
                                LogPrintf(LogName,"Skipped Table\n");
                            else
                            {
                                LogPrintf(LogName,"Skipped table in subgroup (");
                                LogPrintf(LogName,TmpTableName.c_str());
                                LogPrintf(LogName,")\n");
                            }
                        }

                        if (realdim >= 0)         // We should have something to do
                        {
                            char VolgNr[10]; // i.e., up to 999999999 subtables possible
                            //_itoa(j,VolgNr,10);
                            snprintf(VolgNr,10,"%d",(int) j); // snprintf is safer than sprintf
                            TableName = TmpTableName + '.' + VolgNr;
                                                
                            if (ListsOfTables.find(TableName) == ListsOfTables.end()) // Not found
                            {
                                ListsOfTables[TableName]=""; // New table entry
                                TableList.clear();           // No statuses known yet
                            }
                            else
                                TableList = ListsOfTables[TableName];

                            // Check whether table is new (first time or different from previous run due to backtracking))
                            newtab = TestNewTable(TableList,Tab,BTab); 

                            switch (newtab)
                            {
                                case OLD_TAB:  // Read old results
                                    ReturnCode = ReadOldStats(TableList,Tab);
                                    Update(BTab, Tab, ReturnCode, MaxBounds); // Update suppressions in BTab
                                    break;
                                case NEW_TAB:
                                    PPDEBUG ? JJUit=OpenFile(OutName.c_str(),"a") : JJUit=OpenFile(OutName.c_str(),"w");

                                    for (unsigned int uk=0;uk<SubGs[i]->Gname.size();uk++)
                                        fprintf(JJUit,"%d",SubGs[i]->Gname[uk]);
                                                                
                                    fprintf(JJUit,"\n");
                                    fclose(JJUit);

                                    Tab.SaveBasisStatsBefore(TableList,BTab); // Save statuses of table before JJ
                                    try                                       // Apply protection (JJ)
                                    {
                                        /*if (DISTANCE!=0) 
                                            ReturnCode = Suppress(Solver,Tab,realdim,false,MaxCost,Hierarchical,ObjVal);
                                        else
                                            ReturnCode = Suppress(Solver,Tab,realdim,true,MaxCost,Hierarchical,ObjVal);*/
                                        ReturnCode = Suppress(Solver,Tab,realdim,DISTANCE==0,MaxCost,Hierarchical,ObjVal);
                                    }
                                    catch(int code)
                                    {
                                        WriteErrorToLog(LogName,code);
                                        CloseSolver(Solver);
                                        return(code);
                                    }

                                    JJTotalTime += JJTime;
		       
                                    Update(BTab, Tab, ReturnCode, MaxBounds); // Update suppressions in BTab
		       
                                    Tab.SaveBasisStatsAfter(TableList,BTab);  // Save statuses of table after JJ
                                    ListsOfTables[TableName] = TableList;
                                    break;
                                case -90:      // Error: stop
                                    LogPrintf(LogName,"Error in TestNewTable()\nExiting\n");
                                    //fcloseall();
                                    WriteErrorToLog(LogName,HITAS_SHOULDNOTHAPPEN);
                                    return(HITAS_SHOULDNOTHAPPEN);
                                    break;
                            }

                            switch (ReturnCode)
                            {
                                case 100: // Marginal s-cell suppressed, i.e. BackTracking
                                    AddHistory(Tab,BTab);       // Write backtracking information
                                    if (PPDEBUG)
                                    {
                                        LogPrintf(LogName," with BackTracking\n");
                                        BTUit = OpenFile(BTTabsName.c_str(),"a");
                                        fprintf(BTUit,"Track %d, Table ",aantBT);
                                        for (unsigned int uk=0;uk<SubGs[i]->Gname.size();uk++)
                                            fprintf(BTUit,"%d",SubGs[i]->Gname[uk]);
                                        fprintf(BTUit,"\n");
                                        Tab.PrintData(*BTUit);
                                        fprintf(BTUit,"\n");
                                        fclose(BTUit);
                                    }
                                    DaCapo=true;				 // We will Backtrack
                                    break;
                                case 5: // Table containing status 'x' was returned
                                    XUit=OpenFile(XOutName.c_str(),"a");
                                    //PrintSubG(*XUit,SubGs[i],SubGTabs[j]);
                                    PrintSubG(*XUit,SubGs[i],SubGTabs[j-1]);
                                    fprintf(XUit,"\nAfter JJ:\n");
                                    Tab.PrintData(*XUit);
                                    fclose(XUit);
                                break;
                            }
                                                
                            switch (ReturnCode)
                            {
                                case 0:
                                case 100:
                                    PPDEBUG ? JJUit=OpenFile(OutName.c_str(),"a") : JJUit=OpenFile(OutName.c_str(),"w");
                                                                                
                                    Tab.PrintData(*JJUit);    // Tab in format to feed to JJ-routines
                                    if (PPDEBUG)
                                    {
                                        fprintf(JJUit,"Objectfunction value: %d\n",ObjVal);
                                        fprintf(JJUit,"Time needed: %10.2lf seconds\n",JJTime);
                                    }
                                    fclose(JJUit);
                                    aant++;
                                    break;
                            }
                        }
                                        
                        FireUpdateTables((int) 100*j/SubGTabs.size());
        
                        if (ReturnCode == 9)	// Error: infeasible table, need to stop
                        {
                            WriteErrorToLog(LogName,LB_EQ_UB_EQ_INF);

                            LogPrintf(LogName,"\nInfeasible (sub)table problem, see JJUit.dat and InFeas.dat for problem table\nCoordinates of total general of problem table: ");

                            //Vol.Make(Tab.Dim());
                            Vol.resize(Tab.Dim());
                            k=1;
                            for (tel=1;tel<=Tab.BDim();tel++)
                                //if (SubGTabs[j][tel][1] != 0) Vol[k++]=tel;
                                if (SubGTabs[j-1][tel-1][0] != 0) {Vol[k-1]=tel; k++;}

                            for (k=1;k<Tab.Dim();k++)
                            {
                                //LogPrintf(LogName,VarNames[Vol[k]-1][Tab.baseijk[0][Vol[k]]].c_str());
                                LogPrintf(LogName,VarNames[Vol[k-1]-1][Tab.baseijk[0][Vol[k-1]-1]].c_str());
                                LogPrintf(LogName,", ");
                            }
		  
                            //LogPrintf(LogName,VarNames[Vol[Tab.Dim()]-1][Tab.baseijk[0][Vol[Tab.Dim()]]].c_str());
                            LogPrintf(LogName,VarNames[Vol[Tab.Dim()-1]-1][Tab.baseijk[0][Vol[Tab.Dim()-1]-1]].c_str());
                            LogPrintf(LogName,"\n");

                            //Vol.Free();
                            Vol.clear();
		  
                            j = SubGTabs.size() + 1; // to jump out of j-loop
                            i = SubGs.size() + 1;    // to jump out of i-loop
                            DaCapo = false;		 // to jump out of while-loop
                        }

                        Tab.Free();              // Tab not longer needed
                    } // end for-j-loop

                    if (PPDEBUG)
                    {
                        if (ReturnCode != 9) // Do not print in case of infeasible table
                        {
                            LogPrintf(LogName,"\n  sub (");
                            LogPrintf(LogName,to_string(SubGs[i]->Gname[0]));

                            for (unsigned int uk=1;uk<SubGs[i]->Gname.size();uk++)
                            {
                                LogPrintf(LogName,to_string(SubGs[i]->Gname[uk]));
                            }
                            LogPrintf(LogName,") ");
                            LogPrintf(LogName,to_string(aant));
                            LogPrintf(LogName," out of ");
                            LogPrintf(LogName,to_string(SubGTabs.size()));
                            LogPrintf(LogName,"\n");
                        }
                    }
      
                    JJaant+=aant;
                    Totaant+=SubGTabs.size();

                    //FreeAll(SubGTabs);


                    if (DaCapo)               // BackTracking
                    {
                        if (i==SubGs.size())  // Final table
                            {aantBT++;break;}
                        else
                            if (SubGs[i+1]->Gnr!=SubGs[i]->Gnr)  // first finish Group of tables
                                {aantBT++;break;}
                    }
                } // end for-i-loop

                if (!DaCapo)   // Finished!
                {
                    Info = OpenFile(FilesFile,"r");
                    fscanf(Info, "%d\n", &M);        // Read number of variables
                    for (i=1;i<=M+1;i++)             // Hierarchy files and BTAB file
                        fgets(line,MAXLINE,Info);
                    fgets(line,MAXLINE,Info);
                    FileName = strtok(line,"\n");
                    fclose(Info);
                                
                    TabUit=OpenFile(FileName.c_str(),"w");
	 
                    BTab.APrintTabel(*TabUit,"bm");  //Only write coordinates of secondary suppressions
                                
                    fclose(TabUit);

                    LogPrintf(LogName,"Number of BackTrackings: ");
                    LogPrintf(LogName,to_string(aantBT));
                    LogPrintf(LogName,"\n");
                }
	
                BTab.Free();

                SubGs.Free();
            }
        }
 
        // If finished completely, free all other memory
        FreeHierVector(VarStruc);
        //BaseDims.Free();
        BaseDims.clear();

        CSPFreeFileNames(Solver);
        CloseSolver(Solver);
        
        if (PPDEBUG) 
        {
            LogPrintf(LogName,"Number of Tables: ");
            LogPrintf(LogName,to_string(JJaant));
            LogPrintf(LogName,"\n");
        }


        if (ReturnCode == 9) // Error: infeasible table, need to stop
        {
            //fcloseall();
            return(LB_EQ_UB_EQ_INF);
        }

        TotalTime = seconds() - TotalTime;
        Uit = OpenFile(LogName.c_str(),"a");
        fprintf(Uit,"Totaltime: %10.2lf seconds, with %10.2lf seconds JJ-code (= %5.2lf%%)\n",TotalTime,JJTotalTime,100*(JJTotalTime/TotalTime));
        fclose(Uit);
 
        //fcloseall();

        //D.Free();

        return 0;
}
