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

#ifndef I_ProgressListener_h
#define I_ProgressListener_h

class IProgressListener
{
public:
	virtual ~IProgressListener() { }
	virtual void UpdateLB(int Perc) = 0;
        virtual void UpdateUB(int Perc) = 0;
        virtual void UpdateGroups(int Perc) = 0;
        virtual void UpdateTables(int Perc) = 0;
        virtual void UpdateDiscrepancy(double value) = 0;
        virtual void UpdateTime(int value) = 0;
        virtual void UpdateNSuppressed(int value) = 0;
};

#endif // I_ProgressListener_h
