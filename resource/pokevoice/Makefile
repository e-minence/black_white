#------------------------------------------------------------------------------
#
#	ポケモングラフィックコンバート用Makefile
#
#	2008.08.19	tamada@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#※コンバート作業が必要なユーザーの名前を記述する
#------------------------------------------------------------------------------
USERS	=	ichinose satou

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------
NARC = pokevoice_param.narc

#------------------------------------------------------------------------------
#※コピー先へのパスを書く（通常はPROJECT_ARCDIRでよい）
#------------------------------------------------------------------------------
COPYDIR	= $(PROJECT_ARCDIR)
RUBYDIR = ../../tools/pokevoice/

#------------------------------------------------------------------------------
#※サブディレクトリでもMakeしたい場合、ここにディレクトリ名を書く
#------------------------------------------------------------------------------
SUBDIRS	=

#共通ルールファイルのインクルード
include $(PROJECT_RSCDIR)\macro_define
#MWAS,MWLD
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs
include	$(COMMONDEFS_CCTYPE_CW)
#Moduleルール
include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

.PHONY:	do-build clean

ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
do-build: $(NARC)

$(NARC): pokevoice_wb.csv
	ruby $(RUBYDIR)pokevoice.rb pokevoice_wb.csv
	nnsarc -c -l -n $(NARC) pvp_*.bin
endif

#------------------------------------------------------------------------------
#	make do-build ルール
#------------------------------------------------------------------------------
do-build: $(COPYDIR)$(NARC)

$(COPYDIR)$(NARC): $(NARC)
	$(COPY)	$(NARC) $(COPYDIR)

#------------------------------------------------------------------------------
#	ファイル削除ルール
#------------------------------------------------------------------------------
LDIRT_CLEAN = $(COPYDIR)$(NARC)

ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
LDIRT_CLEAN += $(NARC)
endif

