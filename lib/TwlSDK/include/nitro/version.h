#ifndef	TWLSDK_VERSION_H_
#define	TWLSDK_VERSION_H_
#define	SDK_VERSION_DATE		2020091208
#define	SDK_VERSION_TIME		1423
#define	SDK_VERSION_MAJOR		5
#define	SDK_VERSION_MINOR		3
#define	SDK_VERSION_RELSTEP		30076
#define	SDK_BUILDVER_CW_CC		4.0
#define	SDK_BUILDVER_CW_LD		2.0
#define	SDK_BUILDNUM_CW_CC		1024
#define	SDK_BUILDNUM_CW_LD		96
#define	SDK_BUILDVER_CC			4.0
#define	SDK_BUILDVER_LD			2.0
#define	SDK_BUILDNUM_CC			1024
#define	SDK_BUILDNUM_LD			96
#if 0	// for Makefile
TWL_VERSION_DATE_AND_TIME	=	20091208 1423
TWL_VERSION_DATE				=	2020091208
TWL_VERSION_TIME				=	1423
TWL_VERSION_MAJOR			=	5
TWL_VERSION_MINOR			=	3
TWL_VERSION_RELSTEP			=	30076
TWL_VERSION_BUILDVER_CW_CC	=	4.0
TWL_VERSION_BUILDVER_CW_LD	=	2.0
TWL_VERSION_BUILDNUM_CW_CC	=	1024
TWL_VERSION_BUILDNUM_CW_LD	=	96
TWL_VERSION_BUILDVER_CC		=	4.0
TWL_VERSION_BUILDVER_LD		=	2.0
TWL_VERSION_BUILDNUM_CC		=	1024
TWL_VERSION_BUILDNUM_LD		=	96
#
#  RELSTEP PR1=10100 PR2=10200 ...
#          RC1=20100 RC2=20200 ...
#          RELEASE=30000
#
#endif

#ifndef SDK_VERSION_NUMBER
#define SDK_VERSION_NUMBER(major, minor, relstep) \
(((major) << 24) | ((minor) << 16) | ((relstep) << 0))
#define SDK_CURRENT_VERSION_NUMBER \
SDK_VERSION_NUMBER(SDK_VERSION_MAJOR, SDK_VERSION_MINOR, SDK_VERSION_RELSTEP)
#endif

#endif
