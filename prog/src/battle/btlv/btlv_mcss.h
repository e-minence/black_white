
//============================================================================================
/**
 * @file  btlv_mcss.h
 * @brief �|�P����MCSS�Ǘ�
 * @author  soga
 * @date  2008.11.14
 */
//============================================================================================

#pragma once

#include "system/mcss.h"
#include "poke_tool/poke_tool.h"
#include "battle/battle.h"
#include "battle/btl_common.h"

//�|�P�����̗����ʒu��`
// 1vs1
//    BB
//
// AA
//
// 2vs2
//    D B
//
// A C
//
// 3vs3
//     F
//    D B
//
// A C
//  E
typedef enum{
  BTLV_MCSS_POS_AA = 0,
  BTLV_MCSS_POS_BB,
  BTLV_MCSS_POS_A,
  BTLV_MCSS_POS_B,
  BTLV_MCSS_POS_C,
  BTLV_MCSS_POS_D,
  BTLV_MCSS_POS_E,
  BTLV_MCSS_POS_F,

  BTLV_MCSS_POS_MAX,

  BTLV_MCSS_POS_TR_AA = BTLV_MCSS_POS_MAX,
  BTLV_MCSS_POS_TR_BB,
  BTLV_MCSS_POS_TR_A,
  BTLV_MCSS_POS_TR_B,
  BTLV_MCSS_POS_TR_C,
  BTLV_MCSS_POS_TR_D,

  BTLV_MCSS_POS_TOTAL,    //�|�P�����ƃg���[�i�[�����킹��MAX�l

  BTLV_MCSS_POS_PAIR_BIT = 0x02,
  BTLV_MCSS_POS_ERROR = 0xff,
}BtlvMcssPos;

typedef enum{
  BTLV_MCSS_PROJ_PERSPECTIVE = 0,
  BTLV_MCSS_PROJ_ORTHO,
  BTLV_MCSS_PROJ_MAX,
}BTLV_MCSS_PROJECTION;

typedef enum
{
  BTLV_MCSS_MEPACHI_ON    = MCSS_MEPACHI_ON,
  BTLV_MCSS_MEPACHI_OFF   = MCSS_MEPACHI_OFF,
  BTLV_MCSS_MEPACHI_FLIP  = MCSS_MEPACHI_FLIP,
  BTLV_MCSS_MEPACHI_ALWAYS_ON,    //��ɃI��
  BTLV_MCSS_MEPACHI_ALWAYS_OFF,   //��ɃI��������
}BTLV_MCSS_MEPACHI_FLAG;

typedef enum
{
  BTLV_MCSS_VANISH_ON   = MCSS_VANISH_ON,
  BTLV_MCSS_VANISH_OFF  = MCSS_VANISH_OFF,
  BTLV_MCSS_VANISH_FLIP = MCSS_VANISH_FLIP,
  BTLV_MCSS_EFFECT_VANISH_ON,
  BTLV_MCSS_EFFECT_VANISH_OFF,
  BTLV_MCSS_EFFECT_VANISH_POP,
}BTLV_MCSS_VANISH_FLAG;

typedef enum
{
  BTLV_MCSS_ANM_STOP_ON         = MCSS_ANM_STOP_ON,
  BTLV_MCSS_ANM_STOP_OFF        = MCSS_ANM_STOP_OFF,
  BTLV_MCSS_ANM_STOP_ALWAYS_ON  = MCSS_ANM_STOP_ALWAYS,
  BTLV_MCSS_ANM_STOP_ALWAYS_OFF,
}BTLV_MCSS_ANM_STOP_FLAG;

typedef enum{
  BTLV_MCSS_STATUS_FLAG_MIGAWARI          = 0x00000001,    //�݂����t���O
  BTLV_MCSS_STATUS_FLAG_RARE              = 0x00000002,    //���A�t���O
  BTLV_MCSS_STATUS_FLAG_LOOKING_MIGAWARI  = 0x00000004,    //�݂����t���O�i�����ځj

  BTLV_MCSS_STATUS_FLAG_MIGAWARI_OFF          = ( BTLV_MCSS_STATUS_FLAG_MIGAWARI  ^ 0xffffffff ),         //�݂����t���O
  BTLV_MCSS_STATUS_FLAG_RARE_OFF              = ( BTLV_MCSS_STATUS_FLAG_RARE      ^ 0xffffffff ),         //���A�t���O
  BTLV_MCSS_STATUS_FLAG_LOOKING_MIGAWARI_OFF  = ( BTLV_MCSS_STATUS_FLAG_LOOKING_MIGAWARI ^ 0xffffffff ),  //�݂����t���O�i�����ځj
}BTLV_MCSS_STATUS_FLAG;

