#------------------------------------------------------------------------------
#
#	�퓬�w�i�e�[�u���f�[�^�R���o�[�g�pMakefile
#
#	2009.11.25	sogabe@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#���R���o�[�g��Ƃ��K�v�ȃ��[�U�[�̖��O���L�q����
#------------------------------------------------------------------------------
include user.def

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
LDIRT_CLEAN += $(TARGET_NARC) $(TARGET_NAIX) *.s *.bin
endif

.PHONY:	do-build

ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
.SUFFIXES: .s .bin

#�p�[�\�i���f�[�^����
do-build: batt_bg_tbl.narc

%.bin:%.s
	$(MWAS) $< -o $*.o
	$(MWLD) -dis -o $*.elf $*.o
	$(ELF2BIN) $*.elf
	rm $*.o
	rm $*.elf

batt_bg_tbl.narc: zone_spec_table.bin batt_bg.bin batt_stage.bin
	nnsarc -c -l -n -i batt_bg_tbl.narc zone_spec_table.bin batt_bg.bin batt_stage.bin

endif

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
do-build:	$(TARGETDIR)$(TARGET_NARC) $(TARGETDIR)$(TARGET_NAIX)

$(TARGETDIR)$(TARGET_NARC):	$(TARGET_NARC)
	$(COPY)	$(TARGET_NARC) $(TARGETDIR)

$(TARGETDIR)$(TARGET_NAIX):	$(TARGET_NAIX)
	$(COPY)	$(TARGET_NAIX) $(TARGETDIR)

