//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_inline.h
 *	@brief  �C�����C���֐�
 *	@author	Toru=Nagihashi
 *	@date		2010.04.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <gflib.h>
#include "gamesystem/game_data.h"

#include "poke_tool/poke_tool.h"
#include "item/itemsym.h"
#include "waza_tool/wazano_def.h"
#include "savedata/mystery_data.h"
#include "poke_tool/poke_memo.h"

#include "system/gfl_use.h"
#include "print/global_msg.h"

#include "net_app/gts_tool.h"

//--------------------------------------------------------------
/**
 * @brief  �s�v�c�f�[�^����̃|�P�����쐬
 * @param  GIFT_PACK_DATA   �Z�[�u�f�[�^�Ɋi�[����Ă�s�v�c�f�[�^
 * @param  HEAPID                 POKEMON_PARAM������HEAPID
 * @param  GAMEDATA 
 * @retval POKEMON_PARAM  ���s������NULL �I������J�����Ă�������
 */
//--------------------------------------------------------------

static inline POKEMON_PARAM* Mystery_CreatePokemon(const GIFT_PACK_DATA* pPack, HEAPID heapID, GAMEDATA* pGameData)
{
  enum
  { 
    RND_TBL_HP,
    RND_TBL_POW,
    RND_TBL_DEF,
    RND_TBL_SPEPOW,
    RND_TBL_SPEDEF,
    RND_TBL_AGI,
    RND_TBL_MAX,
  };

  POKEMON_PARAM* pp;
  const GIFT_PRESENT_POKEMON* pGift= &pPack->data.pokemon;
  u16 level=pGift->level;
  u32 id = pGift->id_no;
  u32 monsno = pGift->mons_no;
  PtlSetupPow pow=0;
  u64 rand;
  u32 buff;
  u8  sex = pGift->sex;
  u8 tokusei = pGift->speabino;
  PtlTokuseiSpec  tokuseiSpec;

  u8 rnd_tbl[ RND_TBL_MAX ];
  u16 waza_tbl[ PTL_WAZA_MAX ];

  //for���ŏ��������ʉ������邽�߂�
  //�e�[�u���ɓ����
  rnd_tbl[RND_TBL_HP] = pGift->hp_rnd;
  rnd_tbl[RND_TBL_POW]  = pGift->pow_rnd;
  rnd_tbl[RND_TBL_DEF]  = pGift->def_rnd;
  rnd_tbl[RND_TBL_SPEPOW] = pGift->spepow_rnd;
  rnd_tbl[RND_TBL_SPEDEF] = pGift->spedef_rnd;
  rnd_tbl[RND_TBL_AGI]  = pGift->agi_rnd;
  //���Ɠ���
  waza_tbl[ 0 ] = pGift->waza1;
  waza_tbl[ 1 ] = pGift->waza2;
  waza_tbl[ 2 ] = pGift->waza3;
  waza_tbl[ 3 ] = pGift->waza4;

  if(pPack->gift_type != MYSTERYGIFT_TYPE_POKEMON){
    return NULL;
  }
  
  if(pGift->mons_no > MONSNO_END){  //�����X�^�[�ԍ������݂��Ȃ����͍��Ȃ�
    return NULL;
  }
  if(pGift->mons_no == 0){  //�����X�^�[�ԍ������݂��Ȃ����͍��Ȃ�
    return NULL;
  }
  if(pGift->level > 100){  //���x��100����͍��Ȃ�
    return NULL;
  }
  if(pGift->item > ITEM_DATA_MAX){  //�A�C�e���ԍ��ԈႢ
    return NULL;
  }

  
  
  if(pGift->level == 0){  //���x���O�w��͗���
    level = GFUser_GetPublicRand(100)+1;
  }
  if(pGift->id_no == 0){  //ID�O�w��͎������e
    id = MyStatus_GetID(GAMEDATA_GetMyStatus(pGameData));
  }
  if( sex == 0xFF )
  { 
    //POKETOOL_CalcPersonalRandSpec�̎d�l�ŁA�Y��������ꍇ�APTL_SEX_UNKNOWN���w�肷��ƃ����_��
    sex = PTL_SEX_UNKNOWN;
  }

  //�̒l
  {
    int i;
    for( i = 0; i < RND_TBL_MAX; i++ )
    { 
      if( rnd_tbl[i] == 0xFF )
      { 
        rnd_tbl[i]  = GFUser_GetPublicRand(32);
      }
    }
  }
  pp = PP_Create( monsno, level, id, heapID );

  pow = PTL_SETUP_POW_PACK(
      rnd_tbl[ RND_TBL_HP ],rnd_tbl[ RND_TBL_POW ],rnd_tbl[ RND_TBL_DEF ],
      rnd_tbl[ RND_TBL_SPEPOW ],rnd_tbl[ RND_TBL_SPEDEF ],rnd_tbl[ RND_TBL_AGI ] );


  if( tokusei == 4) //4�͓���1or2or3
  { 
    tokusei = GFUser_GetPublicRand(3);
  }
  switch( tokusei )
  { 
  case 0: //�����P
    tokuseiSpec = PTL_TOKUSEI_SPEC_1;
    break;
  case 1: //�����Q
    tokuseiSpec = PTL_TOKUSEI_SPEC_2;
    break;
  case 2: //�����R
    tokuseiSpec = PTL_TOKUSEI_SPEC_1; //pp�쐬��ݒ肷��̂�POKETOOL_CalcPersonalRandSpec�ɓn���l�͉��ł��悢
    break;
  case 3: //�����Por�Q
    tokuseiSpec = PTL_TOKUSEI_SPEC_BOTH;
    break;

  default:
    return NULL;

  }

  //�̗������ݒ肵�Ă���΁A���̒l���w�肵���������ʓ��͖����������͂��Ȃ̂ŁA
  //���ڐݒ肷��
  if(pGift->rnd != 0){
    rand = pGift->rnd;
  }
  else
  { 
    //�̗������ݒ肳��Ă��Ȃ��Ƃ������A�ȉ��̐ݒ���s�Ȃ�
    if(pGift->rare == 0){
      rand = POKETOOL_CalcPersonalRandSpec( id, monsno, pGift->form_no, sex, tokuseiSpec, PTL_RARE_SPEC_FALSE );
    }
    else if(pGift->rare == 1){
      rand = POKETOOL_CalcPersonalRandSpec( id, monsno, pGift->form_no, sex, tokuseiSpec, PTL_RARE_SPEC_BOTH );
    }
    else if(pGift->rare == 2){
      rand = POKETOOL_CalcPersonalRandSpec( id, monsno, pGift->form_no, sex, tokuseiSpec, PTL_RARE_SPEC_TRUE );
    }
  }

  PP_SetupEx( pp, pGift->mons_no, level, id, pow, rand );

  PP_Put(pp, ID_PARA_item, pGift->item);

  //��U�f�t�H���g�Z��ݒ肵�A�Z�ݒ�
  //�Z�͉��o���Ɋo����
  {
    int i;
    PP_SetWazaDefault( pp );

    for( i = 0; i < PTL_WAZA_MAX; i++ )
    { 
      if( waza_tbl[i] != 0 && waza_tbl[i] < WAZANO_MAX )
      { 
        if( PTL_WAZASET_FAIL == PP_SetWaza( pp, waza_tbl[i] ) )
        { 
          PP_SetWazaPush( pp, waza_tbl[i] );
        }
      }
    }
  }

  //�����_�����ڎw��ł͂Ȃ��Ƃ��̂ݓ������w�肷��
  if(pGift->rnd == 0)
  {
    if( tokusei == 2 )
    { 
      PP_SetTokusei3( pp, monsno, pGift->form_no );
    }
  }

  PP_Put(pp, ID_PARA_form_no, pGift->form_no);  //�t�H�����ԍ��͂��ׂċ���
  
  if(pGift->get_ball!=0){
    PP_Put(pp, ID_PARA_get_ball, pGift->get_ball);
  }
  else
  { 
    PP_Put(pp, ID_PARA_get_ball, ITEM_MONSUTAABOORU );
  }

  if(pGift->get_level!=0){
    PP_Put(pp, ID_PARA_get_level, pGift->get_level);  
  }

  PP_Put(pp, ID_PARA_style, pGift->style);  
  PP_Put(pp, ID_PARA_beautiful, pGift->beautiful);  
  PP_Put(pp, ID_PARA_cute, pGift->cute);  
  PP_Put(pp, ID_PARA_clever, pGift->clever);  
  PP_Put(pp, ID_PARA_strong, pGift->strong);  
  PP_Put(pp, ID_PARA_fur, pGift->strong);  

  //���{���ݒ�
  { 
    int i;
    u32 id_para_ribbon;
    for( i = 0; i < GTS_TOOL_GetDistributeRibbonMax(); i++ )
    { 
      if( (pGift->ribbon_no & (1<<i)) != 0 )
      { 
        id_para_ribbon  = GTS_TOOL_GetDistributeRibbon( i );
        PP_Put(pp, id_para_ribbon, TRUE );  
      }
    }
  }


  //�o�[�W�����w�肪�O�Ȃ�Ύ�����ROM
  if(pGift->version==0){
    PP_Put(pp, ID_PARA_get_cassette, CasetteVersion );
  }
  else
  { 
    PP_Put(pp, ID_PARA_get_cassette, pGift->version);
  }
   
  if( pGift->country_code == 0)
  { 
    PP_Put(pp, ID_PARA_country_code, CasetteLanguage );  
  }
  else
  { 
    PP_Put(pp, ID_PARA_country_code, pGift->country_code);  
  }

  if( pGift->nickname[0]==0){ 
    GFL_HEAP_FreeMemory(pp);
    return NULL;
  }
  else if(pGift->nickname[0]!= GFL_STR_GetEOMCode()){
    OS_TPrintf( "�j�b�N�l�[���擪�@%d\n", pGift->nickname[0] );
    PP_Put(pp, ID_PARA_nickname_raw, (u32)pGift->nickname);
  }

  { 
    u8 seikaku = pGift->seikaku;
    if( seikaku == 0xFF )
    { 
      seikaku = GFUser_GetPublicRand(PTL_SEIKAKU_MAX);
    }
    PP_Put(pp, ID_PARA_seikaku, seikaku);  
  }
  PP_Put(pp, ID_PARA_get_place, pGift->get_place);  
  PP_Put(pp, ID_PARA_birth_place, pGift->birth_place);  

  if( pGift->oyaname[0]==0){ 
    GFL_HEAP_FreeMemory(pp);
    return NULL;
  }
  else if( pGift->oyaname[0] == GFL_STR_GetEOMCode())
  {
    PP_Put(pp, ID_PARA_oyaname_raw, (u32)MyStatus_GetMyName(GAMEDATA_GetMyStatus(pGameData)));
  }
  else
  {
    PP_Put(pp, ID_PARA_oyaname_raw, (u32)pGift->oyaname);
  }
  if(pGift->oyasex <= PM_NEUTRAL){
    PP_Put(pp, ID_PARA_oyasex, pGift->oyasex);
  }
  else{
    PP_Put(pp, ID_PARA_oyasex, MyStatus_GetMySex(GAMEDATA_GetMyStatus(pGameData)));
  }

  if(pGift->egg){       //�^�}�S���ǂ��� TRUE�����܂�
    PP_Put(pp, ID_PARA_tamago_flag, TRUE);

    //�^�}�S�̏ꍇ�A�j�b�N�l�[���Ɂu�^�}�S�v�Ɠ����
    {
      STRBUF  *p_strbuf = GFL_MSG_CreateString( GlobalMsg_PokeName, MONSNO_TAMAGO );
      PP_Put(pp, ID_PARA_nickname, (u32)p_strbuf );
      GFL_STR_DeleteBuffer( p_strbuf );
    }
  }

  //�z�z�|�P�����t���O�{�g���[�i�[����
  { 
    const u32 year    = MYSTERYDATA_GetYear( pPack->recv_date );
    const u32 month   = MYSTERYDATA_GetMonth( pPack->recv_date );
    const u32 day     = MYSTERYDATA_GetDay( pPack->recv_date );
    u32 place;

    //�^�}�S�Ȃ��GetPlace
    //�|�P�����Ȃ��BirthPlace
    if( pGift->egg )
    {
      place = pGift->get_place;
    }
    else
    { 
      place = pGift->birth_place;
    }

    POKE_MEMO_SetTrainerMemoPokeDistribution( PP_GetPPPPointer(pp) , place , year-2000, month , day );
  }

  PP_Renew( pp );

  if(PP_Get(pp,ID_PARA_fusei_tamago_flag, NULL)){  //�s���|�P�������o���Ă��܂����ꍇ
    GFL_HEAP_FreeMemory(pp);
    return NULL;
  }

  return pp;
}