typedef enum{
  BTLV_MCSS_REVERSE_DRAW_OFF = 0,
  BTLV_MCSS_REVERSE_DRAW_ON,
}BTLV_MCSS_REVERSE_DRAW;

typedef struct _BTLV_MCSS BTLV_MCSS;
typedef struct _BTLV_MCSS_WORK BTLV_MCSS_WORK;

typedef struct
{
  BTLV_MCSS_WORK* bmw;
  BtlvMcssPos     position;
  int             axis;               //��]��
  int             shift;              //��]�V�t�g
  fx32            radius_h;           //���������a
  fx32            radius_v;           //�c�������a
  int             frame;              //1��]���t���[����
  int             rotate_wait;        //��]�E�G�C�g
  int             count;              //��]��
  int             rotate_after_wait;  //1��]������̃E�G�C�g
  int             angle;
  int             speed;
  int             rotate_wait_count;
  int             rotate_after_wait_count;
  u32             tcb_id;
}BTLV_MCSS_MOVE_CIRCLE_PARAM;

typedef struct
{
  BTLV_MCSS_WORK* bmw;
  BtlvMcssPos     position;
  VecFx32         pos;              //�ړ�����W
  int             dir;              //����
  fx32            angle;            //�p�x
  fx32            speed;            //���x
  fx32            radius;           //���a
  int             frame;            //�t���[��
  u32             tcb_id;
}BTLV_MCSS_MOVE_SIN_PARAM;

extern  BTLV_MCSS_WORK* BTLV_MCSS_Init( BtlRule rule, GFL_TCBSYS *tcb_sys, HEAPID heapID );
extern  void            BTLV_MCSS_Exit( BTLV_MCSS_WORK *bmw );
extern  void            BTLV_MCSS_Main( BTLV_MCSS_WORK *bmw );
extern  void            BTLV_MCSS_Draw( BTLV_MCSS_WORK *bmw );
extern  void            BTLV_MCSS_Add( BTLV_MCSS_WORK *bmw, const POKEMON_PARAM *pp, int position );
extern  void            BTLV_MCSS_AddTrainer( BTLV_MCSS_WORK *bmw, int tr_type, int position );
extern  void            BTLV_MCSS_Del( BTLV_MCSS_WORK *bmw, int position );
extern  void            BTLV_MCSS_SetScaleOffset( BTLV_MCSS_WORK *bmw, fx32 scale_ofs_m, fx32 scale_ofs_e );
extern  void            BTLV_MCSS_SetPosition( BTLV_MCSS_WORK *bmw, int position, fx32 pos_x, fx32 pos_y, fx32 pos_z );
extern  void            BTLV_MCSS_SetOrthoMode( BTLV_MCSS_WORK *bmw );
extern  void            BTLV_MCSS_ResetOrthoMode( BTLV_MCSS_WORK *bmw );
extern  void            BTLV_MCSS_SetOrthoModeByPos( BTLV_MCSS_WORK *bmw, BtlvMcssPos pos );
extern  void            BTLV_MCSS_ResetOrthoModeByPos( BTLV_MCSS_WORK *bmw, BtlvMcssPos pos );
extern  void            BTLV_MCSS_SetMepachiFlag( BTLV_MCSS_WORK *bmw, int position, int flag );
extern  int             BTLV_MCSS_GetAnmStopFlag( BTLV_MCSS_WORK *bmw, int position );
extern  void            BTLV_MCSS_SetAnmStopFlag( BTLV_MCSS_WORK *bmw, int position, int flag );
extern  BTLV_MCSS_VANISH_FLAG BTLV_MCSS_GetVanishFlag( BTLV_MCSS_WORK *bmw, int position );
extern  void            BTLV_MCSS_SetVanishFlag( BTLV_MCSS_WORK *bmw, int position, BTLV_MCSS_VANISH_FLAG flag );
extern  void            BTLV_MCSS_GetPokeDefaultPos( BTLV_MCSS_WORK *bmw, VecFx32 *pos, int position );
extern  void            BTLV_MCSS_GetPokeDefaultPosByRule( BTLV_MCSS_WORK *bmw, VecFx32 *pos,
                                                           BtlvMcssPos position, BtlRule rule );
