//======================================================================
/**
 * @file  def_cmd.h
 * @brief スクリプトコマンド定義
 * @date  2009.09.17
 * @author  tamada GAME FREAK inc.
 *
 * @todo  scrcmd.cのコマンド列とあわせる必要があるので、自動生成にしたい
 */
//======================================================================
//======================================================================
//  macro
//======================================================================
  .macro  INIT_CMD
DEF_CMD_COUNT  =  0
  .endm
    
  .macro  DEF_CMD symname
\symname  =  DEF_CMD_COUNT
DEF_CMD_COUNT  =  ( DEF_CMD_COUNT + 1 )
  .endm

//======================================================================
//  命令シンボル宣言
//======================================================================
  INIT_CMD
  //基本システム命令
  DEF_CMD EV_SEQ_NOP
  DEF_CMD EV_SEQ_DUMMY
  DEF_CMD EV_SEQ_END
  DEF_CMD EV_SEQ_TIME_WAIT
  DEF_CMD EV_SEQ_CALL
  DEF_CMD EV_SEQ_RET
  
  //判定演算用命令
  DEF_CMD EV_SEQ_PUSH_VALUE
  DEF_CMD EV_SEQ_PUSH_WORK
  DEF_CMD EV_SEQ_POP_WORK
  DEF_CMD EV_SEQ_POP
  DEF_CMD EV_SEQ_CALC_ADD
  DEF_CMD EV_SEQ_CALC_SUB
  DEF_CMD EV_SEQ_CALC_MUL
  DEF_CMD EV_SEQ_CALC_DIV
  DEF_CMD EV_SEQ_PUSH_FLAG
  DEF_CMD EV_SEQ_CMP_STACK

  //データロード・ストア関連
  DEF_CMD EV_SEQ_LD_REG_VAL
  DEF_CMD EV_SEQ_LD_REG_WDATA
  DEF_CMD EV_SEQ_LD_REG_ADR
  DEF_CMD EV_SEQ_LD_ADR_VAL
  DEF_CMD EV_SEQ_LD_ADR_REG
  DEF_CMD EV_SEQ_LD_REG_REG
  DEF_CMD EV_SEQ_LD_ADR_ADR
  
  //比較命令
  DEF_CMD EV_SEQ_CP_REG_REG
  DEF_CMD EV_SEQ_CP_REG_VAL
  DEF_CMD EV_SEQ_CP_REG_ADR
  DEF_CMD EV_SEQ_CP_ADR_REG
  DEF_CMD EV_SEQ_CP_ADR_VAL
  DEF_CMD EV_SEQ_CP_ADR_ADR
  DEF_CMD EV_SEQ_CP_WK_VAL
  DEF_CMD EV_SEQ_CP_WK_WK
  
  //仮想マシン関連
  DEF_CMD EV_SEQ_VM_ADD
  DEF_CMD EV_SEQ_CHG_COMMON_SCR
  DEF_CMD EV_SEQ_CHG_LOCAL_SCR
  
  //分岐命令
  DEF_CMD EV_SEQ_JUMP
  DEF_CMD EV_SEQ_IF_JUMP
  DEF_CMD EV_SEQ_IF_CALL
  DEF_CMD EV_SEQ_OBJ_ID_JUMP
  DEF_CMD EV_SEQ_BG_ID_JUMP
  DEF_CMD EV_SEQ_PLAYER_DIR_JUMP
  
  //イベントワーク関連
  DEF_CMD EV_SEQ_FLAG_SET
  DEF_CMD EV_SEQ_FLAG_RESET
  DEF_CMD EV_SEQ_FLAG_CHECK
  DEF_CMD EV_SEQ_FLAG_CHECK_WK
  DEF_CMD EV_SEQ_FLAG_SET_WK
  
  //ワーク操作関連
  DEF_CMD EV_SEQ_ADD_WK
  DEF_CMD EV_SEQ_SUB_WK
  DEF_CMD EV_SEQ_LD_WK_VAL
  DEF_CMD EV_SEQ_LD_WK_WK
  DEF_CMD EV_SEQ_LD_WK_WKVAL
  
  //フィールドイベント共通処理
  DEF_CMD EV_SEQ_COMMON_PROC_FIELD_EVENT_START
  DEF_CMD EV_SEQ_COMMON_PROC_FIELD_EVENT_END

  //キー入力関連
  DEF_CMD EV_SEQ_ABKEYWAIT
  DEF_CMD EV_SEQ_LASTKEYWAIT
  
  //会話ウィンドウ
  DEF_CMD EV_SEQ_TALKMSG
  DEF_CMD EV_SEQ_TALKMSG_ALLPUT
  DEF_CMD EV_SEQ_TALKWIN_OPEN
  DEF_CMD EV_SEQ_TALKWIN_CLOSE
  
  //フィールド　吹き出しウィンドウ
  DEF_CMD EV_SEQ_BALLOONWIN_OBJMSG_OPEN
  DEF_CMD EV_SEQ_BALLOONWIN_TALKOBJ_OPEN
  DEF_CMD EV_SEQ_BALLOONWIN_CLOSE
  
  //動作モデル
  DEF_CMD EV_SEQ_OBJ_ANIME      
  DEF_CMD EV_SEQ_OBJ_ANIME_WAIT
  DEF_CMD EV_SEQ_MOVE_CODE_GET
  DEF_CMD EV_SEQ_OBJ_POS_GET
  DEF_CMD EV_SEQ_PLAYER_POS_GET
  DEF_CMD EV_SEQ_OBJ_ADD
  DEF_CMD EV_SEQ_OBJ_DEL
  DEF_CMD EV_SEQ_OBJ_ADD_EV
  DEF_CMD EV_SEQ_OBJ_DEL_EV
	DEF_CMD EV_SEQ_OBJ_POS_CHANGE

  //動作モデル　イベント関連
  DEF_CMD EV_SEQ_OBJ_TURN
  
  //はい、いいえ　処理
  DEF_CMD EV_SEQ_YES_NO_WIN
  
  //WORDSET関連
  DEF_CMD EV_SEQ_PLAYER_NAME
  DEF_CMD EV_SEQ_ITEM_NAME
  DEF_CMD EV_SEQ_ITEM_WAZA_NAME
  DEF_CMD EV_SEQ_WAZA_NAME
  DEF_CMD EV_SEQ_POCKET_NAME

  //視線トレーナー関連
  DEF_CMD EV_SEQ_EYE_TRAINER_MOVE_SET
  DEF_CMD EV_SEQ_EYE_TRAINER_MOVE_SINGLE
  DEF_CMD EV_SEQ_EYE_TRAINER_MOVE_DOUBLE
  DEF_CMD EV_SEQ_EYE_TRAINER_TYPE_GET
  DEF_CMD EV_SEQ_EYE_TRAINER_ID_GET
  
  //トレーナーバトル関連
  DEF_CMD EV_SEQ_TRAINER_ID_GET
  DEF_CMD EV_SEQ_TRAINER_BTL_SET
  DEF_CMD EV_SEQ_TRAINER_MULTI_BTL_SET
  DEF_CMD EV_SEQ_TRAINER_MSG_SET
  DEF_CMD EV_SEQ_TRAINER_TALK_TYPE_GET
  DEF_CMD EV_SEQ_REVENGE_TRAINER_TALK_TYPE_GET
  DEF_CMD EV_SEQ_TRAINER_TYPE_GET
  DEF_CMD EV_SEQ_TRAINER_BGM_SET
  DEF_CMD EV_SEQ_LOSE
  DEF_CMD EV_SEQ_LOSE_CHECK
  DEF_CMD EV_SEQ_SEACRET_POKE_RETRY_CHECK
  DEF_CMD EV_SEQ_HAIFU_POKE_RETRY_CHECK
  DEF_CMD EV_SEQ_2VS2_BATTLE_CHECK
  DEF_CMD EV_SEQ_DEBUG_BTL_SET
  DEF_CMD EV_SEQ_BATTLE_RESULT_GET
  
  //トレーナーフラグ関連
  DEF_CMD EV_SEQ_TRAINER_FLAG_SET
  DEF_CMD EV_SEQ_TRAINER_FLAG_RESET
  DEF_CMD EV_SEQ_TRAINER_FLAG_CHECK
  
  //BGM
  DEF_CMD EV_SEQ_BGM_PLAY
  DEF_CMD EV_SEQ_BGM_STOP
  DEF_CMD EV_SEQ_BGM_PLAYER_PAUSE
  DEF_CMD EV_SEQ_BGM_PLAY_CHECK
  DEF_CMD EV_SEQ_BGM_FADEOUT
  DEF_CMD EV_SEQ_BGM_FADEIN
  DEF_CMD EV_SEQ_BGM_NOW_MAP_RECOVER
  
  //SE
  DEF_CMD EV_SEQ_SE_PLAY
  DEF_CMD EV_SEQ_SE_STOP
  DEF_CMD EV_SEQ_SE_WAIT
  
  //ME
  DEF_CMD EV_SEQ_ME_PLAY
  DEF_CMD EV_SEQ_ME_WAIT
  
  //メニュー
  DEF_CMD EV_SEQ_BMPMENU_INIT
  DEF_CMD EV_SEQ_BMPMENU_INIT_EX
  DEF_CMD EV_SEQ_BMPMENU_MAKE_LIST
  DEF_CMD EV_SEQ_BMPMENU_START
  
  //画面フェード
  DEF_CMD EV_SEQ_DISP_FADE_START
  DEF_CMD EV_SEQ_DISP_FADE_CHECK
  
  //アイテム関連
  DEF_CMD	EV_SEQ_ADD_ITEM
	DEF_CMD	EV_SEQ_SUB_ITEM
	DEF_CMD	EV_SEQ_ADD_ITEM_CHK
	DEF_CMD	EV_SEQ_ITEM_CHK
  DEF_CMD	EV_SEQ_GET_ITEM_NUM
	DEF_CMD	EV_SEQ_WAZA_ITEM_CHK
  DEF_CMD EV_SEQ_GET_POCKET_ID

  //ミュージカル関連
  DEF_CMD EV_SEQ_MUSICAL_CALL

  //マップ遷移関連
  DEF_CMD EV_SEQ_MAP_CHANGE_SAND_STREAM
  DEF_CMD EV_SEQ_MAP_CHANGE

  //ジムギミック関連
  DEF_CMD EV_SEQ_GYM_ELEC_INIT
  DEF_CMD EV_SEQ_GYM_ELEC_PUSH_SW
  DEF_CMD EV_SEQ_GYM_NORMAL_INIT
  DEF_CMD EV_SEQ_GYM_NORMAL_UNROCK
  DEF_CMD EV_SEQ_GYM_NORMAL_CHK_UNROCK
  DEF_CMD EV_SEQ_GYM_NORMAL_MOVE_WALL

  //その他
  DEF_CMD EV_SEQ_CHG_LANGID
  DEF_CMD EV_SEQ_GET_RND
  DEF_CMD EV_SEQ_GET_NOW_MSG_ARCID
  DEF_CMD EV_SEQ_CHECK_TEMOTI_POKERUS
  DEF_CMD EV_SEQ_GET_TIMEZONE
  DEF_CMD EV_SEQ_GET_TRAINER_CARD_RANK
  DEF_CMD EV_SEQ_POKEMON_RECOVER
  DEF_CMD EV_SEQ_POKECEN_RECOVER_ANIME
  DEF_CMD EV_SEQ_GET_LANG_ID
  DEF_CMD EV_SEQ_CHECK_TEMOTI_HP

  //ポケモン鳴き声
  DEF_CMD EV_SEQ_VOICE_PLAY
  DEF_CMD EV_SEQ_VOICE_WAIT

