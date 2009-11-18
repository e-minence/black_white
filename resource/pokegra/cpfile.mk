#------------------------------------------------------------------------------
#
#	ポケモングラフィックコンバート用Makefile
#
#	2008.08.19	tamada@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#※コンバート作業が必要なユーザーの名前を記述する
#------------------------------------------------------------------------------
#USERS	=	yoshida tsuruta_saya kitakaze_tomohiro mizutani_megumi sogabe
include user.def

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#※コピー先へのパスを書く（通常はPROJECT_ARCDIRでよい）
#------------------------------------------------------------------------------

#共通ルールファイルのインクルード
include $(PROJECT_RSCDIR)\macro_define

#------------------------------------------------------------------------------
#※サブディレクトリでもMakeしたい場合、ここにディレクトリ名を書く
#------------------------------------------------------------------------------
SUBDIRS	=

#リソースファイルの存在するディレクトリを指定

#LINK情報を書き換えたファイルを格納するディレクトリを指定
COPY_DIR = ./conv/

#スクリプトが存在するディレクトリを指定
SCRIPT_DIR = ../../tools/pokegra/

.PHONY:	do-build clean

ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする

do-build:
	ruby $(SCRIPT_DIR)tschk.rb $(COPY_DIR)pokegra_wb.lst ../../../pokemon_wb_doc/pokegra/ $(COPY_DIR)

endif

#------------------------------------------------------------------------------
#	make cleanルール
#------------------------------------------------------------------------------
clean:
ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
	-rm $(COPY_DIR)*.ncg
	-rm $(COPY_DIR)*.nce
	-rm $(COPY_DIR)*.nmc
	-rm $(COPY_DIR)*.ncl
endif


