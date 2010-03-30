//==============================================================================
/**
 * @file    dendou_save.c
 * @brief   �a������Z�[�u�f�[�^
 * @author  matsuda
 * @date    2010.03.12(��)
 *
 * @note    WB�ł͓a������f�[�^�͊O���Z�[�u�œ����Z�[�u�̂Q�ӏ��Ƀf�[�^�������܂��B
 *          �O���Z�[�u�Ɏ��]���̓a������f�[�^�łP�T�Ԃ�̓a������f�[�^���A
 *          �����Z�[�u�Ɉ�������f�[�^�͏��N���A���́u�m�ɏ��������v���̃f�[�^��
 *          ���ĕێ����܂��B�O���̓a������͍X�V����Ă����܂����A�����͈�x����̏������݂ł�
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/dendou_save.h"
#include "pm_define.h"



//==============================================================================
//  �萔��`
//==============================================================================
enum {
  POKENAME_BUFSIZE = MONS_NAME_SIZE+EOM_SIZE,   // 11
  OYANAME_BUFSIZE = PERSON_NAME_SIZE+EOM_SIZE,  // 8
};

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static int inline recIndex_to_datIndex( const DENDOU_SAVEDATA* data, int recIndex );




//----------------------------------------------------------
/**
 *  �a������|�P�����̃f�[�^�i60 bytes�j
 */
//----------------------------------------------------------
typedef struct {
  u16 monsno;       ///< �|�P�����i���o�[�i�O�Ȃ�f�[�^�S�̂������j
  u8  level;        ///< ���x��
  u8  formNumber:6; ///< �t�H����
  u8  sex:2;        ///< ����
  u32 personalRandom;   ///< �̗���
  u32 idNumber;     ///< ID
  STRCODE nickname[ POKENAME_BUFSIZE ]; ///< �j�b�N�l�[��
  STRCODE oyaname[ OYANAME_BUFSIZE ];   ///< ���▼
  u16 waza[ 4 ];      ///< �Z�i���o�[
}DENDOU_POKEMON_DATA_INSIDE;


//----------------------------------------------------------
/**
 *  �a������P�����R�[�h�f�[�^�i364 bytes�j(�����ɂ��P�������܂��j
 */
//----------------------------------------------------------
struct DENDOU_RECORD{
  DENDOU_POKEMON_DATA_INSIDE  pokemon[ TEMOTI_POKEMAX ];  ///< �|�P�����f�[�^
  u16     year;       ///< �a��������t�i�N�j 2006 �`
  u8      month;        ///< �a��������t�i���j 1�`12
  u8      day;        ///< �a��������t�i���j 1�`31
};


//----------------------------------------------------------
/**
 *  �a������Z�[�u�f�[�^�{�́i10920 bytes�j
 */
//----------------------------------------------------------
struct _DENDOU_SAVEDATA {

  DENDOU_RECORD record[ DENDOU_RECORD_MAX ];

  u32   savePoint;    ///< record�z�񒆂́A���ɃZ�[�u����index
  u32   latestNumber; ///< �ŐV�f�[�^�̓a������ʎZ��
};




//=============================================================================================
/**
 * @brief �a������f�[�^�Z�[�u�f�[�^�T�C�Y��Ԃ�
 * @retval  u32   
 */
//=============================================================================================
u32 DendouData_GetWorkSize( void )
{
  return sizeof(DENDOU_SAVEDATA);
}

//=============================================================================================
/**
 * @brief   �N���A�|�P�����Z�[�u�f�[�^�T�C�Y��Ԃ�
 * @retval  u32   
 */
//=============================================================================================
u32 GameClearData_GetWorkSize( void )
{
  return sizeof(DENDOU_RECORD);
}

//------------------------------------------------------------------
/**
 * �f�[�^������
 *
 * @param   data    
 *
 */
//------------------------------------------------------------------
void DendouData_Init( void *work )
{
  GFL_STD_MemClear( work, sizeof(DENDOU_SAVEDATA) );
}

