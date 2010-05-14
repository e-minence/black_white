//============================================================================
/**
 *  @file   manual_data.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL_DATA
 */
//============================================================================


// �f�o�b�O�@�\
//#define DEBUG_OPEN_FLAG_ALL_ON    // ���ꂪ��`����Ă���Ƃ��A�I�[�v���t���O���S��ON�ɂȂ��Ă���
//#define DEBUG_READ_FLAG_ALL_OFF   // ���ꂪ��`����Ă���Ƃ��A���[�h�t���O���S��OFF�ɂȂ��Ă���
//#define DEBUG_READ_FLAG_ALL_ON    // ���ꂪ��`����Ă���Ƃ��A���[�h�t���O���S��ON�ɂȂ��Ă���


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamedata_def.h"
#include "gamesystem/game_data.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "app/app_taskmenu.h"

#include "../../../../resource/fldmapdata/flagwork/flag_define.h"
#include "savedata/save_control.h"
#include "savedata/c_gear_data.h"
#include "savedata/etc_save.h"
#include "field/eventwork.h"

#include "manual_graphic.h"
#include "manual_def.h"
#include "manual_common.h"
#include "manual_data.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_manual.h"
#include "msg/msg_manual_text.h"
#include "manual.naix"
#include "manual_image.naix"


// ���\�[�X�Ő��������w�b�_
#include "../../../../resource/manual/manual_data_def.h"
#include "../../../../resource/manual/manual_open_flag.h"


// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// u16�̔z��Y����

// �J�e�S���J�n�ꏊ�t�@�C��
#define CATE_REF_U16_ARRAY_IDX_TOTAL_CATE_NUM             (0)
#define CATE_REF_U16_ARRAY_IDX_CATE_BYTE_POS_START        (1)

// �J�e�S���t�@�C������1�J�e�S���f�[�^
#define CATE_DATA_U16_ARRAY_IDX_CATE_NO                   (0)
#define CATE_DATA_U16_ARRAY_IDX_CATE_GMM_ID               (1)
#define CATE_DATA_U16_ARRAY_IDX_TITLE_NUM                 (2)
#define CATE_DATA_U16_ARRAY_IDX_TITLE_IDX_START           (3)

// �^�C�g���J�n�ꏊ�t�@�C��
#define TITLE_REF_U16_ARRAY_IDX_TOTAL_TITLE_NUM           (0)
#define TITLE_REF_U16_ARRAY_IDX_TITLE_BYTE_POS_START      (1)

// �^�C�g���t�@�C������1�^�C�g���f�[�^
#define TITLE_DATA_U16_ARRAY_IDX_CATE_NO                  (0)
#define TITLE_DATA_U16_ARRAY_IDX_TITLE_NO                 (1)
#define TITLE_DATA_U16_ARRAY_IDX_SERIAL_NO                (2)
#define TITLE_DATA_U16_ARRAY_IDX_TITLE_GMM_ID             (3)
#define TITLE_DATA_U16_ARRAY_IDX_OPEN_FLAG                (4)
#define TITLE_DATA_U16_ARRAY_IDX_READ_FLAG                (5)
#define TITLE_DATA_U16_ARRAY_IDX_PAGE_NUM                 (6)
#define TITLE_DATA_U16_ARRAY_IDX_PAGE_START               (7)

#define TITLE_DATA_U16_ARRAY_IDX_PAGE_DATA_TOTAL          (2)  // �y�[�W�̃f�[�^�̍��v��gmm��ID�Ɖ摜��ID��2��
#define TITLE_DATA_U16_ARRAY_IDX_PAGE_DATA_GMM_ID         (0)  // �y�[�W�̃f�[�^�̂���gmm��ID�̈ʒu
#define TITLE_DATA_U16_ARRAY_IDX_PAGE_DATA_IMAGE_ID       (1)  // �y�[�W�̃f�[�^�̂����摜��ID�̈ʒu


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
struct  _MANUAL_DATA_WORK
{
  u16* title_file;       // �^�C�g���t�@�C��
  u16* title_ref_file;   // �^�C�g���J�n�ꏊ�t�@�C��
  u16* cate_file;        // �J�e�S���t�@�C��
  u16* cate_ref_file;    // �J�e�S���J�n�ꏊ�t�@�C��
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// �^�C�g���t�@�C��
static  u16* Manual_Data_TitleGetTitleData( MANUAL_DATA_WORK* work, u16 title_idx );

// �^�C�g���J�n�ꏊ�t�@�C��
static  u16  Manual_Data_TitleRefGetTitleBytePos( MANUAL_DATA_WORK* work, u16 title_idx );

// �J�e�S���t�@�C��
static  u16* Manual_Data_CateGetCateData( MANUAL_DATA_WORK* work, u16 cate_idx );

// �J�e�S���J�n�ꏊ�t�@�C��
static  u16  Manual_Data_CateRefGetCateBytePos( MANUAL_DATA_WORK* work, u16 cate_idx );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//-------------------------------------
/// Load����
//=====================================
MANUAL_DATA_WORK*  MANUAL_DATA_Load( ARCHANDLE* handle, HEAPID heap_id )
{
  MANUAL_DATA_WORK* work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(MANUAL_DATA_WORK) ); 
  u32 size;
  
