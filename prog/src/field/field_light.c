//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   field_light.c
 *  @brief    �t�B�[���h���C�g�V�X�e��
 *  @author   tomoya takahashi
 *  @date   2009.03.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "arc_def.h"
#include "message.naix"

#include "print/wordset.h"
#include "print/gf_font.h"
#include "print/printsys.h"

#include "msg/msg_d_tomoya.h"

#include "font/font.naix"


#include  "field_light.h"

//-----------------------------------------------------------------------------
/**
 *          �R�[�f�B���O�K��
 *    ���֐���
 *        �P�����ڂ͑啶������ȍ~�͏������ɂ���
 *    ���ϐ���
 *        �E�ϐ�����
 *            const�ɂ� c_ ��t����
 *            static�ɂ� s_ ��t����
 *            �|�C���^�ɂ� p_ ��t����
 *            �S�č��킳��� csp_ �ƂȂ�
 *        �E�O���[�o���ϐ�
 *            �P�����ڂ͑啶��
 *        �E�֐����ϐ�
 *            �������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------
#ifdef PM_DEBUG
//#define DEBUG_LIGHT_AUTO  // ���C�g�𒋑��߂�
#endif

#ifdef DEBUG_LIGHT_AUTO
static const u32 sc_DEBUG_LIGHT_AUTO_END_TIME[] = {
  0, 7200, 7700, 8100,
  9000, 14070, 21600, 30000, 34000,
  36200, 37200, 38200, 39000, 42000, 43200,
};
#endif


//-----------------------------------------------------------------------------
/**
 *          �萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
/// �V�[�P���X
//=====================================
enum {
  FIELD_LIGHT_SEQ_NORMAL,   // �ʏ�
  FIELD_LIGHT_SEQ_COLORFADE,  // �J���[�t�F�[�h�i�O���w��f�[�^�j
} ;

//-------------------------------------
/// ���C�g�e�[�u���ő吔
//=====================================
#define LIGHT_TABLE_MAX_SIZE  ( 16 )

//-------------------------------------
/// �J���[�t�F�[�h
//=====================================
enum {
  COLOR_FADE_SEQ_INIT,
  COLOR_FADE_SEQ_IN,
  COLOR_FADE_SEQ_OUT,
  COLOR_FADE_SEQ_END,
} ;


//-------------------------------------
/// �A�[�J�C�u�f�[�^
//=====================================
#define LIGHT_ARC_ID      ( ARCID_FIELD_LIGHT )
#define LIGHT_ARC_SEASON_NUM  ( PMSEASON_TOTAL )



#ifdef DEBUG_FIELD_LIGHT
// �f�o�b�N�Ǘ��V�[�P���X
enum{
  DEBUG_LIGHT_SEQ_LIGHT,
  DEBUG_LIGHT_SEQ_MATERIAL,
  DEBUG_LIGHT_SEQ_OTHER,

  DEBUG_LIGHT_SEQ_NUM,
};

// ���C�g�Ǘ����ڐ�
enum {
  DEBUG_CONT_LIGHT00_FLAG,
  DEBUG_CONT_LIGHT00_RGB,
  DEBUG_CONT_LIGHT00_VEC,

  DEBUG_CONT_LIGHT01_FLAG,
  DEBUG_CONT_LIGHT01_RGB,
  DEBUG_CONT_LIGHT01_VEC,

  DEBUG_CONT_LIGHT02_FLAG,
  DEBUG_CONT_LIGHT02_RGB,
  DEBUG_CONT_LIGHT02_VEC,

  DEBUG_CONT_LIGHT03_FLAG,
  DEBUG_CONT_LIGHT03_RGB,
  DEBUG_CONT_LIGHT03_VEC,

  DEBUG_CONT_LIGHT_NUM,
} ;

// �}�e���A���Ǘ�
enum {
  DEBUG_CONT_MATERIAL_DEFFUSE,
  DEBUG_CONT_MATERIAL_AMBIENT,
  DEBUG_CONT_MATERIAL_SPECULAR,
  DEBUG_CONT_MATERIAL_EMISSION,

  DEBUG_CONT_MATERIAL_NUM,
};

// ���̂��Ǘ�
enum {
  DEBUG_CONT_OTHER_FOG,
  DEBUG_CONT_OTHER_BG,

  DEBUG_CONT_OTHER_NUM,
};

#define DEBUG_PRINT_X ( 10 )


#endif  // DEBUG_FIELD_LIGHT

//-----------------------------------------------------------------------------
/**
 *          �\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
/// ���C�g�P�f�[�^
//=====================================
typedef struct {
  u32     endtime;
  u8      light_flag[4];
  GXRgb   light_color[4];
  VecFx16   light_vec[4];

  GXRgb   diffuse;
  GXRgb   ambient;
  GXRgb   specular;
  GXRgb   emission;
  GXRgb   fog_color;
  GXRgb   bg_color;

} LIGHT_DATA;

//-------------------------------------
/// RGB�t�F�[�h
//=====================================
typedef struct {
  u16 r_start;
  s16 r_dist;
  u16 g_start;
  s16 g_dist;
  u16 b_start;
  s16 b_dist;
} RGB_FADE;


//-------------------------------------
/// �����x�N�g���t�F�[�h
//=====================================
typedef struct {
  VecFx32 start;  // �J�n�x�N�g��
  VecFx32 normal; // ��]��
  u16 way_dist; // ��]��
} VEC_FADE;


//-------------------------------------
/// ���C�g�t�F�[�h
//=====================================
typedef struct {

  // �t�F�[�h���Ȃ����
  u32     endtime;
  u8      light_flag[4];

  RGB_FADE    light_color[4];
  VEC_FADE    light_vec[4];

  RGB_FADE    diffuse;
  RGB_FADE    ambient;
  RGB_FADE    specular;
  RGB_FADE    emission;
  RGB_FADE    fog_color;
  RGB_FADE    bg_color;

  u16       count;
  u16       count_max;
} LIGHT_FADE;


//-------------------------------------
/// �J���[�t�F�[�h
//=====================================
typedef struct {
  u16   seq;
  u16   insync;
  u16   outsync;
  GXRgb color;
} COLOR_FADE;


//-------------------------------------
/// �t�B�[���h���C�g�V�X�e��
//=====================================
struct _FIELD_LIGHT {
  u32   seq;  // �V�[�P���X

  // GFLIB���C�g�V�X�e��
  GFL_G3D_LIGHTSET* p_liblight;

  // FOG�V�X�e��
  FIELD_FOG_WORK* p_fog;

  // �f�[�^�o�b�t�@
  u32     data_num;     // �f�[�^��
  LIGHT_DATA data[LIGHT_TABLE_MAX_SIZE];        // �f�[�^
  u32     now_index;      // ���̔��f�C���f�b�N�X
  u16     default_lightno;


  // ���f���
  LIGHT_DATA  reflect_data;   // ���f�f�[�^
  u16     reflect_flag;   // ���f�t���O
  BOOL    change;       // �f�[�^�ݒ�t���O

  // ���f�t�F�[�h���
  LIGHT_FADE  fade;

  // �J���[�t�F�[�h
  COLOR_FADE  color_fade;

  // ���f����
  s32     time_second;

#ifdef DEBUG_FIELD_LIGHT
  BOOL  debug_flag;
  s16   debug_cont_seq;
  s16   debug_cont_select;
  s32   debug_print_req;

  u16   debug_rotate_xz;
  u16   debug_rotate_y;

  WORDSET*    p_debug_wordset;
  GFL_FONT*   p_debug_font;
  GFL_MSGDATA*  p_debug_msgdata;
  STRBUF*     p_debug_strbuff;
  STRBUF*     p_debug_strbuff_tmp;
#endif  // DEBUG_FIELD_LIGHT

};


//-----------------------------------------------------------------------------
/**
 *          �v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
/// �V�X�e��
//=====================================
static void FIELD_LIGHT_ReflectSub( const FIELD_LIGHT* cp_sys, FIELD_FOG_WORK* p_fog, GFL_G3D_LIGHTSET* p_liblight );
static void FIELD_LIGHT_ForceReflect( const FIELD_LIGHT* cp_sys, FIELD_FOG_WORK* p_fog, GFL_G3D_LIGHTSET* p_liblight );
static void FIELD_LIGHT_LoadData( FIELD_LIGHT* p_sys, u32 light_no, u32 heapID );
static void FIELD_LIGHT_LoadDataEx( FIELD_LIGHT* p_sys, u32 arcid, u32 dataid, u32 heapID );
static void FIELD_LIGHT_ReleaseData( FIELD_LIGHT* p_sys );
static s32  FIELD_LIGHT_SearchNowIndex( const FIELD_LIGHT* cp_sys, int rtc_second );


//-------------------------------------
/// RGB�t�F�[�h
//=====================================
static void RGB_FADE_Init( RGB_FADE* p_wk, GXRgb start, GXRgb end );
static GXRgb RGB_FADE_Calc( const RGB_FADE* cp_wk, u16 count, u16 count_max );

//-------------------------------------
/// �����x�N�g���t�F�[�h
//=====================================
static void VEC_FADE_Init( VEC_FADE* p_wk, const VecFx16* cp_start, const VecFx16* cp_end );
static void VEC_FADE_Calc( const VEC_FADE* cp_wk, u16 count, u16 count_max, VecFx16* p_ans );

//-------------------------------------
/// ���C�g�t�F�[�h
//=====================================
static void LIGHT_FADE_Init( LIGHT_FADE* p_wk, const LIGHT_DATA* cp_start, const LIGHT_DATA* cp_end );
static void LIGHT_FADE_InitEx( LIGHT_FADE* p_wk, const LIGHT_DATA* cp_start, const LIGHT_DATA* cp_end, u32 sync );
static void LIGHT_FADE_InitColor( LIGHT_FADE* p_wk, const LIGHT_DATA* cp_start, GXRgb end_color, u32 sync );
static void LIGHT_FADE_Main( LIGHT_FADE* p_wk );
static BOOL LIGHT_FADE_IsFade( const LIGHT_FADE* cp_wk );
static void LIGHT_FADE_GetData( const LIGHT_FADE* cp_wk, LIGHT_DATA* p_data );


//-------------------------------------
/// �J���[�t�F�[�h
//=====================================
static void COLOR_FADE_Init( COLOR_FADE* p_wk, u16 insync, u16 outsync, GXRgb color );
static BOOL COLOR_FADE_Main( COLOR_FADE* p_wk, LIGHT_FADE* p_fade, const LIGHT_DATA* cp_refdata, const LIGHT_DATA* cp_nowdata );
static BOOL COLOR_FADE_IsFade( const COLOR_FADE* cp_wk );


#ifdef DEBUG_FIELD_LIGHT
//-------------------------------------
/// ���C�g�f�o�b�N�@�\
//=====================================
static void DEBUG_LIGHT_ContLight( FIELD_LIGHT* p_wk );
static void DEBUG_LIGHT_ContMaterial( FIELD_LIGHT* p_wk );
static void DEBUG_LIGHT_ContOther( FIELD_LIGHT* p_wk );

static void DEBUG_LIGHT_PrintLight( FIELD_LIGHT* p_wk, GFL_BMPWIN* p_win );
static void DEBUG_LIGHT_PrintMaterial( FIELD_LIGHT* p_wk, GFL_BMPWIN* p_win );
static void DEBUG_LIGHT_PrintOther( FIELD_LIGHT* p_wk, GFL_BMPWIN* p_win );

static GXRgb DEBUG_LIGHT_ContRgb( GXRgb rgb );
static void DEBUG_LIGHT_InitContVec( FIELD_LIGHT* p_wk, const VecFx16* cp_vec );
static BOOL DEBUG_LIGHT_ContVec( FIELD_LIGHT* p_wk, VecFx16* p_vec );

static void DEBUG_LIGHT_SetWordsetRgb( FIELD_LIGHT* p_wk, u32 bufstart, GXRgb rgb );
static void DEBUG_LIGHT_SetWordsetVec( FIELD_LIGHT* p_wk, u32 bufstart, const VecFx16* cp_vec );
#endif // DEBUG_FIELD_LIGHT





//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���C�g  �V�X�e���쐬
 *
 *  @param  light_no    ���C�g�i���o�[
 *  @param  rtc_second    �b��
 *  @param  p_fog     �t�H�O�V�X�e��
 *  @param  p_liblight    ���C�g�Ǘ��V�X�e��
 *  @param  heapID      �q�[�v
 *
 *  @return �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
FIELD_LIGHT* FIELD_LIGHT_Create( u32 light_no, int rtc_second, FIELD_FOG_WORK* p_fog, GFL_G3D_LIGHTSET* p_liblight, u32 heapID )
{
  FIELD_LIGHT* p_sys;

  p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_LIGHT) );

  //  1/2�ōl����
  rtc_second /= 2;

  // ���C�g���ǂݍ���
  FIELD_LIGHT_LoadData( p_sys, light_no, heapID );

  // �f�[�^���f
  p_sys->reflect_flag = TRUE;

  // ��������ݒ�
  p_sys->now_index = FIELD_LIGHT_SearchNowIndex( p_sys, rtc_second );

  // �f�[�^���f
  GFL_STD_MemCopy( &p_sys->data[p_sys->now_index], &p_sys->reflect_data, sizeof(LIGHT_DATA) );
  p_sys->change = TRUE;

  // �t�H�O�V�X�e����ۑ�
  p_sys->p_fog = p_fog;

  // gflib
  p_sys->p_liblight = p_liblight;

  return p_sys;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���C�g  �V�X�e���j��
 *
 *  @param  p_sys     �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_Delete( FIELD_LIGHT* p_sys )
{
  // �f�[�^�j��
  FIELD_LIGHT_ReleaseData( p_sys );

  GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���C�g  �V�X�e�����C��
 *
 *  @param  p_sys     �V�X�e�����[�N
 *  @param  rtc_second    ����RTC���ԁi�b�P�ʁj
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_Main( FIELD_LIGHT* p_sys, int rtc_second )
{
  int starttime;

#ifdef DEBUG_FIELD_LIGHT
  if( p_sys->debug_flag ){
    return;
  }
#endif

  //  1/2�ōl����
  rtc_second /= 2;

  // ���C�g�f�[�^�̗L���`�F�b�N
  if( p_sys->data_num == 0 ){
    return ;
  }

  // ���C�g�f�[�^�ύX�`�F�b�N
  switch( p_sys->seq ){
  // �ʏ�
  case FIELD_LIGHT_SEQ_NORMAL:
    if( (p_sys->now_index - 1) < 0 ){
      starttime = 0;
    }else{
      starttime = p_sys->data[ p_sys->now_index-1 ].endtime;
    }

    //OS_TPrintf( "starttime %d endtime %d now %d\n", starttime, p_sys->data[ p_sys->now_index ].endtime, rtc_second );
    // ���̃e�[�u���͈͓̔�����Ȃ����`�F�b�N
    if( (starttime > rtc_second) ||
      (p_sys->data[ p_sys->now_index ].endtime <= rtc_second) ){

      // �ύX
      p_sys->now_index  = (p_sys->now_index + 1) % p_sys->data_num;

      // �t�F�[�h�ݒ�
      LIGHT_FADE_Init( &p_sys->fade, &p_sys->reflect_data, &p_sys->data[ p_sys->now_index ] );
    }
    break;

  // ���C�g�t�F�[�h�i�O���w��f�[�^�j
  case FIELD_LIGHT_SEQ_COLORFADE: // �J���[�t�F�[�h�i�O���w��f�[�^�j
    // �F�ɂ���
    if( COLOR_FADE_Main( &p_sys->color_fade, &p_sys->fade, &p_sys->reflect_data, &p_sys->data[ p_sys->now_index ] ) ){
      p_sys->seq = FIELD_LIGHT_SEQ_NORMAL;
    }
    break;

  }

  // ���C�g�t�F�[�h
  if( LIGHT_FADE_IsFade( &p_sys->fade ) ){
    LIGHT_FADE_Main( &p_sys->fade );
    LIGHT_FADE_GetData( &p_sys->fade, &p_sys->reflect_data );
    p_sys->change = TRUE;
  }

  // rtc���Ԃ�ۑ�
  p_sys->time_second = rtc_second;

}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�[�^���f�֐�
 *
 *  @param  p_sys
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_Reflect( FIELD_LIGHT* p_sys, BOOL force )
{
  // �f�[�^�ݒ菈����
  if( p_sys->change || force ){
    FIELD_LIGHT_ReflectSub( p_sys, p_sys->p_fog, p_sys->p_liblight );
    p_sys->change = FALSE;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���C�g  ���C�g���̕ύX
 *
 *  @param  light_no    ���C�g�i���o�[
 *  @param  heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_Change( FIELD_LIGHT* p_sys, u32 light_no, u32 heapID )
{
  // ���C�g�����ēǂݍ���
  FIELD_LIGHT_ReleaseData( p_sys );
  FIELD_LIGHT_LoadData( p_sys, light_no, heapID );

  // ��������ݒ�
  p_sys->now_index = FIELD_LIGHT_SearchNowIndex( p_sys, p_sys->time_second );

  // �t�F�[�h�J�n
  LIGHT_FADE_Init( &p_sys->fade, &p_sys->reflect_data, &p_sys->data[ p_sys->now_index ] );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���C�g  ���C�g���̕ύX�@�O�����
 *
 *  @param  arcid     �A�[�NID
 *  @param  dataid    �f�[�^ID
 *  @param  sync      �t�F�[�h�V���N���i�P�ȏ�j
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_ChangeEx( FIELD_LIGHT* p_sys, u32 arcid, u32 dataid, s32 sync, u32 heapID )
{
  // ���C�g�����ēǂݍ���
  FIELD_LIGHT_ReleaseData( p_sys );
  FIELD_LIGHT_LoadDataEx( p_sys, arcid, dataid, heapID );

  // ��������ݒ�
  p_sys->now_index = FIELD_LIGHT_SearchNowIndex( p_sys, p_sys->time_second );

  // �t�F�[�h�J�n
  LIGHT_FADE_InitEx( &p_sys->fade, &p_sys->reflect_data, &p_sys->data[ p_sys->now_index ], sync );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���C�g  ��{���C�g�ɖ߂�i�j
 *
 *  @param  p_sys
 *  @param  heapID
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_ReLoadDefault( FIELD_LIGHT* p_sys, u32 heapID )
{
  // ���C�g�����ēǂݍ���
  FIELD_LIGHT_ReleaseData( p_sys );
  FIELD_LIGHT_LoadData( p_sys, p_sys->default_lightno, heapID );

  // ��������ݒ�
  p_sys->now_index = FIELD_LIGHT_SearchNowIndex( p_sys, p_sys->time_second );

  // �t�F�[�h�J�n
  LIGHT_FADE_Init( &p_sys->fade, &p_sys->reflect_data, &p_sys->data[ p_sys->now_index ] );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J���[�t�F�[�h�J�n
 *
 *  @param  p_sys   �V�X�e�����[�N
 *  @param  color   �F
 *  @param  insync    �t�F�[�h�C���V���N��
 *  @param  outsync   �t�F�[�h�A�E�g�V���N��
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_COLORFADE_Start( FIELD_LIGHT* p_sys, GXRgb color, u32 insync, u32 outsync )
{
  COLOR_FADE_Init( &p_sys->color_fade, insync, outsync, color );
  p_sys->seq = FIELD_LIGHT_SEQ_COLORFADE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J���[�t�F�[�h�����܂�
 *
 *  @param  cp_sys    �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
BOOL FIELD_LIGHT_COLORFADE_IsFade( const FIELD_LIGHT* cp_sys )
{
  return COLOR_FADE_IsFade( &cp_sys->color_fade );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���C�g  ���f�t���O�ݒ�
 *
 *  @param  p_sys   ���[�N
 *  @param  flag    �t���O
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_SetReflect( FIELD_LIGHT* p_sys, BOOL flag )
{
  p_sys->reflect_flag = flag;
  if( p_sys->reflect_flag ){
    // �f�[�^���f
    p_sys->change = TRUE;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���C�g  ���f�t���O���擾
 *
 *  @param  cp_sys    ���[�N
 *
 *  @retval TRUE  ���f
 *  @retval FALSE ���f���Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_LIGHT_GetReflect( const FIELD_LIGHT* cp_sys )
{
  return cp_sys->reflect_flag;
}



//----------------------------------------------------------------------------
/**
 *  @brief  �����邩�擾
 *
 *  @param  cp_sys    �V�X�e�����[�N
 *
 *  @retval TRUE  ��
 *  @retval FALSE ����ȊO
 */
//-----------------------------------------------------------------------------
BOOL FIELD_LIGHT_GetNight( const FIELD_LIGHT* cp_sys )
{
  if( (cp_sys->time_second >= 34200) || (cp_sys->time_second < 7200) ){
    return TRUE;
  }
  return FALSE;
}


#ifdef DEBUG_FIELD_LIGHT
//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���C�g�f�o�b�N  ������
 *
 *  @param  p_sys   �V�X�e�����[�N
 *  @param  heapID    �q�[�v�h�c
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_DEBUG_Init( FIELD_LIGHT* p_sys, HEAPID heapID )
{
  GF_ASSERT( !p_sys->p_debug_wordset );
  GF_ASSERT( !p_sys->p_debug_msgdata );

  // ���[�h�Z�b�g�쐬
  p_sys->p_debug_wordset = WORDSET_Create( heapID );
  p_sys->p_debug_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_tomoya_dat, heapID );

  p_sys->p_debug_strbuff    = GFL_STR_CreateBuffer( 256, heapID );
  p_sys->p_debug_strbuff_tmp  = GFL_STR_CreateBuffer( 256, heapID );

  // �t�H���g�f�[�^
  p_sys->p_debug_font = GFL_FONT_Create(
    ARCID_FONT, NARC_font_large_gftr,
    GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

  p_sys->debug_flag = TRUE;

  p_sys->debug_print_req = TRUE;

  GFL_UI_KEY_SetRepeatSpeed( 4,8 );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h���C�g�@���[�N�j��
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_DEBUG_Exit( FIELD_LIGHT* p_sys )
{
  GF_ASSERT( p_sys->p_debug_wordset );
  GF_ASSERT( p_sys->p_debug_msgdata );


  // �t�H���g�f�[�^
  GFL_FONT_Delete( p_sys->p_debug_font );
  p_sys->p_debug_font = NULL;


  GFL_MSG_Delete( p_sys->p_debug_msgdata );
  p_sys->p_debug_msgdata = NULL;

  WORDSET_Delete( p_sys->p_debug_wordset );
  p_sys->p_debug_wordset = NULL;

  GFL_STR_DeleteBuffer( p_sys->p_debug_strbuff );
  p_sys->p_debug_strbuff = NULL;
  GFL_STR_DeleteBuffer( p_sys->p_debug_strbuff_tmp );
  p_sys->p_debug_strbuff_tmp = NULL;

  p_sys->debug_flag = FALSE;

  GFL_UI_KEY_SetRepeatSpeed( 8,15 );
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g���f�o�b�N�R���g���[��
 *
 *  @param  p_sys �V�X�e��
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_DEBUG_Control( FIELD_LIGHT* p_sys )
{
  p_sys->debug_print_req = FALSE;

  // LR�Ń��C�g��������
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){

    if( p_sys->now_index > 0 ){
      p_sys->now_index --;
    }else{
      p_sys->now_index = p_sys->data_num-1;
    }
    p_sys->debug_print_req = TRUE;

    // �f�[�^���f
    GFL_STD_MemCopy( &p_sys->data[p_sys->now_index], &p_sys->reflect_data, sizeof(LIGHT_DATA) );
    p_sys->change = TRUE;

  }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R ){

    p_sys->now_index = (p_sys->now_index + 1) % p_sys->data_num;
    p_sys->debug_print_req = TRUE;

    // �f�[�^���f
    GFL_STD_MemCopy( &p_sys->data[p_sys->now_index], &p_sys->reflect_data, sizeof(LIGHT_DATA) );
    p_sys->change = TRUE;
  }

  // �^�b�`�ŁA���ڂ�ύX
  {
    u32 x, y;
    if( GFL_UI_TP_GetPointTrg( &x, &y ) ){

      if( x<128 ){
        // ���ڃ��h��
        if( p_sys->debug_cont_seq > 0 ){
          p_sys->debug_cont_seq --;
        }else{
          p_sys->debug_cont_seq  = DEBUG_LIGHT_SEQ_NUM-1;
        }
        p_sys->debug_print_req    = TRUE;
        p_sys->debug_cont_select  = 0;
      }else{
        // ���ڃX�X��
        p_sys->debug_cont_seq   = (p_sys->debug_cont_seq+1) % DEBUG_LIGHT_SEQ_NUM;
        p_sys->debug_print_req    = TRUE;
        p_sys->debug_cont_select  = 0;
      }
    }
  }

  // ���C�g�R���g���[��
  switch( p_sys->debug_cont_seq ){
  case DEBUG_LIGHT_SEQ_LIGHT:
    DEBUG_LIGHT_ContLight( p_sys );
    break;
  case DEBUG_LIGHT_SEQ_MATERIAL:
    DEBUG_LIGHT_ContMaterial( p_sys );
    break;
  case DEBUG_LIGHT_SEQ_OTHER:
    DEBUG_LIGHT_ContOther( p_sys );
    break;

  default:
    GF_ASSERT(0);
    break;
  }

  if( p_sys->change ){
    FIELD_LIGHT_ForceReflect( p_sys, p_sys->p_fog, p_sys->p_liblight );
    p_sys->change = FALSE;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����r�b�g�}�b�v�E�B���h�E�ɕ\��
 *
 *  @param  cp_sys  �V�X�e��
 *  @param  p_win �E�B���h�E
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_DEBUG_PrintData( FIELD_LIGHT* p_sys, GFL_BMPWIN* p_win )
{
  if( p_sys->debug_print_req ){
    // �r�b�g�}�b�v�N���A
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_win ), 15 );

    //�`��
    switch( p_sys->debug_cont_seq ){
    case DEBUG_LIGHT_SEQ_LIGHT:
      DEBUG_LIGHT_PrintLight( p_sys, p_win );
      break;
    case DEBUG_LIGHT_SEQ_MATERIAL:
      DEBUG_LIGHT_PrintMaterial( p_sys, p_win );
      break;
    case DEBUG_LIGHT_SEQ_OTHER:
      DEBUG_LIGHT_PrintOther( p_sys, p_win );
      break;

    default:
      GF_ASSERT(0);
      break;
    }

  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g�Ǘ�����
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void DEBUG_LIGHT_ContLight( FIELD_LIGHT* p_wk )
{
  u32 cont_light = 0;
  GXRgb change_rgb;
  BOOL vec_cont_init = FALSE;

  // �㉺�A�I�����ڂ�ς���
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){

    if( p_wk->debug_cont_select > 0 ){
      p_wk->debug_cont_select --;
    }else{
      p_wk->debug_cont_select = DEBUG_CONT_LIGHT_NUM-1;
    }
    p_wk->debug_print_req = TRUE;

    // �x�N�g���Ǘ����@�̍X�V�v��
    vec_cont_init = TRUE;

  }else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){

    p_wk->debug_cont_select = (p_wk->debug_cont_select + 1) % DEBUG_CONT_LIGHT_NUM;
    p_wk->debug_print_req = TRUE;

    // �x�N�g���Ǘ����@�̍X�V�v��
    vec_cont_init = TRUE;
  }

  // �Ǘ����C�g����
  cont_light = p_wk->debug_cont_select / 3;

  //  �Ǘ�
  switch( p_wk->debug_cont_select ){
  case DEBUG_CONT_LIGHT00_FLAG:
  case DEBUG_CONT_LIGHT01_FLAG:
  case DEBUG_CONT_LIGHT02_FLAG:
  case DEBUG_CONT_LIGHT03_FLAG:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
      p_wk->reflect_data.light_flag[ cont_light ] ^= 1;
      p_wk->change = TRUE;
      p_wk->debug_print_req = TRUE;
    }
    break;

  case DEBUG_CONT_LIGHT00_RGB:
  case DEBUG_CONT_LIGHT01_RGB:
  case DEBUG_CONT_LIGHT02_RGB:
  case DEBUG_CONT_LIGHT03_RGB:

    change_rgb = DEBUG_LIGHT_ContRgb( p_wk->reflect_data.light_color[ cont_light ] );
    if( change_rgb != p_wk->reflect_data.light_color[ cont_light ] ){
      p_wk->reflect_data.light_color[ cont_light ] = change_rgb;
      p_wk->change = TRUE;
      p_wk->debug_print_req = TRUE;
    }
    break;

  case DEBUG_CONT_LIGHT00_VEC:
  case DEBUG_CONT_LIGHT01_VEC:
  case DEBUG_CONT_LIGHT02_VEC:
  case DEBUG_CONT_LIGHT03_VEC:
    if( vec_cont_init ){
      DEBUG_LIGHT_InitContVec( p_wk, &p_wk->reflect_data.light_vec[ cont_light ] );
    }

    if( DEBUG_LIGHT_ContVec( p_wk, &p_wk->reflect_data.light_vec[ cont_light ] ) ){
      p_wk->change = TRUE;
      p_wk->debug_print_req = TRUE;
    }
    break;

  default:
    GF_ASSERT(0);
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�e���A���Ǘ�����
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void DEBUG_LIGHT_ContMaterial( FIELD_LIGHT* p_wk )
{
  GXRgb change_rgb;

  // �㉺�A�I�����ڂ�ς���
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){

    if( p_wk->debug_cont_select > 0 ){
      p_wk->debug_cont_select --;
    }else{
      p_wk->debug_cont_select = DEBUG_CONT_MATERIAL_NUM-1;
    }
    p_wk->debug_print_req = TRUE;

  }else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){

    p_wk->debug_cont_select = (p_wk->debug_cont_select + 1) % DEBUG_CONT_MATERIAL_NUM;
    p_wk->debug_print_req = TRUE;
  }

  //  �Ǘ�
  switch( p_wk->debug_cont_select ){
  case DEBUG_CONT_MATERIAL_DEFFUSE:
    change_rgb = DEBUG_LIGHT_ContRgb( p_wk->reflect_data.diffuse );
    if( change_rgb != p_wk->reflect_data.diffuse ){
      p_wk->reflect_data.diffuse = change_rgb;
      p_wk->change = TRUE;
      p_wk->debug_print_req = TRUE;
    }
    break;

  case DEBUG_CONT_MATERIAL_AMBIENT:
    change_rgb = DEBUG_LIGHT_ContRgb( p_wk->reflect_data.ambient );
    if( change_rgb != p_wk->reflect_data.ambient ){
      p_wk->reflect_data.ambient = change_rgb;
      p_wk->change = TRUE;
      p_wk->debug_print_req = TRUE;
    }
    break;

  case DEBUG_CONT_MATERIAL_SPECULAR:
    change_rgb = DEBUG_LIGHT_ContRgb( p_wk->reflect_data.specular );
    if( change_rgb != p_wk->reflect_data.specular ){
      p_wk->reflect_data.specular = change_rgb;
      p_wk->change = TRUE;
      p_wk->debug_print_req = TRUE;
    }
    break;

  case DEBUG_CONT_MATERIAL_EMISSION:
    change_rgb = DEBUG_LIGHT_ContRgb( p_wk->reflect_data.emission );
    if( change_rgb != p_wk->reflect_data.emission ){
      p_wk->reflect_data.emission = change_rgb;
      p_wk->change = TRUE;
      p_wk->debug_print_req = TRUE;
    }
    break;

  default:
    GF_ASSERT(0);
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���̑��Ǘ�����
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void DEBUG_LIGHT_ContOther( FIELD_LIGHT* p_wk )
{
  GXRgb change_rgb;

  // �㉺�A�I�����ڂ�ς���
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){

    if( p_wk->debug_cont_select > 0 ){
      p_wk->debug_cont_select --;
    }else{
      p_wk->debug_cont_select = DEBUG_CONT_OTHER_NUM-1;
    }
    p_wk->debug_print_req = TRUE;

  }else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){

    p_wk->debug_cont_select = (p_wk->debug_cont_select + 1) % DEBUG_CONT_OTHER_NUM;
    p_wk->debug_print_req = TRUE;
  }

  //  �Ǘ�
  switch( p_wk->debug_cont_select ){
  case DEBUG_CONT_OTHER_FOG:
    change_rgb = DEBUG_LIGHT_ContRgb( p_wk->reflect_data.fog_color );
    if( change_rgb != p_wk->reflect_data.fog_color ){
      p_wk->reflect_data.fog_color = change_rgb;
      p_wk->change = TRUE;
      p_wk->debug_print_req = TRUE;
    }
    break;

  case DEBUG_CONT_OTHER_BG:
    change_rgb = DEBUG_LIGHT_ContRgb( p_wk->reflect_data.bg_color );
    if( change_rgb != p_wk->reflect_data.bg_color ){
      p_wk->reflect_data.bg_color = change_rgb;
      p_wk->change = TRUE;
      p_wk->debug_print_req = TRUE;
    }
    break;

  default:
    GF_ASSERT(0);
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g���̕`��
 */
//-----------------------------------------------------------------------------
static void DEBUG_LIGHT_PrintLight( FIELD_LIGHT* p_wk, GFL_BMPWIN* p_win )
{
  int i;

  //  ����t���[������\��
  WORDSET_RegisterNumber( p_wk->p_debug_wordset, 0, p_wk->reflect_data.endtime, 5, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  // �v�����g
  GFL_MSG_GetString( p_wk->p_debug_msgdata, D_TOMOYA_LIGHT_FRAME, p_wk->p_debug_strbuff_tmp );

  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 160, 0, p_wk->p_debug_strbuff, p_wk->p_debug_font );

  // ���C�g������������
  for( i=0; i<4; i++ ){

    // ���C�g�i���o�[
    WORDSET_RegisterNumber( p_wk->p_debug_wordset, 0, i, 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );

    // ���C�g�n�m�E�n�e�e
    WORDSET_RegisterNumber( p_wk->p_debug_wordset, 1, p_wk->reflect_data.light_flag[i], 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );

    // RGB
    DEBUG_LIGHT_SetWordsetRgb( p_wk, 2, p_wk->reflect_data.light_color[i] );

    // ����
    DEBUG_LIGHT_SetWordsetVec( p_wk, 5, &p_wk->reflect_data.light_vec[i] );

    // �v�����g
    GFL_MSG_GetString( p_wk->p_debug_msgdata, D_TOMOYA_LIGHT_FLAG, p_wk->p_debug_strbuff_tmp );

    WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), DEBUG_PRINT_X, i*(3*16), p_wk->p_debug_strbuff, p_wk->p_debug_font );

  }

  // �J�[�\���̕`��
  GFL_BMP_Fill( GFL_BMPWIN_GetBmp( p_win ), 1, p_wk->debug_cont_select*16+2, 8, 8, 1 );

  GFL_BMPWIN_TransVramCharacter( p_win );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�e���A�����̕`��
 */
//-----------------------------------------------------------------------------
static void DEBUG_LIGHT_PrintMaterial( FIELD_LIGHT* p_wk, GFL_BMPWIN* p_win )
{
  //  ����t���[������\��
  WORDSET_RegisterNumber( p_wk->p_debug_wordset, 0, p_wk->reflect_data.endtime, 5, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  GFL_MSG_GetString( p_wk->p_debug_msgdata, D_TOMOYA_LIGHT_FRAME, p_wk->p_debug_strbuff_tmp );
  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), DEBUG_PRINT_X, 72, p_wk->p_debug_strbuff, p_wk->p_debug_font );



  //  �f�B�t���[�Y
  DEBUG_LIGHT_SetWordsetRgb( p_wk, 0, p_wk->reflect_data.diffuse );
  GFL_MSG_GetString( p_wk->p_debug_msgdata, D_TOMOYA_DIFFUSE, p_wk->p_debug_strbuff_tmp );
  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), DEBUG_PRINT_X, 0, p_wk->p_debug_strbuff, p_wk->p_debug_font );

  //  �A���r�G���g
  DEBUG_LIGHT_SetWordsetRgb( p_wk, 0, p_wk->reflect_data.ambient );
  GFL_MSG_GetString( p_wk->p_debug_msgdata, D_TOMOYA_AMBIENT, p_wk->p_debug_strbuff_tmp );
  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), DEBUG_PRINT_X, 16, p_wk->p_debug_strbuff, p_wk->p_debug_font );

  //  �X�y�L�����[
  DEBUG_LIGHT_SetWordsetRgb( p_wk, 0, p_wk->reflect_data.specular );
  GFL_MSG_GetString( p_wk->p_debug_msgdata, D_TOMOYA_SPECULAR, p_wk->p_debug_strbuff_tmp );
  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), DEBUG_PRINT_X, 32, p_wk->p_debug_strbuff, p_wk->p_debug_font );

  //  �G�~�b�V����
  DEBUG_LIGHT_SetWordsetRgb( p_wk, 0, p_wk->reflect_data.emission );
  GFL_MSG_GetString( p_wk->p_debug_msgdata, D_TOMOYA_EMISSION, p_wk->p_debug_strbuff_tmp );
  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), DEBUG_PRINT_X, 48, p_wk->p_debug_strbuff, p_wk->p_debug_font );


  // �J�[�\���`��
  GFL_BMP_Fill( GFL_BMPWIN_GetBmp( p_win ), 1, p_wk->debug_cont_select*16+2, 8, 8, 1 );

  GFL_BMPWIN_TransVramCharacter( p_win );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���̑����̕`��
 */
//-----------------------------------------------------------------------------
static void DEBUG_LIGHT_PrintOther( FIELD_LIGHT* p_wk, GFL_BMPWIN* p_win )
{
  //  ����t���[������\��
  WORDSET_RegisterNumber( p_wk->p_debug_wordset, 0, p_wk->reflect_data.endtime, 5, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  GFL_MSG_GetString( p_wk->p_debug_msgdata, D_TOMOYA_LIGHT_FRAME, p_wk->p_debug_strbuff_tmp );
  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), DEBUG_PRINT_X, 72, p_wk->p_debug_strbuff, p_wk->p_debug_font );



  //  �t�H�O
  DEBUG_LIGHT_SetWordsetRgb( p_wk, 0, p_wk->reflect_data.fog_color );
  GFL_MSG_GetString( p_wk->p_debug_msgdata, D_TOMOYA_FOG, p_wk->p_debug_strbuff_tmp );
  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), DEBUG_PRINT_X, 0, p_wk->p_debug_strbuff, p_wk->p_debug_font );

  // �w��
  DEBUG_LIGHT_SetWordsetRgb( p_wk, 0, p_wk->reflect_data.bg_color );
  GFL_MSG_GetString( p_wk->p_debug_msgdata, D_TOMOYA_BG, p_wk->p_debug_strbuff_tmp );
  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), DEBUG_PRINT_X, 16, p_wk->p_debug_strbuff, p_wk->p_debug_font );

  // �J�[�\���`��
  GFL_BMP_Fill( GFL_BMPWIN_GetBmp( p_win ), 1, p_wk->debug_cont_select*16+2, 8, 8, 1 );

  GFL_BMPWIN_TransVramCharacter( p_win );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �F�Ǘ�
 *
 *  @param  rgb   �F
 *
 *  @return �F���
 */
//-----------------------------------------------------------------------------
static GXRgb DEBUG_LIGHT_ContRgb( GXRgb rgb )
{
  u8 r,g,b;

  r = (rgb & GX_RGB_R_MASK )>> GX_RGB_R_SHIFT;
  g = (rgb & GX_RGB_G_MASK )>> GX_RGB_G_SHIFT;
  b = (rgb & GX_RGB_B_MASK )>> GX_RGB_B_SHIFT;

  // R
  if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_Y ){
    r = (r+1) % 32;
  }
  // G
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_X ){
    g = (g+1) % 32;
  }
  // B
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A ){
    b = (b+1) % 32;
  }

  return GX_RGB( r, g, b );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �x�N�g���Ǘ��J�n����
 *
 *  @param  p_wk  ���[�N
 *  @param  cp_vec  �x�N�g��
 */
//-----------------------------------------------------------------------------
static void DEBUG_LIGHT_InitContVec( FIELD_LIGHT* p_wk, const VecFx16* cp_vec )
{
  fx32 xz_dist;

  p_wk->debug_rotate_xz = FX_Atan2Idx( cp_vec->x, cp_vec->z );
  xz_dist   = FX_Sqrt( FX_Mul( cp_vec->z, cp_vec->z ) + FX_Mul( cp_vec->x, cp_vec->x ) );
  p_wk->debug_rotate_y  = FX_Atan2Idx( cp_vec->y, xz_dist );

}

//----------------------------------------------------------------------------
/**
 *  @brief  �x�N�g���Ǘ�
 *
 *  @param  p_vec �x�N�g��
 */
//-----------------------------------------------------------------------------
#define DEBUG_LIGHT_CONT_VEC_ROTATE_ADD ( 182 )
static BOOL DEBUG_LIGHT_ContVec( FIELD_LIGHT* p_wk, VecFx16* p_vec )
{
  BOOL change = FALSE;
  MtxFx33 mtx_xz, mtx_y;

  // �㉺���E��]
  // X ��
  if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_X ){
    // Y�����́A�X�O�x�`�Q�V�O�x�܂ł����������Ȃ�
    if( p_wk->debug_rotate_y >= ((0xffff/4) + DEBUG_LIGHT_CONT_VEC_ROTATE_ADD) ){
      p_wk->debug_rotate_y -= DEBUG_LIGHT_CONT_VEC_ROTATE_ADD;
    }else{
      p_wk->debug_rotate_y = (0xffff/4);
    }
    change = TRUE;
  }
  // B ��
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_B ){

    // Y�����́A�P�W�O�x�`�R�U�O�x�܂ł����������Ȃ�
    if( (p_wk->debug_rotate_y+DEBUG_LIGHT_CONT_VEC_ROTATE_ADD) < ((0xffff/4)*3)  ){
      p_wk->debug_rotate_y += DEBUG_LIGHT_CONT_VEC_ROTATE_ADD;
    }else{
      p_wk->debug_rotate_y = ((0xffff/4)*3);
    }

    change = TRUE;
  }
  // Y ��
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_Y ){

    p_wk->debug_rotate_xz -= DEBUG_LIGHT_CONT_VEC_ROTATE_ADD;
    change = TRUE;
  }
  // A �E
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A ){

    p_wk->debug_rotate_xz += DEBUG_LIGHT_CONT_VEC_ROTATE_ADD;
    change = TRUE;
  }

  if( change ){

    p_vec->y = FX_Mul( FX_SinIdx( p_wk->debug_rotate_y ), FX32_ONE );
    p_vec->x = FX_Mul( FX_CosIdx( p_wk->debug_rotate_y ), FX32_ONE );
    p_vec->z = FX_Mul( FX_CosIdx( p_wk->debug_rotate_xz ), p_vec->x );
    p_vec->x = FX_Mul( FX_SinIdx( p_wk->debug_rotate_xz ), p_vec->x );
  }

  return change;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�Z�b�g�ɂq�f�a������
 *
 *  @param  p_wk    ���[�N
 *  @param  bufstart  �o�b�t�@�J�n�C���f�b�N�X
 *  @param  rgb     �q�f�a
 */
//-----------------------------------------------------------------------------
static void DEBUG_LIGHT_SetWordsetRgb( FIELD_LIGHT* p_wk, u32 bufstart, GXRgb rgb )
{
  u8 r,g,b;

  r = (rgb & GX_RGB_R_MASK )>> GX_RGB_R_SHIFT;
  g = (rgb & GX_RGB_G_MASK )>> GX_RGB_G_SHIFT;
  b = (rgb & GX_RGB_B_MASK )>> GX_RGB_B_SHIFT;

  // RGB��ݒ�
  WORDSET_RegisterNumber( p_wk->p_debug_wordset, bufstart+0, r, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( p_wk->p_debug_wordset, bufstart+1, g, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( p_wk->p_debug_wordset, bufstart+2, b, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�Z�b�g�Ƀx�N�g��������
 *
 *  @param  p_wk    ���[�N
 *  @param  bufstart  �o�b�t�@�J�n�C���f�b�N�X
 *  @param  cp_vec    �x�N�g��
 */
//-----------------------------------------------------------------------------
static void DEBUG_LIGHT_SetWordsetVec( FIELD_LIGHT* p_wk, u32 bufstart, const VecFx16* cp_vec )
{
  WORDSET_RegisterNumber( p_wk->p_debug_wordset, bufstart+0, cp_vec->x, 5, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( p_wk->p_debug_wordset, bufstart+1, cp_vec->y, 5, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( p_wk->p_debug_wordset, bufstart+2, cp_vec->z, 5, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
}



#endif // DEBUG_FIELD_LIGHT






//-----------------------------------------------------------------------------
/**
 *      �v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g���̐ݒ�
 *
 *  @param  cp_sys  �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_LIGHT_ReflectSub( const FIELD_LIGHT* cp_sys, FIELD_FOG_WORK* p_fog, GFL_G3D_LIGHTSET* p_liblight )
{
  int i;
  VecFx16 dummy_vec = {0};
  u16   dummy_col = 0;

  if( cp_sys->reflect_flag ){

    for( i=0; i<4; i++ ){
      if( cp_sys->reflect_data.light_flag[i] ){
        GFL_G3D_LIGHT_SetVec( p_liblight, i, (VecFx16*)&cp_sys->reflect_data.light_vec[i] );
        GFL_G3D_LIGHT_SetColor( p_liblight, i, (u16*)&cp_sys->reflect_data.light_color[i] );
      }else{

        GFL_G3D_LIGHT_SetVec( p_liblight, i, &dummy_vec );
        GFL_G3D_LIGHT_SetColor( p_liblight, i, &dummy_col );
      }
    }

    NNS_G3dGlbMaterialColorDiffAmb( cp_sys->reflect_data.diffuse,
        cp_sys->reflect_data.ambient, TRUE );

    NNS_G3dGlbMaterialColorSpecEmi( cp_sys->reflect_data.specular,
        cp_sys->reflect_data.emission, FALSE );

    FIELD_FOG_SetColorRgb( p_fog, cp_sys->reflect_data.fog_color );

    G3X_SetClearColor(cp_sys->reflect_data.bg_color,31,0x7fff,0,FALSE);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �������C�g���f
 *
 *  @param  cp_sys      �V�X�e�����[�N
 *  @param  p_fog     �t�H�O���[�N
 *  @param  p_liblight    ���C�g���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_LIGHT_ForceReflect( const FIELD_LIGHT* cp_sys, FIELD_FOG_WORK* p_fog, GFL_G3D_LIGHTSET* p_liblight )
{
  int i;
  VecFx16 dummy_vec = {0};
  u16   dummy_col = 0;

  for( i=0; i<4; i++ ){
    if( cp_sys->reflect_data.light_flag[i] ){
      GFL_G3D_LIGHT_SetVec( p_liblight, i, (VecFx16*)&cp_sys->reflect_data.light_vec[i] );
      GFL_G3D_LIGHT_SetColor( p_liblight, i, (u16*)&cp_sys->reflect_data.light_color[i] );
    }else{

      GFL_G3D_LIGHT_SetVec( p_liblight, i, &dummy_vec );
      GFL_G3D_LIGHT_SetColor( p_liblight, i, &dummy_col );
    }
  }

  NNS_G3dGlbMaterialColorDiffAmb( cp_sys->reflect_data.diffuse,
      cp_sys->reflect_data.ambient, TRUE );

  NNS_G3dGlbMaterialColorSpecEmi( cp_sys->reflect_data.specular,
      cp_sys->reflect_data.emission, FALSE );

  FIELD_FOG_SetColorRgb( p_fog, cp_sys->reflect_data.fog_color );

  G3X_SetClearColor(cp_sys->reflect_data.bg_color,31,0x7fff,0,FALSE);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g���̓ǂݍ��ݏ���
 *
 *  @param  p_sys   �V�X�e�����[�N
 *  @param  light_no  ���C�g�i���o�[
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_LIGHT_LoadData( FIELD_LIGHT* p_sys, u32 light_no, u32 heapID )
{
  // ��{�G�߁E���C�g�i���o�[�ݒ�
  p_sys->default_lightno  = light_no;

  FIELD_LIGHT_LoadDataEx( p_sys, LIGHT_ARC_ID, light_no, heapID );


}

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g���̓ǂݍ��ݏ���
 *
 *  @param  p_sys     �V�X�e�����[�N
 *  @param  arcid     �A�[�J�C�uID
 *  @param  dataid      �f�[�^ID
 *  @param  heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_LIGHT_LoadDataEx( FIELD_LIGHT* p_sys, u32 arcid, u32 dataid, u32 heapID )
{
  u32 size;
  int i, j;

  size = GFL_ARC_GetDataSize( arcid, dataid );
  p_sys->data_num = size / sizeof(LIGHT_DATA);
  GF_ASSERT( p_sys->data_num < LIGHT_TABLE_MAX_SIZE );

  GFL_ARC_LoadData( p_sys->data, arcid, dataid );


  // �����x�N�g���̒l���ꉞ�P�ʃx�N�g���ɂ���B
  for( i=0; i<p_sys->data_num; i++ ){

    for( j=0; j<4; j++ ){
      VEC_Fx16Normalize( &p_sys->data[i].light_vec[j], &p_sys->data[i].light_vec[j] );
    }

#ifdef DEBUG_LIGHT_AUTO
    // ���Ԃ̕������㏑��
    p_sys->data[i].endtime = sc_DEBUG_LIGHT_AUTO_END_TIME[i];
#endif

#if 0
    // �f�[�^�̃f�o�b�N�\��
    OS_TPrintf( "data number %d\n", i );
    OS_TPrintf( "endtime  %d\n", p_sys->data[i].endtime );
    for( j=0; j<4; j++ ){
      OS_TPrintf( "light_flag %d\n", p_sys->data[i].light_flag[i] );
      OS_TPrintf( "light_color r=%d g=%d b=%d\n",
          (p_sys->data[i].light_color[i] & GX_RGB_R_MASK)>>GX_RGB_R_SHIFT,
          (p_sys->data[i].light_color[i] & GX_RGB_G_MASK)>>GX_RGB_G_SHIFT,
          (p_sys->data[i].light_color[i] & GX_RGB_B_MASK)>>GX_RGB_B_SHIFT );
      OS_TPrintf( "light_vec x=0x%x y=0x%x z=0x%x\n",
          p_sys->data[i].light_vec[i].x,
          p_sys->data[i].light_vec[i].y,
          p_sys->data[i].light_vec[i].z );
    }

    OS_TPrintf( "diffuse r=%d g=%d b=%d\n",
        (p_sys->data[i].diffuse & GX_RGB_R_MASK)>>GX_RGB_R_SHIFT,
        (p_sys->data[i].diffuse & GX_RGB_G_MASK)>>GX_RGB_G_SHIFT,
        (p_sys->data[i].diffuse & GX_RGB_B_MASK)>>GX_RGB_B_SHIFT );

    OS_TPrintf( "ambient r=%d g=%d b=%d\n",
        (p_sys->data[i].ambient & GX_RGB_R_MASK)>>GX_RGB_R_SHIFT,
        (p_sys->data[i].ambient & GX_RGB_G_MASK)>>GX_RGB_G_SHIFT,
        (p_sys->data[i].ambient & GX_RGB_B_MASK)>>GX_RGB_B_SHIFT );

    OS_TPrintf( "specular r=%d g=%d b=%d\n",
        (p_sys->data[i].specular & GX_RGB_R_MASK)>>GX_RGB_R_SHIFT,
        (p_sys->data[i].specular & GX_RGB_G_MASK)>>GX_RGB_G_SHIFT,
        (p_sys->data[i].specular & GX_RGB_B_MASK)>>GX_RGB_B_SHIFT );

    OS_TPrintf( "emission r=%d g=%d b=%d\n",
        (p_sys->data[i].emission & GX_RGB_R_MASK)>>GX_RGB_R_SHIFT,
        (p_sys->data[i].emission & GX_RGB_G_MASK)>>GX_RGB_G_SHIFT,
        (p_sys->data[i].emission & GX_RGB_B_MASK)>>GX_RGB_B_SHIFT );

    OS_TPrintf( "fog_color r=%d g=%d b=%d\n",
        (p_sys->data[i].fog_color & GX_RGB_R_MASK)>>GX_RGB_R_SHIFT,
        (p_sys->data[i].fog_color & GX_RGB_G_MASK)>>GX_RGB_G_SHIFT,
        (p_sys->data[i].fog_color & GX_RGB_B_MASK)>>GX_RGB_B_SHIFT );
#endif
  }

}

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g���̔j���@����
 *
 *  @param  p_sys   �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_LIGHT_ReleaseData( FIELD_LIGHT* p_sys )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  rtc�b�̂Ƃ��̃f�[�^�C���f�b�N�X���擾
 *
 *  @param  cp_sys      ���[�N
 *  @param  rtc_second    rtc�b
 *
 *  @return �f�[�^�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static s32  FIELD_LIGHT_SearchNowIndex( const FIELD_LIGHT* cp_sys, int rtc_second )
{
  int i;

  for( i=0; i<cp_sys->data_num; i++ ){

    if( cp_sys->data[i].endtime > rtc_second ){
      return i;
    }
  }

  // ���肦�Ȃ�
  GF_ASSERT( 0 );
  return 0;
}


//----------------------------------------------------------------------------
/**
 *  @brief  RGB�t�F�[�h ������
 *
 *  @param  p_wk    ���[�N
 *  @param  start   �J�n�F
 *  @param  end     �I���F
 */
//-----------------------------------------------------------------------------
static void RGB_FADE_Init( RGB_FADE* p_wk, GXRgb start, GXRgb end )
{
  p_wk->r_start   = (start & GX_RGB_R_MASK )>> GX_RGB_R_SHIFT;
  p_wk->r_dist    = ((end & GX_RGB_R_MASK )>> GX_RGB_R_SHIFT) - p_wk->r_start;
  p_wk->g_start   = (start & GX_RGB_G_MASK )>> GX_RGB_G_SHIFT;
  p_wk->g_dist    = ((end & GX_RGB_G_MASK )>> GX_RGB_G_SHIFT) - p_wk->g_start;
  p_wk->b_start   = (start & GX_RGB_B_MASK )>> GX_RGB_B_SHIFT;
  p_wk->b_dist    = ((end & GX_RGB_B_MASK )>> GX_RGB_B_SHIFT) - p_wk->b_start;
}

//----------------------------------------------------------------------------
/**
 *  @brief  RGB�t�F�[�h �v�Z����
 *
 *  @param  cp_wk     ���[�N
 *  @param  count     �J�E���g�l
 *  @param  count_max   �J�E���g�ő�l
 *
 *  @return RGB�J���[
 */
//-----------------------------------------------------------------------------
static GXRgb RGB_FADE_Calc( const RGB_FADE* cp_wk, u16 count, u16 count_max )
{
  u8 r, g, b;

  r = (cp_wk->r_dist * count) / count_max;
  r += cp_wk->r_start;
  g = (cp_wk->g_dist * count) / count_max;
  g += cp_wk->g_start;
  b = (cp_wk->b_dist * count) / count_max;
  b += cp_wk->b_start;

  return GX_RGB( r, g, b );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����x�N�g���t�F�[�h  ������
 *
 *  @param  p_wk      ���[�N
 *  @param  cp_start    �J�n
 *  @param  cp_end      �I��
 */
//-----------------------------------------------------------------------------
static void VEC_FADE_Init( VEC_FADE* p_wk, const VecFx16* cp_start, const VecFx16* cp_end )
{
  fx32 cos;
  VecFx16 normal;

  VEC_Set( &p_wk->start, cp_start->x, cp_start->y, cp_start->z );
  VEC_Fx16CrossProduct( cp_start, cp_end, &normal );
  VEC_Set( &p_wk->normal, normal.x, normal.y, normal.z );
  cos       = VEC_Fx16DotProduct( cp_start, cp_end );
  p_wk->way_dist  = FX_AcosIdx( cos );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����x�N�g���̃t�F�[�h���v�Z
 *
 *  @param  cp_wk     ���[�N
 *  @param  count     �J�E���g�l
 *  @param  count_max   �J�E���g�ő�l
 *  @param  p_ans     �v�Z����
 */
//-----------------------------------------------------------------------------
static void VEC_FADE_Calc( const VEC_FADE* cp_wk, u16 count, u16 count_max, VecFx16* p_ans )
{
  u16 rotate;
  MtxFx43 mtx;
  VecFx32 way;

  rotate = (cp_wk->way_dist * count) / count_max;
  MTX_RotAxis43( &mtx, &cp_wk->normal, FX_SinIdx( rotate ), FX_CosIdx( rotate ) );

  MTX_MultVec43( &cp_wk->start, &mtx, &way );
  VEC_Normalize( &way, &way );

  VEC_Fx16Set( p_ans, way.x, way.y, way.z );
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g�t�F�[�h����  ������
 *
 *  @param  p_wk      ���[�N
 *  @param  cp_start    �J�n���C�g���
 *  @param  cp_end      �I�����C�g���
 */
//-----------------------------------------------------------------------------
static void LIGHT_FADE_Init( LIGHT_FADE* p_wk, const LIGHT_DATA* cp_start, const LIGHT_DATA* cp_end )
{
  LIGHT_FADE_InitEx( p_wk, cp_start, cp_end, LIGHT_FADE_COUNT_MAX );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g�t�F�[�h����  ������  �V���N�w��
 *
 *  @param  p_wk      ���[�N
 *  @param  cp_start    �J�n���C�g���
 *  @param  cp_end      �I�����C�g���
 *  @param  sync      �g�p�V���N��
 */
//-----------------------------------------------------------------------------
static void LIGHT_FADE_InitEx( LIGHT_FADE* p_wk, const LIGHT_DATA* cp_start, const LIGHT_DATA* cp_end, u32 sync )
{
  int i;

  p_wk->count     = 0;
  p_wk->count_max   = sync;


  // �t�F�[�h���Ȃ������ۑ�
  p_wk->endtime = cp_end->endtime;

  for( i=0; i<4; i++ ){

    RGB_FADE_Init( &p_wk->light_color[i], cp_start->light_color[i], cp_end->light_color[i] );
    VEC_FADE_Init( &p_wk->light_vec[i], &cp_start->light_vec[i], &cp_end->light_vec[i] );

    p_wk->light_flag[i] = cp_end->light_flag[i];
  }

  RGB_FADE_Init( &p_wk->diffuse, cp_start->diffuse, cp_end->diffuse );
  RGB_FADE_Init( &p_wk->ambient, cp_start->ambient, cp_end->ambient );
  RGB_FADE_Init( &p_wk->specular, cp_start->specular, cp_end->specular );
  RGB_FADE_Init( &p_wk->emission, cp_start->emission, cp_end->emission );
  RGB_FADE_Init( &p_wk->fog_color, cp_start->fog_color, cp_end->fog_color );
  RGB_FADE_Init( &p_wk->bg_color, cp_start->bg_color, cp_end->bg_color );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J���[�t�F�[�h
 *
 *  @param  p_wk    ���[�N
 *  @param  cp_start  �J�n���C�g�f�[�^
 *  @param  end_color �I���F���
 *  @param  sync    ������V���N��
 */
//-----------------------------------------------------------------------------
static void LIGHT_FADE_InitColor( LIGHT_FADE* p_wk, const LIGHT_DATA* cp_start, GXRgb end_color, u32 sync )
{
  int i;
  p_wk->count     = 0;
  p_wk->count_max   = sync;


  // �t�F�[�h���Ȃ������ۑ�
  p_wk->endtime = cp_start->endtime;

  for( i=0; i<4; i++ ){

    RGB_FADE_Init( &p_wk->light_color[i], cp_start->light_color[i], end_color );
    VEC_FADE_Init( &p_wk->light_vec[i], &cp_start->light_vec[i], &cp_start->light_vec[i] );

    p_wk->light_flag[i] = cp_start->light_flag[i];
  }

  RGB_FADE_Init( &p_wk->diffuse, cp_start->diffuse, end_color );
  RGB_FADE_Init( &p_wk->ambient, cp_start->ambient, end_color );
  RGB_FADE_Init( &p_wk->specular, cp_start->specular, end_color );
  RGB_FADE_Init( &p_wk->emission, cp_start->emission, end_color );
  RGB_FADE_Init( &p_wk->fog_color, cp_start->fog_color, cp_start->fog_color );
  RGB_FADE_Init( &p_wk->bg_color, cp_start->bg_color, cp_start->bg_color );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g�t�F�[�h����  ���C��
 *
 *  @param  p_wk    ���[�N
 */
//-----------------------------------------------------------------------------
static void LIGHT_FADE_Main( LIGHT_FADE* p_wk )
{
  if( p_wk->count < p_wk->count_max ){
    p_wk->count ++;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g�t�F�[�h����  �t�F�[�h���`�F�b�N
 *
 *  @param  cp_wk ���[�N
 *
 *  @retval TRUE  �t�F�[�h��
 *  @retval FALSE �t�F�[�h���ĂȂ�
 */
//-----------------------------------------------------------------------------
static BOOL LIGHT_FADE_IsFade( const LIGHT_FADE* cp_wk )
{
  if( cp_wk->count >= cp_wk->count_max ){
    return FALSE;
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g�t�F�[�h����  ���C�g�f�[�^�擾
 *
 *  @param  cp_wk   ���[�N
 *  @param  p_data    ���C�g�f�[�^�i�[��
 */
//-----------------------------------------------------------------------------
static void LIGHT_FADE_GetData( const LIGHT_FADE* cp_wk, LIGHT_DATA* p_data )
{
  int i;

  p_data->endtime   = cp_wk->endtime;

  for( i=0; i<4; i++ ){
    p_data->light_color[i]  = RGB_FADE_Calc( &cp_wk->light_color[i], cp_wk->count, cp_wk->count_max );
    VEC_FADE_Calc( &cp_wk->light_vec[i], cp_wk->count, cp_wk->count_max, &p_data->light_vec[i] );

    p_data->light_flag[i] = cp_wk->light_flag[i];
  }

  p_data->diffuse   = RGB_FADE_Calc( &cp_wk->diffuse, cp_wk->count, cp_wk->count_max );
  p_data->ambient   = RGB_FADE_Calc( &cp_wk->ambient, cp_wk->count, cp_wk->count_max );
  p_data->specular  = RGB_FADE_Calc( &cp_wk->specular, cp_wk->count, cp_wk->count_max );
  p_data->emission  = RGB_FADE_Calc( &cp_wk->emission, cp_wk->count, cp_wk->count_max );
  p_data->fog_color = RGB_FADE_Calc( &cp_wk->fog_color, cp_wk->count, cp_wk->count_max );
  p_data->bg_color  = RGB_FADE_Calc( &cp_wk->bg_color, cp_wk->count, cp_wk->count_max );

}


//----------------------------------------------------------------------------
/**
 *  @brief  �J���[�t�F�[�h�J�n
 *
 *  @param  p_wk    ���[�N
 *  @param  insync    �C���V���N
 *  @param  outsync   �A�E�g�V���N
 *  @param  color   �F
 */
//-----------------------------------------------------------------------------
static void COLOR_FADE_Init( COLOR_FADE* p_wk, u16 insync, u16 outsync, GXRgb color )
{
  p_wk->seq   = COLOR_FADE_SEQ_INIT;
  p_wk->insync  = insync;
  p_wk->outsync = outsync;
  p_wk->color   = color;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J���[�t�F�[�h���C��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_fade    �t�F�[�h�V�X�e��
 *  @param  cp_refdata  ���f���Ă�̃��C�g�f�[�^
 *  @param  cp_nowdata  ���̃��C�g�f�[�^
 *
 *  @retval TRUE  �t�F�[�h����
 *  @retval FALSE �t�F�[�h��
 */
//-----------------------------------------------------------------------------
static BOOL COLOR_FADE_Main( COLOR_FADE* p_wk, LIGHT_FADE* p_fade, const LIGHT_DATA* cp_refdata, const LIGHT_DATA* cp_nowdata )
{
  switch( p_wk->seq ){
  case COLOR_FADE_SEQ_INIT:
    LIGHT_FADE_InitColor( p_fade, cp_refdata, p_wk->color, p_wk->insync );
    p_wk->seq = COLOR_FADE_SEQ_IN;
    break;

  case COLOR_FADE_SEQ_IN:
    if( !LIGHT_FADE_IsFade( p_fade ) ){
      LIGHT_FADE_InitEx( p_fade, cp_refdata, cp_nowdata, p_wk->outsync );
      p_wk->seq = COLOR_FADE_SEQ_OUT;
    }
    break;

  case COLOR_FADE_SEQ_OUT:
    if( !LIGHT_FADE_IsFade( p_fade ) ){
      p_wk->seq = COLOR_FADE_SEQ_END;
    }
    break;

  case COLOR_FADE_SEQ_END:
    return TRUE;

  default:
    GF_ASSERT(0);
    break;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J���[�t�F�[�h�����҂�
 *
 *  @param  cp_wk ���[�N
 *
 *  @retval TRUE  �t�F�[�h��
 *  @retval FALSE �t�F�[�h����
 */
//-----------------------------------------------------------------------------
static BOOL COLOR_FADE_IsFade( const COLOR_FADE* cp_wk )
{
  if( cp_wk->seq == COLOR_FADE_SEQ_END ){
    return FALSE;
  }
  return TRUE;
}




