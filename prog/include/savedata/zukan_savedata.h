//============================================================================================
/**
 * @file  zukan_savedata.h
 * @brief �}�ӃZ�[�u�f�[�^�A�N�Z�X�����Q�w�b�_
 * @author  mori / tamada GAME FREAK inc.
 * @date  2009.10.26
 */
//============================================================================================
#ifndef __ZUKAN_SAVEDATA_H__
#define __ZUKAN_SAVEDATA_H__

#include "savedata/save_control.h"

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

  // ZukanSave_GetPokeSexFlag�n�֐��̃G���[�R�[�h
  ZUKANSAVE_GET_SEX_ERR = 0xffffffff,


  // �S���}�Ӎő�
  ZUKANSAVE_ZENKOKU_MONSMAX   = MONSNO_END,

  // �W���E�g�}�Ӎő吔
  ZUKANSAVE_JOHTO_MONSMAX = ISSHU_MAX,

  // �S���}�ӂ͕߂܂��������Ȃ̂Ń}�i�t�B�[�̃`�F�b�N�͕K�v�ɂȂ�(08.06.20)
  // �[���R�N�}�ӊ����ɕK�v�ȃ|�P�����̐�
  ZUKANSAVE_ZENKOKU_COMP_NOPOKE_NUM = 9,  // �s�K�v�ȃ|�P������
  ZUKANSAVE_ZENKOKU_COMP_NUM  = ZUKANSAVE_ZENKOKU_MONSMAX - ZUKANSAVE_ZENKOKU_COMP_NOPOKE_NUM,

  // �W���E�g�}�ӊ����ɕK�v�ȃ|�P�����̐�
  ZUKANSAVE_JOHTO_COMP_NOPOKE_NUM = 2,  // �s�K�v�ȃ|�P������
  ZUKANSAVE_JOHTO_COMP_NUM  = ZUKANSAVE_JOHTO_MONSMAX - ZUKANSAVE_JOHTO_COMP_NOPOKE_NUM,



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
//  �Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
extern ZUKAN_SAVEDATA * SaveData_GetZukanSave( SAVE_CONTROL_WORK * sv);


//----------------------------------------------------------
//  �Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int ZukanSave_GetWorkSize(void);
extern ZUKAN_SAVEDATA * ZukanSave_AllocWork( HEAPID heapID );


//----------------------------------------------------------
//----------------------------------------------------------
extern void ZukanSave_Init(ZUKAN_SAVEDATA * zs);

extern u16 ZukanSave_GetPokeGetCount(const ZUKAN_SAVEDATA * zs);
extern u16 ZukanSave_GetPokeSeeCount(const ZUKAN_SAVEDATA * zs);

extern u16 ZukanSave_GetJohtoPokeGetCount(const ZUKAN_SAVEDATA * zs);
extern u16 ZukanSave_GetJohtoPokeSeeCount(const ZUKAN_SAVEDATA * zs);

// �|�P���������o�^�E�|�P�����߂܂����o�^
extern void ZukanSave_SetPokeSee(ZUKAN_SAVEDATA * zs, const POKEMON_PARAM * pp);
extern void ZukanSave_SetPokeGet(ZUKAN_SAVEDATA * zs, const POKEMON_PARAM * pp);

// �[���R�N�}�ӎ����Ă���Ƃ��[���S���p�̊e������Ԃ�
// �W���E�g�}�ӂ��������Ă��Ȃ��Ƃ��[���W���E�g�}�ӗp�̊e������Ԃ�
extern u16 ZukanSave_GetZukanPokeGetCount(const ZUKAN_SAVEDATA * zs);
extern u16 ZukanSave_GetZukanPokeSeeCount(const ZUKAN_SAVEDATA * zs);


// �[���R�N�}�Ӂ@�W���E�g�}��
// �����ɕK�v�ȃ|�P���������ŃJ�E���g�����l���擾
extern u16 ZukanSave_GetZenkokuGetCompCount(const ZUKAN_SAVEDATA * zs);
extern u16 ZukanSave_GetJohtoGetCompCount(const ZUKAN_SAVEDATA * zs);

extern BOOL ZukanSave_CheckZenkokuComp(const ZUKAN_SAVEDATA * zs);
extern BOOL ZukanSave_CheckJohtoComp(const ZUKAN_SAVEDATA * zs);

