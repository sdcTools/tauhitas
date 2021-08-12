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

#ifndef __ACONST_H__
#define __ACONST_H__

#include "AMyvector.h"
#include <vector>
extern int MINCOUNT;     // Minimale aantal bijdragers
extern double LOWERMARG; // Percentage voor ondergrens marge secundaire cel
extern double UPPERMARG; // Percentage voor bovengrens marge secundaire cel
// Voor JJ-functies:
extern double MINTABVAL; // Absolute ondergrens voor tabelwaarden
extern double MAXTABVAL; // Absolute bovengrens voor tabelwaarden
//extern CString OUTDIR;     // Uitvoer directory
extern const char *OUTDIR;
// Algemeen:
extern short DISTANCE;   // 1 als afstandsgewicht gebruikt moet worden
//extern int D1[5];		 // de 5 gewichten in richting 1
//extern int D2[5];		 // de 5 gewichten in richting 2
//extern int D3[5];		 // de 5 gewichten in richting 3
//extern Vector<int[5]> D;
extern std::vector< std::vector<int> > D;
extern double MAXDISTANCE;
extern double JJTime;    // om de tijd binnen de JJ-routines te meten
extern double MAXWEIGHT; // Om kosten naar te schalen
extern int APRIORI;
extern double APRIORILB;
extern double APRIORIUB;
extern bool DOSINGLETONS;
extern bool DOSINGLEWITHSINGLE;
extern bool DOSINGLEWITHMORE;
extern bool DOCOUNTBOUNDS;
extern bool DOLIFTUP;
extern int LINKED;		// 1 als linked tables gebruikt moet worden
extern int DECIMALS;

#define _ReadStat        15   // Integer die aangeeft of een tabel is ingelezen
#endif
