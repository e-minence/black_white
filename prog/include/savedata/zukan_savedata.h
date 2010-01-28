//============================================================================================
/**
 * @file  zukan_savedata.h
 * @brief �}�ӃZ�[�u�f�[�^�A�N�Z�X�����Q�w�b�_
 * @author  mori / tamada GAME FREAK inc.
 * @date  2009.10.26
 *
 * �W���E�g���C�b�V�����H�Ƃ��̓x�ɖ��O������������̂��Ȃ�Ȃ̂ŁA
 * ���ꂩ��͑S���́uZENKOKU�v�A�n���}�ӂ̎��́uLOCAL�v�ƋL�q���鎖�ɂ��܂��B
 *
 */
//============================================================================================
#ifndef __ZUKAN_SAVEDATA_H__
#define __ZUKAN_SAVEDATA_H__

#include "gamesystem/gamedata_def.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#define ISSHU_MAX ( 156 )     // @TODO ISSHU_MAX��`�� monsno_def.h�Ő錾���ꂽ�����

//============================================================================================
//============================================================================================
enum{
  ZUKANSAVE_RANDOM_PACHI = 0,   // �p�b�`�[��������

  // �|�P�������ʎ擾�p
  ZUKANSAVE_SEX_FIRST = 0,
  ZUKANSAVE_SEX_SECOND,

  // ZUKANSAVE_GetPokeSexFlag�n�֐��̃G���[�R�[�h
  ZUKANSAVE_GET_SEX_ERR = 0xffffffff,


  // �S���}�Ӎő�
  ZUKANSAVE_ZENKOKU_MONSMAX   = MONSNO_END,

  // �C�b�V���}�Ӎő吔
  ZUKANSAVE_LOCAL_MONSMAX = ISSHU_MAX,

  // �S���}�ӂ͕߂܂��������Ȃ̂Ń}�i�t�B�[�̃`�F�b�N�͕K�v�ɂȂ�(08.06.20)
  // �[���R�N�}�ӊ����ɕK�v�ȃ|�P�����̐�
  ZUKANSAVE_ZENKOKU_COMP_NOPOKE_NUM = 9,  // �s�K�v�ȃ|�P������
  ZUKANSAVE_ZENKOKU_COMP_NUM  = ZUKANSAVE_ZENKOKU_MONSMAX - ZUKANSAVE_ZENKOKU_COMP_NOPOKE_NUM,

  // �C�b�V���}�ӊ����ɕK�v�ȃ|�P�����̐�
  ZUKANSAVE_LOCAL_COMP_NOPOKE_NUM = 2,  // �s�K�v�ȃ|�P������
  ZUKANSAVE_LOCAL_COMP_NUM  = ZUKANSAVE_LOCAL_MONSMAX - ZUKANSAVE_LOCAL_COMP_NOPOKE_NUM,



  // �V�[�E�V�@�V�[�h���S �V�F�C�~�@�M���e�B�i�̌`��
  ZUKANSAVE_UMIUSHI_FORM_NUM = 2, //(������ς��������ł̓Z�[�u�̈�̕����⃏�[�N�T�C�Y�͕ς��Ȃ��̂ŁA�`�󐔂��ς�����Ƃ��͓������ύX���邱��)

  // �~�m���b�`�@�~�m���X�@�s�`���[�@�`��
  ZUKANSAVE_MINOMUSHI_FORM_NUM = 3, //(������ς��������ł̓Z�[�u�̈�̕����⃏�[�N�T�C�Y�͕ς��Ȃ��̂ŁA�`�󐔂��ς�����Ƃ��͓������ύX���邱��)

  // �f�I�L�V�X�̌`��
  ZUKANSAVE_DEOKISISU_FORM_NUM = 4,
  ZUKANSAVE_DEOKISISU_FORM_SAVEDATA_CUTNUM = 2, // �f�I�L�V�X�Z�[�u�̈�́A�}篁@�ߊl�����t���O�̂���1byte���ÂɊ���U���Ă��܂��B�e1byte�ɃZ�[�u�ł���`��̐��ł�
  POKEZUKAN_DEOKISISU_INIT = 0xf, // ���������i�[����Ă���l

