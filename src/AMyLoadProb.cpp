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

#include <stdio.h>
#include <math.h>
#include <time.h>
#include "Aconst.h"
#include "AErrors.h"
#include "ATabs.h"
#include "AMyLoadProb.h"
#include "AMyIO.h"
#include "resource.h"
#include "WrapCSP.h"
#include <vector>

void PrintAdditionalInfo(std::vector<addcellinfo>& Cells, std::vector<addsuminfo>& Constraints, char** names, int i0);

extern bool PPDEBUG;

// Conversion functions on coordinates versus position

// 1-dim: m = i
// 2-dim: m = i*N[2] + j
// 3-dim: m = i*N[3]*N[2] + j*N[3] + k
// 4-dim: m = i*N[4]*N[3]*N[2] + j*N[4]*N[3] + k*N[4] + s
// n-dim: ...

// TabDims[1] = N[Dim], TabDims[2] = N[Dim-1], ..., TabDims[Dim-1] = N[2], TabDims[Dim] = 1

// Converts position m into coordinates Gijk
static bool Calc_ijk(int m, Vector<int>& Gijk, Vector<int> TabDims)
{
  int i;
  int prod=1;
  div_t x;
  int y=m;
  if (Gijk.size() == 1)
  {
	Gijk[1] = m;
	prod = m;
  }
  else
  {
	for (i=1;i<Gijk.size();i++)
	{
	    x = div(y,TabDims[i]);
	    Gijk[Gijk.size()-i+1] = x.rem;
	    y=x.quot;
		prod *= Gijk[Gijk.size()-i+1];  // If at least one 0 then marginal cell
	}
    Gijk[1]=x.quot;
    prod *= Gijk[Gijk.size()-i+1];
  }
  return (prod == 0); // If at least one 0 then marginal cell
}

// Converts position Gijk into co�rdinates m
static bool Calc_m(Vector<int> Gijk, Vector<int> TabDims, int& m)
{
	int i, prod=1;
	int factor=1;
	
	for (i=1;i<=TabDims.size();i++)
		factor *= TabDims[i];		// Start met N[Dim]*N[Dim-1]*...*N[2]*1
	
	m = 0;
	for (i=1;i<=Gijk.size();i++)
	{
		factor /= TabDims[TabDims.size()-i+1];  // i=1: deel door TabDims[Dim] = 1
		m += Gijk[i]*factor;
		prod *= Gijk[i];
	}
	return (prod == 0); // If at least one 0 then marginal cell
}

// Creates CellName from co�rdinates
static std::string CellName(Vector<int> Gijk)
{
        int i;
        std::string tmpName;
        
        tmpName.append("x");
        for (i=1;i<=Gijk.size();i++)
        {
            tmpName.append("-");
            tmpName.append(to_string(Gijk[i]));
	}

	return tmpName;
}

