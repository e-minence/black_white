//============================================================================================
/**
 * @file   trcard_cgx_def.h
 * @bfief  トレーナーカード画面定義ヘッダ
 * @author Akito Mori / Nozomu Saito (WB移植は有泉くん)
 * @date   10.03.09
 */
//============================================================================================
#ifndef __TRCARD_CGX_DEF_H__
#define __TRCARD_CGX_DEF_H__

#define TR_BG_BASE    (1) //プラス方向に使っていく
//トレーナー画像サイズ  (男女主人公のキャラが32x4、ユニオントレーナーは10*11)
#define TR_BG_SIZE    (32*4)  

//メッセージ表示領域(マイナス方向に使っていく)
#define MSG_BG_BASE (1023)

//サイン
#define SIGN_CGX  (TR_BG_BASE)
#define SIGN_PX (4)
#define SIGN_PY (11)
#define SIGN_SX (24)
#define SIGN_SY (8)
#define SIGN_BYTE_SIZE  (SIGN_SX*SIGN_SY*64)
#define TR_SIGN_SIZE  (SIGN_SX*SIGN_SY) //サインサイズ

#define SIGN_ANIME_PX ( 10 )
#define SIGN_ANIME_PY ( 11 )
#define SIGN_ANIME_SX ( 12 )
#define SIGN_ANIME_SY (  8 )


//リーダーアイコン
#define LEADER_ICON_X (7)
#define LEADER_ICON_JY  (7)
#define LEADER_ICON_KY  (14)
#define LEADER_ICON_OX  (6)
#define LEADER_ICON_OY  (5)
#define LEADER_ICON_SX  (5)
#define LEADER_ICON_SY  (4)
#define LEADER_ICON_PY  (9)

#endif  //__TRCARD_CGX_DEF_H__
