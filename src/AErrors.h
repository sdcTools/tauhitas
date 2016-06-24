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

#ifndef __AERRORS_H
#define __AERRORS_H

#include "resource.h"

/* Variables for JJ, used to be defined in cspdef.h */
extern double MAX_TIME;
extern double ZERO;
extern double ZERO_1;
extern double ZERO_2;
extern double INF;
extern long MAX_COLS_LP;
extern long MAX_ROWS_LP;
extern long MAX_CUTS_POOL;
extern long MAX_CUTS_ITER;
extern double MIN_VIOLA;
extern double MAX_SLACK;
extern double FEAS_TOL;
extern double OPT_TOL;

extern std::string LogName;

extern const char *OUTDIR;

extern FILE *LogFile;

extern StringTable ErrorStrings;
#endif