void GameClearData_Init( void *work )
{
  GFL_STD_MemClear( work, sizeof(DENDOU_RECORD) );
}


//=============================================================================================
/**
 * @brief �a�����背�R�[�h�f�[�^�֎莝���|�P�������m�F������ŏ�������
 *
 * @param   record    
 * @param   party   
 * @param   date    
 * @param   heap_id   
 */
//=============================================================================================
void DendouRecord_Add( DENDOU_RECORD* record, const POKEPARTY* party, const RTCDate* date, HEAPID heap_id )
{
  POKEMON_PARAM*  pp;
  STRBUF*         strbuf;
  int  pokeCount, i, p;
  BOOL fastFlag;

  GF_ASSERT(record!=NULL);

  pokeCount = PokeParty_GetPokeCount( party );

  strbuf = GFL_STR_CreateBuffer( POKENAME_BUFSIZE, heap_id );

  GFL_STD_MemClear( record->pokemon,  sizeof(DENDOU_POKEMON_DATA_INSIDE)*TEMOTI_POKEMAX );

  for(i=0, p=0; i<pokeCount; i++)
  {
    pp = PokeParty_GetMemberPointer( party, i );

    fastFlag = PP_FastModeOn( pp );

    if( PP_Get(pp, ID_PARA_tamago_flag, NULL) == FALSE )
    {
      record->pokemon[p].monsno = PP_Get( pp, ID_PARA_monsno, NULL );
      record->pokemon[p].level = PP_Get( pp, ID_PARA_level, NULL );
      record->pokemon[p].formNumber = PP_Get( pp, ID_PARA_form_no, NULL );
      record->pokemon[p].sex = PP_Get( pp, ID_PARA_sex, NULL );
      record->pokemon[p].personalRandom = PP_Get( pp, ID_PARA_personal_rnd, NULL );
      record->pokemon[p].idNumber = PP_Get( pp, ID_PARA_id_no, NULL );
      record->pokemon[p].waza[0] = PP_Get( pp, ID_PARA_waza1, NULL );
      record->pokemon[p].waza[1] = PP_Get( pp, ID_PARA_waza2, NULL );
      record->pokemon[p].waza[2] = PP_Get( pp, ID_PARA_waza3, NULL );
      record->pokemon[p].waza[3] = PP_Get( pp, ID_PARA_waza4, NULL );

      if(strbuf)
      {
        PP_Get( pp, ID_PARA_nickname, strbuf );
        GFL_STR_GetStringCode( strbuf, record->pokemon[p].nickname, POKENAME_BUFSIZE );

        PP_Get( pp, ID_PARA_oyaname, strbuf );
        GFL_STR_GetStringCode( strbuf, record->pokemon[p].oyaname, OYANAME_BUFSIZE );
      }
      else
      {
        record->pokemon[p].nickname[0] = GFL_STR_GetEOMCode();
        record->pokemon[p].oyaname[0]  = GFL_STR_GetEOMCode();
      }

      p++;
    }
    
    PP_FastModeOff( pp, fastFlag );
  }

  record->year = date->year;
  record->month = date->month;
  record->day = date->day;

  if(strbuf)
  {
    GFL_STR_DeleteBuffer( strbuf );
  }
  
}

//------------------------------------------------------------------
/**
 * �a�����背�R�[�h(=�N���A���f�[�^)�P���ǉ�
 *
 * @param   data    �a������Z�[�u�f�[�^�|�C���^
 * @param   party   �a������p�[�e�B�[�̃f�[�^
 * @param   date    �a��������t�f�[�^�iNITRO�̃f�[�^���̂܂܁j
 *
 */
