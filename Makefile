#! make -f
#----------------------------------------------------------------------------
# Project:  Nintendo DS  Development environment
# File:     Makefile
#
# Copyright 2006 GameFreak.inc  All rights reserved.
#----------------------------------------------------------------------------


RESOURCE_DIR = ./resource/
PROG_DIR = ./prog/


#  make�����ꍇ�ɂ��ׂč����悤�ɋL�q���Ă�������
#--------------------------------------- ----------------------------
do-build:
	$(MAKE) -C $(RESOURCE_DIR)
	$(MAKE) -C $(PROG_DIR)


# clean�����ꍇ�ɂ��ׂď�����悤�ɋL�q���Ă�������
#--------------------------------------------------------------------
.PHONY : clean	prog resource

clean:
	$(MAKE) -C $(RESOURCE_DIR) clean
	$(MAKE) -C $(PROG_DIR) clean

#--------------------------------------------------------------------
prog:
	$(MAKE) -C $(PROG_DIR)

resource:
	$(MAKE) -C $(RESOURCE_DIR)

