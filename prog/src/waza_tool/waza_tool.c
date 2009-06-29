//============================================================================================
/**
 * @file  waza_tool.c
 * @bfief �Z�p�����[�^�c�[���Q�iWB�����Łj
 * @author  HisashiSogabe
 * @date  08.11.13
 */
//============================================================================================
#include  <gflib.h>

#include  "arc_def.h"

#include  "waza_tool/waza_tool.h"
#include  "waza_tool/wazano_def.h"
#include  "waza_tool/wazadata.h"

#include  "waza_tool_def.h"

/*
typedef struct{
  u16 monsno;
  s16 hp;

  u16 hpmax;
  u8  eqp;
  u8  atc;

  u32 condition;

  u8  speabino;
  u8  sex;
  u8  type1;
  u8  type2;

}WAZA_DAMAGE_CALC_PARAM;
*/

//============================================================================================
/**
 * �v���g�^�C�v�錾
 */
//============================================================================================
void    WT_WazaTableDataGet( void *buffer );
ARCHANDLE *WT_WazaDataParaHandleOpen( HEAPID heapID );
void    WT_WazaDataParaHandleClose( ARCHANDLE *handle );
u32     WT_WazaDataParaGet( int waza_no, int id );
u32     WT_WazaDataParaGetHandle( ARCHANDLE *handle, int waza_no, int id );
u8      WT_PPMaxGet( u16 wazano, u8 maxupcnt );
u32     WT_WazaDataParaGetAct( WAZA_TABLE_DATA *wtd, int id );

static  void  WT_WazaDataGet( int waza_no, WAZA_TABLE_DATA *wtd );

//============================================================================================
/**
 *  �Z�f�[�^�e�[�u���f�[�^��Alloc�����������ɓW�J
 *
 * @param[in] heapID  �f�[�^��W�J���郁�����q�[�vID
 */
//============================================================================================
void  WT_WazaTableDataGet( void *buffer )
{
  GFL_ARC_LoadDataOfs( buffer, ARCID_WAZA_TBL, 0, 0, sizeof( WAZA_TABLE_DATA ) * ( WAZANO_MAX + 1 ) );
}

//============================================================================================
/**
 *  �Z�f�[�^�e�[�u���t�@�C���n���h���I�[�v��
 *
 * @param[in] heapID  �n���h����Alloc����q�[�vID
 */
//============================================================================================
ARCHANDLE *WT_WazaDataParaHandleOpen( HEAPID heapID )
{
  return GFL_ARC_OpenDataHandle( ARCID_WAZA_TBL, heapID );
}

//============================================================================================
/**
 *  �Z�f�[�^�e�[�u���t�@�C���n���h���N���[�Y
 *
 * @param[in] handle  �N���[�Y����Handle
 */
//============================================================================================
void  WT_WazaDataParaHandleClose( ARCHANDLE *handle )
{
  GFL_ARC_CloseDataHandle( handle );
}

//============================================================================================
/**
 *  �Z�f�[�^�e�[�u������l���擾
 *
 * @param[in] waza_no   �擾�������Z�i���o�[
 * @param[in] id      �擾�������l�̎��
 */
//============================================================================================
u32 WT_WazaDataParaGet( int waza_no, int id )
{
  WAZA_TABLE_DATA wtd;

  WT_WazaDataGet( waza_no, &wtd );

  return  WT_WazaDataParaGetAct( &wtd, id );
}

//============================================================================================
/**
 *  �Z�f�[�^�e�[�u������l���擾�i�t�@�C���n���h�����g�p����j
 *
 * @param[in] waza_no   �擾�������Z�i���o�[
 * @param[in] id      �擾�������l�̎��
 */
//============================================================================================
u32 WT_WazaDataParaGetHandle( ARCHANDLE *handle, int waza_no, int id )
{
  WAZA_TABLE_DATA wtd;

  GFL_ARC_LoadDataByHandle( handle, waza_no, &wtd );

  return  WT_WazaDataParaGetAct( &wtd, id );
}

//============================================================================================
/**
 *  PPMax���擾
 *
 * @param[in] wazano    �擾����Z�i���o�[
 * @param[in] maxupcnt  �}�b�N�X�A�b�v���g�p������
 *
 * @retval  PPMAX
 */
//============================================================================================
u8  WT_PPMaxGet( u16 wazano, u8 maxupcnt )
{
  u8  ppmax;

  if( maxupcnt > 3 ){
    maxupcnt = 3;
  }

  ppmax = WT_WazaDataParaGet( wazano, ID_WTD_pp );
  ppmax += ( ppmax * 20 * maxupcnt ) / 100;

  return  ppmax;
}

//============================================================================================
/**
 *  �Z�e�[�u���\���̃f�[�^����l���擾
 *
 * @param[in] wtd   �Z�e�[�u���\���̃f�[�^�ւ̃|�C���^
 * @param[in] id    �擾�������Z�f�[�^��Index
 */
