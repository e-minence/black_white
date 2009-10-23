//======================================================================
/**
 * @file  scr_seq_def.h
 * @bfief  スクリプトコマンドのマクロ定義ファイル
 * @author  GAME FREAK inc.
 *
 */
//======================================================================

//スクリプトで使用するヘッダーファイル
  .include  "usescript.h"

//データを自然な境界にアラインするかの設定
  .option alignment off

//スクリプトコマンドテーブル用マクロの定義
  .include "scrcmd_table/def_table_macro.h"
//スクリプト定義テーブル
  .include "../../../prog/src/field/scrcmd_table.cdat"

//======================================================================
// イベントデータ関連
//======================================================================
//--------------------------------------------------------------
/**
 *  @def  _EVENT_DATA
 *  @brief  イベントデータ　テーブル宣言
 *  @param adrs 宣言するデータラベルアドレス
 */
//--------------------------------------------------------------
  .macro  _EVENT_DATA  adrs
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  @def  _EVENT_DATA_END
 *  @brief  イベントデータ　テーブル宣言終了
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _EVENT_DATA_END
  .short  EV_DATA_END_CODE
  .endm

//--------------------------------------------------------------
/**
 *  @def  _SP_EVENT_DATA_END
 *  @brief  特殊スクリプト終了
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _SP_EVENT_DATA_END
  .short   SP_SCRID_NONE
  .endm


//=============================================================================
/**
 *	特殊スクリプト関連
 */
//=============================================================================

//--------------------------------------------------------------
/**
 * @brief 特殊スクリプト登録：シーン発動イベントの登録
 * @param adrs  シーン発動イベントテーブルのアドレス
 *
 * @note
 * シーンスクリプト（フィールドでワークの値が指定値の場合に強制発動するスクリプト）を
 * 登録する
 */
//--------------------------------------------------------------
	.macro	_SCENE_CHANGE_LABEL	adrs
	.short	SP_SCRID_SCENE_CHANGE
	.long	((\adrs-.)-4)
	.endm

//--------------------------------------------------------------
/**
 * @brief シーン発動イベントの要素記述
 * @param wk      参照するワーク指定ID
 * @param num     上記ワークの発動条件とする値
 * @param scr_id  スクリプトを指定するID
 *
	起動条件記述
 */
//--------------------------------------------------------------
	.macro	_SCENE_CHANGE_DATA		wk,num,scr_id
	.short	\wk
	.short	\num
	.short	\scr_id
	.endm

//--------------------------------------------------------------
/**
 * @brief シーン発動イベントの要素終端
 *
 * _SCENE_CHANGE_LABELで指定したシーン発動イベントテーブルの最後には
 * 必ず_SCENE_CHANGE_ENDを記述する必要がある
 */
//--------------------------------------------------------------
	.macro	_SCENE_CHANGE_END
	.short	0
	.endm

//--------------------------------------------------------------
/**
 *	@brief  特殊スクリプト登録：ゾーン切り替え処理
 *	@param  scr_id  登録するスクリプトID
 *
 *	@note
 *	ゾーン切り替えのタイミングで呼ばれます。
 *	歩いてマップをまたいだタイミング、
 *	出入口・そらを飛ぶなどフィールドマップがないタイミング
 *	の両方で呼ばれます。
 *	後者の際は、使用できるコマンドの制限に気をつけること！
 */
//--------------------------------------------------------------
	.macro	_ZONE_CHANGE_LABEL	scr_id
	.short	SP_SCRID_ZONE_CHANGE
	.short	\scr_id
	.short	0		//4byteに合わせるためにダミー
	.endm

//--------------------------------------------------------------
/**
 *	@brief  特殊スクリプト登録：マップ初回初期化処理
 *	@param  scr_id  登録するスクリプトID
 *	@note
 *	マップができて、ＯＢＪが配置されたあと呼ばれる。
 *	そのマップに入ったときに呼ばれます。
 */
//--------------------------------------------------------------
	.macro	_FIELD_INIT_LABEL	scr_id
	.short	SP_SCRID_FIELD_INIT
	.short	\scr_id
	.short	0		//4byteに合わせるためにダミー
	.endm

//--------------------------------------------------------------
/**
 *	@brief  特殊スクリプト登録：マップ復帰初期化処理
 *	@param  scr_id  登録するスクリプトID
 *	@note
 *	マップができて、ＯＢＪが配置されたあと呼ばれる。
 *	マップ復帰処理（メニューやバトルからの復帰）のたびによばれます。
 */
//--------------------------------------------------------------
	.macro	_FIELD_RECOVER_LABEL	scr_id
	.short	SP_SCRID_FIELD_RECOVER
	.short	\scr_id
	.short	0		//4byteに合わせるためにダミー
	.endm


//======================================================================
//
//
//  イベント開始・終了
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * 通常イベント開始宣言
 */
//--------------------------------------------------------------
  .macro  EVENT_START label
\label:
  .short  EV_SEQ_COMMON_PROC_FIELD_EVENT_START
  .endm

//--------------------------------------------------------------
/**
 * 通常イベント終了宣言
 */
//--------------------------------------------------------------
  .macro  EVENT_END
  .short  EV_SEQ_COMMON_PROC_FIELD_EVENT_END
  .short  EV_SEQ_END
  .endm

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
  .macro  INIT_EVENT_START  label
\label:
  .endm

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
  .macro  INIT_EVENT_END
  .short  EV_SEQ_END
  .endm


//======================================================================
//  基本コマンド
//======================================================================
//--------------------------------------------------------------
/**
 *  @def  _NOP
 *  @brief  何もしない
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _NOP
  .short  EV_SEQ_NOP
  .endm

//--------------------------------------------------------------
/**
 *  @def  _DUMMY
 *  @brief  ダミー
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _DUMMY
  .short  EV_SEQ_DUMMY
  .endm

//--------------------------------------------------------------
/**
 *  @def  _END
 *  @brief  スクリプトの終了
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _END
  .short  EV_SEQ_END
  .endm

//--------------------------------------------------------------
/**
 *  @def  _TIME_WAIT
 *  @brief  ウェイト
 *  @param time 待ちフレーム数
 */
//--------------------------------------------------------------
#define _TIME_WAIT( time ) _ASM_TIME_WAIT time

  .macro  _ASM_TIME_WAIT time, ret_wk
  .short  EV_SEQ_TIME_WAIT
  .short  \time
  .endm


//--------------------------------------------------------------
/**
 *  _CALL コール
 *  @param adrs 呼び出すラベルアドレス
 */
//--------------------------------------------------------------
  .macro  _CALL adrs
  .short  EV_SEQ_CALL
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _RET コールを呼び出したアドレスに戻る
 */
//--------------------------------------------------------------
  .macro  _RET
  .short  EV_SEQ_RET
  .endm

//--------------------------------------------------------------
/**
 * @def _DEBUG_PRINT_WK
 * @brief ワークの値を出力する
 * @param work 値を出力するワーク
 */
//--------------------------------------------------------------
#define _DEBUG_PRINT_WK( work ) \
    _ASM_DEBUG_PRINT_WK  work

  .macro  _ASM_DEBUG_PRINT_WK work
  .short  EV_SEQ_DEBUG_PRINT_WK
  .short  \work
  .endm

//======================================================================
//    判定演算用命令
//  ※スクリプトコンパイラが使用しています。
//  　スクリプト担当者は使用しないで下さい。
//======================================================================
//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
  .macro  _CMP_STACK  cond
  .short  EV_SEQ_CMP_STACK
  .short   \cond
  .endm
//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _PUSH_WORK  wk
  .short  EV_SEQ_PUSH_WORK
  .short  \wk
  .endm

//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _PUSH_VALUE  value
  .short  EV_SEQ_PUSH_VALUE
  .short  \value
  .endm
//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _PUSH_FLAG  flag
  .short  EV_SEQ_PUSH_FLAG
  .short  \flag
  .endm
//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _POP_WORK  wk
  .short  EV_SEQ_POP_WORK
  .short  \wk
  .endm

//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _POP
  .short  EV_SEQ_POP
  .endm


//======================================================================
//  データロード・ストア関連
//======================================================================
//--------------------------------------------------------------
/**
 * _LD_REG_VAL 仮想マシンの汎用レジスタに1byteの値を格納
 * @param r 値を格納するレジスタ
 * @param val 格納する値
 */
//--------------------------------------------------------------
  .macro  _LD_REG_VAL  r,val
  .short  EV_SEQ_LD_REG_VAL
  .byte  \r
  .byte  \val
  .endm

//--------------------------------------------------------------
/**
 * _LD_REG_WDATA 仮想マシンの汎用レジスタに4byteの値を格納
 * @param r 値を格納するレジスタ
 * @param val 格納する値
 */
//--------------------------------------------------------------
  .macro  _LD_REG_WDATA  r,wdata
  .short  EV_SEQ_LD_REG_WDATA
  .byte  \r
  .long  \wdata
  .endm

//--------------------------------------------------------------
/**
 * _LD_REG_ADR 仮想マシンの汎用レジスタにアドレスを格納
 * @param reg アドレスを格納するレジスタ
 * @@aram ADDRESS 格納するアドレス
 */
//--------------------------------------------------------------
  .macro  _LD_REG_ADR  reg,ADDRESS
  .short  EV_SEQ_LD_REG_ADR
  .byte  \reg
  .long  \ADDRESS
  .endm

//--------------------------------------------------------------
/**
 * _LD_ADR_VAL アドレスの中身に値を代入
 * @param adrs 値を代入するアドレス
 * @param val 代入する値
 */
//--------------------------------------------------------------
  .macro  _LD_ADR_VAL  adrs,val
  .short  EV_SEQ_LD_ADR_VAL
  .long  \adrs
  .byte  \val
  .endm

//--------------------------------------------------------------
/**
 * _LD_ADR_REG アドレスの中身に仮想マシンの汎用レジスタの値を代入
 * @param adrs 値を代入するアドレス
 * @param val 代入する値が格納されたレジスタ
 */
//--------------------------------------------------------------
  .macro  _LD_ADR_REG  adrs,reg
  .short  EV_SEQ_LD_ADR_REG
  .long  \adrs
  .byte  \reg
  .endm

//--------------------------------------------------------------
/**
 * _LD_REG_REG 仮想マシンの汎用レジスタの値を汎用レジスタにコピー
 * @param r1 コピー先レジスタ
 * @param r2 コピー元レジスタ
 */
//--------------------------------------------------------------
  .macro  _LD_REG_REG  r1,r2
  .short  EV_SEQ_LD_REG_REG
  .byte  \r1,\r2
  .endm

//--------------------------------------------------------------
/**
 * _LD_ADR_ADR アドレスの中身にアドレスの中身を代入
 * @param r1 代入先アドレス
 * @param r2 代入元アドレス
 */
//--------------------------------------------------------------
  .macro  _LD_ADR_ADR  adr1,adr2
  .short  EV_SEQ_LD_ADR_ADR
  .long  \adr1
  .long  \adr2
  .endm

//======================================================================
//  比較命令
//======================================================================
//--------------------------------------------------------------
/**
 *  _CP_REG_REG 仮想マシンの汎用レジスタを比較
 *  @param r1 比較するレジスタ
 *  @param r2 r1と比較するレジスタ
 */
//--------------------------------------------------------------
  .macro  _CP_REG_REG  r1,r2
  .short  EV_SEQ_CP_REG_REG
  .byte  \r1,\r2
  .endm

//--------------------------------------------------------------
/**
 *  _CP_REG_VAL 仮想マシンの汎用レジスタと値を比較
 *  @param r1 比較するレジスタ
 *  @param val 比較する値
 */
//--------------------------------------------------------------
  .macro  _CP_REG_VAL  r1,val
  .short  EV_SEQ_CP_REG_VAL
  .byte  \r1,\val
  .endm

//--------------------------------------------------------------
/**
 *  _CP_REG_ADR 仮想マシンの汎用レジスタとアドレスの中身を比較
 *  @param r1 比較するレジスタ
 *  @param val 比較する値
 */
//--------------------------------------------------------------
  .macro  _CP_REG_ADR  r1,adrs
  .short  EV_SEQ_CP_REG_ADR
  .byte  \r1
  .long  \adrs
  .endm

//--------------------------------------------------------------
/**
 *  _CP_ADR_REG アドレスの中身と仮想マシンの汎用レジスタを比較
 *  @param adrs 比較するアドレス
 *  @param r1 比較するレジスタ
 */
//--------------------------------------------------------------
  .macro  _CP_ADR_REG  adrs,r1
  .short  EV_SEQ_CP_ADR_REG
  .long  \adrs
  .byte  \r1
  .endm

//--------------------------------------------------------------
/**
 *  _CP_ADR_VAL アドレスの中身と値を比較
 *  @param adrs 比較するアドレス
 *  @param val 比較する値
 */
//--------------------------------------------------------------
  .macro  _CP_ADR_VAL  adrs,val
  .short  EV_SEQ_CP_ADR_VAL
  .long  \adrs
  .byte  \val
  .endm

//--------------------------------------------------------------
/**
 *  _CP_ADR_ADR アドレスの中身とアドレスの中身を比較
 *  @param adr1 比較するアドレスその1
 *  @param adr2 比較するアドレスその2
 */
//--------------------------------------------------------------
  .macro  _CP_ADR_ADR  adr1,adr2
  .short  EV_SEQ_CP_ADR_ADR
  .long  \adr1
  .long  \adr2
  .endm

//--------------------------------------------------------------
/**
 *  _CMPVAL ワークと値の比較
 *  @param wk 比較するワーク
 *  @param val 比較する値
 */
//--------------------------------------------------------------
  .macro  _CMPVAL  wk,val
  .short  EV_SEQ_CP_WK_VAL
  .short  \wk
  .short  \val
  .endm

//--------------------------------------------------------------
/**
 *  _CMPWK ワークとワークの比較
 *  @param 比較するワークその1
 *  @param 比較するワークその2
 */
//--------------------------------------------------------------
  .macro  _CMPWK  wk1,wk2
  .short  EV_SEQ_CP_WK_WK
  .short  \wk1
  .short  \wk2
  .endm

#if 0 //wb null
//--------------------------------------------------------------
/**
 * デバッグ用：ワーク情報表示
 */
//--------------------------------------------------------------
  .macro  _DEBUG_WATCH_WORK  wk
  .short  EV_SEQ_DEBUG_WATCH_VALUE
  .short  \wk
  .endm
#endif //wb null

//======================================================================
//  仮想マシン関連
//======================================================================
//--------------------------------------------------------------
/**
 *  _VM_ADD 仮想マシン追加(切り替えはせず、並列で動作します！)
 *  @param id 動作させる仮想マシンID
 */
//--------------------------------------------------------------
  .macro  _VM_ADD id
  .short  EV_SEQ_VM_ADD
  .short  \id
  .endm

//--------------------------------------------------------------
/**
 * _CHG_COMMON_SCR ローカルスクリプトをウェイト状態にして、
 * 共通スクリプトを動作させます
 * @param id 動作させるスクリプトID
 */
//--------------------------------------------------------------
  .macro  _CHG_COMMON_SCR id
  .short  EV_SEQ_CHG_COMMON_SCR
  .short  \id
  .endm

//--------------------------------------------------------------
/**
 *  _CHG_LOCAL_SCR 共通スクリプトを終了して、ローカルスクリプトを再開させます
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _CHG_LOCAL_SCR
  .short  EV_SEQ_CHG_LOCAL_SCR
  .endm

//======================================================================
//  分岐命令
//======================================================================
//--------------------------------------------------------------
/**
 *  _JUMP ジャンプ
 *  @param adrs 飛び先のアドレス
 */
//--------------------------------------------------------------
  .macro  _JUMP adrs
  .short  EV_SEQ_JUMP
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IF_JUMP IFジャンプ
 *  @param cond 比較コマンド LT,EQ,GT...
 *  @param adrs condと一致していた際のジャンプ先アドレス
 */