//--------------------------------------------------------------
/**
 * @brief  �s�v�c�f�[�^����̃A�C�e���쐬
 * @param  GIFT_PACK_DATA   �Z�[�u�f�[�^�Ɋi�[����Ă�s�v�c�f�[�^
 * @retval itemsym.h�̃A�C�e���ԍ� �s���̏ꍇ ITEM_DUMMY_DATA
 */
//--------------------------------------------------------------
static inline int Mystery_CreateItem(const GIFT_PACK_DATA* pPack)
{
  const GIFT_PRESENT_ITEM* pGift =  &pPack->data.item;

  if(pPack->gift_type != MYSTERYGIFT_TYPE_ITEM){
    return ITEM_DUMMY_DATA;
  }

  return pGift->itemNo;
}

//--------------------------------------------------------------
/**
 * @brief  �s�v�c�f�[�^�����GPower�쐬
 * @param  GIFT_PACK_DATA   �Z�[�u�f�[�^�Ɋi�[����Ă�s�v�c�f�[�^
 * @retval GPowerID
 */
//--------------------------------------------------------------
static inline int Mystery_CreateGPower(const GIFT_PACK_DATA* pPack)
{
  const GIFT_PRESENT_POWER* pGift = &pPack->data.gpower;

  if(pPack->gift_type != MYSTERYGIFT_TYPE_POWER){
    return GPOWER_ID_NULL;
  }

  //�z�z�f�[�^�ȊO��������s��
  if( GPOWER_ID_EGG_INC_S <= pGift->type && pGift->type <= GPOWER_ID_CAPTURE_S )
  {
    return pGift->type;
  }
  else
  {
    return GPOWER_ID_NULL;
  }
}
