//============================================================================================
/**
 * @file  sheimi_normalform.c
 * @bfief �V�F�C�~���ԉz���t�H�����߂�
 * @author  Saito
 */
//============================================================================================
#include <gflib.h>

#include "sheimi_normalform.h"
#include "poke_tool/monsno_def.h" //for MONSNO_SHEIMI FORMNO_�`
#include "system/timezone.h"    //for TIMEZONE_�`
#include "system/rtc_tool.h"  //for PM_RTC_GetTimeZoneChangeHour

//--------------------------------------------------------------
/**
 * @brief   POKEPARTY�̃V�F�C�~��S�ăm�[�}���t�H�����ɂ���
 *
 * @param   gdata �Q�[���f�[�^�|�C���^
 * @param   ppt		POKEPARTY�ւ̃|�C���^
 */
//--------------------------------------------------------------
void SHEIMI_NFORM_ChangeNormal(GAMEDATA * gdata, POKEPARTY *ppt)
{
	int pos, count, monsno, form_no;
	POKEMON_PARAM *pp;
	int set_form_no = FORMNO_SHEIMI_LAND;
  ZUKAN_SAVEDATA *zw = GAMEDATA_GetZukanSave( gdata );
  BOOL see = FALSE;

	count = PokeParty_GetPokeCount(ppt);
	for(pos = 0; pos < count; pos++)
  {
		pp = PokeParty_GetMemberPointer(ppt,pos);
		monsno = PP_Get(pp, ID_PARA_monsno, NULL);
		form_no = PP_Get(pp, ID_PARA_form_no, NULL);
		if(monsno == MONSNO_SHEIMI && form_no == FORMNO_SHEIMI_SKY)
    {
      PP_ChangeFormNo( pp, set_form_no );
      //�}�ӓo�^�u�����v
      if (!see)
      {
        ZUKANSAVE_SetPokeSee(zw, pp);
        see = TRUE;
      }
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   ��������ɕς�����^�C�~���O�Ȃ�΁A�莝���̃V�F�C�~��S�ăm�[�}���t�H�����ɂ���
 *
 * @param   gdata     �Q�[���f�[�^�|�C���^
 * @param   ppt				POKEPARTY�ւ̃|�C���^
 * @param   min_diff		�o�߂�������(���P��)
 * @param   now				���ݎ���
 * @parm    inSeason    ���݋G��
 *
 * @retval  TRUE:�m�[�}���t�H�����ւ̕ύX���s����
 */
//--------------------------------------------------------------
BOOL SHEIMI_NFORM_ChangeNormal_TimeUpdate(GAMEDATA * gdata, POKEPARTY *ppt, int min_diff, const RTCTime * now, int inSeason)
{
	s32 hour, min_pos;

  int night,morning;

  night = PM_RTC_GetTimeZoneChangeHour( inSeason, TIMEZONE_NIGHT );
  morning = PM_RTC_GetTimeZoneChangeHour( inSeason, TIMEZONE_MORNING );
	
	//���݂�NG�����̏ꍇ(�t�̂Ƃ�20:00 �` 27:59  night�`morning)
	if(now->hour >= night || now->hour < morning){
		hour = now->hour;
		if(hour < morning){
			hour += 24;		//��)�[��2���Ȃ��26���ɕϊ�����
		}
		hour -= night;	//nihgt������̃I�t�Z�b�g�ɂ���
		min_pos = now->minute + hour * 60;	//night�����牽���o�߂��Ă��邩

		min_diff++;	//�b���̒[�����l�����Đ؂�グ(�[���������̂�1/60�Ȃ̂Ńf�t�H�Ő؂�グ

		if(min_pos < min_diff){
			//OS_TPrintf("�V�F�C�~ form �X�V\n");
			SHEIMI_NFORM_ChangeNormal(gdata, ppt);
			return TRUE;
		}
		return FALSE;
	}
	else{
	//���݂�OK�����̏ꍇ(�t�̂Ƃ�4:00 �` 19:59 morning�`night)��min_diff��NG�������܂����ł��Ȃ����`�F�b�N
		min_pos = now->minute + (now->hour - morning) * 60;	//morning�����牽���o�߂��Ă��邩
		if(min_pos < min_diff){
			//OS_TPrintf("�V�F�C�~ form �X�V\n");
			SHEIMI_NFORM_ChangeNormal(gdata, ppt);
			return TRUE;
		}
		return FALSE;
	}
}

