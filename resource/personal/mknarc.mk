#------------------------------------------------------------------------------
#
#	ポケモンパーソナル用Makefile
#
#	2008.08.19	tamada@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#※コンバート作業が必要なユーザーの名前を記述する
#------------------------------------------------------------------------------
include user.def

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------
PERSONAL	= personal.narc
PERSONAL_NAIX	= personal.naix
PERSONAL_CSV	= personal_wb_fix.csv
WOTBL			= wotbl.narc
EVOTBL		= evo.narc
PMSTBL		= pms.narc
GROWTBL		= growtbl.narc
ICONSCR		= pokeicon_all.scr
ICONATTR	= attr_list.txt

#------------------------------------------------------------------------------
#※コピー先へのパスを書く（通常はPROJECT_ARCDIRでよい）
#------------------------------------------------------------------------------
TARGETDIR	= $(PROJECT_ARCDIR)personal/
POKEICONDIR	= ../pokeicon/graphic/

#------------------------------------------------------------------------------
#※サブディレクトリでもMakeしたい場合、ここにディレクトリ名を書く
#------------------------------------------------------------------------------
SUBDIRS	=

PERFILES:=$(wildcard per_*.bin)
WOTFILES:=$(wildcard wot_*.bin)
EVOFILES:=$(wildcard evo_*.bin)
PMSFILES:=$(wildcard pms_*.bin)

ELF2BIN = ../../tools/elf2bin.exe
PMSEED = ../../tools/personal_conv/pmseed.exe

#共通ルールファイルのインクルード
include $(PROJECT_RSCDIR)\macro_define
#MWAS,MWLD
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs
include	$(COMMONDEFS_CCTYPE_CW)
#Moduleルール
include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#------------------------------------------------------------------------------
#	ファイル削除ルール
#------------------------------------------------------------------------------
LDIRT_CLEAN = $(TARGETDIR)$(PERSONAL) $(TARGETDIR)$(PERSONAL_NAIX) $(TARGETDIR)$(WOTBL) $(TARGETDIR)$(EVOTBL) $(TARGETDIR)$(PMSTBL)
ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
LDIRT_CLEAN += $(PERSONAL) $(PERSONAL_NAIX) $(WOTBL) $(EVOTBL) $(PMSTBL) $(PERSONAL_CSV) *.bin
endif

.PHONY:	do-build fake

ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
.SUFFIXES: .s .bin

#パーソナルデータ生成
do-build: narc #$(PERSONAL_CSV)

narc:
personal.narc: $(PERFILES)
	nnsarc -c -l -n -i personal.narc per_*.bin zenkoku_to_chihou.bin
wotbl.narc: $(WOTFILES)
	nnsarc -c -l -n wotbl.narc wot_*.bin
evo.narc: $(EVOFILES)
	nnsarc -c -l -n evo.narc evo_*.bin
pms.narc: $(PMSFILES)
	nnsarc -c -l -n pms.narc pms_*.bin

#$(PERSONAL_CSV): personal_wb.csv
#	ruby ../../tools/personal_conv/wazamachine_split.rb personal_wb.csv $(PERSONAL_CSV)
endif

#------------------------------------------------------------------------------
#	make do-build ルール
#------------------------------------------------------------------------------
do-build:	$(TARGETDIR)$(PERSONAL) $(TARGETDIR)$(PERSONAL_NAIX) $(TARGETDIR)$(WOTBL) $(TARGETDIR)$(EVOTBL) $(TARGETDIR)$(PMSTBL) $(TARGETDIR)$(GROWTBL) $(POKEICONDIR)$(ICONSCR) $(POKEICONDIR)$(ICONATTR)

$(TARGETDIR)$(PERSONAL):	$(PERSONAL)
	$(COPY)	$(PERSONAL) $(TARGETDIR)

$(TARGETDIR)$(PERSONAL_NAIX):	$(PERSONAL_NAIX)
	$(COPY)	$(PERSONAL_NAIX) $(TARGETDIR)

$(TARGETDIR)$(WOTBL):	$(WOTBL)
	$(COPY)	$(WOTBL) $(TARGETDIR)

$(TARGETDIR)$(EVOTBL):	$(EVOTBL)
	$(COPY)	$(EVOTBL) $(TARGETDIR)

$(TARGETDIR)$(PMSTBL):	$(PMSTBL)
	$(COPY)	$(PMSTBL) $(TARGETDIR)

$(TARGETDIR)$(GROWTBL):	$(GROWTBL)
	$(COPY)	$(GROWTBL) $(TARGETDIR)

$(POKEICONDIR)$(ICONSCR):	$(ICONSCR)
	$(COPY)	$(ICONSCR) $(POKEICONDIR)

$(POKEICONDIR)$(ICONATTR):	$(ICONATTR)
	$(COPY)	$(ICONATTR) $(POKEICONDIR)

fake:
	nnsarc -c -l -n -i personal.narc per_*.bin zenkoku_to_chihou.bin
	nnsarc -c -l -n wotbl.narc wot_*.bin
	nnsarc -c -l -n evo.narc evo_*.bin
	nnsarc -c -l -n pms.narc pms_*.bin
	$(COPY)	$(PERSONAL) $(TARGETDIR)
	$(COPY)	$(PERSONAL_NAIX) $(TARGETDIR)
	$(COPY)	$(WOTBL) $(TARGETDIR)
	$(COPY)	$(EVOTBL) $(TARGETDIR)
	$(COPY)	$(PMSTBL) $(TARGETDIR)

