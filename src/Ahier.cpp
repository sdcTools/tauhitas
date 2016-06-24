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

#include "AHier.h"
#include "ADefs.h"
#include "AMyIO.h"
#include <iostream>

// Public functions of class Hierarchy
void Hierarchy::Free()
{
   depth=0;
   dim=0;
   level=0;
   position=0;
   delete[] body;
};

//
// Retourneer (sub-)hierarchy van laatste categorie op level Lnr
//
void ReturnCat(Hierarchy& hier, int Lnr, Hierarchy& out)
{
  int hierdepth = hier.Depth();
  int i;

  if (Lnr <= hierdepth)
  {
    out = hier;
    for (i=1;i<Lnr;i++)
      out = out[out.Dim()];
  }
};

void ReturnDepth(Hierarchy& hier, int& Depth)
{
  int i,d=0;

  for (i=1;i<=hier.Dim();i++)
    if (hier[i].Depth() > d) d = hier[i].Depth();
  
  Depth=d;
}

//
// Voeg een extra SubLevel toe en
// verhoog dim van Level met 1
//
void AddSubLevel(Hierarchy& hier, Hierarchy& Subhier)
{
  int i;
  Hierarchy& hulphier = hier;
  Hierarchy oldLevel = hulphier;
  char* oldname = hier.name;
  int hierdim = hulphier.Dim();
  int hierdepth = hulphier.Depth();
  if (hulphier.NoBody()) {hierdim = 0; hierdepth=1;} // Body is leeg
  hulphier.Create(hierdim + 1);
  hulphier.IncDepth(hierdepth);
  hier.name = oldname;
  for (i=1;i<=hierdim;i++)
  {
    hulphier[i].name = oldLevel[i].name;
    hulphier[i] = oldLevel[i];
  }
  hulphier[hierdim + 1] = Subhier;
  hulphier[hierdim + 1].name = Subhier.name;
  oldLevel.Free();
};
//
// Inlezen van Hierarchische structuur van ��n variabele uit file
// RemoveBogi = true verwijdert boguslevels en houdt info bij in BogusList
void ReadHierarch(Hierarchy& out, int& Bdim, const char* filename, bool RemoveBogi, StringMap& BogusList)
{
  Hierarchy  dummy;
  FILE *in;
  char line[256];
  char* catname, *name;
  int LevelNr,j,hulpdim;
  int start=0;

  in = OpenFile(filename,"r");    // Open file met hierarchische structuur
  if (in==NULL) 
  {
      LogPrintf(LogName,"File ");
      LogPrintf(LogName,filename);
      LogPrintf(LogName,"not found\n");
      WriteErrorToLog(LogName,HITAS_FILENOTFOUND);
      exit(HITAS_FILENOTFOUND);
  }
  out.Init();                  // Initialiseer nieuwe variabele
  dummy.Init();
  Bdim=0;
  while (!feof(in))
  {
    if (fgets(line,256,in) != NULL)  // Lees categorie in
    {
      Bdim++;
      catname = strrchr(line,'.');  // Bepaal Level van ingelezen categorie
      (catname == NULL) ? catname=line : catname=&catname[1];
      LevelNr = strlen(line) - strlen(catname);
 
      int namelength = strlen(catname)-1;
      if (catname[namelength] != '\n') namelength++;
      name = (char*) calloc(namelength+1,sizeof(char));
	  if (name==NULL) 
	  {
                LogPrintf(LogName,"Not enough memory for string name\n");
		WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
		exit(HITAS_NOTENOUGHMEMORY);
	  }
      strncat(name,catname,namelength);  // name bevat nu "puur" de naam
    
      if (LevelNr == 0) out.name = name;
      dummy.name = name;

      if (LevelNr > out.Depth())         // Nog niet bestaand level
      {
        if (LevelNr != 1)              // Sublevel van level 0 moet apart
        {
          Hierarchy hulphier;
          ReturnCat(out,LevelNr-1,hulphier);
          hulpdim = hulphier.Dim();
          AddSubLevel(hulphier[hulpdim],dummy); // Sublevel aan laatste
          out.IncDepth(1);                      // categorie level Lnr-1
          Hierarchy* hhier = &out;
          Hierarchy tmp = out;
          for (j=1;j<LevelNr;j++)
          {
            hhier = &tmp[hhier->Dim()];
            hhier->IncDepth(1);
            tmp = *hhier;
          }
        }                                       // toevoegen
        else
          AddSubLevel(out,dummy);
      }
      else                              // Bestaand level
        if (LevelNr != 0)               // Als level=0 dan niets doen
	{
          if (LevelNr != 1)             // Sublevel van level 0 moet apart
	  {
            Hierarchy hulphier;
            ReturnCat(out,LevelNr-1,hulphier);
            hulpdim = hulphier.Dim();
            AddSubLevel(hulphier[hulpdim],dummy); // Sublevel aan laatste
	  }                                       // categorie level Lnr-1
          else                                    // toevoegen
            AddSubLevel(out,dummy);
	}
     }
  }
  fclose(in);
  dummy.Free();
  
  if (RemoveBogi)
	RemoveBogussen(out, Bdim, BogusList); // Verwijder de levels met ��n uitsplitsing
                               // en pas totaal aantal codes aan 
  
  DefineLevels(out,0);     // Bereken de levelnummers 

  DefineDepths(out);

  DefinePositions(out,start);  // Bereken de intern te gebruiken codes
};

