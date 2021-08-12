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

#include "AMyIO.h"

// Used to replace _tcsspnp(str1,str2) that is only defined for Microsoft
#define _strspnp(_str1, _str2) ((*((_str1)+strspn(_str1,_str2))) ? ((_str1)+strspn(_str1,_str2)) : NULL)

std::string to_string(int _Val)
{   // convert int to string
    char _Buf[64];
    snprintf(_Buf, 64, "%d", _Val);
    return (std::string(_Buf));
}

void WriteErrorToLog(std::string LogName, int ErrorCode) // Finds ErrorString belonging to ErrorCode and prints it in LogFile
{
        std::string buffer;

        buffer.append("Exited with code ");
        buffer.append(to_string(ErrorCode));
        buffer.append(": ");
        buffer.append(ErrorStrings[ErrorCode].c_str());
        buffer.append("\n");
	LogPrintf(LogName,buffer);
}

void LogPrintf(std::string LogName, std::string LogMessage) // Appends LogMessage to file LogName
{
	LogFile = OpenFile(LogName.c_str(),"a");
	fprintf(LogFile,LogMessage.c_str());
	fclose(LogFile);
}

void LogPrintf(std::string LogName, const char* LogMessage) // Appends LogMessage to file LogName
{
	LogFile = OpenFile(LogName.c_str(),"a");
	fprintf(LogFile,LogMessage);
	fclose(LogFile);
}

std::string PrepFile(const char* Name)
{
    std::string ReturnVal;
    ReturnVal.append(OUTDIR);
    ReturnVal.append(Name);
    return ReturnVal;
}

FILE* OpenFile(const char* Name, const char* stat)
{
  FILE* TmpFile;
  TmpFile = fopen(Name,stat);
  if (TmpFile == NULL)
  {
	if (strcmp(stat,"w")==0)
	{
		LogPrintf(LogName,"Not able to create file ");
                LogPrintf(LogName,Name);
                LogPrintf(LogName," \n");
		WriteErrorToLog(LogName,HITAS_FILEWRITE);
		throw(HITAS_FILEWRITE);
	}
	if (strcmp(stat,"r")==0 || strcmp(stat,"a")==0)
	{
                LogPrintf(LogName,"File ");
                LogPrintf(LogName,Name);
                LogPrintf(LogName," not found\n");
		WriteErrorToLog(LogName,HITAS_FILENOTFOUND);
		throw(HITAS_FILENOTFOUND);
	}
  }	
  return(TmpFile);
};

void PrintConstants(const char* WegFileName, int M)
{
  int i;
  int a = (int) (log(MAXTABVAL)/log(10.0)+4);
  FILE *weg;
  
  weg = OpenFile(WegFileName,"a");
  
#ifdef SECBOUNDS
  fprintf(weg,"Secundaries using adjusted safety-ranges\n");
#endif
 
  fprintf(weg,"MINCOUNT : %*d (%d is safe)\n",a,MINCOUNT,MINCOUNT+1);
  fprintf(weg,"LOWERMARG: %*.*lf\n",a,2,LOWERMARG);
  fprintf(weg,"UPPERMARG: %*.*lf\n",a,2,UPPERMARG);
  if (DOLIFTUP) fprintf(weg,"DOLIFTUP : %*s\n",a,"true");
  else fprintf(weg,"DOLIFTUP : %*s\n",a,"false");
  fprintf(weg,"MINTABVAL: %*.*lf\n",a,2,MINTABVAL);
  fprintf(weg,"MAXTABVAL: %*.*lf\n",a,2,MAXTABVAL);
  fprintf(weg,"DECIMALS : %*d\n",a,DECIMALS);
  fprintf(weg,"OUTDIR   : %*s\n",a,OUTDIR);
  fprintf(weg,"MAXWEIGHT: %*d\n",a,(int) MAXWEIGHT);
  fprintf(weg,"APRIORI  : %*d\n",a,APRIORI);
  if (APRIORI==1)
  {
	  fprintf(weg,"APRIORILB: %*.*lf\n",a,2,APRIORILB);
	  fprintf(weg,"APRIORIUB: %*.*lf\n",a,2,APRIORIUB);
  }
  fprintf(weg,"DISTANCE : %*d\n",a,DISTANCE);
  if (DISTANCE!=0)
  {
	for (i=1;i<=M;i++)
            //fprintf(weg,"  D%d: %d %d %d %d %d\n",i,D[i][0],D[i][1],D[i][2],D[i][3],D[i][4]);
            fprintf(weg,"  D%d: %d %d %d %d %d\n",i,D[i-1][0],D[i-1][1],D[i-1][2],D[i-1][3],D[i-1][4]);
  }
  fprintf(weg,"LINKED   : %*d\n",a,LINKED);
  fclose(weg);
}

