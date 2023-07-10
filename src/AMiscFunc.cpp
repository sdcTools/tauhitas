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

#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <cassert>
#include "AErrors.h"

#include "resource.h"

#include "AMiscFunc.h"
#include "AMyIO.h"
#include "AMyLoadProb.h"
#include "WrapCSP.h"

double MyRound(double x, int dec)
{
	double tmp;
	tmp = x*pow(10.0,dec);
	return floor(tmp+0.5)*pow(10.0,-dec);
}

double seconds()
{
  return((double)clock()/CLOCKS_PER_SEC);
}

// 1-dim: m = i
// 2-dim: m = i*N[2] + j
// 3-dim: m = i*N[3]*N[2] + j*N[3] + k
// 4-dim: m = i*N[4]*N[3]*N[2] + j*N[4]*N[3] + k*N[4] + s

// TabDims[1] = N[Dim], TabDims[2] = N[Dim-1], ..., TabDims[Dim] = 1

// Returns TRUE if cell in margin
//static bool Getijk(int m, Vector<int>& Gijk, Vector<int> TabDims)
static bool Getijk(int m, std::vector<int>& Gijk, std::vector<int> TabDims)
{
    // Assume that Gijk.size() >= 1
    assert(Gijk.size()>=1);
    //int i;
    size_t i;
    int prod=1;
    div_t x={0};
    int y=m;
    if (Gijk.size() == 1){
	//Gijk[1] = m;
        Gijk[0] = m;
	prod = m;
    }
    else{
	//for (i=1;i<Gijk.size();i++)
        for (i=1;i<Gijk.size();i++){
	    //x = div(y,TabDims[i]);
            x = div(y,TabDims[i-1]);
	    //Gijk[Gijk.size()-i+1] = x.rem;
            Gijk[Gijk.size()-i] = x.rem;
	    y=x.quot;
            //prod *= Gijk[Gijk.size()-i+1];  // If at least one 0 then marginal cell
            prod *= Gijk[Gijk.size()-i];  // If at least one 0 then marginal cell
	}
        //Gijk[1]=x.quot;
        Gijk[0]=x.quot;
        //prod *= Gijk[Gijk.size()-i+1];
        prod *= Gijk[Gijk.size()-i];
    }
    return (prod == 0); // If at least one 0 then marginal cell
}

////////////////////////////////////////////////////////////////////////////////////////////
// 1-dim: m = i
// 2-dim: m = i*N[2] + j
// 3-dim: m = i*N[3]*N[2] + j*N[3] + k
// 4-dim: m = i*N[4]*N[3]*N[2] + j*N[4]*N[3] + k*N[4] + s

// TabDims[1] = N[Dim], TabDims[2] = N[Dim-1], ..., TabDims[Dim-1] = N[2], TabDims[Dim] = 1
//static bool Calc_m(Vector<int> Gijk, Vector<int> TabDims, int& m)
static bool Calc_m(std::vector<int> Gijk, std::vector<int> TabDims, int& m)
{
	int prod=1;
	int factor=1;
	
	for (size_t i=1;i<=TabDims.size();i++)
		//factor *= TabDims[i];		// Start with N[Dim]*N[Dim-1]*...*N[2]*1
            factor *= TabDims[i-1];		// Start with N[Dim]*N[Dim-1]*...*N[2]*1
	
	m = 0;
	for (size_t i=1;i<=Gijk.size();i++)
	{
		//factor /= TabDims[TabDims.size()-i+1];  // i=1: divide by TabDims[Dim] = 1
                factor /= TabDims[TabDims.size()-i];  // i=1: divide by TabDims[Dim] = 1
		//m += Gijk[i]*factor;
                m += Gijk[i-1]*factor;
		//prod *= Gijk[i];
                prod *= Gijk[i-1];
	}
	return (prod == 0); // If at least one 0 then marginal cell
}

// Treats a single cell in a margin
static void MarginalCell(JJTable& Tab, int m, cell* BCell, int &MargWeight)
{
  // Initialize BCell as empty
  Tab.data[m] = 0;                   
  Tab.lpl[m]  = 0;
  Tab.upl[m]  = 0;
  Tab.count[m] = 0;
  Tab.status[m] = 'z';               
  Tab.costs[m] = 0;
  double LPRO = 0.9;				 // Default +/- 10%
  double UPRO = 1.1;

  // If BCell is not empty, adjust
  if (BCell != NULL)              
  {
    Tab.count[m] = BCell->CelCount;
    Tab.data[m] = (double) BCell->value;
    switch(BCell->status)
    {
      case 'x': Tab.status[m] = 'u';  // This way it will continue. Should produce ERROR-message????
                Tab.NumberOfUnsafeCells++; // Count unsafe cells
                Tab.lpl[m] = fabs(ceil(LPRO*Tab.data[m])); 
		Tab.upl[m] = fabs(ceil(UPRO*Tab.data[m])); 
                break;
      case 'b':  // marginal suppressed as secondary, treat like primary but save info that it is a backtrack-cell
		Tab.status[m] = 'b';
		Tab.lpl[m] = fabs(BCell->Pbounds[0]); // Protection levels as taken from BTinfo file
		Tab.upl[m] = fabs(BCell->Pbounds[1]); 
		Tab.NumberOfUnsafeCells++; // Count unsafe cells
                break;
      case 'm':  // marginal suppressed as secondary, treat like primary but save info that it is a secondary
		Tab.status[m] = 'm';
		Tab.lpl[m] = fabs(BCell->Pbounds[0]);
		Tab.upl[m] = fabs(BCell->Pbounds[1]); 
		Tab.NumberOfUnsafeCells++; // Count unsafe cells
                break;
      case 'u': Tab.status[m] = 'u';  // marginal suppressed as primary
                Tab.lpl[m] = fabs(BCell->Pbounds[0]);
		Tab.upl[m] = fabs(BCell->Pbounds[1]);
		Tab.NumberOfUnsafeCells++; // Count unsafe cells
                break;
      case 'z': // Empty or protected cell, weight may be used to calculate scaling factor, so cannot be 0
                Tab.status[m] = 'z';   // Empty cells remain empty cells
		Tab.costs[m] = MargWeight; 
                Tab.NumberOfSafeCells++; // Count safe cells
                break;
      case 's': Tab.status[m] = 's';  // Safe cell, but use large weight (is marginal cell)
		Tab.costs[m] = MargWeight;
		Tab.NumberOfSafeCells++; // Count safe cells;
		break;
    }
    if (DISTANCE==0)
	  Tab.costs[m] = BCell->CelCost;
  }
  else Tab.NumberOfSafeCells++;  // If BCell is NULL then status 'z', i.e. safe
  

  if (APRIORI==0) // then [MINTABVAL,MAXTABVAL] is written in ParameterFile
  {
    Tab.lb[m]  = MINTABVAL;
    Tab.ub[m]  = MAXTABVAL;
  }
  else // lb and ub are calculated using q from p-q-rule
  {
    Tab.lb[m]  = floor(Tab.data[m] - APRIORILB*fabs(Tab.data[m]));
    Tab.ub[m]  =  ceil(Tab.data[m] + APRIORIUB*fabs(Tab.data[m]));
  }

  if ( (Tab.data[m] - Tab.lpl[m]) < Tab.MinMargVal) Tab.MinMargVal = Tab.data[m] - Tab.lpl[m];
}

