#------------------------------------------------------------------------------
#  narcを作るmakefile  
#  コピーしてtargetnameを書き換えれば汎用的に使えます
#  2008.08.19
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#※コンバートを行う人のユーザーIDをここに書く
#------------------------------------------------------------------------------
USERS	= kageyama_shota sogabe

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------
targetname =

#------------------------------------------------------------------------------
#※コピー先へのパスを書く（通常はPROJECT_ARCDIRでよい）
#------------------------------------------------------------------------------
TARGETDIR = ../../prog/src/test/soga/
MSGDIR = ../message/src/debug/

#------------------------------------------------------------------------------
#※サブディレクトリでもMakeしたい場合、ここにディレクトリ名を書く
#------------------------------------------------------------------------------
SUBDIRS	=

#共通ルールファイルのインクルード
include	$(PROJECT_RSCDIR)\macro_define

ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
# サフィックスルールを定義することで 依存関係を自動で判断してくれる
.SUFFIXES: 
endif

.PHONY:	do-build clean 

#------------------------------------------------------------------------------
#	make do-build ルール
#------------------------------------------------------------------------------
ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
do-build:	out_end $(TARGETDIR)ev_bgm_list.cdat $(MSGDIR)ev_bgm_list.gmm
# コンバート
out_end: ev_bgm_list.txt
	ruby ../../tools/effectviewer/effectviewer.rb ev_bgm_list.txt ../message/template.gmm
else
do-build:	$(TARGETDIR)ev_bgm_list.cdat $(MSGDIR)ev_bgm_list.gmm
endif

# コピーだけ実行する
$(TARGETDIR)ev_bgm_list.cdat: ev_bgm_list.cdat
	$(COPY) ev_bgm_list.cdat $(TARGETDIR)

$(MSGDIR)ev_bgm_list.gmm: ev_bgm_list.gmm
	$(COPY) ev_bgm_list.gmm $(MSGDIR)

#------------------------------------------------------------------------------
#	make clean ルール
#※生成ファイルを削除するように記述を変更してください
#------------------------------------------------------------------------------
clean:
ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、中間ファイル削除
	rm -f out_end
endif
	rm -f $(TARGETDIR)ev_bgm_list.cdat
	rm -f $(MSGDIR)ev_bgm_list.gmm