// Inlezen van constantes die in const.dat zijn gedefinieerd
void ReadConstants(const char* ConstFileName)
{
  int MAXLINE = 256; // Maximale lengte van een regel in const.dat in bytes
  int MAXNUMCONST = 16; // Maximale aantal mogelijke constantes in const.dat
  char line[256];
  char *cat, *cat2;
  FILE* ConstFile;
  int i;

  char constants[][20] = {"MINCOUNT","LOWERMARG","UPPERMARG",
	                  "MINTABVAL","MAXTABVAL","DISTANCE",
		          "D1","D2","D3","D4",
		          "APRIORI","APRIORILB","APRIORIUB","MAXWEIGHT","LINKED","DECIMALS"};

  ConstFile = OpenFile(ConstFileName,"r");
  
  // Eerst defaults zetten
  MINCOUNT=3; LOWERMARG=0.9; UPPERMARG=1.1; MINTABVAL=0;
  DISTANCE=0; MAXTABVAL=20000000; MAXWEIGHT=20000; 
  DECIMALS=0;
  LINKED=0;  // Default: no linked tables
  DOLIFTUP=false;

  //D.Make(4);
  D.resize(4);
  for (int i=1; i<=4; i++){
      D[i-1].resize(5,1);
  }      
  /*D[1][0]=D[1][1]=D[1][2]=D[1][3]=D[1][4]=1;
  D[2][0]=D[2][1]=D[2][2]=D[2][3]=D[2][4]=1;
  D[3][0]=D[3][1]=D[3][2]=D[3][3]=D[3][4]=1;
  D[4][0]=D[4][1]=D[4][2]=D[4][3]=D[4][4]=1;
  */
  
  APRIORI = 0; // Default: no APRIORI bounds
  APRIORILB = 0;
  APRIORIUB = 2000000000;

  LogPrintf(LogName,"-----------------\nReading constants\n-----------------\n");
  // Vervolgens vervangen door waarden uit file const.dat als aanwezig
  while (fgets(line,MAXLINE,ConstFile))
  {
    if ((*line==';')||(*line=='[')) continue; // Regel bevat commentaar of groepsnaam
    cat = strtok(line,";\t\n");
    if (!cat) continue;		            // Regel is leeg
    cat = strtok(line," =;\t\n");       // cat is nu "constante-naam"
    for (i=0; i<MAXNUMCONST; i++){
 	if (strcmp(cat, constants[i])==0) // Gevonden!
 		break;
    }
    if (i>=MAXNUMCONST) // Niet gevonden!
    {
	LogPrintf(LogName,"Ignoring unknown constant ");
        LogPrintf(LogName,cat);
        LogPrintf(LogName," in constants-file\n");
    }
    cat = strtok(0,"=;\t\n");          // Rest van de regel tot een spatie
    switch (i) {
     case 0:	{MINCOUNT = atoi(cat)-1;
		 if (MINCOUNT<0) 
		 {
                     LogPrintf(LogName,"MINCOUNT should be at least 1\n");
                     MINCOUNT = 0;
		 }
		 break;
                }; // MINCOUNT + 1 is safe
     case 1:	{LOWERMARG = atof(cat);break;};
     case 2:	{UPPERMARG = atof(cat);break;};
     case 3:	{MINTABVAL = atof(cat);break;};
     case 4:	{MAXTABVAL = atof(cat);break;};
     case 5:	{DISTANCE  = atoi(cat);break;};
     case 6:	{
		 if (DISTANCE != 0)
		 {
			cat2 = _strspnp(cat," =;\t\n");
			cat2 = strtok(cat2," =;\t\n");
			//D[1][0] = atoi(cat2); 
                        D[0][0] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[1][1] = atoi(cat2); 
                        D[0][1] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[1][2] = atoi(cat2); 
                        D[0][2] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[1][3] = atoi(cat2); 
                        D[0][3] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[1][4] = atoi(cat2); break;
                        D[0][4] = atoi(cat2); break;
		 }
		 else
                        LogPrintf(LogName,"D1 specified but not used because DISTANCE == 0\n");
		};
     case 7:    {
		 if (DISTANCE != 0)
		 {
			cat2 = _strspnp(cat," =;\t\n");
			cat2 = strtok(cat2," =;\t\n");
			//D[2][0] = atoi(cat2); 
                        D[1][0] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[2][1] = atoi(cat2); 
                        D[1][1] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[2][2] = atoi(cat2); 
                        D[1][2] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[2][3] = atoi(cat2); 
                        D[1][3] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[2][4] = atoi(cat2); break;
                        D[1][4] = atoi(cat2); break;
		 }
                 else
                        LogPrintf(LogName,"D2 specified but not used because DISTANCE == 0\n");
		};
     case 8:	{
 		 if (DISTANCE != 0)
		 {
			cat2 = _strspnp(cat," =;\t\n");
			cat2 = strtok(cat2," =;\t\n");
			//D[3][0] = atoi(cat2); 
                        D[2][0] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[3][1] = atoi(cat2); 
                        D[2][1] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[3][2] = atoi(cat2); 
                        D[2][2] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[3][3] = atoi(cat2); 
                        D[2][3] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[3][4] = atoi(cat2); break;
                        D[2][4] = atoi(cat2); break;
		 }
		 else
                        LogPrintf(LogName,"D3 specified but not used because DISTANCE == 0\n");
		};
     case 9:	{
		 if (DISTANCE != 0)
		 {
		 	cat2 = _strspnp(cat," =;\t\n");
			cat2 = strtok(cat2," =;\t\n");
			//D[4][0] = atoi(cat2); 
                        D[3][0] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[4][1] = atoi(cat2); 
                        D[3][1] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[4][2] = atoi(cat2); 
                        D[3][2] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[4][3] = atoi(cat2); 
                        D[3][3] = atoi(cat2); 
			cat2=strtok(NULL," =;\t\n");
			//D[4][4] = atoi(cat2); break;
                        D[3][4] = atoi(cat2); break;
		 }
		 else 
                        LogPrintf(LogName,"D4 specified but not used because DISTANCE == 0\n");
		};
     case 10:	APRIORI = atoi(cat); break;
     case 11:	APRIORILB = atof(cat); break;
     case 12:	APRIORIUB = atof(cat); break;
     case 13:	MAXWEIGHT = atof(cat); break;
     case 14:	LINKED = atoi(cat); break;
     case 15:	DECIMALS = atoi(cat);break;
     default:	break;
    }
  }
	if (DISTANCE)
	{
            //MAXDISTANCE = __max(D[1][4],D[2][4]);
            MAXDISTANCE = __max(D[0][4],D[1][4]);
            //MAXDISTANCE = __max(MAXDISTANCE,D[3][4]);
            MAXDISTANCE = __max(MAXDISTANCE,D[2][4]);
            //MAXDISTANCE = __max(MAXDISTANCE,D[4][4]);
            MAXDISTANCE = __max(MAXDISTANCE,D[3][4]);
	}
	else 
            MAXDISTANCE = 0;

	if (MINTABVAL < 0) DOLIFTUP = true; // 03/04/2009 PPdW If some negative cell-value, need to do something when suppressing
  fclose(ConstFile);
};

