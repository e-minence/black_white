#------------------------------------------------------------------------------
#
#	�g���[�i�[�f�[�^�pMakefile
#
#	2009.07.27	sogabe@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#���R���o�[�g��Ƃ��K�v�ȃ��[�U�[�̖��O���L�q����
#------------------------------------------------------------------------------
include user.def

#------------------------------------------------------------------------------
#�������ɍ쐬����narc��������
#------------------------------------------------------------------------------
targetname1	= trdata
NARCNAME1	= $(targetname1).narc
targetname2	= trpoke
NARCNAME2	= $(targetname2).narc

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------
TARGETDIR	= $(PROJECT_ARCDIR)

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
LDIRT_CLEAN = $(TARGETDIR)/$(NARCNAME1) $(TARGETDIR)/$(NARCNAME2)
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
LDIRT_CLEAN += *.bin $(NARCNAME1) $(NARCNAME2)
endif

#�R���o�[�g�Ώۂ̃t�@�C���c���[�𐶐�
DATAFILES:=$(wildcard trdata_*.bin)
POKEFILES:=$(wildcard trpoke_*.bin)

.PHONY:	do-build

ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���

.SUFFIXES: .bin

do-build: trdata.narc
trdata.narc: $(DATAFILES)
	nnsarc -c -l -n -i trdata.narc trdata_*.bin

do-build: trpoke.narc
trpoke.narc: $(POKEFILES)
	nnsarc -c -l -n -i trpoke.narc trpoke_*.bin

endif

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
do-build:	$(TARGETDIR)/$(NARCNAME1)	$(TARGETDIR)/$(NARCNAME2)

$(TARGETDIR)/$(NARCNAME1):	$(NARCNAME1)
	$(COPY)	$(NARCNAME1) $(TARGETDIR)

$(TARGETDIR)/$(NARCNAME2):	$(NARCNAME2)
	$(COPY)	$(NARCNAME2) $(TARGETDIR)

