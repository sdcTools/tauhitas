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

#ifndef __DEFS_H
#define __DEFS_H

#include "ALList.h"
#include "AMyvector.h"
#include "AHier.h"
#include "ATabs.h"
#include "AErrors.h"
#include "resource.h"

extern StringTable ErrorStrings;

extern void WriteErrorToLog(std::string LogName, int ErrorCode);

//void DefineNonHierStruc(Vector<Hierarchy> S, Vector< Vector<int> >& ITab);
void DefineNonHierStruc(std::vector<Hierarchy> S, std::vector< std::vector<int> >& ITab);
void DefineLevels(Hierarchy& Var, int d);
void DefinePositions(Hierarchy& Var, int& startp);
void DefineDepths(Hierarchy& Var);
//void DefineSubGTabs(Vector<Hierarchy> Vars, Level& SubG, Vector< Vector< Vector<int> > >& SubGT);
void DefineSubGTabs(std::vector<Hierarchy> Vars, Level& SubG, std::vector< std::vector< std::vector<int> > >& SubGT);
//int  DefineSubGroups(Vector<Hierarchy> S, LList& SG);
int DefineSubGroups(std::vector<Hierarchy> S, LList& SG);
//void GetCats(Hierarchy Vari, int LNr, Vector< Vector<int> >& Cosi);
void GetCats(Hierarchy Vari, int LNr, std::vector< std::vector<int> >& Cosi);
//void FreeAll(Vector< Vector< Vector<int> > >& Coords);
//void FProduct(Vector< Vector<int> > V1, Vector< Vector< Vector<int> > >& V2);
void FProduct(std::vector< std::vector<int> > V1, std::vector< std::vector< std::vector<int> > >& V2);
//void Product(Vector< Vector<int> > V1,Vector< Vector< Vector<int> > >& V2);
void Product(std::vector< std::vector<int> > V1, std::vector< std::vector< std::vector<int> > >& V2);

#endif /* __DEFS_H */
