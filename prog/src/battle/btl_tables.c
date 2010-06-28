//=============================================================================================
/**
 * @file  btl_tables.c
 * @brief �|�P����WB �Ă��Ƃ��ȃT�C�Y�̃e�[�u����VRAM_H�ɒu���ăA�N�Z�X����
 * @author  taya
 *
 * @date  2010.05.27  �쐬
 */
//=============================================================================================

#include "poke_tool\poketype.h"
#include "poke_tool\monsno_def.h"
#include "waza_tool\wazano_def.h"
#include "item\itemsym.h"
#include "item\item.h"
#include "battle\battle_bg_def.h"

#include "btl_common.h"
#include "btlv\btlv_effect.h"
#include "btl_tables.h"

//----------------------------------------------------------------------------------
/**
 * �e�[�u���v�f�ɊY���l�����݂��邩����
 *
 * @param   value       ���肷��l
 * @param   table       �e�[�u���A�h���X
 * @param   tableElems  �e�[�u���v�f��
 *
 * @retval  BOOL    �Y���l���e�[�u���Ɋ܂܂ꂽ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL checkTableElems( u16 value, const u16* table, u32 tableElems )
{
  u32 i;
  for(i=0; i<tableElems; ++i)
  {
    if( table[i] == value ){
      return TRUE;
    }
  }
  return FALSE;
}

/**
 *  �A���R�[�����O�Ώۂ̃��U������
 */
BOOL BTL_TABLES_IsMatchEncoreFail( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_NULL,    WAZANO_ANKOORU,  WAZANO_OUMUGAESI,
    WAZANO_HENSIN,  WAZANO_MONOMANE, WAZANO_SUKETTI,
  };

  return checkTableElems( waza, table, NELEMS(table) );
}
/**
 *  �����ǂ莸�s���U����
 */
BOOL BTL_TABLES_IsSakidoriFailWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_NULL, WAZANO_MIRAAKOOTO, WAZANO_KAUNTAA, WAZANO_METARUBAASUTO,
    WAZANO_KIAIPANTI, WAZANO_SAKIDORI, WAZANO_DOROBOU, WAZANO_HOSIGARU,
    WAZANO_OSYABERI, WAZANO_WARUAGAKI,
  };

  return checkTableElems( waza, table, NELEMS(table) );
}

/**
 *  ���ԍ��U�����U����
 */
BOOL BTL_TABLES_IsDelayAttackWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_NULL, WAZANO_MIRAIYOTI, WAZANO_HAMETUNONEGAI,
  };

  return checkTableElems( waza, table, NELEMS(table) );
}

/**
 *  �v���b�V���[�Ώۃ��U����
 */
BOOL BTL_TABLES_IsPressureEffectiveWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_YOKODORI, WAZANO_HUUIN,
    WAZANO_MAKIBISI, WAZANO_DOKUBISI, WAZANO_SUTERUSUROKKU,

  };

  return checkTableElems( waza, table, NELEMS(table) );
}


/**
 *  �W���G���n���������Ȃ����U����
 */
BOOL BTL_TABLES_IsJuelOmmitWaza( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_HONOONOTIKAI, WAZANO_KUSANOTIKAI, WAZANO_MIZUNOTIKAI,
  };

  return checkTableElems( waza, table, NELEMS(table) );
}



/**
 *  �A�C�e���R�[���Ŕ������Ă͂����Ȃ��A�C�e��������
 */
BOOL BTL_TABLES_CheckItemCallNoEffect( u16 itemID )
{
  static const u16 table[] = {
    ITEM_KIAINOHATIMAKI,  ITEM_PAWAHURUHAABU,   ITEM_DASSYUTUBOTAN,
    ITEM_ZYAPONOMI,       ITEM_RENBUNOMI,       ITEM_IBANNOMI,
    ITEM_SENSEINOTUME,    ITEM_KYUUKON,         ITEM_ZYUUDENTI,
  };

  return checkTableElems( itemID, table, NELEMS(table) );
}


