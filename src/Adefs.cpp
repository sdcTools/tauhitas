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

#include "ADefs.h"
#include <iostream>

void DefineNonHierStruc(Vector<Hierarchy> S, Vector< Vector<int> >& ITab)
{
  int i,j,M=S.size();
  int VN;
  ITab.Make(M);
  for (i=1;i<=M;i++)          // M = aantal variabelen
  {
	VN = S[i].Dim();   // 1 + aantal cats op level 1
	ITab[i].Make(VN); 
	for (j=1;j<=VN;j++)
	  ITab[i][j] = S[i][j].position;
  }
}

void DefineLevels(Hierarchy& Var, int d)
{
   int i;
   Var.SetLevel(d);
   for (i=1; i<=Var.Dim(); i++)
     DefineLevels(Var[i], d+1);
};

void DefinePositions(Hierarchy& Var, int& startp)
{
   int i;
   Var.position = startp++;
   for (i=1; i<=Var.Dim(); i++)
   	 DefinePositions(Var[i], startp);
};

void DefineDepths(Hierarchy& Var)
{
   int i,Depth;
   
   for (i=1; i<=Var.Dim();i++) DefineDepths(Var[i]);
   
   ReturnDepth(Var,Depth);
   
   if (Var.Depth() != 0) Var.SetDepth(Depth + 1);
}

int DefineSubGroups(Vector<Hierarchy> S, LList& SG)
{
  LList V;
  int M=S.size(); //Aantal Variabelen
  int i;
//  char buffer[40];

  if (M<=1) 
  {
	  LogPrintf(LogName,"Error in number of Vars\n");
	  WriteErrorToLog(LogName,HITAS_NOVARIABLES);
	  return(HITAS_NOVARIABLES);
  }

  SG.Make(0);
  SG.Fill();
  
  for (i=1;i<=M;i++)
  {
    V.Make(S[i].Depth());
    V.Fill();
    SG.Oplus(V);
    V.Free();
  }
  SG.Sort();
  return 0;
}

void DefineSubGTabs(Vector<Hierarchy> Vars, Level& SubG, Vector< Vector< Vector <int> > >& SubGT)
{
  int i,j;
  unsigned int usi;
  Vector<int> Levels;
  Vector< Vector< Vector<int> > > Coords;
  Levels.Make(SubG.Gname.size());
  for (usi=0;usi<SubG.Gname.size();usi++)
	  Levels[usi+1] = SubG.Gname[usi];

  Coords.Make(Levels.size());
  for (i=1;i<=Levels.size();i++)        // voor iedere variabele
  {
    if (Levels[i] != 0)
    {
      Coords[i].Init();
      GetCats(Vars[i],Levels[i],Coords[i]);
    }
    else                        // Level 0 apart behandelen
    {
      Coords[i].Make(1);
      Coords[i][1].Make(1);
      Coords[i][1][1] = 0;
    }
  }
  // Free some memory
  Levels.Free();

  // Construeer de coordinaten van de tabellen in SubG

  SubGT.Make(1);                     // Eerste var gewoon copieren
  SubGT[1].Make(Coords[1].size());
  for (i=1;i<=Coords[1].size();i++)
  {
    SubGT[1][i].Make(Coords[1][i].size());
    for (j=1;j<=Coords[1][i].size();j++)
      SubGT[1][i][j] = Coords[1][i][j];
  }

  if (Coords.size() > 1)             // overige vars toevoegen
  {
    FProduct(Coords[2],SubGT);       // eerste keer is speciaal

    for (i=3;i<=Coords.size();i++)   // kruisen met rest vars
      Product(Coords[i],SubGT);
  }

  // Free local variables
  FreeAll(Coords);
};


// Bij eerste vermenigvuldiging wordt V2[i].size() ALTIJD 2
void FProduct(Vector< Vector<int> > V1, Vector< Vector< Vector<int> > >& V2)
{
  int i,j,k,m;
  Vector< Vector< Vector<int> > > OldV = V2;
  int hdim = V1.size()*V2[1].size();

  V2.Make(hdim);

  for (i=1;i<=OldV[1].size();i++)
  {
    for (j=1;j<=V1.size();j++)
    {
      m = (i-1)*V1.size() + j;
      V2[m].Make(2);
      V2[m][1].Make(OldV[1][i].size());
      for (k=1;k<=OldV[1][i].size();k++)
        V2[m][1][k] = OldV[1][i][k];
      V2[m][2].Make(V1[j].size());
      for (k=1;k<=V1[j].size();k++)
        V2[m][2][k] = V1[j][k];
    }
  }
  // Free local variables
  FreeAll(OldV);
};

// Bij latere vermenigvuldiging wordt V2[i].size() NIET ALTIJD 2
void Product(Vector< Vector<int> > V1, Vector< Vector< Vector<int> > >& V2)
{
  int i,j,k,m,ki;
  Vector< Vector< Vector<int> > > OldV = V2;
  int hdim = V1.size()*V2.size();

  V2.Make(hdim);

  for (i=1;i<=OldV.size();i++)
  {
    for (j=1;j<=V1.size();j++)
    {
      m = (i-1)*V1.size() + j;
      V2[m].Make(OldV[i].size()+1);
      for (k=1;k<=OldV[i].size();k++)
      {
        V2[m][k].Make(OldV[i][k].size());
        for (ki=1;ki<=OldV[i][k].size();ki++)
          V2[m][k][ki] = OldV[i][k][ki];
      }
      V2[m][OldV[i].size()+1].Make(V1[j].size());
      for (ki=1;ki<=V1[j].size();ki++)
        V2[m][OldV[i].size()+1][ki] = V1[j][ki];
    }
  }
  // Free local variables
  FreeAll(OldV);
};

void GetCats(Hierarchy Vari, int LNr, Vector< Vector<int> >& Cosi)
{
  int j;
  if (LNr == 1)
  {
    if (Vari.Dim() != 0)
    {
      Cosi.Grow(1);
      Cosi[Cosi.size()].Make(Vari.Dim());
      for (j=1;j<=Vari.Dim();j++)
        Cosi[Cosi.size()][j] = Vari[j].position;
    }
  }
  else
    if (LNr > 1)
      for (j=1;j<=Vari.Dim();j++)
        if (Vari[j].NoBody() == false)
          GetCats(Vari[j],LNr-1,Cosi);    // Afdalen in hierarchie
};

void FreeAll(Vector< Vector< Vector<int> > >& Coords)
{
  int i,j;
  for (i=1;i<=Coords.size();i++)
  {
    for (j=1;j<=Coords[i].size();j++)
      Coords[i][j].Free();
    Coords[i].Free();
  }
  Coords.Free();
};
