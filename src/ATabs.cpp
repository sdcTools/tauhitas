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

#include "ATabs.h"
#include "AMyIO.h"

// Om zuiniger met geheugen om te gaan:
// simpele Table structuur
//
// Public functions
//void Table::Make(Vector<int> Dims)
void Table::Make(std::vector<int> Dims)
{
  //subdims.Make(Dims.size()); // subsizes van (n-1)-dimensionale tabel + size dim n
  subdims.clear();
  subdims.resize(Dims.size()); // subsizes van (n-1)-dimensionale tabel + size dim n

  size = 1;
  for (size_t i=1;i<=subdims.size()-1;i++) // vector van (n-1)-dimensionale tabel
  {
    //size *= Dims[i];
    size *= Dims[i-1];
    //subdims[i]=1;
    subdims[i-1]=1;
    for (size_t j=i+1;j<=(subdims.size()-1);j++)
      //subdims[i] *= Dims[j];
      subdims[i-1] *= Dims[j-1];
  }
  //subdims[subdims.size()] = Dims[subdims.size()];
  subdims[subdims.size()-1] = Dims[subdims.size()-1];
  
  body = new column[size];
  if (body==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
	  exit(HITAS_NOTENOUGHMEMORY);
  }
  for (int i=0;i<size;i++)
    body[i] = NULL;
}

//cell* Table::operator[] (Vector<int> ijk)
cell* Table::operator[] (std::vector<int> ijk)
{
  int position;
  char buffer[256];
  if (ijk.size() != subdims.size())
  {
      sprintf(buffer,"ijk.size()=%zd should equal subdims.size()=%zd\n",ijk.size(),subdims.size());
      LogPrintf(LogName,"Dimensions don't match.\n Something is seriously wrong in your table!\n");
      LogPrintf(LogName,buffer);
      WriteErrorToLog(LogName,HITAS_TABDIMSDONTMATCH);
      throw(HITAS_TABDIMSDONTMATCH);      // Was exit()
  }
  else
  {
    position=0;                         // position is in (n-1)-dim. tabel
    for (size_t i=1;i<=ijk.size()-1;i++)
      //position += ijk[i]*subdims[i];
        position += ijk[i-1]*subdims[i-1];
  }
  if (body[position]==NULL)
    return NULL;
  else
    //return &body[position][ijk[ijk.size()]];  // laatste index is in de kolom
                                              // "op" de (n-1)-dim. tabel
    return &body[position][ijk[ijk.size()-1]];  // last index is in the column
                                                // "on top of" the (n-1)-dim. table
}

//void Table::MakeColumn(Vector<int> ijk)
void Table::MakeColumn(std::vector<int> ijk)
{
  int position=0;                         // position is in (n-1)-dim. tabel
  for (size_t i=1;i<=ijk.size()-1;i++)
    //position += ijk[i]*subdims[i];
      position += ijk[i-1]*subdims[i-1];
  //CreateColumn(body[position], subdims[subdims.size()]);
  CreateColumn(body[position], subdims[subdims.size()-1]);
}

void Table::Free()
{
  int i;
  for (i=0;i<size;i++)
  {
    delete[] body[i];
    body[i]=NULL;
  }
  delete[] body;
  body = NULL;
  //subdims.Free();
  subdims.clear();
  size = 0;
}

int Table::ReadData(const char* FName)
{
  FILE* Invoer;

  Invoer=fopen(FName,"r");
  if (Invoer==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_FILENOTFOUND);
	  return(HITAS_FILENOTFOUND);
  }

  int coord,counter;
  double value,lb,ub,cost;
  int position;
  char status;

  while (!feof(Invoer))
  {
    position=0;               // position is in (n-1)-dim. tabel
    for (size_t k=1;k<subdims.size();k++)
    {
	fscanf(Invoer,"%d ",&coord);
	//position += coord*subdims[k];
        position += coord*subdims[k-1];
    }
    if (body[position] == NULL)
    {
        //CreateColumn(body[position],subdims[subdims.size()]);
        CreateColumn(body[position],subdims[subdims.size()-1]);
    }

    fscanf(Invoer,"%d %lf %c",&coord,&value,&status);
	
    body[position][coord].value=value;
    body[position][coord].status=status;
	
	if (strchr("ubm",status)!=NULL)
	{
                fscanf(Invoer,"%lf %lf",&lb,&ub);
                if (body[position][coord].Pbounds.size()==0)  
                {
                        body[position][coord].Pbounds.push_back(lb);    // Voorlopig alleen lpl en upl
                        body[position][coord].Pbounds.push_back(ub);
                }
                else
                {
                        body[position][coord].Pbounds[0]=lb;
                        body[position][coord].Pbounds[1]=ub;
                }
	}

	fscanf(Invoer,"%d %lf\n",&counter,&cost); // read remainder of line

	if (status != 'n')						  // neglegt remainder of line for 'n'-cells
	{
		body[position][coord].CelCount = counter;
		body[position][coord].CelCost = cost;
	}
  }
  fclose(Invoer);
  return 0;
}

