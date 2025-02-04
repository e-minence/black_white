#------------------------------------------------------------------------------
#  通信アイコンnarcを作るmakefile  
#  コピーしてtargetnameを書き換えれば汎用的に使えます
#  2008.08.19
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#※コンバートを行う人のユーザーIDをここに書く
#------------------------------------------------------------------------------
USERS	= tomoya_takahashi fuchino lee_hyunjung #ohno #tamada

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------
targetname = field_light

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

##########################################
#コンバートファイル
##########################################
XLSFILE = $(wildcard *.xls)

##########################################
#ターゲットファイル
##########################################

#CSV
CSV_FILE	= $(XLSFILE:.xls=.csv)

#TIMEZONE
TIMEZONE_FILE = light_timezone.lst

#ダミー
DUMMYFILE = dummy.txt

LIST_FILE 	= list.txt



#------------------------------------------------------------------------------
#	#サフィックス
#	#コンバート対象者のみ、コンバートのルールを有効にする
#------------------------------------------------------------------------------
ifeq	($(CONVERTUSER),true)	


.SUFFIXES: .xls .csv

#コンバート環境
.xls.csv:
	echo > $@	#ダミー中間ファイル生成
	$(PROJECT_ROOT)tools/exceltool/ExcelSeetConv.exe -o $*_00.csv -p 1 -s csv $<
	$(PROJECT_ROOT)tools/exceltool/ExcelSeetConv.exe -o $*_01.csv -p 2 -s csv $<
	$(PROJECT_ROOT)tools/exceltool/ExcelSeetConv.exe -o $*_02.csv -p 3 -s csv $<
	$(PROJECT_ROOT)tools/exceltool/ExcelSeetConv.exe -o $*_03.csv -p 4 -s csv $<

	perl conv.pl $*_00.csv $*_00.dat $(TIMEZONE_FILE)
	perl conv.pl $*_01.csv $*_01.dat $(TIMEZONE_FILE)
	perl conv.pl $*_02.csv $*_02.dat $(TIMEZONE_FILE)
	perl conv.pl $*_03.csv $*_03.dat $(TIMEZONE_FILE)

	cp $*_00.csv $*.csv	#ダミー

endif



.PHONY:	do-build clean subdir

#------------------------------------------------------------------------------
#	make do-build ルール
#------------------------------------------------------------------------------
do-build:	subdir $(DUMMYFILE) $(TARGETDIR)/$(targetname).narc

subdir:
	@$(MAKE_SUBDIR)


# narc のコピーだけ実行する
$(TARGETDIR)/$(targetname).narc: $(targetname).narc
	$(COPY) $(targetname).narc $(TARGETDIR)
	$(COPY) $(targetname).naix $(TARGETDIR)


#コンバート対象者のみ、元ファイルからコンバートするルールを有効にする
ifeq	($(CONVERTUSER),true)	

$(DUMMYFILE):$(TIMEZONE_FILE)
	-ls dummy > $(DUMMYFILE)
	rm -f *.dat
	rm -f *.csv

# コピー先のnarcが古い場合実行する
$(targetname).narc: $(CSV_FILE) $(TXTFILE)
	-ls *.dat > $(LIST_FILE)
	$(ARCLIST_MAKE) $(LIST_FILE)
	nnsarc -c -l -n -i $(targetname) -S $(LIST_FILE)
	$(NAIXCUT) $(targetname).naix
	-rm	$(LIST_FILE)

else

$(DUMMYFILE):$(TIMEZONE_FILE)
	-ls dummy > $(DUMMYFILE)

endif

#------------------------------------------------------------------------------
#	make clean ルール
#※生成ファイルを削除するように記述を変更してください
#------------------------------------------------------------------------------
clean:
	@$(MAKE_SUBDIR)
	rm -f $(TARGETDIR)/$(targetname).narc
	rm -f $(TARGETDIR)/$(targetname).naix

#コンバート対象者のみ、中間ファイル削除
ifeq	($(CONVERTUSER),true)	
	rm -f *.dat
	rm -f *.csv
	rm -f *.narc
	rm -f *.naix
	rm -f $(targetname).narc
	rm -f $(targetname).naix
endif