  work->title_file     = GFL_ARCHDL_UTIL_LoadEx( handle, NARC_manual_manual_data_title_dat,     FALSE, heap_id, &size );
  work->title_ref_file = GFL_ARCHDL_UTIL_LoadEx( handle, NARC_manual_manual_data_title_ref_dat, FALSE, heap_id, &size );
  work->cate_file      = GFL_ARCHDL_UTIL_LoadEx( handle, NARC_manual_manual_data_cate_dat,      FALSE, heap_id, &size );
  work->cate_ref_file  = GFL_ARCHDL_UTIL_LoadEx( handle, NARC_manual_manual_data_cate_ref_dat,  FALSE, heap_id, &size );

  return work;
}

//-------------------------------------
/// Unload����
//=====================================
void               MANUAL_DATA_Unload( MANUAL_DATA_WORK* work )
{
  GFL_HEAP_FreeMemory( work->cate_ref_file );
  GFL_HEAP_FreeMemory( work->cate_file );
  GFL_HEAP_FreeMemory( work->title_ref_file );
  GFL_HEAP_FreeMemory( work->title_file );

  GFL_HEAP_FreeMemory( work );
}

//-------------------------------------
/// �^�C�g���t�@�C��
//=====================================
u16  MANUAL_DATA_TitleGetTitleGmmId( MANUAL_DATA_WORK* work, u16 title_idx )
                                          // title_idx�͑S�^�C�g���ɂ�����o�Ȕԍ�
                                          // title_idx�^�C�g����gmm��ID��߂�l�Ƃ��ĕԂ�
{
  u16* title_data = Manual_Data_TitleGetTitleData( work, title_idx );
  u16  title_gmm_id = title_data[TITLE_DATA_U16_ARRAY_IDX_TITLE_GMM_ID];
  return title_gmm_id;
}
u16  MANUAL_DATA_TitleGetOpenFlag( MANUAL_DATA_WORK* work, u16 title_idx )
{
  u16* title_data = Manual_Data_TitleGetTitleData( work, title_idx );
  u16  open_flag = title_data[TITLE_DATA_U16_ARRAY_IDX_OPEN_FLAG];
  return open_flag;
}
u16  MANUAL_DATA_TitleGetReadFlag( MANUAL_DATA_WORK* work, u16 title_idx )
{
  u16* title_data = Manual_Data_TitleGetTitleData( work, title_idx );
  u16  read_flag = title_data[TITLE_DATA_U16_ARRAY_IDX_READ_FLAG];
  return read_flag;
}
u16  MANUAL_DATA_TitleGetPageNum( MANUAL_DATA_WORK* work, u16 title_idx )
{
  u16* title_data = Manual_Data_TitleGetTitleData( work, title_idx );
  u16  page_num = title_data[TITLE_DATA_U16_ARRAY_IDX_PAGE_NUM];
  return page_num;
}
u16  MANUAL_DATA_TitleGetPageGmmId( MANUAL_DATA_WORK* work, u16 title_idx, u16 page_order )
                                          // title_idx�͑S�^�C�g���ɂ�����o�Ȕԍ�
                                          // title_idx�^�C�g�����ɂ�����page_order�Ԗڂ̃y�[�W�́Agmm��ID��߂�l�Ƃ��ĕԂ�
{
  u16* title_data = Manual_Data_TitleGetTitleData( work, title_idx );
  u16  page_gmm_id = title_data[TITLE_DATA_U16_ARRAY_IDX_PAGE_START + TITLE_DATA_U16_ARRAY_IDX_PAGE_DATA_TOTAL * page_order + TITLE_DATA_U16_ARRAY_IDX_PAGE_DATA_GMM_ID];
  return page_gmm_id;
}
u16  MANUAL_DATA_TitleGetPageImageId( MANUAL_DATA_WORK* work, u16 title_idx, u16 page_order )
                                          // title_idx�͑S�^�C�g���ɂ�����o�Ȕԍ�
                                          // title_idx�^�C�g�����ɂ�����page_order�Ԗڂ̃y�[�W�́A�摜��ID��߂�l�Ƃ��ĕԂ�
{
  u16* title_data = Manual_Data_TitleGetTitleData( work, title_idx );
  u16  page_image_id = title_data[TITLE_DATA_U16_ARRAY_IDX_PAGE_START + TITLE_DATA_U16_ARRAY_IDX_PAGE_DATA_TOTAL * page_order + TITLE_DATA_U16_ARRAY_IDX_PAGE_DATA_IMAGE_ID];
  return page_image_id;
}

