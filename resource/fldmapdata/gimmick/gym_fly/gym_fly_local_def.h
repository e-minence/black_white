//======================================================================
/**
 * @file	gym_fly_local_def.h
 * @brief	数値定義ファイル
 * @note	このファイルはコンバーターから出力されます
 *
 */
//======================================================================

//大砲のグリッド位置座標
#define CAN1_GX (15)
#define CAN1_GY (0)
#define CAN1_GZ (28)
#define CAN2_GX (10)
#define CAN2_GY (2)
#define CAN2_GZ (19)
#define CAN3_GX (20)
#define CAN3_GY (4)
#define CAN3_GZ (29)
#define CAN4_GX (10)
#define CAN4_GY (4)
#define CAN4_GZ (25)
#define CAN5_GX (15)
#define CAN5_GY (4)
#define CAN5_GZ (13)
#define CAN6_GX (5)
#define CAN6_GY (6)
#define CAN6_GZ (8)
#define CAN7_GX (7)
#define CAN7_GY (6)
#define CAN7_GZ (36)
#define CAN8_GX (26)
#define CAN8_GY (6)
#define CAN8_GZ (34)
#define CAN9_GX (24)
#define CAN9_GY (6)
#define CAN9_GZ (6)
#define CAN10_GX (15)
#define CAN10_GY (10)
#define CAN10_GZ (3)
//着地座標
#define STAND1_GX (15)
#define STAND1_GY (2)
#define STAND1_GZ (19)
#define STAND2_GX (15)
#define STAND2_GY (0)
#define STAND2_GZ (35)
#define STAND3_GX (21)
#define STAND3_GY (2)
#define STAND3_GZ (19)
#define STAND4_GX (3)
#define STAND4_GY (4)
#define STAND4_GZ (19)
#define STAND5_GX (10)
#define STAND5_GY (4)
#define STAND5_GZ (29)
#define STAND6_GX (10)
#define STAND6_GY (4)
#define STAND6_GZ (13)
#define STAND7_GX (20)
#define STAND7_GY (4)
#define STAND7_GZ (25)
#define STAND8_GX (25)
#define STAND8_GY (4)
#define STAND8_GZ (13)
#define STAND9_GX (5)
#define STAND9_GY (4)
#define STAND9_GZ (13)
#define STAND10_GX (5)
#define STAND10_GY (6)
#define STAND10_GZ (34)
#define STAND11_GX (26)
#define STAND11_GY (6)
#define STAND11_GZ (36)
#define STAND12_GX (26)
#define STAND12_GY (6)
#define STAND12_GZ (4)
#define STAND13_GX (15)
#define STAND13_GY (10)
#define STAND13_GZ (6)
#define STAND14_GX (15)
#define STAND14_GY (0)
#define STAND14_GZ (32)
//各大砲の初期回転状態 0
#define CAN1_ROT (0)	//回転なし 上向き
#define CAN2_ROT (0)	//回転なし 上向き
#define CAN3_ROT (1)	//半時計90度 左向き
#define CAN4_ROT (3)	//反時計270度 右向き
#define CAN5_ROT (1)	//反時計90度 左向き
#define CAN6_ROT (2)	//反時計180度 下向き
#define CAN7_ROT (3)	//反時計270度 右向き
#define CAN8_ROT (0)	//回転なし 上向き
#define CAN9_ROT (1)	//反時計90度 左向き
#define CAN10_ROT (2)	//反時計180度 下向き
//大砲扉の開くフレーム
#define CAN_UP_OP_FRM (1)	//基準大砲上向きのとき
#define CAN_DOWN_OP_FRM (1)	//基準大砲下向きのとき
#define CAN_LEFT_OP_FRM (15)	//基準大砲左向きのとき
#define CAN_RIGHT_OP_FRM (15)	//基準大砲右向きのとき
//大砲扉の開ききるフレーム
#define CAN_UP_OPED_FRM (15)	//基準大砲上向きのとき
#define CAN_DOWN_OPED_FRM (15)	//基準大砲下向きのとき
#define CAN_LEFT_OPED_FRM (30)	//基準大砲左向きのとき
#define CAN_RIGHT_OPED_FRM (30)	//基準大砲右向きのとき
//大砲扉の閉まりきるフレーム
#define CAN_UP_CL_FRM (30)	//基準大砲上向きのとき
#define CAN_DOWN_CL_FRM (30)	//基準大砲下向きのとき
#define CAN_LEFT_CL_FRM (45)	//基準大砲左向きのとき
#define CAN_RIGHT_CL_FRM (45)	//基準大砲右向きのとき
//大砲が火を吹くフレーム
#define CAN_UP_FIRE_FRM (69)
#define CAN_DOWN_FIRE_FRM (69)
#define CAN_RIGHT_FIRE_FRM (84)
#define CAN_LEFT_FIRE_FRM (84)
//大砲のを見るカメラのピッチ
#define CAN1_PITCH (6000)
#define CAN2_PITCH (7000)
#define CAN3_PITCH (3000)
#define CAN4_PITCH (4000)
#define CAN5_PITCH (9000)
#define CAN6_PITCH (7000)
#define CAN7_PITCH (10000)
#define CAN8_PITCH (8000)
#define CAN9_PITCH (4000)
#define CAN10_PITCH (5000)
#define CAN11_PITCH (11000)
#define CAN12_PITCH (2000)
#define CAN13_PITCH (6000)
#define CAN14_PITCH (7000)
//大砲を見るカメラのヨー
#define CAN1_YAW (3000)
#define CAN2_YAW (-2000)
#define CAN3_YAW (3000)
#define CAN4_YAW (-3000)
#define CAN5_YAW (6000)
#define CAN6_YAW (-3000)
#define CAN7_YAW (-3000)
#define CAN8_YAW (8000)
#define CAN9_YAW (-9000)
#define CAN10_YAW (4000)
#define CAN11_YAW (11000)
#define CAN12_YAW (-3000)
#define CAN13_YAW (-15000)
#define CAN14_YAW (0)
//カメラ回転にかかるフレーム
#define CAN1_ROT_COST_FRM (20)
#define CAN2_ROT_COST_FRM (20)
#define CAN3_ROT_COST_FRM (20)
#define CAN4_ROT_COST_FRM (20)
#define CAN5_ROT_COST_FRM (20)
#define CAN6_ROT_COST_FRM (20)
#define CAN7_ROT_COST_FRM (20)
#define CAN8_ROT_COST_FRM (20)
#define CAN9_ROT_COST_FRM (20)
#define CAN10_ROT_COST_FRM (20)
#define CAN11_ROT_COST_FRM (20)
#define CAN12_ROT_COST_FRM (20)
#define CAN13_ROT_COST_FRM (20)
#define CAN14_ROT_COST_FRM (20)
//自機飛行アニメに追いつきを開始するフレーム
#define CAM1_TRACE_START_FRM (50)
#define CAM2_TRACE_START_FRM (50)
#define CAM3_TRACE_START_FRM (50)
#define CAM4_TRACE_START_FRM (50)
#define CAM5_TRACE_START_FRM (50)
#define CAM6_TRACE_START_FRM (50)
#define CAM7_TRACE_START_FRM (50)
#define CAM8_TRACE_START_FRM (50)
#define CAM9_TRACE_START_FRM (50)
#define CAM10_TRACE_START_FRM (47)
#define CAM11_TRACE_START_FRM (50)
#define CAM12_TRACE_START_FRM (47)
#define CAM13_TRACE_START_FRM (50)
#define CAM14_TRACE_START_FRM (46)
//カメラが自機に追いつくフレーム数
#define CAM1_TRACE_COST_FRM (25)
#define CAM2_TRACE_COST_FRM (25)
#define CAM3_TRACE_COST_FRM (25)
#define CAM4_TRACE_COST_FRM (25)
#define CAM5_TRACE_COST_FRM (25)
#define CAM6_TRACE_COST_FRM (25)
#define CAM7_TRACE_COST_FRM (25)
#define CAM8_TRACE_COST_FRM (25)
#define CAM9_TRACE_COST_FRM (25)
#define CAM10_TRACE_COST_FRM (40)
#define CAM11_TRACE_COST_FRM (25)
#define CAM12_TRACE_COST_FRM (30)
#define CAM13_TRACE_COST_FRM (20)
#define CAM14_TRACE_COST_FRM (1)
//着地フレームオフセット（最終フレームからどれくらい前か）
#define CAM1_STAND_FRM (12)
#define CAM2_STAND_FRM (11)
#define CAM3_STAND_FRM (14)
#define CAM4_STAND_FRM (24)
#define CAM5_STAND_FRM (10)
#define CAM6_STAND_FRM (13)
#define CAM7_STAND_FRM (13)
#define CAM8_STAND_FRM (12)
#define CAM9_STAND_FRM (11)
#define CAM10_STAND_FRM (10)
#define CAM11_STAND_FRM (15)
#define CAM12_STAND_FRM (-10)
#define CAM13_STAND_FRM (12)
#define CAM14_STAND_FRM (16)
//壁当たりフレーム
#define WALL_HIT_FRM (70)
//カメラ振動数
#define CAM_SHAKE_NUM (2)
//カメラ振幅
#define CAM_SHAKE_WIDTH (8)
//カメラ振動間隔
#define CAM_SHAKE_WAIT (1)
