/*
 * error.h
 *
 *  Created on: Jul 16, 2012
 *      Author: rghunter
 */

#ifndef ERROR_H_
#define ERROR_H_

#define STAT_RED 0x00000004
#define	STAT_GREEN 0x00000800
#define STAT_USB 0x80000000

#define ON 1
#define OFF 0


void error_red_flash(void);
void error_green_flash(void);

#endif /* ERROR_H_ */
