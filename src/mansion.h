/*
 * mansion.h
 *
 * Header file for the Mystery Mansion adventure.  This is not used extensively
 * at present.
 *
 * Copyright (c) 1999,2000 James Garnett, garnett@catbelly.com.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * This software is provided ``as is'' and any express or implied warranties, 
 * including, but not limited to, the implied warranties of merchantability 
 * and fitness for a particular purpose are disclaimed.  
 *
 * $Log: mansion.h,v $
 * Revision 1.1  2000/01/23 20:56:41  garnett
 * Initial revision
 *
 */
#include <stdio.h>
#include <string.h>


/*
 * General macros
 */
#define IFSD(I)		((I)-100*(int)((I)/100.00))
#define ITFD(I)		((I)/100-((I)/10000)*100)
#define ITD(I)		((I)/100-((I)/1000)*10)
#define IFD(I)		((I)/1000-((I)/10000)*10)
#define RN(X)		((X)*7.7 - (int)((X)*7.7))

/*
 * Prototypes
 */
void pak(void);
