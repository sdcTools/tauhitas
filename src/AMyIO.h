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

#ifndef __MYIO_H
#define __MYIO_H

#include <math.h>
#include <cstdio>
#include <stdlib.h>
//#include <tchar.h>
#include "Aconst.h"
#include "ATabs.h"
#include "ALList.h"
#include "AMyvector.h"

#define __min(a,b)  (((a) < (b)) ? (a) : (b))
#define __max(a,b)  (((a) > (b)) ? (a) : (b))
extern bool PPDEBUG;
extern double seconds();
extern std::string LogName;

std::string to_string(int _Val);

void WriteErrorToLog(std::string LogName, int ErrorCode);

std::string PrepFile(const char* Name);

FILE* OpenFile(const char* Name, const char* stat);
void PrintConstants(const char* WegFileName, int M);

void ReadConstants(const char* ConstFileName);

void LogPrintf(std::string LogName, std::string LogMessage);
void LogPrintf(std::string LogName, const char* LogMessage);

void AReadHistory(Table BTab, int M, const char* FileName, const char* States);
void ReadBTInfo(Table& BTab, int M, const char* FileName, const char* States);
//void PrintSubG(FILE &FName, Level* SGi, Vector< Vector<int> > SGTj);
void PrintSubG(FILE &FName, Level* SGi, std::vector< std::vector<int> > SGTj);
void AddHistory(JJTable& BTtab, Table& BTab);
/*void PrintBasisTabel(FILE& uitfile, Table& BasisTab, char* States, std::vector< std::vector<std::string> >& Vars, 
			ExInCodeLijst& Codes, std::vector<StringMap>& BogusLists,
                        Vector<int>& Coords, std::vector<std::string>& ExCoords);*/
#endif
