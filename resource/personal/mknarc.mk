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
PERSONAL	= personal.narc
PERSONAL_NAIX	= personal.naix
PERSONAL_CSV	= personal_wb_fix.csv
WOTBL			= wotbl.narc
EVOTBL		= evo.narc
PMSTBL		= pms.narc
GROWTBL		= growtbl.narc

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------
TARGETDIR	= $(PROJECT_ARCDIR)personal/

#------------------------------------------------------------------------------
#���T�u�f�B���N�g���ł�Make�������ꍇ�A�����Ƀf�B���N�g����������
#------------------------------------------------------------------------------
SUBDIRS	=

PERFILES:=$(wildcard per_*.bin)
WOTFILES:=$(wildcard wot_*.bin)
EVOFILES:=$(wildcard evo_*.bin)
PMSFILES:=$(wildcard pms_*.bin)

ELF2BIN = ../../tools/elf2bin.exe
PMSEED = ../../tools/personal_conv/pmseed.exe

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
LDIRT_CLEAN = $(TARGETDIR)$(PERSONAL) $(TARGETDIR)$(PERSONAL_NAIX) $(TARGETDIR)$(WOTBL) $(TARGETDIR)$(EVOTBL) $(TARGETDIR)$(PMSTBL)
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
LDIRT_CLEAN += $(PERSONAL) $(PERSONAL_NAIX) $(WOTBL) $(EVOTBL) $(PMSTBL) $(PERSONAL_CSV) *.bin
endif

.PHONY:	do-build

ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
.SUFFIXES: .s .bin

#�p�[�\�i���f�[�^����
do-build: narc $(PERSONAL_CSV)

narc:
personal.narc: $(PERFILES)
	nnsarc -c -l -n -i personal.narc per_*.bin zenkoku_to_chihou.bin
wotbl.narc: $(WOTFILES)
	nnsarc -c -l -n wotbl.narc wot_*.bin
evo.narc: $(EVOFILES)
	nnsarc -c -l -n evo.narc evo_*.bin
pms.narc: $(PMSFILES)
	nnsarc -c -l -n pms.narc pms_*.bin

$(PERSONAL_CSV): personal_wb.csv
	ruby ../../tools/personal_conv/wazamachine_split.rb personal_wb.csv $(PERSONAL_CSV)
endif

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
do-build:	$(TARGETDIR)$(PERSONAL) $(TARGETDIR)$(PERSONAL_NAIX) $(TARGETDIR)$(WOTBL) $(TARGETDIR)$(EVOTBL) $(TARGETDIR)$(PMSTBL) $(TARGETDIR)$(GROWTBL)

$(TARGETDIR)$(PERSONAL):	$(PERSONAL)
	$(COPY)	$(PERSONAL) $(TARGETDIR)

$(TARGETDIR)$(PERSONAL_NAIX):	$(PERSONAL_NAIX)
	$(COPY)	$(PERSONAL_NAIX) $(TARGETDIR)

$(TARGETDIR)$(WOTBL):	$(WOTBL)
	$(COPY)	$(WOTBL) $(TARGETDIR)

$(TARGETDIR)$(EVOTBL):	$(EVOTBL)
	$(COPY)	$(EVOTBL) $(TARGETDIR)

$(TARGETDIR)$(PMSTBL):	$(PMSTBL)
	$(COPY)	$(PMSTBL) $(TARGETDIR)

$(TARGETDIR)$(GROWTBL):	$(GROWTBL)
	$(COPY)	$(GROWTBL) $(TARGETDIR)

