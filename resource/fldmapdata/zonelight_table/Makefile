#------------------------------------------------------------------------------
#  通信アイコンnarcを作るmakefile  
#  コピーしてtargetnameを書き換えれば汎用的に使えます
#  2008.08.19
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#※コンバートを行う人のユーザーIDをここに書く
#------------------------------------------------------------------------------
USERS	= tomoya_takahashi fuchino nakatsui obata_toshihiro #ohno #tamada

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------
targetname = field_zone_light

#------------------------------------------------------------------------------
#※コピー先へのパスを書く（通常はPROJECT_ARCDIRでよい）
#------------------------------------------------------------------------------
TARGETDIR = $(PROJECT_ARCDIR)fieldmap/

#------------------------------------------------------------------------------
#※サブディレクトリでもMakeしたい場合、ここにディレクトリ名を書く
#------------------------------------------------------------------------------
SUBDIRS	=

#共通ルールファイルのインクルード
include	$(PROJECT_RSCDIR)\macro_define

##########################################
#コンバートファイル
##########################################
NCLFILE = $(wildcard *.ncl)
NCEFILE = $(wildcard *.nce)
NSCFILE = $(wildcard *.nsc)
IMDFILE = $(wildcard *.imd)
ICAFILE = $(wildcard *.ica)
IVAFILE = $(wildcard *.iva)
IMAFILE = $(wildcard *.ima)
ITPFILE = $(wildcard *.itp)
ITAFILE = $(wildcard *.ita)

TXTFILE = $(wildcard *.txt)
XLSFILE = $(wildcard zone_*.xls)

LISTXLSFILE = light_list.xls
LISTCSVFILE = light_list.csv

#TIMEZONE
TIMEZONE_FILE = ../../field_light/light_timezone.lst

#ダミー
DUMMYFILE = dummy.txt

ZONE_HADER = ../zonetable/zone_id.h

##########################################
#ターゲットファイル
##########################################
#2D
NCLRFILE = $(NCLFILE:.ncl=.NCLR)
NCERFILE = $(NCEFILE:.nce=.NCER)
NSCRFILE = $(NSCFILE:.nsc=.NSCR)
	
#3D
NSBMDFILE = $(IMDFILE:.imd=.nsbmd)
NSBCAFILE = $(ICAFILE:.ica=.nsbca)
NSBVAFILE = $(IVAFILE:.iva=.nsbva)
NSBMAFILE = $(IMAFILE:.ima=.nsbma)
NSBTPFILE = $(ITPFILE:.itp=.nsbtp)
NSBTAFILE = $(ITAFILE:.ita=.nsbta)

#DAT
DATFILE = $(XLSFILE:.xls=.dat)

LIST_FILE 	= list.txt
OBJ_FILE 	= $(NCLRFILE) $(NCERFILE) $(NSCRFILE) \
			  $(NSBMDFILE) $(NSBCAFILE) $(NSBVAFILE) \
			  $(NSBMAFILE) $(NSBTPFILE) $(NSBTAFILE) \
			  $(DATFILE) $(LISTXLSFILE)



#------------------------------------------------------------------------------
#	#サフィックス
#	#コンバート対象者のみ、コンバートのルールを有効にする
#------------------------------------------------------------------------------
ifeq	($(CONVERTUSER),true)	


.SUFFIXES: .ncl .nce .nsc .NCLR .NCER .NSCR .imd .ica .iva .ima .itp .ita .nsbmd .nsbtx .nsbca .nsbma .nsbva .nsbtp .nsbta .xls .dat

#コンバート環境
#2d
.ncl.NCLR:
	g2dcvtr $<

.nce.NCER:
	g2dcvtr $< -br

.nsc.NSCR:
	g2dcvtr $<

#3d
.imd.nsbmd:
	g3dcvtr $<

.ica.nsbca:
	g3dcvtr $<

.iva.nsbva:
	g3dcvtr $<

.ima.nsbma:
	g3dcvtr $<

.itp.nsbtp:
	g3dcvtr $<

.ita.nsbta:
	g3dcvtr $<


#light
.xls.dat:
	$(PROJECT_ROOT)tools/exceltool/ExcelSeetConv.exe -o $*.csv -p 0 -s csv $<
	perl ../../field_light/conv.pl $*.csv $@ $(TIMEZONE_FILE)

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
$(targetname).narc: $(OBJ_FILE) $(TXTFILE)
	-ls *.dat > $(LIST_FILE)
	-ls *.N*R >> $(LIST_FILE)
	-ls *.nsb* >> $(LIST_FILE)

	$(PROJECT_ROOT)tools/exceltool/ExcelSeetConv.exe -o $(LISTCSVFILE) -p 0 -s csv $(LISTXLSFILE)
	perl table_conv.pl $(LISTCSVFILE) $(ZONE_HADER) $(LIST_FILE)
	-ls *.bin >> $(LIST_FILE)

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
	rm -f *.N*R
	rm -f *.nsb*
	rm -f *.dat
	rm -f *.csv
	rm -f *.narc
	rm -f *.naix
	rm -f $(targetname).narc
	rm -f $(targetname).naix
endif