extern  fx32            BTLV_MCSS_GetPokeDefaultScale( BTLV_MCSS_WORK *bmw, int position );
extern  fx32            BTLV_MCSS_GetPokeDefaultScaleEx( BTLV_MCSS_WORK *bmw, int position, BTLV_MCSS_PROJECTION proj );
extern  void            BTLV_MCSS_GetScale( BTLV_MCSS_WORK *bmw, int position, VecFx32 *scale );
extern  void            BTLV_MCSS_SetScale( BTLV_MCSS_WORK *bmw, int position, VecFx32 *scale );
extern  void            BTLV_MCSS_SetShadowVanishFlag( BTLV_MCSS_WORK *bmw, int position, u8 flag );
extern  void            BTLV_MCSS_SetAnmSpeed( BTLV_MCSS_WORK *bmw, int position, fx32 speed );
extern  void            BTLV_MCSS_MovePosition( BTLV_MCSS_WORK *bmw, int position, int move_type,
                                                VecFx32 *pos, int frame, int wait, int count );
extern  void            BTLV_MCSS_MoveScale( BTLV_MCSS_WORK *bmw, int position, int move_type,
                                             VecFx32 *scale, int frame, int wait, int count );
extern  void            BTLV_MCSS_MoveDefaultScale( BTLV_MCSS_WORK *bmw, int position, int move_type,
                                                    VecFx32 *scale, int frame, int wait, int count );
extern  void            BTLV_MCSS_MoveShadowScale( BTLV_MCSS_WORK *bmw, int position, int type,
                                                   VecFx32 *scale, int frame, int wait, int count );
extern  void            BTLV_MCSS_MoveRotate( BTLV_MCSS_WORK *bmw, int position, int type,
                                              VecFx32 *rotate, int frame, int wait, int count );
extern  void            BTLV_MCSS_MoveBlink( BTLV_MCSS_WORK *bmw, int position, int type, int wait, int count );
extern  void            BTLV_MCSS_MoveAlpha( BTLV_MCSS_WORK *bmw, int position, int type,
                                             int alpha, int frame, int wait, int count );
extern  void            BTLV_MCSS_MoveCircle( BTLV_MCSS_WORK *bmw, BTLV_MCSS_MOVE_CIRCLE_PARAM* bmmcp );
extern  void            BTLV_MCSS_MoveSin( BTLV_MCSS_WORK *bmw, BTLV_MCSS_MOVE_SIN_PARAM* bmmsp );
extern  void            BTLV_MCSS_MoveMosaic( BTLV_MCSS_WORK *bmw, int position, int type,
                                              int mosaic, int frame, int wait, int count );
extern  BOOL            BTLV_MCSS_CheckTCBExecute( BTLV_MCSS_WORK *bmw, int position );
extern  BOOL            BTLV_MCSS_CheckTCBExecuteAllPos( BTLV_MCSS_WORK *bmw );
extern  BOOL            BTLV_MCSS_CheckExist( BTLV_MCSS_WORK *bmw, int position );
extern  void            BTLV_MCSS_SetPaletteFade( BTLV_MCSS_WORK *bmw, int position,
                                                  u8 start_evy, u8 end_evy, u8 wait, u32 rgb );
