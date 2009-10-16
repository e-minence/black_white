//============================================================================================
/**
 * @file  wordset.c
 * @brief �P�ꃂ�W���[������
 * @author  taya
 * @date  2005.09.28
 */
//============================================================================================
#include <gflib.h>

#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_msg.h"
#include "str_tool_local.h"

#include "arc_def.h"
#include "message.naix"


/*----------------------------------*/
/** ���@��                          */
/*----------------------------------*/
enum {
  WORDSET_GRAMMER_NONE,   ///< �g�p���Ȃ�
  WORDSET_GRAMMER_MALE,   ///< �j��
  WORDSET_GRAMMER_FEMALE,   ///< ����
  WORDSET_GRAMMER_NEUTRAL,  ///< ����
};


/*----------------------------------*/
/** �P��p�����[�^                  */
/*----------------------------------*/
typedef struct {
  u8  def_article;    ///< �芥���h�c
  u8  indef_article;  ///< �s�芥���h�c
  u8  preposition;    ///< �O�u���h�c
  u8  grammer : 7;    ///< ���@��
  u8  form : 1;       ///< �P���E�����i0 = �P���j
  u8  deco_id;        ///< �f�R����ID
}WORDSET_PARAM;


typedef struct {
  WORDSET_PARAM param;
  STRBUF*     str;
}WORD;



struct _WORDSET{

  u32     max;
  u32     heapID;
  WORD*   word;
  STRBUF*   tmpBuf;

};



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void InitParam(WORDSET_PARAM* param);
static void RegisterWord( WORDSET* wordset, u32 bufID, const STRBUF* str, const WORDSET_PARAM* param );



//======================================================================================================
// �V�X�e���������E�I��
//======================================================================================================

//------------------------------------------------------------------
/**
 * �P��Z�b�g���W���[���쐬
 *
 * @param   heapID      �쐬��q�[�v�h�c
 *
 * @retval  WORDSET*    �P��Z�b�g
 */
//------------------------------------------------------------------
WORDSET*  WORDSET_Create( HEAPID heapID )
{
  return WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_DEFAULT_BUFLEN, heapID );
}

//------------------------------------------------------------------
/**
 * �P��Z�b�g���W���[���쐬�i�o�^�P�ꐔ�E�����񒷂̎w��Łj
 *
 * @param   word_max
 * @param   buflen
 * @param   heapID
 *
 * @retval  WORDSET*
 */
//------------------------------------------------------------------
WORDSET* WORDSET_CreateEx( u32 word_max, u32 buflen, HEAPID heapID )
{
  GF_ASSERT(word_max);
  GF_ASSERT(buflen);

  {
    WORDSET* wordset = NULL;
    u32 i;

    do {

      WORDSET* wordset = GFL_HEAP_AllocMemory( heapID, sizeof(WORDSET) );
      if( wordset == NULL ){ break; }

      wordset->max = word_max;
      wordset->heapID = heapID;

      wordset->tmpBuf = GFL_STR_CreateBuffer( buflen, heapID );
      if( wordset->tmpBuf == NULL ){ break; }

      wordset->word = GFL_HEAP_AllocMemory( heapID, sizeof(WORD)*word_max );
      if( wordset->word == NULL ){ break; }

      for(i=0; i<word_max; i++)
      {
        InitParam(&(wordset->word[i].param));
        wordset->word[i].str = GFL_STR_CreateBuffer( buflen, heapID );
        if( wordset->word[i].str == NULL ){
          break;
        }
      }

      if( i == word_max )
      {
        return wordset;
      }

    }while(0);

    if( wordset )
    {
      WORDSET_Delete(wordset);
    }
    return NULL;
  }
}

//------------------------------------------------------------------
/**
 * �P��Z�b�g�j��
 *
 * @param   wordset
 *
 */
//------------------------------------------------------------------
void WORDSET_Delete( WORDSET* wordset )
{
  u32 i;

  GF_ASSERT(wordset->max);

  if( wordset->word )
  {
    for(i=0; i<wordset->max; i++)
    {
      if( wordset->word[i].str )
      {
        GFL_STR_DeleteBuffer( wordset->word[i].str );
      }
      else
      {
        break;
      }
    }
    GFL_HEAP_FreeMemory( wordset->word );
  }

  if( wordset->tmpBuf )
  {
    GFL_STR_DeleteBuffer(wordset->tmpBuf);
  }

  wordset->max = 0;
  GFL_HEAP_FreeMemory(wordset);
}


