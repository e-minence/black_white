#------------------------------------------------------------------------------
#
#	�|�P�����p�[�\�i���pMakefile
#
#	2008.08.19	tamada@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#���R���o�[�g��Ƃ��K�v�ȃ��[�U�[�̖��O���L�q����
#------------------------------------------------------------------------------
USERS	=	sogabe niishino

#------------------------------------------------------------------------------
#�������ɍ쐬����narc��������
#------------------------------------------------------------------------------
PERSONAL	= personal.narc
WOTBL			= wotbl.narc
EVOTBL		= evo.narc
GROWTBL		= growtbl.narc

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------
TARGETDIR	= $(PROJECT_ARCDIR)personal/

#------------------------------------------------------------------------------
#���T�u�f�B���N�g���ł�Make�������ꍇ�A�����Ƀf�B���N�g����������
#------------------------------------------------------------------------------
SUBDIRS	=

PERFILES:=$(wildcard per_*.s)
WOTFILES:=$(wildcard wot_*.s)
EVOFILES:=$(wildcard evo_*.s)

ELF2BIN = ../../tools/elf2bin.exe

#���ʃ��[���t�@�C���̃C���N���[�h
include $(PROJECT_RSCDIR)\macro_define
#MWAS,MWLD
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs
include	$(COMMONDEFS_CCTYPE_CW)
#Module���[��
include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

.PHONY:	do-build clean

ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
.SUFFIXES: .s .bin

#�p�[�\�i���f�[�^����
do-build: personal wot evo narc

personal: $(PERFILES:.s=.bin)
%.bin:%.s
	$(MWAS) $< -o $*.o
	$(MWLD) -dis -o $*.elf $*.o
	$(ELF2BIN) $*.elf
	rm $*.o
	rm $*.elf

wot: $(WOTFILES:.s=.bin)
%.bin:%.s
	$(MWAS) $< -o $*.o
	$(MWLD) -dis -o $*.elf $*.o
	$(ELF2BIN) $*.elf
	rm $*.o
	rm $*.elf

evo: $(EVOFILES:.s=.bin)
%.bin:%.s
	$(MWAS) $< -o $*.o
	$(MWLD) -dis -o $*.elf $*.o
	$(ELF2BIN) $*.elf
	rm $*.o
	rm $*.elf

narc:
personal.narc: $(PERFILES:.s=.bin)
	nnsarc -c -l -n personal.narc per_*.bin
wotbl.narc: $(WOTFILES:.s=.bin)
	nnsarc -c -l -n wotbl.narc wot_*.bin
evo.narc: $(EVOFILES:.s=.bin)
	nnsarc -c -l -n evo.narc evo_*.bin

#�i���e�[�u���f�[�^����
#do-build: evo.narc
#evo.narc: $(EVOFILES:.s=.bin)
#	nnsarc -c -l -n evo.narc evo_*.bin
#	../../../../pmgs_conv/personalcnv/pmseed

endif

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
do-build:	$(TARGETDIR)$(PERSONAL) $(TARGETDIR)$(WOTBL) $(TARGETDIR)$(EVOTBL) $(TARGETDIR)$(GROWTBL)

$(TARGETDIR)$(PERSONAL):	$(PERSONAL)
	$(COPY)	$(PERSONAL) $(TARGETDIR)

$(TARGETDIR)$(WOTBL):	$(WOTBL)
	$(COPY)	$(WOTBL) $(TARGETDIR)

$(TARGETDIR)$(EVOTBL):	$(EVOTBL)
	$(COPY)	$(EVOTBL) $(TARGETDIR)

$(TARGETDIR)$(GROWTBL):	$(GROWTBL)
	$(COPY)	$(GROWTBL) $(TARGETDIR)

#------------------------------------------------------------------------------
#	make clean���[��
#------------------------------------------------------------------------------
clean:
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
	-rm -f $(PERSONAL)
	-rm -f $(WOTBL)
	-rm -f $(EVOTBL)
	-rm -f *.bin
endif
	-rm -f $(TARGETDIR)$(PERSONAL)
	-rm -f $(TARGETDIR)$(WOTBL)
	-rm -f $(TARGETDIR)$(EVOTBL)

