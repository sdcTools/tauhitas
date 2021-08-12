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

#ifndef __TABS_H
#define __TABS_H
#include <stdlib.h>
#include <cstdio>
#include <malloc.h>
#include <math.h>
#include <vector>
#include <string>
#include <string.h>
#include "Aconst.h"
#include "AHier.h"
#include "AErrors.h"
//#include "Amain.h"
#include "AMyvector.h"

//typedef CArray<CMap<int,int,CString,LPCSTR>, CMap<int,int,CString,LPCSTR>& > InExCodeLijst;
typedef std::vector< std::map<int,std::string> > InExCodeLijst;
//typedef CArray<CMap<CString,LPCSTR,int,int>, CMap<CString,LPCSTR,int,int>& > ExInCodeLijst;
typedef std::vector< std::map<std::string,int> > ExInCodeLijst;

// Om zuiniger met geheugen om te gaan:
// simpele Table structuur

typedef struct
	{double value; // Celwaarde
         char status;  // Status van de cel
         std::vector<double> Pbounds; // Protection levels
         std::vector<double> Ebounds; // External (apriori) bounds
	 int CelCount; // Aantal bijdragers
	 double CelCost;  // Cost to suppress this cell
	} cell;

typedef struct
	{int position;	// cell-number
	 double value;	// cell value
	 int weight;
	 char status;
	 double lb;
	 double ub;
	 double lpl;
	 double upl;
	 double spl;
	} addcellinfo;

typedef struct
	{double rhs;
	 int ncard;  // number of cells in restriction
	 int* Cells; // Cells in restriction
	} addsuminfo;

extern void WriteErrorToLog(std::string LogName, int ErrorCode);

class Table
{
  typedef cell* column;
  private:
    //Vector<int> subdims;
    std::vector<int> subdims;
    int size;
    column* body;
    void CreateColumn(column& newcol, int N);
  public:
    //void Getijk(int m, Vector<int> Gijk);
    void Getijk(int m, std::vector<int>& Gijk);
    //void MakeColumn(Vector<int> ijk);
    void MakeColumn(std::vector<int> ijk);
    int Dim() {return subdims.size();};
    int Size() {return size;};
    //int ColumnSize() {return subdims[subdims.size()];};
    int ColumnSize() {return subdims[subdims.size()-1];};
    //void Make(Vector<int> Dims);
    void Make(std::vector<int> Dims);
    column Column(int i) {if (body[i]==NULL) return NULL; else return body[i];};
    //cell* operator[] (Vector<int> ijk);
    cell* operator[] (std::vector<int> ijk);
    column* operator[] (int i) {return &body[i];};
    int ReadData(const char* FName);
    void Free();
    int PrintData();
    void APrintTabel(FILE& uitfile, const char* States);
    void PrintStatusCells(const char* States, InExCodeLijst& TerugLijst, FILE& out);
};

// Tabel in formaat om aan JJ-routines te voeren
class JJTable
{
  private:
    int size;
    int basedim;
  public:
    int BDim(){return basedim;};
    int Dim() {return N.size();};
    int Size(){return size;};
    //Vector<int> N;        // Voor JJ: maximaal N1, N2, N3
    std::vector<int> N;        // for JJ: max N1, N2, N3
    //Vector<int*> ijk;     // Voor JJ: maximaal *ijk1, *ijk2, *ijk3
    std::vector<int*> ijk;     // for JJ: max *ijk1, *ijk2, *ijk3
    //Vector<int>* baseijk; // Met de coordinaten in de bodemtabel
    std::vector<int>* baseijk; // With the coordinates in teh basetable
    int *weight,*count;
    int NumberOfUnsafeCells;
    int NumberOfSafeCells;
    double *data,*lpl,*upl,*lb,*ub,*costs;
    double MinMargVal, MinInteriorVal;
    bool Skip;
    char *status;
    std::vector<addsuminfo> AdditionalConstraints;
    std::vector<addcellinfo> AdditionalCells;
    void PrintData(FILE& Uitfile);
    //void PrintWeights(FILE& Uitfile, Vector< Vector<int> > SGTab);
    void PrintWeights(FILE& Uitfile, std::vector< std::vector<int> > SGTab);
    void PrintStats(FILE& Uitfile);
    void PrintAdditionalInfo(FILE& Uitfile);
    void PrintBasisStats(FILE& Uitfile, Table& BTab);
    void SaveBasisStatsBefore(std::string& TableList, Table& BTab);
    void SaveBasisStatsAfter(std::string& TableList, Table& BTab);
    //void Init(Vector<int> L);
    void Init(std::vector<int> L);
    void Free();
};
#endif   /* __TABS_H */
