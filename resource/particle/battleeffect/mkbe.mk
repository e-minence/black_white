
#------------------------------------------------------------------------------
#
#	ポケモングラフィックコンバート用Makefile
#
#	2008.08.19	tamada@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#※コンバート作業が必要なユーザーの名前を記述する
#------------------------------------------------------------------------------
USERS	=	fujiwara yoshida sogabe

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------
targetname	= spa
NARCNAME	= $(targetname).narc
NAIXNAME	= $(targetname).naix
HEADNAME	= $(targetname)_def.h

#------------------------------------------------------------------------------
#※コピー先へのパスを書く（通常はPROJECT_ARCDIRでよい）
#------------------------------------------------------------------------------
TARGETDIR	= $(PROJECT_ARCDIR)particle/battleeffect

#共通ルールファイルのインクルード
include $(PROJECT_RSCDIR)\macro_define

#------------------------------------------------------------------------------
#※サブディレクトリでもMakeしたい場合、ここにディレクトリ名を書く
#------------------------------------------------------------------------------
SUBDIRS	= 

#コンバート対象のファイルツリーを生成

.PHONY:	do-build clean

ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
.SUFFIXES:	
endif

#------------------------------------------------------------------------------
#	make do-build ルール
#------------------------------------------------------------------------------
do-build:	$(TARGETDIR)/$(NARCNAME)

$(TARGETDIR)/$(NARCNAME):	$(NARCNAME)
	$(COPY)	$(NARCNAME) $(TARGETDIR)
	$(COPY) $(NAIXNAME) $(TARGETDIR)
	$(COPY) $(HEADNAME) $(TARGETDIR)

ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
$(NARCNAME): *.spa
	nnsarc -c -l -n -i $(NARCNAME) *.spa
	$(NAIXCUT) $(NAIXNAME)
	../../../tools/etd.exe $(NAIXNAME)
endif

#------------------------------------------------------------------------------
#	make cleanルール
#------------------------------------------------------------------------------
clean:


