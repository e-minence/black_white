//============================================================================================
/**
 * @file  straw_poke_form.c
 * @bfief ���|�P�����i�~�m���b�`�j�t�H�[�����`�F���W
 * @author  Saito
 */
//============================================================================================
#include <gflib.h>

#include "straw_poke_form.h"
#include "poke_tool/monsno_def.h"  // for MONSNO_�����X�^�[��
#include "savedata/zukan_savedata.h"

#include "../../resource/battle/batt_bg_tbl/batt_bg_tbl.h"

#define BATT_BG_OBONID_MAX  (25)

const u16 StrawFromTbl[BATT_BG_OBONID_MAX][2] = {
  { BATT_BG_OBONID_00,FORMNO_412_SUNA},		    //�n�ʁi�l�G�Ȃ��j            ��
  { BATT_BG_OBONID_01,FORMNO_412_SUNA},				//�n�ʁi�l�G����j            ��
  { BATT_BG_OBONID_02,FORMNO_412_GOMI},				//���i���O�j                  �S�~
	{ BATT_BG_OBONID_03,FORMNO_412_GOMI},				//���i�����j                  �S�~
	{ BATT_BG_OBONID_04,FORMNO_412_SUNA},				//���A                        ��
	{ BATT_BG_OBONID_05,FORMNO_412_KUSA},				//�Ő��i�l�G�Ȃ��j            ��
	{ BATT_BG_OBONID_06,FORMNO_412_KUSA},				//�Ő��i�l�G����j            ��
	{ BATT_BG_OBONID_07,FORMNO_412_SUNA},				//�R��                        ��
	{ BATT_BG_OBONID_08,FORMNO_412_KUSA},				//����i���O�j                ��
	{ BATT_BG_OBONID_09,FORMNO_412_KUSA},				//����i�����j                ��
	{ BATT_BG_OBONID_10,FORMNO_412_SUNA},				//�����i���O�j                ��
	{ BATT_BG_OBONID_11,FORMNO_412_SUNA},				//�����i�����j�@�@�@�@�@�@    ��
	{ BATT_BG_OBONID_12,FORMNO_412_KUSA},				//�󂢎��n                    ��
	{ BATT_BG_OBONID_13,FORMNO_412_KUSA},				//�X��i�����j                ��
	{ BATT_BG_OBONID_14,FORMNO_412_GOMI},       //�p���X��p                  �S�~
	{ BATT_BG_OBONID_15,FORMNO_412_GOMI},				//�l�V���i�S�[�X�g�j��p�@    �S�~
	{ BATT_BG_OBONID_16,FORMNO_412_GOMI},				//�l�V���i�i���j��p�@�@�@    �S�~
	{ BATT_BG_OBONID_17,FORMNO_412_GOMI},				//�l�V���i���j��p�@�@�@�@    �S�~
	{ BATT_BG_OBONID_18,FORMNO_412_GOMI},				//�l�V���i�G�X�p�[�j��p�@    �S�~
	{ BATT_BG_OBONID_19,FORMNO_412_GOMI},			  //N��p                       �S�~
	{ BATT_BG_OBONID_20,FORMNO_412_GOMI},				//�Q�[�`�X��p                �S�~
	{ BATT_BG_OBONID_21,FORMNO_412_SUNA},				//�`�����s�I����p            ��
	{ BATT_BG_OBONID_22,FORMNO_412_KUSA},				//�G���J�E���g���i�l�G����j  ��
	{ BATT_BG_OBONID_23,FORMNO_412_KUSA},				//�G���J�E���g���i�l�G�Ȃ��j  ��
	{ BATT_BG_OBONID_24,FORMNO_412_GOMI},				//�A�X�t�@���g                �S�~
};

//--------------------------------------------------------------
/**
 * @brief   ���|�P�������w�肵���G�߂̃t�H�����ɂ���
 *
 * @param   gdata   �Q�[���f�[�^�|�C���^
 * @param   ppt		POKEPARTY�ւ̃|�C���^
 * @param   inEnvor     �퓬���~�h�c
 */
//--------------------------------------------------------------
void STRAW_POKE_FORM_ChangeForm(GAMEDATA * gdata, POKEMON_PARAM *pp, const u32 inEnvor)
{
  int i;
  int monsno;
  int form;
  ZUKAN_SAVEDATA *zw = GAMEDATA_GetZukanSave( gdata );
	
	monsno = PP_Get(pp, ID_PARA_monsno, NULL);
  for (i=0;i<BATT_BG_OBONID_MAX;i++)
  {
    if ( StrawFromTbl[i][0] == inEnvor )
    {
      form = StrawFromTbl[i][1];
      if ( monsno == MONSNO_MINOMUTTI )
      {
        PP_ChangeFormNo( pp, form );
        //�}�ӓo�^�u�����v
        ZUKANSAVE_SetPokeSee(zw, pp);
      }
      break;
    }
  }
}