  // ���g���̌`��
  ZUKANSAVE_ROTOM_FORM_NUM = 6, // (������ς��������ł̓Z�[�u�̈�̕����⃏�[�N�T�C�Y�͕ς��Ȃ��̂ŁA�`�󐔂��ς�����Ƃ��͓������ύX���邱��)

  // �|�������̌`�󐔁i�Z�[�u�f�[�^�ɂ͊֌W����܂���j
  ZUKANSAVE_POWARUN_FORM_NUM = 4,

  // �`�F�����̌`�󐔁i�Z�[�u�f�[�^�ɂ͊֌W����܂���j
  ZUKANSAVE_CHERIMU_FORM_NUM = 2,
};

//----------------------------------------------------------
// ����J�^ �\���̐錾
//----------------------------------------------------------
typedef struct ZUKAN_SAVEDATA ZUKAN_SAVEDATA;

//----------------------------------------------------------
//  �Z�[�u�f�[�^�擾�̂��߂̊֐��iGAMEDATA�o�R�j
//----------------------------------------------------------
extern ZUKAN_SAVEDATA * GAMEDATA_GetZukanSave( GAMEDATA * gamedata );


//----------------------------------------------------------
//  �Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int ZUKANSAVE_GetWorkSize(void);
extern ZUKAN_SAVEDATA * ZUKANSAVE_AllocWork( HEAPID heapID );


//----------------------------------------------------------
//----------------------------------------------------------
extern void ZUKANSAVE_Init(ZUKAN_SAVEDATA * zs);

extern u16 ZUKANSAVE_GetPokeGetCount(const ZUKAN_SAVEDATA * zs);
extern u16 ZUKANSAVE_GetPokeSeeCount(const ZUKAN_SAVEDATA * zs);

extern u16 ZUKANSAVE_GetLocalPokeGetCount(const ZUKAN_SAVEDATA * zs);
extern u16 ZUKANSAVE_GetLocalPokeSeeCount(const ZUKAN_SAVEDATA * zs);

// �|�P���������o�^�E�|�P�����߂܂����o�^
extern void ZUKANSAVE_SetPokeSee(ZUKAN_SAVEDATA * zs, const POKEMON_PARAM * pp);
extern void ZUKANSAVE_SetPokeGet(ZUKAN_SAVEDATA * zs, const POKEMON_PARAM * pp);

// �[���R�N�}�ӎ����Ă���Ƃ��[���S���p�̊e������Ԃ�
// �C�b�V���}�ӂ��������Ă��Ȃ��Ƃ��[���C�b�V���}�ӗp�̊e������Ԃ�
extern u16 ZUKANSAVE_GetZukanPokeGetCount(const ZUKAN_SAVEDATA * zs);
extern u16 ZUKANSAVE_GetZukanPokeSeeCount(const ZUKAN_SAVEDATA * zs);


// �[���R�N�}�Ӂ@�C�b�V���}��
// �����ɕK�v�ȃ|�P���������ŃJ�E���g�����l���擾
extern u16 ZUKANSAVE_GetZenkokuGetCompCount(const ZUKAN_SAVEDATA * zs);
extern u16 ZUKANSAVE_GetLocalGetCompCount(const ZUKAN_SAVEDATA * zs);

extern BOOL ZUKANSAVE_CheckZenkokuComp(const ZUKAN_SAVEDATA * zs);
extern BOOL ZUKANSAVE_CheckLocalComp(const ZUKAN_SAVEDATA * zs);