//--------------------------------------------------------------
  .macro  _IF_JUMP cond,adrs
  .short  EV_SEQ_IF_JUMP
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IF_CALL IFコール
 *  @param cond 比較コマンド LT,EQ,GT...
 *  @param adrs condと一致していた際に呼び出すアドレス
 */
//--------------------------------------------------------------
  .macro  _IF_CALL cond,adrs
  .short  EV_SEQ_IF_CALL
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IFVAL_JUMP 値を比較、条件どおりの場合分岐(ジャンプ)
 *  @param wk 比較するワーク
 *  @param cond 比較コマンド LT,RQ,GT...
 *  @param val wkと比較する値
 *  @param adrs condと一致していた際のジャンプ先アドレス
 */
//--------------------------------------------------------------
  .macro  _IFVAL_JUMP wk,cond,val,adrs
  _CMPVAL  \wk,\val
  .short  EV_SEQ_IF_JUMP
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IFVAL_CALL 値を比較、条件どおりの場合分岐(コール)
 *  @param wk 比較するワーク
 *  @param cond 比較コマンド LT,RQ,GT...
 *  @param val wkと比較する値
 *  @param adrs condと一致していた際に呼び出すアドレス
 */
//--------------------------------------------------------------
  .macro  _IFVAL_CALL wk,cond,val,adrs
  _CMPVAL  \wk,\val
  .short  EV_SEQ_IF_CALL
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IFWK_JUMP ワークを比較、条件どおりの場合分岐(ジャンプ)
 *  @param wk1 比較するワーク1
 *  @param cond 比較コマンド LT,RQ,GT...
 *  @param wk2 と比較するワーク2
 *  @param adrs condと一致していた際のジャンプ先アドレス
 */
//--------------------------------------------------------------
  .macro  _IFWK_JUMP wk1,cond,wk2,adrs
  _CMPWK  \wk1,\wk2
  .short  EV_SEQ_IF_JUMP
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IFWK_CALL ワークを比較、条件どおりの場合分岐(コール)
 *  @param wk1 比較するワーク1
 *  @param cond 比較コマンド LT,RQ,GT...
 *  @param wk2 と比較するワーク2
 *  @param adrs condと一致していた際に呼び出すアドレス
 */
//--------------------------------------------------------------
  .macro  _IFWK_CALL wk1,cond,wk2,adrs
  _CMPWK  \wk1,\wk2
  .short  EV_SEQ_IF_CALL
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm


//======================================================================
//  イベントフラグ関連
//======================================================================
//--------------------------------------------------------------
/**
 *  _FLAG_SET フラグのセット
 *  @param num セットするフラグナンバー
 */
//--------------------------------------------------------------
#define   _FLAG_SET( num ) _ASM_FLAG_SET num

  .macro  _ASM_FLAG_SET num
  .short  EV_SEQ_FLAG_SET
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _FLAG_RESET フラグのリセット
 *  @param リセットするフラグナンバー
 */
//--------------------------------------------------------------
#define _FLAG_RESET( num ) _ASM_FLAG_RESET num

  .macro  _ASM_FLAG_RESET num
  .short  EV_SEQ_FLAG_RESET
  .short  \num
  .endm


//--------------------------------------------------------------
/**
 *  _FLAG_CHECK フラグチェック
 *  @param チェックするフラグナンバー
 *  @param 結果を格納するワーク
 */
//--------------------------------------------------------------
#define _FLAG_CHECK( num, wk ) _ASM_FLAG_CHECK num,wk
  
  .macro  _ASM_FLAG_CHECK num,wk
  .short  EV_SEQ_FLAG_CHECK
  .short  \num
  .short  \wk
  .endm

//--------------------------------------------------------------
/**
 *  _ASM_FLAG_CHECK_VM フラグチェック
 *  @param チェックするフラグナンバー
 */
//--------------------------------------------------------------
  .macro  _ASM_FLAG_CHECK_VM num
  .short  EV_SEQ_FLAG_CHECK_VM
  .short  \num
  .endm

//--------------------------------------------------------------
/*
 *  _IF_FLAGON_JUMP フラグONの時に分岐(JUMP)
 *  @param num チェックするフラグナンバー
 *  @param adrs 条件が一致していた際のジャンプ先アドレス
 */
//--------------------------------------------------------------
  .macro  _IF_FLAGON_JUMP num,adrs
  _ASM_FLAG_CHECK_VM \num
  _IF_JUMP  FLGON,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_FLAGOFF_JUMP フラグOFFの時に分岐(JUMP)
 *  @param num チェックするフラグナンバー
 *  @param adrs 条件が一致していた際のジャンプ先アドレス
 */
//--------------------------------------------------------------

  .macro  _IF_FLAGOFF_JUMP num,adrs
  _ASM_FLAG_CHECK_VM \num
  _IF_JUMP  FLGOFF,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_FLAGON_CALL フラグONの時に分岐(CALL)
 *  @param num チェックするフラグナンバー
 *  @param adrs 条件が一致していた際のジャンプ先アドレス
 */
//--------------------------------------------------------------
  .macro  _IF_FLAGON_CALL num,adrs
  _ASM_FLAG_CHECK_VM \num
  _IF_CALL  FLGON,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_FLAGOFF_CALL フラグOFFの時に分岐(CALL)
 *  @param num チェックするフラグナンバー
 *  @param adrs 条件が一致していた際のジャンプ先アドレス
 */
//--------------------------------------------------------------
  .macro  _IF_FLAGOFF_CALL num,adrs
  _ASM_FLAG_CHECK_VM \num
  _IF_CALL  FLGOFF,\adrs
  .endm

//--------------------------------------------------------------
/**
 * _FLAG_CHECK_WK ワークの値をフラグナンバーとしてチェックする！
 * @param wk チェックする値が格納されたワーク
 * @param ret_wk 結果を格納するワーク
 */
//--------------------------------------------------------------
  .macro  _FLAG_CHECK_WK wk,ret_wk
  .short  EV_SEQ_FLAG_CHECK_WK
  .short  \wk
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * _FLAG_SET_WK ワークの値をフラグナンバーとしてセットする！
 * @param wk フラグナンバーが格納されたワーク
 */
//--------------------------------------------------------------
  .macro  _FLAG_SET_WK wk
  .short  EV_SEQ_FLAG_SET_WK
  .short  \wk
  .endm

//======================================================================
//  ワーク操作関連
//======================================================================
//--------------------------------------------------------------
/**
 *  _ADD_WK ワークに値を足す
 *  @param 値を足すワーク
 *  @param num 足す値
 */
//--------------------------------------------------------------
  .macro  _ADD_WK wk,num
  .short  EV_SEQ_ADD_WK
  .short  \wk
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _SUB_WK ワークから値を引く
 *  @param 値を引くワーク
 *  @param num 引く値
 */
//--------------------------------------------------------------
  .macro  _SUB_WK wk,num
  .short  EV_SEQ_SUB_WK
  .short  \wk
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _LDVAL ワークに値を代入
 *  @param wk 値を格納するワーク
 *  @param val 格納する値
 */
//--------------------------------------------------------------
#define _LDVAL( wk, val ) _ASM_LDVAL wk,val

  .macro  _ASM_LDVAL  wk,val
  .short  EV_SEQ_LD_WK_VAL
  .short  \wk
  .short  \val
  .endm

//--------------------------------------------------------------
/**
 *  _LDWK ワークにワークの値を代入
 *  @param wk1 代入先ワーク
 *  @param wk2 代入する値が格納されたワーク
 */
//--------------------------------------------------------------
#define _LDWK( wk1, wk2 ) _ASM_LDWK wk1,wk2

  .macro  _ASM_LDWK  wk1,wk2
  .short  EV_SEQ_LD_WK_WK
  .short  \wk1
  .short  \wk2
  .endm

//--------------------------------------------------------------
/**
 *  _LDWKVAL ワークに値かワークの値を代入
 *  @param wk1 代入先ワーク
 *  @param wk2 代入する値もしくはワーク
 */
//--------------------------------------------------------------
  .macro  _ASM_LDWKVAL  wk1,wk2
  .short  EV_SEQ_LD_WK_WKVAL
  .short  \wk1
  .short  \wk2
  .endm

//======================================================================
//  キー入力関連
//======================================================================
//--------------------------------------------------------------
/**
 *  _AB_KEYWAIT キー待ち
 *  @param none
 */
//--------------------------------------------------------------
#define _AB_KEYWAIT() _ASM_AB_KEYWAIT

  .macro  _ASM_AB_KEYWAIT
  .short  EV_SEQ_ABKEYWAIT
  .endm


//--------------------------------------------------------------
/**
 *  _LAST_KEYWAIT メッセージ最後のキー待ち
 *
 *  ※ABキーだけでなく、十字キーでも抜けることができる
 */
//--------------------------------------------------------------
#define _LAST_KEYWAIT() _ASM_LAST_KEYWAIT

  .macro  _ASM_LAST_KEYWAIT
  .short  EV_SEQ_LASTKEYWAIT
  .endm

//======================================================================
//  フィールド　会話ウィンドウ
//======================================================================
//--------------------------------------------------------------
/**
 *  _TALKMSG 展開メッセージを表示(1byte)
 *  @param  msg_id  表示するメッセージID
 */
//--------------------------------------------------------------
#define _TALKMSG( msg_id ) _ASM_TALK_MSG msg_id

  .macro  _ASM_TALK_MSG msg_id
  .short  EV_SEQ_TALKMSG
  .short  \msg_id
  .endm

//--------------------------------------------------------------
/**
 *  _TALKMSG_ALLPUT 展開メッセージを表示(1byte)
 *  @param  msg_id  表示するメッセージID
 */
//--------------------------------------------------------------
#define _TALKMSG_ALLPUT( msg_id ) _ASM_TALKMSG_ALLPUT msg_id

  .macro  _ASM_TALKMSG_ALLPUT msg_id
  .short  EV_SEQ_TALKMSG_ALLPUT
  .short  \msg_id
  .endm

//--------------------------------------------------------------
/**
 *  _TALKWIN_OPEN 会話ウィンドウ開く
 *  @param none
 */
//--------------------------------------------------------------
#define _TALKWIN_OPEN() _ASM_TALKWIN_OPEN

  .macro  _ASM_TALKWIN_OPEN
  .short  EV_SEQ_TALKWIN_OPEN
  .endm

//--------------------------------------------------------------
/**
 *  _TALKWIN_CLOSE 会話ウィンドウ閉じる
 *  @param none
 */
//--------------------------------------------------------------
#define _TALKWIN_CLOSE()  _ASM_TALKWIN_CLOSE

  .macro  _ASM_TALKWIN_CLOSE
  .short  EV_SEQ_TALKWIN_CLOSE
  .endm

//======================================================================
//  フィールド　吹き出しウィンドウ
//======================================================================
//--------------------------------------------------------------
/**
 *  @def  _BALLOONWIN_OBJMSG_OPEN
 *  @brief  吹き出しウィンドウ描画
 *  @param msg_id 表示するメッセージID
 *  @param obj_id 吹き出しを出す対象OBJ ID
 */
//--------------------------------------------------------------
#define _BALLOONWIN_OBJMSG_OPEN( msg_id , obj_id ) _ASM_BALLOONWIN_OBJMSG_OPEN msg_id, obj_id

  .macro _ASM_BALLOONWIN_OBJMSG_OPEN msg_id, obj_id
  .short  EV_SEQ_BALLOONWIN_OBJMSG_OPEN
  .short 0x0400
  .short \msg_id
  .byte \obj_id
  .endm

//--------------------------------------------------------------
/**
 *  @def  _BALLOONWIN_TALKOBJ_OPEN
 *  @brief  吹き出しウィンドウ描画　話し掛けOBJ専用
 *  @param msg_id 表示するメッセージID
 */
//--------------------------------------------------------------
#define _BALLOONWIN_TALKOBJ_OPEN( msg_id ) _ASM_BALLOONWIN_TALKOBJ_OPEN msg_id

  .macro _ASM_BALLOONWIN_TALKOBJ_OPEN msg_id
  .short  EV_SEQ_BALLOONWIN_TALKOBJ_OPEN
  .short 0x0400
  .short \msg_id
  .endm

//--------------------------------------------------------------
/**
 * @def _BALLOONWIN_TALKOBJ_OPEN_ARC
 *
 * @brief 吹き出しウィンドウ描画 話しかけOBJ専用、メッセージアーカイブ指定付き
 * 
 *  @param  arc_id  メッセージアーカイブ指定ID
 *  @param msg_id 表示するメッセージID
 */
//--------------------------------------------------------------
#define _BALLOONWIN_TALKOBJ_OPEN_ARC( arc_id, msg_id ) \
    _ASM_BALLOONWIN_TALKOBJ_OPEN_ARC arc_id, msg_id

  .macro  _ASM_BALLOONWIN_TALKOBJ_OPEN_ARC arc_id, msg_id
  .short  EV_SEQ_BALLOONWIN_TALKOBJ_OPEN
  .short \arc_id
  .short \msg_id
  .endm

//--------------------------------------------------------------
/**
 *  _BALLOONWIN_CLOSE 吹き出しウィンドウ閉じる
 *  @param none
 */
//--------------------------------------------------------------
#define _BALLOONWIN_CLOSE() _ASM_BALLOONWIN_CLOSE

  .macro  _ASM_BALLOONWIN_CLOSE
  .short  EV_SEQ_BALLOONWIN_CLOSE
  .endm

//--------------------------------------------------------------
/**
 *  @def _GOLD_WIN_OPEN
 *  @brief 所持金ウィンドウを表示する
 *  @param x 表示x位置(キャラ単位)
 *  @param y 表示x位置(キャラ単位)
 *  @param none
 */
//--------------------------------------------------------------
#define _GOLD_WIN_OPEN( x, y ) \
    _ASM_GOLD_WIN_OPEN x, y

  .macro _ASM_GOLD_WIN_OPEN x, y
  .short EV_SEQ_GOLD_WIN_OPEN
  .short \x
  .short \y
  .endm

//--------------------------------------------------------------
/**
 *  @def _GOLD_WIN_CLOSE
 *  @brief 所持金ウィンドウを消去する
 *  @param none
 */
//--------------------------------------------------------------
#define _GOLD_WIN_CLOSE() \
    _ASM_GOLD_WIN_CLOSE

  .macro _ASM_GOLD_WIN_CLOSE
  .short EV_SEQ_GOLD_WIN_CLOSE
  .endm

//--------------------------------------------------------------
/**
 *  @def _GOLD_WIN_UPDATE
 *  @brief 所持金ウィンドウを更新する
 *  @param x 表示x位置(キャラ単位)
 *  @param y 表示x位置(キャラ単位)
 *  @param none
 */
//--------------------------------------------------------------
#define _GOLD_WIN_UPDATE( x, y ) \
    _ASM_GOLD_WIN_UPDATE x, y

  .macro _ASM_GOLD_WIN_UPDATE x, y
  .short EV_SEQ_GOLD_WIN_UPDATE
  .short \x
  .short \y
  .endm

//======================================================================
//  動作モデル
//======================================================================
//--------------------------------------------------------------
/**
 *  _ANIME_LABEL アニメーションデータラベル
 *  @param label データラベル名
 */
//--------------------------------------------------------------
  .macro  _ANIME_LABEL label
  .align  4
  \label:
  .endm

//--------------------------------------------------------------
/**
 *  _ANIME_DATA アニメーションデータ(fldmmdl_code.h参照)
 *  @param code アニメーションコード
 *  @param num code実行回数
 */
//--------------------------------------------------------------
  .macro  _ANIME_DATA  code,num
  .short  \code
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _OBJ_ANIME フィールドOBJ アニメーションコマンド
 *  @param obj_id アニメーションを行うOBJ ID
 *  @param adrs アニメーションコマンドデータラベル
 *
 *  @note 注意！ 動作停止をセットしたい時は、
 *  _OBJ_PAUSE_ALLをもう一度呼ばないといけない！
 */