//------------------------------------------------------------------
/**
 * �P��p�����[�^������
 *
 * @param   param   �p�����[�^�\���̂ւ̃|�C���^
 *
 */
//------------------------------------------------------------------
static void InitParam(WORDSET_PARAM* param)
{
  GFL_STD_MemClear( param, sizeof(*param) );
  param->deco_id = PMS_DECOID_NULL;
}



//======================================================================================================
// �P��Z�b�g���W���[���Ɋe��P���o�^����
//======================================================================================================

//------------------------------------------------------------------
/**
 * �P�ꕶ���񁕃p�����[�^���w��o�b�t�@�ɓo�^
 *
 * @param   wordset   �P��Z�b�g���W���[��
 * @param   bufID   �o�b�t�@�h�c
 * @param   str     ������
 * @param   param   ������ɕt������p�����[�^
 *
 */
//------------------------------------------------------------------
static void RegisterWord( WORDSET* wordset, u32 bufID, const STRBUF* str, const WORDSET_PARAM* param )
{
  GF_ASSERT_MSG( bufID < wordset->max, "bufID=%d, wordmax=%d", bufID, wordset->max );

  if( bufID < wordset->max )
  {
    if( param != NULL )
    {
      wordset->word[bufID].param = *param;
    }
    else
    {
      InitParam( &wordset->word[bufID].param );
    }
    GFL_STR_CopyBuffer( wordset->word[bufID].str, str );
  }
}


