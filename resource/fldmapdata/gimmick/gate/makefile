#------------------------------------------------------------------------------
#  narcを作るmakefile  
#  コピーしてtargetnameを書き換えれば汎用的に使えます
#  2008.08.19
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#※コンバートを行う人のユーザーIDをここに書く
#------------------------------------------------------------------------------
USERS	= obata_toshihiro murakami_naoto fuchino 

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------
targetname_gate   = gate
targetname_zone   = zone
targetname_spnews = spnews

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

ICACONV             = ..\..\..\..\tools\ica\ica_converter.rb
XLSCONV             = ../../../../tools/exceltool/xls2xml/tab_out.rb
GRAPHIC_SOURCE      = ./graphic
GRAPHIC_BIN         = ./graphic/bin
SETTING_SOURCE      = ./setting
SETTING_BIN         = ./setting/bin
BIN_ZONE_DATA       = $(SETTING_BIN)/zone_data
BIN_SPNEWS_DATA     = $(SETTING_BIN)/spnews_data
ELBOARD_ZONE_DATA   = $(BIN_ZONE_DATA)/*.bin 
ELBOARD_SPNEWS_DATA = $(BIN_SPNEWS_DATA)/*.bin

CONVERTDATA_GATE   = $(GRAPHIC_BIN)/gelboard01.nsbmd \
							       $(GRAPHIC_BIN)/gelboard01.nsbtx \
							       $(GRAPHIC_BIN)/gelboard01_1.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_2.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_3.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_4.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_5.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_6.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_7.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_fast_1.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_fast_2.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_fast_3.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_fast_4.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_fast_5.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_fast_6.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_fast_7.nsbta \
							       $(GRAPHIC_BIN)/gelboard01_tv00.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv01.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv02.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv03.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv04.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv05.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv06.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv07.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv08.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv09.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv10.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv11.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv12.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv13.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv14.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv15.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv16.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv17.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv18.nsbtp \
							       $(GRAPHIC_BIN)/gelboard01_tv19.nsbtp
CONVERTDATA_ZONE   = $(ELBOARD_ZONE_DATA)
CONVERTDATA_SPNEWS = $(ELBOARD_SPNEWS_DATA)

ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
# サフィックスルールを定義することで 依存関係を自動で判断してくれる
.SUFFIXES: 
endif

.PHONY:	do-build clean subdir

#------------------------------------------------------------------------------
#	make do-build ルール
#------------------------------------------------------------------------------
do-build:	subdir $(TARGETDIR)/$(targetname_gate).narc $(TARGETDIR)/$(targetname_zone).narc $(TARGETDIR)/$(targetname_spnews).narc

subdir:
	@$(MAKE_SUBDIR)

ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
# コンバート
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01.imd -emdl -o $(GRAPHIC_BIN)/gelboard01.nsbmd
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01.imd -etex -o $(GRAPHIC_BIN)/gelboard01.nsbtx
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_1.ita -o $(GRAPHIC_BIN)/gelboard01_1.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_2.ita -o $(GRAPHIC_BIN)/gelboard01_2.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_3.ita -o $(GRAPHIC_BIN)/gelboard01_3.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_4.ita -o $(GRAPHIC_BIN)/gelboard01_4.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_5.ita -o $(GRAPHIC_BIN)/gelboard01_5.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_6.ita -o $(GRAPHIC_BIN)/gelboard01_6.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_7.ita -o $(GRAPHIC_BIN)/gelboard01_7.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_fast_1.ita -o $(GRAPHIC_BIN)/gelboard01_fast_1.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_fast_2.ita -o $(GRAPHIC_BIN)/gelboard01_fast_2.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_fast_3.ita -o $(GRAPHIC_BIN)/gelboard01_fast_3.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_fast_4.ita -o $(GRAPHIC_BIN)/gelboard01_fast_4.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_fast_5.ita -o $(GRAPHIC_BIN)/gelboard01_fast_5.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_fast_6.ita -o $(GRAPHIC_BIN)/gelboard01_fast_6.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_fast_7.ita -o $(GRAPHIC_BIN)/gelboard01_fast_7.nsbta
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv00.itp -o $(GRAPHIC_BIN)/gelboard01_tv00.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv01.itp -o $(GRAPHIC_BIN)/gelboard01_tv01.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv02.itp -o $(GRAPHIC_BIN)/gelboard01_tv02.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv03.itp -o $(GRAPHIC_BIN)/gelboard01_tv03.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv04.itp -o $(GRAPHIC_BIN)/gelboard01_tv04.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv05.itp -o $(GRAPHIC_BIN)/gelboard01_tv05.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv06.itp -o $(GRAPHIC_BIN)/gelboard01_tv06.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv07.itp -o $(GRAPHIC_BIN)/gelboard01_tv07.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv08.itp -o $(GRAPHIC_BIN)/gelboard01_tv08.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv09.itp -o $(GRAPHIC_BIN)/gelboard01_tv09.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv10.itp -o $(GRAPHIC_BIN)/gelboard01_tv10.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv11.itp -o $(GRAPHIC_BIN)/gelboard01_tv11.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv12.itp -o $(GRAPHIC_BIN)/gelboard01_tv12.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv13.itp -o $(GRAPHIC_BIN)/gelboard01_tv13.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv14.itp -o $(GRAPHIC_BIN)/gelboard01_tv14.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv15.itp -o $(GRAPHIC_BIN)/gelboard01_tv15.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv16.itp -o $(GRAPHIC_BIN)/gelboard01_tv16.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv17.itp -o $(GRAPHIC_BIN)/gelboard01_tv17.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv18.itp -o $(GRAPHIC_BIN)/gelboard01_tv18.nsbtp
	g3dcvtr $(GRAPHIC_SOURCE)/gelboard01_tv19.itp -o $(GRAPHIC_BIN)/gelboard01_tv19.nsbtp
# タブ区切り
$(SETTING_SOURCE)/elboard_zone_data.txt: $(SETTING_SOURCE)/elboard_zone_data.xls
	ruby $(XLSCONV) $< > $@ 
$(SETTING_SOURCE)/elboard_spnews_data.txt: $(SETTING_SOURCE)/elboard_spnews_data.xls
	ruby $(XLSCONV) $< > $@ 
endif


# narc のコピーだけ実行する
$(TARGETDIR)/$(targetname_gate).narc: $(targetname_gate).narc
	$(COPY) $(targetname_gate).narc $(TARGETDIR)
	$(COPY) $(targetname_gate).naix $(TARGETDIR) 

$(TARGETDIR)/$(targetname_zone).narc: $(targetname_zone).narc
	$(COPY) $(targetname_zone).narc $(TARGETDIR)
	$(COPY) $(targetname_zone).naix $(TARGETDIR) 

$(TARGETDIR)/$(targetname_spnews).narc: $(targetname_spnews).narc
	$(COPY) $(targetname_spnews).narc $(TARGETDIR)

ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、元ファイルからコンバートするルールを有効にする
# コピー先のnarcが古い場合実行する
$(targetname_gate).narc: $(CONVERTDATA_GATE)
	nnsarc -r -l -n -i $(targetname_gate) $^
	$(NAIXCUT) $(targetname_gate).naix

$(targetname_zone).narc: $(SETTING_SOURCE)/elboard_zone_data.txt
	rm -f $(CONVERTDATA_ZONE)
	ruby $(SETTING_SOURCE)/elboard_zone_data_conv.rb $< $(BIN_ZONE_DATA) 
	nnsarc -r -l -n -i $(targetname_zone) $(CONVERTDATA_ZONE)
	$(NAIXCUT) $(targetname_zone).naix

$(targetname_spnews).narc: $(SETTING_SOURCE)/elboard_spnews_data.txt
	rm -f $(CONVERTDATA_SPNEWS)
	ruby $(SETTING_SOURCE)/elboard_spnews_data.rb $< $(BIN_SPNEWS_DATA)
	nnsarc -r -l -n $(targetname_spnews) $(CONVERTDATA_SPNEWS)

endif



#------------------------------------------------------------------------------
#	make clean ルール
#※生成ファイルを削除するように記述を変更してください
#------------------------------------------------------------------------------
clean:
	@$(MAKE_SUBDIR)
ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、中間ファイル削除
	rm -f $(CONVERTDATA_GATE)
	rm -f $(CONVERTDATA_ZONE)
	rm -f $(CONVERTDATA_SPNEWS)
	rm -f $(targetname_gate).narc
	rm -f $(targetname_gate).naix
	rm -f $(targetname_zone).narc
	rm -f $(targetname_zone).naix
	rm -f $(targetname_spnews).narc
endif
	rm -f $(TARGETDIR)$(targetname_gate).narc
	rm -f $(TARGETDIR)$(targetname_gate).naix
	rm -f $(TARGETDIR)$(targetname_zone).narc
	rm -f $(TARGETDIR)$(targetname_zone).naix
	rm -f $(TARGETDIR)$(targetname_spnews).narc


