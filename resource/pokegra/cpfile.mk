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

#------------------------------------------------------------------------------
#���R�s�[��ւ̃p�X�������i�ʏ��PROJECT_ARCDIR�ł悢�j
#------------------------------------------------------------------------------

#���ʃ��[���t�@�C���̃C���N���[�h
include $(PROJECT_RSCDIR)\macro_define

#------------------------------------------------------------------------------
#���T�u�f�B���N�g���ł�Make�������ꍇ�A�����Ƀf�B���N�g����������
#------------------------------------------------------------------------------
SUBDIRS	=

#���\�[�X�t�@�C���̑��݂���f�B���N�g�����w��

#LINK���������������t�@�C�����i�[����f�B���N�g�����w��
COPY_DIR = ./conv/

#�X�N���v�g�����݂���f�B���N�g�����w��
SCRIPT_DIR = ../../tools/pokegra/

.PHONY:	do-build clean fake

ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���

do-build: normal other

normal:
	ruby $(SCRIPT_DIR)tschk.rb $(COPY_DIR)pokegra_wb.lst ../../../pokemon_wb_doc/pokegra/ $(COPY_DIR)

other:
	ruby $(SCRIPT_DIR)tschk.rb $(COPY_DIR)otherform_wb.lst ../../../pokemon_wb_doc/pokegra/ $(COPY_DIR)

fake:
	ruby fake_list_make.rb $(COPY_DIR)otherform_wb.lst $(COPY_DIR)otherform_wb_fake.lst
	ruby $(SCRIPT_DIR)tschk.rb $(COPY_DIR)otherform_wb_fake.lst ../../../pokemon_wb_doc/pokegra/ $(COPY_DIR)

endif

#------------------------------------------------------------------------------
#	make clean���[��
#------------------------------------------------------------------------------
clean:
ifeq	($(CONVERTUSER),true)	#�R���o�[�g�Ώێ҂̂݁A�R���o�[�g�̃��[����L���ɂ���
	-rm $(COPY_DIR)*.ncg
	-rm $(COPY_DIR)*.nce
	-rm $(COPY_DIR)*.nmc
	-rm $(COPY_DIR)*.ncl
endif

