/*
 * mansion.h
 *
 * Header file for the Mystery Mansion adventure.  This is not used extensively
 * at present.
 *
 * This software is provided ``as is'' and any express or implied warranties, 
 * including, but not limited to, the implied warranties of merchantability 
 * and fitness for a particular purpose are disclaimed.  
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
