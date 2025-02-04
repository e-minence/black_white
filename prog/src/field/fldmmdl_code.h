//======================================================================
/**
 * @file  fieldobj_code.h
 * @brief  フィールド動作モデルで使用するシンボル,コード宣言。
 * アセンブラソース読み込みアリ enum禁止
 * @author  kagaya
 * @date  05.07.20
 */
//======================================================================
#ifndef MMDL_CODE_H_FILE
#define MMDL_CODE_H_FILE

//======================================================================
//  方向
//======================================================================
#if 0 //field_dir.hへ
#define DIR_UP      (0)          ///<上向き
#define DIR_DOWN    (1)          ///<下向き
#define DIR_LEFT    (2)          ///<左向き
#define DIR_RIGHT    (3)          ///<右向き
#define DIR_LEFTUP    (4)          ///<左上向き
#define DIR_RIGHTUP    (5)          ///<右上向き
#define DIR_LEFTDOWN  (6)          ///<左下向き
#define DIR_RIGHTDOWN  (7)          ///<右下向き
#define DIR_MAX      (8)          ///<向き最大
#define DIR_NOT      (9)          ///<向きが無い
#define DIR_MAX4    (DIR_RIGHT+1)    ///<四方向最大
#endif

//======================================================================
//  特殊ID
//======================================================================
//--------------------------------------------------------------
//  フィールド動作モデル 特殊ID
//--------------------------------------------------------------
#define MMDL_ID_PLAYER   (0xff)  ///<自機専用ID
#define MMDL_ID_PAIR     (0xfe)  ///<連れ歩き専用ID
#define MMDL_ID_GIMMICK  (0xfd)  ///<ジム等の仕掛け専用ID
#define MMDL_ID_COMMACTOR (0xfc) ///<通信アクター専用ID
#define MMDL_ID_EVENTOBJ_00 (0xfb) ///<スクリプトイベント内で追加されるOBJID

//スクリプト配置は原則禁止
#define MMDL_ID_EVENTOBJ_01 (0xfa) ///<スクリプトイベント内で追加されるOBJID
#define MMDL_ID_EVENTOBJ_02 (0xf0) ///<スクリプトイベント内で追加されるOBJID

//T01-R01で配置される特殊OBJ用ID
#define MMDL_ID_T01_R01_RIVAL (MMDL_ID_EVENTOBJ_01)
#define MMDL_ID_T01_R01_SUPPORT (MMDL_ID_EVENTOBJ_02)

//--------------------------------------------------------------
//  エイリアスID 元:PL script_def.h
//--------------------------------------------------------------
#define SP_SCRID_ALIES (0xffff)

