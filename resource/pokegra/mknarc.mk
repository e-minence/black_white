#------------------------------------------------------------------------------
#
#	�|�P�����O���t�B�b�N�R���o�[�g�pMakefile
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
targetname	= pokegra_wb
NARCNAME	= $(targetname).narc
NAIXNAME	= $(targetname).naix

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------
TARGETDIR	= $(PROJECT_ARCDIR)pokegra

#���ʃ��[���t�@�C���̃C���N���[�h
include $(PROJECT_RSCDIR)\macro_define

#------------------------------------------------------------------------------
#���T�u�f�B���N�g���ł�Make�������ꍇ�A�����Ƀf�B���N�g����������
#------------------------------------------------------------------------------
SUBDIRS	=

.PHONY:	do-build clean


#NARC�ɕK�v��scr�t�@�C����
SCRFILE = pokegra_wb.scr
OTHERFILE = otherform_wb.scr
OTHERPLTTFILE = otherpltt_wb.scr

#�|�P�����m���X�g�b�v�A�j�����X�g�f�[�^�쐬
NONSTOPANMFILE = pokeanime_nonstop.csv
NONSTOPANMDIR	 = "w:\\debug_document\\�퓬�֘A\\�|�P�����A�j��\\"

#���\�[�X�t�@�C���̑��݂���f�B���N�g�����w��
RES_DIR = ./conv/

#�R���o�[�g�����t�@�C�����i�[����f�B���N�g�����w��
CNV_DIR = ./

#�X�N���v�g�����݂���f�B���N�g�����w��
SCRIPT_DIR = ../../tools/pokegra/
COMP_SCRIPT_DIR = ../../tools/pokegra/comp/

#�R���o�[�g�Ώۂ̃t�@�C���c���[�𐶐�
NCGFILE		:= $(wildcard $(RES_DIR)????_???_*.ncg)
NCGCFILE	:= $(wildcard $(RES_DIR)????_???c_*.ncg)
NMCFILE		:= $(wildcard $(RES_DIR)*.nmc)
NCEFILE		:= $(wildcard $(RES_DIR)*.nce)
NCLFILE	:= $(wildcard $(RES_DIR)*.ncl)
NCGCFILE	+= pfwb_migawari.ncg pbwb_migawari.ncg

ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
.SUFFIXES:
endif

#------------------------------------------------------------------------------
#	make do-build ���[��
#------------------------------------------------------------------------------
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
do-build:	nmc ncg ncgc nce ncl $(NONSTOPANMFILE) $(NARCNAME) $(TARGETDIR)/$(NARCNAME)
else
do-build:	$(TARGETDIR)/$(NARCNAME)
endif

$(TARGETDIR)/$(NARCNAME):	$(NARCNAME) $(NAIXNAME)
	$(COPY)	$(NARCNAME) $(TARGETDIR)
	$(COPY) $(NAIXNAME) $(TARGETDIR)

sub_dir:
	@$(MAKE_SUBDIR)

ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���

#nmc�f�[�^����̐������[��
ncg: $(notdir $(NCGFILE:.ncg=.NCGR))
%.NCGR: $(RES_DIR)%.ncg
	perl $(COMP_SCRIPT_DIR)ncg.pl $< $(CNV_DIR)

ncgc: $(notdir $(NCGCFILE:.ncg=.NCBR))
%.NCBR: $(RES_DIR)%.ncg
	perl $(COMP_SCRIPT_DIR)ncgc.pl $< $(CNV_DIR)

nce: $(notdir $(NMCFILE:.nmc=.NCEC))
%.NCEC: $(RES_DIR)%.nce $(SCRIPT_DIR)nce.pl
	perl $(SCRIPT_DIR)nce.pl $(RES_DIR)$*.nce $(CNV_DIR)

nmc: $(notdir $(NMCFILE:.nmc=.NMCR))
%.NMCR: $(RES_DIR)%.nmc $(RES_DIR)%.nce
	perl $(COMP_SCRIPT_DIR)nmc.pl $< $(CNV_DIR)

ncl: $(notdir $(NCLFILE:.ncl=.NCLR))
%.NCLR: $(RES_DIR)%.ncl
	perl $(SCRIPT_DIR)ncl.pl $< $(CNV_DIR)

$(NARCNAME): $(notdir $(NCGFILE:.ncg=.NCGR)) $(notdir $(NMCFILE:.nmc=.NMCR)) $(notdir $(NMCFILE:.nmc=.NCEC)) $(notdir $(NCGCFILE:.ncg=.NCBR)) $(notdir $(NCLFILE:.ncl=.NCLR)) $(SCRFILE) $(OTHERFILE) $(OTHERPLTTFILE)
	nnsarc -c -l -n -i $(NARCNAME) -S $(SCRFILE) -S $(OTHERFILE) -S $(OTHERPLTTFILE)
	$(NAIXCUT) $(NAIXNAME)

$(NONSTOPANMFILE): $(NARCNAME)
	ruby ..\..\tools\pokegra\nonstop_list_mk.rb pokegra_wb_nonstop.lst
	$(COPY)	$(NONSTOPANMFILE) $(NONSTOPANMDIR)
endif

#------------------------------------------------------------------------------
#	make clean���[��
#------------------------------------------------------------------------------
clean:
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
	@$(MAKE_SUBDIR)
	-rm -f *.N*R
	-rm -f *.N*C
	-rm -f err.txt
	-rm -f $(NARCNAME)
	-rm -f $(NAIXNAME)
endif
	-rm -f $(TARGETDIR)/$(NARCNAME)
	-rm -f $(TARGETDIR)/$(NAIXNAME)
