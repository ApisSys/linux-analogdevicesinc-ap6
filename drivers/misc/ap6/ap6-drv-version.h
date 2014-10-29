/**
 * @file ap6-drv-version.h
 *
 *  @date 	1 mars 2013
 *  @author mdelacour
 */

#ifndef AP6_DRV_VERSION_H_
#define AP6_DRV_VERSION_H_


#define XSTR(a) STR(a)
#define STR(a) #a

/** Driver Version Major		*/
#define VERSION_MAJOR	0

/** Driver Version Minor		*/
#define VERSION_MINOR	1

/** Driver Version String		*/
#define VERSION_STRING XSTR(VERSION_MAJOR.VERSION_MINOR)


#endif /* AP6_DRV_VERSION_H_ */