void RemoveBogussen(Hierarchy& hier, int& BDim, StringMap& BogusList)
{
  int i;
  Hierarchy tmphier;
  
  for (i=1;i<=hier.Dim();i++)   // Zak door Hierarchie heen
    RemoveBogussen(hier[i], BDim, BogusList);
    
  if (hier.Dim()==1)  // slecht ��n subcategorie, dus bogus
  {
    BogusList[hier.name] = hier[1].name;
    tmphier = hier;
    hier = hier[1];
    tmphier.Free();
    free(tmphier.name);
    BDim--;           // Aantal categorieen is met 1 afgenomen 
  }
};

//void MakeCodeList(Hierarchy& Var, CMap<CString, LPCSTR, int, int>& CodeList, CMap<CString, LPCSTR, int, int>& AllCodes)
void MakeCodeList(Hierarchy& Var, std::map<std::string, int>& CodeList, std::map<std::string, int>& AllCodes)
{
  int i;
  if (Var.NoBody() == false)       // Body is dus NIET leeg
    for (i=1;i<=Var.Dim();i++)
    {
        AllCodes[Var[i].name] = Var[i].position; 
        if (Var[i].Depth() == 0)
                CodeList[Var[i].name] = Var[i].position;
        else
                MakeCodeList(Var[i],CodeList,AllCodes);
    }
};

void ReadCodeNames(const char* FileName, std::vector<std::string>& Var)
{
  FILE* in;
  std::string Reg;
  char str[256];
  
  in = OpenFile(FileName,"r");
  if (in==NULL) 
  {
        LogPrintf(LogName,"File ");
        LogPrintf(LogName,FileName);
        LogPrintf(LogName,"not found\n");
        WriteErrorToLog(LogName,HITAS_FILENOTFOUND);
	exit(HITAS_FILENOTFOUND);
  }
  while(!feof(in))
  {
    if (fgets(str,256,in)!=NULL) // No error reading
	{
  	  Reg=str;
          Reg.erase(std::remove(Reg.begin(), Reg.end(), '.'), Reg.end());
          Reg.erase(std::remove(Reg.begin(), Reg.end(), '\n'), Reg.end());
          Var.push_back(Reg);
	}
  } 
  fclose(in);
};

void PrintHierarch(Hierarchy& Var, FILE& filename)
{
  int i;
  if (Var.NoBody() == false)       // Body is dus NIET leeg
    for (i=1;i<=Var.Dim();i++)
    {
      fprintf(&filename,"%10s %6d %6d %6d\n", Var[i].name, Var[i].position, Var[i].Level(), Var[i].Depth());
      PrintHierarch(Var[i],filename);
    }
};

void FreeHierarchy(Hierarchy& Var)
{
  int j;
  if (Var.NoBody() == false)
    for (j=1;j<=Var.Dim();j++)
    {
      FreeHierarchy(Var[j]);
      Var[j].Free();
      free(Var[j].name);
    }
};
