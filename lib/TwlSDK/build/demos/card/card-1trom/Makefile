#! make -f
#----------------------------------------------------------------------------
# Project:  TwlSDK - demos - CARD - card-1trom
# File:     Makefile
#
# Copyright 2007-2008 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Date:: 2008-09-18#$
# $Rev: 8573 $
# $Author: okubata_ryoma $
#----------------------------------------------------------------------------

SUBDIRS         =	

#----------------------------------------------------------------------------
TARGET_PLATFORM	= TWL NITRO

TARGET_BIN      = main.srl

SRCS            = main.c
SRCDIR          = ../card-mrom/src

# TWL_ROMSPEEDを明示的に指定するとデフォルトの.rsfファイルへ反映されます。
# 独自に用意した.rsfファイルがある場合は, Property セクション内に
#  RomSpeedType  {1TROM|MROM}
# と記述する必要があります。
TWL_ROMSPEED    = 1TROM

# 転送速度計測用のサンプルファイル
MAKEROM_ROMROOT     = $(ROOT)/build/demos/snd/stream/files
MAKEROM_ROMFILES    = kart_title.32.wav


include	$(TWLSDK_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(TWLSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
