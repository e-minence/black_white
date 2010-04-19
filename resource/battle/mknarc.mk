#------------------------------------------------------------------------------
#
#	�퓬�w�i�e�[�u���f�[�^�R���o�[�g�pMakefile
#
#	2009.11.25	sogabe@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#���R���o�[�g��Ƃ��K�v�ȃ��[�U�[�̖��O���L�q����
#------------------------------------------------------------------------------
#include user.def
USERS	=	yoshida kitakaze_tomohiro

#------------------------------------------------------------------------------
#�������ɍ쐬����narc��������
#------------------------------------------------------------------------------
TARGET	= batt_bg_tbl
TARGET_NARC	= $(TARGET).narc
TARGET_NAIX	= $(TARGET).naix

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------
TARGETDIR	= $(PROJECT_ARCDIR)
RES_DIR	= ./batt_bg_tbl/

#------------------------------------------------------------------------------
#���T�u�f�B���N�g���ł�Make�������ꍇ�A�����Ƀf�B���N�g����������
#------------------------------------------------------------------------------
SUBDIRS	=

ELF2BIN = ../../tools/elf2bin.exe

#���ʃ��[���t�@�C���̃C���N���[�h
include $(PROJECT_RSCDIR)\macro_define
#MWAS,MWLD
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs
include	$(COMMONDEFS_CCTYPE_CW)
#Module���[��
include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#------------------------------------------------------------------------------
#	�t�@�C���폜���[��
#------------------------------------------------------------------------------
LDIRT_CLEAN = $(TARGETDIR)$(TARGET_NARC) $(TARGETDIR)$(TARGET_NAIX)
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
LDIRT_CLEAN += $(RES_DIR)$(TARGET_NARC) $(RES_DIR)$(TARGET_NAIX) $(RES_DIR)*.s $(RES_DIR)*.bin
endif

.PHONY:	do-build

ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
.SUFFIXES: .s .bin

#�p�[�\�i���f�[�^����
do-build: $(RES_DIR)batt_bg_tbl.narc

$(RES_DIR)%.bin: $(RES_DIR)%.s ../battle/battgra_wb_def.h
	$(MWAS) $< -o $(RES_DIR)$*.o
	$(MWLD) -dis -o $(RES_DIR)$*.elf $(RES_DIR)$*.o
	$(ELF2BIN) $(RES_DIR)$*.elf
	rm $(RES_DIR)$*.o
	rm $(RES_DIR)$*.elf

$(RES_DIR)batt_bg_tbl.narc: $(RES_DIR)zone_spec_table.bin $(RES_DIR)batt_bg.bin $(RES_DIR)batt_stage.bin
	cd $(RES_DIR); nnsarc -c -l -n -i batt_bg_tbl.narc zone_spec_table.bin batt_bg.bin batt_stage.bin

endif

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
do-build:	$(TARGETDIR)$(TARGET_NARC) $(TARGETDIR)$(TARGET_NAIX)

$(TARGETDIR)$(TARGET_NARC):	$(RES_DIR)$(TARGET_NARC)
	$(COPY)	$(RES_DIR)$(TARGET_NARC) $(TARGETDIR)

$(TARGETDIR)$(TARGET_NAIX):	$(RES_DIR)$(TARGET_NAIX)
	$(COPY)	$(RES_DIR)$(TARGET_NAIX) $(TARGETDIR)