//------------------------------------------------------------------
void DendouData_AddRecord( DENDOU_SAVEDATA* data, const POKEPARTY* party, const RTCDate* date, HEAPID heap_id )
{
  DENDOU_RECORD*  record;
  POKEMON_PARAM*  pp;
  STRBUF*         strbuf;
  int  pokeCount, i, p;
  BOOL fastFlag;

  GF_ASSERT(data != NULL);
  GF_ASSERT(data->savePoint < DENDOU_RECORD_MAX);

  // �������ݏꏊ�Z�o�i�ő�l(9999)�ȍ~�͖�������j
  if( data->latestNumber >= DENDOU_NUMBER_MAX )
  {
    return;
  }
  record = &data->record[ data->savePoint ];

  // ���R�[�h�P����������
  DendouRecord_Add( record, party, date, heap_id );

  // �������݃|�C���g�i�߂�
  if(++(data->savePoint) >= DENDOU_RECORD_MAX)
  {
    data->savePoint = 0;
  }
  data->latestNumber++;

}



//=============================================================================================
/**
 * @brief �a�����R�[�h(=�N���A���f�[�^)�̎莝���|�P�����̐���Ԃ�
 *
 * @param   record    
 *
 * @retval  u32   �莝���|�P�����̐�
 */
//=============================================================================================
u32  DendouRecord_GetPokemonCount( const DENDOU_RECORD* record )
{
  int i;
  for(i=0; i<TEMOTI_POKEMAX; i++)
  {
    if(record->pokemon[i].monsno == 0)
    {
      break;
    }
  }
  return i;
  
}

//=============================================================================================
/**
 * @brief   �a�����R�[�h(=�N���A���f�[�^)�̃|�P��������Ԃ�
 *
 * @param   record      
 * @param   poke_pos    �莝���|�P�����̈ʒu�i0-5�j
 * @param   poke_data   �����������ރ|�C���^
 */
//=============================================================================================
void DendouRecord_GetPokemonData( const DENDOU_RECORD* record, int poke_pos, DENDOU_POKEMON_DATA* poke_data )
{
  const DENDOU_POKEMON_DATA_INSIDE*  recPoke;
  int i;
  GF_ASSERT( record != NULL);
  GF_ASSERT( poke_pos  < 6);

  recPoke = &record->pokemon[poke_pos];

  poke_data->monsno         = recPoke->monsno;
  poke_data->level          = recPoke->level;
  poke_data->personalRandom = recPoke->personalRandom;
  poke_data->idNumber       = recPoke->idNumber;
  poke_data->formNumber     = recPoke->formNumber;
  poke_data->sex            = recPoke->sex;

  GFL_STR_SetStringCodeOrderLength( poke_data->nickname, recPoke->nickname, POKENAME_BUFSIZE );
  GFL_STR_SetStringCodeOrderLength( poke_data->oyaname,  recPoke->oyaname,  OYANAME_BUFSIZE );

  for(i=0; i<4; i++)
  {
    poke_data->waza[i] = recPoke->waza[i];
  }
  
}


//=============================================================================================
/**
 * @brief   �a�����R�[�h(=�N���A���f�[�^)�̓��t��Ԃ�
 *
 * @param   record  
 * @param   date    ���t���������ރ|�C���^
 */
//=============================================================================================
void DendouRecord_GetDate( const DENDOU_RECORD* record, RTCDate* date )
{
  GF_ASSERT(record != NULL);

  date->year  = record->year;
  date->month = record->month;
  date->day   = record->day;

  // �j���͕ۑ����Ȃ����A�ꉞ�N���A���Ă���
  date->week = 0;
  
}



//------------------------------------------------------------------
/**
 * �a������f�[�^���̃��R�[�h������Ԃ�
 *
 * @param   data  �a������f�[�^�|�C���^
 *
 * @retval  u32   ����
 */
//------------------------------------------------------------------
u32 DendouData_GetRecordCount( const DENDOU_SAVEDATA* data )
{
  GF_ASSERT(data != NULL);
  GF_ASSERT(data->savePoint < DENDOU_RECORD_MAX);

  if( data->latestNumber >= DENDOU_RECORD_MAX )
  {
    return DENDOU_RECORD_MAX;
  }
  return data->latestNumber;
}
//------------------------------------------------------------------
/**
 * �a������f�[�^���A����ڂ̓a�����肩��Ԃ�
 *
 * @param   data    �a������f�[�^�|�C���^
 * @param   index   ���R�[�h�C���f�b�N�X�i0���ŐV�ŁA���ɋ����Ȃ��Ă����j
 *
 * @retval  u32     
 */