//-------------------------------------
/// �^�C�g���J�n�ꏊ�t�@�C��
//=====================================
u16  MANUAL_DATA_TitleRefGetTotalTitleNum( MANUAL_DATA_WORK* work )
{
  u16 total_title_num = work->title_ref_file[TITLE_REF_U16_ARRAY_IDX_TOTAL_TITLE_NUM];
  return total_title_num;
}

//-------------------------------------
/// �J�e�S���t�@�C��
//=====================================
u16  MANUAL_DATA_CateGetCateGmmId( MANUAL_DATA_WORK* work, u16 cate_idx )
                                          // cate_idx�͑S�J�e�S���ɂ�����o�Ȕԍ�
                                          // cate_idx�J�e�S����gmm��ID��߂�l�Ƃ��ĕԂ�
{
  u16* cate_data = Manual_Data_CateGetCateData( work, cate_idx );
  u16  cate_gmm_id = cate_data[CATE_DATA_U16_ARRAY_IDX_CATE_GMM_ID];
  return cate_gmm_id;
}
u16  MANUAL_DATA_CateGetTitleNum( MANUAL_DATA_WORK* work, u16 cate_idx )
                                          // cate_idx�͑S�J�e�S���ɂ�����o�Ȕԍ�
                                          // cate_idx�J�e�S�����ɂ�����^�C�g���̌���߂�l�Ƃ��ĕԂ�
{
  u16* cate_data = Manual_Data_CateGetCateData( work, cate_idx );
  u16  title_num = cate_data[CATE_DATA_U16_ARRAY_IDX_TITLE_NUM];
  return title_num;
}
u16  MANUAL_DATA_CateGetTitleIdx( MANUAL_DATA_WORK* work, u16 cate_idx, u16 title_order )
                                          // cate_idx�͑S�J�e�S���ɂ�����o�Ȕԍ�
                                          // cate_idx�J�e�S�����ɂ�����title_order�Ԗڂ̃^�C�g���́A�S�^�C�g���ɂ�����o�Ȕԍ���߂�l�Ƃ��ĕԂ�
{
  u16* cate_data = Manual_Data_CateGetCateData( work, cate_idx );
  u16  title_idx = cate_data[CATE_DATA_U16_ARRAY_IDX_TITLE_IDX_START + title_order];
  return title_idx;
}

//-------------------------------------
/// �J�e�S���J�n�ꏊ�t�@�C��
//=====================================
u16  MANUAL_DATA_CateRefGetTotalCateNum( MANUAL_DATA_WORK* work )
{
  u16 total_cate_num = work->cate_ref_file[CATE_REF_U16_ARRAY_IDX_TOTAL_CATE_NUM];
  return total_cate_num;
}

//-------------------------------------
/// �I�[�v���t���O
//=====================================
BOOL MANUAL_DATA_OpenFlagIsOpen( MANUAL_DATA_WORK* work, u16 open_flag, GAMEDATA* gamedata )
{
#ifdef DEBUG_OPEN_FLAG_ALL_ON
  return TRUE;
#else

  BOOL is_open = FALSE;
  switch( open_flag )
  {
  case MANUAL_OPEN_FLAG_START:
    {
      is_open = TRUE;
    }
    break;
  case MANUAL_OPEN_FLAG_CGERA:
    {
      if( CGEAR_SV_GetCGearONOFF( CGEAR_SV_GetCGearSaveData(GAMEDATA_GetSaveControlWork(gamedata)) ) )
      {
        is_open = TRUE;
      }
    }
    break;
  case MANUAL_OPEN_FLAG_NCLEAR:
    {
      if( EVENTWORK_CheckEventFlag(GAMEDATA_GetEventWork(gamedata), SYS_FLAG_GAME_CLEAR) )
      {
        is_open = TRUE;
      }
    }
    break;
  }
  return is_open;

#endif
}

