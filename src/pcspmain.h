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

int   PCSPoptimize(char*,int,int,int,int,int);

int   PCSPloadprob(int,int,double*,int,double*,int*,int*,int*,char*,double*,double*,double*,double*,double*,char**,int*,int*,signed char*);
int   PCSPtestprob(int,double*,int,double*,int*,int*,int*,char*,double*,double*,double*,double*,double*,/*char**,*/int*,int*,signed char*);
int   PCSPfreeprob(void);
int   PCSPsolution(double*, double*,char**,double**,double**,double**);
int   PCSPrelbounds(int,int*,double*,double*);
int   PCSPabsbounds(int,int*,double*,double*);
int   PCSPpartialbounds();
int   PCSPwrite(char*);
int	  PCSP_audit_solution(double*,double *,char**);