void AReadHistory(Table BTab, int M, const char* FileName, const char* States)
{
 char buffer[30];
 int i;
 FILE *History = fopen(FileName,"r");

 if (History != NULL) // Als history in te lezen, dan doen
 {
   LogPrintf(LogName,"\nReading and processing file ");
   LogPrintf(LogName,FileName);
   LogPrintf(LogName,"\n");
   //Vector<int> ijk;							  // Voor inlezen coordinaten
   std::vector<int> ijk;							  // Voor inlezen coordinaten
   //ijk.Make(M);
   ijk.resize(M);
   while (!feof(History))
   {
     for (i=1;i<=M;i++)
       //fscanf(History,"%d ",&ijk[i]);
         fscanf(History,"%d ",&ijk[i-1]);
     fscanf(History,"%c\n",&BTab[ijk]->status);
     // Controleer ingelezen status
     if (strchr(States,BTab[ijk]->status) == NULL)
     {
       snprintf(buffer,28,"Unknown state '%c' of cell (",BTab[ijk]->status);
       LogPrintf(LogName,buffer);
       for (i=1;i<M;i++)
       {
            //LogPrintf(LogName,to_string(ijk[i]));
            LogPrintf(LogName,to_string(ijk[i-1]));
            LogPrintf(LogName,", ");
       }
       //LogPrintf(LogName,to_string(ijk[M]));
       LogPrintf(LogName,to_string(ijk[M-1]));
       LogPrintf(LogName,") read in file ");
       LogPrintf(LogName,FileName);
       LogPrintf(LogName,", continuing anyway\n");
     }
   }
   fclose(History);
   //ijk.Free();
 }
 else 
 {
     LogPrintf(LogName,"\nNo file ");
     LogPrintf(LogName,FileName);
     LogPrintf(LogName," found => assuming no user imposed status.\n");
 }
};

