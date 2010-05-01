#! make -f
#----------------------------------------------------------------------------
# Project:  Nintendo DS  Development environment
# File:     Makefile
#
# Copyright 2006 GameFreak.inc  All rights reserved.
#----------------------------------------------------------------------------


#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#	�����Ⴄ�Ƃ����Make�ł��Ȃ��悤�ɂ��邽�߂̂��܂��Ȃ�
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#���݂̃f�B���N�g���icygwin�̃p�X����ϊ�����j
NOWDIR	=	$(shell pwd|tr [A-Z] [a-z] |sed -e 's/\/cygdrive\///')/
#���ϐ����z�肵�Ă���f�B���N�g������A�F����菜��������
TGTDIR	= $(shell echo $(PROJECT_ROOT)|tr [A-Z] [a-z]|sed -e 's/://')
ifneq ($(NOWDIR),$(TGTDIR))
$(warning �R���p�C�������m�F���Ă�������)
$(error $(NOWDIR) != $(TGTDIR) )
endif

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



MULTIBOOT_ARC_DIR = ./multiboot/arc/
MULTIBOOT_DIR = ./multiboot/
MULTIBOOT2_ARC_DIR = ./multiboot2/arc/
MULTIBOOT2_DIR = ./multiboot2/
RESOURCE_DIR = ./resource/
PROG_DIR = ./prog/
ARC_DIR = ./prog/arc
LIB_DIR = ./lib


.PHONY : do-build rom force clean	prog resource arc test landdata dmyfmmdl build_model area_data

#  make�����ꍇ�ɂ��ׂč����悤�ɋL�q���Ă�������
#--------------------------------------- ----------------------------
do-build:
	$(MAKE) -C $(RESOURCE_DIR)
#	$(MAKE) -C $(MULTIBOOT_ARC_DIR)
#	$(MAKE) -C $(MULTIBOOT_DIR)
#	$(MAKE) -C $(MULTIBOOT_DIR) installsrl
	$(MAKE) -C $(ARC_DIR)
	$(MAKE) -C $(PROG_DIR)


rom:
	touch prog/src/test/testmode.c
	$(MAKE)

force:
	rm -rf prog/depend/*
	rm -rf multiboot/depend/*
	$(MAKE)

#-------------------------------------------------------------------
#  �F�Ⴂ�EROM�ō쐬�p
#-------------------------------------------------------------------
white:
	cp ./prog/spec/version_w ./prog/version
	$(MAKE) -C $(PROG_DIR) clean
	$(MAKE) -C $(ARC_DIR) clean
	$(MAKE) -C $(RESOURCE_DIR)
	$(MAKE) -C $(ARC_DIR) 
	$(MAKE) -C $(PROG_DIR) 
	cp ./prog/spec/version ./prog/version

black:
	cp ./prog/spec/version_b ./prog/version
	$(MAKE) -C $(PROG_DIR) clean
	$(MAKE) -C $(ARC_DIR) clean
	$(MAKE) -C $(RESOURCE_DIR)
	$(MAKE) -C $(ARC_DIR) 
	$(MAKE) -C $(PROG_DIR) 
	cp ./prog/spec/version ./prog/version

white_rom:
	cp ./prog/spec/version_wr ./prog/version
	$(MAKE) -C $(PROG_DIR) clean
	$(MAKE) -C $(ARC_DIR) clean
	$(MAKE) -C $(RESOURCE_DIR)
	$(MAKE) -C $(ARC_DIR) 
	$(MAKE) -C $(PROG_DIR) 
	cp ./prog/spec/version ./prog/version

black_rom:
	cp ./prog/spec/version_br ./prog/version
	$(MAKE) -C $(PROG_DIR) clean
	$(MAKE) -C $(ARC_DIR) clean
	$(MAKE) -C $(RESOURCE_DIR)
	$(MAKE) -C $(ARC_DIR) 
	$(MAKE) -C $(PROG_DIR) 
	cp ./prog/spec/version ./prog/version

master_rom:
	$(MAKE) white_rom
	cp ./prog/bin/ARM9-TS.HYB/Rom/main.srl ./rom/IRAJ.srl
	$(MAKE) black_rom
	cp ./prog/bin/ARM9-TS.HYB/Rom/main.srl ./rom/IRBJ.srl

master:
	$(MAKE) white
	cp ./prog/bin/ARM9-TS.HYB/Release/main.srl ./rom/IRAJ_r.srl
	$(MAKE) black
	cp ./prog/bin/ARM9-TS.HYB/Release/main.srl ./rom/IRBJ_r.srl

# clean�����ꍇ�ɂ��ׂď�����悤�ɋL�q���Ă�������
#--------------------------------------------------------------------
clean:
	$(MAKE) -C $(RESOURCE_DIR) -j3 clean
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

mb:
	$(MAKE) -C $(RESOURCE_DIR)
	$(MAKE) -C $(MULTIBOOT_ARC_DIR)
	$(MAKE) -C $(MULTIBOOT_DIR)
#	$(MAKE) -C $(MULTIBOOT_DIR) installsrl
	$(MAKE) -C $(MULTIBOOT2_ARC_DIR)
	$(MAKE) -C $(MULTIBOOT2_DIR)
#	$(MAKE) -C $(MULTIBOOT2_DIR) installsrl
	$(MAKE) -C $(ARC_DIR)
	$(MAKE) -C $(PROG_DIR)

test:
	$(MAKE) CONVERTUSER=true clean
	$(MAKE) CONVERTUSER=true
	$(MAKE) CONVERTUSER=false clean
	$(MAKE) CONVERTUSER=false


sddebug:
	$(MAKE) -C $(PROG_DIR) clean
	$(MAKE) PM_DEBUG_SD_PRINT=yes


# �t�B�[���h�}�b�v�@�n�`�f�[�^�̂�clean make
#--------------------------------------------------------------------
FLD_LAND_DATA_DIR = $(RESOURCE_DIR)/fldmapdata/land_data/

landdata:
	$(MAKE) -C $(FLD_LAND_DATA_DIR) clean
	$(MAKE) -C $(FLD_LAND_DATA_DIR)
	$(MAKE)

# �t�B�[���h���샂�f���@�_�~�[���f���쐬
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

eventdata:
	$(MAKE) -C $(RESOURCE_DIR)/fldmapdata/eventdata clean
	$(MAKE)

#----------------------------------------------------------------------------
#	make��A������\��
#----------------------------------------------------------------------------
date:
	$(MAKE)
	date


