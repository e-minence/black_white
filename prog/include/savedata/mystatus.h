//============================================================================================
/**
 * @file  mystatus.h
 * @brief ������ԃf�[�^�A�N�Z�X�p�w�b�_
 * @author  tamada GAME FREAK inc.
 * @date  2005.10.27
 */
//============================================================================================
#ifndef __MYSTATUS_H__
#define __MYSTATUS_H__

#include <strbuf.h>

#include "savedata/save_control.h"  //SAVE_CONTROL_WORK�Q�Ƃ̂���


//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief ������ԃf�[�^�^��`
 */
//----------------------------------------------------------
typedef struct _MYSTATUS MYSTATUS;

#define POKEMON_DP_ROM_CODE ( 0 )   ///< PokemonDP �� �����Ă��� RomCode

//============================================================================================
//============================================================================================
//----------------------------------------------------------
//  �Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int MyStatus_GetWorkSize(void);
extern MYSTATUS * MyStatus_AllocWork(u32 heapID);
extern void MyStatus_Copy(const MYSTATUS * from, MYSTATUS * to);

//----------------------------------------------------------
//  MYSTATUS����̂��߂̊֐�
//----------------------------------------------------------
extern void MyStatus_Init(MYSTATUS * my);

// ���O���O�Ŗ��߂��Ă肤���`�F�b�N
// (�f�[�^�������Ă��Ȃ���Ԃ����`�F�b�N����)
extern BOOL MyStatus_CheckNameClear( const MYSTATUS * my );

//���O
extern void MyStatus_SetMyName(MYSTATUS * my, const STRCODE * name);
extern const STRCODE * MyStatus_GetMyName(const MYSTATUS * my);
extern void MyStatus_CopyNameStrCode( const MYSTATUS * my, STRCODE *buf, int dest_length );
extern STRBUF * MyStatus_CreateNameString(const MYSTATUS * my, int heapID);
extern void MyStatus_CopyNameString( const MYSTATUS * my, STRBUF *buf );
extern void MyStatus_SetMyNameFromString(MYSTATUS * my, const STRBUF * str);

//ID
extern void MyStatus_SetID(MYSTATUS * my, u32 id);
extern u32 MyStatus_GetID(const MYSTATUS * my);
extern u16 MyStatus_GetID_Low(const MYSTATUS * my);

//ProfileID
extern void MyStatus_SetProfileID(MYSTATUS * my, s32 id);
extern s32 MyStatus_GetProfileID(const MYSTATUS * my);

//����
extern void MyStatus_SetMySex(MYSTATUS * my, int sex);
extern u32 MyStatus_GetMySex(const MYSTATUS * my);

//�o�b�W  @todo �폜  MISC�Ɉړ�
//extern BOOL MyStatus_GetBadgeFlag(const MYSTATUS * my, int badge_id);
//extern void MyStatus_SetBadgeFlag(MYSTATUS * my, int badge_id);
//extern int MyStatus_GetBadgeCount(const MYSTATUS * my);

//����  @todo �폜  MISC�Ɉړ�
//extern u32 MyStatus_GetGold(const MYSTATUS * my);
//extern u32 MyStatus_SetGold(MYSTATUS * my, u32 gold);
//extern u32 Mystatus_AddGold(MYSTATUS * my, u32 add);
//extern u32 Mystatus_SubGold(MYSTATUS * my, u32 sub);

// ���j�I�����[���Ŏg�������̌�����
extern u8 MyStatus_GetTrainerView( const MYSTATUS *my );
extern void MyStatus_SetTrainerView( MYSTATUS *my, u8 view );

// ROM�R�[�h = PM_VERSION�������l
extern u8 MyStatus_GetRomCode( const MYSTATUS * my );
extern void MyStatus_SetRomCode( MYSTATUS * my, u8 rom_code );
extern u8 PokemonDP_GetRomCode( void );

// ���[�W�����R�[�h  //PM_LANG�������l
extern u8  MyStatus_GetRegionCode( const MYSTATUS * my );
extern void  MyStatus_SetRegionCode( MYSTATUS * my, u8 region_code );

// �Q�[���N���A  @todo�폜�\��
//void MyStatus_SetDpClearFlag( MYSTATUS * my );
//int MyStatus_GetDpClearFlag( MYSTATUS * my );

// �S���}�� @todo�폜�\��
//void MyStatus_SetDpZenkokuFlag( MYSTATUS * my );
//int MyStatus_GetDpZenkokuFlag( MYSTATUS * my );

// �O���Z�[�u�f�[�^�������ς݃t���O
extern void MyStatus_SetExtraInitFlag(MYSTATUS * my);
extern u32 MyStatus_GetExtraInitFlag(const MYSTATUS * my);


//----------------------------------------------------------
//  �Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
extern MYSTATUS * SaveData_GetMyStatus(SAVE_CONTROL_WORK * sv);


//  myStatus���m���������̂��ǂ����𒲂ׂ�
BOOL MyStatus_Compare(const MYSTATUS * my, const MYSTATUS * target);

//----------------------------------------------------------
//  �f�o�b�O�p�f�[�^�����̂��߂̊֐�
//----------------------------------------------------------
#ifdef PM_DEBUG
extern void DEBUG_MyStatus_DummyNameSet(MYSTATUS *mystatus, HEAPID heap_id);
#endif  //PM_DEBUG

#ifdef CREATE_INDEX
extern void *Index_Get_Mystatus_Name_Offset(MYSTATUS *my);
extern void *Index_Get_Mystatus_Id_Offset(MYSTATUS *my);
extern void *Index_Get_Mystatus_Sex_Offset(MYSTATUS *my);
#endif

#endif //__MYSTATUS_H__