//--------------------------------------------------------------
#define _OBJ_ANIME( obj_id, adrs ) _ASM_OBJ_ANIME obj_id, adrs

  .macro  _ASM_OBJ_ANIME obj_id, adrs
  .short  EV_SEQ_OBJ_ANIME
  .short  \obj_id 
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _OBJ_ANIME_WAIT アニメーションコマンド終了待ち
 *  @param none
 */
//--------------------------------------------------------------
#define _OBJ_ANIME_WAIT() _ASM_OBJ_ANIME_WAIT

  .macro  _ASM_OBJ_ANIME_WAIT
  .short  EV_SEQ_OBJ_ANIME_WAIT
  .endm
  
//--------------------------------------------------------------
/**
 *  _MOVE_CODE_GET 動作コード取得(fldmmdl_code.h)
 *  @param ret_wk 動作コードを格納するワーク
 *  @param obj_id 動作コードを取得するOBJ ID
 */
//--------------------------------------------------------------
#define  _MOVE_CODE_GET(  ret_wk,obj_id ) _ASM_MOVE_CODE_GET ret_wk, obj_id

  .macro  _ASM_MOVE_CODE_GET  ret_wk, obj_id
  .short  EV_SEQ_MOVE_CODE_GET 
  .short  \ret_wk
  .short  \obj_id
  .endm


//--------------------------------------------------------------
/**
 * @def _OBJ_POS_GET
 * @brief OBJの位置取得
 * @param obj_id 取得するOBJ ID
 * @param x X座標格納先
 * @param z Z座標格納先
 */
//--------------------------------------------------------------
#define _OBJ_POS_GET( obj_id,x,z ) _ASM_OBJ_POS_GET obj_id, x, z

  .macro  _ASM_OBJ_POS_GET  obj_id, x, z
  .short  EV_SEQ_OBJ_POS_GET
  .short  \obj_id 
  .short  \x
  .short  \z
  .endm

//--------------------------------------------------------------
/**
 * @def _PLAYER_POS_GET
 * @brief 主人公の位置取得
 * @param x X座標格納先
 * @param z Z座標格納先
 */
//--------------------------------------------------------------
#define _PLAYER_POS_GET( x, z ) _ASM_PLAYER_POS_GET x, z

  .macro  _ASM_PLAYER_POS_GET x, z
  .short  EV_SEQ_PLAYER_POS_GET
  .short  \x
  .short  \z
  .endm

//--------------------------------------------------------------
/**
 * @def _PLAYER_DIR_GET
 * @brief プレイヤーの方向取得
 * @param ret_wk    方向を受け取るワーク
 *
 * @note  方向についてはprog/include/field/field_dir.hを参照
 * @todo  グリッドマップのみでレールマップのことは考慮されていない
 */
//--------------------------------------------------------------
#define _PLAYER_DIR_GET( ret_wk ) \
    _ASM_PLAYER_DIR_GET ret_wk

    .macro  _ASM_PLAYER_DIR_GET ret_wk
    .short  EV_SEQ_PLAYER_DIR_GET
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * _OBJ_ADD_EX OBJを追加
 * @param x 表示するグリッドX座標
 * @param z 表示するグリッドZ座標
 * @param dir 方向 DIR_UP等
 * @param id 識別用OBJ ID
 * @param code 表示コードBOY1等
 * @param move 動作コードMV_RND等
 */
//--------------------------------------------------------------
#define _OBJ_ADD_EX( x,z,dir,id,code,move ) _ASM_OBJ_ADD_EX x, z, dir, id, code, move

.macro _ASM_OBJ_ADD_EX x, z, dir, id, code, move
  .short EV_SEQ_OBJ_ADD
  .short \x
  .short \z
  .short \dir
  .short \id
  .short \code
  .short \move
.endm

 
//--------------------------------------------------------------
/**
 * _OBJ_DEL_EX OBJを削除
 * @param id 削除するOBJ ID
 */
//--------------------------------------------------------------
#define _OBJ_DEL_EX( id ) _ASM_OBJ_DEL_EX id

.macro  _ASM_OBJ_DEL_EX id
  .short EV_SEQ_OBJ_DEL
  .short \id
.endm
 
//--------------------------------------------------------------
/**
 * _OBJ_ADD ゾーン内で配置されているOBJ一覧内、指定のIDを持つOBJを追加
 * @param id 配置OBJ内で追加するOBJ ID
 */
//--------------------------------------------------------------
#define _OBJ_ADD( id ) \
  _ASM_OBJ_ADD id

  .macro _ASM_OBJ_ADD id
  .short EV_SEQ_OBJ_ADD_EV
  .short \id
  .endm

//--------------------------------------------------------------
/**
 * ！！使用禁止です！！　タイミングをみてコマンドを削除予定
 * _OBJ_DEL_EV 指定のOBJ_IDを持ったOBJを削除。
 * 削除時、対象OBJで指定されているイベントフラグをONにする。
 * @param id 配置OBJ内で追加するOBJ ID
 */
//--------------------------------------------------------------
#define _OBJ_DEL( id )  \
    _ASM_OBJ_DEL id

  .macro _ASM_OBJ_DEL id
  .short EV_SEQ_OBJ_DEL_EV
  .short \id
  .endm

//--------------------------------------------------------------
/**
 * _OBJ_POS_CHANGE 指定のOBJ_IDを持ったOBJの座標位置を変更。
 * @param id 座標変更するOBJ ID
 * @param gx グリッドX座標
 * @param gy グリッドY座標
 * @param gz グリッドZ座標
 * @param dir 方向 DIR_UP等
 */
//--------------------------------------------------------------
#define _OBJ_POS_CHANGE( id, gx, gy, gz, dir )  \
    _ASM_OBJ_POS_CHANGE id, gx, gy, gz, dir

	.macro	_ASM_OBJ_POS_CHANGE	id,gx,gy,gz,dir
	.short	EV_SEQ_OBJ_POS_CHANGE
	.short	\id
	.short	\gx
	.short	\gy
	.short	\gz
	.short	\dir
	.endm

//--------------------------------------------------------------
/**
 * @def _PLAYER_REQUEST
 * @brief 自機にリクエストを出す
 * @param reqbit FIELD_PLAYER_REQBIT_NORMAL等
 */
//--------------------------------------------------------------
#define _PLAYER_REQUEST( reqbit ) _ASM_PLAYER_REQUEST reqbit

  .macro  _ASM_PLAYER_REQUEST reqbit
  .short  EV_SEQ_PLAYER_REQUEST
  .short  \reqbit
  .endm

//======================================================================
//  動作モデル　イベント関連
//======================================================================
//--------------------------------------------------------------
/**
 *  _TURN_HERO_SITE 話しかけたOBJを自機方向へ振り向き
 *  @param none
 */
//--------------------------------------------------------------
#define _TURN_HERO_SITE() _ASM_TURN_HERO_SITE

.macro  _ASM_TURN_HERO_SITE
.short  EV_SEQ_OBJ_TURN
.endm

//======================================================================
//  はい、いいえ　処理
//======================================================================
//--------------------------------------------------------------
/**
 *  _YES_NO_WIN はい、いいえ選択
 *  @param ret_wk はい、いいえ選択結果格納先
 *  @retval 0のとき、「はい」を選択
 *  @retval 1のとき、「いいえ」またはキャンセル
 */
//--------------------------------------------------------------
#define _YES_NO_WIN( ret_wk ) _ASM_YES_NO_WIN ret_wk

  .macro  _ASM_YES_NO_WIN ret_wk
  .short  EV_SEQ_YES_NO_WIN
  .short  \ret_wk
  .endm

//======================================================================
//  WORDSET関連
//======================================================================
//--------------------------------------------------------------
/**
 *  _PLAYER_NAME 主人公名を指定タグにセット
 *  @param idx セットするタグナンバー
 */
//--------------------------------------------------------------
#define _PLAYER_NAME( idx ) _ASM_PLAYER_NAME idx

  .macro  _ASM_PLAYER_NAME idx
  .short  EV_SEQ_PLAYER_NAME
  .byte   \idx
  .endm

//--------------------------------------------------------------
/**
 *  _ITEM_NAME アイテム名を指定タグにセット
 *  @param idx セットするタグナンバー
 *  @param itemno 名前を取得するアイテムナンバー
 */
//--------------------------------------------------------------
#define _ITEM_NAME( idx, itemno ) _ASM_ITEM_NAME idx, itemno
  
  .macro _ASM_ITEM_NAME idx, itemno
  .short EV_SEQ_ITEM_NAME
  .byte \idx
  .short \itemno
  .endm

//--------------------------------------------------------------
/**
 *	技マシンの技名を指定タグにセット
 *  @param idx セットするタグナンバー
 *  @param itemno 名前を取得するアイテムナンバー
 */
//--------------------------------------------------------------
#define _ITEM_WAZA_NAME( idx, itemno ) _ASM_ITEM_WAZA_NAME idx, itemno

	.macro	_ASM_ITEM_WAZA_NAME	idx,itemno
	.short	EV_SEQ_ITEM_WAZA_NAME
	.byte	\idx
	.short	\itemno
	.endm

//--------------------------------------------------------------
/**
 *	技名を指定タグにセット
 *  @param idx セットするタグナンバー
 *  @param wazano 名前を取得する技番号
 */
//--------------------------------------------------------------
#define _WAZA_NAME( idx, waza ) _ASM_WAZA_NAME idx,waza

	.macro	_ASM_WAZA_NAME	idx,waza
	.short	EV_SEQ_WAZA_NAME
	.byte	\idx
	.short	\waza
	.endm

//--------------------------------------------------------------
/**
 * @def _POKETYPE_NAME
 * @brief ポケモン・わざのタイプ名を指定タグにセット
 *  @param idx セットするタグナンバー
 *  @param poketype 名前を取得するタイプ指定ID
 */
//--------------------------------------------------------------
#define _POKETYPE_NAME( idx, poketype ) \
    _ASM_POKETYPE_NAME idx, poketype

    .macro  _ASM_POKETYPE_NAME idx, poketype
    .short  EV_SEQ_POKETYPE_NAME
    .byte   \idx
    .short  \poketype
    .endm

//--------------------------------------------------------------
/**
 * @def _MONS_NAME
 * @brief ずかんナンバーからポケモン名を指定タグにセット
 *  @param idx セットするタグナンバー
 *  @param monsno 名前を取得するポケモンのずかんナンバー
 */
//--------------------------------------------------------------
#define _MONS_NAME( idx, monsno ) \
    _ASM_MONS_NAME idx, monsno

    .macro  _ASM_MONS_NAME idx, monsno
    .short  EV_SEQ_MONS_NAME
    .byte   \idx
    .short  \monsno
    .endm

//--------------------------------------------------------------
/**
 *  ポケット名を指定タグにセット
 * @param idx セットするタグナンバー
 * @param pocket_id   バッグのポケット名を指すID
 */
//--------------------------------------------------------------
#define _POCKET_NAME( idx, pocket_id )  _ASM_POCKET_NAME idx, pocket_id

  .macro  _ASM_POCKET_NAME idx, pocket_id
  .short  EV_SEQ_POCKET_NAME
  .byte   \idx
  .short  \pocket_id
  .endm

//--------------------------------------------------------------
/**
 *  手持ちポケモンの種族名を指定タグにセット
 * @param idx セットするタグナンバー
 * @param pos セットするポケモンを指定
 */
//--------------------------------------------------------------
#define _TEMOTI_MONS_NAME( idx, pos ) \
    _ASM_TEMOTI_MONS_NAME idx, pos

  .macro _ASM_TEMOTI_MONS_NAME idx, pos
  .short EV_SEQ_TEMOTI_MONS_NAME
  .byte  \idx
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 *  手持ちポケモンのニックネームを指定タグにセット
 * @param idx セットするタグナンバー
 * @param pos セットするポケモンを指定
 */
//--------------------------------------------------------------
#define _TEMOTI_NICK_NAME( idx, pos ) \
    _ASM_TEMOTI_NICK_NAME idx, pos

  .macro _ASM_TEMOTI_NICK_NAME idx, pos
  .short EV_SEQ_TEMOTI_NICK_NAME
  .byte  \idx
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 *  育て屋のポケモンの種族名を指定タグにセット
 * @param idx セットするタグナンバー
 * @param pos セットするポケモンを指定
 */
//--------------------------------------------------------------
#define _SODATEYA_MONS_NAME( idx, pos ) \
    _ASM_SODATEYA_MONS_NAME idx, pos

  .macro _ASM_SODATEYA_MONS_NAME idx, pos
  .short EV_SEQ_SODATEYA_MONS_NAME
  .byte  \idx
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 *  育て屋のポケモンのニックネームを指定タグにセット
 * @param idx セットするタグナンバー
 * @param pos セットするポケモンを指定
 */
//--------------------------------------------------------------
#define _SODATEYA_NICK_NAME( idx, pos ) \
    _ASM_SODATEYA_NICK_NAME idx, pos

  .macro _ASM_SODATEYA_NICK_NAME idx, pos
  .short EV_SEQ_SODATEYA_NICK_NAME
  .byte  \idx
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * 数値を指定タグにセット
 * @param  idx セットするタグナンバー
 * @param  num セットする数値
 * @param keta 桁数
 */
//--------------------------------------------------------------
#define _NUMBER( idx, num, keta ) \
    _ASM_NUMBER idx, num, keta

  .macro _ASM_NUMBER idx, num, keta
  .short EV_SEQ_NUMBER
  .byte  \idx
  .short \num
  .short \keta
  .endm

//======================================================================
//  視線トレーナー関連
//======================================================================
//--------------------------------------------------------------
/**
 *  _EYE_TRAINER_MOVE_SET 視線：トレーナー移動呼び出し
 *  @param pos 移動させるトレーナー データ番号0,1
 */
//--------------------------------------------------------------
  .macro  _EYE_TRAINER_MOVE_SET  pos
  .short  EV_SEQ_EYE_TRAINER_MOVE_SET
  .short  \pos
  .endm
 
//--------------------------------------------------------------
/**
 *  _EYE_TRAINER_MOVE_SET 視線：トレーナー移動　シングル
 *  @param pos 移動させるトレーナー データ番号0,1
 */
//--------------------------------------------------------------
  .macro  _EYE_TRAINER_MOVE_SINGLE pos
  .short  EV_SEQ_EYE_TRAINER_MOVE_SINGLE
  .short  \pos
  .endm
 
//--------------------------------------------------------------
/**
 *  _EYE_TRAINER_MOVE_SET 視線：トレーナー移動　ダブル
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _EYE_TRAINER_MOVE_DOUBLE
  .short  EV_SEQ_EYE_TRAINER_MOVE_DOUBLE
  .endm

//--------------------------------------------------------------
/**
 *  _EYE_TRAINER_TYPE_GET 視線：トレーナータイプ取得
 *  @param ret_wk トレーナータイプ格納先 SCR_EYE_TR_TYPE_SINGLE等が格納される
 */
//--------------------------------------------------------------
  .macro  _EYE_TRAINER_TYPE_GET ret_wk
  .short  EV_SEQ_EYE_TRAINER_TYPE_GET
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  視線：トレーナーID取得
 */
//--------------------------------------------------------------
  .macro  _EYE_TRAINER_ID_GET  pos,ret_wk
  .short  EV_SEQ_EYE_TRAINER_ID_GET
  .short  \pos
  .short  \ret_wk
  .endm

//======================================================================
//  トレーナーバトル関連
//======================================================================
//--------------------------------------------------------------
/**
 *  _TRAINER_ID_GET トレーナーID取得
 *  @param wk ID格納先ワーク
 */