// Inlezen van BackTrack informatie
void ReadBTInfo(Table& BTab, int M, const char* FileName, const char* States)
{
 int i;
 double lb,ub;
 char buffer[30];
 FILE *BTInfo = fopen(FileName,"r");
 if (BTInfo != NULL) // Als BackTrackInfo in te lezen, dan doen
 {
   if (PPDEBUG)
   {
        LogPrintf(LogName,"\nReading and processing file ");
        LogPrintf(LogName,FileName);
        LogPrintf(LogName,"\n");
   }
   //Vector<int> ijk;							  // Voor inlezen coordinaten
   std::vector<int> ijk;							  // Voor inlezen coordinaten
   //ijk.Make(M);
   ijk.resize(M);
   while (!feof(BTInfo))
   {
     for (i=1;i<=M;i++)
       //fscanf(BTInfo,"%d ",&ijk[i]);
         fscanf(BTInfo,"%d ",&ijk[i-1]);
     fscanf(BTInfo,"%c",&BTab[ijk]->status);
     
	 // Controleer ingelezen status
     if (strchr(States,BTab[ijk]->status) == NULL)
     {
	   snprintf(buffer,28,"Unknown state '%c' of cell (",BTab[ijk]->status);
	   LogPrintf(LogName,buffer);
           for (i=1;i<M;i++)
	   {
                //LogPrintf(LogName,to_string(ijk[i]));
                LogPrintf(LogName,to_string(ijk[i-1]));
                LogPrintf(LogName,", ");
	   }
           //LogPrintf(LogName,to_string(ijk[M]));
           LogPrintf(LogName,to_string(ijk[M-1]));
           LogPrintf(LogName,") read in file ");
           LogPrintf(LogName,FileName);
           LogPrintf(LogName,", continuing anyway\n");
     }
	 
	 // Voorlopig alleen Pbounds lpl en upl inlezen voor status u en b
	 if (strchr("ub",BTab[ijk]->status))
	 {
		fscanf(BTInfo," %lf %lf",&lb, &ub);
		//if (BTab[ijk]->Pbounds.GetSize()==0) 
                if (BTab[ijk]->Pbounds.size()==0) 
		{
			BTab[ijk]->Pbounds.push_back(lb);
			BTab[ijk]->Pbounds.push_back(ub);
		}
		else
		{
			BTab[ijk]->Pbounds[0]=lb;
			BTab[ijk]->Pbounds[1]=ub;
		}
	 }

     fscanf(BTInfo,"\n"); 
   }
   fclose(BTInfo);
   //ijk.Free();
 }
 else 
 {
	 if (PPDEBUG)
	 {
                LogPrintf(LogName,"\nNo file ");
                LogPrintf(LogName,FileName);
                LogPrintf(LogName," found => assuming no BackTrack info.\n");
	 }
 }
};

void PrintSubG(FILE &FName, Level* SGi, std::vector< std::vector<int> > SGTj)
{
  size_t i, k, m;
  //if (&FName != NULL)   // References/addresses cannot be NULL, so is superfluous check
  //{
        for (i=0;i<SGi->Gname.size();i++)
	  fprintf(&FName,"%d",SGi->Gname[i]);
	fprintf(&FName,"\t(");
	for (k=1;k<=SGTj.size();k++)
	{
		fprintf(&FName,"(");
		//for (m=1;m<=SGTj[k].size();m++)
                for (m=1;m<=SGTj[k-1].size();m++)
			//fprintf(&FName,"%d ",SGTj[k][m]);
                    fprintf(&FName,"%d ",SGTj[k-1][m-1]);
		fprintf(&FName,") ");
	}
	fprintf(&FName,")\n");
  //}
  /*else
  {
        for (i=0;i<SGi->Gname.size();i++)
	{
	  LogPrintf(LogName,to_string(SGi->Gname[i]));
	}
	LogPrintf(LogName,"\t(");
	for (k=1;k<=SGTj.size();k++)
	{
		LogPrintf(LogName,"(");
		for (m=1;m<=SGTj[k].size();m++)
		{
			LogPrintf(LogName,to_string(SGTj[k][m]));
                        LogPrintf(LogName," ");
		}
		LogPrintf(LogName,") ");
	}
	LogPrintf(LogName,")\n");
  }*/
};

