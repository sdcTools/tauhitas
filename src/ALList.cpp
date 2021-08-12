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

#include "ALList.h"
#include "AMyIO.h"
#include "resource.h"
#include <iostream>

bool Compare(const Level& L1, const Level& L2) {return L1.Gnr < L2.Gnr;};

void LList::Make(int L)
{
        Level TmpLevel;
	data.assign(L+1,TmpLevel); // Array of Levels
	dim = L;
}

void LList::Free()
{
        data.erase(data.begin(),data.end());
}

void LList::Sort()
{
        std::sort(data.begin(), data.end(), Compare);
}

void LList::Fill()
{
	for (int i=0; i<=dim; i++)
	{
                //data[i].Gname.assign(1,1);
		//data[i].Gname[0] = i;
                data[i].Gname.assign(1,i);
		data[i].Gnr = i;
	}
}

void LList::Oplus(LList& L1)
{
	int i,j,k,pos;
	int AddDim = L1.size();
	int OldDim = dim;
	int OldClassSize=0;
	if (OldDim!=0) OldClassSize = data[0].Gname.size();
        
	LevelArray OldBody;
        Level TmpLevel;
        
	if (OldDim != 0)
	{
                OldBody.assign(OldDim+1,TmpLevel);
		for (i=0;i<=OldDim;i++)
		{
                        OldBody[i].Gname.assign(OldClassSize,1);
			for (j=0;j<OldClassSize;j++)
				OldBody[i].Gname[j] = data[i].Gname[j];
			OldBody[i].Gnr = data[i].Gnr;
		}
	}

	dim = (AddDim+1)*(OldDim+1) - 1;
        
        data.assign(dim+1,TmpLevel);
        
	pos=0;
	for (i=0;i<=OldDim;i++)
	{
		for (j=0;j<=AddDim;j++)
		{
                        data[pos].Gname.assign(OldClassSize+1,1);
			if (OldDim!=0) for (k=0;k<OldClassSize;k++) data[pos].Gname[k] = OldBody[i].Gname[k];
                        
			data[pos].Gname[OldClassSize] = L1[j]->Gname[0];
			if (OldDim!=0)
				data[pos].Gnr = OldBody[i].Gnr + L1[j]->Gnr;
			else
				data[pos].Gnr = L1[j]->Gnr;
			pos++;
		}
	}
        
	OldBody.erase(OldBody.begin(),OldBody.end());
}

void LList::Printf()
{
	FILE* LogFile;
	int i;
        unsigned int j;
        std::string buffer;
        buffer.append(OUTDIR);
        buffer.append("GroupInfo.txt");
	LogFile = OpenFile(buffer.c_str(),"w");
	for(i=0;i<=dim;i++)
	{
		fprintf(LogFile,"%2d: ",data[i].Gnr);
                for (j=0;j<data[i].Gname.size();j++)
			fprintf(LogFile,"%2d",data[i].Gname[j]);
		fprintf(LogFile,"\n");
	}
	fclose(LogFile);
}

/*
// LevelArray functions
int LevelArray::Compare(const Level *plevel1, const Level *plevel2)
{
	if (plevel1->Gnr < plevel2->Gnr)
		return -1;
	else
		if (plevel1->Gnr > plevel2->Gnr)
			return +1;
		else
			return 0;
}


void LevelArray::Sort(LEVELCOMPAREFN pfnCompare)
{
	//Level* plevels = GetData();
        Level* plevels = ();
	//qsort(plevels,GetSize(),sizeof(Level),(GENERICCOMPAREFN) pfnCompare);
        qsort(plevels,GetSize(),sizeof(Level),(GENERICCOMPAREFN) pfnCompare);
}
 */
