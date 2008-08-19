#! make -f
#----------------------------------------------------------------------------
# Project:  Nintendo DS  Development environment
# File:     Makefile
#
# Copyright 2006 GameFreak.inc  All rights reserved.
#----------------------------------------------------------------------------


RESOURCE_DIR = ./resource/
PROG_DIR = ./prog/
ARC_DIR = ./prog/arc


#  make�����ꍇ�ɂ��ׂč����悤�ɋL�q���Ă�������
#--------------------------------------- ----------------------------
do-build:
	$(MAKE) -C $(RESOURCE_DIR)
	$(MAKE) -C $(ARC_DIR)
	$(MAKE) -C $(PROG_DIR)


# clean�����ꍇ�ɂ��ׂď�����悤�ɋL�q���Ă�������
#--------------------------------------------------------------------
.PHONY : clean	prog resource arc

clean:
	$(MAKE) -C $(RESOURCE_DIR) clean
	$(MAKE) -C $(PROG_DIR) clean
	$(MAKE) -C $(ARC_DIR) clean
	rm -r ./prog/filetree/a

#--------------------------------------------------------------------
prog:
	$(MAKE) -C $(PROG_DIR)

resource:
	$(MAKE) -C $(RESOURCE_DIR)

arc:
	$(MAKE) -C $(ARC_DIR)

