
#ifndef SHELL_H
#define SHELL_H

#include "itypes.h"

/* state numbers */

#define S0_SHELL_INIT	S0_
#define S1_SHELL_WAIT	S1_
#define S2_SHELL_ERRR	S2_
#define S3_SHELL_RCFG	S3_
#define S4_SHELL_WCFG	S4_
#define S5_SHELL_REPL	S5_

/* reflex numbers */

//#define R0_DRE		R0_
//#define R0_RXC		R0_

struct shell_data {
	u8 *rxb;
	u8 txb[32];
};

#endif