int Table::PrintData()
{
  int i,j;
  FILE* out;
  out = OpenFile(PrepFile("smalluit.dat").c_str(),"a");
  if (out==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_FILEWRITE);
	  return(HITAS_FILEWRITE);
  }
  
  //Vector<int> coords;
  std::vector<int> coords;
  int dim = subdims.size()-1;
  //coords.Make(dim);
  coords.resize(dim);
  for (i=0; i<size;i++)
  {
    if (body[i] != NULL)
    {
      Getijk(i,coords);
      for (j=1;j<=dim;j++)
        //fprintf(out,"%4d ",coords[j]);
          fprintf(out,"%4d ",coords[j-1]);
      //for (j=0;j<=subdims[subdims.size()]-1;j++)
      for (j=0;j<=subdims[subdims.size()-1]-1;j++)
        fprintf(out,"%10.0lf(%c) ",body[i][j].value, body[i][j].status);
      fprintf(out,"\n");
    }
  }
  fclose(out);
  //coords.Free();
  return 0;
}

void Table::APrintTabel(FILE& uitfile, const char* States)
{
  int i,j,k;
  //Vector<int> coords;
  std::vector<int> coords;
  int dim = subdims.size()-1;
  //coords.Make(dim);
  coords.resize(dim,1);
  for (i=0; i<size;i++)
  {
    if (body[i] != NULL)
    {
        Getijk(i,coords);
        //for (k=0;k<=subdims[subdims.size()]-1;k++)
        for (k=0;k<subdims[subdims.size()-1];k++)
	{
                if (strchr(States,body[i][k].status) != NULL)
                {
		  for (j=1;j<=dim;j++)
                      //fprintf(&uitfile,"%4d ",coords[j]);
                      fprintf(&uitfile,"%4d ",coords[j-1]);
                  fprintf(&uitfile,"%4d\n",k);
		}
	}
    }
  }
  //coords.Free();
}


void Table::PrintStatusCells(const char* States, InExCodeLijst& TerugLijst, FILE& out)
{
  int i,j,k;
  //Vector<int> coords;
  std::vector<int> coords;
  int dim = subdims.size()-1;
  
  //coords.Make(dim);
  coords.resize(dim);
  for (i=0; i<size;i++)
  {
    if (body[i] != NULL) // Om ruimte te sparen alleen niet-lege cellen wegschrijven 
    {
      Getijk(i,coords);
      //for (j=0;j<=subdims[subdims.size()]-1;j++)
      for (j=0;j<=subdims[subdims.size()-1]-1;j++)
        if (strchr(States,body[i][j].status))
        {
          for (k=1;k<=dim;k++)     // Schrijf coordinaten
          { 
              fprintf(&out,"%9s ", TerugLijst[k-1].find(coords[k])->second.c_str());
          }
          fprintf(&out,"%9s ", TerugLijst[dim].find(j)->second.c_str());
          fprintf(&out,"%c\n", body[i][j].status);
        }
    }
  }
  //coords.Free();
}


void Table::Getijk(int m, std::vector<int>& Gijk)
{
  div_t x;
  int y=m;
  for (size_t i=1;i<=Gijk.size();i++)
  {
    //x = div(y,subdims[i]);
    x = div(y,subdims[i-1]);
    //Gijk[i]=x.quot;
    Gijk[i-1]=x.quot;
    y=x.rem;
  }
}

//
// Private functions
//
void Table::CreateColumn(column& newcol, int N)
{
	if (newcol != NULL) 
	{
		WriteErrorToLog(LogName,HITAS_RECREATECOLUMN);
		exit(HITAS_RECREATECOLUMN);
	}
	newcol = new cell[N];
	if (newcol==NULL) 
	{
		WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
		exit(HITAS_NOTENOUGHMEMORY);
	}
	for (int i=0;i<N;i++)
	{
		newcol[i].Pbounds.resize(0);
                newcol[i].Ebounds.resize(0);
	}
}