extern BOOL ZUKANSAVE_GetPokeGetFlag(const ZUKAN_SAVEDATA * zs, u16 monsno);
extern BOOL ZUKANSAVE_GetPokeSeeFlag(const ZUKAN_SAVEDATA * zs, u16 monsno);
extern u32 ZUKANSAVE_GetPokeSexFlag(const ZUKAN_SAVEDATA * zw, u16 monsno, int first_second, HEAPID heapId );
extern u32 ZUKANSAVE_GetPokeAnnoonForm(const ZUKAN_SAVEDATA * zs, int count, BOOL get_f );
extern u32 ZUKANSAVE_GetPokeAnnoonNum(const ZUKAN_SAVEDATA * zs, BOOL get_f);
extern u32 ZUKANSAVE_GetPokeSiiusiForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZUKANSAVE_GetPokeSiiusiSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZUKANSAVE_GetPokeSiidorugoForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZUKANSAVE_GetPokeSiidorugoSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZUKANSAVE_GetPokeMinomuttiForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZUKANSAVE_GetPokeMinomuttiSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZUKANSAVE_GetPokeMinomesuForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZUKANSAVE_GetPokeMinomesuSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZUKANSAVE_GetPokePityuuForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZUKANSAVE_GetPokePityuuSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZUKANSAVE_GetPokeRandomFlag(const ZUKAN_SAVEDATA * zs, u8 random_poke);
extern u32 ZUKANSAVE_GetPokeDeokisisuForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZUKANSAVE_GetPokeDeokisisuFormSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZUKANSAVE_GetPokeForm( const ZUKAN_SAVEDATA* zs, int monsno, int count );
extern u32 ZUKANSAVE_GetPokeFormNum( const ZUKAN_SAVEDATA* zs, int monsno );

extern BOOL ZUKANSAVE_GetZukanGetFlag(const ZUKAN_SAVEDATA * zs);
extern void ZUKANSAVE_SetZukanGetFlag(ZUKAN_SAVEDATA * zs);

extern BOOL ZUKANSAVE_GetZenkokuZukanFlag(const ZUKAN_SAVEDATA * zs);
extern void ZUKANSAVE_SetZenkokuZukanFlag(ZUKAN_SAVEDATA * zs);

extern BOOL ZUKANSAVE_GetGraphicVersionUpFlag(const ZUKAN_SAVEDATA * zs);
extern void ZUKANSAVE_SetGraphicVersionUpFlag(ZUKAN_SAVEDATA * zs);

extern BOOL ZUKANSAVE_GetTextVersionUpFlag(const ZUKAN_SAVEDATA * zs, u16 monsno, u32 country_code);

extern void ZUKANSAVE_SetTextVersionUpMasterFlag( ZUKAN_SAVEDATA * zs );
extern BOOL ZUKANSAVE_GetTextVersionUpMasterFlag(const ZUKAN_SAVEDATA * zs);

extern void ZUKANSAVE_Copy(const ZUKAN_SAVEDATA * from, ZUKAN_SAVEDATA * to);


//----------------------------------------------------------
//  �f�o�b�O�p
//----------------------------------------------------------
extern void Debug_ZUKANSAVE_Make(ZUKAN_SAVEDATA * zs, int start, int end, BOOL see_flg, HEAPID heapId);
extern void Debug_ZUKANSAVE_Make_PM_LANG(ZUKAN_SAVEDATA * zs, int start, int end, BOOL see_flg, HEAPID heapId);
extern void Debug_ZUKANSAVE_LangSetRand( ZUKAN_SAVEDATA * zs, int start, int end );
extern void Debug_ZUKANSAVE_LangSetAll( ZUKAN_SAVEDATA * zs, int start, int end );
extern void Debug_ZUKANSAVE_LangSet( ZUKAN_SAVEDATA * zs, int start, int end, u8 lang );
extern void Debug_ZUKANSAVE_AnnoonGetSet( ZUKAN_SAVEDATA * zs, int start, int end, HEAPID heapId );
extern void Debug_ZUKANSAVE_DeokisisuBuckUp( ZUKAN_SAVEDATA * zs );

#ifdef CREATE_INDEX
extern void *Index_Get_Zukan_Offset(ZUKAN_SAVEDATA *zs);
extern void *Index_Get_Zenkoku_Zukan_Offset(ZUKAN_SAVEDATA *zs);
extern void *Index_Get_Get_Flag_Offset(ZUKAN_SAVEDATA *zs);
extern void *Index_Get_See_Flag_Offset(ZUKAN_SAVEDATA *zs);
extern void *Index_Get_Sex_Flag_Offset(ZUKAN_SAVEDATA *zs);
#endif

#endif  // __ZUKAN_SAVEADATA_H__
