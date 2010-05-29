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
#include "buflen.h"

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief ������ԃf�[�^�^��`
 */
//----------------------------------------------------------
typedef struct _MYSTATUS MYSTATUS;

//----------------------------------------------------------
/**
 * @brief ������ԃf�[�^�^��`
 */
//----------------------------------------------------------
struct _MYSTATUS {
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];    // 16
  u32 id;               ///< 20 PlayerID
  u32 profileID;        ///< 24 GameSyncID
  u8 nation;            ///< ��  25
  u8 area;              ///< �n�� 26
  u8 region_code;       ///< PM_LANG   27
  u8 rom_code;          ///< PM_VERSION   28
  u8 trainer_view;      ///< ���j�I�����[�����ł̌����ڃt���O29
  u8 sex;               ///< ���� 30
  u8 dummy2;            // 31
  u8 dummy3;            // 32
};


#define MYSTATUS_SAVE_SIZE (32)

#define POKEMON_DP_ROM_CODE ( 0 )   ///< PokemonDP �� �����Ă��� RomCode

#define MYSTATUS_UNIONVIEW_MAX    ( 8 )   ///< ���j�I�������ڍő吔�i�������j���W���Ȃ̂łP�U����j
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

//�� �n��
extern int MyStatus_GetMyNation(const MYSTATUS * my);
extern int MyStatus_GetMyArea(const MYSTATUS * my);

// ���̊֐��������ƁA�n���V�ɓo�^����܂���B WIFIHISTORY_SetMyNationArea���g���Ă�������
extern void MyStatus_SetMyNationArea(MYSTATUS * my, int nation, int area);


// ���j�I�����[���Ŏg�������̌�����
extern u8 MyStatus_GetTrainerView( const MYSTATUS *my );
extern void MyStatus_SetTrainerView( MYSTATUS *my, u8 view );

// ROM�R�[�h = PM_VERSION�������l
extern u8 MyStatus_GetRomCode( const MYSTATUS * my );
extern void MyStatus_SetRomCode( MYSTATUS * my, u8 rom_code );

// ���[�W�����R�[�h  //PM_LANG�������l
extern u8  MyStatus_GetRegionCode( const MYSTATUS * my );
extern void  MyStatus_SetRegionCode( MYSTATUS * my, u8 region_code );

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
