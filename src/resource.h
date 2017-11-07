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

#ifndef __RECSOURCE_H
#define __RECSOURCE_H
#include <map>
#include <string>

#define VERSIONNUMBER "4.2.3"

// ID's for JJ variables
#define JJZERO          101
#define JJZERO1         102
#define JJZERO2         103
#define JJINF           104
#define JJMAXCOLSLP     105
#define JJMAXROWSLP     106
#define JJMAXCUTSPOOL   107
#define JJMAXCUTSITER   108
#define JJMINVIOLA      109
#define JJMAXSLACK      110
#define JJFEASTOL       111
#define JJOPTTOL        112
#define JJMAXTIME       113

// Error constants
#define HITAS_FILENOTFOUND              5000
#define HITAS_FILEWRITE                 5001
#define HITAS_NOTENOUGHMEMORY           5002
#define HITAS_WRONGHITASCALL            5003
#define HITAS_RECREATECOLUMN            5004
#define HITAS_SHOULDNOTHAPPEN           5005
#define HITAS_NOVARIABLES               5006
#define HITAS_TABDIMSDONTMATCH          5007
#define HITAS_JJNOTLOADED               5008
#define LB_EQ_UB_EQ_INF                 5009
#define HITAS_MAXINT_ERROR              5010
#define HITAS_SKIP_ERROR                5011
#define HITAS_UNKNOWNSOLVER             5012
#define JJ_FILENOTFOUND                 6000
#define JJ_FILEWRITE                    6001
#define JJ_NOTENOUGHMEMORY              6002
#define JJ_NOFRACSOL                    6003
#define JJ_NOFEASSOL                    6004
#define JJ_NONETOPTSOL                  6005
#define JJ_NODUALOPTSOL                 6006
#define JJ_NOOPTIMIZESOL                6007
#define JJ_NOSOLUTION                   6008
#define JJ_UNKNOWNBRANCHTYPE            6009
#define JJ_UNKNOWNCONSTRAINTTYPE        6010
#define JJ_NOBRANCHCONSTRAINT           6011
#define JJ_NOBRIDGECONSTRAINT           6012
#define JJ_NOCAPACONSTRAINT             6013
#define JJ_NOCOVERCONSTRAINT            6014
#define JJ_TOOMANYGLOBALROWS            6015
#define JJ_TOOMANYLPVARS                6016
#define JJ_DEFAULT                      7000
#define JJ_CHECKLP                      7001
#define JJ_LPSOLVER                     7002
#define JJ_HEURISTIC                    7003
#define JJ_NOXPDONGLE                   8000
#define JJ_NOCPLEXLICENCE               8001

typedef std::map<int,std::string> StringTable; 

#endif
