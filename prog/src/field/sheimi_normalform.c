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

//--------------------------------------------------------------
/**
 * @brief   POKEPARTY�̃V�F�C�~��S�ăm�[�}���t�H�����ɂ���
 *
 * @param   ppt		POKEPARTY�ւ̃|�C���^
 */
//--------------------------------------------------------------
void SHEIMI_NFORM_ChangeNormal(POKEPARTY *ppt)
{
	int pos, count, monsno, form_no;
	POKEMON_PARAM *pp;
	int set_form_no = FORMNO_SHEIMI_LAND;
	
	count = PokeParty_GetPokeCount(ppt);
	for(pos = 0; pos < count; pos++)
  {
		pp = PokeParty_GetMemberPointer(ppt,pos);
		monsno = PP_Get(pp, ID_PARA_monsno, NULL);
		form_no = PP_Get(pp, ID_PARA_form_no, NULL);
		if(monsno == MONSNO_SHEIMI && form_no == FORMNO_SHEIMI_SKY)
    {
      PP_ChangeFormNo( pp, set_form_no );
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   ��������ɕς�����^�C�~���O�Ȃ�΁A�莝���̃V�F�C�~��S�ăm�[�}���t�H�����ɂ���
 *
 * @param   ppt				POKEPARTY�ւ̃|�C���^
 * @param   min_diff		�o�߂�������(���P��)
 * @param   now				���ݎ���
 *
 * @retval  TRUE:�m�[�}���t�H�����ւ̕ύX���s����
 */
//--------------------------------------------------------------
BOOL SHEIMI_NFORM_ChangeNormal_TimeUpdate(POKEPARTY *ppt, int min_diff, const RTCTime * now)
{
	s32 hour, min_pos;
	
	//���݂�NG�����̏ꍇ(20:00 �` 27:59)
	if(now->hour >= 20 || now->hour < 4){
		hour = now->hour;
		if(hour < 4){
			hour += 24;		//��)�[��2���Ȃ��26���ɕϊ�����
		}
		hour -= 20;	//20������̃I�t�Z�b�g�ɂ���
		min_pos = now->minute + hour * 60;	//20�����牽���o�߂��Ă��邩

		min_diff++;	//�b���̒[�����l�����Đ؂�グ(�[���������̂�1/60�Ȃ̂Ńf�t�H�Ő؂�グ

		if(min_pos < min_diff){
			//OS_TPrintf("�V�F�C�~ form �X�V\n");
			SHEIMI_NFORM_ChangeNormal(ppt);
			return TRUE;
		}
		return FALSE;
	}
	else{
	//���݂�OK�����̏ꍇ(4:00 �` 19:59)��min_diff��NG�������܂����ł��Ȃ����`�F�b�N
		min_pos = now->minute + (now->hour - 4) * 60;	//4�����牽���o�߂��Ă��邩
		if(min_pos < min_diff){
			//OS_TPrintf("�V�F�C�~ form �X�V\n");
			SHEIMI_NFORM_ChangeNormal(ppt);
			return TRUE;
		}
		return FALSE;
	}
}