//============================================================================================
u32 WT_WazaDataParaGetAct( WAZA_TABLE_DATA *wtd, int id )
{
  u32 ret;

  switch( id ){
  case ID_WTD_battleeffect:
    ret = wtd->battleeffect;
    break;
  case ID_WTD_kind:
    ret = wtd->kind;
    break;
  case ID_WTD_damage:
    ret = wtd->damage;
    break;
  case ID_WTD_wazatype:
    ret = wtd->wazatype;
    break;
  case ID_WTD_hitprobability:
    ret = wtd->hitprobability;
    break;
  case ID_WTD_pp:
    ret = wtd->pp;
    break;
  case ID_WTD_addeffect:
    ret = wtd->addeffect;
    break;
  case ID_WTD_attackrange:
    ret = wtd->attackrange;
    break;
  case ID_WTD_attackpri:
    ret = wtd->attackpri;
    break;
  case ID_WTD_flag:
    ret = wtd->flag;
    break;
  case ID_WTD_ap_no:        //�R���e�X�g�p�p�����[�^�@�A�s�[��No
    ret = wtd->condata.ap_no;
    break;
  case ID_WTD_contype:      //�R���e�X�g�p�p�����[�^�@�R���e�X�g�Z�^�C�v
    ret = wtd->condata.contype;
    break;
  }
  return ret;
}

//============================================================================================
/**
 *              �O�����J���Ȃ��֐��S
 */
//============================================================================================
//============================================================================================
/**
 *  �Z�e�[�u���\���̃f�[�^���擾
 *
 * @param[in] waza_no   �擾�������Z�i���o�[
 * @param[out]  wtd     �擾�����Z�f�[�^�̊i�[����w��
 */
//============================================================================================
static  void  WT_WazaDataGet( int waza_no, WAZA_TABLE_DATA *wtd )
{
  GFL_ARC_LoadData( wtd, ARCID_WAZA_TBL, waza_no );
}



//=============================================================================================
/**
 * @file  wazadata.c
 * @brief �|�P����WB  ���U�f�[�^�A�N�Z�T
 * @author  taya
 *
 * @date  2008.10.14  �쐬
 */
//=============================================================================================

#include "waza_tool/wazadata.h"


enum {
  HITRATIO_MUST = 101,
};

//------------------------------------------------------------------------------
/**
 * �萔�錾
 */
//------------------------------------------------------------------------------
enum {
  WSEQ_SINMPLESICK_NEMURI = 1,
  WSEQ_SINMPLESICK_DOKU = 66,
  WSEQ_SINMPLESICK_MAHI = 67,
  WSEQ_SINMPLESICK_MOUDOKU = 33,
  WSEQ_DAMAGESICK_DOKU = 2,
  WSEQ_DAMAGESICK_YAKEDO = 4,
  WSEQ_DAMAGESICK_KOORI = 5,
  WSEQ_DAMAGESICK_MAHI = 6,
  WSEQ_DAMAGESICK_KONRAN = 76,
  WSEQ_SIMPLEEFF_ATK = 10,
  WSEQ_SIMPLEEFF_DEF = 11,
  WSEQ_SIMPLEEFF_SPATK = 13,
  WSEQ_SIMPLEEFF_AVOID = 16,

  WSEQ_SIMPLEEFFEX_ATK = 50,
  WSEQ_SIMPLEEFFEX_DEF = 51,
  WSEQ_SIMPLEEFFEX_AGI = 52,
  WSEQ_SIMPLEEFFEX_SPATK = 53,
  WSEQ_SIMPLEEFFEX_SPDEF = 54,
  WSEQ_SIMPLEEFFEX_DOWN_ATK = 58,
  WSEQ_SIMPLEEFFEX_DOWN_DEF = 59,
  WSEQ_SIMPLEEFFEX_DOWN_AGI = 60,
  WSEQ_SIMPLEEFFEX_DOWN_SPDEF = 62,
  WSEQ_SIMPLEEFFCOMP_ATK_DEF = 208,
  WSEQ_SIMPLEEFFCOMP_DEF_SPDEF = 206,
  WSEQ_SIMPLEEFFCOMP_SPATK_SPDEF = 211,
  WSEQ_SIMPLEEFFCOMP_ATK_AGI = 212,

