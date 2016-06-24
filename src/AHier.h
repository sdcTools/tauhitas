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

#ifndef __HIER_H
#define __HIER_H
#include <malloc.h>
#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <vector>

class Hierarchy
{
  private:
    int depth;    // Aantal Levels
    int dim;      // Aantal categori�n op hoogste level
    int level;    // Levelnummer
    Hierarchy* body;
  public:
    char* name;
    int position;
    const int Depth() const {return depth;};
    const int Dim() const {return dim;};
    const int Level() const {return level;};
    void SetLevel(int n) {level = n;};
	void SetDepth(int n) {depth = n;};
    void IncDepth(int n) {depth += n;};
    void IncDim(int n) {dim += n;};
    Hierarchy& Body() {return *body;};
    Hierarchy& operator [] (int i) {return body[i-1];};              // i=1,...
    const Hierarchy& operator [] (int i) const {return body[i-1];};  // i=1,...
    void Init() {body = NULL; depth = 0; dim = 0; position = 0; name = NULL;};
    void Create(int N = 1)
       {body = new Hierarchy[N]; depth = 0; dim = N; position = 0; name = NULL;};
    void Free();
    bool NoBody() const {return (body == NULL);};
};

//typedef CMap<CString,LPCSTR,CString,LPCSTR> StringMap;
typedef std::map<std::string, std::string> StringMap;

// Procedures voor/met Hierarchische structuren
// implementatie in hier.cpp
void ReturnCat(Hierarchy& hier, int Lnr, Hierarchy& out);
void ReturnDepth(Hierarchy& hier, int& Depth);
void AddSubLevel(Hierarchy& hier, Hierarchy& Subhier);
void ReadHierarch(Hierarchy& out, int& Bdim, const char* filename, bool RemoveBogi, StringMap& BogusList);
//void ReadCodeNames(char* FileName, CArray<CString,LPCSTR>& Var);
void ReadCodeNames(const char* FileName, std::vector<std::string>& Var);
void PrintHierarch(Hierarchy& Var, FILE& filename);
void FreeHierarchy(Hierarchy& Var);
//void MakeCodeList(Hierarchy& Var, CMap<CString, LPCSTR, int, int>& CodeList, CMap<CString, LPCSTR, int, int>& AllCodes);
void MakeCodeList(Hierarchy& Var, std::map<std::string, int>& CodeList, std::map<std::string, int>& AllCodes);
void RemoveBogussen(Hierarchy& hier, int& BDim, StringMap& BogusList);

#endif /* __HIER_H  */
