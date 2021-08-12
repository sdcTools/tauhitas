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

#ifndef __LLIST_H
#define __LLIST_H

#include <string.h>
#include <cstdio>
#include <stdlib.h>
#include <algorithm>
#include <vector>

#ifndef MaxM
#define MaxM 11   // Maximale aantal variabelen is default 10
                  // (+1 voor null-delimiter) Ook vanwege decodering naam
#endif

struct Level {
    std::vector<int> Gname; 
    int Gnr;
};

//typedef int (__cdecl *GENERICCOMPAREFN) (const void *elem1, const void *elem2);
//typedef int (__cdecl *LEVELCOMPAREFN) (const Level *elem1, const Level *elem2);

typedef std::vector<Level> LevelArray;
/*class LevelArray : public std::vector<Level>
{
public:
	void Sort(LEVELCOMPAREFN pfnCompare = Compare);
protected:
	static int __cdecl Compare(const Level *plevel1, const Level *plevel2);
};
*/
class LList
{
 private:
   int dim;
   LevelArray data;
 public:
   int size() {return dim;};
   void Free();
   Level* operator[] (int i) {return &data[i];};
   void Make(int L);
   void Fill();
   void Sort();
   void Oplus(LList& L1);
   void Printf();
};
#endif   /* __LLIST_H */