//------------------------------------------------------------------
/**
 * �C�ӕ�������p�����[�^�t���œo�^
 *
 * @param   wordset     �P��Z�b�g
 * @param   bufID     �o�b�t�@ID
 * @param   word      ������
 * @param   sex       ���ʃR�[�h
 * @param   singular_flag �P�^���iTRUE�ŒP���j
 * @param   lang      ����R�[�h
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterWord( WORDSET* wordset, u32 bufID, const STRBUF* word, u32 sex, BOOL singular_flag, u32 lang )
{
  WORDSET_PARAM  param;

  InitParam( &param );
  param.grammer = sex;
  param.form = !singular_flag;

  RegisterWord( wordset, bufID, word, &param );
}


//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����푰����o�^ (POKEMON_MARAM)
 *
 * @param   bufID   �o�b�t�@ID
 * @param   pp      �|�P�����p�����[�^
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeMonsName( WORDSET* wordset, u32 bufID, const POKEMON_PARAM* pp )
{
  u32 monsno;

  // [[[�����Ń|�P�����̐��ʓ����`�F�b�N�ł���]]]
  monsno = PP_Get( pp, ID_PARA_monsno, NULL );
  GFL_MSG_GetString( GlobalMsg_PokeName, monsno, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}
//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����푰����o�^ (POKEMON_PASO_PARAM)
 *
 * @param   bufID   �o�b�t�@ID
 * @param   ppp     �|�P�����p�����[�^
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeMonsNamePPP( WORDSET* wordset, u32 bufID, const POKEMON_PASO_PARAM* ppp )
{
  u32 monsno;

  // [[[�����Ń|�P�����̐��ʓ����`�F�b�N�ł���]]]
  monsno = PPP_Get( ppp, ID_PARA_monsno, NULL );
  GFL_MSG_GetString( GlobalMsg_PokeName, monsno, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}
//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����̃j�b�N�l�[����o�^ (POKEMON_PARAM)
 *
 * @param   bufID   �o�b�t�@ID
 * @param   pp      �|�P�����p�����[�^
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeNickName( WORDSET* wordset, u32 bufID, const POKEMON_PARAM* pp )
{
  // [[[�����Ń|�P�����̐��ʓ����`�F�b�N�ł���]]]
  PP_Get( pp, ID_PARA_nickname, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}
//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����̃j�b�N�l�[����o�^ (POKEMON_PASO_PARAM)
 *
 * @param   bufID   �o�b�t�@ID
 * @param   ppp     �|�P�����p�����[�^
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeNickNamePPP( WORDSET* wordset, u32 bufID, const POKEMON_PASO_PARAM* ppp )
{
  // [[[�����Ń|�P�����̐��ʓ����`�F�b�N�ł���]]]
  PPP_Get( ppp, ID_PARA_nickname, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}
//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����̐e����o�^
 *
 * @param   bufID   �o�b�t�@ID
 * @param   ppp   �|�P�����p�����[�^
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeOyaName( WORDSET* wordset,  u32 bufID, const POKEMON_PARAM* pp )
{
  // [[[�����Őe�̐��ʂ��`�F�b�N�ł���]]]
  PP_Get( pp, ID_PARA_oyaname, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);

}
//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ��U����o�^
 *
 * @param   bufID     �o�b�t�@ID
 * @param   wazaID      ���UID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterWazaName( WORDSET* wordset, u32 bufID, u32 wazaID )
{
  GFL_MSGDATA *man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
          NARC_message_wazaname_dat, GetHeapLowID(wordset->heapID) );
  if( man )
  {
    GFL_MSG_GetString( man, wazaID, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
    GFL_MSG_Delete( man );
  }
}
//------------------------------------------------------------------
/**
 * �w��o�b�t�@�ɃA�C�e����
 *
 * @param   bufID     �o�b�t�@ID
 * @param   itemID    �A�C�e��ID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterItemName( WORDSET* wordset, u32 bufID, u32 itemID )
{
  GFL_MSGDATA *man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
          NARC_message_itemname_dat, GetHeapLowID(wordset->heapID) );
  if( man )
  {
    GFL_MSG_GetString( man, itemID, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
    GFL_MSG_Delete( man );
  }
}
//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����̂Ƃ���������o�^
 *
 * @param   bufID     �o�b�t�@ID
 * @param   tokuseiID   �Ƃ�����ID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterTokuseiName( WORDSET* wordset, u32 bufID, u32 tokuseiID )
{
  GFL_MSGDATA *man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
          NARC_message_tokusei_dat, GetHeapLowID(wordset->heapID) );
  if( man )
  {
    GFL_MSG_GetString( man, tokuseiID, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
    GFL_MSG_Delete( man );
  }
}
//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����̂���������o�^
 *
 * @param   bufID     �o�b�t�@ID
 * @param   seikakuID   ��������ID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterSeikaku( WORDSET* wordset, u32 bufID, u32 seikakuID )
{
  GFL_MSGDATA *man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
          NARC_message_chr_dat, GetHeapLowID(wordset->heapID) );
  if( man )
  {
    GFL_MSG_GetString( man, seikakuID, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
    GFL_MSG_Delete( man );
  }
}
//------------------------------------------------------------------
/**
 * �w��o�b�t�@�ɐ�����o�^
 *
 * @param   bufID   �o�b�t�@ID
 * @param   number    ���l
 * @param   keta    ����
 * @param   dispType  ���`�^�C�v
 * @param   codeType  �����R�[�h�^�C�v
 *
 * dispType :  STR_NUM_DISP_LEFT    ���l��
 *             STR_NUM_DISP_RIGHT   �E�l�߁i�X�y�[�X���߁j
 *             STR_NUM_DISP_ZERO    �E�l�߁i�[�����߁j
 *
 *
 * codeType :  STR_NUM_CODE_DEFAULT  ���[�J���C�Y���̃f�t�H���g���i���{���S�p���^�C�O�����p���j�ɂȂ�
 *             STR_NUM_CODE_ZENKAKU  �S�p���w��
 *             STR_NUM_CODE_HANKAKU  ���p���w��
 */
