#------------------------------------------------------------------------------
#
#	�|�P�����p�[�\�i���pMakefile
#
#	2008.08.19	tamada@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#���R���o�[�g��Ƃ��K�v�ȃ��[�U�[�̖��O���L�q����
#------------------------------------------------------------------------------

USERS	=	sogabe nishino

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

.PHONY:	do-build clean

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

#------------------------------------------------------------------------------
#	make clean���[��
#------------------------------------------------------------------------------
clean:
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
	-rm -f *.s
	-rm -f $(HEADER)
	-rm -f $(GMMFILE)
	-rm -f $(MONSNOHASHFILE)
	-rm -f $(WOTHASHFILE)
	-rm -f $(LSTFILE)
	-rm -f *.scr
	-rm -f out_end
endif
	-rm -f $(TARGETDIR)$(HEADER)
	-rm -f $(GMMDIR)$(GMMFILE)
	-rm -f $(HASHDIR)$(MONSNOHASHFILE)
	-rm -f $(HASHDIR)$(WOTHASHFILE)
	-rm -f $(LSTDIR)$(LSTFILE)
	-rm -f $(SCRDIR)*.scr