/**
 *  �g�p����Ώۃ|�P�����̑I�����s���K�v�������A�C�e��������
 */
BOOL BTL_TABLES_IsNoTargetItem( u16 itemID )
{
  static const u16 table[] = {
    ITEM_ENEKONOSIPPO,  ITEM_PIPPININGYOU, ITEM_POKEZYARASI,
  };

  return checkTableElems( itemID, table, NELEMS(table) );
}

/**
 *  �u�Ȃ肫��v�Ɏ��s����Ƃ���������
 */
BOOL BTL_TABLES_CheckNarikiriFailTokusei( u16 tokuseiID )
{
  static const u16 table[] = {
    POKETOKUSEI_TOREESU,       POKETOKUSEI_TENKIYA,         POKETOKUSEI_MARUTITAIPU,
    POKETOKUSEI_FURAWAAGIFUTO, POKETOKUSEI_DARUMAMOODO,     POKETOKUSEI_IRYUUJON,
    POKETOKUSEI_KAWARIMONO,    POKETOKUSEI_FUSIGINAMAMORI,
  };

  if( BTL_TABLES_IsNeverChangeTokusei(tokuseiID) ){
    return TRUE;
  }

  return checkTableElems( tokuseiID, table, NELEMS(table) );
}

/**
 *  �u�Ȃ��܂Â���v�Ɏ��s����Ƃ���������
 */

BOOL BTL_TABLES_CheckNakamaDukuriFailTokusei( u16 tokuseiID )
{
  // �u�Ȃ肫��v�ƈꏏ
  return BTL_TABLES_CheckTraceFailTokusei( tokuseiID );
}

/**
 *  �g���[�X�ł��Ȃ��Ƃ���������
 */
BOOL BTL_TABLES_CheckTraceFailTokusei( u16 tokuseiID )
{
  // �g���[�X�̏ꍇ�́u�ӂ����Ȃ܂���v��������
  if( tokuseiID == POKETOKUSEI_FUSIGINAMAMORI ){
    return FALSE;
  }

  // ���Ƃ́u�Ȃ肫��v�ƈꏏ
  return BTL_TABLES_CheckNarikiriFailTokusei( tokuseiID );
}

/**
 *  �X�L���X���b�v�ł��Ȃ��Ƃ���������i����E�����̂ǂ��炩����ł��Y��������_���j
 */
BOOL BTL_TABLES_CheckSkillSwapFailTokusei( u16 tokuseiID )
{
  static const u16 table[] = {
    POKETOKUSEI_FUSIGINAMAMORI, POKETOKUSEI_MARUTITAIPU, POKETOKUSEI_IRYUUJON,
  };

  if( BTL_TABLES_IsNeverChangeTokusei(tokuseiID) ){
    return TRUE;
  }

  return checkTableElems( tokuseiID, table, NELEMS(table) );
}

/**
 *  ��΂ɏ���������Ă͂����Ȃ��Ƃ���������
 */
BOOL BTL_TABLES_IsNeverChangeTokusei( u16 tokuseiID )
{
  // ����Ƃ��}���`�^�C�v����
  return ( tokuseiID == POKETOKUSEI_MARUTITAIPU );
}


/**
 *  �A���Z�E�X�̃v���[�g����
 */
BOOL BTL_TABLES_IsMatchAruseusPlate( u16 itemID )
{
  static const u16 table[] = {
    ITEM_HINOTAMAPUREETO,
    ITEM_SIZUKUPUREETO,
    ITEM_IKAZUTIPUREETO,
    ITEM_MIDORINOPUREETO,
    ITEM_TURARANOPUREETO,
    ITEM_KOBUSINOPUREETO,
    ITEM_MOUDOKUPUREETO,
    ITEM_DAITINOPUREETO,
    ITEM_AOZORAPUREETO,
    ITEM_HUSIGINOPUREETO,
    ITEM_TAMAMUSIPUREETO,
    ITEM_GANSEKIPUREETO,
    ITEM_MONONOKEPUREETO,
    ITEM_RYUUNOPUREETO,
    ITEM_KOWAMOTEPUREETO,
    ITEM_KOUTETUPUREETO,
  };

  return checkTableElems( itemID, table, NELEMS(table) );
}

