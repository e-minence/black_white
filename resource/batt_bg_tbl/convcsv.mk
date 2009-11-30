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

HEADER = batt_bg_tbl.h
HASH = zone_spec.rb

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------
HEADER_DIR	= ../../prog/include/battle/
HASH_DIR		= ../../tools/hash/

#------------------------------------------------------------------------------
#���T�u�f�B���N�g���ł�Make�������ꍇ�A�����Ƀf�B���N�g����������
#------------------------------------------------------------------------------
SUBDIRS	=

#���ʃ��[���t�@�C���̃C���N���[�h
include $(PROJECT_RSCDIR)\macro_define
#MWAS,MWLD
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs
include	$(COMMONDEFS_CCTYPE_CW)
#Module���[��
include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

.PHONY:	do-build

#------------------------------------------------------------------------------
#	�t�@�C���폜���[��
#------------------------------------------------------------------------------
LDIRT_CLEAN = $(HEADER_DIR)$(HEADER) $(HASH_DIR)$(HASH)

ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
LDIRT_CLEAN += $(HEADER) $(HASH) out_end
endif

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���

#�p�[�\�i���f�[�^����

do-build: out_end

out_end: wb_battle_bg.xls ../../tools/batt_bg_tbl/batt_bg_tbl_cnv.rb
	ruby ../../tools/exceltool/xls2xml/tab_out.rb -c wb_battle_bg.xls > wb_battle_bg.csv
	ruby ../../tools/batt_bg_tbl/batt_bg_tbl_cnv.rb wb_battle_bg.csv ./
endif

do-build: $(HEADER_DIR)$(HEADER) $(HASH_DIR)$(HASH)

$(HEADER_DIR)$(HEADER):	$(HEADER)
	$(COPY)	$(HEADER) $(HEADER_DIR)

$(HASH_DIR)$(HASH):	$(HASH)
	$(COPY)	$(HASH) $(HASH_DIR)