//======================================================================
//  フィールド動作モデル 動作コード
//======================================================================
//--------------------------------------------------------------
//  フィールド動作モデル 動作コード
//--------------------------------------------------------------
#define MV_DMY      0x00    ///<動作無し
#define MV_PLAYER    0x01    ///<自機専用
#define MV_DIR_RND    0x02    ///<ランダムに方向切り替え
#define MV_RND      0x03    ///<ランダムに移動
#define MV_RND_V    0x04    ///<ランダムに縦移動
#define MV_RND_H    0x05    ///<ランダムに横移動
#define MV_RND_UL    0x06    ///<ランダムに上左方向切り替え
#define MV_RND_UR    0x07    ///<ランダムに上右方向切り替え
#define MV_RND_DL    0x08    ///<ランダムに下左方向切り替え
#define MV_RND_DR    0x09    ///<ランダムに下右方向切り替え
#define MV_RND_UDL    0x0a    ///<ランダムに上下左方向切り替え
#define MV_RND_UDR    0x0b    ///<ランダムに上下右方向切り替え
#define MV_RND_ULR    0x0c    ///<ランダムに上左右方向切り替え
#define MV_RND_DLR    0x0d    ///<ランダムに下左右方向切り替え
#define MV_UP      0x0e    ///<上固定
#define MV_DOWN      0x0f    ///<下固定
#define MV_LEFT      0x10    ///<左固定
#define MV_RIGHT    0x11    ///<右固定
#define MV_SPIN_L    0x12    ///<左回転
#define MV_SPIN_R    0x13    ///<右回転
#define MV_RT2      0x14    ///<ルート2
#define MV_RTURLD       0x15        ///<ルート3 上右左下
#define MV_RTRLDU       0x16        ///<ルート3 右左下上
#define MV_RTDURL       0x17        ///<ルート3 下上右左
#define MV_RTLDUR       0x18        ///<ルート3 左下上右
#define MV_RTULRD       0x19        ///<ルート3 上左右下
#define MV_RTLRDU       0x1a        ///<ルート3 左右下上
#define MV_RTDULR       0x1b        ///<ルート3 下上左右
#define MV_RTRDUL       0x1c        ///<ルート3 右下上左
#define MV_RTLUDR       0x1d        ///<ルート3 左上下右
#define MV_RTUDRL       0x1e        ///<ルート3 上下右左
#define MV_RTRLUD       0x1f        ///<ルート3 右左上下
#define MV_RTDRLU       0x20        ///<ルート3 下右左上
#define MV_RTRUDL       0x21        ///<ルート3 右上下左
#define MV_RTUDLR       0x22        ///<ルート3 上下左右
#define MV_RTLRUD       0x23        ///<ルート3 左右上下
#define MV_RTDLRU       0x24        ///<ルート3 下左右上
#define MV_RTUL         0x25        ///<ルート4 上左
#define MV_RTDR         0x26        ///<ルート4 下右
#define MV_RTLD         0x27        ///<ルート4 左下
#define MV_RTRU         0x28        ///<ルート4 右上
#define MV_RTUR         0x29        ///<ルート4 上右
#define MV_RTDL         0x2a        ///<ルート4 下左
#define MV_RTLU         0x2b        ///<ルート4 左上
#define MV_RTRD         0x2c        ///<ルート4 右下
#define MV_RND_UD    0x2d    ///<ランダムに上下切り替え
#define MV_RND_LR    0x2e    ///<ランダムに左右切り替え
#define MV_SEED      0x2f    ///<木の実
#define MV_PAIR      0x30    ///<連れ歩き
#define MV_REWAR    0x31    ///<再戦
#define MV_TR_PAIR    0x32    ///<トレーナー連れ歩き
#define MV_HIDE_SNOW  0x33    ///<隠れ蓑　雪
#define MV_HIDE_SAND  0x34    ///<隠れ蓑　砂
#define MV_HIDE_GRND  0x35    ///<隠れ蓑　土
#define MV_HIDE_KUSA  0x36    ///<隠れ蓑　草
#define MV_COPYU    0x37    ///<ものまね上
#define MV_COPYD    0x38    ///<ものまね下
#define MV_COPYL    0x39    ///<ものまね左
#define MV_COPYR    0x3a    ///<ものまね右
#define MV_COPYLGRASSU  0x3b    ///<ものまね上
#define MV_COPYLGRASSD  0x3c    ///<ものまね下
#define MV_COPYLGRASSL  0x3d    ///<ものまね左
#define MV_COPYLGRASSR  0x3e    ///<ものまね右
#define MV_ALONGW_L    0x3f    ///<壁沿い左(m
#define MV_ALONGW_R    0x40    ///<壁沿い右(m
#define MV_ALONGW_LRL  0x41    ///<壁沿い両手左(m
#define MV_ALONGW_LRR  0x42    ///<壁沿い両手右(m
#define MV_RND_H_LIM  0x43 ///<ランダムに横移動 移動チェック移動制限のみ
#define MV_COMM_ACTOR 0x44 ///<通信アクター動作
#define MV_RAIL_DMY 0x45 ///<レールマップ　動作なし
#define MV_RAIL_DIR_RND_ALL    0x46    ///<レールマップ　ランダムに方向切り替え
#define MV_RAIL_DIR_RND_UL    0x47    ///<レールマップ　ランダムに方向切り替え
#define MV_RAIL_DIR_RND_UR    0x48    ///<レールマップ　ランダムに方向切り替え
#define MV_RAIL_DIR_RND_DL    0x49    ///<レールマップ　ランダムに方向切り替え
#define MV_RAIL_DIR_RND_DR    0x4a    ///<レールマップ　ランダムに方向切り替え
#define MV_RAIL_DIR_RND_UDL    0x4b    ///<レールマップ　ランダムに方向切り替え
#define MV_RAIL_DIR_RND_UDR    0x4c    ///<レールマップ　ランダムに方向切り替え
#define MV_RAIL_DIR_RND_ULR    0x4d    ///<レールマップ　ランダムに方向切り替え
#define MV_RAIL_DIR_RND_DLR    0x4e    ///<レールマップ　ランダムに方向切り替え
#define MV_RAIL_DIR_RND_UD    0x4f    ///<レールマップ　ランダムに方向切り替え
#define MV_RAIL_DIR_RND_LR    0x50    ///<レールマップ　ランダムに方向切り替え
#define MV_RAIL_RND      0x51    ///<ランダムに移動    (移動制限あり)
#define MV_RAIL_RND_V    0x52    ///<ランダムに縦移動  (移動制限あり)
#define MV_RAIL_RND_H    0x53    ///<ランダムに横移動  (移動制限あり) 
#define MV_RAIL_RT2    0x54    ///<ルート2分岐
#define MV_CODE_MAX    0x55    ///<コード最大（プログラム判定専用
#define MV_CODE_NOT    0xff    ///<非動作コード(プログラム判定専用

/*　未対応
#define MV_SLEEP  ///<眠り　ダッシュで起きる
#define MV_DIVE    ///<素潜り

#define MV_HIDE_SKY      ///<隠れ蓑　空
#define MV_HIDE_SWIM    ///<隠れ蓑　海
#define MV_HIDE_SNOWMAN    ///<隠れ蓑　雪だるま
*/

//======================================================================
//  フィールド動作モデル イベントタイプ
//  タイプを増やした際は以下の変更が必要。
//  ・fldmmdl_move_1内定義のデータテーブルの更新
//  ・トレーナーチェック関数MMDL_TOOL_GetTrainerEventType()更新
//======================================================================
//--------------------------------------------------------------
///  動作モデル イベントタイプ　特に無し
//--------------------------------------------------------------
#define EV_TYPE_NORMAL 0x00

//--------------------------------------------------------------
/// 動作モデル イベントタイプ　トレーナー
/// Param0に視線距離
//--------------------------------------------------------------
#define EV_TYPE_TR 0x01
#define EV_TYPE_TRAINER (0x01) //old

//--------------------------------------------------------------
///  動作モデル　イベントタイプ　トレーナー全方位検知
///  Param0に視線距離
//--------------------------------------------------------------
#define EV_TYPE_TR_EYEALL 0x02
#define EV_TYPE_TRAINER_EYEALL (0x02) //old