/**
 *  �C���Z�N�^�̃J�Z�b�g����
 */
BOOL BTL_TABLES_IsMatchInsectaCasette( u16 itemID )
{
  static const u16 table[] = {
    ITEM_AKUAKASETTO,
    ITEM_INAZUMAKASETTO,
    ITEM_BUREIZUKASETTO,
    ITEM_HURIIZUKASETTO,
  };

  return checkTableElems( itemID, table, NELEMS(table) );
}


/**
 *  BG�^�C�v���牮�O���ǂ�������i�_�[�N�{�[���p�j
 */
BOOL BTL_TABLES_IsOutdoorBGType( u16 bgType )
{
  static const u16 table[] = {
    BATTLE_BG_TYPE_GRASS,         ///< ���ނ�
    BATTLE_BG_TYPE_GRASS_SEASON,  ///< ���ނ�(�l�G�L��)
    BATTLE_BG_TYPE_CITY,          ///< �X
    BATTLE_BG_TYPE_CITY_SEASON,   ///< �X(�l�G�L��)
    BATTLE_BG_TYPE_CAVE,          ///< ���A
    BATTLE_BG_TYPE_CAVE_DARK,     ///< ���A(�Â�)
    BATTLE_BG_TYPE_FOREST,        ///< �X
    BATTLE_BG_TYPE_MOUNTAIN,      ///< �R
    BATTLE_BG_TYPE_SEA,           ///< �C
    BATTLE_BG_TYPE_SAND,          ///< ����
  };
  return checkTableElems( bgType, table, NELEMS(table) );
}

/**
 *  ������Ԃ肪����������Ώۂ̂Ƃ���������
 */
BOOL BTL_TABLES_IsMatchKatayaburiTarget( u16 tokuseiID )
{
  static const u16 table[] = {
    POKETOKUSEI_FUSIGINAMAMORI, POKETOKUSEI_BOUON,        POKETOKUSEI_FUYUU,
    POKETOKUSEI_SUNAGAKURE,     POKETOKUSEI_YUKIGAKURE,   POKETOKUSEI_TYOSUI,
    POKETOKUSEI_KABUTOAAMAA,    POKETOKUSEI_HIRAISIN,     POKETOKUSEI_YOBIMIZU,
    POKETOKUSEI_SHERUAAMAA,     POKETOKUSEI_TENNEN,       POKETOKUSEI_HIRAISIN,
    POKETOKUSEI_YOBIMIZU,       POKETOKUSEI_KYUUBAN,      POKETOKUSEI_TANJUN,
    POKETOKUSEI_TIDORIASI,      POKETOKUSEI_HAADOROKKU,   POKETOKUSEI_FIRUTAA,
    POKETOKUSEI_MORAIBI,        POKETOKUSEI_DENKIENJIN,   POKETOKUSEI_NENCHAKU,
    POKETOKUSEI_FUSIGINAUROKO,  POKETOKUSEI_ATUISIBOU,    POKETOKUSEI_TAINETU,
    POKETOKUSEI_SIROIKEMURI,    POKETOKUSEI_KURIABODY,    POKETOKUSEI_SURUDOIME,
    POKETOKUSEI_KAIRIKIBASAMI,  POKETOKUSEI_SEISINRYOKU,  POKETOKUSEI_RINPUN,
    POKETOKUSEI_GANJOU,         POKETOKUSEI_SIMERIKE,     POKETOKUSEI_DONKAN,
    POKETOKUSEI_JUUNAN,         POKETOKUSEI_MAIPEESU,     POKETOKUSEI_MIZUNOBEERU,
    POKETOKUSEI_RIIFUGAADO,     POKETOKUSEI_FUMIN,        POKETOKUSEI_HATOMUNE,
    POKETOKUSEI_YARUKI,         POKETOKUSEI_MENEKI,       POKETOKUSEI_MAGUMANOYOROI,
    POKETOKUSEI_AMANOJAKU,      POKETOKUSEI_FURENDOGAADO, POKETOKUSEI_MARUTISUKEIRU,
    POKETOKUSEI_TEREPASII,      POKETOKUSEI_HATOMUNE,     POKETOKUSEI_MIRAKURUSUKIN,
    POKETOKUSEI_MAJIKKUMIRAA,   POKETOKUSEI_SOUSYOKU,     POKETOKUSEI_TIKUDEN,
    POKETOKUSEI_KANSOUHADA,     POKETOKUSEI_FURAWAAGIFUTO,
    POKETOKUSEI_HEVIMETARU,     POKETOKUSEI_RAITOMETARU,
  };

  return checkTableElems( tokuseiID, table, NELEMS(table) );

}