  WSEQ_DAMAGE_EFF_ATK = 68,
  WSEQ_DAMAGE_EFF_DEF = 69,
  WSEQ_DAMAGE_EFF_AGI = 70,
  WSEQ_DAMAGE_EFF_SPATK = 71,
  WSEQ_DAMAGE_EFF_SPDEF = 72,
  WSEQ_DAMAGE_EFF_AVOID = 73,
  WSEQ_SHRINK = 31,
  WSEQ_MUST_SHRINK = 158,
  WSEQ_WEATHER_RAIN = 136,
  WSEQ_WEATHER_SHINE = 137,
  WSEQ_WEATHER_STORM = 115,
  WSEQ_WEATHER_SNOW = 164,
  WSEQ_CRITICAL_UP = 43,
  WSEQ_ICHIGEKI = 38,
  WSEQ_EFF_SICK1 = 118,
  WSEQ_EFF_SICK2 = 166,
  WSEQ_SIMPLE_RECOVER = 32,
  WSEQ_SIMPLE_RECOVER2 = 214,
  WSEQ_YUUWAKU = 265,
  WSEQ_HEAT1 = 125,
  WSEQ_HEAT2 = 253,
  WSEQ_MARUKU = 156,
  WSEQ_FUMITUKE = 150,
  WSEQ_AKUMU = 107,
  WSEQ_FURAFURA = 199,
  WSEQ_MEROMERO = 120,

  WSEQ_REACT_HAKAI = 80,  ///< ���̃^�[���A�N�V�����ł��Ȃ�

  WSEQ_REACT_A1 = 48,   /// 1/4����
  WSEQ_REACT_B1 = 198,  /// 1/3����
  WSEQ_REACT_B2 = 253,  /// 1/3�����i�t���A�h���C�u�j
  WSEQ_REACT_B3 = 263,  /// 1/3�����i�{���e�b�J�[�j
  WSEQ_REACT_C1 = 269,  /// 1/2����

  WSEQ_PUSHOUT = 28,
  WSEQ_DRAIN = 3,

};

// @@@ ���o�[�W�����̃��U�f�[�^�V�[�P���X���A�V�o�[�W�����̃f�[�^�e�[�u���Ƃ��Ĉ������߂�
// �ꎞ�I�ȕϊ��p�e�[�u��
typedef struct {
  u16 wseq;
  u16 category;

  WazaSick  sick;
  u8        sickCont;
  u8        sickTurnMin;
  u8        sickTurnMax;
  struct {
    WazaRankEffect  type;
    s8              value;
  }rankEff[WAZA_RANKEFF_NUM_MAX];

  u8 shrinkFlg;
  u8 weather;
  u8 reactionRatio;
  u8 criticalRank;

}SEQ_PARAM;