//--------------------------------------------------------------
///  動作モデル　イベントタイプ　アイテム
//--------------------------------------------------------------
#define EV_TYPE_ITEM 0x03

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　トレーナーきょろきょろ
/// Param0に視線距離。
/// Param1にきょろきょろを開始するグリッド数。
//--------------------------------------------------------------
#define EV_TYPE_TR_KYORO 0x04 ///<トレーナータイプ　きょろきょろ
#define EV_TYPE_TRAINER_KYORO (0x04)

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　トレーナー停止回転
/// Param0に視線距離。
/// Param1に回転を開始する開始するグリッド数。
//--------------------------------------------------------------
#define EV_TYPE_TR_SPINS_L 0x05 ///<トレーナータイプ停止回転左周り
#define EV_TYPE_TR_SPINS_R 0x06 ///<トレーナータイプ停止回転右回り
#define EV_TYPE_TR_SPINM_L 0x07 ///<トレーナータイプ移動回転左周り
#define EV_TYPE_TR_SPINM_R 0x08 ///<トレーナータイプ移動回転右回り

//old
#define EV_TYPE_TRAINER_SPIN_STOP_L (0x05)
#define EV_TYPE_TRAINER_SPIN_STOP_R (0x06)
#define EV_TYPE_TRAINER_SPIN_MOVE_L (0x07)
#define EV_TYPE_TRAINER_SPIN_MOVE_R (0x08)

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　メッセージID指定
//--------------------------------------------------------------
#define EV_TYPE_MSG 0x09

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　視線距離内に自機が来たら逃げる
/// Param0に視線距離。
/// ４点移動系のみ。視線ヒットチェック対象ではない。
//--------------------------------------------------------------
#define EV_TYPE_TR_ESC 0x0a
#define EV_TYPE_TRAINER_ESCAPE (0x0a) //old

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　ダッシュに反応して方向変更
/// Param0に視線距離。
/// ２方向切り替え、３方向切り替え、SPINタイプ限定
//--------------------------------------------------------------
#define EV_TYPE_TR_D_ALT 0x0b
#define EV_TYPE_TRAINER_DASH_ALTER (0x0b) //old

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　ダッシュに反応して動く
/// Param0に視線距離。
/// Param1に移動するグリッド数。/// 動作モデル　イベントタイプ
//--------------------------------------------------------------
#define EV_TYPE_TR_D_REA 0x0c
#define EV_TYPE_TRAINER_DASH_REACT (0x0c) //old

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　ダッシュに反応して移動速度アップ
/// Param0に視線距離。
//--------------------------------------------------------------
#define EV_TYPE_TR_D_ACC 0x0d
#define EV_TYPE_TRAINER_DASH_ACCEL (0x0d) //old

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　トレーナーきょろきょろ　ダッシュで速度Up版
/// Param0に視線距離。
/// Param1にきょろきょろを開始するグリッド数。
//--------------------------------------------------------------
#define EV_TYPE_TR_D_KYORO (0x0e)
#define EV_TYPE_TRAINER_DASH_ACCEL_KYORO (0x0e) //old

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　トレーナー停止回転 ダッシュで速度Up版
/// Param0に視線距離。
/// Param1に回転を開始する開始するグリッド数。
//--------------------------------------------------------------
#define EV_TYPE_TR_D_SPINS_L 0x0f ///<トレーナータイプ停止回転左周り
#define EV_TYPE_TR_D_SPINS_R 0x10 ///<トレーナータイプ停止回転右回り
#define EV_TYPE_TR_D_SPINM_L 0x11 ///<トレーナータイプ移動回転左周り
#define EV_TYPE_TR_D_SPINM_R 0x12 ///<トレーナータイプ移動回転右回り

//old
#define EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_L (0x0f)
#define EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_R (0x10)
#define EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_L (0x11)
#define EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_R (0x12)

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　ダッシュに反応して移動速度アップ
/// EV_TYPE_TR_D_ACCの非トレーナー版
//--------------------------------------------------------------
#define EV_TYPE_D_ACC (0x13)

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　停止回転
/// Param0に視線距離。
/// Param1に回転を開始する開始するグリッド数。
/// EV_TYPE_TR_SPINS_Lの非トレーナー版
//--------------------------------------------------------------
#define EV_TYPE_SPINS_L (0x14)

//--------------------------------------------------------------
/// 動作モデル　イベントタイプ　最大数
//--------------------------------------------------------------
#define EV_TYPE_MAX (0x15)

//======================================================================
//  フィールド動作モデル 描画ステータス
//======================================================================
//--------------------------------------------------------------
///  フィールド動作モデル 描画ステータス　基本
//--------------------------------------------------------------
#define DRAW_STA_STOP     0x00    ///<停止
#define DRAW_STA_WALK_32F 0x01    ///<移動 32フレーム
#define DRAW_STA_WALK_16F 0x02    ///<移動 16フレーム
#define DRAW_STA_WALK_8F  0x03    ///<移動 8フレーム
#define DRAW_STA_WALK_4F  0x04    ///<移動 4フレーム
#define DRAW_STA_WALK_2F  0x05    ///<移動 2フレーム
#define DRAW_STA_WALK_6F  0x06    ///<移動 6フレーム
#define DRAW_STA_WALK_3F  0x07    ///<移動 3フレーム
#define DRAW_STA_WALK_7F  0x08    ///<移動 7フレーム
#define DRAW_STA_WALK_12F 0x09    ///<移動 10フレーム
#define DRAW_STA_MAX      0x0a    ///<描画ステータス最大

///描画ステータス四方向分
#define DRAW_STA_DIR4_MAX (DRAW_STA_MAX*DIR_MAX4)

//--------------------------------------------------------------
//  基本+演技専用ステータス
//--------------------------------------------------------------
//１種類
#define DRAW_STA_ACT0 (DRAW_STA_MAX)
#define DRAW_STA_ACT0_ANMNO_MAX ((DRAW_STA_MAX*DIR_MAX4)+1)