//////////////////////////////////////////////////////////////////
// SubTable with possibly  negative cell-values
// If necessary, adjust Tab.data Tab.ub, Tab,lb
// Tab.weight and Tab.cost should be given correctly in BaseTab
//
//void LiftSubTableUp(TauHiTaSCtrl* ppCtrl, JJTable& Tab)
static void LiftSubTableUp(JJTable& Tab)
{
	int Dim = Tab.Dim();
	int m,i,marg;
	int fac;
	FILE* Debug;
        FILE* logfile;
	std::string buffer;

	if (PPDEBUG)
	{
            logfile = OpenFile(LogName.c_str(),"a");
            fprintf(logfile,"Table lifted by %lf on each interior cell.\n",(-Tab.MinInteriorVal + 1));
            fclose(logfile);

            buffer.clear();
            buffer.append(OUTDIR);
            buffer.append("Lifting.log");
            
            Debug = fopen(buffer.c_str(),"a");
            fprintf(Debug,"Lifting Table\n");
	    Tab.PrintData(*Debug);
	}

	for (m=0;m<Tab.Size();m++)
	{
		marg=1;
		for (i=1;i<=Dim;i++)
			marg *= Tab.ijk[i][m];

		if (marg != 0) // Interior cell
		{
			Tab.data[m] += -Tab.MinInteriorVal + 1;
			Tab.lb[m] += -Tab.MinInteriorVal + 1;			// Maybe set to 0?
			Tab.ub[m] += -Tab.MinInteriorVal + 1;
		}
		else       // Marginal cell
		{
			fac = 1;
			for (i=1;i<=Dim;i++)
			{
				if (Tab.ijk[i][m] == 0) fac *= (Tab.N[i] - 1);
			}
			Tab.data[m] += fac*(-Tab.MinInteriorVal + 1);
			Tab.lb[m] += fac*(-Tab.MinInteriorVal + 1);		// Maybe set to 0?
			Tab.ub[m] += fac*(-Tab.MinInteriorVal + 1);
		}
	}

	if (PPDEBUG)
	{
		fprintf(Debug,"Lifted to\n");
		Tab.PrintData(*Debug);
		fclose(Debug);
	}
}

