#------------------------------------------------------------------------------
#
#	�|�P�����p�[�\�i���pMakefile
#
#	2008.08.19	tamada@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#���R���o�[�g��Ƃ��K�v�ȃ��[�U�[�̖��O���L�q����
#------------------------------------------------------------------------------
include user.def

#------------------------------------------------------------------------------
#�������ɍ쐬����narc��������
#------------------------------------------------------------------------------

HEADER = monsno_def.h
MONSNOHASHFILE = monsno_hash.rb
WOTHASHFILE = wazaoboe_hash.rb
GMMFILE = monsname.gmm
SCRFILE = pokegra_wb.scr
LSTFILE = pokegra_wb.lst
OTHERFORMFILE = otherform_wb.scr
OTHERPLTTFILE = otherpltt_wb.scr

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------
TARGETDIR	= ../../prog/include/poke_tool/
GMMDIR	= ../message/src/
HASHDIR	= ../../tools/hash/
SCRDIR = ../pokegra/
LSTDIR = ../pokegra/conv/

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
LDIRT_CLEAN = $(TARGETDIR)$(HEADER) $(GMMDIR)$(GMMFILE) $(HASHDIR)$(MONSNOHASHFILE) $(HASHDIR)$(WOTHASHFILE) $(LSTDIR)$(LSTFILE) $(SCRDIR)*.scr
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
LDIRT_CLEAN += *.s $(HEADER) $(GMMFILE) $(MONSNOHASHFILE) $(WOTHASHFILE) $(LSTFILE) *.scr out_end
endif

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���

#�p�[�\�i���f�[�^����

do-build: out_end

out_end: personal_wb.csv
	ruby ../../tools/personal_conv/personal_conv.rb $< ../message/template.gmm
endif

do-build: $(TARGETDIR)$(HEADER) $(GMMDIR)$(GMMFILE) $(HASHDIR)$(MONSNOHASHFILE) $(HASHDIR)$(WOTHASHFILE) $(SCRDIR)$(SCRFILE) $(LSTDIR)$(LSTFILE) $(SCRDIR)$(OTHERFORMFILE) $(SCRDIR)$(OTHERPLTTFILE)

$(TARGETDIR)$(HEADER):	$(HEADER)
	$(COPY)	$(HEADER) $(TARGETDIR)

$(GMMDIR)$(GMMFILE):	$(GMMFILE)
	$(COPY)	$(GMMFILE) $(GMMDIR)

$(HASHDIR)$(MONSNOHASHFILE):	$(MONSNOHASHFILE)
	$(COPY)	$(MONSNOHASHFILE) $(HASHDIR)

$(HASHDIR)$(WOTHASHFILE):	$(WOTHASHFILE)
	$(COPY)	$(WOTHASHFILE) $(HASHDIR)

$(SCRDIR)$(SCRFILE):	$(SCRFILE)
	$(COPY)	$(SCRFILE) $(SCRDIR)

$(LSTDIR)$(LSTFILE):	$(LSTFILE)
	$(COPY)	$(LSTFILE) $(LSTDIR)

$(SCRDIR)$(OTHERFORMFILE):	$(OTHERFORMFILE)
	$(COPY)	$(OTHERFORMFILE) $(SCRDIR)

$(SCRDIR)$(OTHERPLTTFILE):	$(OTHERPLTTFILE)
	$(COPY)	$(OTHERPLTTFILE) $(SCRDIR)

