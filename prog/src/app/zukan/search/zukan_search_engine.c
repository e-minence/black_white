//============================================================================
/**
 *  @file   zukan_search_engine.c
 *  @brief  �}�ӌ����G���W��
 *  @author Koji Kawada
 *  @data   2010.02.05
 *  @note   
 *  ���W���[�����FZUKAN_SEARCH_ENGINE
 */
//============================================================================
//#define DEBUG_KAWADA


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "savedata/zukan_savedata.h"
#include "poke_tool/poke_personal.h"

#include "zukan_search_engine.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "zukan_data.naix"

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// monsno�Aformno�̎擾
#define MONSNO(no) (no&0x03FF)
#define FORMNO(no) ((no&0xFC00)>>6)
/*
    |15 14 13 12 11 10|09 08 07 06 05 04 03 02 01 00|
    |     FORMNO      |           MONSNO            |
    |  0<=      <64   |        0<=      <1024       |
*/


// �������ԍ�
/*
# �������ԍ�
%initial_to_no_tbl =
(
  "�A"=>0,  "�C"=>1,  "�E"=>2,  "�G"=>3,  "�I"=>4,
  "�J"=>5,  "�L"=>6,  "�N"=>7,  "�P"=>8,  "�R"=>9,
  "�T"=>10, "�V"=>11, "�X"=>12, "�Z"=>13, "�\"=>14,
  "�^"=>15, "�`"=>16, "�c"=>17, "�e"=>18, "�g"=>19,
  "�i"=>20, "�j"=>21, "�k"=>22, "�l"=>23, "�m"=>24,
  "�n"=>25, "�q"=>26, "�t"=>27, "�w"=>28, "�z"=>29,
  "�}"=>30, "�~"=>31, "��"=>32, "��"=>33, "��"=>34,
  "��"=>35,           "��"=>36,           "��"=>37,
  "��"=>38, "��"=>39, "��"=>40, "��"=>41, "��"=>42,
  "��"=>43,                               "��"=>44,
  "��"=>45, 
);
$initial_to_no_unknown   = 46;  # �s���ȓ������̂Ƃ�
*/


// �`�ԍ�
/*
%style_enum =
(
  "�ی^"=>0,
  "�r�^"=>1,
  "���^"=>2,
  "�����^"=>3,
  "�l���^"=>4,
  "�l���H�^"=>5,
  "�W���^"=>6,
  "�����^"=>7,
  "�������^"=>8,
  "�񑫐K���^"=>9,
  "�񑫐l�^"=>10,
  "�񖇉H�^"=>11,
  "�����r���^"=>12,
  "�r�^"=>13,
);
$style_enum_empty   = 14;  # ���͂���Ă��Ȃ��Ƃ�
$style_enum_unknown = 15;  # %style_enum�ɗ񋓂���Ă��Ȃ��`�̂Ƃ�
*/


// �S�|�P�����A�S�t�H�����ɂ��Đ���ς݂̃f�[�^
static const int full_list_data[ZKNCOMM_LIST_SORT_ROW_MAX] =
{
	NARC_zukan_data_zkn_sort_chihou_dat,
	NARC_zukan_data_zkn_sort_aiueo_dat,
	NARC_zukan_data_zkn_sort_heavy_dat,
	NARC_zukan_data_zkn_sort_light_dat,
	NARC_zukan_data_zkn_sort_high_dat,
	NARC_zukan_data_zkn_sort_short_dat,
};


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
static u16 Zukan_Search_Engine_CreateFullList( u8 mode, u8 row, HEAPID heap_id, u16** list );
static void Zukan_Search_Engine_CreateExData( HEAPID heap_id,
                                    u16* chihou_num,           u16** chihou_list,
                                    u16* initial_num,          u8**  initial_list,
                                    u16* style_num,            u8**  style_list,
                                    u16* next_form_pos_num,    u16** next_form_pos_list );

static u8 GetStyle( u8* style_list, u16* next_form_pos_list, u16 monsno, u16 formno );