int LoadTableIntoPCSP(const char* Solver, JJTable& Tab)
{
        int i, j, m, t, s, k, Nzeros, pos, ncells, nsums, nlist, fac;//, nAddCellsBase;
        unsigned int usi;
        int* zeros, *position, *ncard, *list,  *weights;
        double *rhs, *data, *lb, *ub, *lpl, *upl, *spl;
	char *status;
        char **names;
        signed char *val;
	bool marginal;
        std::string tmpname;

	Vector<int> Gijk;
	Gijk.Make(Tab.Dim());

	Vector<int> TabDims;
	TabDims.Make(Tab.Dim());
        
        ////////////////////////////////////////////////////////////////////////////////////
	// If negative cell value is present in interior then make cell values positive
	// NB: In the current function LoadTableIntoPCSP additional cells need to be lifted
	// as well. See later in this routine.
	//
	if (Tab.MinInteriorVal < 0) LiftSubTableUp(Tab); 
	////////////////////////////////////////////////////////////////////////////////////
        
	zeros = (int *)malloc(Tab.Dim()*sizeof(int));

	ncells = Tab.Size();			// ncells = N[1]*N[2]*...*N[Dims]

	nsums = 0;     				// Number of sum-restrictions
        for (i=1;i<=Tab.Dim();i++)		// 1-dim: 1, 2-dim: N[1] + N[2], 3-dim: N[1]*N[2] + N[1]*N[3] + N[2]*N[3],
		nsums += ncells/Tab.N[i];	// 4-dim: N[1]*N[2]*N[3] + N[1]*N[2]*N[4] + N[1]*N[3]*N[4] + N[2]*N[3]*N[4], n-dim: ...

	nlist = Tab.Dim()*ncells;

	// Add memory to allow for additional cells and constraints
        ncells += Tab.AdditionalCells.size(); // Additional Cells from Tricks
        nsums += Tab.AdditionalConstraints.size(); // Additional Constraints from Tricks
        for (usi=0;usi<Tab.AdditionalConstraints.size();usi++)
			nlist += Tab.AdditionalConstraints[usi].ncard;

	// Allocate memory
	names = (char **)malloc( ncells*sizeof(char *) );
        position = (int *)malloc( ncells*sizeof(int) );
	
	data = (double *)malloc( ncells*sizeof(double) );
	weights = (int *)malloc( ncells*sizeof(int) );
	lb = (double *)malloc( ncells*sizeof(double) );
	ub = (double *)malloc( ncells*sizeof(double) );
	lpl = (double *)malloc( ncells*sizeof(double) );
	upl = (double *)malloc( ncells*sizeof(double) );
	spl = (double *)malloc( ncells*sizeof(double) );
	status = (char *)malloc( ncells*sizeof(char) );
	list  = (int *)malloc( nlist*sizeof(int) );
        val   = (signed char *)malloc( nlist*sizeof(signed char) );
	ncard = (int *)malloc( nsums*sizeof(int) );
        rhs   = (double *)malloc( nsums*sizeof(double) );

         if( !position || !names || !spl){
                LogPrintf(LogName,"ERROR: Not enough memory for Loading Table into PCSP\n");
                WriteErrorToLog(LogName,JJ_NOTENOUGHMEMORY);
                throw(JJ_NOTENOUGHMEMORY);
        }
        if( !ncard || !rhs || !list || !val )
        {
                LogPrintf(LogName,"ERROR: Not enough memory for GENERAL DATA STRUCTURE\n");
                WriteErrorToLog(LogName,JJ_NOTENOUGHMEMORY);
                throw(JJ_NOTENOUGHMEMORY);
        }

	// TabDims[1] = N[Dim], TabDims[2] = N[Dim-1], ..., TabDims[Dim-1] = N[2], TabDims[Dim] = 1
	for (i=1;i<Tab.Dim();i++)
		TabDims[i] = Tab.N[Tab.Dim()-i+1];
	TabDims[Tab.Dim()] = 1;

	// Construct sum-restrictions
	t = s = 0;

	for (m=0;m<Tab.Size();m++)
	{
		marginal = Calc_ijk(m,Gijk,TabDims);  // Calc_ijk returns true in case of marginal cell
                names[m] = (char*) malloc((CellName(Gijk).size()+1)*sizeof(char));
		strcpy(names[m],CellName(Gijk).c_str());
		data[m] = Tab.data[m];
		weights[m] = Tab.weight[m];
		lpl[m] = Tab.lpl[m];
		upl[m] = Tab.upl[m];
		spl[m] = 0;     // sliding protection level not used in HiTaS
		if ((Tab.status[m] == 'b') || (Tab.status[m] == 'm')) // Backtrack and secondary cells should be primary cells in this round
			Tab.status[m] = 'u';
		status[m] = Tab.status[m];
		lb[m] = Tab.lb[m];
		ub[m] = Tab.ub[m];

		if (marginal) 
		{
			Nzeros = 0;
			for (i=1;i<=Tab.Dim();i++)
				if(Gijk[i] == 0) zeros[Nzeros++] = i;	// Which coordinates are zero? Final value of Nzeros is number of zero-coordinates.
			//Loop over one "zero-coordinate" at a time to construct the restriction in that direction
			for (j=0;j<Nzeros;j++)
			{
				rhs[t] = 0;
				for (k=0;k<Tab.N[zeros[j]];k++)
				{
					Gijk[zeros[j]] = k;			// Loop over zero-coordinate
					Calc_m(Gijk,TabDims,pos);
					val[s] = ( k ? 1 : -1 );
                                        list[s++] = pos;
				}
				ncard[t++] = Tab.N[zeros[j]];
				Gijk[zeros[j]] = 0;				// In case of more zeros, reset looped one to original value (== 0)
			}
		}
	}

        for (usi=0;usi<Tab.AdditionalCells.size();usi++)
	{
		// Adding Cell from tricks
		data[m+usi] = Tab.AdditionalCells[usi].value;
                
                tmpname.clear();
                tmpname.append("add-");
                tmpname.append(to_string(m+usi));
                
		names[m+usi] = (char*)malloc((tmpname.size()+1)*sizeof(char));
                strcpy(names[m+usi],tmpname.c_str());
                
		weights[m+usi] = Tab.AdditionalCells[usi].weight;
		status[m+usi] = Tab.AdditionalCells[usi].status;
		lpl[m+usi] = Tab.AdditionalCells[usi].lpl;
		upl[m+usi] = Tab.AdditionalCells[usi].upl;
		spl[m+usi] = Tab.AdditionalCells[usi].spl;
		ub[m+usi] = Tab.AdditionalCells[usi].ub;
		lb[m+usi] = Tab.AdditionalCells[usi].lb;

		//Adding Constraint from tricks
		rhs[t+usi] = Tab.AdditionalConstraints[usi].rhs;
		ncard[t+usi] = Tab.AdditionalConstraints[usi].ncard;
		for (j=0;j<Tab.AdditionalConstraints[usi].ncard;j++)
		{
			val[s] = (j ? 1 : -1);
			list[s++] = Tab.AdditionalConstraints[usi].Cells[j];
		}
                
                if (Tab.MinInteriorVal < 0) 
		{
                        fac = (Tab.AdditionalConstraints[usi].ncard - 1);
			data[m+usi] += fac*(-Tab.MinInteriorVal + 1);
			Tab.AdditionalCells[usi].value = data[m+usi];
			ub[m+usi]   += fac*(-Tab.MinInteriorVal + 1);
			Tab.AdditionalCells[usi].ub = ub[m+usi];
			lb[m+usi]   += fac*(-Tab.MinInteriorVal + 1);
			Tab.AdditionalCells[usi].lb = lb[m+usi];
		}
	}

	if (PPDEBUG)
	{
            if (Tab.AdditionalCells.size() > 0)
		PrintAdditionalInfo(Tab.AdditionalCells,Tab.AdditionalConstraints,names,Tab.Size());
	}

        if ( CSPloadprob(Solver,nsums,rhs,ncells,data,weights,status,lpl,upl,lb,ub,names,ncard,list,val) )
        {
                LogPrintf(LogName," ERROR: loading the CSP instance\n");
                WriteErrorToLog(LogName,HITAS_JJNOTLOADED);
		throw(HITAS_JJNOTLOADED);
        }

	// Free memory
	Gijk.Free();
	TabDims.Free();
        for(m=0;m<ncells;m++)
        {
                free( names[m] );
                names[m] = NULL;
        }
        free(names); names=NULL;
        free(position); position=NULL;
        free(list); list=NULL;
        free(val); val=NULL;
        free(ncard); ncard=NULL;
        free(rhs); rhs=NULL;
	free(spl); spl=NULL;
	free(data); data=NULL;
	free(weights); weights=NULL;
	free(lb); lb=NULL;
	free(ub); ub=NULL;
	free(lpl); lpl=NULL;
	free(upl); upl=NULL;
	free(status); status=NULL;
	free(zeros); zeros=NULL;

	return 0;	
}

