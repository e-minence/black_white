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
NUMHEADER = monsnum_def.h
MONSNOHASHFILE = monsno_hash.rb
WOTHASHFILE = wazaoboe_hash.rb
GMMFILE = monsname.gmm
SCRFILE = pokegra_wb.scr
LSTFILE = pokegra_wb.lst
POKELIST = pokelist.txt
OTHERFORMFILE = otherform_wb.scr
OTHERPLTTFILE = otherpltt_wb.scr
OTHERLSTFILE = otherform_wb.lst
NONSTOPLSTFILE = pokegra_wb_nonstop.lst

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------
TARGETDIR	= ../../prog/include/poke_tool/
GMMDIR	= ../message/src/
HASHDIR	= ../../tools/hash/
SCRDIR = ../pokegra/
LSTDIR = ../pokegra/conv/
POKELISTDIR = ../../tools/btl_subway/

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

.PHONY:	do-build fake

#------------------------------------------------------------------------------
#	�t�@�C���폜���[��
#------------------------------------------------------------------------------
LDIRT_CLEAN = $(TARGETDIR)$(HEADER) $(TARGETDIR)$(NUMHEADER) $(GMMDIR)$(GMMFILE) $(HASHDIR)$(MONSNOHASHFILE) $(HASHDIR)$(WOTHASHFILE) $(LSTDIR)*.lst $(SCRDIR)*.scr $(POKELISTDIR)$(POKELIST) $(SCRDIR)$(NONSTOPLSTFILE)
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
LDIRT_CLEAN += *.s $(HEADER) $(NUMHEADER) $(GMMFILE) $(MONSNOHASHFILE) $(WOTHASHFILE) $(LSTFILE) $(POKELIST) $(NONSTOPLSTFILE) *.scr out_end
endif

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���

#�p�[�\�i���f�[�^����

do-build: out_end

out_end: personal_wb.csv
	ruby ../../tools/personal_conv/personal_conv.rb $< ../message/template.gmm

fake:
	ruby ../../tools/personal_conv/personal_conv.rb personal_wb_fake.csv ../message/template.gmm
endif

do-build: $(TARGETDIR)$(HEADER) $(TARGETDIR)$(NUMHEADER) $(GMMDIR)$(GMMFILE) $(HASHDIR)$(MONSNOHASHFILE) $(HASHDIR)$(WOTHASHFILE) $(SCRDIR)$(SCRFILE) $(LSTDIR)$(LSTFILE) $(LSTDIR)$(OTHERLSTFILE) $(SCRDIR)$(OTHERFORMFILE) $(SCRDIR)$(OTHERPLTTFILE) $(POKELISTDIR)$(POKELIST) $(SCRDIR)$(NONSTOPLSTFILE)

$(TARGETDIR)$(HEADER):	$(HEADER)
	$(COPY)	$(HEADER) $(TARGETDIR)

$(TARGETDIR)$(NUMHEADER):	$(NUMHEADER)
	$(COPY)	$(NUMHEADER) $(TARGETDIR)

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

$(LSTDIR)$(OTHERLSTFILE):	$(OTHERLSTFILE)
	$(COPY)	$(OTHERLSTFILE) $(LSTDIR)

$(POKELISTDIR)$(POKELIST):	$(POKELIST)
	$(COPY)	$(POKELIST) $(POKELISTDIR)

$(SCRDIR)$(NONSTOPLSTFILE):	$(NONSTOPLSTFILE)
	$(COPY)	$(NONSTOPLSTFILE) $(SCRDIR)

