#! make -f
#----------------------------------------------------------------------------
# Project:  Nintendo DS  Development environment
# File:     Makefile
#
# Copyright 2006 GameFreak.inc  All rights reserved.
#----------------------------------------------------------------------------

MULTIBOOT_ARC_DIR = ./multiboot/arc/
MULTIBOOT_DIR = ./multiboot/
RESOURCE_DIR = ./resource/
PROG_DIR = ./prog/
ARC_DIR = ./prog/arc
LIB_DIR = ./lib


.PHONY : do-build clean	prog resource arc test landdata dmyfmmdl build_model area_data

#  makeした場合にすべて作られるように記述してください
#--------------------------------------- ----------------------------
do-build:
	$(MAKE) -C $(RESOURCE_DIR)
	$(MAKE) -C $(MULTIBOOT_ARC_DIR)
	$(MAKE) -C $(MULTIBOOT_DIR)
	$(MAKE) -C $(MULTIBOOT_DIR) installsrl
	$(MAKE) -C $(ARC_DIR)
	$(MAKE) -C $(PROG_DIR)


# cleanした場合にすべて消えるように記述してください
#--------------------------------------------------------------------
clean:
	$(MAKE) -C $(RESOURCE_DIR) clean
	$(MAKE) -C $(MULTIBOOT_ARC_DIR) clean
	$(MAKE) -C $(MULTIBOOT_DIR) clean
	$(MAKE) -C $(PROG_DIR) clean
	$(MAKE) -C $(ARC_DIR) clean
	rm -rf ./prog/filetree/a

#--------------------------------------------------------------------
prog:
	$(MAKE) -C $(PROG_DIR)

resource:
	$(MAKE) -C $(RESOURCE_DIR)

arc:
	$(MAKE) -C $(ARC_DIR)

test:
	$(MAKE) CONVERTUSER=true clean
	$(MAKE) CONVERTUSER=true
	$(MAKE) CONVERTUSER=false clean
	$(MAKE) CONVERTUSER=false

# フィールドマップ　地形データのみclean make
#--------------------------------------------------------------------
FLD_LAND_DATA_DIR = $(RESOURCE_DIR)/fldmapdata/land_data/

landdata:
	$(MAKE) -C $(FLD_LAND_DATA_DIR) clean
	$(MAKE) -C $(FLD_LAND_DATA_DIR)
	$(MAKE)

# フィールド動作モデル　ダミーモデル作成
#--------------------------------------------------------------------
FLD_MMDL_DATA_DIR = $(RESOURCE_DIR)/fldmmdl/

fmmdldmy:
	$(MAKE) -C $(FLD_MMDL_DATA_DIR) dmyres


build_model:
	$(MAKE) -C $(RESOURCE_DIR)/fldmapdata/build_model clean
	$(MAKE)

area_data:
	$(MAKE) -C $(RESOURCE_DIR)/fldmapdata/area_data clean
	$(MAKE)


