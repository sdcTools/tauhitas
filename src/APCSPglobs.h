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

extern int            S_max;      /*      max number of suppressed cells */

extern int            Rncells;      /* number of cells after reduction      */
extern int            Rnsums;       /* number of sums after reduction       */
extern double         *Rrhs;        /* RHS for sums after reduction         */
extern int            ncells;       /* number of cells in the table         */
extern int            nrows;        /* number of current constraints        */
extern int            mac;          /* number of current columns in the LP  */
extern int            mar;          /* number of current rows in the LP     */
extern int            nsensitive;   /* number of sensitive cells            */
extern int            nprot_level;  /* number of protection levels          */
extern int            nprot_levelZ; /* number of Z+ & Z- protection levels  */

/* Globals data statistics */
extern int            iterations;   /* number of iterations                 */
extern int            branchs;      /* number of studied branch-tree nodes  */
extern int            ntail;        /* number of iterations for tailing-off */
extern int            nsupport;     /* number of variables in 'support' (LB)*/
extern int            nbetter;      /* number of variables in 'better'  (UB)*/
extern double         lowerb;       /* current lower bound                  */
extern double         upperb;       /* current upper bound (heuristic)      */
extern float          t0;           /* inicial clock-ticks CPU time         */
extern float          theur;        /* CPU time to for the heuristic        */
extern float          topti;        /* CPU time to find the current lowerb  */
extern char           ubtype;       /* UB type: find by Lp / find by Heuris */
extern int            cpool;        /* total number of POOL constraints     */
extern int            cbend;        /* total number of BENDERS constraints  */
extern int            ccapa;        /* total number of CAPACITY constraints */
extern int            cbrid;        /* total number of CAPACITY constraints */
extern int            ccove;        /* total number of COVER constraints    */
extern int            ccove2;       /* total number of COVER2 constraints   */
extern int            cgomo;        /* total number of GOMORY constraints   */
extern int            cmatc;        /* total number of COMB constraints     */
extern int            cpath;        /* total number of PATH constraints     */
extern int            cpath2;       /* total number of PATH2 constraints    */
extern int            cinitial;     /* total number of INITIAL constraints  */
extern int            bad_heuristic;/* flag =1 when no heuristic was applied */