#ifdef DEBUG_KAWADA
static void Zukan_Search_Engine_MakeTestData( ZUKAN_SAVEDATA* zkn_sv, HEAPID heap_id );
#endif


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
#define BLOCK_FULL_FLAG_ON_AND_CONTINUE(i)       \
    {                                            \
      full_flag[i] = 1;                          \
      continue;                                  \
    }

//------------------------------------------------------------------
/**
 *  @brief          �������� 
 *
 *  @param[in]      zkn_sv      �}�ӃZ�[�u�f�[�^
 *  @param[in]      term        ��������
 *  @param[in]      heap_id     �q�[�vID
 *  @param[out]     list        ���݂�monsno�̗�(�߂�l0�̂Ƃ�NULL)
 *
 *  @retval         list�̃T�C�Y 
 *
 *  @note           list�͌Ăяo�����ŉ�����ĉ������B
 *
 */
//------------------------------------------------------------------
u16 ZUKAN_SEARCH_ENGINE_Search(
               const ZUKAN_SAVEDATA*      zkn_sv,
               const ZKNCOMM_LIST_SORT*   term,
               HEAPID                     heap_id,
               u16**                      list )
{
  // �S�|�P�����A�S�t�H�����ɂ��Đ���ς݂̃f�[�^
  u16  full_num;
  u16* full_list;
  u8*  full_flag;

  // �|�P�����p�[�\�i���ɂ͂Ȃ��ǉ��f�[�^
  u16  chihou_num;
  u16* chihou_list;         // �n���}�Ӕԍ��̗�((0<=monsno<=MONSNO_END)��(MONSNO_END+1)�̃f�[�^)
  u16  initial_num;
  u8*  initial_list;        // �������ԍ��̗�((0<=monsno<=MONSNO_END)��(MONSNO_END+1)�̃f�[�^)
  u16  style_num;
  u8*  style_list;          // �`�ԍ��̗�((0<=monsno<=MONSNO_END,TAMAGO,�ʃt�H����)��(MONSNO_END+1+1+�ʃt�H������)�̃f�[�^)
  u16  next_form_pos_num;   // ���̃t�H�����̃f�[�^�̈ʒu���Q�Ƃł����
  u16* next_form_pos_list;  // ((0<=monsno<=MONSNO_END,TAMAGO,�ʃt�H����)��(MONSNO_END+1+1+�ʃt�H������)�̃f�[�^)

  // �|�P�����p�[�\�i���f�[�^
  POKEMON_PERSONAL_DATA* ppd = NULL;
  
  // �������ʂ̃f�[�^
  u16  res_num;
  u16* res_list;

  // �J�E���g
  u16 i;
  u16 j;

#ifdef DEBUG_KAWADA
  Zukan_Search_Engine_MakeTestData( (ZUKAN_SAVEDATA*)zkn_sv, heap_id );
#endif

#ifdef DEBUG_KAWADA
  OS_Printf( "ZUKAN_SEARCH_ENGINE : Search Start!\n" );
  OS_Printf( "MONSNO_END = %d\n", MONSNO_END );
#endif

  // �S�|�P�����A�S�t�H�����ɂ��Đ���ς݂̃f�[�^�𓾂�
  full_num = Zukan_Search_Engine_CreateFullList( term->mode, term->row, heap_id, &full_list );
  // �t���O���X�g
  full_flag = GFL_HEAP_AllocClearMemory( heap_id, sizeof(u8)*full_num );  // full_num��0�Ƃ������Ƃ͂��蓾�Ȃ�

  // �|�P�����p�[�\�i���ɂ͂Ȃ��ǉ��f�[�^�𓾂�
  Zukan_Search_Engine_CreateExData( heap_id,
                                    &chihou_num,           &chihou_list,
                                    &initial_num,          &initial_list,
                                    &style_num,            &style_list,
                                    &next_form_pos_num,    &next_form_pos_list );

  // ���������ɂ���či�荞��
  for( i=0; i<full_num; i++ )
  {
    u16 monsno = MONSNO(full_list[i]);
    u16 formno = FORMNO(full_list[i]);

    u16 target_formno;

    GF_ASSERT_MSG( 1<=monsno && monsno<=MONSNO_END,  "ZUKAN_SEARCH_ENGINE : full_list��monsno���ُ�ł��B\n" );

    // �����Ă��Ȃ���΂Ȃ�Ȃ�
    if( !ZUKANSAVE_GetPokeSeeFlag( zkn_sv, monsno ) )
      BLOCK_FULL_FLAG_ON_AND_CONTINUE(i)

    // monsno�̃|�P�����̌��ݒ��ڂ��Ă���t�H�����ԍ��𓾂�
    {
      u32 sex;
      BOOL rare;
      u32 form;
      ZUKANSAVE_GetDrawData( (ZUKAN_SAVEDATA*)zkn_sv, monsno, &sex, &rare, &form, heap_id );
      target_formno = (u16)form;
    }

    // ���ݒ��ڂ��Ă���t�H�����ƈ�v���Ă��Ȃ���΂Ȃ�Ȃ�
    if( formno != target_formno )
      BLOCK_FULL_FLAG_ON_AND_CONTINUE(i)

    // �\�[�g�������A�d���̏ꍇ�̓|�P������߂܂��Ă��Ȃ���΂Ȃ�Ȃ�(�߂܂��Ȃ��ƍ����A�d�����������Ȃ��̂�)
    // ���������Ń|�P�^�C�v���ݒ肳��Ă���Ƃ��̓|�P������߂܂��Ă��Ȃ���΂Ȃ�Ȃ�(�߂܂��Ȃ��ƃ|�P�^�C�v���������Ȃ��̂�)
    if(
           ( ZKNCOMM_LIST_SORT_ROW_WEIGHT_HI <= term->row && term->row <= ZKNCOMM_LIST_SORT_ROW_HEIGHT_LOW )
        || ( term->type1 != ZKNCOMM_LIST_SORT_NONE || term->type2 != ZKNCOMM_LIST_SORT_NONE )
    )
    {
      // �߂܂��Ă��Ȃ���΂Ȃ�Ȃ�
      if( !ZUKANSAVE_GetPokeGetFlag( zkn_sv, monsno ) )
        BLOCK_FULL_FLAG_ON_AND_CONTINUE(i)
    }

    // �n���}�ӂ̏ꍇ�͒n���}�ӂɓo�ꂵ�Ă��Ȃ���΂Ȃ�Ȃ�
    if( term->mode == ZKNCOMM_LIST_SORT_MODE_LOCAL )
    {
      if( chihou_list[monsno] == 0 )
        BLOCK_FULL_FLAG_ON_AND_CONTINUE(i)
    }

    // ����������v���Ă��Ȃ���΂Ȃ�Ȃ�
    if( term->name != ZKNCOMM_LIST_SORT_NONE )
    {
      if( term->name != initial_list[monsno] )
        BLOCK_FULL_FLAG_ON_AND_CONTINUE(i)
    }

    // �|�P�����p�[�\�i���f�[�^���g�p
    {
      u8 type1;
      u8 type2;
      u8 color;

      if( ppd ) POKE_PERSONAL_CloseHandle( ppd );  // 1�O�̌㏈��
      ppd = POKE_PERSONAL_OpenHandle( monsno, formno, heap_id );
     
      // �^�C�v����v���Ă��Ȃ���΂Ȃ�Ȃ�
      type1 = (u8)POKE_PERSONAL_GetParam( ppd, POKEPER_ID_type1 );
      type2 = (u8)POKE_PERSONAL_GetParam( ppd, POKEPER_ID_type2 );
      if( term->type1 != ZKNCOMM_LIST_SORT_NONE && term->type2 != ZKNCOMM_LIST_SORT_NONE )  // �^�C�v1�Ɉ�v�A�^�C�v2�Ɉ�v
      {
        if(
            !(    ( type1 == term->type1 && type2 == term->type2 )
               || ( type1 == term->type2 && type2 == term->type1 )
            )
        )
          BLOCK_FULL_FLAG_ON_AND_CONTINUE(i)
      }
      else if( term->type1 != ZKNCOMM_LIST_SORT_NONE && term->type2 == ZKNCOMM_LIST_SORT_NONE )  // �^�C�v1�Ɉ�v
      {
        if( !( type1 == term->type1 || type2 == term->type1 ) )
          BLOCK_FULL_FLAG_ON_AND_CONTINUE(i)
      }
      else if( term->type1 == ZKNCOMM_LIST_SORT_NONE && term->type2 != ZKNCOMM_LIST_SORT_NONE )  // �^�C�v2�Ɉ�v
      {
        if( !( type1 == term->type2 || type2 == term->type2 ) )
          BLOCK_FULL_FLAG_ON_AND_CONTINUE(i)
      }

      // �F����v���Ă��Ȃ���΂Ȃ�Ȃ�
      if( term->color != ZKNCOMM_LIST_SORT_NONE )
      {
        color = (u8)POKE_PERSONAL_GetParam( ppd, POKEPER_ID_color );
        if( color != term->color )
          BLOCK_FULL_FLAG_ON_AND_CONTINUE(i)
      }
    }
    
    // �`����v���Ă��Ȃ���΂Ȃ�Ȃ�
    if( term->form != ZKNCOMM_LIST_SORT_NONE )
    {
      u8 style = GetStyle( style_list, next_form_pos_list, monsno, formno );
      if( style != term->form )
        BLOCK_FULL_FLAG_ON_AND_CONTINUE(i)
    }
  }

  if( ppd ) POKE_PERSONAL_CloseHandle( ppd );  // �Ō�̌㏈��

  // ��������
  {
    res_num = 0;
    for( i=0; i<full_num; i++ )
    {
      if( full_flag[i] == 0 ) res_num++;
    }
    if( res_num > 0 )
    {
      res_list = GFL_HEAP_AllocMemory( heap_id, sizeof(u16)*res_num );
      j = 0; 
      for( i=0; i<full_num; i++ )
      {
        if( full_flag[i] == 0 )
        {
          res_list[j] = MONSNO(full_list[i]);
          j++;
        }
      }
    }
    else
    {
      res_list = NULL;
    }
  }

  // ��n��
  GFL_HEAP_FreeMemory( full_list );
  GFL_HEAP_FreeMemory( full_flag );
  GFL_HEAP_FreeMemory( chihou_list );
  GFL_HEAP_FreeMemory( initial_list );
  GFL_HEAP_FreeMemory( style_list );
  GFL_HEAP_FreeMemory( next_form_pos_list );

#ifdef DEBUG_KAWADA
  OS_Printf( "ZUKAN_SEARCH_ENGINE : Search End!\n" );
#endif

#ifdef DEBUG_KAWADA
  {
    u16 i;
    OS_Printf( "ZUKAN_SEARCH_ENGINE_Search  Result\n" );
    OS_Printf( "num=%d\n", res_num );
    OS_Printf( "order  monsno\n" );
    for( i=0; i<res_num; i++ )
    {
      OS_Printf( "%3d    %3d\n", i, res_list[i] );
    }      
    OS_Printf( "End\n" );
  }
#endif

  // �Ԃ�
  *list = res_list;
  return res_num;
}