//--------------------------------------------------------------
#define _TRAINER_ID_GET( wk ) \
  _ASM_TRAINER_ID_GET wk

  .macro  _ASM_TRAINER_ID_GET  wk
  .short  EV_SEQ_TRAINER_ID_GET
  .short  \wk
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_BTL_SET トレーナー戦闘呼び出し
 *  @param tr_id_0 トレーナーID0
 *  @param tr_id_1 トレーナーID1
 */
//--------------------------------------------------------------
#define _TRAINER_BTL_SET( tr_id0, tr_id1 ) \
    _ASM_TRAINER_BTL_SET tr_id0, tr_id1
  
  .macro  _ASM_TRAINER_BTL_SET  tr_id_0,tr_id_1
  .short  EV_SEQ_TRAINER_BTL_SET
  .short  \tr_id_0
  .short  \tr_id_1
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_MULTI_BTL_SET トレーナー戦闘（マルチバトル）呼び出し
 *  @param partner_id パートナーID
 *  @param tr_id_0 トレーナーID0
 *  @param tr_id_1 トレーナーID1
 */
//--------------------------------------------------------------
#define _TRAINER_MULTI_BTL_SET( partner_id, tr_id_0, tr_id_1 ) \
  _ASM_TRAINER_MULTI_BTL_SET  partner_id, tr_id_0, tr_id_1

  .macro  _ASM_TRAINER_MULTI_BTL_SET  partner_id, tr_id_0, tr_id_1
  .short  EV_SEQ_TRAINER_MULTI_BTL_SET
  .short  \partner_id
  .short  \tr_id_0
  .short  \tr_id_1
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_MSG_SET トレーナー会話呼び出し
 *  @param tr_id メッセージ表示用トレーナーID
 *  @param kind_id 会話種類
 */
//--------------------------------------------------------------
#define _TRAINER_MSG_SET( tr_id,kind_id ) \
  _ASM_TRAINER_MSG_SET  tr_id,kind_id

  .macro  _ASM_TRAINER_MSG_SET  tr_id,kind_id
  .short  EV_SEQ_TRAINER_MSG_SET
  .short  \tr_id
  .short  \kind_id
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_TALK_TYPE_GET トレーナー会話の種類取得
 *  @param wk1 会話種類格納先ワーク 戦闘前
 *  @param wk2 会話種類格納先ワーク 戦闘後
 *  @param wk2 会話種類格納先ワーク ポケモン１体
 */
//--------------------------------------------------------------
#define _TRAINER_TALK_TYPE_GET(  wk1,wk2,wk3 ) \
  _ASM_TRAINER_TALK_TYPE_GET  wk1,wk2,wk3

  .macro  _ASM_TRAINER_TALK_TYPE_GET  wk1,wk2,wk3
  .short  EV_SEQ_TRAINER_TALK_TYPE_GET
  .short  \wk1
  .short  \wk2
  .short  \wk3
  .endm

//--------------------------------------------------------------
/**
 *  _REVENGE_TRAINER_TALK_TYPE_GET 再戦トレーナー会話の種類取得
 *  @param wk1 会話種類格納先ワーク 戦闘前
 *  @param wk2 会話種類格納先ワーク 戦闘後
 *  @param wk2 会話種類格納先ワーク ポケモン１体
 */
//--------------------------------------------------------------
#define _REVENGE_TRAINER_TALK_TYPE_GET(  wk1,wk2,wk3 ) \
  _ASM_REVENGE_TRAINER_TALK_TYPE_GET  wk1,wk2,wk3

  .macro  _ASM_REVENGE_TRAINER_TALK_TYPE_GET  wk1,wk2,wk3
  .short  EV_SEQ_REVENGE_TRAINER_TALK_TYPE_GET
  .short  \wk1
  .short  \wk2
  .short  \wk3
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_TYPE_GET トレーナータイプ取得
 *  @param ret_wk タイプ格納先ワーク
 */
//--------------------------------------------------------------
#define _TRAINER_TYPE_GET( ret_wk ) \
  _ASM_TRAINER_TYPE_GET  ret_wk

  .macro  _ASM_TRAINER_TYPE_GET  ret_wk
  .short  EV_SEQ_TRAINER_TYPE_GET
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_BGM_SET トレーナーBGMセット
 *  @param tr_id トレーナーID
 */
//--------------------------------------------------------------
#define _TRAINER_BGM_SET(  tr_id ) \
  _ASM_TRAINER_BGM_SET  tr_id

  .macro  _ASM_TRAINER_BGM_SET  tr_id
  .short  EV_SEQ_TRAINER_BGM_SET
  .short  \tr_id
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_LOSE トレーナー敗北
 *  @param none
 */
//--------------------------------------------------------------
#define _TRAINER_LOSE() \
  _ASM_TRAINER_LOSE

  .macro  _ASM_TRAINER_LOSE
  .short  EV_SEQ_LOSE
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_LOSE_CHECK トレーナー敗北チェック
 *  @param ret_wk 結果格納先 0=敗北 1=勝利
 */
//--------------------------------------------------------------
#define _TRAINER_LOSE_CHECK( ret_wk ) _ASM_TRAINER_LOSE_CHECK ret_wk

  .macro  _ASM_TRAINER_LOSE_CHECK ret_wk
  .short  EV_SEQ_LOSE_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * _NORMAL_LOSE  敗北処理
 * @param none
 */
//--------------------------------------------------------------
#define _NORMAL_LOSE() \
  _ASM_NORMAL_LOSE

  .macro  _ASMNORMAL_LOSE
  .short  EV_SEQ_LOSE
  .endm

//--------------------------------------------------------------
/**
 *  _LOSE_CHECK 敗北チェック
 *  @param ret_wk 結果格納先 0=敗北 1=勝利
 */
//--------------------------------------------------------------
#define  _LOSE_CHECK(  ret_wk ) \
  _ASM_LOSE_CHECK  ret_wk

  .macro  _ASM_LOSE_CHECK  ret_wk
  .short  EV_SEQ_LOSE_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _SEACRET_POKE_RETRY_CHECK 隠しポケモン再戦可不可チェック
 *  @param ret_wk 結果格納先 
 */
//--------------------------------------------------------------
  .macro  _SEACRET_POKE_RETRY_CHECK ret_wk
  .short  EV_SEQ_SEACRET_POKE_RETRY_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _HAIFU_POKE_RETRY_CHECK 配布ポケモン再戦可不可チェック
 *  @param ret_wk 結果格納先
 */
//--------------------------------------------------------------
  .macro  _HAIFU_POKE_RETRY_CHECK ret_wk
  .short  EV_SEQ_HAIFU_POKE_RETRY_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _2VS2_BATTLE_CHECK 手持ちチェック 2vs2が可能か取得
 *  @param ret_wk 結果格納先
 */
//--------------------------------------------------------------
  .macro  _2VS2_BATTLE_CHECK ret_wk
  .short  EV_SEQ_2VS2_BATTLE_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _DEBUG_BTL_SET デバック戦闘呼び出し
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _DEBUG_BTL_SET
  .short  EV_SEQ_DEBUG_BTL_SET
  .endm

//--------------------------------------------------------------
/**
 *  _BATTLE_RESULT_GET 戦闘結果を取得
 *  @param ret_wk 結果格納先
 */
//--------------------------------------------------------------
  .macro  _BATTLE_RESULT_GET ret_wk
  .short  EV_SEQ_BATTLE_RESULT_GET
  .short  \ret_wk
  .endm

//======================================================================
//  トレーナーフラグ
//======================================================================
//--------------------------------------------------------------
/**
 *  _TRAINER_FLAG_SET トレーナーフラグのセット
 *  @param tr_id セットするトレーナーID
 */
//--------------------------------------------------------------
  .macro  _TRAINER_FLAG_SET tr_id
  .short  EV_SEQ_TRAINER_FLAG_SET
  .short  \tr_id
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_FLAG_RESET トレーナーフラグのリセット
 *  @param tr_id リセットするトレーナーID
 */
//--------------------------------------------------------------
  .macro  _TRAINER_FLAG_RESET tr_id
  .short  EV_SEQ_TRAINER_FLAG_RESET
  .short  \tr_id
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_FLAG_CHECK トレーナーフラグチェック
 *  @param tr_id チェックするトレーナーID
 */
//--------------------------------------------------------------
  .macro  _TRAINER_FLAG_CHECK tr_id
  .short  EV_SEQ_TRAINER_FLAG_CHECK
  .short  \tr_id
  .endm

//--------------------------------------------------------------
/**
 *  _IF_TR_FLAGON_JUMP フラグONの時に分岐(JUMP)
 *  @param tr_id トレーナーID
 *  @param adrs 条件が一致していた際のジャンプ先アドレス
 */
//--------------------------------------------------------------
  .macro  _IF_TR_FLAGON_JUMP tr_id,adrs
  _TRAINER_FLAG_CHECK \tr_id
  _IF_JUMP  FLGON,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_TR_FLAGOFF_JUMP フラグOFFの時に分岐(JUMP)
 *  @param tr_id トレーナーID
 *  @param adrs 条件が一致していた際のジャンプ先アドレス
 */
//--------------------------------------------------------------
  .macro  _IF_TR_FLAGOFF_JUMP tr_id,adrs
  _TRAINER_FLAG_CHECK \tr_id
  _IF_JUMP  FLGOFF,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_TR_FLAGON_CALL フラグONの時に分岐(CALL)
 *  @param tr_id トレーナーID
 *  @param adrs 条件が一致していた際のジャンプ先アドレス
 */
//--------------------------------------------------------------
  .macro  _IF_TR_FLAGON_CALL tr_id,adrs
  _TRAINER_FLAG_CHECK \tr_id
  _IF_CALL  FLGON,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_TR_FLAGOFF_CALL フラグOFFの時に分岐(CALL)
 *  @param tr_id トレーナーID
 *  @param adrs 条件が一致していた際のジャンプ先アドレス
 */
//--------------------------------------------------------------
  .macro  _IF_TR_FLAGOFF_CALL tr_id,adrs
  _TRAINER_FLAG_CHECK \tr_id
  _IF_CALL  FLGOFF,\adrs
  .endm

//======================================================================
//  サウンド BGM
//======================================================================
//--------------------------------------------------------------
/**
  * _BGM_PLAY BGMを鳴らす
  * @param no BGM Number
  */
//--------------------------------------------------------------
#define _BGM_PLAY(no) _ASM_BGM_PLAY no

   .macro  _ASM_BGM_PLAY no
  .short  EV_SEQ_BGM_PLAY
  .short  \no
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_STOP BGMを止める
 *  @param none
 */
//--------------------------------------------------------------
#define _BGM_STOP() _ASM_BGM_STOP

  .macro  _ASM_BGM_STOP
  .short  EV_SEQ_BGM_STOP
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_PLAYER_PAUSE シーケンスを一時停止または再開
 *  @param flag (1=停止、0=再開)
 */
//--------------------------------------------------------------
#define _BGM_PLAYER_PAUSE(flag) _ASM_BGM_PLAYER_PAUSE

  .macro  _ASM_BGM_PLAYER_PAUSE flag
  .short  EV_SEQ_BGM_PLAYER_PAUSE
  .byte  \flag
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_FADEOUT BGMフェードアウト
 *  @param vol    = ボリューム
 *  @param frame  = 何フレームかけてフェードするか
 */
//--------------------------------------------------------------
#define _BGM_FADEOUT(vol, frame)  _ASM_BGM_FADEOUT vol, frame

  .macro  _ASM_BGM_FADEOUT vol,frame
  .short  EV_SEQ_BGM_FADEOUT
  .short  \vol
  .short  \frame
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_FADEIN BGMフェードイン
 *  @param frame = 何フレームかけてフェードするか
 */
//--------------------------------------------------------------
#define _BGM_FADEIN(frame)  _ASM_BGM_FADEIN frame

  .macro  _ASM_BGM_FADEIN frame
  .short  EV_SEQ_BGM_FADEIN
  .short  \frame
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_NOW_MAP_RECOVER 現在のマップのBGMを鳴らす
 *  @param none
 */
//--------------------------------------------------------------
#define _BGM_NOW_MAP_RECOVER()  _ASM_BGM_NOW_MAP_RECOVER

  .macro  _ASM_BGM_NOW_MAP_RECOVER
  .short  EV_SEQ_BGM_NOW_MAP_RECOVER
  .endm

//======================================================================
//  サウンド SE
//======================================================================
//--------------------------------------------------------------
/**
 *  _SE_PLAY SEを鳴らす
 *  @param no 再生するSEナンバー
 */
//-------------------------------------------------------------
#define   _SE_PLAY( no )  \
    _ASM_SE_PLAY no

.macro  _ASM_SE_PLAY  no
.short  EV_SEQ_SE_PLAY
.short  \no
.endm

//--------------------------------------------------------------
/**
 *  _SE_STOP SEを止める
 *  @param none
 */
//--------------------------------------------------------------
#define   _SE_STOP() \
  .short  EV_SEQ_SE_STOP

.macro  _ASM_SE_STOP
.short  EV_SEQ_SE_STOP
.endm
//--------------------------------------------------------------
/**
 *  _SE_WAIT SE終了待ち
 *  @param none
 */
//--------------------------------------------------------------
#define   _SE_WAIT()  _ASM_SE_WAIT

.macro  _ASM_SE_WAIT
.short  EV_SEQ_SE_WAIT
.endm

//======================================================================
//  サウンド ME
//======================================================================
//--------------------------------------------------------------
/**
 *  _ME_PLAY MEを鳴らす
 *  @param no 再生するBGMナンバー
 */
//--------------------------------------------------------------
#define _ME_PLAY( no ) _ASM_ME_PLAY no

.macro  _ASM_ME_PLAY  no
  .short  EV_SEQ_ME_PLAY
  .short  \no
.endm

//--------------------------------------------------------------
/**
 *  _ME_WAIT ME終了待ち
 *  @param none
 */
//--------------------------------------------------------------
#define _ME_WAIT() _ASM_ME_WAIT

.macro  _ASM_ME_WAIT
  .short  EV_SEQ_ME_WAIT
.endm

//======================================================================
//  サウンド 鳴き声
//======================================================================
//--------------------------------------------------------------
/**
 * ポケモン鳴き声再生
 *
 * @param monsno  ポケモンナンバー
 * @param formno  フォルムナンバー（特にない場合、０を指定）
 */
//--------------------------------------------------------------
#define _VOICE_PLAY( monsno, formno ) \
    _ASM_VOICE_PLAY monsno, formno

  .macro  _ASM_VOICE_PLAY monsno, formno
  .short  EV_SEQ_VOICE_PLAY
  .short  \monsno
  .byte  0
  .byte  0
  .endm

//--------------------------------------------------------------
/**
 * ポケモン鳴き声再生
 */
//--------------------------------------------------------------
#define _VOICE_WAIT() \
    _ASM_VOICE_WAIT

  .macro  _ASM_VOICE_WAIT
  .short  EV_SEQ_VOICE_WAIT
  .endm

//======================================================================
//  メニュー
//======================================================================
//--------------------------------------------------------------
/**
 *  _BMPMENU_INIT BMPメニュー　初期化
 *  @param x メニュー左上X座標 キャラ単位
 *  @param y メニュー左上Y座標 キャラ単位
 *  @param cursor カーソル初期位置
 *  @param cancel Bボタンキャンセル有効フラグ 1=有効 0=無効
 *  @param ret_wk 結果格納先ワークID
 */
//--------------------------------------------------------------
#define _BMPMENU_INIT( x,y,cursor,cancel,ret_wk ) \
    _ASM_BMPMENU_INIT x,y,cursor,cancel,ret_wk

   .macro  _ASM_BMPMENU_INIT x,y,cursor,cancel,ret_wk
  .short  EV_SEQ_BMPMENU_INIT
  .byte  \x
  .byte  \y
  .byte  \cursor
  .byte  \cancel
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _BMPMENU_INIT_EX BMPメニュー　初期化(読み込んでいるgmmファイルを使用する)
 *  @param x メニュー左上X座標 キャラ単位
 *  @param y メニュー左上Y座標 キャラ単位
 *  @param cursor カーソル初期位置
 *  @param cancel Bボタンキャンセル有効フラグ 1=有効 0=無効
 *  @param ret_wk 結果格納先ワークID
 */
