#! make -f
#----------------------------------------------------------------------------
# Project:  narcリスト生成
# File:     Makefile
#
# Copyright 2008 GameFreak.inc  All rights reserved.
#
#----------------------------------------------------------------------------

#arc以下のnarcとnaixは全てcleanする
NARC_TARGET = $(shell find . -name '*.narc')
NAIX_TARGET = $(shell find . -name '*.naix')
#multibootに乗せるarcは.oが生成されるので
OBJ_TARGET = $(shell find . -name '*.o')
BIN_TARGET = $(shell find . -name '*.bin')
#.hも多かったので、念のため消す
H_TARGET = $(shell find . -name '*.h')
#そのほかのコピーされてくるファイル
ETC_TARGET = battle/pl_batt_obj.narc.org font/font.o 

#Makeで生成されるarc_def.h arc_file.hはmake cleanの削除対象にする
LDIRT_CLEAN	= arc_def.h arc_file.h narc_tree.inc file_tree.inc ../file_tree.inc $(NARC_TARGET) $(NAIX_TARGET) $(H_TARGET) $(ETC_TARGET) $(OBJ_TARGET) $(BIN_TARGET)

#----------------------------------------------------------------------------
VER_FILE	= ../version

include $(VER_FILE)
-include narc_tree.inc

.PHONY:	do-build clean

do-build: narc_tree.inc arc_def.h arc_file.h file_tree.inc arc_tool.lst

narc_tree.inc: ../../tools/arc/narc_tree.pl arc_tool.lst $(VER_FILE)
	perl ../../tools/arc/narc_tree.pl arc_tool.lst $(PM_VERSION) $(PM_DEBUG)

arc_def.h arc_file.h: ../../tools/arc/header.pl arc_tool.lst $(VER_FILE)
	perl ../../tools/arc/header.pl arc_tool.lst $(PM_VERSION) $(PM_DEBUG) no

file_tree.inc: ../../tools/arc/narc.pl arc_tool.lst $(VER_FILE) $(NARC_TREE)
	perl ../../tools/arc/narc.pl arc_tool.lst $(PM_VERSION) $(PM_DEBUG) ./ ../filetree/ no

clean:
	-rm -f $(LDIRT_CLEAN)