/**
 *  �^�C�v�����������s���郂���X�^�[�i���o�[����
 */
BOOL BTL_TABLES_IsTypeChangeForbidPoke( u16 monsno )
{
  return monsno == MONSNO_ARUSEUSU;
}

/**
 *  ���[�U�R���t�B�O�u�킴�G�t�F�N�g�F�݂Ȃ��v�ŃX�L�b�v����G�t�F�N�g�w��
 */
BOOL BTL_TABLES_IsEffectDisableType( u16 effectNo )
{
  static const u16 table[] = {
    BTLEFF_NEMURI, BTLEFF_MAHI, BTLEFF_DOKU, BTLEFF_YAKEDO, BTLEFF_KOORI,
    BTLEFF_KONRAN, BTLEFF_MEROMERO, BTLEFF_NOROI, BTLEFF_AKUMU,
    BTLEFF_SHOOTER_EFFECT, BTLEFF_USE_ITEM, BTLEFF_HP_RECOVER,
  };

  return checkTableElems( effectNo, table, NELEMS(table) );
}


//=============================================================================================
/**
 * �����^���n�[�u�Ŏ�����Ԉُ�e�[�u�����ԃA�N�Z�X
 *
 * @param   idx   0�`
 *
 * @retval  WazaSick    �L��Idx�̎��A��Ԉُ�R�[�h�^����ȊO WAZASICK_NULL
 */
//=============================================================================================
WazaSick  BTL_TABLES_GetMentalSickID( u32 idx )
{
  static const WazaSick table[] = {
    WAZASICK_MEROMERO,      WAZASICK_ICHAMON,   WAZASICK_KANASIBARI,
    WAZASICK_KAIHUKUHUUJI,  WAZASICK_ENCORE,    WAZASICK_TYOUHATSU,
  };

  if( idx < NELEMS(table) ){
    return table[idx];
  }
  return WAZASICK_NULL;
}

//=============================================================================================
/**
 * �^�[���`�F�b�N�������ɏ�Ԉُ�R�[�h�擾
 *
 * @param   idx   �������i0�`�j
 *
 * @retval  WazaSick    �L��idx�̎��A��Ԉُ�R�[�h�^����ȊO WAZASICK_NULL;
 */