//------------------------------------------------------------------
u32 DendouData_GetRecordNumber( const DENDOU_SAVEDATA* data, int index )
{
  GF_ASSERT(data != NULL);
  GF_ASSERT(data->savePoint < DENDOU_RECORD_MAX);
  GF_ASSERT(index < DENDOU_RECORD_MAX);

  return data->latestNumber - index;
}
//------------------------------------------------------------------
/**
 * �a������f�[�^����A�w��̓a�����背�R�[�h�̃|�P�������擾
 *
 * @param   data    �a������f�[�^�|�C���^
 * @param   index   ���R�[�h�C���f�b�N�X�i0���ŐV�ŁA���ɋ����Ȃ��Ă����j
 *
 * @retval  u32   �a������p�[�e�B�[�̃|�P������
 */
//------------------------------------------------------------------
u32 DendouData_GetPokemonCount( const DENDOU_SAVEDATA* data, int index )
{
  u32 i;

  GF_ASSERT(data != NULL);
  GF_ASSERT(data->savePoint < DENDOU_RECORD_MAX);
  GF_ASSERT(index < DENDOU_RECORD_MAX);

  index = recIndex_to_datIndex( data, index );

  return DendouRecord_GetPokemonCount( &data->record[index] );
}
//------------------------------------------------------------------
/**
 * �a������f�[�^����w��̃��R�[�h�̃|�P�����f�[�^�擾
 *
 * @param   data    �a������f�[�^�|�C���^
 * @param   index   ���R�[�h�C���f�b�N�X�i0���ŐV�ŁA���ɋ����Ȃ��Ă����j
 * @param   poke_pos  ���̖ڂ̃|�P�������i0�`5�j
 * @param   poke_data �|�P�����f�[�^���󂯎��\���̂̃|�C���^
 */
//------------------------------------------------------------------
void DendouData_GetPokemonData( const DENDOU_SAVEDATA* data, int index, int poke_pos, DENDOU_POKEMON_DATA* poke_data )
{
  GF_ASSERT(data != NULL);
  GF_ASSERT(data->savePoint < DENDOU_RECORD_MAX);

  index = recIndex_to_datIndex( data, index );

  DendouRecord_GetPokemonData( &data->record[index], poke_pos, poke_data );
}
//------------------------------------------------------------------
/**
 * �a������f�[�^����A�w��̃��R�[�h�̓a��������t�擾
 *
 * @param   data    �a������f�[�^�|�C���^
 * @param   index   ���R�[�h�C���f�b�N�X�i0���ŐV�ŁA���ɋ����Ȃ��Ă����j
 * @param   date    ���t�f�[�^���󂯎��\���̂ւ̃|�C���^
 */
//------------------------------------------------------------------
void DendouData_GetDate( const DENDOU_SAVEDATA* data, int index, RTCDate* date )
{
  GF_ASSERT(data != NULL);
  GF_ASSERT(data->savePoint < DENDOU_RECORD_MAX);
  GF_ASSERT(index < DENDOU_RECORD_MAX);

  index = recIndex_to_datIndex( data, index );
  
  DendouRecord_GetDate( &data->record[index], date );
}

















//------------------------------------------------------------------
/**
 * ���R�[�h�C���f�b�N�X�i�V�������C���f�b�N�X�j���f�[�^�z��C���f�b�N�X�ɕϊ�
 *
 * @param   index   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int inline recIndex_to_datIndex( const DENDOU_SAVEDATA* data, int recIndex )
{
  int datIndex = (data->savePoint - 1 - recIndex);
  if( datIndex < 0 )
  {
    datIndex += DENDOU_RECORD_MAX;
  }
  return datIndex;
}
