
#------------------------------------------------------------------------------
#※コンバート作業が必要なユーザーの名前を記述する
#------------------------------------------------------------------------------
USERS	=	saitou tanoue_reiko unno fuchino

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------
targetname = gym_dragon

#------------------------------------------------------------------------------
#※コピー先へのパスを書く（通常はPROJECT_ARCDIRでよい）
#------------------------------------------------------------------------------
FLDMAP = fieldmap
TARGETDIR = $(PROJECT_ARCDIR)$(FLDMAP)

#------------------------------------------------------------------------------
#※サブディレクトリでもMakeしたい場合、ここにディレクトリ名を書く
#------------------------------------------------------------------------------
SUBDIRS	=

#DEF_HEADER = gym_dragon_local_def.h

#共通ルールファイルのインクルード
include	$(PROJECT_RSCDIR)\macro_define

include data_list

#------------------------------------------------------------------------------
#データ
#------------------------------------------------------------------------------
ARCLIST = arc_list.txt

#LDIRT_CLEAN = $(TARGETDIR)/$(targetname).narc $(TARGETDIR)/$(targetname).naix
#コンバート対象者のみ、中間ファイル削除
#ifeq	($(CONVERTUSER),true)	
#LDIRT_CLEAN += $(targetname).narc $(targetname).naix arc_result.txt *.nsb* *.bin
#endif

LINCLUDES	= $(NITROSDK_ROOT)/include
LINCLUDES	+= $(dir $<)

INCPATH	= -I$(NITROSDK_ROOT)/include

#------------------------------------------------------------------------------
#ツール
#------------------------------------------------------------------------------

DEF_MAKE = ../def_write.rb
XLS_CNV = ../../../../tools/exceltool/xls2xml/tab_out.rb

.PHONY:	do-build clean



do-build: $(TARGETDIR)/$(targetname).narc

clean:
	@rm -f $(TARGETDIR)/$(targetname).narc
	@rm -f $(TARGETDIR)/$(targetname).naix
ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ動作させる
	@rm -f $(targetname).narc
	@rm -f $(targetname).naix
	@rm -f *.nsb*
	@rm -f *.bin
endif


#コピー実行する
$(TARGETDIR)/$(targetname).narc:  $(targetname).narc 
	@echo copying $(targetname).narc
	@$(COPY) $(targetname).narc $(TARGETDIR)
	@$(COPY) $(targetname).naix $(TARGETDIR)

#コンバート対象者のみ、元ファイルからコンバートするルールを有効にする
ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ動作させる
#.SUFFIXES:	.nsbmd .imd
#imdデータからの生成ルール
%.nsbmd:	data/%.imd
#	@echo $<をコンバート
	@g3dcvtr $<

%.nsbca:	data/%.ica
#	@echo $<をコンバート
	@g3dcvtr $<

%.nsbma:	data/%.ima
#	@echo $<をコンバート
	@g3dcvtr $<

%.nsbta:	data/%.ita
#	@echo $<をコンバート
	@g3dcvtr $<

%.nsbtp:	data/%.itp
#	@echo $<をコンバート
	@g3dcvtr $<

# コピー先のnarcが古い場合実行する
$(targetname).narc: $(DATA_LIST)
	@nnsarc -i -cl -l -n $(targetname).narc -S $(ARCLIST) > arc_result.txt
#	@-rm -f *.nsb*
#	@-rm -f *.bin

#$(DEF_HEADER): def_make.xls
#	ruby $(XLS_CNV) -c def_make.xls > def_make.csv
#	ruby $(DEF_MAKE) def_make.csv $(DEF_HEADER)
endif
