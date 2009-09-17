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
HASHFILE = monsno_hash.rb
GMMFILE = monsname.gmm
SCRFILE = pokegra_wb.scr
OTHERFILE = otherpoke_wb.scr

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------
TARGETDIR	= ../../prog/include/poke_tool/
GMMDIR	= ../../resource/message/src/
HASHDIR	= ../../tools/hash/

#------------------------------------------------------------------------------
#���T�u�f�B���N�g���ł�Make�������ꍇ�A�����Ƀf�B���N�g����������
#------------------------------------------------------------------------------
SUBDIRS	=

#���ʃ��[���t�@�C���̃C���N���[�h
include $(PROJECT_RSCDIR)\macro_define

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW

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

do-build: $(TARGETDIR)$(HEADER) $(GMMDIR)$(GMMFILE) $(HASHDIR)$(HASHFILE)

$(TARGETDIR)$(HEADER):	$(HEADER)
	$(COPY)	$(HEADER) $(TARGETDIR)

$(GMMDIR)$(GMMFILE):	$(GMMFILE)
	$(COPY)	$(GMMFILE) $(GMMDIR)

$(HASHDIR)$(HASHFILE):	$(HASHFILE)
	$(COPY)	$(HASHFILE) $(HASHDIR)

#------------------------------------------------------------------------------
#	make clean���[��
#------------------------------------------------------------------------------
clean:
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
	-rm -f *.s
	-rm -f $(HEADER)
	-rm -f $(GMMFILE)
	-rm -f $(HASHFILE)
	-rm -f *.scr
	-rm -f out_end
endif
	-rm -f $(TARGETDIR)$(HEADER)
	-rm -f $(GMMDIR)$(GMMFILE)
	-rm -f $(HASHDIR)$(HASHFILE)