//２種類
#define DRAW_STA_ACT1 (DRAW_STA_ACT0+1)
#define DRAW_STA_ACT1_ANMNO_MAX (DRAW_STA_ACT0_ANMNO_MAX+1)

//３種類
#define DRAW_STA_ACT2 (DRAW_STA_ACT0+2)
#define DRAW_STA_ACT2_ANMNO_MAX (DRAW_STA_ACT0_ANMNO_MAX+2)

//--------------------------------------------------------------
///  PC受付専用ステータス
//--------------------------------------------------------------
#define DRAW_STA_PC_BOW (DRAW_STA_ACT0)
#define DRAW_STA_PCWOMAN_DIR4_MAX (DRAW_STA_DIR4_MAX+1)

//--------------------------------------------------------------
///  波乗り主人公専用ステータス
//--------------------------------------------------------------
#define DRAW_STA_SWIMHERO_MAX (4)

//--------------------------------------------------------------
/// アイテムゲット用描画ステータス
//--------------------------------------------------------------
#define DRAW_STA_ITEMGET_STOP 0x00 ///<停止
#define DRAW_STA_ITEMGET_ANM 0x01 ///<アニメ
#define DRAW_STA_ITEMGET_MAX 0x02 ///<データインデックス最大

//--------------------------------------------------------------
/// PC預けアニメ用描画ステータス
//--------------------------------------------------------------
#define DRAW_STA_PCAZUKE_STOP 0x00 ///<停止
#define DRAW_STA_PCAZUKE_ANM0 0x01 ///<預けアニメ
#define DRAW_STA_PCAZUKE_ANM1 0x02 ///<受け取りアニメ
#define DRAW_STA_PCAZUKE_MAX 0x03 ///<最大

//--------------------------------------------------------------
/// 自機レポート用描画ステータス
//--------------------------------------------------------------
#define DRAW_STA_SAVEHERO 0x00
#define DRAW_STA_SAVEHERO_MAX 0x01

//--------------------------------------------------------------
/// フィールド動作モデル 自機専用描画ステータス
/// 基本歩き系はDRAW_STA_WALK_7Fまで同一
//--------------------------------------------------------------
#define DRAW_STA_HERO_DASH_4F (DRAW_STA_MAX) ///<移動　ダッシュ 4フレーム
#define DRAW_STA_HERO_DASH_6F (DRAW_STA_MAX+1) ///<移動　ダッシュ 6フレーム
#define DRAW_STA_HERO_MAX (DRAW_STA_HERO_DASH_6F+1) ///自機最大

//描画ステータス4方向分 自機
#define DRAW_STA_HERO_DIR4_MAX (DRAW_STA_HERO_MAX*DIR_MAX4)

//--------------------------------------------------------------
/// フィールド動作モデル 自転車自機専用描画ステータス
//--------------------------------------------------------------
#if 0 //wb null
///自機　ジャンプ台
#define DRAW_STA_TAKE_OFF_16F  (DRAW_STA_MAX)
///自機　ジャンプ台
#define DRAW_STA_TAKE_OFF_8F  (DRAW_STA_MAX+1)
///描画ステータス最大　自機
#define DRAW_STA_MAX_CYCLEHERO  (DRAW_STA_TAKE_OFF_8F+1)
#endif

//--------------------------------------------------------------
///  フィールド動作モデル 自機釣り専用描画ステータス
//--------------------------------------------------------------
#define DRAW_STA_FISH_START   0x00
#define DRAW_STA_FISH_END     0x01
#define DRAW_STA_FISH_ON      0x02
#define DRAW_STA_FISH_HIT     0x03
#define DRAW_STA_FISH_MAX     0x04

///4方向分最大
#define DRAW_STA_FISH_DIR4_MAX (DRAW_STA_FISH_MAX*DIR_MAX4)

//--------------------------------------------------------------
///  フィールド動作モデル 自機万歳専用描画ステータス
//--------------------------------------------------------------
#if 0 //wb null
#define DRAW_STA_BANZAI   0x00
#define DRAW_STA_BANZAI_UKE  0x01
#define DRAW_STA_MAX_BANZAI (DRAW_STA_BANZAI_UKE+1)
#endif

//--------------------------------------------------------------
///  フィールド動作モデル シン・ム描画ステータス
//--------------------------------------------------------------
#define DRAW_STA_SHINMU_A_STOP_DOWN 0x00
#define DRAW_STA_SHINMU_A_GUTARI (DRAW_STA_MAX)
#define DRAW_STA_SHINMU_A_FLY_UP (DRAW_STA_MAX+1)
#define DRAW_STA_SHINMU_A_FLY (DRAW_STA_MAX+2)
#define DRAW_STA_SHINMU_A_MAX (DRAW_STA_MAX+3)

#define DRAW_STA_SHINMU_A_ANMNO_STOP   0x00
#define DRAW_STA_SHINMU_A_ANMNO_GUTARI 0x01
#define DRAW_STA_SHINMU_A_ANMNO_FLY_UP 0x02
#define DRAW_STA_SHINMU_A_ANMNO_FLY    0x03
#define DRAW_STA_SHINMU_A_ANMNO_MAX    0x04

#define DRAW_STA_SHINMU_B_STOP 0x00
#define DRAW_STA_SHINMU_B_HOERU     (DRAW_STA_MAX)
#define DRAW_STA_SHINMU_B_TURN      (DRAW_STA_MAX+1)
#define DRAW_STA_SHINMU_B_MAX (DRAW_STA_MAX+2)

