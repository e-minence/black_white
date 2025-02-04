//===========================================================================
/**
 * @file	zonetableformat.h
 * @brief	ゾーン毎データテーブルの定義
 * @author	tamada GAME FREAK inc.
 * @date	2008.11.12
 *
 * プログラム側と常に同じ構造体を維持する必要があるので、注意！
 */
//===========================================================================
//------------------------------------------------------------------
/**
 * @brief	ゾーン毎に持つデータの構成
 */
//------------------------------------------------------------------
typedef struct{
	u8 maprsc_id;						///<マップ種別指定（新規）
	u8 movemodel_id;					///<動作モデルの指定
	u16 area_id;							///<AREA_IDの指定
	u16 matrix_id;						///<マップマトリックスの指定
	u16 script_id;						///<スクリプトファイルのアーカイブID
	u16 sp_script_id;					///<特殊スクリプトファイルのアーカイブID
	u16 msg_id;							///<メッセージファイルのアーカイブID
	u16 bgm_spring_id;					///<BGM指定（春）のID
	u16 bgm_summer_id;					///<BGM指定（夏）のID
	u16 bgm_autumn_id;					///<BGM指定（秋）のID
	u16 bgm_winter_id;					///<BGM指定（冬）のID
	u16 enc_data_id;					///<エンカウント指定
	u16 event_data_id;					///<イベントデータファイルのアーカイブID
  u16 zone_group;           ///<タウンマップ上での表示位置用グループ指定

	u16 placename_id:10;					///<地名メッセージのＩＤ
	u16 placename_flag:6;				///<地名メッセージ用のウィンドウ

	u16 weather_id:6;						///<天候指定のID
  u16 projection:3;            ///<射影オフセット値の指定
	u16 camera_id:7;						  ///<カメラ指定のID

	u16 map_change_type:5;			///<マップ切り替えタイプの指定(src/field/map_change_type.h参照）
	u16 battle_bg_type:5;				///<戦闘背景の指定
	u16 bicycle_flag:1;					///<自転車に乗れるかどうか
	u16 dash_flag:1;						///<ダッシュできるかどうか
	u16 escape_flag:1;					///<あなぬけできるかどうか
	u16 fly_flag:1;             ///<そらをとぶができるかどうか
  u16 sp_bgm_flag:1;          ///<自転車でBGMがかわるかどうか
  u16 palace_ok_flag:1;       ///<パレスへゴーが使えるか？

  u16 camera_area;           ///<カメラエリア指定のID
  u16 pad;  ///<予備

	s32 sx, sy, sz;			///<デフォルトスタート位置（デバッグ用）
}ZONEDATA;

