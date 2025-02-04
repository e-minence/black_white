//============================================================================================
/**
 * @file  pokepara_conv.c
 * @bfief ポケモンパラメータコンバータツール群
 * @author  HisashiSogabe
 * @date  10.02.18
 */
//============================================================================================
#include    <gflib.h>

#include    "poke_tool/poke_tool.h"
#include    "poke_tool/pokepara_conv.h"


//============================================================================================
/**
 * @brief   ポケシフター経由でシャチに持ってきたポケモンパラメータをコンバート
 *
 * @param ppp PokePasoParam
 */
//============================================================================================
void  PPCONV_ConvertPPPFromPokeShifter( POKEMON_PASO_PARAM* ppp )
{ 
  const BOOL fast = PPP_FastModeOn( ppp );
  //性格を個性乱数から導いてワークに代入
  { 
    u8 seikaku = PPP_Get( ppp, ID_PARA_personal_rnd, NULL ) % 25;
    
    PPP_Put( ppp, ID_PARA_seikaku, seikaku );
  }

  //HGSSの捕獲ボールをシャチの捕獲ボール領域にコピー

  { 
    u8 get_cassette = PPP_Get( ppp, ID_PARA_get_cassette, NULL );
    if( ( get_cassette == VERSION_GOLD ) || ( get_cassette == VERSION_SILVER ) )
    { 
      //GSのpoketoolより抜粋
      // 新しい領域に値があるかチェックをしているのは、GSで作った卵をDP,PLで孵化させると
      // カセットバージョンはGSだが新領域は０という状況ができてしまうため、
      // その場合は旧領域を返すようにしました
      // 不思議な贈り物もHGSS名義なのに旧領域に入っている可能性があるので・・・
      u8  get_ball = PPP_Get( ppp, ID_PARA_dummy_p4_1, NULL ) & 0xff;
      if( get_ball != 0 )
      {
        PPP_Put( ppp, ID_PARA_get_ball, get_ball );
      }
    }
  }

  //シャチになって未使用となった領域を０クリア
  PPP_Put( ppp, ID_PARA_dummy_p2_3, 0 );  //プラチナ new_get_place new_birth_place
  PPP_Put( ppp, ID_PARA_dummy_p4_1, 0 );  //HGSS get_ball humor
  
  //名前変換
  {
    STRCODE monsName[MONS_NAME_SIZE+EOM_SIZE];
    PPP_Get( ppp , ID_PARA_nickname_raw , monsName );
    PPCONV_ConvertStr( monsName , monsName , MONS_NAME_SIZE+EOM_SIZE );
    //過去作転送専用・海外のポケモンはニックネームフラグをそのまま引き継ぐので
    PPP_Put( ppp , ID_PARA_nickname_raw_only , (u32)monsName );
  }
  //親の名前変換
  {
    STRCODE oyaName[PERSON_NAME_SIZE+EOM_SIZE];
    PPP_Get( ppp , ID_PARA_oyaname_raw , oyaName );
    PPCONV_ConvertStr( oyaName , oyaName , SAVELEN_PLAYER_NAME+EOM_SIZE );
    PPP_Put( ppp , ID_PARA_oyaname_raw , (u32)oyaName );
  }
  PPP_FastModeOff( ppp , fast );
}



//--------------------------------------------------------------
//	文字列の変換
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
      //該当文字なし
      dst[i] = UnknownCode;
    }
  }
  
  GF_ASSERT_MSG(isEnd,"PPCONV_ConvertStr BufferOver!\n");
}