#undef BLOCK_FULL_FLAG_ON_AND_CONTINUE


//------------------------------------------------------------------
/**
 *  @brief          �S��/�n���}�ӂ̔ԍ������X�g�𓾂� 
 *
 *  @param[in]      mode        ��������
 *  @param[in]      heap_id     �q�[�vID
 *  @param[out]     list        monsno�̗�
 *
 *  @retval         list�̃T�C�Y 
 *
 *  @note           list�͌Ăяo�����ŉ�����ĉ������B
 *  @note           �����߂܂����Ɋ֌W�Ȃ��A���̐}�ӂɓo�ꂷ��S�|�P���������X�g�ɂ��܂��B
 *
 */
//------------------------------------------------------------------
u16 ZUKAN_GetNumberRow(
               u8       mode,  // ZKNCOMM_LIST_SORT_MODE_ZENKOKU / ZKNCOMM_LIST_SORT_MODE_LOCAL
               HEAPID   heap_id,
               u16**    list )
{
  
  if( mode == ZKNCOMM_LIST_SORT_MODE_ZENKOKU )
  {
    // �S���}�Ӕԍ� = monsno
    u16  full_num;
    u16* full_list;
    u16  i;
    
    full_list = GFL_HEAP_AllocMemory( heap_id, sizeof(u16)*MONSNO_END );
    for( i=0; i<MONSNO_END; i++ ) full_list[i] = i +1;
    full_num = MONSNO_END;
  
#ifdef DEBUG_KAWADA
    {
      u16 i;
      OS_Printf( "ZUKAN_GetNumberRow  Result\n" );
      OS_Printf( "num=%d\n", full_num );
      OS_Printf( "order  monsno\n" );
      for( i=0; i<full_num; i++ )
      {
        OS_Printf( "%3d    %3d\n", i, full_list[i] );
      }      
      OS_Printf( "End\n" );
    }
#endif

    *list = full_list;
    return full_num;
  }
  else
  {
    u16* chihou_appear_list;
    u16  chihou_appear_count;

    u16  chihou_appear_num;
    u16* zenkoku_to_chihou_list = POKE_PERSONAL_GetZenkokuToChihouArray( heap_id, &chihou_appear_num );

    u16  full_num;
    u16* full_list;
    u32  size;
    u16  i; 

    full_list = GFL_ARC_UTIL_LoadEx( ARCID_ZUKAN_DATA, NARC_zukan_data_zkn_sort_chihou_dat, FALSE, heap_id, &size );
    full_num = size / sizeof(u16);

    chihou_appear_list = GFL_HEAP_AllocMemory( heap_id, sizeof(u16)*chihou_appear_num );

    chihou_appear_count = 0;
    for( i=0; i<full_num; i++ )
    {
      u16  monsno = full_list[i];
      if( zenkoku_to_chihou_list[monsno] != POKEPER_CHIHOU_NO_NONE )
      {
        chihou_appear_list[chihou_appear_count] = monsno;
        chihou_appear_count++;
      }
    }

    GFL_HEAP_FreeMemory( full_list );
    GFL_HEAP_FreeMemory( zenkoku_to_chihou_list );

#ifdef DEBUG_KAWADA
    {
      u16 i;
      OS_Printf( "ZUKAN_GetNumberRow  Result\n" );
      OS_Printf( "num=%d\n", chihou_appear_num );
      OS_Printf( "order  monsno\n" );
      for( i=0; i<chihou_appear_num; i++ )
      {
        OS_Printf( "%3d    %3d\n", i, chihou_appear_list[i] );
      }      
      OS_Printf( "End\n" );
    }
#endif

    *list = chihou_appear_list;
    return chihou_appear_num;
  }
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// �S�|�P�����A�S�t�H�����ɂ��Đ���ς݂̃f�[�^�𓾂�(�s�v�ɂȂ�����Ăяo������GFL_HEAP_FreeMemory���ĉ�����)
//=====================================
static u16 Zukan_Search_Engine_CreateFullList( u8 mode, u8 row, HEAPID heap_id, u16** list )
{
  u16  full_num;
  u16* full_list;
  if( mode == ZKNCOMM_LIST_SORT_MODE_ZENKOKU && row == ZKNCOMM_LIST_SORT_ROW_NUMBER )
  {
    // �S���}�Ӕԍ� = monsno
    u16 i;
    full_list = GFL_HEAP_AllocMemory( heap_id, sizeof(u16)*MONSNO_END );
    for( i=0; i<MONSNO_END; i++ ) full_list[i] = i +1;
    full_num = MONSNO_END;
  }
  else
  {
    u32  size;
    full_list = GFL_ARC_UTIL_LoadEx( ARCID_ZUKAN_DATA, full_list_data[row], FALSE, heap_id, &size );
    full_num = size / sizeof(u16);
  }
#ifdef DEBUG_KAWADA
  OS_Printf( "full_num = %d\n", full_num );
#endif
  GF_ASSERT_MSG( full_num >= MONSNO_END, "ZUKAN_SEARCH_ENGINE : ����ς݃f�[�^�̌�������܂���B\n" );
  *list = full_list;
  return full_num;
}

//-------------------------------------
/// �|�P�����p�[�\�i���ɂ͂Ȃ��ǉ��f�[�^�𓾂�(�s�v�ɂȂ�����Ăяo������GFL_HEAP_FreeMemory���ĉ�����)
//=====================================
static void Zukan_Search_Engine_CreateExData( HEAPID heap_id,
                                    u16* chihou_num,           u16** chihou_list,
                                    u16* initial_num,          u8**  initial_list,
                                    u16* style_num,            u8**  style_list,
                                    u16* next_form_pos_num,    u16** next_form_pos_list )
{
  u32 size;
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_DATA, heap_id );
  
  *chihou_list = POKE_PERSONAL_GetZenkokuToChihouArray( heap_id, NULL );
  *chihou_num = MONSNO_END+1;

  *initial_list = GFL_ARCHDL_UTIL_LoadEx( handle, NARC_zukan_data_zkn_initial_dat, FALSE, heap_id, &size );
  *initial_num = size / sizeof(u8);

  *style_list = GFL_ARCHDL_UTIL_LoadEx( handle, NARC_zukan_data_zkn_style_dat, FALSE, heap_id, &size );
  *style_num = size / sizeof(u8);

  *next_form_pos_list = GFL_ARCHDL_UTIL_LoadEx( handle, NARC_zukan_data_zkn_next_form_pos_dat, FALSE, heap_id, &size );
  *next_form_pos_num = size / sizeof(u16);

  GFL_ARC_CloseDataHandle( handle );

#ifdef DEBUG_KAWADA
  OS_Printf( "chihou_num = %d\n", *chihou_num );
  OS_Printf( "initial_num = %d\n", *initial_num );
  OS_Printf( "style_num = %d\n", *style_num );
  OS_Printf( "next_form_pos_num = %d\n", *next_form_pos_num );
#endif

  GF_ASSERT_MSG( *chihou_num == MONSNO_END+1, "ZUKAN_SEARCH_ENGINE : �n���}�Ӕԍ��̗�̌����ُ�ł��B\n" );
  GF_ASSERT_MSG( *initial_num == MONSNO_END+1, "ZUKAN_SEARCH_ENGINE : �������ԍ��̗�̌����ُ�ł��B\n" );
  GF_ASSERT_MSG( *style_num > MONSNO_END+1+1, "ZUKAN_SEARCH_ENGINE : �`�ԍ��̗�̌�������܂���B\n" );
  GF_ASSERT_MSG( *next_form_pos_num > MONSNO_END+1+1, "ZUKAN_SEARCH_ENGINE : ���̃t�H�����̃f�[�^�̈ʒu���Q�Ƃł����̌�������܂���B\n" );
}

//-------------------------------------
/// �`�ԍ��𓾂�
//=====================================
static u8 GetStyle( u8* style_list, u16* next_form_pos_list, u16 monsno, u16 formno )
{
  u8  style;
  
  u16 formno_count = 0;
  u16 pos = monsno;

  while( formno_count != formno )
  {
    pos = next_form_pos_list[pos];
    if( pos == 0 ) break;
    formno_count++;
  }

  GF_ASSERT_MSG( pos > 0, "ZUKAN_SEARCH_ENGINE : �t�H�����ԍ����ُ�ł��B\n" );
  
  style = style_list[pos];
  return style;
}


#ifdef DEBUG_KAWADA
//-------------------------------------
/// �e�X�g�f�[�^���쐬����
//=====================================
static void Zukan_Search_Engine_MakeTestData( ZUKAN_SAVEDATA* zkn_sv, HEAPID heap_id )
{
  u16 i;
  for( i=1; i<=MONSNO_END; i++ )
  {
    POKEMON_PARAM* pp = PP_Create( i, 0, 0, heap_id ); 
    ZUKANSAVE_SetPokeGet( zkn_sv, pp );
    GFL_HEAP_FreeMemory( pp );
  }
}
#endif