#define DRAW_STA_SHINMU_B_ANMNO_STOP_U   0x00
#define DRAW_STA_SHINMU_B_ANMNO_STOP_D   0x01
#define DRAW_STA_SHINMU_B_ANMNO_HOERU    0x02
#define DRAW_STA_SHINMU_B_ANMNO_TURN     0x03
#define DRAW_STA_SHINMU_B_ANMNO_MAX      0x04

#define DRAW_STA_SHINMU_C_STOP 0x00
#define DRAW_STA_SHINMU_C_FLY  (DRAW_STA_MAX)

#define DRAW_STA_SHINMU_C_ANMNO_FLY    0x00
#define DRAW_STA_SHINMU_C_ANMNO_MAX    0x01

//--------------------------------------------------------------
///  フィールド動作モデル くもの巣専用描画ステータス
//--------------------------------------------------------------
#define DRAW_STA_SPIDER_STOP 0x00
#define DRAW_STA_SPIDER_REMOVE 0x01
#define DRAW_STA_SPIDER_MAX 0x02

//--------------------------------------------------------------
/// フィールド動作モデル メロディア専用描画ステータス
//--------------------------------------------------------------
#define DRAW_STA_MELODYER_STOP 0x00
#define DRAW_STA_MELODYER_SPIN (DRAW_STA_MAX)
#define DRAW_STA_MELODYER_SPIN_POSE (DRAW_STA_MAX+1)
#define DRAW_STA_MELODYER_SHAKE (DRAW_STA_MAX+2)
#define DRAW_STA_MELODYER_MAX (DRAW_STA_MAX+3)

#define DRAW_STA_MELODYER_ANMNO_STOP 0x00
#define DRAW_STA_MELODYER_ANMNO_SPIN 0x01
#define DRAW_STA_MELODYER_ANMNO_SPIN_POSE 0x02
#define DRAW_STA_MELODYER_ANMNO_SHAKE 0x03
#define DRAW_STA_MELODYER_ANMNO_MAX 0x04