//--------------------------------------------------------------
#define _BMPMENU_INIT_EX( x,y,cursor,cancel,ret_wk ) \
   _ASM_BMPMENU_INIT_EX x,y,cursor,cancel,ret_wk

   .macro  _ASM_BMPMENU_INIT_EX x,y,cursor,cancel,ret_wk
  .short  EV_SEQ_BMPMENU_INIT_EX
  .byte  \x
  .byte  \y
  .byte  \cursor
  .byte  \cancel
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _BMPMENU_MAKE_LIST BMPメニュー　リスト作成
 *  @param msg_id メッセージID
 *  @param param メッセージが選択された際に結果として返す値
 */
//--------------------------------------------------------------
#define _BMPMENU_MAKE_LIST( msg_id, param ) \
    _ASM_BMPMENU_MAKE_LIST msg_id, param

  .macro  _ASM_BMPMENU_MAKE_LIST msg_id,param
  .short  EV_SEQ_BMPMENU_MAKE_LIST
  .short  \msg_id
  .short  \param
  .endm

//--------------------------------------------------------------
/**
 *  _BMPMENU_START BMPメニュー　開始
 *  @param none
 */
//--------------------------------------------------------------
#define _BMPMENU_START() _ASM_BMPMENU_START

  .macro  _ASM_BMPMENU_START
  .short  EV_SEQ_BMPMENU_START
  .endm

//======================================================================
//  画面フェード
//======================================================================
//--------------------------------------------------------------
//  フェードモード(fade.h)
//--------------------------------------------------------------
#define DISP_FADE_BLACKOUT_MAIN (0x01) ///<メイン画面フェード ブラックアウト
#define DISP_FADE_BLACKOUT_SUB  (0x02) ///<サブ画面フェード ブラックアウト
#define DISP_FADE_BLACKOUT (0x03) ///<両画面フェード ブラックアウト
#define DISP_FADE_WHITEOUT_MAIN (0x04) ///<メイン画面フェード ホワイトアウト
#define DISP_FADE_WHITEOUT_SUB  (0x08) ///<サブ画面フェード ホワイトアウト
#define DISP_FADE_WHITEOUT (0x0c) ///<両画面フェード ホワイトアウト

//--------------------------------------------------------------
/**
 *  _DISP_FADE_START 画面フェードスタート
 *  @param mode 画面フェードモード 
 *  @param start_evy 初期輝度(0=元の色〜16=指定輝度）
 *  @param end_evy 終了輝度(0=元の色〜16=指定輝度)
 *  @param speed フェードスピード 0〜
 */
//--------------------------------------------------------------
#define _DISP_FADE_START( mode,start_evy,end_evy,speed ) _ASM_DISP_FADE_START mode,start_evy,end_evy,speed

  .macro  _ASM_DISP_FADE_START mode,start_evy,end_evy,speed
  .short EV_SEQ_DISP_FADE_START
  .short \mode
  .short \start_evy
  .short \end_evy
  .short \speed
  .endm


//--------------------------------------------------------------
/**
 *  _DISP_FADE_START 画面フェード終了チェック
 *  @param none
 */
//--------------------------------------------------------------
#define _DISP_FADE_END_CHECK()  _ASM_DISP_FADE_END_CHECK

  .macro  _ASM_DISP_FADE_END_CHECK
  .short EV_SEQ_DISP_FADE_CHECK
  .endm


//--------------------------------------------------------------
/**
 *  @def  _WHITE_OUT
 *  @brief  ホワイトアウト
 *  @param speed フェードスピード 0〜
 */
//--------------------------------------------------------------
#define _WHITE_OUT( speed ) _DISP_FADE_START( DISP_FADE_WHITEOUT,0,16,speed )

//--------------------------------------------------------------
/**
 *  @def  _WHITE_IN
 *  @brief  ホワイトイン
 *  @param speed フェードスピード 0〜
 */
//--------------------------------------------------------------
#define _WHITE_IN( speed ) _DISP_FADE_START( DISP_FADE_WHITEOUT,16,0,speed )

//--------------------------------------------------------------
/**
 *  @def  _BLACK_OUT
 *  @brief  ブラックアウト
 *  @param  speed フェードスピード 0〜
 */
//--------------------------------------------------------------
#define _BLACK_OUT( speed ) _DISP_FADE_START( DISP_FADE_BLACKOUT,0,16,speed )

//--------------------------------------------------------------
/**
 *  @def  _BLACK_IN
 *  @brief  ブラックイン
 *  @param  speed フェードスピード 0〜
 */
//--------------------------------------------------------------
#define _BLACK_IN( speed ) _DISP_FADE_START( DISP_FADE_BLACKOUT,16,0,speed )

//======================================================================
//  アイテム関連
//======================================================================
//--------------------------------------------------------------
/**
 *	アイテムを加える
 *	@param item_no 加えるアイテムナンバー
 *	@param num 加える個数
 *	@param ret_wk 結果代入先 TRUE=成功 FALSE=失敗
 */
//--------------------------------------------------------------
#define _ADD_ITEM( item_no, num, ret_wk )  _ASM_ADD_ITEM item_no,num,ret_wk

	.macro	_ASM_ADD_ITEM	item_no,num,ret_wk
	.short	EV_SEQ_ADD_ITEM
	.short	\item_no
	.short	\num
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 *	アイテムを減らす
 *	@param item_no 減らすアイテムナンバー
 *	@param num 減らす個数
 *	@param ret_wk 結果代入先 TRUE=成功 FALSE=失敗
 */
//--------------------------------------------------------------
#define _SUB_ITEM( item_no,num,ret_wk ) _ASM_SUB_ITEM item_no,num_ret_wk

	.macro	_ASM_SUB_ITEM item_no, num, ret_wk
	.short	EV_SEQ_SUB_ITEM
	.short	\item_no
	.short	\num
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 *	アイテムを加えられるかチェック
 *	@param item_no 加えるアイテムナンバー
 *	@param num 加える個数
 *	@param ret_wk 結果代入先 TRUE=可能 FALSE=不可能
 */
//--------------------------------------------------------------
#define _ADD_ITEM_CHK( item_no, num, ret_wk ) _ASM_ADD_ITEM_CHK item_no,num,ret_wk

	.macro	_ASM_ADD_ITEM_CHK	item_no,num,ret_wk
	.short	EV_SEQ_ADD_ITEM_CHK
	.short	\item_no
	.short	\num
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 *	バッグのアイテムチェック
 *	@param item_no チェックするアイテムナンバー
 *	@param num チェックする個数
 *	@param ret_wk 結果代入先 TRUE=存在 FALSE=存在しない
 */
//--------------------------------------------------------------
#define _ITEM_CHK( item_no, num, ret_wk ) _ASM_ITEM_CHK item_no,num,ret_wk

	.macro	_ASM_ITEM_CHK		item_no,num,ret_wk
	.short	EV_SEQ_ITEM_CHK
	.short	\item_no
	.short	\num
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 *	バッグのアイテム個数取得
 *	@param item_no 取得するアイテムナンバー
 *	@param ret_wk 結果代入先
 */
//--------------------------------------------------------------
#define _GET_ITEM_NUM( item_no, ret_wk ) _ASM_GET_ITEM_NUM		item_no,ret_wk

	.macro	_ASM_GET_ITEM_NUM		item_no,ret_wk
	.short	EV_SEQ_GET_ITEM_NUM
	.short	\item_no
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 *	技マシンのアイテムナンバーかチェック
 *	@param item_no チェックするアイテムナンバー
 *	@param ret_wk 結果代入先
 *	@retval TRUE=技マシンである
 *	@retval FALSE=技マシンではない
 */
//--------------------------------------------------------------
#define _WAZA_ITEM_CHK( item_no,ret_wk ) _ASM_WAZA_ITEM_CHK item_no,ret_wk

	.macro	_ASM_WAZA_ITEM_CHK		item_no,ret_wk
	.short	EV_SEQ_WAZA_ITEM_CHK
	.short	\item_no
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 * @def _GET_POCKET_ID
 * @brief ポケットIDの取得する
 * @param item_no   アイテムナンバー
 * @param ret_wk    結果を受け取るワーク
 *
 * item_noで指定したアイテムに対応したバッグのポケットIDを取得する
 */
//--------------------------------------------------------------
#define _GET_POCKET_ID( item_no, ret_wk )   \
    _ASM_GET_POCKET_ID item_no, ret_wk


    .macro  _ASM_GET_POCKET_ID item_no, ret_wk
    .short  EV_SEQ_GET_POCKET_ID
    .short  \item_no
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * フィールドのアイテムイベント呼び出し
 * @param no   アイテムナンバー
 * @param num 加える値
 */
//--------------------------------------------------------------
.macro _FLD_ITEM_EVENT no,num
  _ASM_LDVAL SCWK_REG0,\no
  _ASM_LDVAL SCWK_REG1,\num
  _CALL ev_fld_item2
.endm

//======================================================================
//  会話イベント関連
//======================================================================
//--------------------------------------------------------------
/**
 *  _TALK_START_SE_PLAY
 *  会話イベント開始時のSE再生
 *  @param none
 */
//--------------------------------------------------------------
#define _TALK_START_SE_PLAY() _ASM_TALK_START_SE_PLAY

  .macro  _ASM_TALK_START_SE_PLAY
  _ASM_SE_PLAY( SEQ_SE_MESSAGE )
  .endm

//--------------------------------------------------------------
/**
 *  _TALK_OBJ_START
 *  @brief  OBJに対して話し掛け開始(主人公に対して振り向き有り)
 *
 * @note
 *  内部的には_TALK_START_SE_PLAY()と_TURN_HERO_SITE()を呼び出すのと同機能。
 *  今後の会話関連の機能拡張に備えて、これを呼び出すようにしておく。
 */
//--------------------------------------------------------------
#define _TALK_OBJ_START() _ASM_TALK_OBJ_START

.macro  _ASM_TALK_OBJ_START
    _ASM_TALK_START_SE_PLAY
    _ASM_TURN_HERO_SITE
.endm

//--------------------------------------------------------------
/**
 * @def _TALK_START_TURN_NOT
 * @brief  OBJに対して話し掛け開始(主人公に対して振り向きなし）
 *
 * @note
 *  内部的には_TALK_START_SE_PLAY()を呼び出すのと同機能。
 *  今後の会話関連の機能拡張に備えて、これを呼び出すようにしておく。
 */
//--------------------------------------------------------------
#define _TALK_START_TURN_NOT() _TALK_START_SE_PLAY()


//======================================================================
//  ミュージカル関連
//======================================================================
//--------------------------------------------------------------
/**
 *  _MUSICAL_CALL ミュージカル：呼び出し
 *  @param none
 */
//--------------------------------------------------------------
#define _MUSICAL_CALL() _ASM_MUSICAL_CALL

  .macro  _ASM_MUSICAL_CALL
  .short EV_SEQ_MUSICAL_CALL
  .endm

//--------------------------------------------------------------
/**
 *  _MUSICAL_TITLE_NAME ミュージカル：演目名をタグに設定(通常使用可
 *  @param idx セットするタグナンバー
 */
//--------------------------------------------------------------
#define _MUSICAL_TITLE_NAME( idx ) _ASM_MUSICAL_TITLE_NAME

  .macro  _ASM_MUSICAL_TITLE_NAME
  .short EV_SEQ_MUSICAL_TITLE_NAME
  .byte   \idx
  .endm


//======================================================================
//  レポート関連
//======================================================================
//--------------------------------------------------------------
/**
 * @def _REPORT_CALL
 * @brief   セーブ機能呼び出し
 * @param ret_wk    結果を受け取るワーク
 *
 * 共通スクリプトのレポート機能呼び出しから利用されている。
 * 通常のスクリプトから直接呼び出すことはない。
 */
//--------------------------------------------------------------
#define _REPORT_CALL(ret_wk)  \
    _ASM_REPORT_CALL  ret_wk

  .macro  _ASM_REPORT_CALL  ret_wk
  .short  EV_SEQ_REPORT_CALL
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _REPORT_CHECK
 * @brief セーブ状況の確認
 * @param ret_exist  [out]セーブデータが存在するかどうか
 * @param ret_new    [out]現在のプレイデータが新規データかどうか
 * @param ret_volume [out]セーブするデータ量
 */
//--------------------------------------------------------------
#define _REPORT_CHECK( ret_exist, ret_new, ret_volume )  \
    _ASM_REPORT_CHECK  ret_exist, ret_new, ret_volume 

  .macro  _ASM_REPORT_CHECK ret_exist, ret_new, ret_volume 
  .short  EV_SEQ_REPORT_CHECK
  .short  \ret_exist
  .short  \ret_new
  .short  \ret_volume 
  .endm

//--------------------------------------------------------------
/**
 * @def _FIELD_COMM_EXIT
 * @brief フィールドマップの通信を終了する
 *
 * コモン処理 _FIELD_COMM_EXIT_EVENT_CALL を作製してあります。
 * 原則として単体での呼び出しはせず、コモン処理を呼んでください
 */
//--------------------------------------------------------------
#define _FIELD_COMM_EXIT() \
    _ASM_FIELD_COMM_EXIT

  .macro  _ASM_FIELD_COMM_EXIT
  .short  EV_SEQ_FIELD_COMM_EXIT
  .endm

//======================================================================
//  プログラム管理データの取得・セット
//======================================================================
//--------------------------------------------------------------
/**
 *  @def  _CHG_LANGID
 *  @brief  言語ID切り替え ひらがな<->漢字
 *  @param none
 */
//--------------------------------------------------------------
#define   _CHG_LANGID() _ASM_CHG_LANGID

  .macro  _ASM_CHG_LANGID
  .short  EV_SEQ_CHG_LANGID
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_LANG_ID
 * @brief 言語モードIDの取得
 * @param ret_wk
 * @retval  0   ひらがなモード
 * @retval  1   漢字モード
 */
//--------------------------------------------------------------
#define _GET_LANG_ID(ret_wk) \
    _ASM_GET_LANG_ID ret_wk

  .macro  _ASM_GET_LANG_ID
  .short  EV_SEQ_GET_LANG_ID
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  @def  _GET_RND
 *  @brief  ランダム数取得
 *  @param ret_wk 乱数格納先
 *  @param num 乱数上限値
 *
 *  @return 乱数（値の範囲は０〜 num-1 までの間）
 */
//--------------------------------------------------------------
#define _GET_RND( ret_wk, num ) _ASM_GET_RND ret_wk, num

  .macro  _ASM_GET_RND ret_wk, num
  .short  EV_SEQ_GET_RND
  .short  \ret_wk
  .short  \num
  .endm

//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _ASM_GET_NOW_MSG_ARCID  work
  .short  EV_SEQ_GET_NOW_MSG_ARCID
  .short  \work
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TIMEZONE
 * @brief 時間帯の取得
 * @param ret_wk    チェック結果を受け取るワーク
 *
 * 戻り値はprog/include/system/timezone.hを参照
 */
//--------------------------------------------------------------
#define _GET_TIMEZONE( ret_wk ) \
    _ASM_GET_TIMEZONE ret_wk

  .macro  _ASM_GET_TIMEZONE ret_wk
  .short  EV_SEQ_GET_TIMEZONE
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TRAINER_CARD_RANK
 * @brief トレーナーカードランクの取得
 * @param ret_wk    結果を受け取るワーク
 */
