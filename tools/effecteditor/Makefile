#------------------------------------------------------------------------------
#
#	技エフェクト用Makefile
#
#	2009.02.13	sogabe@gamefreak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
#※コンバート作業が必要なユーザーの名前を記述する
#------------------------------------------------------------------------------
USERS	=	

#------------------------------------------------------------------------------
#※ここに作成するnarc名を書く
#------------------------------------------------------------------------------
targetname	= waza_eff
NARCNAME	= $(targetname).narc
NAIXNAME	= $(targetname).naix

EE_INCLUDE_DIR = ../../

#------------------------------------------------------------------------------
#※コピー先へのパスを書く（通常はPROJECT_ARCDIRでよい）
#------------------------------------------------------------------------------
TARGETDIR	= $(PROJECT_ARCDIR)wazaeffect

#------------------------------------------------------------------------------
#※サブディレクトリでもMakeしたい場合、ここにディレクトリ名を書く
#------------------------------------------------------------------------------
SUBDIRS	=

#共通ルールファイルのインクルード
include $(PROJECT_RSCDIR)/macro_define
#include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs
include	$(COMMONDEFS_CCTYPE_CW)
include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#リソースファイルの存在するディレクトリを指定
RES_DIR = ./

#コンバートしたファイルを格納するディレクトリを指定
CNV_DIR = ./

#コンバート対象のファイルツリーを生成
WAZAEFFSRCS	:= $(wildcard $(RES_DIR)*.s)

.PHONY:	do-build clean subdir

.SUFFIXES:	.s

do-build: $(notdir $(WAZAEFFSRCS:.s=.bin))
%.bin: %.s ../../prog/src/battle/btlv/btlv_effvm_def.h
	"$(CWFOLDER_TWL)ARM_Tools/Command_Line_Tools/mwasmarm.exe" $< -o $*.o
	"$(CWFOLDER_TWL)ARM_Tools/Command_Line_Tools/mwldarm.exe" -dis -o $*.elf $*.o
	../elf2bin $*.elf