//=============================================================================================
WazaSick  BTL_TABLES_GetTurnCheckWazaSickByOrder( u32 idx )
{
  static const WazaSick table[] = {

    // �񕜌n
    WAZASICK_AQUARING,        ///< �A�N�A�����O
    WAZASICK_NEWOHARU,        ///< �˂��͂�

    // �h���C���n
    WAZASICK_YADORIGI,        ///< ��ǂ肬�̂���

    // �_���[�W�n
    WAZASICK_DOKU,
    WAZASICK_YAKEDO,
    WAZASICK_AKUMU,
    WAZASICK_NOROI,
    WAZASICK_BIND,

    // ���ʐ؂�`�F�b�N�n
    WAZASICK_TYOUHATSU,
    WAZASICK_ICHAMON,
    WAZASICK_ENCORE,          ///< ���O�Ɏg�������U�����o���Ȃ�
    WAZASICK_KANASIBARI,
    WAZASICK_FLYING,          ///< �ł񂶂ӂ䂤
    WAZASICK_TELEKINESIS,     ///< �e���L�l�V�X
    WAZASICK_KAIHUKUHUUJI,    ///< �����ӂ��ӂ���
    WAZASICK_SASIOSAE,        ///< �����������i�񕜕������؂�Ă��獷�����������؂��ƁA�؂̎��Ƃ��ŉ񕜂ł��邩���j
    WAZASICK_MUSTHIT,         ///< ���U�K�����
    WAZASICK_MUSTHIT_TARGET,  ///< ���葊��ɑ΂��Ă̂ݕK�����

    // ������ʌn
    WAZASICK_AKUBI,

    // �ق�т̂���
    WAZASICK_HOROBINOUTA,     ///< �ق�т̂���

    // �^�[���`�F�b�N�����������͂��n
    WAZASICK_MAHI,
    WAZASICK_NEMURI,
    WAZASICK_KOORI,
    WAZASICK_MEROMERO,
    WAZASICK_KONRAN,
    WAZASICK_IEKI,            ///< �Ƃ�����������
    WAZASICK_MIYABURU,        ///< �݂�Ԃ��đ����[���ł����{�ŉ����違��𗦏㏸������
    WAZASICK_TOOSENBOU,       ///< �ɂ���E���ꂩ�����o���Ȃ�
    WAZASICK_HANEYASUME,      ///< �͂˂₷�߁i��s�^�C�v->���������j
    WAZASICK_WAZALOCK,        ///< ���O�Ɏg�������U�����o���Ȃ���A�s�����I���ł��Ȃ�
    WAZASICK_TAMELOCK,        ///< ���O�Ɏg�������U�����o���Ȃ���A�s�����I���ł��Ȃ��i���߃��U��p�j
    WAZASICK_KODAWARI,        ///< �ŏ��Ɏg�������U�����I���ł��Ȃ�
    WAZASICK_FLYING_CANCEL,   ///< �ӂ䂤��ԂɂȂ�Ȃ�
    WAZASICK_FREEFALL,        ///< �t���[�t�H�[��
    WAZASICK_HITRATIO_UP,     ///< �������㏸

  };

  if( idx < NELEMS(table) ){
    return table[idx];
  }
  return WAZASICK_NULL;
}

/**
 *  �h�����U���s����
 */
static BOOL checkHaseiOmmitCommon( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_YUBIWOHURU,    WAZANO_IKARINOKONA,   WAZANO_NEGOTO,
    WAZANO_NEKONOTE,      WAZANO_MANEKKO,       WAZANO_SAKIDORI,
    WAZANO_OUMUGAESI,     WAZANO_SIZENNOTIKARA, WAZANO_OSYABERI,
    WAZANO_WARUAGAKI,     WAZANO_SUKETTI,       WAZANO_MONOMANE,
  };

  return checkTableElems( waza, table, NELEMS(table) );
}

/**
 *  �u�˂��Ɓv�ŏo�Ȃ����U����
 */
BOOL BTL_TABLES_IsNegotoOmmit( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_SAWAGU,      WAZANO_ROKETTOZUTUKI, WAZANO_GODDOBAADO,
    WAZANO_SOORAABIIMU, WAZANO_GAMAN,         WAZANO_KAMAITATI,
    WAZANO_SORAWOTOBU,  WAZANO_TOBIHANERU,    WAZANO_SYADOODAIBU,
    WAZANO_KIAIPANTI,   WAZANO_HURIIFOORU,
  };

  if( checkHaseiOmmitCommon(waza) ){
    return TRUE;
  }

  return checkTableElems( waza, table, NELEMS(table) );
}