//======================================================================
//  フィールド動作モデル アニメーションコマンドコード
//======================================================================
//--------------------------------------------------------------
//  フィールド動作モデル アニメーションコマンドコード
//--------------------------------------------------------------
#define AC_DIR_U 0x00 ///<上に向く [rail]
#define AC_DIR_D 0x01 ///<下に向く [rail]
#define AC_DIR_L 0x02 ///<左に向く [rail]
#define AC_DIR_R 0x03 ///<右に向く [rail]
#define AC_WALK_U_32F 0x04 ///<上に移動 32フレーム
#define AC_WALK_D_32F 0x05 ///<下に移動 32フレーム
#define AC_WALK_L_32F 0x06 ///<左に移動 32フレーム
#define AC_WALK_R_32F 0x07 ///<右に移動 32フレーム
#define AC_WALK_U_16F 0x08 ///<上に移動 16フレーム [rail]
#define AC_WALK_D_16F 0x09 ///<下に移動 16フレーム [rail]
#define AC_WALK_L_16F 0x0a ///<左に移動 16フレーム [rail]
#define AC_WALK_R_16F 0x0b ///<右に移動 16フレーム [rail]
#define AC_WALK_U_8F 0x0c ///<上に移動 8フレーム [rail]
#define AC_WALK_D_8F 0x0d ///<下に移動 8フレーム [rail]
#define AC_WALK_L_8F 0x0e ///<左に移動 8フレーム [rail]
#define AC_WALK_R_8F 0x0f ///<右に移動 8フレーム [rail]
#define AC_WALK_U_4F 0x10 ///<上に移動 4フレーム [rail]
#define AC_WALK_D_4F 0x11 ///<下に移動 4フレーム [rail]
#define AC_WALK_L_4F 0x12 ///<左に移動 4フレーム [rail]
#define AC_WALK_R_4F 0x13 ///<右に移動 4フレーム [rail]
#define AC_WALK_U_2F 0x14 ///<上に移動 2フレーム [rail]
#define AC_WALK_D_2F 0x15 ///<下に移動 2フレーム [rail]
#define AC_WALK_L_2F 0x16 ///<左に移動 2フレーム [rail]
#define AC_WALK_R_2F 0x17 ///<右に移動 2フレーム [rail]
#define AC_STAY_WALK_U_32F 0x18 ///<上向きにその場歩き 32フレーム
#define AC_STAY_WALK_D_32F 0x19 ///<下向きにその場歩き 32フレーム
#define AC_STAY_WALK_L_32F 0x1a ///<左向きにその場歩き 32フレーム
#define AC_STAY_WALK_R_32F 0x1b ///<右向きにその場歩き 32フレーム
#define AC_STAY_WALK_U_16F 0x1c ///<上向きにその場歩き 16フレーム [rail]
#define AC_STAY_WALK_D_16F 0x1d ///<下向きにその場歩き 16フレーム [rail]
#define AC_STAY_WALK_L_16F 0x1e ///<左向きにその場歩き 16フレーム [rail]
#define AC_STAY_WALK_R_16F 0x1f ///<右向きにその場歩き 16フレーム [rail]
#define AC_STAY_WALK_U_8F 0x20 ///<上向きにその場歩き 8フレーム [rail]
#define AC_STAY_WALK_D_8F 0x21 ///<下向きにその場歩き 8フレーム [rail]
#define AC_STAY_WALK_L_8F 0x22 ///<左向きにその場歩き 8フレーム [rail]
#define AC_STAY_WALK_R_8F 0x23 ///<右向きにその場歩き 8フレーム [rail]
#define AC_STAY_WALK_U_4F 0x24 ///<上向きにその場歩き 4フレーム
#define AC_STAY_WALK_D_4F 0x25 ///<下向きにその場歩き 4フレーム
#define AC_STAY_WALK_L_4F 0x26 ///<左向きにその場歩き 4フレーム
#define AC_STAY_WALK_R_4F 0x27 ///<右向きにその場歩き 4フレーム
#define AC_STAY_WALK_U_2F 0x28 ///<上向きにその場歩き 2フレーム [rail]
#define AC_STAY_WALK_D_2F 0x29 ///<下向きにその場歩き 2フレーム [rail]
#define AC_STAY_WALK_L_2F 0x2a ///<左向きにその場歩き 2フレーム [rail]
#define AC_STAY_WALK_R_2F 0x2b ///<右向きにその場歩き 2フレーム [rail]
#define AC_STAY_JUMP_U_16F 0x2c ///<上向きにその場ジャンプ 16フレーム
#define AC_STAY_JUMP_D_16F 0x2d ///<下向きにその場ジャンプ 16フレーム
#define AC_STAY_JUMP_L_16F 0x2e ///<左向きにその場ジャンプ 16フレーム
#define AC_STAY_JUMP_R_16F 0x2f ///<右向きにその場ジャンプ 16フレーム
#define AC_STAY_JUMP_U_8F 0x30 ///<上向きにその場ジャンプ 8フレーム
#define AC_STAY_JUMP_D_8F 0x31 ///<下向きにその場ジャンプ 8フレーム
#define AC_STAY_JUMP_L_8F 0x32 ///<左向きにその場ジャンプ 8フレーム
#define AC_STAY_JUMP_R_8F 0x33 ///<右向きにその場ジャンプ 8フレーム
#define AC_JUMP_U_1G_8F 0x34 ///<上向きにジャンプ 1grid 8フレーム [rail]
#define AC_JUMP_D_1G_8F 0x35 ///<下向きにジャンプ 1grid 8フレーム [rail]
#define AC_JUMP_L_1G_8F 0x36 ///<左向きにジャンプ 1grid 8フレーム [rail]
#define AC_JUMP_R_1G_8F 0x37 ///<右向きにジャンプ 1grid 8フレーム [rail]
#define AC_JUMP_U_2G_16F 0x38 ///<上向きにジャンプ 2grid 16フレーム [rail]
#define AC_JUMP_D_2G_16F 0x39 ///<下向きにジャンプ 2grid 16フレーム [rail]
#define AC_JUMP_L_2G_16F 0x3a ///<左向きにジャンプ 2grid 16フレーム [rail]
#define AC_JUMP_R_2G_16F 0x3b ///<右向きにジャンプ 2grid 16フレーム [rail]
#define AC_WAIT_1F 0x3c ///<1フレーム待ち [rail]
#define AC_WAIT_2F 0x3d ///<2フレーム待ち [rail]
#define AC_WAIT_4F 0x3e ///<4フレーム待ち [rail]
#define AC_WAIT_8F 0x3f ///<8フレーム待ち [rail]
#define AC_WAIT_15F 0x40 ///<15フレーム待ち [rail]
#define AC_WAIT_16F 0x41 ///<16フレーム待ち [rail]
#define AC_WAIT_32F 0x42 ///<32フレーム待ち [rail]
#define AC_WARP_UP 0x43 ///<上にワープ
#define AC_WARP_DOWN 0x44 ///<下にワープ
#define AC_VANISH_ON 0x45 ///<非表示にする [rail]
#define AC_VANISH_OFF 0x46 ///<表示にする [rail]
#define AC_DIR_PAUSE_ON 0x47 ///<方向固定する [rail]
#define AC_DIR_PAUSE_OFF 0x48 ///<方向固定しない [rail]
#define AC_ANM_PAUSE_ON 0x49 ///<アニメポーズ [rail]
#define AC_ANM_PAUSE_OFF 0x4a ///<アニメポーズを解除 [rail]
#define AC_MARK_GYOE 0x4b ///<!マークを出す [rail]
#define AC_WALK_U_6F 0x4c ///<上方向に移動 6フレーム
#define AC_WALK_D_6F 0x4d ///<下方向に移動 6フレーム
#define AC_WALK_L_6F 0x4e ///<左方向に移動 6フレーム
#define AC_WALK_R_6F 0x4f ///<右方向に移動 6フレーム
#define AC_WALK_U_3F 0x50 ///<上方向に移動 3フレーム
#define AC_WALK_D_3F 0x51 ///<下方向に移動 3フレーム
#define AC_WALK_L_3F 0x52 ///<左方向に移動 3フレーム
#define AC_WALK_R_3F 0x53 ///<右方向に移動 3フレーム
#define AC_WALK_U_1F 0x54 ///<上方向に移動 1フレーム(システム未対応 使用禁止
#define AC_WALK_D_1F 0x55 ///<下方向に移動 1フレーム(システム未対応 使用禁止
#define AC_WALK_L_1F 0x56 ///<左方向に移動 1フレーム(システム未対応 使用禁止
#define AC_WALK_R_1F 0x57 ///<右方向に移動 1フレーム(システム未対応 使用禁止
#define AC_DASH_U_4F 0x58 ///<上方向に移動 4フレーム(自機専用 [rail]
#define AC_DASH_D_4F 0x59 ///<下方向に移動 4フレーム(自機専用 [rail]
#define AC_DASH_L_4F 0x5a ///<左方向に移動 4フレーム(自機専用 [rail]
#define AC_DASH_R_4F 0x5b ///<右方向に移動 4フレーム(自機専用 [rail]
#define AC_JUMPHI_L_1G_16F 0x5c ///<左方向に高いジャンプ 1grid 16フレーム(自機専用
#define AC_JUMPHI_R_1G_16F 0x5d ///<右方向に高いジャンプ 1grid 16フレーム(自機専用
#define AC_JUMPHI_L_3G_32F 0x5e ///<左方向に高いジャンプ 3grid 32フレーム(自機専用
#define AC_JUMPHI_R_3G_32F 0x5f ///<右方向に高いジャンプ 3grid 32フレーム(自機専用
#define AC_WALK_U_7F 0x60 ///<上方向に移動 7フレーム
#define AC_WALK_D_7F 0x61 ///<下方向に移動 7フレーム
#define AC_WALK_L_7F 0x62 ///<左方向に移動 7フレーム
#define AC_WALK_R_7F 0x63 ///<右方向に移動 7フレーム
#define AC_ACT0_STOP 0x64 ///<演技(特定OBJ専用
#define AC_HIDE_PULLOFF 0x65 ///<隠れ蓑脱ぐ(MV_HIDE専用
#define AC_HERO_BANZAI 0x66 ///<PC両手差出(自機専用
#define AC_MARK_SAISEN 0x67 ///<再戦マーク表示
#define AC_HERO_BANZAI_UKE 0x68 ///<PC両手差出受け取り(自機専用
#define AC_WALKGL_U_8F 0x69 ///<左壁移動 上 8フレーム
#define AC_WALKGL_D_8F 0x6a ///<左壁移動 下 8フレーム
#define AC_WALKGL_L_8F 0x6b ///<左壁移動 左 8フレーム
#define AC_WALKGL_R_8F 0x6c ///<左壁移動 右 8フレーム
#define AC_WALKGR_U_8F 0x6d ///<右壁移動 上 8フレーム
#define AC_WALKGR_D_8F 0x6e ///<右壁移動 下 8フレーム
#define AC_WALKGR_L_8F 0x6f ///<右壁移動 左 8フレーム
#define AC_WALKGR_R_8F 0x70 ///<右壁移動 右 8フレーム
#define AC_WALKGU_U_8F 0x71 ///<上壁移動 上 8フレーム
#define AC_WALKGU_D_8F 0x72 ///<上壁移動 下 8フレーム
#define AC_WALKGU_L_8F 0x73 ///<上壁移動 左 8フレーム
#define AC_WALKGU_R_8F 0x74 ///<上壁移動 右 8フレーム
#define AC_JUMP_U_3G_24F 0x75 ///<上向きにジャンプ 3grid 24フレーム
#define AC_JUMP_D_3G_24F 0x76 ///<上向きにジャンプ 3grid 24フレーム
#define AC_JUMP_L_3G_24F 0x77 ///<上向きにジャンプ 3grid 24フレーム
#define AC_JUMP_R_3G_24F 0x78 ///<上向きにジャンプ 3grid 24フレーム
#define AC_DASHGL_U_4F 0x79 ///<左壁移動 上 4フレーム(自機専用
#define AC_DASHGL_D_4F 0x7a ///<左壁移動 下 4フレーム(自機専用
#define AC_DASHGL_L_4F 0x7b ///<左壁移動 左 4フレーム(自機専用
#define AC_DASHGL_R_4F 0x7c ///<左壁移動 右 4フレーム(自機専用
#define AC_DASHGR_U_4F 0x7d ///<右壁移動 上 4フレーム(自機専用
#define AC_DASHGR_D_4F 0x7e ///<右壁移動 下 4フレーム(自機専用
#define AC_DASHGR_L_4F 0x7f ///<右壁移動 左 4フレーム(自機専用
#define AC_DASHGR_R_4F 0x80 ///<右壁移動 右 4フレーム(自機専用
#define AC_DASHGU_U_4F 0x81 ///<上壁移動 上 4フレーム(自機専用
#define AC_DASHGU_D_4F 0x82 ///<上壁移動 下 4フレーム(自機専用
#define AC_DASHGU_L_4F 0x83 ///<上壁移動 左 4フレーム(自機専用
#define AC_DASHGU_R_4F 0x84 ///<上壁移動 右 4フレーム(自機専用
#define AC_JUMPGL_U_1G_8F 0x85 ///<左壁 上向きにジャンプ 1grid 8フレーム
#define AC_JUMPGL_D_1G_8F 0x86 ///<左壁 下向きにジャンプ 1grid 8フレーム
#define AC_JUMPGL_L_1G_8F 0x87 ///<左壁 左向きにジャンプ 1grid 8フレーム
#define AC_JUMPGL_R_1G_8F 0x88 ///<左壁 右向きにジャンプ 1grid 8フレーム
#define AC_JUMPGR_U_1G_8F 0x89 ///<右壁 上向きにジャンプ 1grid 8フレーム
#define AC_JUMPGR_D_1G_8F 0x8a ///<右壁 下向きにジャンプ 1grid 8フレ2ーム
#define AC_JUMPGR_L_1G_8F 0x8b ///<右壁 左向きにジャンプ 1grid 8フレーム
#define AC_JUMPGR_R_1G_8F 0x8c ///<右壁 右向きにジャンプ 1grid 8フレーム
#define AC_JUMPGU_U_1G_8F 0x8d ///<上壁 上向きにジャンプ 1grid 8フレーム
#define AC_JUMPGU_D_1G_8F 0x8e ///<上壁 下向きにジャンプ 1grid 8フレーム
#define AC_JUMPGU_L_1G_8F 0x8f ///<上壁 左向きにジャンプ 1grid 8フレーム
#define AC_JUMPGU_R_1G_8F 0x90 ///<上壁 右向きにジャンプ 1grid 8フレーム
#define AC_WALKGU_U_4F 0x91 ///<上壁移動 上 4フレーム
#define AC_WALKGU_D_4F 0x92 ///<上壁移動 下 4フレーム
#define AC_WALKGU_L_4F 0x93 ///<上壁移動 左 4フレーム
#define AC_WALKGU_R_4F 0x94 ///<上壁移動 右 4フレーム
#define AC_WALKGU_U_2F 0x95 ///<上壁移動 上 2フレーム
#define AC_WALKGU_D_2F 0x96 ///<上壁移動 下 2フレーム
#define AC_WALKGU_L_2F 0x97 ///<上壁移動 左 2フレーム
#define AC_WALKGU_R_2F 0x98 ///<上壁移動 右 2フレーム
#define AC_MARK_GYOE_TWAIT 0x99 ///<!マークを出す グラフィック転送待ち版
#define AC_HERO_ITEMGET 0x9a ///<自機アイテムゲットアニメ [rail]
#define AC_SANDWALK_U_16F 0x9b ///<上に移動 16F移動、表示アニメは2Fアニメ
#define AC_SANDWALK_D_16F 0x9c ///<下に移動 16F移動、表示アニメは2Fアニメ
#define AC_SANDWALK_L_16F 0x9d ///<左に移動 16F移動、表示アニメは2Fアニメ
#define AC_SANDWALK_R_16F 0x9e ///<右に移動 16F移動、表示アニメは2Fアニメ
#define AC_MARK_HATENA 0x9f ///<？マークを出す
#define AC_MARK_ONPU 0xa0 ///<♪マークを出す
#define AC_MARK_TENTEN 0xa1 ///<…マークを出す
#define AC_HERO_CUTIN 0xa2 ///<自機カットインアニメ
#define AC_DASH_U_6F 0xa3 ///<上方向に移動 6フレーム(自機専用
#define AC_DASH_D_6F 0xa4 ///<下方向に移動 6フレーム(自機専用
#define AC_DASH_L_6F 0xa5 ///<左方向に移動 6フレーム(自機専用
#define AC_DASH_R_6F 0xa6 ///<右方向に移動 6フレーム(自機専用
#define AC_WALK_U_12F 0xa7 ///<上に移動 10フレーム
#define AC_WALK_D_12F 0xa8 ///<上に移動 10フレーム
#define AC_WALK_L_12F 0xa9 ///<上に移動 10フレーム
#define AC_WALK_R_12F 0xaa ///<上に移動 10フレーム
#define AC_SPIDER_REMOVE 0xab ///<くもの巣排除アニメ SPIDER専用
#define AC_SHIN_MU_HOERU 0xac ///<シンム　吠えるアニメ
#define AC_SHIN_MU_FLY_L 0xad ///<シンム　飛ぶ　左
#define AC_SHIN_MU_FLY_UPPER 0xae ///<シンム　上昇
#define AC_SHIN_MU_GUTARI 0xaf ///<シンム　ぐったりアニメ
#define AC_SHIN_MU_TURN 0xb0 ///<シンム　振り向き
#define AC_MELODYER_SPIN 0xb1 ///<メロディア 回転
#define AC_MELODYER_SPIN_POSE 0xb2 ///<メロディア 回転+ポーズ
#define AC_MELODYER_SHAKE 0xb3 ///<メロディア 上下揺れ
#define AC_SHIN_MU_OFFS_CLEAR 0xb4 ///<シンム 描画オフセットをクリア
#define AC_ACT0_LOOP 0xb5 ///<演技(特定OBJ専用
#define AC_ACT1_STOP 0xb6 ///<演技(特定OBJ専用
#define AC_ACT1_LOOP 0xb7 ///<演技(特定OBJ専用
#define AC_NINJA_ACT0 0xb8 ///<演技(NINJA専用 出現アニメ
#define AC_NINJA_ACT1 0xb9 ///<演技(NINJA専用 消去アニメ
#define AC_ACT2_STOP 0xba ///<演技(特定OBJ専用
#define AC_ACT2_LOOP 0xbb ///<演技(特定OBJ専用
#define AC_SHIN_MU_C_FLY 0xbc ///<シンムC 滞空アニメ
#define ACMD_MAX 0xbd ///<アニメーションコマンド最大数

#define ACMD_END 0xfe ///<アニメーションコマンド終了
#define ACMD_NOT 0xff ///<非アニメーションコマンド

//芝居系　判り易い名前で置き換え
#define AC_PC_BOW (AC_ACT0_STOP) ///<PCお辞儀
#define AC_RIVEL_ACT0 (AC_ACT0_LOOP) ///<ライバル専用 つま先アニメ
#define AC_SUPPORT_ACT0 (AC_ACT0_STOP) ///<サポート専用 帽子アニメ
#define AC_N_ACT0 (AC_ACT0_STOP) ///<N専用 両手アニメ
#define AC_N_ACT1 (AC_ACT1_STOP) ///<N専用 首アニメ
#define AC_SAGE1_ACT0 (AC_ACT0_STOP) ///<SAGE1専用 両手アニメ
#define AC_CHAMPION_ACT0 (AC_ACT0_STOP) ///<CHAMPION専用 加味揺れアニメ
#define AC_N_ACT2 (AC_ACT2_LOOP) ///<N専用 腕アニメ

#endif //MMDL_CODE_H_FILE
