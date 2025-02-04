#------------------------------------------------------------------------------
#  demo3d用 2D素材のnarcを作るmakefile  
#  2010.04.27
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#ユーザー定義をインクルード
#------------------------------------------------------------------------------
include ..\make_users

ifneq	(,$(filter $(USERNAME),$(ALL_USERS)))
RESMAKE_USER	= true
else
RESMAKE_USER = false
endif

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------
targetname = demo3d_2dgra

#------------------------------------------------------------------------------
#※コピー先へのパスを書く（通常はPROJECT_ARCDIRでよい）
#------------------------------------------------------------------------------
TARGETDIR = $(PROJECT_ARCDIR)

#------------------------------------------------------------------------------
#※サブディレクトリでもMakeしたい場合、ここにディレクトリ名を書く
#------------------------------------------------------------------------------
SUBDIRS	= 

#共通ルールファイルのインクルード
include	$(PROJECT_RSCDIR)\macro_define

# 自動でリソースを集める
lsources:=$(wildcard *.ncl)
gsources:=$(wildcard *.ncg)
ssources:=$(wildcard *.nsc)
esources:=$(wildcard *.nce)

# リソースが吐き出すファイルを自動でつくる
lobjects = $(subst .ncl,.NCLR,$(lsources))
gobjects = $(subst .ncg,.NCGR,$(gsources))
sobjects = $(subst .nsc,.NSCR,$(ssources))
eobjects = $(subst .nce,.NCER,$(esources))
aobjects = $(subst .nce,.NANR,$(esources))

ifeq	($(RESMAKE_USER),true) #リソースコンバート対象者のみ、コンバートのルールを有効にする

# サフィックスルールを定義することで 依存関係を自動で判断してくれる
.SUFFIXES: .NCLR .ncl .NCGR .ncg .NSCR .nsc .NCER .nce

.ncl.NCLR:
	g2dcvtr $<

.ncg.NCGR:
	g2dcvtr $< -bg
	ntrcomp -l 2 -o $*_lz.NCGR $*.NCGR

.nsc.NSCR:
	g2dcvtr $<
	ntrcomp -l 2 -o $*_lz.NSCR $*.NSCR

.nce.NCER:
	g2dcvtr $< -br
	ntrcomp -l 2 -o $*_lz.NCGR $*.NCGR

endif

.PHONY:	do-build clean subdir

#------------------------------------------------------------------------------
#	make do-build ルール
#------------------------------------------------------------------------------
do-build:	subdir $(TARGETDIR)/$(targetname).narc

subdir:
	@$(MAKE_SUBDIR)

# narc のコピーだけ実行する
$(TARGETDIR)/$(targetname).narc: $(targetname).narc
	$(COPY) $(targetname).narc $(TARGETDIR)
	$(COPY) $(targetname).naix $(TARGETDIR)

ifeq	($(RESMAKE_USER),true) #リソースコンバート対象者のみ、元ファイルからコンバートするルールを有効にする
# コピー先のnarcが古い場合実行する
$(targetname).narc: $(lobjects) $(gobjects) $(sobjects) $(eobjects) $(aobjects)
	-ls *.NCLR *_lz.NCGR *_lz.NSCR *.NCER *.NANR > arclist.txt 
	perl $(PROJECT_ROOT)tools/arc_list_make.pl arclist.txt 
	nnsarc -c -l -n -i $(targetname) -S arclist.txt
	$(NAIXCUT) $(targetname).naix
	rm -f *.NCGR *.NCLR *.NSCR *.NCER *.NANR
endif

#------------------------------------------------------------------------------
#	make clean ルール
#※生成ファイルを削除するように記述を変更してください
#------------------------------------------------------------------------------
clean:
	@$(MAKE_SUBDIR)
ifeq	($(RESMAKE_USER),true) #リソースコンバート対象者のみ、中間ファイル削除
	rm -f *.narc
	rm -f *.naix
	rm -f $(targetname).narc
	rm -f $(targetname).naix
	rm -f arclist.txt
endif
	rm -f $(TARGETDIR)/$(targetname).narc
	rm -f $(TARGETDIR)/$(targetname).naix


