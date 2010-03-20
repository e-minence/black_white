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
#include    "poke_tool/pokepara_conv.h"


//============================================================================================
/**
 * @brief   �|�P�V�t�^�[�o�R�ŃV���`�Ɏ����Ă����|�P�����p�����[�^���R���o�[�g
 *
 * @param ppp PokePasoParam
 */
//============================================================================================
void  PPCONV_ConvertPPPFromPokeShifter( POKEMON_PASO_PARAM* ppp )
{ 
  const BOOL fast = PPP_FastModeOn( ppp );
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
  
  //���O�ϊ�
  {
    STRCODE monsName[MONS_NAME_SIZE+EOM_SIZE];
    PPP_Get( ppp , ID_PARA_nickname_raw , monsName );
    PPCONV_ConvertStr( monsName , monsName , MONS_NAME_SIZE+EOM_SIZE );
    PPP_Put( ppp , ID_PARA_nickname_raw , (u32)monsName );
  }
  //�e�̖��O�ϊ�
  {
    STRCODE oyaName[PERSON_NAME_SIZE+EOM_SIZE];
    PPP_Get( ppp , ID_PARA_oyaname_raw , oyaName );
    PPCONV_ConvertStr( oyaName , oyaName , SAVELEN_PLAYER_NAME+EOM_SIZE );
    PPP_Put( ppp , ID_PARA_oyaname_raw , (u32)oyaName );
  }
  PPP_FastModeOff( ppp , fast );
}



//--------------------------------------------------------------
//	������̕ϊ�
//--------------------------------------------------------------
#include "./pokepara_conv_str.cdat"
void PPCONV_ConvertStr( const u16 *src , STRCODE *dst , const u8 dstLen )
{
  const STRCODE EomCode = GFL_STR_GetEOMCode();
  static const u16 UnknownCode = L'?';
  int i,j;
  BOOL isEnd = FALSE;
  for( i=0;i<dstLen;i++ )
  {
    if( src[i] == 0xFFFF )
    {
      dst[i] = EomCode;
      isEnd = TRUE;
      break;
    }
    for( j=0;j<PPCONV_STR_ARR_NUM;j++ )
    {
      if( src[i] == PPCONV_STR_ARR[j][PPCONV_STR_OLD_CODE] )
      {
        dst[i] = PPCONV_STR_ARR[j][PPCONV_STR_NEW_CODE];
        break;
      }
    }
    if( j == PPCONV_STR_ARR_NUM ){
      //�Y�������Ȃ�
      dst[i] = UnknownCode;
    }
  }
  
  GF_ASSERT_MSG(isEnd,"PPCONV_ConvertStr BufferOver!\n");
}
