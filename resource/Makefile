#------------------------------------------------------------------------------
#
#
#
#------------------------------------------------------------------------------
RESOURCE_MACRO	= $(PROJECT_RSCDIR)\macro_define

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#	環境が違うところでMakeできないようにするためのおまじない
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#現在のディレクトリ（cygwinのパスから変換する）
NOWDIR	=	$(shell pwd|tr [A-Z] [a-z] |sed -e 's/\/cygdrive\///')/
#環境変数が想定しているディレクトリから、：を取り除いたもの
TGTDIR	= $(shell echo $(PROJECT_RSCDIR)|tr [A-Z] [a-z]|sed -e 's/://')
ifneq ($(NOWDIR),$(TGTDIR))
$(warning コンパイル環境を確認してください)
$(error $(NOWDIR) != $(TGTDIR) )
endif

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


SUBDIRS	=	wmi \
			demo3d \
			tokusei\
			waza_tbl\
			itemconv\
			shooter_item \
			personal\
			trdata \
			trmsg \
			sound \
			effect_viewer \
			bag \
			test_layout \
			test_graphic\
			place_name \
			field_weather \
			field_weather_light \
			field_light \
			battle\
			fldmapdata/zonetable/zoneindex \
			zukan_data \
			research_radar \
			fld_trade \
            staff_roll \
			manual \
			message\
			fldmmdl \
			encount \
			rebattle_trainer \
			fld3d_ci \
			fldmapdata\
			pokegra\
			net_err\
			font\
			itemicon\
			title\
			mystery\
			startmenu \
			ircbattle \
			wificlub \
			wifi_login \
			wifi_union_obj \
			wifi_leading_obj \
			trainer_case \
			pms \
			pms_gra \
			compatible \
			musical \
			infowin \
			c_gear \
			cg_comm \
			gsync \
			gtsnego \
			bmpoam \
			shop \
			code_input \
			wazatype_icon \
			wifi_note \
			particle/wazaeffect \
			others \
			wazaeffect \
			field_menu \
			pokelist \
			wifi_earth \
			p_status \
			app_menu_common \
			irc_ranking \
			iss \
			townmap \
			unionroom \
			winframe \
			trgra \
			irc_compatible \
			worldtrade \
			calender \
			pokefoot \
			debug \
			config \
			narc_cpy \
			pokeicon \
            b_bag \
            b_plist \
		  regulation \
			mictest \
			pokemon_center \
			namein \
            box \
			battle_championship \
			trade \
			waza_oshie \
			sodateya \
			shopdata \
			fld_shortcut_menu \
			wifi_match \
			trtype_bgm \
			local_tvt \
			palace \
            op_demo \
            op_demo2 \
            gf_logo \
			battle_recorder \
			monolith \
			fourkings_scene \
			multiboot \
			field_wfbc_data/after \
			mail \
			season_display \
			zukan_gra \
			intro \
			regulation_card \
			comm_tvt \
			guru2 \
			guru2_2d \
			oekaki \
			beacon_status \
			tr_ai \
			enceff_imd \
			pl_boat \
			enc_ci_face \
			comm_btl_demo \
			shinka_demo \
			face_up \
			batt_rec_gra \
			sea_temple \
			egg_demo \
			kokuren \
			pokevoice \
			dowsing_gra \
			leader_board \
      report \
      time_icon \
      trialhouse_gra \
      shoujou \
			event_movepoke \
			psel \
			subway_map_gra \
			item_judge \
			p_tree \
			camera_sys \
			cross_comm \
			dendou_pc \
			btl_subway \
			dendou_demo \
			fld3d_ci_se \
			profanity_word \
			symbol \
			manual_image \
			pokeform \
			assert_err


include $(RESOURCE_MACRO)

.PHONY:	do-build	clean
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
do-build:
	@$(MAKE_SUBDIR)
	@$(ECHO_CURDIR)


#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
clean:
	@$(MAKE_SUBDIR)
	@$(ECHO_CURDIR)


