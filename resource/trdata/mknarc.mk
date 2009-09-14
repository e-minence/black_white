#------------------------------------------------------------------------------
#
#	�g���[�i�[�f�[�^�pMakefile
#
#	2009.07.27	sogabe@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#���R���o�[�g��Ƃ��K�v�ȃ��[�U�[�̖��O���L�q����
#------------------------------------------------------------------------------
USERS	=	sogabe

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

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW

#�R���o�[�g�Ώۂ̃t�@�C���c���[�𐶐�
DATAFILES:=$(wildcard trdata_*.s)
POKEFILES:=$(wildcard trpoke_*.s)
ELF2BINDIR = ../../tools/

.PHONY:	do-build clean

ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���

.SUFFIXES: .s .bin

do-build: trdata.narc
trdata.narc: $(DATAFILES:.s=.bin)
	nnsarc -c -l -n trdata.narc trdata_*.bin

do-build: trpoke.narc
trpoke.narc: $(POKEFILES:.s=.bin)
	nnsarc -c -l -n trpoke.narc trpoke_*.bin

do-build: $(DATAFILES:.s=.bin)
%.bin:%.s
	$(MWAS) $< -o $*.o
	$(MWLD) -dis -o $*.elf $*.o
	$(ELF2BINDIR)elf2bin $*.elf
	rm $*.o
	rm $*.elf

do-build: $(POKEFILES:.s=.bin)
%.bin:%.s
	$(MWAS) $< -o $*.o
	$(MWLD) -dis -o $*.elf $*.o
	$(ELF2BINDIR)elf2bin $*.elf
	rm $*.o
	rm $*.elf

endif

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
do-build:	$(TARGETDIR)/$(NARCNAME1)	$(TARGETDIR)/$(NARCNAME2)

$(TARGETDIR)/$(NARCNAME1):	$(NARCNAME1)
	$(COPY)	$(NARCNAME1) $(TARGETDIR)

$(TARGETDIR)/$(NARCNAME2):	$(NARCNAME2)
	$(COPY)	$(NARCNAME2) $(TARGETDIR)

#------------------------------------------------------------------------------
#	make clean���[��
#------------------------------------------------------------------------------
clean:
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
	-rm -f *.o
	-rm -f *.elf
	-rm -f *.bin
	-rm -f $(NARCNAME1)
	-rm -f $(NARCNAME2)
endif
	-rm -f $(TARGETDIR)/$(NARCNAME1)
	-rm -f $(TARGETDIR)/$(NARCNAME2)


