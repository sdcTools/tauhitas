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

//void DefineNonHierStruc(Vector<Hierarchy> S, Vector< Vector<int> >& ITab)
void DefineNonHierStruc(std::vector<Hierarchy> S, std::vector< std::vector<int> >& ITab)
{
  int i,j,M=S.size();
  int VN;
  //ITab.Make(M);
  ITab.resize(M);
  for (i=1;i<=M;i++)          // M = number of variables
  {
	//VN = S[i].Dim();   // 1 + number of cats on level 1
        VN = S[i-1].Dim();   // 1 + number of cats on level 1
	//ITab[i].Make(VN); 
        ITab[i-1].resize(VN); 
	for (j=1;j<=VN;j++)
	  //ITab[i][j] = S[i][j].position;
          ITab[i-1][j-1] = S[i-1][j].position;
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

//int DefineSubGroups(Vector<Hierarchy> S, LList& SG)
int DefineSubGroups(std::vector<Hierarchy> S, LList& SG)
{
  LList V;
  int M=S.size(); //Aantal Variabelen
//  char buffer[40];

  if (M<=1) 
  {
	  LogPrintf(LogName,"Error in number of Vars\n");
	  WriteErrorToLog(LogName,HITAS_NOVARIABLES);
	  return(HITAS_NOVARIABLES);
  }

  SG.Make(0);
  SG.Fill();
  
  for (int i=1;i<=M;i++)
  {
    //V.Make(S[i].Depth());
      V.Make(S[i-1].Depth());
    V.Fill();
    SG.Oplus(V);
    V.Free();
  }
  SG.Sort();
  return 0;
}

//void DefineSubGTabs(Vector<Hierarchy> Vars, Level& SubG, Vector< Vector< Vector <int> > >& SubGT)
void DefineSubGTabs(std::vector<Hierarchy> Vars, Level& SubG, std::vector< std::vector< std::vector <int> > >& SubGT)
{
  //Vector<int> Levels;
  std::vector<int> Levels;
  //Vector< Vector< Vector<int> > > Coords;
  std::vector< std::vector< std::vector<int> > > Coords;
  //Levels.Make(SubG.Gname.size());
  Levels.reserve(SubG.Gname.size());
  for (size_t usi=0;usi<SubG.Gname.size();usi++)
	  //Levels[usi+1] = SubG.Gname[usi];
            Levels.push_back(SubG.Gname[usi]);

  //Coords.Make(Levels.size());
  Coords.resize(Levels.size());
  for (size_t i=1;i<=Levels.size();i++)        // voor iedere variabele
  {
    //if (Levels[i] != 0)
    if (Levels[i-1] != 0)
    {
      //Coords[i].Init();
      //GetCats(Vars[i],Levels[i],Coords[i]);
      GetCats(Vars[i-1],Levels[i-1],Coords[i-1]);
    }
    else                        // Level 0 apart behandelen
    {
      //Coords[i].Make(1);
      Coords[i-1].resize(1);  
      //Coords[i][1].Make(1);
      Coords[i-1][0].resize(1);
      //Coords[i][1][1] = 0;
      Coords[i-1][0][0] = 0;
    }
  }
  // Free some memory
  //Levels.Free();

  // Construeer de coordinaten van de tabellen in SubG

  //SubGT.Make(1);                     // Eerste var gewoon copieren
  SubGT.clear();
  SubGT.assign(1,Coords[0]);           // Eerste var gewoon copieren
  //SubGT[0] = Coords[0];
/*  //SubGT[1].Make(Coords[1].size());
  SubGT[0].resize(Coords[0].size());
  //for (i=1;i<=Coords[1].size();i++)
  for (size_t i=1;i<=Coords[0].size();i++)
  {
    //SubGT[1][i].Make(Coords[1][i].size());
    SubGT[0][i-1].resize(Coords[0][i-1].size());
    //for (j=1;j<=Coords[1][i].size();j++)
    for (size_t j=1;j<=Coords[0][i-1].size();j++)
      //SubGT[1][i][j] = Coords[1][i][j];
      SubGT[0][i-1][j-1] = Coords[0][i-1][j-1];
  }
*/
  if (Coords.size() > 1)             // overige vars toevoegen
  {
    //FProduct(Coords[2],SubGT);       // eerste keer is speciaal
    FProduct(Coords[1], SubGT);       // eerste keer is speciaal

    for (size_t i=3;i<=Coords.size();i++)   // kruisen met rest vars
      //Product(Coords[i],SubGT);
        Product(Coords[i-1],SubGT);
  }

  // Free local variables
  //FreeAll(Coords);
};


// Bij eerste vermenigvuldiging wordt V2[i].size() ALTIJD 2
//void FProduct(Vector< Vector<int> > V1, Vector< Vector< Vector<int> > >& V2)
void FProduct(std::vector< std::vector<int> > V1, std::vector< std::vector< std::vector<int> > >& V2)
{
  size_t i,j,k,m;
  //Vector< Vector< Vector<int> > > OldV = V2;
  std::vector< std::vector< std::vector<int> > > OldV = V2;
  //int hdim = V1.size()*V2[1].size();
  int hdim = V1.size()*V2[0].size();

  //V2.Make(hdim);
  V2.clear();
  V2.resize(hdim);

  //for (i=1;i<=OldV[1].size();i++)
  for (i=1;i<=OldV[0].size();i++)
  {
    for (j=1;j<=V1.size();j++)
    {
      m = (i-1)*V1.size() + j;
      //V2[m].Make(2);
      V2[m-1].clear();
      V2[m-1].resize(2);
      //V2[m][1].Make(OldV[1][i].size());
      V2[m-1][0].clear();
      V2[m-1][0].resize(OldV[0][i-1].size());
      //for (k=1;k<=OldV[1][i].size();k++)
      for (k=1;k<=OldV[0][i-1].size();k++)
        //V2[m][1][k] = OldV[1][i][k];
        V2[m-1][0][k-1] = OldV[0][i-1][k-1];  
      //V2[m][2].Make(V1[j].size());
      V2[m-1][1].clear();
      V2[m-1][1].resize(V1[j-1].size());
      //for (k=1;k<=V1[j].size();k++)
      for (k=1;k<=V1[j-1].size();k++)
        //V2[m][2][k] = V1[j][k];
        V2[m-1][1][k-1] = V1[j-1][k-1];
    }
  }
  // Free local variables
  //FreeAll(OldV);
};

// Bij latere vermenigvuldiging wordt V2[i].size() NIET ALTIJD 2
//void Product(Vector< Vector<int> > V1, Vector< Vector< Vector<int> > >& V2)
void Product(std::vector< std::vector<int> > V1, std::vector< std::vector< std::vector<int> > >& V2)
{
  size_t i,j,k,m,ki;
  //Vector< Vector< Vector<int> > > OldV = V2;
  std::vector< std::vector< std::vector<int> > > OldV = V2;
  int hdim = V1.size()*V2.size();

  //V2.Make(hdim);
  V2.clear();
  V2.resize(hdim);

  for (i=1;i<=OldV.size();i++)
  {
    for (j=1;j<=V1.size();j++)
    {
      m = (i-1)*V1.size() + j;
      //V2[m].Make(OldV[i].size()+1);
      V2[m-1].clear();
      V2[m-1].resize(OldV[i-1].size()+1);
      //for (k=1;k<=OldV[i].size();k++)
      for (k=1;k<=OldV[i-1].size();k++)
      {
        //V2[m][k].Make(OldV[i][k].size());
        V2[m-1][k-1].clear();
        V2[m-1][k-1].resize(OldV[i-1][k-1].size());
        //for (ki=1;ki<=OldV[i][k].size();ki++)
        for (ki=1;ki<=OldV[i-1][k-1].size();ki++)
          //V2[m][k][ki] = OldV[i][k][ki];
          V2[m-1][k-1][ki-1] = OldV[i-1][k-1][ki-1];
      }
      //V2[m][OldV[i].size()+1].Make(V1[j].size());
      V2[m-1][OldV[i-1].size()].clear();
      V2[m-1][OldV[i-1].size()].resize(V1[j-1].size());
      //for (ki=1;ki<=V1[j].size();ki++)
      for (ki=1;ki<=V1[j-1].size();ki++)
        //V2[m][OldV[i].size()+1][ki] = V1[j][ki];
        V2[m-1][OldV[i-1].size()][ki-1] = V1[j-1][ki-1];
    }
  }
  // Free local variables
  //FreeAll(OldV);
};

//void GetCats(Hierarchy Vari, int LNr, Vector< Vector<int> >& Cosi)
void GetCats(Hierarchy Vari, int LNr, std::vector< std::vector<int> >& Cosi)
{
  int j;
  if (LNr == 1)
  {
    if (Vari.Dim() != 0)
    {
      //Cosi.Grow(1);
      Cosi.resize(Cosi.size()+1);
      //Cosi[Cosi.size()].Make(Vari.Dim());
      Cosi[Cosi.size()-1].clear();
      Cosi[Cosi.size()-1].resize(Vari.Dim()); //Cosi.size() is now old size + 1
      for (j=1;j<=Vari.Dim();j++)
        //Cosi[Cosi.size()][j] = Vari[j].position;
        Cosi[Cosi.size()-1][j-1] = Vari[j].position;
    }
  }
  else
    if (LNr > 1)
      for (j=1;j<=Vari.Dim();j++)
        if (Vari[j].NoBody() == false)
          GetCats(Vari[j],LNr-1,Cosi);    // Afdalen in hierarchie
};

/*void FreeAll(Vector< Vector< Vector<int> > >& Coords)
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
*/