extern  void            BTLV_MCSS_SetPaletteFadeBaseColor( BTLV_MCSS_WORK *bmw, int position, u8 evy, u32 rgb );
extern  void            BTLV_MCSS_ResetPaletteFadeBaseColor( BTLV_MCSS_WORK *bmw, int position );
extern  int             BTLV_MCSS_GetMonsNo( BTLV_MCSS_WORK *bmw, int position );
extern  void            BTLV_MCSS_SetHengeParam( BTLV_MCSS_WORK *bmw, int position, const POKEMON_PARAM* pp );
extern  int             BTLV_MCSS_GetFormNo( BTLV_MCSS_WORK *bmw, int position );
extern  u16             BTLV_MCSS_GetWeight( BTLV_MCSS_WORK *bmw, int position );
extern  u32             BTLV_MCSS_GetPersonalRnd( BTLV_MCSS_WORK *bmw, int position );
extern  u16             BTLV_MCSS_GetHPColor( BTLV_MCSS_WORK *bmw, int position );
extern  u32             BTLV_MCSS_GetStatusFlag( BTLV_MCSS_WORK *bmw, int position );
extern  BOOL            BTLV_MCSS_GetNoJump( BTLV_MCSS_WORK *bmw, int position );
extern  u8              BTLV_MCSS_GetFlyFlag( BTLV_MCSS_WORK *bmw, int position );
extern  void            BTLV_MCSS_SetMigawari( BTLV_MCSS_WORK *bmw, int position, int sw, BOOL flag );
extern  void            BTLV_MCSS_SetMosaic( BTLV_MCSS_WORK *bmw, int position, int mosaic );
extern  void            BTLV_MCSS_CopyMAW( BTLV_MCSS_WORK *bmw, int src, int dst );
extern  void            BTLV_MCSS_OverwriteMAW( BTLV_MCSS_WORK *bmw, BtlvMcssPos pos, MCSS_ADD_WORK* maw );
extern  u32             BTLV_MCSS_PlayVoice( BTLV_MCSS_WORK *bmw, int position, int pitch, int volume,
                                             int chorus_vol, int chorus_speed, BOOL play_dir );
extern  void            BTLV_MCSS_SetRotation( BTLV_MCSS_WORK* bmw, int side, int dir );
extern  void            BTLV_MCSS_SetSideChange( BTLV_MCSS_WORK* bmw, BtlvMcssPos pos1, BtlvMcssPos pos2 );
extern  int             BTLV_MCSS_GetCaptureBall( BTLV_MCSS_WORK *bmw, int position );
extern  u32             BTLV_MCSS_GetCells( BTLV_MCSS_WORK *bmw, int position );

extern  void            BTLV_MCSS_MakeMAW( BTLV_MCSS_WORK* bmw, const POKEMON_PARAM *pp, MCSS_ADD_WORK *maw, int position );

extern  BOOL            BTLV_MCSS_SetAnime( BTLV_MCSS_WORK* bmw, int position, int anm_no );
extern  void            BTLV_MCSS_SetAnimeEndCheck( BTLV_MCSS_WORK* bmw, int position );
extern  BOOL            BTLV_MCSS_CheckAnimeExecute( BTLV_MCSS_WORK* bmw, int position );
extern  void            BTLV_MCSS_CheckPositionSetInitPos( BTLV_MCSS_WORK* bmw, int position );
extern  void            BTLV_MCSS_SetReverseDrawFlag( BTLV_MCSS_WORK* bmw, BtlvMcssPos position,
                                                      BTLV_MCSS_REVERSE_DRAW draw_flag );
extern  BOOL            BTLV_MCSS_CheckHengeFlag( BTLV_MCSS_WORK* bmw, BtlvMcssPos position );
extern  void            BTLV_MCSS_PushVanishFlag( BTLV_MCSS_WORK* bmw );
extern  void            BTLV_MCSS_PopVanishFlag( BTLV_MCSS_WORK* bmw );

#ifdef PM_DEBUG
extern  void            BTLV_MCSS_AddDebug( BTLV_MCSS_WORK *bmw, const MCSS_ADD_DEBUG_WORK *madw, int position );
extern  void            BTLV_MCSS_SetMcss3vs3( BTLV_MCSS_WORK *bmw, int flag );
extern  void            BTLV_MCSS_SetMcssRotate( BTLV_MCSS_WORK *bmw, int flag );
extern  void            BTLV_MCSS_SetAnm1LoopFlag( BTLV_MCSS_WORK *bmw, int flag );
extern  void            BTLV_MCSS_ClearExecute( BTLV_MCSS_WORK *bmw );
extern  MCSS_SYS_WORK*  BTLV_MCSS_GetMcssSysWork( BTLV_MCSS_WORK *bmw );
#endif

