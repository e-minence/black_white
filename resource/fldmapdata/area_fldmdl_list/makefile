
#------------------------------------------------------------------------------
#※コンバート作業が必要なユーザーの名前を記述する
#------------------------------------------------------------------------------
USERS	=	saitou

#------------------------------------------------------------------------------
#※サブディレクトリでもMakeしたい場合、ここにディレクトリ名を書く
#------------------------------------------------------------------------------
SUBDIRS	=

#共通ルールファイルのインクルード
include	$(PROJECT_RSCDIR)\macro_define


LINCLUDES	= $(NITROSDK_ROOT)/include
LINCLUDES	+= $(dir $<)

INCPATH	= -I$(NITROSDK_ROOT)/include

#------------------------------------------------------------------------------
#ツール
#------------------------------------------------------------------------------

XLS_CNV = ../../../tools/exceltool/xls2xml/tab_out_sheet.rb
CSV_FILE = zonetable.csv

#------------------------------------------------------------------------------
#読み込むエクセル
#------------------------------------------------------------------------------
ZONE_XLS = ../zonetable/zonetable.xls

.PHONY:	do-build clean



do-build: check

check: $(CSV_FILE)
	ruby afml.rb $(CSV_FILE)

$(CSV_FILE): $(ZONE_XLS)
	ruby $(XLS_CNV) -c -n 0 $(ZONE_XLS) > $(CSV_FILE)

clean:



