//======================================================================
/**
 * @file	plist_plate.c
 * @brief	�|�P�������X�g �v���[�g
 * @author	ariizumi
 * @data	09/06/10
 *
 * ���W���[�����FPLIST_PLATE
 */
//======================================================================
#pragma once

#include "plist_local_def.h"
#include "poke_tool\poke_tool.h"

//�v���[�g�̐F�̎��(���̂܂܃p���b�g�ԍ��ɑΉ�
typedef enum
{
  PPC_NORMAL        = 3,  //�ʏ�
  PPC_NORMAL_SELECT = 7,  //�ʏ�(�I��
  PPC_CHANGE        = 6,  //����ւ�

  //�ȉ��͎����ݒ�
  PPC_DEATH        = 5,  //���S
  PPC_DEATH_SELECT = 10,  //���S(�I��
}PLIST_PLATE_COLTYPE;

//�퓬�Q�����̏��
typedef enum
{
  PPBO_JOIN_1 = 0,  //�����΂��(�ȉ���
  PPBO_JOIN_2,
  PPBO_JOIN_3,
  PPBO_JOIN_4,
  PPBO_JOIN_5,
  PPBO_JOIN_6,
  
  PPBO_JOIN_OK, //�Q���ł���
  PPBO_JOIN_NG, //�Q���ł��Ȃ�
  
  PPBO_INVALLID,  //�o�g���ȊO

}PLIST_PLATE_BATTLE_ORDER;


extern PLIST_PLATE_WORK* PLIST_PLATE_CreatePlate( PLIST_WORK *work , const u8 idx , POKEMON_PARAM *pp );
extern PLIST_PLATE_WORK* PLIST_PLATE_CreatePlate_Blank( PLIST_WORK *work , const u8 idx );

extern void PLIST_PLATE_DeletePlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );
extern void PLIST_PLATE_UpdatePlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );

extern void PLIST_PLATE_SetActivePlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , const BOOL isActive );

extern void PLIST_PLATE_ChangeColor( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , PLIST_PLATE_COLTYPE col );

//����ւ��A�j���Ńv���[�g���ړ�������(�L�����P��
extern void PLIST_PLATE_MovePlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , const u8 moveValue );
extern void PLIST_PLATE_MovePlateXY( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , const s8 subX , const s8 subY );
extern void PLIST_PLATE_ClearPlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , const u8 moveValue );
extern void PLIST_PLATE_ResetParam( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , POKEMON_PARAM *pp , const u8 moveValue );

extern const BOOL PLIST_PLATE_CanSelect( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );
//�v���[�g�̈ʒu�擾(clact�p
extern void PLIST_PLATE_GetPlatePosition( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , GFL_CLACTPOS *pos );
//�v���[�g�̈ʒu�擾(�^�b�`����p
extern void PLIST_PLATE_GetPlateRect( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , GFL_UI_TP_HITTBL *hitTbl );
//�o�g���Q���p��Ԏ擾�E�ݒ�
extern const PLIST_PLATE_BATTLE_ORDER PLIST_PLATE_GetBattleOrder( const PLIST_PLATE_WORK *plateWork );
extern void PLIST_PLATE_SetBattleOrder( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , const PLIST_PLATE_BATTLE_ORDER order );