//--------------------------------------------------------------
#define _GET_TRAINER_CARD_RANK( ret_wk ) \
    _ASM_GET_TRAINER_CARD_RANK ret_wk

  .macro  _ASM_GET_TRAINER_CARD_RANK ret_wk
  .short  EV_SEQ_GET_TRAINER_CARD_RANK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_WEEK
 * @brief 曜日の取得
 * @param ret_wk    結果を受け取るワーク
 */
//--------------------------------------------------------------
#define _GET_WEEK( ret_wk ) \
    _ASM_GET_WEEK ret_wk

  .macro  _ASM_GET_WEEK ret_wk
  .short  EV_SEQ_GET_WEEK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_BADGE_FLAG
 * @brief バッジフラグの取得
 * @param ret_wk    結果を受けるワーク
 * @param badge_id  取得するバッジのID
 */
//--------------------------------------------------------------
#define _GET_BADGE_FLAG( ret_wk, badge_id ) \
    _ASM_GET_BADGE_FLAG ret_wk, badge_id

    .macro  _ASM_GET_BADGE_FLAG ret_wk, badge_id
    .short  EV_SEQ_GET_BADGE_FLAG
    .short  \ret_wk
    .short  \badge_id
    .endm

//--------------------------------------------------------------
/**
 * @def _SET_BADGE_FLAG
 * @brief バッジフラグのセット
 * @param badge_id  セットするバッジのID
 */
//--------------------------------------------------------------
#define _SET_BADGE_FLAG( badge_id ) \
    _ASM_SET_BADGE_FLAG badge_id

    .macro  _ASM_SET_BADGE_FLAG badge_id
    .short  EV_SEQ_SET_BADGE_FLAG
    .short  \badge_id
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_BADGE_COUNT
 * @brief バッジ数の取得
 * @param ret_wk    結果を受けるワーク
 */
//--------------------------------------------------------------
#define _GET_BADGE_COUNT( ret_wk ) \
    _ASM_GET_BADGE_COUNT ret_wk

    .macro  _ASM_GET_BADGE_COUNT ret_wk
    .short  EV_SEQ_GET_BADGE_COUNT
    .short  \ret_wk
    .endm


//======================================================================
//
//
//    ポケモン関連
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * 手持ちポケモン回復
 */
//--------------------------------------------------------------
#define _TEMOTI_POKEMON_KAIFUKU() \
    _ASM_TEMOTI_POKEMON_KAIFUKU

  .macro  _ASM_TEMOTI_POKEMON_KAIFUKU
  .short  EV_SEQ_POKEMON_RECOVER
  .endm

//--------------------------------------------------------------
/**
 * @def _PARTY_POKE_NAME_INPUT
 * @brief 手持ちポケモンの名前入力画面を呼び出す
 * @param ret_wk 入力結果を受け取るワーク
 * @param index  名前を設定する手持ちポケモンのインデックスを指定
 * @retval  TRUEのとき、入力した名前を確定した場合
 * @retval  FALSEのとき、入力をキャンセルした場合
 */
//--------------------------------------------------------------
#define _PARTY_POKE_NAME_INPUT( ret_wk, index ) \
    _ASM_PARTY_POKE_NAME_INPUT ret_wk, index

  .macro  _ASM_PARTY_POKE_NAME_INPUT ret_wk, index
  .short  EV_SEQ_PARTY_POKE_NAME_INPUT
  .short  \ret_wk
  .short  \index
  .endm

//--------------------------------------------------------------
/**
 * @def _PARTY_POKE_EGG_BIRTH
 * @brief 手持ちのタマゴを孵化させる
 * @param ret_wk 生まれたポケモンの手持ちインデックスを受け取るワーク
 */
//--------------------------------------------------------------
#define _PARTY_POKE_EGG_BIRTH( ret_wk ) \
    _ASM_PARTY_POKE_EGG_BIRTH ret_wk

  .macro  _ASM_PARTY_POKE_EGG_BIRTH ret_wk
  .short  EV_SEQ_PARTY_POKE_EGG_BIRTH
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_TEMOTI_POKERUS
 * @brief 手持ちにポケルスがいるかどうかのチェック
 * @param ret_wk    チェック結果を受け取るワーク
 * @retval  TRUEのとき、ポケルス感染したポケモンが手持ちにいる
 * @retval  FALSEのとき、感染したポケモンはいない
 */
//--------------------------------------------------------------
#define _CHECK_TEMOTI_POKERUS( ret_wk ) \
    _ASM_CHECK_TEMOTI_POKERUS ret_wk

  .macro  _ASM_CHECK_TEMOTI_POKERUS ret_wk
  .short  EV_SEQ_CHECK_TEMOTI_POKERUS
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_TEMOTI_HP
 * @brief 手持ちポケモンのHPチェック
 * @param ret_wk  チェック結果を受け取るワーク
 * @param pos     チェックするポケモンの位置（０〜５）
 * @retval  TRUE    チェックしたポケモンのHPが満タン
 * @retval  FALSE   チェックしたポケモンのHPは減っている
 */
//--------------------------------------------------------------
#define _CHECK_TEMOTI_HP( ret_wk, pos ) \
    _ASM_CHECK_TEMOTI_HP ret_wk, pos


  .macro  _ASM_CHECK_TEMOTI_HP ret_wk, pos
  .short  EV_SEQ_CHECK_TEMOTI_HP
  .short  \ret_wk
  .short  \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_TEMOTI_EGG
 * @brief 手持ちポケモンのEGGチェック
 * @param ret_wk  チェック結果を受け取るワーク
 * @param pos     チェックするポケモンの位置（０〜５）
 * @retval  TRUE    チェックしたポケモンがタマゴの場合
 * @retval  FALSE   チェックしたポケモンがタマゴでない場合
 */
//--------------------------------------------------------------
#define _CHECK_TEMOTI_EGG( ret_wk, pos ) \
    _ASM_CHECK_TEMOTI_EGG ret_wk, pos


  .macro  _ASM_CHECK_TEMOTI_EGG ret_wk, pos
  .short  EV_SEQ_CHECK_TEMOTI_EGG
  .short  \ret_wk
  .short  \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_PARTY_POKE_COUNT
 * @brief 手持ちポケモンの数を取得
 * @param ret_wk チェック結果を受け取るワーク
 	!!注意!!
 	返り値は「１」から始まります。手持ちのポジション指定は「0」始まりなので合わせて使う場合は注意。
 * @param count_mode 数え方指定(POKECOUNT_MODE_XXXX)
 		手持ちポケモン数カウントモード
 		POKECOUNT_MODE_TOTAL         (0)  // 手持ちの数
 		POKECOUNT_MODE_NOT_EGG       (1)  // タマゴを除く手持ちの数
 		POKECOUNT_MODE_BATTLE_ENABLE (2)  // 戦える(タマゴと瀕死を除いた)ポケモン数
 		POKECOUNT_MODE_ONLY_EGG      (3)  // タマゴの数(駄目タマゴを除く) 
 		POKECOUNT_MODE_ONLY_DAME_EGG (4)  // 駄目タマゴの数
 	* @retval u16 手持ちポケモンの数
 */
//--------------------------------------------------------------
#define _GET_PARTY_POKE_COUNT( ret_wk, count_mode ) \
    _ASM_GET_PARTY_POKE_COUNT ret_wk, count_mode

  .macro _ASM_GET_PARTY_POKE_COUNT ret_wk, count_mode
  .short EV_SEQ_GET_PARTY_POKE_COUNT
  .short \ret_wk
  .short \count_mode
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_PARTY_POKE_MONSNO
 * @brief 指定した手持ちポケモンのモンスターナンバーを取得する
 * @param ret_wk 結果を受け取るワーク
 * @param pos    モンスターナンバーを調べる手持ちポケモン番号
 * @retval u16   指定ポケモンのモンスターナンバー
 */
//--------------------------------------------------------------
#define _GET_PARTY_POKE_MONSNO( ret_wk, pos ) \
    _ASM_GET_PARTY_POKE_MONSNO ret_wk, pos

  .macro _ASM_GET_PARTY_POKE_MONSNO ret_wk, pos
  .short EV_SEQ_GET_PARTY_POKE_MONSNO
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_PARTY_POKE_FORMNO
 * @brief 指定した手持ちポケモンの形状ナンバーを取得する
 * @param ret_wk 結果を受け取るワーク
 * @param pos    形状ナンバーを調べる手持ちポケモン番号
 * @retval u16   指定ポケモンの形状ナンバー
 */
//--------------------------------------------------------------
#define _GET_PARTY_POKE_FORMNO( ret_wk, pos ) \
    _ASM_GET_PARTY_POKE_FORMNO ret_wk, pos

  .macro _ASM_GET_PARTY_POKE_FORMNO ret_wk, pos
  .short EV_SEQ_GET_PARTY_POKE_FORMNO
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _ADD_POKEMON_TO_PARTY
 * @brief ポケモンを手持ちに追加する
 * @param ret_wk 結果を受け取るワーク
 * @param monsno  モンスターナンバー
 * @param formno  フォームナンバー
 * @param tokusei 特性
 * @param level   レベル
 * @param itemno  所持アイテム
 * @param 
 * @retval BOOL 追加できたらTRUE
 */
//--------------------------------------------------------------
#define _ADD_POKEMON_TO_PARTY( ret_wk, monsno, formno, tokusei, level, itemno ) \
    _ASM_ADD_POKEMON_TO_PARTY ret_wk, monsno, formno, tokusei, level, itemno

  .macro _ASM_ADD_POKEMON_TO_PARTY ret_wk, monsno, formno, tokusei, level, itemno
  .short EV_SEQ_ADD_POKEMON_TO_PARTY
  .short \ret_wk
  .short \monsno
  .short \formno
  .short \tokusei
  .short \level
  .short \itemno
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_POKE_WAZA
 * @brief 指定された技を覚えているか調べる
 * @param ret_wk チェック結果を格納するワーク 0=覚えていない 1=覚えている
 * @param wazano チェックする技ナンバー
 * @param tno 調べる手持ちポケモンの一番号
 */
//--------------------------------------------------------------
#define _CHECK_POKE_WAZA( ret_wk, wazano, tno ) \
    _ASM_CHECK_POKE_WAZA ret_wk, wazano, tno

  .macro _ASM_CHECK_POKE_WAZA ret_wk, wazano, tno
  .short EV_SEQ_CHK_POKE_WAZA
  .short \ret_wk
  .short \wazano
  .short \tno
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_POKE_WAZA_GROUP 
 * @brief 指定された技を覚えているか調べる（手持ち全体）
 * @param ret_wk チェック結果を格納するワーク 技を覚えている手持ち位置。 6=覚えていない
 * @param wazano チェックする技ナンバー
 */
//--------------------------------------------------------------
#define _CHECK_POKE_WAZA_GROUP( ret_wk, wazano ) \
    _ASM_CHECK_POKE_WAZA_GROUP ret_wk, wazano

  .macro _ASM_CHECK_POKE_WAZA_GROUP ret_wk, wazano
  .short EV_SEQ_CHK_POKE_WAZA_GROUP
  .short \ret_wk
  .short \wazano
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TEMOTI_NATSUKI
 * @brief 手持ちポケモンのなつき度チェック
 * @param ret_wk  チェック結果を受け取るワーク
 * @param pos     チェックするポケモンの位置（０〜５）
 * @return        チェックしたポケモンのなつき度
 */
//--------------------------------------------------------------
#define _GET_TEMOTI_NATSUKI( ret_wk, pos ) \
    _ASM_CHECK_GET_TEMOTI_NATSUKI ret_wk, pos

  .macro  _ASM_CHECK_GET_TEMOTI_NATSUKI ret_wk, pos
  .short  EV_SEQ_GET_TEMOTI_NATSUKI
  .short  \ret_wk
  .short  \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _SET_TEMOTI_NATSUKI
 * @brief 手持ちポケモンのなつき度をセット
 * @param pos     なつき度を操作するポケモンの位置（０〜５）
 * @param value   設定したいなつき度
 * @param mode    コマンドの呼び出しモード(0:セット,1:加算,2:減算)
 */
//--------------------------------------------------------------
#define _SET_TEMOTI_NATSUKI( pos, value ) \
    _ASM_CHECK_SET_TEMOTI_NATSUKI pos, value

  .macro  _ASM_CHECK_SET_TEMOTI_NATSUKI pos,value
  .short  EV_SEQ_GET_TEMOTI_NATSUKI
  .short  \ret_wk
  .short  \pos
  .byte   0
  .endm

//--------------------------------------------------------------
/**
 * @def _ADD_TEMOTI_NATSUKI
 * @brief 手持ちポケモンのなつき度を加算
 * @param pos     なつき度を操作するポケモンの位置（０〜５）
 * @param value   加算したい値
 * @param mode    コマンドの呼び出しモード(0:セット,1:加算,2:減算)
 */
//--------------------------------------------------------------
#define _ADD_TEMOTI_NATSUKI( pos, value ) \
    _ASM_CHECK_ADD_TEMOTI_NATSUKI pos, value

  .macro  _ASM_CHECK_ADD_TEMOTI_NATSUKI pos,value
  .short  EV_SEQ_SET_TEMOTI_NATSUKI
  .short  \pos
  .short  \value
  .byte   1
  .endm

//--------------------------------------------------------------
/**
 * @def _SUB_TEMOTI_NATSUKI
 * @brief 手持ちポケモンのなつき度を減算
 * @param pos     なつき度を操作するポケモンの位置（０〜５）
 * @param value   減算したい値
 * @param mode    コマンドの呼び出しモード(0:セット,1:加算,2:減算)
 */
//--------------------------------------------------------------
#define _SUB_TEMOTI_NATSUKI( pos, value ) \
    _ASM_CHECK_SUB_TEMOTI_NATSUKI pos, value

  .macro  _ASM_CHECK_SUB_TEMOTI_NATSUKI pos,value
  .short  EV_SEQ_SET_TEMOTI_NATSUKI
  .short  \pos
  .short  \value
  .byte   2
  .endm

//--------------------------------------------------------------
/**
 * @def _SELECT_TEMOTI_POKE
 * @brief 汎用手持ちポケモン選択
 * @param decide 選択結果の格納先 (TRUE：FALSE 選択したか)
 * @param ret_wk 選択結果の格納先 (０〜５ 選択位置)
 * @param value  予備ワーク (０を入れておいてください
 * @retval u16   選択結果
 */
//--------------------------------------------------------------
#define _SELECT_TEMOTI_POKE( decide , ret_wk , value ) \
    _ASM_SELECT_TEMOTI_POKE decide, ret_wk, value

  .macro  _ASM_SELECT_TEMOTI_POKE decide, ret_wk, value
  .short  EV_SEQ_PARTY_POKE_SELECT
  .short  \decide
  .short  \ret_wk
  .short  \value
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TEMOTI_WAZA_NUM
 * @brief 手持ちポケモンの技個数取得
 * @param ret_wk 選択結果の格納先 
 * @param pos    対象の位置
 * @retval u16   選択結果
 */
//--------------------------------------------------------------
#define _GET_TEMOTI_WAZA_NUM( ret_wk, pos ) \
    _ASM_GET_TEMOTI_WAZA_NUM ret_wk, pos

  .macro  _ASM_GET_TEMOTI_WAZA_NUM ret_wk, pos
  .short  EV_SEQ_GET_TEMOTI_WAZA_NUM
  .short  \ret_wk
  .short  \pos
  .endm


//--------------------------------------------------------------
/**
 * @def _SELECT_WAZA_TEMOTI_POKE
 * @brief 汎用手持ちポケモンの技選択
 * @param decide 選択結果の格納先 (TRUE：FALSE 選択したか)
 * @param ret_wk 選択結果の格納先 (０〜３ 選択位置)
 * @param poke_pos ポケモンの位置
 * @param value  予備ワーク (０を入れておいてください
 * @retval u16   選択結果
 */