//------------------------------------------------------------------
void WORDSET_RegisterNumber( WORDSET* wordset, u32 bufID, s32 number, u32 keta, StrNumberDispType dispType, StrNumberCodeType codeType )
{
  STRTOOL_SetNumber( wordset->tmpBuf, number, keta, dispType, codeType );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL);
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�ɊȈՉ�b�P���o�^
 *
 * @param   wordset   ���[�h�Z�b�g�I�u�W�F�N�g
 * @param   bufID   ���Ԃ̃o�b�t�@�ɓo�^���邩
 * @param   word    �ȈՉ�b�P��i���o�[
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPMSWord( WORDSET* wordset, u32 bufID, PMS_WORD word )
{
  PMSW_GetStr( word, wordset->tmpBuf , wordset->heapID );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
}
//------------------------------------------------------------------
/**
 * �w��o�b�t�@�ɊȈՉ�b�f�R������o�^
 *
 * @param   wordset   ���[�h�Z�b�g�I�u�W�F�N�g
 * @param   bufID     ���Ԃ̃o�b�t�@�ɓo�^���邩
 * @param   decoID    �f�R����
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPMSDeco( WORDSET* wordset, u32 bufID, PMS_DECO_ID decoID )
{
  GF_ASSERT_MSG( bufID < wordset->max, "bufID=%d, wordmax=%d", bufID, wordset->max );

  if( bufID < wordset->max )
  {
    wordset->word[bufID].param.deco_id = decoID;
  }
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����i���U�j�^�C�v����o�^
 *
 * @param   bufID     �o�b�t�@ID
 * @param   typeID      �^�C�vID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeTypeName( WORDSET* wordset, u32 bufID, u32 typeID )
{
  GFL_MSGDATA *man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_typename_dat, wordset->heapID);
  if( man )
  {
    GFL_MSG_GetString( man, typeID, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
    GFL_MSG_Delete(man);
  }
}

//------------------------------------------------------------------
/**
 * �v���C���[����o�^
 *
 * @param   wordset   �P��Z�b�g�I�u�W�F�N�g
 * @param   bufID   �o�b�t�@ID
 * @param   status
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPlayerName( WORDSET* wordset, u32 bufID, const MYSTATUS* my )
{
  MyStatus_CopyNameString( my, wordset->tmpBuf );
  RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�ɍ�����o�^
 *
 * @param   wordset   ���[�h�Z�b�g�I�u�W�F�N�g
 * @param   bufID   ���Ԃ̃o�b�t�@�ɓo�^���邩
 * @param   strID   ��ID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterCountryName( WORDSET* wordset, u32 bufID, u32 countryID )
{
  GFL_MSGDATA *man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_place_msg_world_dat, wordset->heapID);
  if( man )
  {
    GFL_MSG_GetString( man, countryID, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
    GFL_MSG_Delete(man);
  }
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�ɒn�於��o�^
 *
 * @param   wordset   ���[�h�Z�b�g�I�u�W�F�N�g
 * @param   bufID   ���Ԃ̃o�b�t�@�ɓo�^���邩
 * @param   countryID ��ID
 * @param   placeID   �n��ID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterLocalPlaceName( WORDSET* wordset, u32 bufID, u32 countryID, u32 placeID )
{
  u32  datID;

  datID = WIFI_COUNTRY_CountryCodeToPlaceMsgDataID(countryID);
  if( datID )
  {
    if( placeID )
    {
      GFL_MSGDATA *man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, datID, wordset->heapID);
      if( man )
      {
        GFL_MSG_GetString( man, placeID, wordset->tmpBuf );
        RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
        GFL_MSG_Delete(man);
      }
    }
  }
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀg���[�i�[��ʂ�o�^
 *
 * @param   wordset   ���[�h�Z�b�g�I�u�W�F�N�g
 * @param   bufID   ���Ԃ̃o�b�t�@�ɓo�^���邩
 * @param   trID    �g���[�i�[���ID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterTrTypeName( WORDSET* wordset, u32 bufID, TrainerID trID )
{
  //tr_tool/tr_tool.h���C���N���[�h����
  //TT_TrainerTypeSexGet(trID)�Ƃ��邱�ƂŃg���[�i�[�̐��ʂ��擾�ł��܂�
  GFL_MSGDATA*  man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_trtype_dat, wordset->heapID );
  int tr_type = TT_TrainerDataParaGet( trID, ID_TD_tr_type );
  if( man )
  {
    GFL_MSG_GetString( man, tr_type, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
    GFL_MSG_Delete( man );
  }
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀg���[�i�[����o�^
 *
 * @param   wordset   ���[�h�Z�b�g�I�u�W�F�N�g
 * @param   bufID     ���Ԃ̃o�b�t�@�ɓo�^���邩
 * @param   trID      �g���[�i�[���ID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterTrainerName( WORDSET* wordset, u32 bufID, TrainerID trID )
{
  GFL_MSGDATA* man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_trname_dat, wordset->heapID );
  if( man )
  {
    GFL_MSG_GetString( man, trID, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
    GFL_MSG_Delete(man);
  }
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�ɃX�e�[�^�X����o�^
 *
 * @param   wordset   ���[�h�Z�b�g�I�u�W�F�N�g
 * @param   bufID     ���Ԃ̃o�b�t�@�ɓo�^���邩
 * @param   trID      �X�e�[�^�X(�̗́E�U���E�h��E�f�����E���U�E���h�E�����E����E�ő�HP
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterPokeStatusName( WORDSET* wordset, u32 bufID, u8 statusID )
{
  GFL_MSGDATA* man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_status_dat, wordset->heapID );
  if( man )
  {
    GFL_MSG_GetString( man, statusID, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
    GFL_MSG_Delete(man);
  }
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�ɃA�C�e��������|�P�b�g����o�^
 *
 * @param   bufID     �o�b�t�@ID
 * @param   wazaID      �|�P�b�gID
 *
 */
//------------------------------------------------------------------
void WORDSET_RegisterItemPocketName( WORDSET* wordset, u32 bufID, u32 pocketID )
{
  GFL_MSGDATA *man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_itempocket_dat, wordset->heapID);
  if( man )
  {
    GFL_MSG_GetString( man, pocketID, wordset->tmpBuf );
    RegisterWord( wordset, bufID, wordset->tmpBuf, NULL );
    GFL_MSG_Delete(man);
  }
}





//======================================================================================================
// ������W�J
//======================================================================================================

//------------------------------------------------------------------
/**
 * �o�^���ꂽ�P����g���ĕ�����W�J����
 *
 * @param   dst   �W�J��o�b�t�@
 * @param   src   �W�J��������
 *
 */
//------------------------------------------------------------------
void WORDSET_ExpandStr( const WORDSET* wordset, STRBUF* dstbuf, const STRBUF* srcbuf )
{
  const STRCODE* src;
  STRCODE *dst;
  STRCODE EOM_Code, TAG_Code;

  src = GFL_STR_GetStringCodePointer( srcbuf );
  GFL_STR_ClearBuffer( dstbuf );
  EOM_Code = GFL_STR_GetEOMCode();
  TAG_Code = PRINTSYS_GetTagStartCode();

  while( *src != EOM_Code )
  {
    if( *src == TAG_Code )
    {
      if( PRINTSYS_IsWordSetTagGroup(src) )
      {
        u32 word_id;

        word_id = PRINTSYS_GetTagParam( src, 0 );
        GF_ASSERT( word_id < wordset->max );

        // �f�R�L�������o�Ă�����w���Z�^�O�𖄂ߍ���ł���
        if( wordset->word[ word_id ].param.deco_id != PMS_DECOID_NULL ){
          u16 add_x = PMS_DECO_WIDTH;
          PRINTSYS_CreateTagCode( wordset->word[word_id].str, PRINTSYS_TAGGROUP_CTRL_GEN, PRINTSYS_CTRL_GENERAL_X_ADD, 1, &add_x );
        }

        // [[[�C�O�łł̓p�����[�^�̉��߂��K�v]]]
        GFL_STR_AddString( dstbuf, wordset->word[ word_id ].str );
        src = PRINTSYS_SkipTag( src );
      }
      else
      {
        const STRCODE* p = src;

        src = PRINTSYS_SkipTag( src );
        while(p < src)
        {
          GFL_STR_AddCode( dstbuf, *p++ );
        }
      }
    }
    else
    {
      GFL_STR_AddCode( dstbuf, *src++ );
    }
  }
}




//======================================================================================================
// �o�b�t�@�N���A
//======================================================================================================

//------------------------------------------------------------------
/**
 * �����o�b�t�@��S�ċ󕶎���ŃN���A����
 *
 * @param   wordset   WORDSET�ւ̃|�C���^
 *
 */
//------------------------------------------------------------------
void WORDSET_ClearAllBuffer( WORDSET* wordset )
{
  u32 i;
  for(i=0; i<wordset->max; i++)
  {
    GFL_STR_ClearBuffer( wordset->word[i].str );
  }
}