///////////////////////////////////////////////////////////////////////////
// BackTracken omdat in marginaal is onderdrukt
// dus info over de marginalen opnemen in BackTrackFile
// Alleen info over marginalen met status 'm' wegschrijven:
// 'm' is er nieuw bijgekomen en moet als 'b' worden weggeschreven.
// De "oude" 'm' en 'b' cellen worden automatisch meegenomen als er weer
// gebacktracked wordt: 'm' door resultaten van vorige keer in te lezen en
// 'b' door backtrackinfo van vorige keer in te lezen.
void AddHistory(JJTable& BTtab, Table& BTab)
{ 
  FILE* Tmp;

  Tmp=OpenFile(PrepFile("BT.dat").c_str(),"a");
  
  int marg,i,m;

  if (BTtab.Dim() == 1)  // E�n dimensionaal, dus marginaal is 1 getal
  {
     if ((BTab[BTtab.baseijk[0]]!=NULL) && (BTab[BTtab.baseijk[0]]->status=='b')) // Was: op 'm' testen
     {
       for (i=1;i<=BTtab.BDim();i++)
         //fprintf(Tmp,"%d ",BTtab.baseijk[0][i]);
         fprintf(Tmp,"%d ",BTtab.baseijk[0][i-1]);
       fprintf(Tmp,"b %lf %lf\n",BTab[BTtab.baseijk[0]]->Pbounds[0],BTab[BTtab.baseijk[0]]->Pbounds[1]);
     }
  }	 
  else
  {
        for (m=0;m<BTtab.Size();m++)
	{
                marg = 1;
                for (i=1;(i<=BTtab.Dim()) && (marg!=0);i++)
                        //marg *= BTtab.ijk[i][m];      // als marg==0 dan marginaal
                        marg *= BTtab.ijk[i-1][m];      // als marg==0 dan marginaal

                if ((marg==0) && (BTab[BTtab.baseijk[m]]!=NULL) && (BTab[BTtab.baseijk[m]]->status=='b')) // Was: op 'm' testen
                {
                        for (i=1;i<=BTtab.BDim();i++)
                            //fprintf(Tmp,"%d ",BTtab.baseijk[m][i]);
                            fprintf(Tmp,"%d ",BTtab.baseijk[m][i-1]);
                        fprintf(Tmp,"b %lf %lf\n",BTab[BTtab.baseijk[m]]->Pbounds[0],BTab[BTtab.baseijk[m]]->Pbounds[1]);
                }
	}
  }
  fclose(Tmp);
};

//Codes bevat de codelijsten voor de variabelen, zoals gebruikt in de BasisTab
//BogusLists bevat de lijsten met externe codes van boguslevels en hun equivalenten
//Vars bevat de hierarchieen, MET de boguslevels erin
//States bevat de statussen die geprint moeten worden
/*void PrintBasisTabel(FILE& uitfile, Table& BasisTab, char* States, 
		     std::vector< std::vector<std::string> >& Vars, 
		     ExInCodeLijst& Codes, 
		     std::vector<StringMap>& BogusLists,
		     Vector<int>& Coords, std::vector<std::string>& ExCoords)
{
  int j;
  unsigned int i;
  int CoordSize;
  std::string AltCode;
  StringMap OneBogusList;

  if (Coords.size() < BasisTab.Dim()) Coords.Grow(1); // Nog niet bij laatste coordinaat
  
  CoordSize=Coords.size();
  ExCoords.resize(CoordSize);
  
  for (i=1;i<=Vars[CoordSize-1].size();i++)
  { //Als niet Bogus dan direct overnemen uit Vars
    OneBogusList = BogusLists[CoordSize-1];  
    if ( OneBogusList.empty() || (OneBogusList.find(Vars[CoordSize-1][i-1]) == OneBogusList.end()) )
	   AltCode = Vars[CoordSize-1][i-1];
    else
           AltCode = OneBogusList.find(Vars[CoordSize-1][i-1])->second;
	
    ExCoords[CoordSize-1] = Vars[CoordSize-1][i-1].c_str();  //Om bogusnamen weg te kunnen schrijven

    Coords[CoordSize] = Codes[CoordSize-1].find(AltCode)->second; //Zoek interne code

    if (CoordSize < BasisTab.Dim()) //Nog niet bij laatste coordinaat
       PrintBasisTabel(uitfile,BasisTab,States,Vars,Codes,BogusLists,Coords,ExCoords);
    else //Laatste coordinaat, dus kan wegschrijven
    {
       if ((BasisTab[Coords] != NULL) && (strchr(States,BasisTab[Coords]->status)!=NULL))
	   {
                for (j=1;j<=CoordSize;j++)  //Schrijf coordinaten
                    fprintf(&uitfile,"%9s ", ExCoords[j-1].c_str());
		 
		fprintf(&uitfile,"%c\n",BasisTab[Coords]->status); //Schrijf status
	   }
	}
  }
  Coords.Grow(-1);
};*/