// Tabel in formaat om aan JJ-routines te voeren
// Public functions
//void JJTable::Init(Vector<int> L)
void JJTable::Init(std::vector<int> L)
{
  int i,j;
  basedim = L.size();
  int Rdim = 0;
  //for (i=1;i<=basedim;i++)
  for (i=0;i<basedim;i++)
    if (L[i]!=0) Rdim++;

  size=1;
  //N.Make(Rdim);
  N.clear();
  N.resize(Rdim);

  //j=1;
  j=0;
  //for (i=1;i<=basedim;i++)
  for (i=0;i<basedim;i++)
  {
    if (L[i]!=0)
    {
      N[j] = L[i]+1;   // Marginaal erbij
      size *= N[j++];
    }
  }

  //ijk.Make(Rdim);
  ijk.resize(Rdim);
  //for (i=1;i<=Rdim;i++)
  for (i=0;i<Rdim;i++)
  {
        ijk[i] = (int*) malloc(size*sizeof(int));
	if (ijk[i]==NULL) 
	{
		WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
		exit(HITAS_NOTENOUGHMEMORY);
	}
  }

  //baseijk = (Vector<int>*) malloc(size*sizeof(Vector<int>));
  baseijk = new std::vector<int>[size];
  if (baseijk==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
	  exit(HITAS_NOTENOUGHMEMORY);
  }
  for (i=0;i<size;i++){
    //baseijk[i].Make(basedim);
    baseijk[i].clear();
    baseijk[i].resize(basedim);
  }

  weight = (int*) malloc(size*sizeof(int));
  if (weight==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
	  exit(HITAS_NOTENOUGHMEMORY);
  }
  for (i=0;i<size;i++) weight[i] = (int) MAXDISTANCE;            
  
  costs = (double*) malloc(size*sizeof(double));
  if (costs==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
	  exit(HITAS_NOTENOUGHMEMORY);
  }
  for (i=0;i<size;i++) costs[i] = MAXDISTANCE;            

  count = (int*) malloc(size*sizeof(int));
  if (count==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
	  exit(HITAS_NOTENOUGHMEMORY);
  }
  
  data = (double*) malloc(size*sizeof(double));   
  if (data==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
	  exit(HITAS_NOTENOUGHMEMORY);
  }
  
  lpl = (double*) malloc(size*sizeof(double));
  if (lpl==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
	  exit(HITAS_NOTENOUGHMEMORY);
  }
  
  upl = (double*) malloc(size*sizeof(double));
  if (upl==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
	  exit(HITAS_NOTENOUGHMEMORY);
  }
  
  lb = (double*) malloc(size*sizeof(double));
  if (lb==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
	  exit(HITAS_NOTENOUGHMEMORY);
  }
  
  ub = (double*) malloc(size*sizeof(double));
  if (ub==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
	  exit(HITAS_NOTENOUGHMEMORY);
  }
  
  status = (char*) malloc(size*sizeof(char));
  if (status==NULL) 
  {
	  WriteErrorToLog(LogName,HITAS_NOTENOUGHMEMORY);
	  exit(HITAS_NOTENOUGHMEMORY);
  }	  
  for (i=0;i<size;i++)
	  status[i]='s';  // Zonder voorkennis is alles publicabel
}

void JJTable::PrintData(FILE& Uitfile)
{
  int i;
  int a = (int)(log(MAXWEIGHT)/log(10.0) + 1);
  for (i=0;i<size;i++)
  {
    for (size_t j=1;j<=ijk.size();j++)
      //fprintf(&Uitfile,"%3d ",ijk[j][i]);
        fprintf(&Uitfile,"%3d ",ijk[j-1][i]);
    fprintf(&Uitfile, "%15.5lf %*d %c %15.5lf %15.5lf %15.5lf %15.5lf %15.5lf\n",data[i],a,weight[i],status[i],lb[i],ub[i],lpl[i],upl[i],0.0);
  }
  fprintf(&Uitfile,"\n");
}

//void JJTable::PrintWeights(FILE& Uitfile, Vector< Vector<int> > SGTab)
void JJTable::PrintWeights(FILE& Uitfile, std::vector< std::vector<int> > SGTab)
{
  int i,j;
  j=1;
  for (i=1;i<basedim;i++)    // Aantal categorieen, exlc. marginaal
  {
	//if (SGTab[i][1]==0)
      if (SGTab[i-1][0]==0)
	  fprintf(&Uitfile,"0 ");
	else
	  //fprintf(&Uitfile,"%d ",N[j++]-1);
        {fprintf(&Uitfile,"%d ",N[j-1]-1);j++;}
  }
  //if (SGTab[i][1]==0)
  if (SGTab[i-1][0]==0)
	fprintf(&Uitfile,"0\n");
  else
    //fprintf(&Uitfile,"%d\n",N[j]-1);
      fprintf(&Uitfile,"%d\n",N[j-1]-1);

  for (i=0;i<size;i++)
  {
    for (j=1;j<=basedim;j++)
      //fprintf(&Uitfile,"%5d ",baseijk[i][j]);
        fprintf(&Uitfile,"%5d ",baseijk[i][j-1]);
    fprintf(&Uitfile, "%5d\n",weight[i]);
  }
  fprintf(&Uitfile,"\n");
}