/**
 *  �u�l�R�̂āv�ŏo�Ȃ����U����
 */
BOOL BTL_TABLES_IsNekoNoteOmmit( WazaID waza )
{
  static const u16 table[] = {
      WAZANO_KAUNTAA,       WAZANO_MIRAAKOOTO,  WAZANO_MAMORU,      WAZANO_MIKIRI,
      WAZANO_KORAERU,       WAZANO_MITIDURE,    WAZANO_KONOYUBITOMARE,
      WAZANO_YOKODORI,      WAZANO_TEDASUKE,    WAZANO_DOROBOU,
      WAZANO_HOSIGARU,      WAZANO_TORIKKU,     WAZANO_SURIKAE,
      WAZANO_FEINTO,        WAZANO_KIAIPANTI,   WAZANO_HENSIN,
      WAZANO_DORAGONTEERU,  WAZANO_TOMOENAGE,   // ���̓�́u�������炲����v�Ƃ̑g�ݍ��킹�Ń��o�����Ȃ̂�
  };

  if( checkHaseiOmmitCommon(waza) ){
    return TRUE;
  }

  return checkTableElems( waza, table, NELEMS(table) );
}

/**
 *  �u�܂˂����v�ŏo�Ȃ����U����
 */
BOOL BTL_TABLES_IsManekkoOmmit( WazaID waza )
{
  static const u16 table[] = {
      WAZANO_KAUNTAA,       WAZANO_MIRAAKOOTO,  WAZANO_MAMORU,      WAZANO_MIKIRI,
      WAZANO_KORAERU,       WAZANO_MITIDURE,    WAZANO_KONOYUBITOMARE,
      WAZANO_YOKODORI,      WAZANO_TEDASUKE,    WAZANO_DOROBOU,
      WAZANO_HOSIGARU,      WAZANO_TORIKKU,     WAZANO_SURIKAE,
      WAZANO_FEINTO,        WAZANO_KIAIPANTI,   WAZANO_HENSIN,
      WAZANO_DORAGONTEERU,  WAZANO_TOMOENAGE,   // ���̓�́u�������炲����v�Ƃ̑g�ݍ��킹�Ń��o�����Ȃ̂�
  };

  if( checkHaseiOmmitCommon(waza) ){
    return TRUE;
  }

  return checkTableElems( waza, table, NELEMS(table) );
}

/**
 *  ���̂܂ˎ��s�Ώۂ̃��U����
 */
BOOL BTL_TABLES_IsMatchMonomaneFail( WazaID waza )
{
  static const u16 table[] = {
    WAZANO_NULL,    WAZANO_SUKETTI,   WAZANO_MONOMANE,
    WAZANO_HENSIN,  WAZANO_WARUAGAKI, WAZANO_YUBIWOHURU,
    WAZANO_OSYABERI
  };

  return checkTableElems( waza, table, NELEMS(table) );
}


//--------------------------------------------------------------------------
/**
 *  ��т��ӂ�ŏo�Ȃ����U�e�[�u��
 */