//--------------------------------------------------------------
#define _SELECT_WAZA_TEMOTI_POKE( decide , ret_wk , poke_pos , value ) \
    _ASM_SELECT_WAZA_TEMOTI_POKE decide, ret_wk, poke_pos, value

  .macro  _ASM_SELECT_WAZA_TEMOTI_POKE decide, ret_wk, poke_pos, value
  .short  EV_SEQ_PARTY_POKE_WAZA_SELECT
  .short  \decide
  .short  \ret_wk
  .short  \poke_pos
  .short  \value
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TEMOTI_WAZA_ID
 * @brief 汎用手持ちポケモンの技番号取得
 * @param ret_wk 選択結果の格納先 (技ID)
 * @param poke_pos ポケモンの位置
 * @param waza_pos 技の位置
 * @retval u16   選択結果
 */
//--------------------------------------------------------------
#define _GET_TEMOTI_WAZA_ID( ret_wk , poke_pos , waza_pos ) \
    _ASM_GET_TEMOTI_WAZA_ID ret_wk, poke_pos, waza_pos

  .macro  _ASM_GET_TEMOTI_WAZA_ID ret_wk, poke_pos, waza_pos
  .short  EV_SEQ_GET_TEMOTI_WAZA_ID
  .short  \ret_wk
  .short  \poke_pos
  .short  \waza_pos
  .endm

//--------------------------------------------------------------
/**
 * @def _ADD_TEMOTI_WAZA
 * @brief 手持ちポケモンの技セット(追加・置き換え・忘れ対応
 * @param poke_pos ポケモンの位置
 * @param waza_pos 技の位置(5で技を押し出し式で追加
 * @param waza_id  技の番号(0で技を忘れる
 * @retval u16   選択結果
 */
//--------------------------------------------------------------
#define _SET_TEMOTI_WAZA( poke_pos , waza_pos , waza_id ) \
    _ASM_SET_TEMOTI_WAZA poke_pos, waza_pos, waza_id 

  .macro  _ASM_SET_TEMOTI_WAZA poke_pos, waza_pos, waza_id 
  .short  EV_SEQ_SET_TEMOTI_WAZA
  .short  \poke_pos
  .short  \waza_pos
  .short  \waza_id
  .endm

//======================================================================
// お金
//======================================================================
//--------------------------------------------------------------
/**
 * @def _ADD_GOLD
 * @brief お金を増やす
 * @param value 増やす金額(円)
 */
//--------------------------------------------------------------
#define _ADD_GOLD( value ) \
    _ASM_ADD_GOLD value

  .macro _ASM_ADD_GOLD value
  .short EV_SEQ_ADD_GOLD
  .short \value
  .endm

//--------------------------------------------------------------
/**
 * @def _SUBTRACT_GOLD
 * @brief お金を減らす
 * @param value 減らす金額(円)
 */
//--------------------------------------------------------------
#define _SUBTRACT_GOLD( value ) \
    _ASM_SUBTRACT_GOLD value

  .macro _ASM_SUBTRACT_GOLD value
  .short EV_SEQ_SUBTRACT_GOLD
  .short \value
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_GOLD
 * @brief 指定金額を持っているかどうかチェックする
 * @param ret_wk 判定結果を受け取るワーク
 * @param value  判定する金額(円)
 * @retval TRUE  指定金額を持っている場合
 * @retval FALSE 指定金額を持っていない場合
 */
//--------------------------------------------------------------
#define _CHECK_GOLD( ret_wk, value ) \
    _ASM_CHECK_GOLD ret_wk, value

  .macro _ASM_CHECK_GOLD ret_wk, value
  .short EV_SEQ_CHECK_GOLD
  .short \ret_wk
  .short \value
  .endm


//======================================================================
// ボックス関連
//======================================================================

//--------------------------------------------------------------
/**
 * @def _GET_BOX_POKE_COUNT
 * @brief ボックス内のポケモン数を取得する
 * @param ret_wk ポケモン数を受け取るワーク
 * @param mode カウントモード( POKECOUNT_MODE_XXXX )
 */
//--------------------------------------------------------------
#define _GET_BOX_POKE_COUNT( ret_wk, mode ) \
    _ASM_GET_BOX_POKE_COUNT ret_wk, mode

  .macro _ASM_GET_BOX_POKE_COUNT ret_wk, mode
  .short EV_SEQ_GET_BOX_POKE_COUNT
  .short \ret_wk
  .short \mode
  .endm
    
  
//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * マップ遷移コマンド（流砂）
 * @param zone_id   移動先ゾーンID指定
 * @param gx        移動先Ｘ位置指定（グリッド単位）
 * @param gz        移動先Ｙ位置指定（グリッド単位）
 */
//--------------------------------------------------------------
#define _MAP_CHANGE_SAND_STREAM( zone_id, gx, gz )  \
    _ASM_MAP_CHANGE_SAND_STREAM zone_id, gx, gz

  .macro  _ASM_MAP_CHANGE_SAND_STREAM zone_id, gx, gz
  .short  EV_SEQ_MAP_CHANGE_SAND_STREAM
  .short  \zone_id
  .short  \gx
  .short  \gz
  .endm

//--------------------------------------------------------------
/**
 * マップ遷移コマンド
 * @param zone_id   移動先ゾーンID指定
 * @param gx        移動先Ｘ位置指定（グリッド単位）
 * @param gz        移動先Ｙ位置指定（グリッド単位）
 * @param dir       移動後の向き
 */
//--------------------------------------------------------------
#define _MAP_CHANGE( zone_id, gx, gz, dir )  \
    _ASM_MAP_CHANGE zone_id, gx, gz, dir

  .macro  _ASM_MAP_CHANGE zone_id, gx, gz, dir
  .short  EV_SEQ_MAP_CHANGE
  .short  \zone_id
  .short  \gx
  .short  \gz
  .short  \dir
  .endm

//--------------------------------------------------------------
/**
 * マップ遷移コマンド(ユニオンルーム)
 */
//--------------------------------------------------------------
#define _MAP_CHANGE_TO_UNION() \
    _ASM_MAP_CHANGE_TO_UNION 

  .macro  _ASM_MAP_CHANGE_TO_UNION 
  .short  EV_SEQ_MAP_CHANGE_TO_UNION
  .endm

//======================================================================
//
//
//  特殊状況ギミック関連
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * ジムコマンド　電気ジム初期化
 */
//--------------------------------------------------------------
#define _GYM_ELEC_INIT( evt1, evt2 ) \
    _ASM_GYM_ELEC_INIT evt1, evt2

  .macro  _ASM_GYM_ELEC_INIT evt1, evt2
  .short  EV_SEQ_GYM_ELEC_INIT
  .short  \evt1
  .short  \evt2
  .endm

//--------------------------------------------------------------
/**
 * ジムコマンド　電気ジムスイッチ
 * @param sw_idx   スイッチインデックス0〜3
 */
//--------------------------------------------------------------
#define _GYM_ELEC_PUSH_SW( sw_idx ) \
    _ASM_GYM_ELEC_PUSH_SW sw_idx 

  .macro  _ASM_GYM_ELEC_PUSH_SW sw_idx
  .short  EV_SEQ_GYM_ELEC_PUSH_SW
  .short  \sw_idx
  .endm

//--------------------------------------------------------------
/**
 * ジムコマンド　ノーマルジム初期化
 * @param rm_idx   ギミックのある部屋インデックス0〜1
 */
//--------------------------------------------------------------
#define _GYM_NORMAL_INIT() \
    _ASM_GYM_NORMAL_INIT 

  .macro  _ASM_GYM_NORMAL_INIT
  .short  EV_SEQ_GYM_NORMAL_INIT
  .endm

//--------------------------------------------------------------
/**
 * ジムコマンド　ノーマルジムロック解除
 */
//--------------------------------------------------------------
#define _GYM_NORMAL_UNROCK() \
    _ASM_GYM_NORMAL_UNROCK 

  .macro  _ASM_GYM_NORMAL_UNROCK
  .short  EV_SEQ_GYM_NORMAL_UNROCK
  .endm

//--------------------------------------------------------------
/**
 * ジムコマンド　ノーマルジムロック解除チェック
 */
//--------------------------------------------------------------
#define _GYM_NORMAL_CHK_UNROCK(ret_wk) \
    _ASM_GYM_NORMAL_CHK_UNROCK ret_wk

  .macro  _ASM_GYM_NORMAL_CHK_UNROCK ret_wk
  .short  EV_SEQ_GYM_NORMAL_CHK_UNROCK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * ジムコマンド　ノーマルジム本棚移動
 * @param wall_idx   本棚インデックス　現在は0固定
 */
//--------------------------------------------------------------
#define _GYM_NORMAL_MOVE_WALL(wall_idx) \
    _ASM_GYM_NORMAL_MOVE_WALL wall_idx

  .macro  _ASM_GYM_NORMAL_MOVE_WALL wall_idx
  .short  EV_SEQ_GYM_NORMAL_MOVE_WALL
  .short  \wall_idx
  .endm

//--------------------------------------------------------------
/**
 * ジムコマンド　アンチジム初期化
 */
//--------------------------------------------------------------
#define _GYM_ANTI_INIT() \
    _ASM_GYM_ANTI_INIT

  .macro  _ASM_GYM_ANTI_INIT
  .short  EV_SEQ_GYM_ANTI_INIT
  .endm

//--------------------------------------------------------------
/**
 * ジムコマンド　アンチジムスイッチオン
 * @param sw_idx   スイッチインデックス0〜8
 * @note  0,1,2： それぞれ火水草の当たりボタン、3,6:火のハズレボタン　4,7:水のハズレボタン 5,8:草のハズレボタン
 */
//--------------------------------------------------------------
#define _GYM_ANTI_SW_ON(sw_idx) \
    _ASM_GYM_ANTI_SW_ON sw_idx

  .macro  _ASM_GYM_ANTI_SW_ON sw_idx
  .short  EV_SEQ_GYM_ANTI_SW_ON
  .short  \sw_idx
  .endm

//--------------------------------------------------------------
/**
 * ジムコマンド　アンチジムスイッチオフ
 * @param sw_idx   スイッチインデックス0〜8
 * @note  0,1,2： それぞれ火水草の当たりボタン、3,6:火のハズレボタン　4,7:水のハズレボタン 5,8:草のハズレボタン
 */
//--------------------------------------------------------------
#define _GYM_ANTI_SW_OFF(sw_idx) \
    _ASM_GYM_ANTI_SW_OFF sw_idx

  .macro  _ASM_GYM_ANTI_SW_OFF sw_idx
  .short  EV_SEQ_GYM_ANTI_SW_OFF
  .short  \sw_idx
  .endm
  
//--------------------------------------------------------------
/**
 * ジムコマンド　アンチジムスイッチオン
 * @param door_idx   ドアインデックス0〜2
 */
//--------------------------------------------------------------
#define _GYM_ANTI_OPEN_DOOR(door_idx) \
    _ASM_GYM_ANTI_OPEN_DOOR door_idx

  .macro  _ASM_GYM_ANTI_OPEN_DOOR door_idx
  .short  EV_SEQ_GYM_ANTI_OPEN_DOOR
  .short  \door_idx
  .endm

//======================================================================
//
//  モデルアニメ関連
//
//======================================================================
//--------------------------------------------------------------
/**
 * @def _POKECEN_RECOVER_ANIME
 * @brief ポケセン回復アニメ
 * @param poke_count    手持ちのポケモンの数
 */
//--------------------------------------------------------------
#define _POKECEN_RECOVER_ANIME( poke_count )  \
  _ASM_POKECEN_RECOVER_ANIME poke_count

  .macro  _ASM_POKECEN_RECOVER_ANIME poke_count
  .short  EV_SEQ_POKECEN_RECOVER_ANIME
  .short  \poke_count
  .endm

//--------------------------------------------------------------
/**
 * @def _DOOR_ANIME_CREATE
 * @brief ドアアニメコントローラーの生成
 * @param ret_wk  anm_idをうけとるためのワーク
 * @param gx      アニメ生成する配置モデルのX位置（グリッド単位）
 * @param gz      アニメ生成する配置モデルのZ位置（グリッド単位）
 *
 * ret_wkで受け取ったアニメ指定IDを使ってドアアニメ各コマンド
 * （_DOOR_ANIME_DELETE/_DOOR_ANIME_SET/_DOOR_ANIME_WAIT）を
 * 呼び出す。
 */
//--------------------------------------------------------------
#define _DOOR_ANIME_CREATE( ret_wk, gx, gz ) \
    _ASM_DOOR_ANIME_CREATE ret_wk, gx, gz

    .macro _ASM_DOOR_ANIME_CREATE ret_wk, gx, gz
    .short  EV_SEQ_DOOR_ANIME_CREATE
    .short  \ret_wk
    .short  \gx
    .short  \gz
    .endm

//--------------------------------------------------------------
/**
 * @def _DOOR_ANIME_DELETE
 * @brief ドアアニメコントローラーの削除
 * @param anm_id  配置モデルを指定するためのID
 */
//--------------------------------------------------------------
#define _DOOR_ANIME_DELETE( anm_id ) \
     _ASM_DOOR_ANIME_DELETE anm_id

     .macro _ASM_DOOR_ANIME_DELETE anm_id
     .short EV_SEQ_DOOR_ANIME_DELETE
     .short \anm_id
     .endm

//--------------------------------------------------------------
/**
 * @def _DOOR_ANIME_SET
 * @brief ドアアニメのセット
 * @param anm_id    配置モデルを指定するためのID
 * @param anm_type  アニメーションの種類指定
 */
//--------------------------------------------------------------
#define _DOOR_ANIME_SET( anm_id, anm_type ) \
    _ASM_DOOR_ANIME_SET anm_id, anm_type

    .macro _ASM_DOOR_ANIME_SET anm_id, anm_type
    .short  EV_SEQ_DOOR_ANIME_SET
    .short  \anm_id
    .short  \anm_type
    .endm

//--------------------------------------------------------------
/**
 * @def _DOOR_ANIME_WAIT
 * @brief ドアアニメの終了待ち
 * @param anm_id  配置モデルを指定するためのID
 */
//--------------------------------------------------------------
#define _DOOR_ANIME_WAIT( anm_id ) \
    _ASM_DOOR_ANIME_WAIT anm_id

    .macro _ASM_DOOR_ANIME_WAIT anm_id
    .short  EV_SEQ_DOOR_ANIME_WAIT
    .short  \anm_id
    .endm


//======================================================================
//
//
// 育て屋関連
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * 育て屋のポケモンにタマゴが生まれたかどうかのチェック
 * 
 * @param ret_wk 結果の格納先
 * @return       タマゴがある場合 TRUE
 */
//--------------------------------------------------------------
#define _CHECK_SODATEYA_HAVE_EGG( ret_wk ) \
    _ASM_CHECK_SODATEYA_HAVE_EGG ret_wk

  .macro _ASM_CHECK_SODATEYA_HAVE_EGG ret_wk
  .short EV_SEQ_CHECK_SODATEYA_HAVE_EGG
  .short \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * 育て屋のタマゴを受け取る
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_EGG() \
    _ASM_GET_SODATEYA_EGG

  .macro _ASM_GET_SODATEYA_EGG
  .short EV_SEQ_GET_SODATEYA_EGG
  .endm

//--------------------------------------------------------------
/**
 * 育て屋のタマゴを削除する
 */
//--------------------------------------------------------------
#define _DELETE_SODATEYA_EGG() \
    _ASM_DELETE_SODATEYA_EGG

  .macro _ASM_DELETE_SODATEYA_EGG
  .short EV_SEQ_DELETE_SODATEYA_EGG
  .endm

//--------------------------------------------------------------
/**
 * 育て屋に預けているポケモンの数を取得する
 *
 * @param ret_wk 結果の格納先
 * @return       育て屋に預けているポケモンの数
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKEMON_NUM( ret_wk ) \
    _ASM_GET_SODATEYA_POKEMON_NUM ret_wk

  .macro _ASM_GET_SODATEYA_POKEMON_NUM ret_wk
  .short EV_SEQ_GET_SODATEYA_POKEMON_NUM
  .short \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * 育て屋に預けている2体のポケモンの相性をチェックする
 *
 * @param ret_wk 結果の格納先
 * @return 
 */