extern BOOL ZukanSave_GetPokeGetFlag(const ZUKAN_SAVEDATA * zs, u16 monsno);
extern BOOL ZukanSave_GetPokeSeeFlag(const ZUKAN_SAVEDATA * zs, u16 monsno);
extern u32 ZukanSave_GetPokeSexFlag(const ZUKAN_SAVEDATA * zw, u16 monsno, int first_second, HEAPID heapId );
extern u32 ZukanSave_GetPokeAnnoonForm(const ZUKAN_SAVEDATA * zs, int count, BOOL get_f );
extern u32 ZukanSave_GetPokeAnnoonNum(const ZUKAN_SAVEDATA * zs, BOOL get_f);
extern u32 ZukanSave_GetPokeSiiusiForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZukanSave_GetPokeSiiusiSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZukanSave_GetPokeSiidorugoForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZukanSave_GetPokeSiidorugoSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZukanSave_GetPokeMinomuttiForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZukanSave_GetPokeMinomuttiSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZukanSave_GetPokeMinomesuForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZukanSave_GetPokeMinomesuSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZukanSave_GetPokePityuuForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZukanSave_GetPokePityuuSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZukanSave_GetPokeRandomFlag(const ZUKAN_SAVEDATA * zs, u8 random_poke);
extern u32 ZukanSave_GetPokeDeokisisuForm(const ZUKAN_SAVEDATA * zs, int count);
extern u32 ZukanSave_GetPokeDeokisisuFormSeeNum(const ZUKAN_SAVEDATA * zs);
extern u32 ZukanSave_GetPokeForm( const ZUKAN_SAVEDATA* zs, int monsno, int count );
extern u32 ZukanSave_GetPokeFormNum( const ZUKAN_SAVEDATA* zs, int monsno );

extern BOOL ZukanSave_GetZukanGetFlag(const ZUKAN_SAVEDATA * zs);
extern void ZukanSave_SetZukanGetFlag(ZUKAN_SAVEDATA * zs);

extern BOOL ZukanSave_GetZenkokuZukanFlag(const ZUKAN_SAVEDATA * zs);
extern void ZukanSave_SetZenkokuZukanFlag(ZUKAN_SAVEDATA * zs);

extern BOOL ZukanSave_GetGraphicVersionUpFlag(const ZUKAN_SAVEDATA * zs);
extern void ZukanSave_SetGraphicVersionUpFlag(ZUKAN_SAVEDATA * zs);

extern BOOL ZukanSave_GetTextVersionUpFlag(const ZUKAN_SAVEDATA * zs, u16 monsno, u32 country_code);

extern void ZukanSave_SetTextVersionUpMasterFlag( ZUKAN_SAVEDATA * zs );
extern BOOL ZukanSave_GetTextVersionUpMasterFlag(const ZUKAN_SAVEDATA * zs);

extern void ZukanSave_Copy(const ZUKAN_SAVEDATA * from, ZUKAN_SAVEDATA * to);


//----------------------------------------------------------
//  �f�o�b�O�p
//----------------------------------------------------------
extern void Debug_ZukanSave_Make(ZUKAN_SAVEDATA * zs, int start, int end, BOOL see_flg, HEAPID heapId);
extern void Debug_ZukanSave_Make_PM_LANG(ZUKAN_SAVEDATA * zs, int start, int end, BOOL see_flg, HEAPID heapId);
extern void Debug_ZukanSave_LangSetRand( ZUKAN_SAVEDATA * zs, int start, int end );
extern void Debug_ZukanSave_LangSetAll( ZUKAN_SAVEDATA * zs, int start, int end );
extern void Debug_ZukanSave_LangSet( ZUKAN_SAVEDATA * zs, int start, int end, u8 lang );
extern void Debug_ZukanSave_AnnoonGetSet( ZUKAN_SAVEDATA * zs, int start, int end, HEAPID heapId );
extern void Debug_ZukanSave_DeokisisuBuckUp( ZUKAN_SAVEDATA * zs );

#ifdef CREATE_INDEX
extern void *Index_Get_Zukan_Offset(ZUKAN_SAVEDATA *zs);
extern void *Index_Get_Zenkoku_Zukan_Offset(ZUKAN_SAVEDATA *zs);
extern void *Index_Get_Get_Flag_Offset(ZUKAN_SAVEDATA *zs);
extern void *Index_Get_See_Flag_Offset(ZUKAN_SAVEDATA *zs);
extern void *Index_Get_Sex_Flag_Offset(ZUKAN_SAVEDATA *zs);
#endif

#endif  // __ZUKAN_SAVEADATA_H__