//--------------------------------------------------------------------------
static const WazaID YubiFuruOmmit[] = {
  WAZANO_YUBIWOHURU,    WAZANO_NEGOTO,      WAZANO_NEKONOTE,
  WAZANO_MANEKKO,       WAZANO_SAKIDORI,    WAZANO_OUMUGAESI,
  WAZANO_SIZENNOTIKARA, WAZANO_OSYABERI,    WAZANO_WARUAGAKI,

  WAZANO_SUKETTI,       WAZANO_MONOMANE,    WAZANO_KAUNTAA,
  WAZANO_MIRAAKOOTO,    WAZANO_MAMORU,      WAZANO_MIKIRI,
  WAZANO_KORAERU,       WAZANO_MITIDURE,    WAZANO_KONOYUBITOMARE,
  WAZANO_YOKODORI,      WAZANO_TEDASUKE,    WAZANO_DOROBOU,
  WAZANO_HOSIGARU,      WAZANO_TORIKKU,     WAZANO_SURIKAE,
  WAZANO_FEINTO,        WAZANO_KIAIPANTI,   WAZANO_HENSIN,

  WAZANO_TEKUNOBASUTAA, WAZANO_INISIENOUTA,  WAZANO_SINPINOTURUGI,
  WAZANO_HURIIZUBORUTO, WAZANO_KOORUDOHUREA, WAZANO_BAAKUAUTO,
  WAZANO_vJENEREETO,    WAZANO_IBIKI,        WAZANO_FASUTOGAADO,
  WAZANO_WAIDOGAADO,    WAZANO_OSAKINIDOUZO, WAZANO_SAKIOKURI,
};

//=============================================================================================
/**
 * ��т��ӂ�o�Ȃ����U�e�[�u�����擾
 *
 * @param   elems   [out] �e�[�u���v�f��
 *
 * @retval  const WazaID*   �e�[�u���A�h���X
 */
//=============================================================================================
const WazaID* BTL_TABLES_GetYubiFuruOmmitTable( u32* elems )
{
  *elems = NELEMS(YubiFuruOmmit);
  return YubiFuruOmmit;
}
//=============================================================================================
/**
 * ��т��ӂ�ŏo�Ȃ����U������
 *
 * @param   waza
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_TABLES_IsYubiFuruOmmit( WazaID waza )
{
  u32 i;
  for(i=0; i<NELEMS(YubiFuruOmmit); ++i)
  {
    if( YubiFuruOmmit[i] == waza ){
      return TRUE;
    }
  }
  return FALSE;
}

/*--------------------------------------------------------------------------------------------*/
/* �f�o�b�O�p�u��т��ӂ�v�R���g���[��                                                       */
/*--------------------------------------------------------------------------------------------*/

#ifdef PM_DEBUG

/**
 *  �f�o�b�O�u��т��ӂ�e�[�u���i�O���[�o���j�v
 */
int GYubiFuruDebugNumber[ BTL_POS_MAX ];

/**
 *  �u��т��ӂ�e�[�u���v���Z�b�g
 */
void BTL_TABLES_YubifuruDebugReset( void )
{
  u32 i;
  for(i=0; i<NELEMS(GYubiFuruDebugNumber); ++i){
    GYubiFuruDebugNumber[i] = 0;
  }
}
/**
 *  �O������K���ɑ��삵���u��т��ӂ�e�[�u���v�ɋ֎~���U���ݒ肳��Ă�����␳
 */
void BTL_TABLES_YubifuruDebugSetEnd( void )
{
  u32 i;
  for(i=0; i<NELEMS(GYubiFuruDebugNumber); ++i){
    if( BTL_TABLES_IsYubiFuruOmmit(GYubiFuruDebugNumber[i]) )
    {
      BTL_TABLES_YubifuruDebugInc(i);
    }
  }
}
/**
 *  �u��т��ӂ�e�[�u���v�̃i���o�[���C���N�������g�i�֎~���U�̓X�L�b�v�j
 */
void BTL_TABLES_YubifuruDebugInc( u8 pos )
{
  while(1)
  {
    GYubiFuruDebugNumber[pos]++;
    if( GYubiFuruDebugNumber[pos] >= WAZANO_MAX )
    {
      GYubiFuruDebugNumber[pos] = 0;
    }
    if( !BTL_TABLES_IsYubiFuruOmmit( GYubiFuruDebugNumber[pos] ) ){
      break;
    }
  };
}

#endif