//--------------------------------------------------------------
#define _SODATEYA_LOVE_CHECK( ret_wk ) \
    _ASM_SODATEYA_LOVE_CHECK ret_wk

  .macro _ASM_SODATEYA_LOVE_CHECK ret_wk
  .short EV_SEQ_SODATEYA_LOVE_CHECK
  .short \ret_wk
  .endm
//--------------------------------------------------------------
/**
 * ポケモンを育て屋に預ける
 *
 * @param pos 預けるポケモンの手持ち番号
 */
//--------------------------------------------------------------
#define _POKE_TO_SODATEYA( pos ) \
    _ASM_POKE_TO_SODATEYA pos

  .macro _ASM_POKE_TO_SODATEYA pos
  .short EV_SEQ_POKE_TO_SODATEYA
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * ポケモンを育て屋から受け取る
 *
 * @param pos 受け取るポケモンの育て屋内番号(0 or 1)
 */
//--------------------------------------------------------------
#define _POKE_FROM_SODATEYA( pos ) \
    _ASM_POKE_FROM_SODATEYA pos

  .macro _ASM_POKE_FROM_SODATEYA pos
  .short EV_SEQ_POKE_FROM_SODATEYA
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * 育て屋に預けているポケモンのモンスターナンバーを取得する
 *
 * @param ret_wk 取得したモンスターナンバーの格納先
 * @param pos    取得対象ポケモンの育て屋内インデックス
 * @retval u16   指定ポケモンのモンスターナンバー
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_MONSNO( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_MONSNO ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_MONSNO ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_MONSNO
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * 育て屋に預けているポケモンの形状ナンバーを取得する
 *
 * @param ret_wk 取得した形状ナンバーの格納先
 * @param pos    取得対象ポケモンの育て屋内インデックス
 * @retval u16   指定ポケモンの形状ナンバー
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_FORMNO( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_FORMNO ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_FORMNO ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_FORMNO
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * 育て屋に預けているポケモンの現在のレベルを取得する
 *
 * @param ret_wk 取得したレベルの格納先
 * @param pos    取得対象ポケモンの育て屋内インデックス
 * @retval u16   指定ポケモンのレベル
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_LV( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_LV ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_LV ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_LV
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * 育て屋に預けているポケモンについて, 成長したレベルを取得する
 *
 * @param ret_wk 取得した値の格納先
 * @param pos    取得対象ポケモンの育て屋内インデックス
 * @retval u16   指定ポケモンが成長したレベル
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_GROWUP_LV( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_GROWUP_LV ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_GROWUP_LV ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_GROWUP_LV
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * 育て屋に預けているポケモンについて, 引き取り料金を取得する
 *
 * @param ret_wk 取得した値の格納先
 * @param pos    取得対象ポケモンの育て屋内インデックス
 * @retval u16   指定ポケモンの引き取り料金
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_CHARGE( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_CHARGE ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_CHARGE ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_CHARGE
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * 手持ちポケモン選択画面を呼び出す
 *
 * @param ret_wk 選択結果の格納先
 * @retval u16   選択結果
 */
//--------------------------------------------------------------
#define _SODATEYA_SELECT_POKEMON( ret_wk ) \
    _ASM_SODATEYA_SELECT_POKEMON ret_wk

  .macro _ASM_SODATEYA_SELECT_POKEMON ret_wk
  .short EV_SEQ_SODATEYA_SELECT_POKEMON
  .short \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * 育て屋に預けているポケモンについて, 性別を取得する
 *
 * @param ret_wk 取得した値の格納先
 * @param pos    取得対象ポケモンの育て屋内インデックス
 * @retval u16   指定ポケモンの性別
 *               オス = 0
 *               メス = 1
 *               性別なし = 2
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_SEX( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_SEX ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_SEX ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_SEX
  .short \ret_wk
  .short \pos
  .endm

//======================================================================
//  サブプロセスコール関連
//======================================================================
//--------------------------------------------------------------
/**
 * フィールドプロセスを生成する
 */
//--------------------------------------------------------------
#define _FIELD_OPEN() _ASM_FIELD_OPEN

  .macro _ASM_FIELD_OPEN
  .short EV_SEQ_FIELD_OPEN
  .endm

//--------------------------------------------------------------
/**
 * フィールドプロセスを終了する
 */
//--------------------------------------------------------------
#define _FIELD_CLOSE() _ASM_FIELD_CLOSE

  .macro _ASM_FIELD_CLOSE
  .short EV_SEQ_FIELD_CLOSE
  .endm

//--------------------------------------------------------------
/**
 * サブプロセスワークを解放する
 *
 * ＊使用は玉田さんの許可制です。
 * 　プロセスコールは原則として、呼び出し〜戻り値取得までをOneコマンドで実現する設計にすること！
 */
//--------------------------------------------------------------
#define _FREE_SUBPROC_WORK() _ASM_SUBPROC_WORK

  .macro _ASM_SUBPROC_WORK
  .short EV_SEQ_FREE_SUB_PROC_WORK
  .endm

//--------------------------------------------------------------
/**
 *  バッグプロセスを呼び出す
 *  @param  mode  呼び出しモード script_def.h SCR_BAGMODE_SELL他
 *  @param  ret_mode  バッグの終了モード　script_def.h SCR_PROC_RETMODE_EXIT 他
 *  @param  ret_item  選択アイテムNo(0なら選択アイテム無し)
 *
 *  サブプロセスワークを解放しないので、使用後に _FREE_SUBPROC_WORK()を呼び出すこと
 */
//--------------------------------------------------------------
#define _CALL_BAG_PROC( mode, ret_mode, ret_item ) \
    _ASM_CALL_BAG_PROC mode, ret_mode, ret_item

  .macro  _ASM_CALL_BAG_PROC mode, ret_mode, ret_item
  .short  EV_SEQ_CALL_BAG_PROC
  .short  \mode
  .short  \ret_mode
  .short  \ret_item
  .endm

//--------------------------------------------------------------
/**
 *  ボックスプロセスを呼び出す
 */
//--------------------------------------------------------------
#define _CALL_BOX_PROC() \
    _ASM_CALL_BOX_PROC 

  .macro  _ASM_CALL_BOX_PROC 
  .short  EV_SEQ_CALL_BOX_PROC
  .endm

//======================================================================
//  ショップ関連
//======================================================================
//--------------------------------------------------------------
/**
 * @def _SHOP_CALL
 * @brief 簡易イベントコマンド：ショップイベント呼び出し
 * @param shop_id
 */
//--------------------------------------------------------------
#define _SHOP_CALL( ) _ASM_SHOP_CALL  SCR_SHOPID_NULL 

//--------------------------------------------------------------
/**
 * @def _SHOP_CALL
 * @brief 簡易イベントコマンド：固定ショップイベント呼び出し
 * @param shop_id
 */
//--------------------------------------------------------------
#define _FIX_SHOP_CALL( shop_id ) _ASM_SHOP_CALL shop_id

//--------------------------------------------------------------
/**
 * @def _CALL_SHOP_PROC_BUY
 * @brief ショップイベント呼び出し
 * @param shop_id SCR_SHOPID_NULL を指定すると変動ショップ、それ以外なら固定ショップ
 *
 * shop_scr.ev専用　ゾーンスクリプトから直接呼び出さないこと！
 */
//--------------------------------------------------------------
#define _CALL_SHOP_PROC_BUY( shop_id, ret_mode )  _ASM_CALL_SHOP_PROC_BUY shop_id,ret_mode

  .macro  _ASM_CALL_SHOP_PROC_BUY shop_id, ret_mode
  .short  EV_SEQ_CALL_SHOP_PROC_BUY
  .short  \shop_id
  .short  \ret_mode
  .endm

//======================================================================
// WiFiクラブ関連
//======================================================================
//--------------------------------------------------------------
/**
 * @brief    GSIDが正しいかをチェックする
 */
//--------------------------------------------------------------
#define _WIFI_CHECK_MY_GSID( ret_work ) _ASM_WIFI_CHECK_MY_GSID ret_work

  .macro  _ASM_WIFI_CHECK_MY_GSID ret_work
  .short  EV_SEQ_WIFI_CHECK_MY_GSID
  .short  \ret_work
  .endm

//--------------------------------------------------------------------
/**
 * @brief   友だち手帳に登録されている人数を返す 
 */
//--------------------------------------------------------------------
#define _WIFI_GET_FRIEND_NUM( ret_work ) _ASM_WIFI_GET_FRIEND_NUM ret_work

  .macro  _ASM_WIFI_GET_FRIEND_NUM ret_work
  .short  EV_SEQ_WIFI_GET_FRIEND_NUM
  .short  \ret_work
  .endm

//--------------------------------------------------------------------
/**
 * @brief   Wi-Fiクラブイベントを呼び出す
 */
//--------------------------------------------------------------------
#define _WIFI_CLUB_EVENT_CALL( ) _ASM_WIFI_CLUB_EVENT_CALL

  .macro  _ASM_WIFI_CLUB_EVENT_CALL
  .short  EV_SEQ_WIFI_CLUB_EVENT_CALL
  .endm

//======================================================================
//  秘伝技関連
//======================================================================
//--------------------------------------------------------------
/**
 * なみのり開始
 * @param none
 */
//--------------------------------------------------------------
#define _NAMINORI() _ASM_NAMINORI
  
  .macro _ASM_NAMINORI
  .short EV_SEQ_NAMINORI
  .endm

//--------------------------------------------------------------
/**
 * 滝登り開始
 * @param no 技を使う手持ちポケモン位置番号
 */
//--------------------------------------------------------------
#define _TAKINOBORI( no ) _ASM_TAKINOBORI no
  
  .macro _ASM_TAKINOBORI no
  .short EV_SEQ_TAKINOBORI
  .short \no
  .endm

//======================================================================
//
//  簡易コマンド
//  ※スクリプトコマンドを組み合わせたもの
//
//======================================================================
  .include  "easy_event_def.h"

//--------------------------------------------------------------
/**
 * 簡易メッセージ表示 BG用
 * @param msg_id 表示するメッセージID
 */
//--------------------------------------------------------------
#define _EASY_MSG( msg_id )   _ASM_EASY_MSG msg_id


//--------------------------------------------------------------
/**
 * 簡易吹き出しメッセージ表示 話し掛けOBJ用
 * @param msg_id 表示するメッセージID
 */
//--------------------------------------------------------------
#define _EASY_BALLOONWIN_TALKOBJ_MSG( msg_id )  _ASM_EASY_BALLOONWIN_TALKOBJ_MSG msg_id


//--------------------------------------------------------------
/**
 * 簡易イベント入手イベント
 *
 * @param itemno        取得するアイテムナンバー
 * @param num           取得するアイテムの数
 * @param flag          アイテム取得したかどうかの判定用フラグ
 * @param before_msg    入手前のメッセージ
 * @param finish_msg    入手後のメッセージ
 * @param after_msg     入手後、再び話かけたときのメッセージ
 *
 */
//--------------------------------------------------------------
#define _EASY_TALK_ITEM_EVENT( itemno, num, flag, before_msg, finish_msg, after_msg) \
    _ASM_EASY_TALK_ITEM_EVENT itemno, num, flag, before_msg, finish_msg, after_msg

//--------------------------------------------------------------
/**
 * イベント入手イベント「▼」待ちあり
 */
//--------------------------------------------------------------
#define _ITEM_EVENT_KEYWAIT(itemno, num)   \
    _ASM_ITEM_EVENT_KEYWAIT itemno, num

//--------------------------------------------------------------
/**
 * イベント入手イベント「▼」待ちなし
 */
//--------------------------------------------------------------
#define _ITEM_EVENT_NOWAIT(itemno, num)   \
    _ASM_ITEM_EVENT_KEYWAIT itemno, num

//--------------------------------------------------------------
/**
 * 簡易鳴き声メッセージコマンド
 *
 * @param monsno
 * @param msg_id
 */
//--------------------------------------------------------------
#define _EASY_TALK_POKEVOICE( monsno, msg_id ) \
    _ASM_EASY_TALK_POKEVOICE monsno, msg_id

//--------------------------------------------------------------
/**
 * @def _REPORT_EVENT_CALL
 * @brief 簡易イベントコマンド：セーブ呼び出し
 * @param ret_wk
 */
//--------------------------------------------------------------
#define _REPORT_EVENT_CALL( ret_wk )  \
    _ASM_REPORT_EVENT_CALL  ret_wk

//--------------------------------------------------------------
/**
 * @def _FIELD_COMM_EXIT_EVENT_CALL
 * @brief 簡易イベントコマンド：フィールド通信切断イベント呼び出し
 * @param ret_wk  切断処理の戻り値を取得
 *                SCR_FIELD_COMM_EXIT_OK:切断終了 SCR_FIELD_COMM_EXIT_NG:なんらかのエラーが発生
 */
//--------------------------------------------------------------
#define _FIELD_COMM_EXIT_EVENT_CALL( ret_wk )  \
    _ASM_FIELD_COMM_EXIT_EVENT_CALL  ret_wk

//--------------------------------------------------------------
/**
 * カメラ操作開始
 */
//--------------------------------------------------------------
#define _CAMERA_START() _ASM_CAMERA_START
  
  .macro _ASM_CAMERA_START
  .short EV_SEQ_CAMERA_START
  .endm

//--------------------------------------------------------------
/**
 * カメラ操作終了
 */
//--------------------------------------------------------------
#define _CAMERA_END() _ASM_CAMERA_END
  
  .macro _ASM_CAMERA_END
  .short EV_SEQ_CAMERA_END
  .endm  

//--------------------------------------------------------------
/**
 * カメラパージ
 */
//--------------------------------------------------------------
#define _CAMERA_PURGE() _ASM_CAMERA_PURGE
  
  .macro _ASM_CAMERA_PURGE
  .short EV_SEQ_CAMERA_PURGE
  .endm

//--------------------------------------------------------------
/**
 * カメラバインド
 */
//--------------------------------------------------------------
#define _CAMERA_BIND() _ASM_CAMERA_BIND
  
  .macro _ASM_CAMERA_BIND
  .short EV_SEQ_CAMERA_BIND
  .endm

//--------------------------------------------------------------
/**
 * カメラ移動
 * @param pitch     ピッチ
 * @param yaw       ヨー
 * @param dist      距離
 * @param x         注視点Ｘ
 * @param y         注視点Ｙ
 * @param z         注視点Ｚ
 * @param frame     移動フレーム
 */
//--------------------------------------------------------------
#define _CAMERA_MOVE(pitch, yaw, dist, x, y, z, frame) \
    _ASM_CAMERA_MOVE pitch, yaw, dist, x, y, z, frame
  
  .macro _ASM_CAMERA_MOVE pitch, yaw, dist, x, y, z, frame
  .short EV_SEQ_CAMERA_MOVE
  .short \pitch
  .short \yaw
  .short \dist
  .long \x
  .long \y
  .long \z
  .short \frame
  .endm

//--------------------------------------------------------------
/**
 * カメラ復帰移動
 * @param frame   移動フレーム
 */
//--------------------------------------------------------------
#define _CAMERA_RECV_MOVE(frame) _ASM_CAMERA_RECV_MOVE frame
  
  .macro _ASM_CAMERA_RECV_MOVE frame
  .short EV_SEQ_CAMERA_RECV_MOVE
  .short \frame
  .endm

//--------------------------------------------------------------
/**
 * カメラ移動待ち
 */
//--------------------------------------------------------------
#define _CAMERA_WAIT_MOVE() _ASM_CAMERA_WAIT_MOVE
  
  .macro _ASM_CAMERA_WAIT_MOVE
  .short EV_SEQ_CAMERA_WAIT_MOVE
  .endm


