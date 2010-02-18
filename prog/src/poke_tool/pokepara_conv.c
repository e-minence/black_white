//============================================================================================
/**
 * @file  pokepara_conv.c
 * @bfief �|�P�����p�����[�^�R���o�[�^�c�[���Q
 * @author  HisashiSogabe
 * @date  10.02.18
 */
//============================================================================================
#include    <gflib.h>

#include    "poke_tool/poke_tool.h"

//============================================================================================
/**
 * @brief   �|�P�V�t�^�[�o�R�ŃV���`�Ɏ����Ă����|�P�����p�����[�^���R���o�[�g
 *
 * @param ppp PokePasoParam
 */
//============================================================================================
void  PPCONV_ConvertPPPFromPokeShifter( POKEMON_PASO_PARAM* ppp )
{ 
  //���i�����������瓱���ă��[�N�ɑ��
  { 
    u8 seikaku = PPP_Get( ppp, ID_PARA_personal_rnd, NULL ) % 25;
    
    PPP_Put( ppp, ID_PARA_seikaku, seikaku );
  }

  //HGSS�̕ߊl�{�[�����V���`�̕ߊl�{�[���̈�ɃR�s�[
  { 
    u8 get_cassette = PPP_Get( ppp, ID_PARA_get_cassette, NULL );
    if( ( get_cassette == VERSION_GOLD ) || ( get_cassette == VERSION_SILVER ) )
    { 
      u8  get_ball = PPP_Get( ppp, ID_PARA_dummy_p4_1, NULL ) & 0xff;
      PPP_Put( ppp, ID_PARA_get_ball, get_ball );
    }
  }

  //�V���`�ɂȂ��Ė��g�p�ƂȂ����̈���O�N���A
  PPP_Put( ppp, ID_PARA_dummy_p2_3, 0 );  //�v���`�i new_get_place new_birth_place
  PPP_Put( ppp, ID_PARA_dummy_p4_1, 0 );  //HGSS get_ball humor
}
