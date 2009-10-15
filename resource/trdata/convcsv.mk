#------------------------------------------------------------------------------
#
#	�|�P�����p�[�\�i���pMakefile
#
#	2008.08.19	tamada@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#���R���o�[�g��Ƃ��K�v�ȃ��[�U�[�̖��O���L�q����
#------------------------------------------------------------------------------

USERS	=	nishino

#------------------------------------------------------------------------------
#�������ɍ쐬����narc��������
#------------------------------------------------------------------------------

NO_HEADER = trno_def.h
TYPE_HEADER = trtype_def.h
GENDER_HEADER = trtype_sex.h
NAME_GMMFILE = trname.gmm
TYPE_GMMFILE = trtype.gmm
SCRFILE = trfgra_wb.scr

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------
TARGETDIR	= ../../prog/include/tr_tool/
GMMDIR	= ../../resource/message/src/

#------------------------------------------------------------------------------
#���T�u�f�B���N�g���ł�Make�������ꍇ�A�����Ƀf�B���N�g����������
#------------------------------------------------------------------------------
SUBDIRS	=

#���ʃ��[���t�@�C���̃C���N���[�h
include $(PROJECT_RSCDIR)/macro_define
#MWAS,MWLD
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs
include	$(COMMONDEFS_CCTYPE_CW)
#Module���[��
include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#------------------------------------------------------------------------------
#	�t�@�C���폜���[��
#------------------------------------------------------------------------------
LDIRT_CLEAN = $(TARGETDIR)$(NO_HEADER) $(TARGETDIR)$(TYPE_HEADER) $(TARGETDIR)$(GENDER_HEADER) $(GMMDIR)$(NAME_GMMFILE) $(GMMDIR)$(TYPE_GMMFILE)
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
LDIRT_CLEAN += *.s $(NO_HEADER) $(TYPE_HEADER) $(GENDER_HEADER) $(NAME_GMMFILE) $(TYPE_GMMFILE) *.scr out_end
endif

.PHONY:	do-build

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���

#�p�[�\�i���f�[�^����

do-build: out_end

out_end: trainer_wb.csv
	ruby ../../tools/trdata_conv/trdata_conv.rb $< ../message/template.gmm
endif

do-build: $(TARGETDIR)$(NO_HEADER) $(TARGETDIR)$(TYPE_HEADER) $(TARGETDIR)$(GENDER_HEADER) $(GMMDIR)$(NAME_GMMFILE) $(GMMDIR)$(TYPE_GMMFILE)

$(TARGETDIR)$(NO_HEADER):	$(NO_HEADER)
	$(COPY)	$(NO_HEADER) $(TARGETDIR)

$(TARGETDIR)$(TYPE_HEADER):	$(TYPE_HEADER)
	$(COPY)	$(TYPE_HEADER) $(TARGETDIR)

$(TARGETDIR)$(GENDER_HEADER):	$(GENDER_HEADER)
	$(COPY)	$(GENDER_HEADER) $(TARGETDIR)

$(GMMDIR)$(NAME_GMMFILE):	$(NAME_GMMFILE)
	$(COPY)	$(NAME_GMMFILE) $(GMMDIR)

$(GMMDIR)$(TYPE_GMMFILE):	$(TYPE_GMMFILE)
	$(COPY)	$(TYPE_GMMFILE) $(GMMDIR)

