
include $(PROJECT_RSCDIR)\macro_define

#複雑な依存関係があるため、順序を入れ替えないこと。
#zonetable/zoneindexが先頭、
#zonetableが末尾、という順番は変更してはいけない。

ifeq ($(SCRIPT_DEBUGGER_MODE),yes)

SUBDIRS	= flagwork \
					eventdata/groupdata \
					eventdata/define \
					script \
					eventdata/binary

else

SUBDIRS	=	flagwork \
					area_data \
					land_data \
					land_data_patch \
					camera_data \
					gimmick \
					map_matrix \
					map_replace \
					build_model \
					mm_list \
					eventdata/groupdata \
					eventdata/define \
					scr_cam_prm \
					script \
					effect \
					special_bgm \
					rail_data \
					zonefog_table \
					zonelight_table \
					fld_gimmick_assign \
					gimmick \
					camera_scroll \
					camera_scene \
					entrance_camera \
					pushrock \
					eventdata/binary \
					crowd_people \
					envse \
					timezone \
					diving_spot \
					zonetable/fieldskill_mapeffect \
					zonetable
endif
					

.PHONY:	do-build clean

do-build:
	@$(MAKE_SUBDIR)

clean:
	@$(MAKE_SUBDIR)

imd_check:
	ruby ../../tools/nix_check/map_nix_check.rb > result.txt

