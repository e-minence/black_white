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
USERS	=	takahashi yoshida kitakaze_tomohiro

#------------------------------------------------------------------------------
#�������ɍ쐬����narc��������
#------------------------------------------------------------------------------

HEADER	= batt_bg_tbl.h
HASH		= zone_spec.rb
SCR			= battgra_3d_wb.scr

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------
HEADER_DIR	= ../../prog/include/battle/
HASH_DIR		= ../../tools/hash/
RES_DIR			= ./batt_bg_tbl/

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
LDIRT_CLEAN = $(HEADER_DIR)$(HEADER) $(HASH_DIR)$(HASH) $(SCR)

ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
LDIRT_CLEAN += $(RES_DIR)$(HEADER) $(RES_DIR)$(HASH) $(RES_DIR)$(SCR) $(RES_DIR)out_end
endif

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���

#�p�[�\�i���f�[�^����

do-build: $(RES_DIR)out_end

$(RES_DIR)out_end: $(RES_DIR)wb_battle_bg.xls ../../tools/batt_bg_tbl/batt_bg_tbl_cnv.rb
	cd $(RES_DIR);ruby ../../../tools/exceltool/xls2xml/tab_out_direct.rb wb_battle_bg.xls -s > wb_battle_bg.csv
	cd $(RES_DIR);ruby ../../../tools/batt_bg_tbl/batt_bg_tbl_cnv.rb wb_battle_bg.csv ./
endif

do-build: $(HEADER_DIR)$(HEADER) $(HASH_DIR)$(HASH) $(SCR)

$(HEADER_DIR)$(HEADER):	$(RES_DIR)$(HEADER)
	$(COPY)	$(RES_DIR)$(HEADER) $(HEADER_DIR)

$(HASH_DIR)$(HASH):	$(RES_DIR)$(HASH)
	$(COPY)	$(RES_DIR)$(HASH) $(HASH_DIR)

$(SCR):	$(RES_DIR)$(SCR)
	$(COPY)	$(RES_DIR)$(SCR) ./

