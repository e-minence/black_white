#! make -f
#----------------------------------------------------------------------------
# Project:  TWLDWC
#
# Copyright 2005-2009 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#----------------------------------------------------------------------------
include ../depend.mk

SUBDIRS     =

#----------------------------------------------------------------------------

SRCS    = dbs_print.c dbs_console.c dbs_demo.c

TARGET_LIB	= libdwcdbs$(LIBSUFFIX).a

CCFLAGS_MSGSTYLE = -msgstyle gcc

include $(SDK_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------

do-build:       $(TARGETS)

include $(SDK_ROOT)/build/buildtools/modulerules
