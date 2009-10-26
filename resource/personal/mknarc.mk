#------------------------------------------------------------------------------
#
#	ポケモンパーソナル用Makefile
#
#	2008.08.19	tamada@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#※コンバート作業が必要なユーザーの名前を記述する
#------------------------------------------------------------------------------
USERS	=	nishino

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------
PERSONAL	= personal.narc
WOTBL			= wotbl.narc
EVOTBL		= evo.narc
PMSTBL		= pms.narc
GROWTBL		= growtbl.narc

#------------------------------------------------------------------------------
#※コピー先へのパスを書く（通常はPROJECT_ARCDIRでよい）
#------------------------------------------------------------------------------
TARGETDIR	= $(PROJECT_ARCDIR)personal/

#------------------------------------------------------------------------------
#※サブディレクトリでもMakeしたい場合、ここにディレクトリ名を書く
#------------------------------------------------------------------------------
SUBDIRS	=

PERFILES:=$(wildcard per_*.s)
WOTFILES:=$(wildcard wot_*.s)
EVOFILES:=$(wildcard evo_*.s)
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
LDIRT_CLEAN = $(TARGETDIR)$(PERSONAL) $(TARGETDIR)$(WOTBL) $(TARGETDIR)$(EVOTBL) $(TARGETDIR)$(PMSTBL)
ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
LDIRT_CLEAN += $(PERSONAL) $(WOTBL) $(EVOTBL) $(PMSTBL) *.bin
endif

.PHONY:	do-build

ifeq	($(CONVERTUSER),true)	#コンバート対象者のみ、コンバートのルールを有効にする
.SUFFIXES: .s .bin

#パーソナルデータ生成
do-build: personal wot evo narc

personal: $(PERFILES:.s=.bin)
%.bin:%.s
	$(MWAS) $< -o $*.o
	$(MWLD) -dis -o $*.elf $*.o
	$(ELF2BIN) $*.elf
	rm $*.o
	rm $*.elf

wot: $(WOTFILES:.s=.bin)
%.bin:%.s
	$(MWAS) $< -o $*.o
	$(MWLD) -dis -o $*.elf $*.o
	$(ELF2BIN) $*.elf
	rm $*.o
	rm $*.elf

evo: $(EVOFILES:.s=.bin)
%.bin:%.s
	$(MWAS) $< -o $*.o
	$(MWLD) -dis -o $*.elf $*.o
	$(ELF2BIN) $*.elf
	rm $*.o
	rm $*.elf

narc:
personal.narc: $(PERFILES:.s=.bin)
	nnsarc -c -l -n personal.narc per_*.bin
wotbl.narc: $(WOTFILES:.s=.bin)
	nnsarc -c -l -n wotbl.narc wot_*.bin
evo.narc: $(EVOFILES:.s=.bin)
	nnsarc -c -l -n evo.narc evo_*.bin
pms.narc: $(PMSFILES)
	nnsarc -c -l -n pms.narc pms_*.bin

endif

#------------------------------------------------------------------------------
#	make do-build ルール
#------------------------------------------------------------------------------
do-build:	$(TARGETDIR)$(PERSONAL) $(TARGETDIR)$(WOTBL) $(TARGETDIR)$(EVOTBL) $(TARGETDIR)$(PMSTBL) $(TARGETDIR)$(GROWTBL)

$(TARGETDIR)$(PERSONAL):	$(PERSONAL)
	$(COPY)	$(PERSONAL) $(TARGETDIR)

$(TARGETDIR)$(WOTBL):	$(WOTBL)
	$(COPY)	$(WOTBL) $(TARGETDIR)

$(TARGETDIR)$(EVOTBL):	$(EVOTBL)
	$(COPY)	$(EVOTBL) $(TARGETDIR)

$(TARGETDIR)$(PMSTBL):	$(PMSTBL)
	$(COPY)	$(PMSTBL) $(TARGETDIR)

$(TARGETDIR)$(GROWTBL):	$(GROWTBL)
	$(COPY)	$(GROWTBL) $(TARGETDIR)