void PrintAdditionalInfo(std::vector<addcellinfo>& Cells, std::vector<addsuminfo>& Constraints, char** names, int i0)
{
	FILE *JJ;
	int j;
        unsigned int i;
		
	JJ = OpenFile(PrepFile("Tricks.dat").c_str(),"a");
        fprintf(JJ,"Number of additional cells: %d\n",Cells.size());
        for (i=0; i<Cells.size();i++)
	{
		fprintf(JJ,"%15s %15.5lf %5d %c %15.5lf %15.5lf %15.5lf %15.5lf %15.5lf\n",names[i0+i],Cells[i].value,Cells[i].weight,Cells[i].status,
						Cells[i].lb,Cells[i].ub,Cells[i].lpl,Cells[i].upl,Cells[i].spl);
	}
        fprintf(JJ,"Number of additional constraints: %d\n",Constraints.size());
        for (i=0;i<Constraints.size();i++)
	{
		fprintf(JJ,"%1.0lf %3d: ",Constraints[i].rhs, Constraints[i].ncard);
		fprintf(JJ,"%15s (%d) ",names[Constraints[i].Cells[0]],-1);
		for (j=1;j<Constraints[i].ncard;j++)
			fprintf(JJ,"%15s (%d) ",names[Constraints[i].Cells[j]],1);
		fprintf(JJ,"\n");
	}
	fclose(JJ);
}