static const SEQ_PARAM* getSeqParam( WazaID waza )
{
  static const SEQ_PARAM SeqParamTable[] = {
    {
      WSEQ_SINMPLESICK_NEMURI, WAZADATA_CATEGORY_SIMPLE_SICK,
      POKESICK_NEMURI, WAZASICK_CONT_TURN, 2, 5,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SINMPLESICK_DOKU, WAZADATA_CATEGORY_SIMPLE_SICK,
      POKESICK_DOKU, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SINMPLESICK_MAHI, WAZADATA_CATEGORY_SIMPLE_SICK,
      POKESICK_MAHI, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_DAMAGESICK_DOKU, WAZADATA_CATEGORY_DAMAGE_SICK,
      POKESICK_DOKU, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_DAMAGESICK_YAKEDO, WAZADATA_CATEGORY_DAMAGE_SICK,
      POKESICK_YAKEDO, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_DAMAGESICK_KOORI, WAZADATA_CATEGORY_DAMAGE_SICK,
      POKESICK_KOORI, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_DAMAGESICK_MAHI, WAZADATA_CATEGORY_DAMAGE_SICK,
      POKESICK_MAHI, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFF_ATK, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_ATTACK, 1 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFF_DEF, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_DEFENCE, 1 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFF_SPATK, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_SP_ATTACK, 1 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFF_AVOID, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_AVOID, 1 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFEX_ATK, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_ATTACK, 2 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFEX_DEF, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_DEFENCE, 2 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFEX_AGI, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_AGILITY, 2 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFEX_SPATK, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_SP_ATTACK, 2 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFEX_SPDEF, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_SP_DEFENCE, 2 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFCOMP_ATK_DEF, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_ATTACK, 1 }, { WAZA_RANKEFF_DEFENCE, 1 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFCOMP_DEF_SPDEF, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_DEFENCE, 1 }, { WAZA_RANKEFF_SP_DEFENCE, 1 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFCOMP_SPATK_SPDEF, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_SP_ATTACK, 1 }, { WAZA_RANKEFF_SP_DEFENCE, 1 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFCOMP_ATK_AGI, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_ATTACK, 1 }, { WAZA_RANKEFF_AGILITY, 1 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFEX_DOWN_ATK, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_ATTACK, -2 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFEX_DOWN_DEF, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_DEFENCE, -2 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFEX_DOWN_AGI, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_AGILITY, -2 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_SIMPLEEFFEX_DOWN_SPDEF, WAZADATA_CATEGORY_SIMPLE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_SP_DEFENCE, -2 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_DAMAGE_EFF_ATK, WAZADATA_CATEGORY_DAMAGE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_ATTACK, -1 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_DAMAGE_EFF_DEF, WAZADATA_CATEGORY_DAMAGE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_DEFENCE, -1 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_DAMAGE_EFF_SPATK, WAZADATA_CATEGORY_DAMAGE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_SP_ATTACK, -1 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_DAMAGE_EFF_SPDEF, WAZADATA_CATEGORY_DAMAGE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_SP_DEFENCE, -1 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
      WSEQ_DAMAGE_EFF_AVOID, WAZADATA_CATEGORY_DAMAGE_EFFECT,
      POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_AVOID, -1 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_SHRINK, WAZADATA_CATEGORY_SIMPLE_DAMAGE,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      TRUE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_MUST_SHRINK, WAZADATA_CATEGORY_SIMPLE_DAMAGE,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      TRUE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_WEATHER_RAIN, WAZADATA_CATEGORY_WEATHER,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_RAIN, 0, 0,
    },{
        WSEQ_WEATHER_SHINE, WAZADATA_CATEGORY_WEATHER,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_SHINE, 0, 0,
    },{
        WSEQ_WEATHER_STORM, WAZADATA_CATEGORY_WEATHER,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_SAND, 0, 0,
    },{
        WSEQ_WEATHER_SNOW, WAZADATA_CATEGORY_WEATHER,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_SNOW, 0, 0,
    },{
        WSEQ_REACT_A1, WAZADATA_CATEGORY_SIMPLE_DAMAGE,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 25, 0,
    },{
        WSEQ_REACT_B1, WAZADATA_CATEGORY_SIMPLE_DAMAGE,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 33, 0,
    },{
        WSEQ_REACT_B2, WAZADATA_CATEGORY_SIMPLE_DAMAGE,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 33, 0,
    },{
        WSEQ_REACT_B3, WAZADATA_CATEGORY_SIMPLE_DAMAGE,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 33, 0,
    },{
        WSEQ_REACT_C1, WAZADATA_CATEGORY_SIMPLE_DAMAGE,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 50, 0,
    },{
        WSEQ_REACT_HAKAI, WAZADATA_CATEGORY_SIMPLE_DAMAGE,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_CRITICAL_UP, WAZADATA_CATEGORY_SIMPLE_DAMAGE,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 1,
    },{
        WSEQ_ICHIGEKI, WAZADATA_CATEGORY_ICHIGEKI,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_PUSHOUT, WAZADATA_CATEGORY_PUSHOUT,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_DRAIN, WAZADATA_CATEGORY_DRAIN,
        POKESICK_NULL, WAZASICK_CONT_PERMANENT, 0, 0,
      { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
      FALSE, BTL_WEATHER_NONE, 50, 0,
    },{
        WSEQ_EFF_SICK1, WAZADATA_CATEGORY_EFFECT_SICK,
        WAZASICK_KONRAN, WAZASICK_CONT_PERMANENT, 0, 0,
        { { WAZA_RANKEFF_ATTACK, 2 }, { WAZA_RANKEFF_NULL, 0 } },
        FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_EFF_SICK2, WAZADATA_CATEGORY_EFFECT_SICK,
        WAZASICK_KONRAN, WAZASICK_CONT_PERMANENT, 0, 0,
        { { WAZA_RANKEFF_SP_ATTACK, 1 }, { WAZA_RANKEFF_NULL, 0 } },
        FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_SIMPLE_RECOVER, WAZADATA_CATEGORY_SIMPLE_RECOVER,
        WAZASICK_KONRAN, WAZASICK_CONT_PERMANENT, 0, 0,
        { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
        FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_SIMPLE_RECOVER2, WAZADATA_CATEGORY_SIMPLE_RECOVER,
        WAZASICK_KONRAN, WAZASICK_CONT_PERMANENT, 0, 0,
        { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
        FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_YUUWAKU, WAZADATA_CATEGORY_SIMPLE_EFFECT,
        WAZASICK_NULL, 0, 0, 0,
        { { WAZA_RANKEFF_SP_ATTACK, -2 }, { WAZA_RANKEFF_NULL, 0 } },
        FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_HEAT1, WAZADATA_CATEGORY_DAMAGE_SICK,
        WAZASICK_YAKEDO, WAZASICK_CONT_PERMANENT, 0, 0,
        { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
        FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_HEAT2, WAZADATA_CATEGORY_DAMAGE_SICK,
        WAZASICK_YAKEDO, WAZASICK_CONT_PERMANENT, 0, 0,
        { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
        FALSE, BTL_WEATHER_NONE, 33, 0,
    },{
        WSEQ_MARUKU, WAZADATA_CATEGORY_SIMPLE_EFFECT,
        WAZASICK_NULL, 0, 0, 0,
        { { WAZA_RANKEFF_DEFENCE, 1 }, { WAZA_RANKEFF_NULL, 0 } },
        FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_FUMITUKE, WAZADATA_CATEGORY_SIMPLE_DAMAGE,
        WAZASICK_NULL, 0, 0, 0,
        { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
        TRUE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_AKUMU, WAZADATA_CATEGORY_SIMPLE_SICK,
        WAZASICK_AKUMU, WAZASICK_CONT_PERMANENT, 0, 0,
        { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
        FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_FURAFURA, WAZADATA_CATEGORY_SIMPLE_SICK,
        WAZASICK_KONRAN, WAZASICK_CONT_PERMANENT, 0, 0,
        { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
        FALSE, BTL_WEATHER_NONE, 0, 0,
    },{
        WSEQ_MEROMERO, WAZADATA_CATEGORY_SIMPLE_SICK,
        WAZASICK_MEROMERO, WAZASICK_CONT_POKE, 0, 0,
        { { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
        FALSE, BTL_WEATHER_NONE, 0, 0,
    }

/*
  typedef struct {
    u16 wseq;
    u16 category;

    WazaSick  sick;
    u8        sickCont;
    u8        sickTurnMin;
    u8        sickTurnMax;
    struct {
      WazaRankEffect  type;
      s8              value;
    }rankEff[WAZA_RANKEFF_NUM_MAX];

    u8 shrinkFlg;
    u8 weather;
    u8 reactionRatio;
    u8 criticalRank;

  }SEQ_PARAM;
*/
  };
  u16 seq = WT_WazaDataParaGet( waza, ID_WTD_battleeffect );
  u16 i;

  for(i=0; i<NELEMS(SeqParamTable); ++i)
  {
    if( SeqParamTable[i].wseq == seq )
    {
      return &SeqParamTable[i];
    }
  }
  return NULL;
}

// ���U�D��x
s8   WAZADATA_GetPriority( WazaID id )
{
  return WT_WazaDataParaGet( id, ID_WTD_attackpri );
}
// ���U�J�e�S��
WazaCategory  WAZADATA_GetCategory( WazaID id )
{
  const SEQ_PARAM* seqparam = getSeqParam( id );

  if( seqparam )
  {
    return seqparam->category;
  }

  if( WAZADATA_IsDamage(id) )
  {
    return WAZADATA_CATEGORY_SIMPLE_DAMAGE;
  }
  else
  {
    return WAZADATA_CATEGORY_OTHERS;
  }
}

PokeType WAZADATA_GetType( WazaID id )
{
  return WT_WazaDataParaGet( id, ID_WTD_wazatype );
}

u16 WAZADATA_GetPower( WazaID id )
{
  return WT_WazaDataParaGet( id, ID_WTD_damage );
}

WazaDamageType WAZADATA_GetDamageType( WazaID id )
{
//  return WT_WazaDataParaGet( id, ID_WTD_kind );
  switch( WT_WazaDataParaGet( id, ID_WTD_kind ) ){
  case 0:   return  WAZADATA_DMG_PHYSIC;  ///< �����_���[�W
  case 1:   return  WAZADATA_DMG_SPECIAL;
  default:  return WAZADATA_DMG_NONE;
  }
}

// ������
u8 WAZADATA_GetHitRatio( WazaID id )
{
  u8 ret = WT_WazaDataParaGet( id, ID_WTD_hitprobability );
  TAYA_Printf("[WAZATL] GetHitRatio : waza=%d, ratio=%d\n", id, ret);
  if( ret == 0 )
  {
    ret = 100;
  }
  return ret;
}

// �K���t���O�B�΂߂������Ȃǂ�TRUE��Ԃ��B
BOOL WAZADATA_IsAlwaysHit( WazaID id )
{
  return WAZADATA_GetHitRatio(id) == HITRATIO_MUST;
}

// �_���[�W���U���ǂ���
BOOL WAZADATA_IsDamage( WazaID id )
{
  // @@@ ���܂͂Ă��Ɓ[
  return WAZADATA_GetPower(id) != 0;
}

// �ō��q�b�g��
u8 WAZADATA_GetMaxHitCount( WazaID id )
{
  return 1;
}

// �ڐG���U���ǂ���
BOOL WAZADATA_IsTouch( WazaID id )
{
  // @@@ ���܂͂Ă��Ɓ[
  return WAZADATA_IsDamage( id );
}




// ���ʔ͈�
WazaTarget WAZADATA_GetTarget( WazaID id )
{
  enum {
    OLD_RANGE_NORMAL    = 0,
    OLD_RANGE_NONE      = 1,
    OLD_RANGE_RANDOM    = 2,
    OLD_RANGE_ENEMY2    = 4,
    OLD_RANGE_OTHER_ALL = 8,
    OLD_RANGE_USER      = 16,
    OLD_RANGE_FRIEND_ALL = 32,
    OLD_RANGE_WEATHER   = 64,
    OLD_RANGE_MAKIBISI  = 128,
    OLD_RANGE_TEDASUKE  = 256,
    OLD_RANGE_TUBO      = 512,
    OLD_RANGE_SAKIDORI  = 1024,
  };

  switch( id ){
  case WAZANO_TEKUSUTYAA2:  return WAZA_TARGET_OTHER_SELECT;
  case WAZANO_KAUNTAA:
  case WAZANO_MIRAAKOOTO:
  case WAZANO_METARUBAASUTO:
     return WAZA_TARGET_UNKNOWN;
  default:
    break;
  }

  {
    u32 old_range = WT_WazaDataParaGet( id, ID_WTD_attackrange );

    switch( old_range ){
    case OLD_RANGE_NORMAL:    return WAZA_TARGET_OTHER_SELECT;
    case OLD_RANGE_NONE:      return WAZA_TARGET_UNKNOWN;
    case OLD_RANGE_ENEMY2:    return WAZA_TARGET_ENEMY_ALL;
    case OLD_RANGE_OTHER_ALL: return WAZA_TARGET_OTHER_ALL;
    case OLD_RANGE_USER:      return WAZA_TARGET_USER;
    case OLD_RANGE_FRIEND_ALL:return WAZA_TARGET_SIDE_FRIEND;
    case OLD_RANGE_RANDOM:    return WAZA_TARGET_ENEMY_RANDOM;
    case OLD_RANGE_WEATHER:   return WAZA_TARGET_FIELD;
    case OLD_RANGE_MAKIBISI:  return WAZA_TARGET_SIDE_ENEMY;
    case OLD_RANGE_TEDASUKE:  return WAZA_TARGET_FRIEND_SELECT;
    case OLD_RANGE_TUBO:      return WAZA_TARGET_FRIEND_USER_SELECT;
    case OLD_RANGE_SAKIDORI:  return WAZA_TARGET_ENEMY_SELECT;
    }
  }

  return WAZA_TARGET_OTHER_SELECT;///< �����ȊO�̂P�́i�I���j
}


// �N���e�B�J�������N
u8 WAZADATA_GetCriticalRank( WazaID id )
{
  const SEQ_PARAM* seq = getSeqParam( id );
  if( seq ){
    return seq->criticalRank;
  }
  return 0;
}

//=============================================================================================
/**
 * �������i����ɗ^�����_���[�W�̉����𔽓��Ƃ��Ď������󂯂邩�j��Ԃ�
 *
 * @param   id
 *
 * @retval  u8    �������i�����Ȃ��Ȃ�0, max100�j
 */
//=============================================================================================
u8 WAZADATA_GetReactionRatio( WazaID id )
{
  const SEQ_PARAM* seq = getSeqParam( id );
  if( seq ){
    return seq->reactionRatio;
  }
  return 0;
}
//=============================================================================================
/**
 * �h���C���Z�̃_���[�W�l->��HP�Ҍ���
 *
 * @param   id
 *
 * @retval  u8
 */
//=============================================================================================
u8 WAZADATA_GetDrainRatio( WazaID id )
{
  // @@@ ���܂͂Ă��Ɓ[
  const SEQ_PARAM* seq = getSeqParam( id );
  if( seq ){
    return seq->reactionRatio;
  }
  return 0;
}
//=============================================================================================
/**
 * �����N���ʂ̎�ސ���Ԃ�
 *
 * @param   id
 *
 * @retval  u8
 */
//=============================================================================================
u8 WAZADATA_GetRankEffectCount( WazaID id )
{
  const SEQ_PARAM* seq = getSeqParam( id );
  if( seq ){
    u8 cnt = 0;
    if( seq->rankEff[0].type != WAZA_RANKEFF_NULL ){ ++cnt; }
    if( seq->rankEff[1].type != WAZA_RANKEFF_NULL ){ ++cnt; }
    return cnt;
  }
  return 0;
}
//=============================================================================================
/**
 * �����N���ʂ��擾
 *
 * @param   id        [in] ���UID
 * @param   idx       [in] ���Ԗڂ̃����N���ʂ��H�i0�`�j
 * @param   volume    [out] �����N���ʂ̒��x�i+�Ȃ�A�b�v, -�Ȃ�_�E��, �߂�l==WAZA_RANKEFF_NULL�̎��̂�0�j
 *
 * @retval  WazaRankEffect    �����N����ID
 */
//=============================================================================================
WazaRankEffect WAZADATA_GetRankEffect( WazaID id, u32 idx, int* volume )
{
  GF_ASSERT(idx < WAZA_RANKEFF_NUM_MAX);
  {
    const SEQ_PARAM* seq = getSeqParam( id );
    if( seq ){
      if( seq->rankEff[idx].type != WAZA_RANKEFF_NULL )
      {
        *volume = seq->rankEff[idx].value;
        return seq->rankEff[idx].type;
      }
    }
    *volume = 0;
    return WAZA_RANKEFF_NULL;
  }
}
//=============================================================================================
/**
 * �����N���ʂ̔�������Ԃ�
 *
 * @param   id    ���UID
 *
 * @retval  u32   �������i�p�[�Z���e�[�W = 0�`100�j
 */
//=============================================================================================
u32 WAZADATA_GetRankEffPer( WazaID id )
{
  // @@@ ���͂Ă��Ƃ���
  return WT_WazaDataParaGet( id, ID_WTD_addeffect );
}
//=============================================================================================
/**
 * ���U�ɂ���Ĕ��������Ԉُ�ID���擾
 *
 * @param   id
 * @param   param   [out] �p�����[�^�擾�p�\���̃|�C���^
 *
 * @retval  WazaSick    ��Ԉُ�ID�i�������Ȃ��ꍇ WAZASICK_NULL�j
 */
//=============================================================================================
WazaSick WAZADATA_GetSick( WazaID id )
{
  const SEQ_PARAM* seq = getSeqParam( id );
  if( seq ){
    return seq->sick;
  }
  return WAZASICK_NULL;
}
//=============================================================================================
/**
 * ���U�ɂ���Ĕ��������Ԉُ�̌p���p�����[�^���擾
 *
 * @param   id
 *
 * @retval  WAZA_SICKCONT_PARAM
 */
//=============================================================================================
WAZA_SICKCONT_PARAM WAZADATA_GetSickCont( WazaID id )
{
  WAZA_SICKCONT_PARAM  param;

  const SEQ_PARAM* seq = getSeqParam( id );
  if( seq ){
    param.type = seq->sickCont;
    param.turnMin = seq->sickTurnMin;;
    param.turnMax = seq->sickTurnMin;
  }else{
    GF_ASSERT(0);
    param.type = 0;
    param.turnMin = 0;
    param.turnMax = 0;
  }
  return param;
}
//=============================================================================================
/**
 * �ǉ��̏�Ԉُ픭������Ԃ�
 *
 * @param   id    ���UID
 *
 * @retval  u32   �������i�p�[�Z���e�[�W = 0�`100�j
 */
//=============================================================================================
u32 WAZADATA_GetSickPer( WazaID id )
{
  // @@@ ���͂Ă��Ƃ���
  return WT_WazaDataParaGet( id, ID_WTD_addeffect );
}
//=============================================================================================
/**
 * �ǉ��́u�Ђ�݁v����Ԃ�
 *
 * @param   id    ���UID
 *
 * @retval  u32   �������i�p�[�Z���e�[�W = 0�`100�j
 */
//=============================================================================================
u32 WAZADATA_GetShrinkPer( WazaID id )
{
  const SEQ_PARAM* seq = getSeqParam( id );
  if( seq && seq->shrinkFlg ){
    return WT_WazaDataParaGet( id, ID_WTD_addeffect );
  }
  return 0;
}
//=============================================================================================
/**
 * ���U�g�p��A����ĂP�^�[�������Ȃ��Ȃ��Ă��܂����`�F�b�N
 *
 * @param   id
 *
 * @retval  BOOL    �����Ȃ��Ȃ��Ă��܂��Ȃ�TRUE
 */
//=============================================================================================
BOOL WAZADATA_IsTire( WazaID id )
{
  const SEQ_PARAM* seq = getSeqParam( id );
  if( seq && (seq->wseq == WSEQ_REACT_HAKAI) ){
    return TRUE;
  }
  return FALSE;
}
//=============================================================================================
/**
 * �V����ʂ�Ԃ�
 *
 * @param   id
 *
 * @retval  BtlWeather
 */
//=============================================================================================
BtlWeather WAZADATA_GetWeather( WazaID id )
{
  const SEQ_PARAM* seq = getSeqParam( id );
  if( seq ){
    return seq->weather;
  }
  return BTL_WEATHER_NONE;
}
//=============================================================================================
/**
 * ���U�g�p�ɂ��HP�񕜗��i���j���擾
 *
 * @param   id
 *
 * @retval  u8
 */
//=============================================================================================
u8 WAZADATA_GetRecoverHPRatio( WazaID id )
{
  const SEQ_PARAM* seq = getSeqParam( id );
  if( seq && (seq->wseq == WSEQ_SIMPLE_RECOVER) ){
    return 50;
  }
  return 0;
}
//=============================================================================================
/**
 * ���U�C���[�W�`�F�b�N
 *
 * @param   id    ���UID
 * @param   img   �C���[�WID
 *
 * @retval  BOOL    ���U���w�肳�ꂽ�C���[�W�ɑ�����ꍇ��TRUE
 */
//=============================================================================================
BOOL WAZADATA_IsImage( WazaID id, WazaImage img )
{
  // @@@ ���͂Ă��Ɓ[
  switch( img ){
  case WAZA_IMG_HEAT:
    if( id == WAZANO_SEINARUHONOO ){ return TRUE; }
    if( id == WAZANO_KAENGURUMA ){ return TRUE; }
    if( id == WAZANO_HUREADORAIBU ){ return TRUE; }
    break;
  case WAZA_IMG_PUNCH:
    if( id == WAZANO_MEGATONPANTI) { return TRUE; }
    if( id == WAZANO_DOREINPANTI) { return TRUE; }
    if( id == WAZANO_HONOONOPANTI) { return TRUE; }
    if( id == WAZANO_REITOUPANTI) { return TRUE; }
    if( id == WAZANO_KAMINARIPANTI) { return TRUE; }
    if( id == WAZANO_SYADOOPANTI) { return TRUE; }
    if( id == WAZANO_RENZOKUPANTI) { return TRUE; }
    if( id == WAZANO_PIYOPIYOPANTI) { return TRUE; }
    if( id == WAZANO_BAKURETUPANTI) { return TRUE; }
    if( id == WAZANO_MAHHAPANTI) { return TRUE; }
    if( id == WAZANO_BARETTOPANTI) { return TRUE; }
    if( id == WAZANO_KOMETTOPANTI) { return TRUE; }
    if( id == WAZANO_SUKAIAPPAA) { return TRUE; }

    break;
  case WAZA_IMG_SOUND:
    break;
  }

  return FALSE;
}
//=============================================================================================
/**
 * ���U�t���O�擾
 *
 * @param   id
 * @param   flag
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL WAZADATA_GetFlag( WazaID id, WazaFlag flag )
{
  GF_ASSERT( flag < WAZAFLAG_MAX );

  // @@@ ���͂Ă��Ɓ[
  switch( flag ){
  case WAZAFLAG_MAMORU:
    return WAZADATA_IsDamage( id );

  case WAZAFLAG_TAME:
    if( id == WAZANO_KAMAITATI ){ return TRUE; }
    if( id == WAZANO_ROKETTOZUTUKI ){ return TRUE; }
    if( id == WAZANO_SORAWOTOBU ){ return TRUE; }
    if( id == WAZANO_SOORAABIIMU ){ return TRUE; }
    if( id == WAZANO_DAIBINGU ){ return TRUE; }
    if( id == WAZANO_ANAWOHORU ){ return TRUE; }
    if( id == WAZANO_GODDOBAADO ){ return TRUE; }
    if( id == WAZANO_TOBIHANERU ){ return TRUE; }
    if( id == WAZANO_SYADOODAIBU ){ return TRUE; }
    return FALSE;

  default:
    break;

  }
  return FALSE;
}



#ifdef PM_DEBUG
void WAZADATA_PrintDebug( void )
{
  static const struct {
    const char* name;
    u16 id;
  }tbl[] = {
    { "�݂��ł��ۂ��i�ʏ�j",        WAZANO_MIZUDEPPOU },
    { "�˂��Ղ��i����Q�́j",        WAZANO_NEPPUU },
    { "�Ȃ݂̂�i�R�́j",            WAZANO_NAMINORI },
    { "�����Ԃ񂵂�i�����j",        WAZANO_KAGEBUNSIN },
    { "���낢����i��j",            WAZANO_KUROIKIRI },
    { "�������i�����_���j",        WAZANO_KUROIKIRI },
    { "���܂����i��j",              WAZANO_AMAGOI },
    { "���΂��i�����_���j",        WAZANO_ABARERU },
    { "�A���}�Z���s�[�i�����Q�́j",  WAZANO_AROMASERAPII },
    { "�܂��т��i�܂��т��j",        WAZANO_MAKIBISI },
    { "�Ă������i�Ă������j",        WAZANO_TEDASUKE },
    { "�ڂ��i�ڂ��j",        WAZANO_TUBOWOTUKU },
    { "�����ǂ�i�����ǂ�j",        WAZANO_SAKIDORI },
    { "�I�E���������i�Ȃ��j",        WAZANO_OUMUGAESI },
  };
  u32 i, range;

  OS_TPrintf("***** ���U���ʔ͈͒l�`�F�b�N *****\n");
  for(i=0; i<NELEMS(tbl); ++i)
  {
    range = WT_WazaDataParaGet( tbl[i].id, ID_WTD_attackrange );
    OS_TPrintf("%s\t=%d\n", tbl[i].name, range);

  }
}

#endif
