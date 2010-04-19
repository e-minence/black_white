//============================================================================================
/**
 * @file  seasonpoke_form.c
 * @bfief �G�߃|�P�����t�H�[�����`�F���W
 * @author  Saito
 */
//============================================================================================
#include <gflib.h>

#include "seasonpoke_form.h"
#include "gamesystem/pm_season.h"
#include "poke_tool/monsnum_def.h"  // for MONSNO_����

//--------------------------------------------------------------
/**
 * @brief   �莝���G�߃|�P�������w�肵���G�߂̃t�H�����ɂ���
 *
 * @param   gdata   �Q�[���f�[�^�|�C���^
 * @param   ppt		POKEPARTY�ւ̃|�C���^
 * @param   inSeason    �G�߂h�c
 */
//--------------------------------------------------------------
void SEASONPOKE_FORM_ChangeForm(GAMEDATA * gdata, POKEPARTY *ppt, const u8 inSeason)
{
	int pos, count, monsno;
	POKEMON_PARAM *pp;
	int set_form_no_poke511;
  int set_form_no_poke527;
  BOOL see511, see527;

  ZUKAN_SAVEDATA *zw = GAMEDATA_GetZukanSave( gdata );
  see511 = FALSE;
  see527 = FALSE;

  //�w��l�G�ŕ���
  switch(inSeason){
  case PMSEASON_SPRING:
    set_form_no_poke511 = FORMNO_511_SPRING;
    set_form_no_poke527 = FORMNO_527_SPRING;
    break;
  case PMSEASON_SUMMER:
    set_form_no_poke511 = FORMNO_511_SUMMER;
    set_form_no_poke527 = FORMNO_527_SUMMER;
    break;
  case PMSEASON_AUTUMN:
    set_form_no_poke511 = FORMNO_511_AUTUMN;
    set_form_no_poke527 = FORMNO_527_AUTUMN;
    break;
  case PMSEASON_WINTER:
    set_form_no_poke511 = FORMNO_511_WINTER;
    set_form_no_poke527 = FORMNO_527_WINTER;
    break;
  default:
    GF_ASSERT_MSG(0,"season error %d",inSeason);
    set_form_no_poke511 = FORMNO_511_SPRING;
    set_form_no_poke527 = FORMNO_527_SPRING;
  }
	
	count = PokeParty_GetPokeCount(ppt);
	for(pos = 0; pos < count; pos++)
  {
		pp = PokeParty_GetMemberPointer(ppt,pos);
		monsno = PP_Get(pp, ID_PARA_monsno, NULL);
		if ( monsno == MONSNO_511 )
    {
      PP_ChangeFormNo( pp, set_form_no_poke511 );
      //�}�ӓo�^�u�����v
      if (!see511)
      {
        ZUKANSAVE_SetPokeSee(zw, pp);
        see511 = TRUE;
      }
    }
    else if ( monsno == MONSNO_527 ){
      PP_ChangeFormNo( pp, set_form_no_poke527 );
      //�}�ӓo�^�u�����v
      if (!see527)
      {
        ZUKANSAVE_SetPokeSee(zw, pp);
        see527 = TRUE;
      }
    }
	}
}