//-------------------------------------
/// ���[�h�t���O
//=====================================
BOOL MANUAL_DATA_ReadFlagIsRead( MANUAL_DATA_WORK* work, u16 read_flag, GAMEDATA* gamedata )
{
#if defined DEBUG_READ_FLAG_ALL_OFF
  return FALSE;
#elif defined DEBUG_READ_FLAG_ALL_ON
  return TRUE;
#else

  ETC_SAVE_WORK* etc_sv = SaveData_GetEtc(GAMEDATA_GetSaveControlWork(gamedata));
  BOOL is_read = EtcSave_GetManualFlag( etc_sv, read_flag );
  return is_read;

#endif
}
void MANUAL_DATA_ReadFlagSetRead( MANUAL_DATA_WORK* work, u16 read_flag, GAMEDATA* gamedata )
{
  ETC_SAVE_WORK* etc_sv = SaveData_GetEtc(GAMEDATA_GetSaveControlWork(gamedata));
  EtcSave_SetManualFlag( etc_sv, read_flag );
}

//-------------------------------------
/// �摜��ID
//=====================================
BOOL MANUAL_DATA_ImageIdIsValid( MANUAL_DATA_WORK* work, u16 image_id )  // �摜��ID���L��(�摜����)������(�摜�Ȃ�)����Ԃ�(�L���̂Ƃ�TRUE)
{
  if( image_id != MANUAL_DATA_IMAGE_NONE ) return TRUE;
  return FALSE;
}
u16  MANUAL_DATA_ImageIdGetNoImage( MANUAL_DATA_WORK* work )  // �摜�Ȃ��̂Ƃ��ɕ\������摜��ID�𓾂�
{
  return MANUAL_DATA_NO_IMAGE_ID;
}


//------------------------------------------------------------------
/**
 *  @brief           
 *
 *  @param[in]       
 *  @param[in,out]       
 *
 *  @retval          
 */
//------------------------------------------------------------------

//-------------------------------------
/// 
//=====================================


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================

//-------------------------------------
/// �^�C�g���t�@�C��
//=====================================
static  u16* Manual_Data_TitleGetTitleData( MANUAL_DATA_WORK* work, u16 title_idx )
{
  u16  title_byte_pos = Manual_Data_TitleRefGetTitleBytePos( work, title_idx );
  u8*  title_file_byte = (u8*)(work->title_file);
  u16* title_data = (u16*)( &(title_file_byte[title_byte_pos]) );
  return title_data;
}

//-------------------------------------
/// �^�C�g���J�n�ꏊ�t�@�C��
//=====================================
static  u16  Manual_Data_TitleRefGetTitleBytePos( MANUAL_DATA_WORK* work, u16 title_idx )
{
  u16* title_byte_pos = &(work->title_ref_file[TITLE_REF_U16_ARRAY_IDX_TITLE_BYTE_POS_START]);
  return title_byte_pos[title_idx];
}

//-------------------------------------
/// �J�e�S���t�@�C��
//=====================================
static  u16* Manual_Data_CateGetCateData( MANUAL_DATA_WORK* work, u16 cate_idx )
{
  u16  cate_byte_pos = Manual_Data_CateRefGetCateBytePos( work, cate_idx );
  u8*  cate_file_byte = (u8*)(work->cate_file);
  u16* cate_data = (u16*)( &(cate_file_byte[cate_byte_pos]) );
  return cate_data;
}

//-------------------------------------
/// �J�e�S���J�n�ꏊ�t�@�C��
//=====================================
static  u16  Manual_Data_CateRefGetCateBytePos( MANUAL_DATA_WORK* work, u16 cate_idx )
{
  u16* cate_byte_pos = &(work->cate_ref_file[CATE_REF_U16_ARRAY_IDX_CATE_BYTE_POS_START]);
  return cate_byte_pos[cate_idx];
}