void JJTable::PrintStats(FILE& Uitfile)
{
  int i;
  for (i=0;i<size;i++)
    fprintf(&Uitfile, "%c",status[i]);
}

void JJTable::PrintBasisStats(FILE& Uitfile, Table& BTab)
{
  int i;
  for (i=0;i<size;i++)
    if (BTab[baseijk[i]] != NULL)
      fprintf(&Uitfile,"%c",BTab[baseijk[i]]->status);
	else
      fprintf(&Uitfile,"z");
}

void JJTable::PrintAdditionalInfo(FILE& Uitfile)
{
	int j;
        unsigned int i;

        fprintf(&Uitfile,"Number of additional cells: %zu\n",AdditionalCells.size());
	for (i=0;i<AdditionalCells.size();i++)
	{
                fprintf(&Uitfile,"%d %lf %d %c %lf %lf %lf %lf %lf\n",AdditionalCells[i].position,AdditionalCells[i].value,AdditionalCells[i].weight,AdditionalCells[i].status,
						AdditionalCells[i].lb,AdditionalCells[i].ub,AdditionalCells[i].lpl,AdditionalCells[i].upl,AdditionalCells[i].spl);
	}
	fprintf(&Uitfile,"Number of additional constraints: %zu\n",AdditionalConstraints.size());
	for (i=0;i<AdditionalConstraints.size();i++)
	{
		fprintf(&Uitfile,"%lf %d: ",AdditionalConstraints[i].rhs,AdditionalConstraints[i].ncard);
		fprintf(&Uitfile,"%d (%d) ",AdditionalConstraints[i].Cells[0],-1);
		for (j=1;j<AdditionalConstraints[i].ncard;j++)
			fprintf(&Uitfile,"%d (%d) ",AdditionalConstraints[i].Cells[j],1);
		fprintf(&Uitfile,"\n");
	}
}


void JJTable::SaveBasisStatsBefore(std::string& TableList, Table& BTab)
{
  int i;
  TableList.clear();
  for (i=0;i<size;i++)
    if (BTab[baseijk[i]] != NULL)
      TableList += BTab[baseijk[i]]->status;
    else
      TableList += "z";
}

void JJTable::SaveBasisStatsAfter(std::string& TableList, Table& BTab)
{
  int i;
  for (i=0;i<size;i++)
    if (BTab[baseijk[i]] != NULL)
      TableList += BTab[baseijk[i]]->status;
    else
      TableList += "z";
}

void JJTable::Free()
{
  int i;
  unsigned int usi;
  
  AdditionalCells.erase(AdditionalCells.begin(),AdditionalCells.end());

  for (usi=0;usi<AdditionalConstraints.size();usi++)
  {
	free(AdditionalConstraints[usi].Cells);
	AdditionalConstraints[usi].Cells = NULL;
  }
  
  AdditionalConstraints.erase(AdditionalConstraints.begin(),AdditionalConstraints.end());
  
  //N.Free();
  N.clear();
  
  // std::vector<int*> ijk
  for (size_t i=1;i<=ijk.size();i++)
  {
        //free(ijk[i]);
        free(ijk[i-1]);
	//ijk[i] = NULL;
        ijk[i-1] = NULL;
  }
  //ijk.Free();
  ijk.clear();
  
  //Vector<int>* baseijk
  for (i=0;i<size;i++)
    //baseijk[i].Free();
      baseijk[i].clear();
  
  //free(baseijk);
  delete[] baseijk;
  baseijk=NULL;

  free(weight);
  weight=NULL;
  free(count);
  count=NULL;
  free(data);
  data=NULL;
  free(lpl);
  lpl=NULL;
  free(upl);
  upl=NULL;
  free(lb);
  lb=NULL;
  free(ub);
  ub=NULL;
  free(status);
  status=NULL;
  free(costs);
  costs=NULL;

  NumberOfUnsafeCells = 0;
  NumberOfSafeCells = 0;
  basedim = 0;
  size = 0;
}
