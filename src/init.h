
#ifndef INIT_H
#define INIT_H

#include "itypes.h"

/*
	 DIV : TICKRATE RANGE, Hz (F_CPU = 16MHz)
	   1 : 62500 - .......
	   8 :  7844 - 2000000
	  64 :   981 -  250000
	 256 :   246 -   62500
	1024 :    62     15625
*/

#define PRESCALE	64
#define TICKRATE	1000
#define TOP(tickrate,prescale) (F_CPU/(prescale)/(tickrate))

#define UBRRVAL(brate) (F_CPU/16/(brate) - 1)

#endif