// Treats a single cell in interior
static void SetCell(JJTable& Tab, int m, cell* BCell)
{
  std::string buffer;

  // Initialize BCell as empty
  Tab.data[m]   = 0;
  Tab.status[m] = 'z';
  Tab.lpl[m]    = 0;
  Tab.upl[m]    = 0;
  Tab.count[m]  = 0;
  if (DISTANCE == 0)
	Tab.costs[m] = 0;

  // If BCell is not empty, adjust
  if (BCell != NULL)
  {
    Tab.count[m] = BCell->CelCount;
    Tab.data[m] = (double) BCell->value;

    switch (BCell->status)
	{
	  case 'n':     Tab.status[m] = 'n'; // Cell belongs to table that will be skipped
			break;
	  case 'b': // BackTrack cell, treat like primary but save info that it is a backtrack-cell
			Tab.status[m] = 'b';
		        Tab.costs[m] = 0;
		        Tab.lpl[m] = fabs(BCell->Pbounds[0]);
			Tab.upl[m] = fabs(BCell->Pbounds[1]);
			Tab.NumberOfUnsafeCells++; // Count unsafe cells
			break;
	  case 'u':     Tab.status[m] = 'u';
		        Tab.costs[m] = 0;
		        Tab.lpl[m] = fabs(BCell->Pbounds[0]);
			Tab.upl[m] = fabs(BCell->Pbounds[1]);
			Tab.NumberOfUnsafeCells++; // Count unsafe cells
			break;
	  case 's':     Tab.status[m] = 's';
		        Tab.NumberOfSafeCells++; // Count safe cells
		        break;
	  case 'z':     Tab.status[m] = 'z';
		        //Tab.costs[m] = 0;
			Tab.NumberOfSafeCells++; // Count safe cells
		        break;
	  case 'm': 
	  case 'x':     buffer.assign("State ");
                        buffer.append(&BCell->status);
                        buffer.append(" should not be possible in SetCel\n");
			LogPrintf(LogName,buffer);
		        break;
	}
    if (DISTANCE==0)
	  Tab.costs[m] = BCell->CelCost;
  }
  else
  {
	  Tab.NumberOfSafeCells++;  // If BCell is NULL then status 'z', i.e. safe
	  Tab.costs[m] = 0;
  }


  if (APRIORI==0) // then [MINTABVAL,MAXTABVAL] is written in ParameterFile
  {           
    Tab.lb[m]  = MINTABVAL;
    Tab.ub[m]  = MAXTABVAL;
  }
  else	// lb and ub are calculated using q from p-q-rule
  {
    Tab.lb[m]  = floor(Tab.data[m] - APRIORILB*fabs(Tab.data[m]));
    Tab.ub[m]  =  ceil(Tab.data[m] + APRIORIUB*fabs(Tab.data[m]));
  }

  if ( (Tab.data[m] - Tab.lpl[m]) < Tab.MinInteriorVal) Tab.MinInteriorVal = Tab.data[m] - Tab.lpl[m];

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Adjust Marginal weights to have the weights larger than interior weights
// If necessary, scale all weight, such that largest weight equals MAXWEIGHT
static double AdjustMarginalWeights(JJTable& Tab, double MaxCost)  // MaxCost is largest weight from interior
{
  int i, j, m, marg;
  double MinMargWeight,AddWeight,fac;
  int dims = Tab.Dim();
  
  // Total General
  AddWeight=MaxCost;
  for (i=1;i<=dims;i++)
	  //AddWeight = (Tab.N[i]-1)*AddWeight;
      AddWeight = (Tab.N[i-1]-1)*AddWeight;
  Tab.costs[0] += AddWeight;
   
  // When weight becomes too large: scale all weights
  fac = MAXWEIGHT/Tab.costs[0];
  
  // PWOF 20170120: Allways scale in compliance with Optimal
  for (m=0;m<Tab.Size();m++) Tab.costs[m] = fac*Tab.costs[m];
  MaxCost = fac*MaxCost;

  MinMargWeight = Tab.costs[0];
  Tab.weight[0] = (int) ceil(Tab.costs[0]);

  for (m=1;m<Tab.Size();m++)
  {
	  marg = 1;
	  for (j=1;j<=dims;j++)
            //marg *= Tab.ijk[j][m];
            marg *= Tab.ijk[j-1][m];

	  if (marg == 0) // marginal cell
	  {
		AddWeight = MaxCost;
		for (i=1;i<=dims;i++)
			//if (Tab.ijk[i][m] == 0) AddWeight = (Tab.N[i]-1)*AddWeight; // all marginals
                    if (Tab.ijk[i-1][m] == 0) AddWeight = (Tab.N[i-1]-1)*AddWeight; // all marginals
		Tab.costs[m] += AddWeight;
		MinMargWeight = __min(Tab.costs[m],MinMargWeight);
	  }
	  Tab.weight[m] = (int) ceil(Tab.costs[m]);
  }
  return MinMargWeight;
}

// Scale all weight, such that largest weight equals MAXWEIGHT
static void ScaleMarginalWeights(JJTable& Tab)
{
        int m;
        double fac;

        // When weight of Total General is too large: scale all weights
        // PWOF 20170120: Always scale; in compliance with Optimal
        // PWOF 20171128: Need Tab.costs[0] != 0
        fac = MAXWEIGHT/Tab.costs[0];

        for (m=0;m<Tab.Size();m++)
        {
                Tab.costs[m] = fac*Tab.costs[m];
                Tab.weight[m] = (int) ceil(Tab.costs[m]);
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple distance function
//static void SetDistanceCostsPerDim(JJTable& Tab, int m, int DimIndex, int DimWeights[5])
static void SetDistanceCostsPerDim(JJTable& Tab, int m, int DimIndex, std::vector<int> DimWeights)
{
	int i, j, dist, pos;
	double weight = Tab.costs[m]; // Initializing to original cost, in case it will not change
	//Vector<int> Gijk;
        std::vector<int> Gijk;
	//Gijk.Make(Tab.Dim());
        Gijk.resize(Tab.Dim());
	//Vector<int> TabDims;
        std::vector<int> TabDims;
	//TabDims.Make(Tab.Dim());
        TabDims.resize(Tab.Dim());

	for (i=1;i<Tab.Dim();i++)
		//TabDims[i] = Tab.N[Tab.Dim()-i+1];
                TabDims[i-1] = Tab.N[Tab.Dim()-i];
	//TabDims[Tab.Dim()] = 1;
        TabDims[Tab.Dim()-1] = 1;

        for (j=1;j<=Tab.Dim();j++)  // Needs to be done once for all cells in this row
            //Gijk[j] = Tab.ijk[j][m];
            Gijk[j-1] = Tab.ijk[j-1][m];
        
	//for (i=1;i<Tab.N[DimIndex];i++)     // single row, excluding marginal
        for (i=1;i<Tab.N[DimIndex-1];i++)     // single row, excluding marginal
        {
          //dist = Tab.ijk[DimIndex][m] - i;  // distance to primary within row
            dist = Tab.ijk[DimIndex-1][m] - i;  // distance to primary within row
					// negative when cell "precedes" the primary, 
					// positive when cell "follows" the primary

          if (abs(dist)>=5)		// maximum distance, i.e., use DimWeights[4]
		  weight = DimWeights[4];
	  else 
	  {
            if (dist)		// if dist==0 then nothing to do and index would go wrong
		weight = DimWeights[abs(dist)-1];
	  }
	  
	  //Gijk[DimIndex] = i; // set final index of position for cell i in this row
          Gijk[DimIndex-1] = i; // set final index of position for cell i in this row
	  Calc_m(Gijk,TabDims,pos); 

          if (((Tab.status[pos]=='s') || (Tab.status[pos]=='z')) && (weight<Tab.costs[pos])) Tab.costs[pos] = weight;
        }
        
        //Not needed for std::vector. Is restricted to this scope.
	//Gijk.Free();
	//TabDims.Free();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Adjust safety ranges:
// min((1-LOWERMARG)*value, maxLowerBound) and
// min((UPPERMARG-1)*value, maxUpperBound)
// NB: BCell->value may be negative! 
// NB: BCell->value may be zero, but lpl and upl should be > 0 IS NOT FEASIBLE: cell-value>=0, cell-value minus lpl < 0!
//static void CalcPbounds(cell* BCell, const CArray<double, double>& bounds)
static void CalcPbounds(cell* BCell, const std::vector<double>& bounds)
{
#ifdef SECBOUNDS
  if (BCell->Pbounds.size() == 0) BCell->Pbounds.resize(2);
  BCell->Pbounds[0] = __min((1.0-LOWERMARG)*fabs(BCell->value),bounds[0]);
  if (BCell->Pbounds[0] < 0.0001) BCell->Pbounds[0] = 0.0001;
  BCell->Pbounds[1] = __min((UPPERMARG-1.0)*fabs(BCell->value),bounds[1]);
  if (BCell->Pbounds[1] < 0.0001) BCell->Pbounds[1] = 0.0001;
#else
  if (BCell->Pbounds.size() == 0) BCell->Pbounds.resize(2);
  BCell->Pbounds[0] = (1.0-LOWERMARG)*fabs(BCell->value);
  if (BCell->Pbounds[0] < 0.0001) BCell->Pbounds[0] = 0.0001;
  BCell->Pbounds[1] = (UPPERMARG-1.0)*fabs(BCell->value);
  if (BCell->Pbounds[1] < 0.0001) BCell->Pbounds[1] = 0.0001;
#endif

  if (APRIORI==1) // Take care of possible inconsistencies between APRIORI en protection levels
  {
	BCell->Pbounds[0] = __min(APRIORILB*fabs(BCell->value), BCell->Pbounds[0]);
	BCell->Pbounds[1] = __min(APRIORIUB*fabs(BCell->value), BCell->Pbounds[1]);
  }
  else // PWOF 2 juni 2010: BCell->Ebounds should contain apriori bounds, but is not available yet
  {
	BCell->Pbounds[0] = __min(BCell->value - MINTABVAL, BCell->Pbounds[0]);
	BCell->Pbounds[1] = __min(MAXTABVAL - BCell->value, BCell->Pbounds[1]);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Will be applied after table is lifted in case of negative interior cells
// Adds constraint on frequency-unsafe cells that together they should have at least MINCOUNT contributors
// Y has arrays of frequency-unsafe cells for each row
// Yc has total number of contributors of all unsafe cells for each row map<key=std::string,value=int>
static void SetCountBounds(JJTable& Tab, CountInfo& Y, TotCountInfo &Yc)
{
    unsigned int i;
    int rowcount, cellposition;
    double value;
    bool DoIt;
    //std::vector<int> Cellpositions; ??Not used
    addcellinfo CellInfo;
    addsuminfo Constraint;
    std::string row;
    TotCountInfo::iterator pos;
    std::vector<celinfo>* Cellen;
    celinfo Cel;
//        celinfo Cel1, Cel2;

    cellposition = Tab.Size() - 1 + Tab.AdditionalCells.size();

    for (pos=Yc.begin(); pos != Yc.end(); ++pos)
    {
	DoIt = false;
        row = pos->first;
        rowcount = pos->second;
        if (Y.find(row)!=Y.end())
	{
            Cellen = Y.find(row)->second;
            if ( (rowcount <= MINCOUNT) && (Cellen->size() > 1) )	// Not enough contributors. If only a single cell then no additional restriction needed
            {
                if (Cellen->size() > 2) DoIt = true;		// More than 2 unsafe cells => should apply the trick
		else		// Exactly 2 unsafe cells => should apply the trick, but take DOSINGLETONS into account
                {
                    if ((Cellen->at(0).count == 1) || (Cellen->at(1).count == 1))	// At least one singleton
                    {
			if ((Cellen->at(0).count == 1) && (Cellen->at(1).count == 1)) // Two singletons
                            DoIt = !DOSINGLEWITHSINGLE;	// If DOSINGLEWITHSINGLE = true then already dealt with in DoSingletons
			else					// One singleton and one dominance unsafe cell
                            DoIt = !DOSINGLEWITHMORE;	// If DOSINGLEWITHMORE = true then already dealt with in DoSingletons
                        }
                }
		//} // Only DoIt possible if (rowcount <= MINCOUNT) && (Cellen->size() > 1)
					
                if (DoIt)	// Construct additional "pseudo-cell" and corresponding constraint
                {
                    cellposition++;
			
                    Constraint.rhs = 0.0;
                    Constraint.ncard = Cellen->size() + 1;
                    Constraint.Cells = (int*) malloc(Constraint.ncard*sizeof(int));
                    Constraint.Cells[0] = cellposition;
                    value = 0;
                    for (i=0;i<Cellen->size();i++)
                    {
                        Cel = Cellen->at(i);
                        Constraint.Cells[i+1] = Cel.position;
                        value += Cel.value;		// Total value of "problem"-cells
                    }
				
                    CellInfo.position = cellposition;
                    CellInfo.value = value;
                    CellInfo.weight = 1;
                    CellInfo.status = 'u';
                    CellInfo.lb = (Constraint.ncard - 1)*MINTABVAL;
                    CellInfo.ub = (Constraint.ncard - 1)*MAXTABVAL;
                    CellInfo.lpl = TRICK_LPL;
                    CellInfo.upl = TRICK_UPL;
                    CellInfo.spl = TRICK_SPL;
			
                    Tab.AdditionalCells.push_back(CellInfo);
                    Tab.AdditionalConstraints.push_back(Constraint);
                }
            } // ?? Should be here instead on 31 lines earlier ??
        }
    }
}

//////////////////////////////////////////////////////////////////////
// Spos is position of singleton
// row has fixed index (indices)
// combining row with Spos determines whish index is running
static void GetCel2(JJTable &Tab, std::string row, int Spos, celinfo& Cel2, int NonFixedIndex)
{
    int i,k,Npos;
    std::string::size_type pt;

    //Vector<int> Gijk;
    std::vector<int> Gijk;
    //Vector<int> TabDims;
    std::vector<int> TabDims;
    //Gijk.Make(Tab.Dim());
    Gijk.resize(Tab.Dim());
    //TabDims.Make(Tab.Dim());
    TabDims.resize(Tab.Dim());
        
    // TabDims[1] = Tab.N[Dim], TabDims[2] = Tab.N[Dim-1] ... TabDims[Dim] = 1
    // Needed for Calc_m
    for (i=1;i<Tab.Dim();i++)
	//TabDims[i] = Tab.N[Tab.Dim()-i+1];
        TabDims[i-1] = Tab.N[Tab.Dim()-i];
    //TabDims[Tab.Dim()] = 1;
    TabDims[Tab.Dim()-1] = 1;

    pt = row.find('.');
    if (pt == std::string::npos){		// No . found, so Tab.Dim == 1
	//for (k=1;k<Tab.N[1];k++)
        for (k=1;k<Tab.N[0];k++){
            if ((Tab.status[k]=='u') && (k!=Spos)){
                // Looking for dominance primary
		Cel2.count = Tab.count[k];
		Cel2.position = k;
		Cel2.value = Tab.data[k];
		break; // Found
            }//
	}
    }
    else{	// Read "fixed indices"
	for (i=1;i<=Tab.Dim();i++){
            if (i != NonFixedIndex){
                //Gijk[i] = atoi(row.substr(0,pt).c_str());
                Gijk[i-1] = atoi(row.substr(0,pt).c_str());
                row = row.substr(pt+1);
                pt = row.find('.');
            }
	}
    }

    //for (k=1;k<Tab.N[NonFixedIndex];k++)
    for (k=1;k<Tab.N[NonFixedIndex-1];k++){
	//Gijk[NonFixedIndex] = k;
        Gijk[NonFixedIndex-1] = k;
	Calc_m(Gijk,TabDims,Npos);
	if ((Tab.status[Npos]=='u') && (Npos != Spos)){ // Looking for dominance primary
            Cel2.count = Tab.count[Npos];
            Cel2.position = Npos;
            Cel2.value = Tab.data[Npos];
            break; // Found
	}
    }

    //Not needed for std::vector. Is restricted to this scope.
    //Gijk.Free();
    //TabDims.Free();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Will be applied after table is lifted in case of negative interior cells
// Adds constraint on singleton cells that they should not be able to protect each other
// Only applicable in case of exactly two unsafe cells in a row with at least one singleton
// Y has arrays of frequency-unsafe cells for each row
// Yu has total number of unsafe cells for each row
static void DoSingletons(JJTable& Tab, CountInfo& Y, TotCountInfo& Yu, int NonFixedIndex)
{
    std::string row;
    TotCountInfo::iterator pos;
    addcellinfo CellInfo = addcellinfo();
    addsuminfo Constraint = addsuminfo();
    bool DoIt;
    std::vector<celinfo>* Cellen;
    celinfo Cel1 = celinfo(), Cel2 = celinfo();
    int cellposition, rowcount;

    cellposition = Tab.Size() - 1 + Tab.AdditionalCells.size();

    for(pos = Yu.begin(); pos != Yu.end(); ++pos){
	DoIt = false;
        row = pos->first;
        rowcount = pos->second;
		
        if (Y.find(row)!= Y.end()){
            if (rowcount == 2){					// Exactly two primary cells with at least one frequency-unsafe cell
                Cellen = Y.find(row)->second;
                if (Cellen->size() == 2){			// Two frequency-unsafe cells
                    Cel1 = Cellen->at(0);
                    Cel2 = Cellen->at(1);
                    if (Cel1.count == 1 || Cel2.count == 1){ // At least one singleton
			if ( (Cel1.count == 1) && (Cel2.count == 1) ) // We have two singletons
                            DoIt = DOSINGLEWITHSINGLE;	      // and we might want to do the trick
			else					      // We have one singleton and one frequency-unsafe cell 
                            DoIt = DOSINGLEWITHMORE;	      // and we might want to do the trick
                    }
		}
		else{	// One frequency-unsafe cell and one dominance unsafe cell
                    Cel1 = Cellen->at(0);
                    if ( Cel1.count == 1 ){ // Cel1 is singleton 
			GetCel2(Tab,row,Cel1.position,Cel2,NonFixedIndex);	// find dominance unsafe cell
			DoIt = DOSINGLEWITHMORE;				// and we might want to do the trick
                    }
                    else DoIt = false;
		}
		//} // Only DoIt possible if exactly 2 primaries

                if (DoIt){			// Construct additional "pseudo-cell" and corresponding constraint
                    cellposition++;
                    
                    Constraint.rhs = 0.0;
                    Constraint.ncard = 3;
                    Constraint.Cells = (int*) malloc(3*sizeof(int));
                    Constraint.Cells[0] = cellposition;
                    Constraint.Cells[1] = Cel1.position;
                    Constraint.Cells[2] = Cel2.position;
                                
                    CellInfo.position = cellposition;
                    CellInfo.value = Cel1.value + Cel2.value;
                    CellInfo.weight = 1;
                    CellInfo.status = 'u';
                    CellInfo.lb = (Constraint.ncard - 1)*MINTABVAL;
                    CellInfo.ub = (Constraint.ncard - 1)*MAXTABVAL;
                    CellInfo.lpl = TRICK_LPL;
                    CellInfo.upl = TRICK_UPL;
                    CellInfo.spl = TRICK_SPL;
			
                    Tab.AdditionalConstraints.push_back(Constraint);
                    Tab.AdditionalCells.push_back(CellInfo);
                }
            } // ?? Should be here instead on 26 lines earlier ??
	}
    }
}

static void SumPrimaryCells(TotCountInfo& Cc, std::string row)
{
        (Cc.find(row) != Cc.end()) ? Cc[row]++ : Cc[row] = 1;
}

static void SumCountRuleCells(TotCountInfo& Cc, std::string row, int aantal)
{
        (Cc.find(row) != Cc.end()) ? Cc[row] += aantal : Cc[row] = aantal;
}

static void AddCountCell(CountInfo& C, std::string row, int pos, double value, int aantal)
{
	celinfo Cel;
	std::vector<celinfo>* Cellen;
        
        Cel.position = pos;
        Cel.count    = aantal;
	Cel.value    = value;

        if (C.find(row) != C.end()){
                C[row]->push_back(Cel);
        }
        else
	{
          Cellen = new std::vector<celinfo>;
          Cellen->push_back(Cel);
          C[row] = Cellen;
	}
}

static void FreeCountCells(CountInfo& C, TotCountInfo& Cc)
{
  if (C.size() != 0)
  {
    for (CountInfo::iterator it=C.begin(); it != C.end(); ++it)
    {
        it->second->erase(it->second->begin(),it->second->end());
        delete it->second;
    }
    C.erase(C.begin(),C.end());
    Cc.erase(Cc.begin(),Cc.end());
  }
}

// If DoCosts = true, maximum cost is calculated
// If DoCosts = false, DISTANCE can be used

//static void FillJJ(JJTable& Tab, Vector< Vector<int> > SGTab, Table& BTab, std::vector<double>& bounds, bool DoCosts, double& MaxCost)
static void FillJJ(JJTable& Tab, std::vector< std::vector<int> > SGTab, Table& BTab, std::vector<double>& bounds, bool DoCosts, double& MaxCost)
{
    int Weight = INFWEIGHT;            
    int dims = Tab.Dim();
    int zerodims = Tab.BDim() - Tab.Dim(); // number of "dummy"-dimensions
    int i,j,k,m,tel;
    int UpperM = Tab.Size(); // number of cells in table Tab
    bool DoNoTricks=true;

    CountInfo DummyCountInfo;
    TotCountInfo DummyTotCountInfo;
    std::string DummyString;
  
    std::vector<CountInfo> CI;	// Array with cellinfo of frequency/count-unsafe cells per Row
    CI.assign(dims, DummyCountInfo);
    std::vector<TotCountInfo> TCIc; // Number of frequency/count-unsafe cells per Row
    TCIc.assign(dims, DummyTotCountInfo);
    std::vector<TotCountInfo> TCIu; // Number of primary unsafe cells per Row
    TCIu.assign(dims, DummyTotCountInfo);
    std::vector<std::string> Rows;
    Rows.assign(dims, DummyString);
  
    MaxCost=0.0; // Will change when DoCosts=true, if DoCosts=false, it will stay at 0.0
  
    //Vector<int> Gijk;
    //Gijk.Make(dims);
    std::vector<int> Gijk;
    Gijk.resize(dims);
    
    //Vector<int> Vol;                    // Non-empty variables in BaseTable
    //Vol.Make(dims);
    std::vector<int> Vol;                    // Non-empty variables in BaseTable
    Vol.resize(dims);
  
    //Vector<int> Leeg;
    //Leeg.Make(Tab.BDim()-Tab.Dim()); 
    std::vector<int> Leeg;
    Leeg.resize(Tab.BDim()-Tab.Dim()); 
  
    //Vector<int> Divs;		      // Need Divs[1] = Tab.N[Dim], Divs[2] = Tab.N[Dim-1], Divs[Dim] = 1;
    //Divs.Make(dims);
    std::vector<int> Divs;		      // Need Divs[0] = Tab.N[Dim], Divs[1] = Tab.N[Dim-1], ..., Divs[Dim-1] = 1;
    Divs.resize(dims);
    for (tel=1;tel<dims;tel++)
        //Divs[tel] = Tab.N[dims-tel+1];
        Divs[tel-1] = Tab.N[dims-tel];
    //Divs[dims]=1;
    Divs[dims-1]=1;

    Tab.Skip = false;		      // Default: do not skip
  
    j=1;k=1;

    for (i=1;i<=Tab.BDim();i++)	      // BDim can be larger than Dim => search for "empty" variables
    {
        //if (SGTab[i][1] == 0) Leeg[j++] = i;  // "empty" variable found
        if (SGTab[i-1][0] == 0) {Leeg[j-1] = i; j++;}  // "empty" variable found
        //else Vol[k++] = i;                    // Vol[] has ID's of "non-empty" variables
        else {Vol[k-1] = i; k++;}                    // Vol[] has ID's of "non-empty" variables
    }

    for (m=0;m<UpperM;m++)           // Loop over all cells of JJTable to be constructed
    {
        if (Getijk(m,Gijk,Divs))       // Marginal cel if at least one Gijk[i] == 0, in which case Getijk returns true
	{
            //for (tel=1;tel<=zerodims;tel++) Tab.baseijk[m][Leeg[tel]] = 0;
            for (tel=1;tel<=zerodims;tel++) Tab.baseijk[m][Leeg[tel-1]-1] = 0;

            for (tel=1;tel<=dims;tel++) 
            {
		//Tab.ijk[tel][m] = Gijk[tel];
                Tab.ijk[tel-1][m] = Gijk[tel-1];
		//Tab.baseijk[m][Vol[tel]] = (Gijk[tel]==0) ? SGTab[Vol[tel]][1] - 1 : SGTab[Vol[tel]][Gijk[tel]];
                Tab.baseijk[m][Vol[tel-1]-1] = (Gijk[tel-1]==0) ? SGTab[Vol[tel-1]-1][0] - 1 : SGTab[Vol[tel-1]-1][Gijk[tel-1]-1];
            }

            MarginalCell(Tab,m,BTab[Tab.baseijk[m]],Weight);

            if ((BTab[Tab.baseijk[m]]!=NULL)&&((Tab.status[m]=='u')||(Tab.status[m]=='b')||(Tab.status[m]=='m')))  // Primaries (real and temporarily)
            {
                if(bounds[0]<BTab[Tab.baseijk[m]]->Pbounds[0]) bounds[0] = BTab[Tab.baseijk[m]]->Pbounds[0];
                if(bounds[1]<BTab[Tab.baseijk[m]]->Pbounds[1]) bounds[1] = BTab[Tab.baseijk[m]]->Pbounds[1];
            }

            if (Tab.status[m]=='u') // Only real primaries are candidates for "tricks"
            {
                if (dims == 1) Rows[0] = '0';   // Only a single row
		else
		{
                    for (i=0;i<dims;i++)
                    {
                        Rows[i].clear();
                                        
			for (j=1;j<=dims;j++)
			{
                            if ((j-1) != i) 
                            {
                                //Rows[i].append(to_string(Tab.ijk[j][m]));	// Rows[i] becomes j.k. with j and k "non-running indices"
                                Rows[i].append(to_string(Tab.ijk[j-1][m]));	// Rows[i] becomes j.k. with j and k "non-running indices"
                                Rows[i].append(".");
                            }
			}
			//if (Tab.ijk[i+1][m] != 0)   // Do not include marginal of "marginal of running index"
                        if (Tab.ijk[i][m] != 0)   // Do not include marginal of "marginal of running index"
			{
                            SumPrimaryCells(TCIu[i],Rows[i]);
			
                            if ((Tab.count[m] == 1) || ((Tab.count[m] < MINCOUNT) && (Tab.count[m]>0)))
				AddCountCell(CI[i],Rows[i],m,Tab.data[m],Tab.count[m]);
                            if (MINCOUNT > 0)
                                SumCountRuleCells(TCIc[i],Rows[i],Tab.count[m]);
                            }
                        }
                }
            }		
	}
	else  // Internal cell
	{
            //for (tel=1;tel<=zerodims;tel++) Tab.baseijk[m][Leeg[tel]] = 0;
            for (tel=1;tel<=zerodims;tel++) Tab.baseijk[m][Leeg[tel-1]-1] = 0;

            for (tel=1;tel<=dims;tel++)
            {
		//Tab.ijk[tel][m]=Gijk[tel];
                Tab.ijk[tel-1][m]=Gijk[tel-1];
		//Tab.baseijk[m][Vol[tel]] = SGTab[Vol[tel]][Gijk[tel]];
                Tab.baseijk[m][Vol[tel-1]-1] = SGTab[Vol[tel-1]-1][Gijk[tel-1]-1];
            }

            SetCell(Tab,m,BTab[Tab.baseijk[m]]);

            if (Tab.status[m]=='n')
            {
		Tab.Skip = true;  // Table is to be skipped (linked tables method)
		goto StopFilling; // Jump out of for-loop
            }

            if ((BTab[Tab.baseijk[m]]!=NULL)&&((Tab.status[m]=='u')||(Tab.status[m]=='b')))  // Primaries (real and temporarily, status[m]=='m' not possible)
            {
                if(bounds[0]<BTab[Tab.baseijk[m]]->Pbounds[0]) bounds[0] = BTab[Tab.baseijk[m]]->Pbounds[0];
                if(bounds[1]<BTab[Tab.baseijk[m]]->Pbounds[1]) bounds[1] = BTab[Tab.baseijk[m]]->Pbounds[1];
            }

            if (DoCosts) MaxCost = __max(MaxCost, Tab.costs[m]);  

            if (Tab.status[m]=='u') 
            {
		if (dims == 1) Rows[0] = '0';   // Only a single row
		else
		{
                    for (i=0;i<dims;i++)
                    {
			Rows[i].clear();
			for (j=1;j<=dims;j++)
			{
                            if ((j-1) != i) 
                            {
                                //Rows[i].append(to_string(Tab.ijk[j][m]));	// Rows[i] becomes j.k. with j and k "non-running indices"
                                Rows[i].append(to_string(Tab.ijk[j-1][m]));	// Rows[i] becomes j.k. with j and k "non-running indices"
                                Rows[i].append(".");
                            }
			}
                    }
		}

		for (i=0;i<dims;i++) SumPrimaryCells(TCIu[i],Rows[i]);   // Count number of primary unsafe cells in Row[i]

                if (!DoCosts)
                {
                    if (DISTANCE != 0)
                    {
                        for (tel=1;tel<=dims;tel++) 
                        {                                    
                            //SetDistanceCostsPerDim(Tab,m,tel,D[Vol[tel]]);
                            SetDistanceCostsPerDim(Tab,m,tel,D[Vol[tel-1]-1]);
                        }
                    }
                }

		if ((Tab.count[m]==1) || ((Tab.count[m]<MINCOUNT) && (Tab.count[m]>0)))
                    for (i=0;i<dims;i++)
                        AddCountCell(CI[i],Rows[i],m,Tab.data[m],Tab.count[m]);  // Cellinfo of frequency-unsafe cells in Row[i]
                                            
		if (MINCOUNT > 0)
                    for (i=0;i<dims;i++) SumCountRuleCells(TCIc[i],Rows[i],Tab.count[m]);	// Count number of contributors in all unsafe cells in Row[i]
            }
	}
    }

    for (i=0;i<dims;i++)
        DoNoTricks = (DoNoTricks && CI[i].empty()); // If all CI[i] empty, no Tricks possible

    if (!DoNoTricks) // Tricks are possible
    {
	if ((MINCOUNT > 0) && (DOCOUNTBOUNDS))
            for (i=0;i<dims;i++) SetCountBounds(Tab,CI[i],TCIc[i]);

	if (DOSINGLETONS)
            for (i=0;i<dims;i++) DoSingletons(Tab,CI[i],TCIu[i],i+1);
    }

    StopFilling: ;

    for (i=0;i<dims;i++)  // Free memory
    {
	FreeCountCells(CI[i],TCIc[i]);
        Rows.erase(Rows.begin(),Rows.end());
        TCIu[i].erase(TCIu[i].begin(),TCIu[i].end());
    }

    //Not needed for std::vector. Is limited to this scope.
    //Gijk.Free();
    //Vol.Free();
    //Leeg.Free();
    //Divs.Free();
}

// Filling a table in JJ-format, determining maximum cost of interior cells if DoCosts=true
//int FillTable(JJTable& Tab, Vector< Vector<int> > SGTab, Table& BTab, std::vector<double>& bounds, bool DoCosts, double& MaxCost)
int FillTable(JJTable& Tab, std::vector< std::vector<int> > SGTab, Table& BTab, std::vector<double>& bounds, bool DoCosts, double& MaxCost)
{
  //Vector<int> HVec;
  std::vector<int> HVec;

  //HVec.Make(SGTab.size());
  HVec.resize(SGTab.size());
  for (size_t i=1;i<=SGTab.size();i++)    // number of categories per variable
  {
      //if (SGTab[i][1] == 0) HVec[i] = 0;  // is only marginal ie., no "real" dimension
      if (SGTab[i-1][0] == 0) HVec[i-1] = 0;  // is only marginal ie., no "real" dimension
      //else HVec[i] = SGTab[i].size();
      else HVec[i-1] = SGTab[i-1].size();
  }
  Tab.Init(HVec);
  //HVec.Free(); Not needed for std::vector

  // Only up to 4 dimensional tables (cpu-time restrictions)
  // Tab.Dim() gives "real" dimension = # (HVec[i] <> 0)
  
  if (Tab.Dim() > 4) 
  {
      LogPrintf(LogName,"WARNING: Dimension ");
      LogPrintf(LogName,to_string(Tab.Dim()));
      LogPrintf(LogName," not implemented (yet)!\n");
      return(-1);
  }

  Tab.MinInteriorVal = 0;
  Tab.MinMargVal = 0;
  Tab.NumberOfSafeCells = 0;
  Tab.NumberOfUnsafeCells = 0;
  Tab.Skip = false;
  
  FillJJ(Tab,SGTab,BTab,bounds,DoCosts,MaxCost);

  if (Tab.Skip)		// Table to be skipped 
	  return -11;
  else
	  return Tab.Dim(); // No error
}

// Apply JJ routines to find suppress pattern
int Suppress(const char* Solver, JJTable& Tab, int Rdim, bool DoCosts, double& MaxCost, int Hierarch,  int &ObjVal)  
{
  clock_t start;
  FILE* FUit;
  int lcost, ucost;
  int i, marg, j;
  int MinMargCost;
  int ReturnCodeE=0; 
  int Load;

  JJTime=0;

  if (Tab.NumberOfSafeCells==Tab.Size()) return (1); // Only safe cells: nothing to do

  if (Tab.NumberOfUnsafeCells==Tab.Size()) return (2); // Only unsafe cells: nothing to do

  
  // If DISTANCE !=0 (DoCosts==false) we get MaxCost=0: no adjustment, only scaling up to MAXWEIGHT
  if (DoCosts)
      MinMargCost = (int) AdjustMarginalWeights(Tab,MaxCost);
  else
  {
      MinMargCost = 0.0;
      ScaleMarginalWeights(Tab);
  }
  
  if (PPDEBUG)
  {
    FUit = OpenFile(PrepFile("TestTabOut.dat").c_str(),"a");
        for (i=0;i<Tab.Size();i++)
    {
        fprintf(FUit,"%3d %15.5lf %15.5lf %5d\n",i,Tab.data[i],Tab.costs[i],Tab.weight[i]);
    }
    fclose(FUit);
  }
  
  if (PPDEBUG)
  {
      FUit = OpenFile(LogName.c_str(),"a");
      fprintf(FUit,"\nMinimal Marginal Cost: %d",MinMargCost);
      fprintf(FUit,"\nMaxCost: %lf\n",MaxCost);
      fclose(FUit);
  }

  start = clock();
  
  Load = LoadTableIntoPCSP(Solver,Tab);

  if (Load != 0) 
  {
	  LogPrintf(LogName,"Not able to load problem\n");
	  WriteErrorToLog(LogName,HITAS_JJNOTLOADED);
	  throw(HITAS_JJNOTLOADED);
  }

//  CSPoptimize(Solver,NULL);
  if (CSPoptimize(Solver,NULL)){
        ucost = CSPGetDoubleConstant(Solver,JJINF);
        lcost = ucost;
  }
  else{
        CSPsolution(Solver,&lcost, &ucost, Tab.status);
  }
  ObjVal = (int) ucost;
  CSPfreeprob(Solver);

  JJTime = (double) (clock()-start)/CLOCKS_PER_SEC;

  // Check for feasibility: infeasible if lcost = ucost = INF
  if ((lcost >= CSPGetDoubleConstant(Solver,JJINF) - 0.1) && (ucost >= CSPGetDoubleConstant(Solver,JJINF) - 0.1))
  {
        FUit = OpenFile(PrepFile("InFeas.dat").c_str(),"w");
        
	Tab.PrintData(*FUit);
	fclose(FUit);
    return (9); // Go back to main program
  }

  // Check for "no solution" or BackTracking
  for (i=0;i<Tab.Size();i++)
  { // Marginal additionally suppressed
	if (Hierarch != 1) // i.e. hierarchical
        {
                marg = 1;
                for (j=1;(j<=Tab.Dim()) && (marg!=0);j++)
                //marg *= Tab.ijk[j][i];      // if marg==0 then marginal cell
                marg *= Tab.ijk[j-1][i];      // if marg==0 then marginal cell

                if ((Tab.status[i]=='m') && (marg==0)) 
                {
                        ReturnCodeE = 2; break; // Backtracken (at least one cell)
                }
                if (Tab.status[i] == 'x')
                {
                        ReturnCodeE = 5; break; // NoSolution
                }
        }
  }

  switch (ReturnCodeE)
  {
     case  0: {return(0);}
     case  2: {return(100);}   // Marginal suppressed: backtracking

     case  5: {return(5);}     // Error: x-status
     default: {return(-10);}   // Unknown error
  }
}

// Adjust statusses in BaseTable BTab
int Update(Table& BTab, JJTable& Tab, int RetCode, std::vector<double>& bounds)
{
  // In case of new secondaries: adjust safety ranges with CalcPBounds

  int i,m,marg;

  // Read old results: get info from base table AFTER update of last run.
  // Include everything, also marginals etc.

  if (RetCode == _ReadStat) 
  {
     for (i=0;i<Tab.Size();i++)
	 if (BTab[Tab.baseijk[i]] != NULL)
	 { 
           BTab[Tab.baseijk[i]]->status = Tab.status[i];
	   switch (Tab.status[i])
	   {
	    case 'm': CalcPbounds(BTab[Tab.baseijk[i]],bounds); break;
	   }
	 }
  }
  else
  if (Tab.Dim() == 1)
  {
    // Als in bodemtabel leeg dan zo laten. Geen BackTrack secundaire veranderen
    // en maginaalcel (m==0) ongemoeid laten
    switch (Tab.status[0])   // Totaal generaal
    {
      case 'm': BTab[Tab.baseijk[0]]->status = 'b';
		        CalcPbounds(BTab[Tab.baseijk[0]],bounds); break;
    }

	for (m=1;m<Tab.Size();m++)  // Binnenwerk
	{	
          if (BTab[Tab.baseijk[m]] != NULL) 
	  {
    	        switch (Tab.status[m])
		{
		  case 'm': CalcPbounds(BTab[Tab.baseijk[m]],bounds); break;
		}
                if (BTab[Tab.baseijk[m]]->status != 'b')  
                BTab[Tab.baseijk[m]]->status = Tab.status[m];   // Is binnenwerk van Tab
          }
        }
  }
  else
  {
    for (m=0;m<Tab.Size();m++)
    {
      if (BTab[Tab.baseijk[m]] != NULL)  // Alleen als niet leeg is, anders toch 'z'
      {
	if (Tab.status[m] == 'm') CalcPbounds(BTab[Tab.baseijk[m]],bounds);

        marg=1;
        for (i=1;(i<=Tab.Dim()) && (marg!=0);i++)
          //marg *= Tab.ijk[i][m];     
          marg *= Tab.ijk[i-1][m];       
		
	if (marg==0)    // Als marg==0 dan marginaalcel
        {
	   if (Tab.status[m]=='m') // Nieuwe 'm'
		  BTab[Tab.baseijk[m]]->status = 'b';
	}
        else             // Binnenwerk
	{
	   if (BTab[Tab.baseijk[m]]->status != 'b')  
             BTab[Tab.baseijk[m]]->status = Tab.status[m];
	}
      }
    }
  }
  return 0; // No error
}

//int TestNewTable(CString TableList,JJTable& Tab,Table& BTab)
int TestNewTable(std::string& TableList,JJTable& Tab,Table& BTab)
{
  char *buffer;  
  if (TableList.empty()) return NEW_TAB; // Geen statussen bekend
  else
  {
        for (unsigned int k=0;k<TableList.size()/2;k++)
	{
	  if (BTab[Tab.baseijk[k]] != NULL)
	  {
                if (BTab[Tab.baseijk[k]]->status != TableList.at(k)) return NEW_TAB; // Status in basistabel is anders dan vorige keer
	  }
	  else   // Basistabel is leeg
	  {
                if (TableList.at(k) != 'z') 
		{
                    buffer = new char [45];
                    snprintf(buffer,42,"ERROR: Basistabel is leeg, maar Tstat = %c\n",TableList.at(k));
                    LogPrintf(LogName,buffer);
                    delete [] buffer;
                    return -90;        // Error genereren PWOF 02-12-2011
		}
	  }
	}
	return OLD_TAB;
  }
}

//int ReadOldStats(CString TableList, JJTable& Tab)
int ReadOldStats(std::string TableList, JJTable& Tab)
{
  int startk = TableList.size()/2;
  for (int k=0;k<startk;k++)
  {
    Tab.status[k] = TableList[k+startk];
  }  
  return _ReadStat;
}
