//============================================================================================
/**
 * @file    mystery_createdata.c
 * @brief   �s�v�c�ʐM�f�[�^����Q�[���p�f�[�^�����֐�
             @todo RC4��������

 * @author  k.ohno
 * @date    10.01.27
 */
//============================================================================================


#include <gflib.h>
#include "gamesystem/game_data.h"

#include "poke_tool/poke_tool.h"
#include "item/itemsym.h"
#include "waza_tool/wazano_def.h"
#include "savedata/mystery_data.h"
#include "poke_tool/poke_memo.h"

#include "net_app/mystery.h"
#include "system/gfl_use.h"

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

POKEMON_PARAM* MYSTERY_CreatePokemon(const GIFT_PACK_DATA* pPack, HEAPID heapID, GAMEDATA* pGameData)
{
  POKEMON_PARAM* pp;
  const GIFT_PRESENT_POKEMON* pGift= &pPack->data.pokemon;
  u16 level=pGift->level;
  u32 id = pGift->id_no;
  u32 monsno = pGift->mons_no;
  PtlSetupPow pow=0;
  u64 rand;
  u32 buff;
  u8 tokusei = pGift->speabino & 0x7f;
  BOOL tokuseiBit = (pGift->speabino & 0x80) ? TRUE : FALSE;

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
    level = GFUser_GetPublicRand(99)+1;
  }
  if(pGift->id_no == 0){  //ID�O�w��͎������e
    id = MyStatus_GetID(GAMEDATA_GetMyStatus(pGameData));
  }



  pp = PP_Create( monsno, level, id, heapID );

  pow = PTL_SETUP_POW_PACK(pGift->hp_rnd,pGift->pow_rnd,
                           pGift->def_rnd,
                           pGift->spepow_rnd,pGift->spedef_rnd,pGift->agi_rnd);


  PP_Put(pp, ID_PARA_speabino, pGift->speabino);  

  if(pow==0){
    pow = PTL_SETUP_POW_AUTO;
  }
  if(pGift->rnd != 0){
    rand = pGift->rnd;
  }
  else if(pGift->rare == 0){
    rand = POKETOOL_CalcPersonalRandEx( id, monsno, pGift->form_no, pGift->sex, tokuseiBit, FALSE);
  }
  else if(pGift->rare == 1){
    rand = PTL_SETUP_ID_AUTO;
  }
  else if(pGift->rare == 2){
    rand = POKETOOL_CalcPersonalRandEx( id, monsno, pGift->form_no, pGift->sex, tokuseiBit, TRUE);
  }

  PP_SetupEx( pp, pGift->mons_no, level, id, pow, rand );

  PP_Put(pp, ID_PARA_item, pGift->item);

  if(pGift->waza1!=0 && pGift->waza1<WAZANO_MAX){
    PP_SetWazaPush(pp, pGift->waza1);
  }
  if(pGift->waza2!=0 && pGift->waza2<WAZANO_MAX){
    PP_SetWazaPush(pp, pGift->waza2);
  }
  if(pGift->waza3!=0 && pGift->waza3<WAZANO_MAX){
    PP_SetWazaPush(pp, pGift->waza3);
  }
  if(pGift->waza4!=0 && pGift->waza4<WAZANO_MAX){
    PP_SetWazaPush(pp, pGift->waza4);
  }
  PP_Put(pp, ID_PARA_form_no, pGift->form_no);  //�t�H�����ԍ��͂��ׂċ���
  
  if(pGift->get_ball!=0){
    PP_Put(pp, ID_PARA_get_ball, pGift->get_ball);
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


  if(pGift->version!=0){
    PP_Put(pp, ID_PARA_get_cassette, pGift->version);
  }
    
  PP_Put(pp, ID_PARA_country_code, pGift->country_code);  

  if( pGift->nickname[0]==0){ 
    GFL_HEAP_FreeMemory(pp);
    return NULL;
  }
  else if(pGift->nickname[0]!= GFL_STR_GetEOMCode()){
    OS_TPrintf( "�j�b�N�l�[���擪�@%d\n", pGift->nickname[0] );
    PP_Put(pp, ID_PARA_nickname_raw, (u32)pGift->nickname);
  }
  

  PP_Put(pp, ID_PARA_seikaku, pGift->seikaku);  
  PP_Put(pp, ID_PARA_get_place, pGift->get_place);  
  PP_Put(pp, ID_PARA_birth_place, pGift->birth_place);  

  if( pGift->oyaname[0]==0){ 
    GFL_HEAP_FreeMemory(pp);
    return NULL;
  }
  if(pGift->oyaname[0]!= GFL_STR_GetEOMCode()){
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

    POKE_MEMO_SetTrainerMemoPokeDistribution( PP_GetPPPPointer(pp) , place , year , month , day );
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
int MYSTERY_CreateItem(const GIFT_PACK_DATA* pPack)
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
 * @retval @todo����
 */
//--------------------------------------------------------------
int MYSTERY_CreateGPower(const GIFT_PACK_DATA* pPack)
{
  const GIFT_PRESENT_POWER* pGift = &pPack->data.gpower;

  if(pPack->gift_type != MYSTERYGIFT_TYPE_POWER){
    return 0;
  }
  return pGift->type;
}
