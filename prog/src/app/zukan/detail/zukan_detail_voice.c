//============================================================================
/**
 *  @file   zukan_detail_voice.c
 *  @brief  �}�ӏڍ׉�ʂ̖������
 *  @author Koji Kawada
 *  @data   2010.02.09
 *  @note   
 *  ���W���[�����FZUKAN_DETAIL_VOICE
 */
//============================================================================
//#define DEBUG_KAWADA


//#define DEBUG_SOUND_TEST  // ���ꂪ��`����Ă���Ƃ��A�T�E���h�̃e�X�g���\


#define GRAPH_GATHER       // ���ꂪ��`����Ă���Ƃ��A���W�����O���t��`�悷��
#ifdef GRAPH_GATHER
  #define GRAPH_SWAP_BMPWIN  // ���ꂪ��`����Ă���Ƃ��A�O���t�`��p�Ƀr�b�g�}�b�v�E�B���h�E��2�p�ӂ���
#endif  // ����r�b�g�}�b�v��GFL_BMP_Print����Ɗ댯��������Ȃ��A�Ǝv���A2�p�ӂ����B(memcpy, memmove�ǂ���Ƃ��ē����̂�������Ȃ�����)
        // �ǂ���GFL_BMP_Print�Ɏ��Ԃ��|�����Ă���悤���BVRAM�ɂ�����VRAM���m�ŃR�s�[����Α������낤���H

#define USE_SGRAPH  // ���ꂪ��`����Ă���Ƃ��ASGRAPH(Scroll GRAPH)���g�p����

//#define TIME_BMPWIN        // ���ꂪ��`����Ă���Ƃ��A���Ԃ𕶎��Ƃ��ĕ\������


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/bmp_winframe.h"
#include "system/poke2dgra.h"
#include "gamesystem/gamedata_def.h"
#include "savedata/zukan_savedata.h"
#include "print/printsys.h"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/global_msg.h"
#include "sound/pm_sndsys.h"             // BGM
#include "../../../field/field_sound.h"  // BGM
#include "sound/pm_wb_voice.h"  // wb�ł�pm_voice�ł͂Ȃ���������g��

#include "zukan_detail_def.h"
#include "zukan_detail_common.h"
#include "zukan_detail_graphic.h"
#include "zukan_detail_voice.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_zkn.h"
#include "zukan_gra.naix"

// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// ���C��BG�t���[��
#define BG_FRAME_M_PANEL           (GFL_BG_FRAME2_M)
#define BG_FRAME_M_TIME            (GFL_BG_FRAME3_M)
#define BG_FRAME_M_REAR            (GFL_BG_FRAME0_M)
// ���C��BG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_M_PANEL       (1)
#define BG_FRAME_PRI_M_TIME        (2)
#define BG_FRAME_PRI_M_REAR        (3)

// ���C��BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_PANEL             = 1,
  BG_PAL_NUM_M_TIME              = 1,
  BG_PAL_NUM_M_REAR              = ZKNDTL_COMMON_REAR_BG_PAL_NUM,
};
// �ʒu
enum
{
  BG_PAL_POS_M_PANEL             = 0,
  BG_PAL_POS_M_TIME              = 1,
  BG_PAL_POS_M_REAR              = 2,
};

// ���C��OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_NUMBER       = 4,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_NUMBER       = 0,
};


// �T�uBG�t���[��
#define BG_FRAME_S_NAME           (GFL_BG_FRAME2_S)
#define BG_FRAME_S_REAR           (GFL_BG_FRAME0_S)
// �T�uBG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_S_NAME       (1)
#define BG_FRAME_PRI_S_REAR       (3)

// �T�uBG�p���b�g
enum
{
  BG_PAL_NUM_S_NAME              = 1,
  BG_PAL_NUM_S_REAR              = ZKNDTL_COMMON_REAR_BG_PAL_NUM,
};
// �ʒu
enum
{
  BG_PAL_POS_S_NAME              = 0,
  BG_PAL_POS_S_REAR              = 1,
};

// OBJ�������ւ���ۂɗ���Ȃ��悤�ɁA2�����݂ɕ\������
typedef enum
{
  OBJ_SWAP_0,
  OBJ_SWAP_1,
  OBJ_SWAP_MAX,
}
OBJ_SWAP;

// �T�uOBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_S_POKE             = 1,  // OBJ_PAL_NUM_S_POKE * OBJ_SWAP_MAX �{�g�p����
  OBJ_PAL_NUM_S_FIELD            = 1,
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_POKE             = 0,
  OBJ_PAL_POS_S_FIELD            = OBJ_PAL_POS_S_POKE + OBJ_PAL_NUM_S_POKE * OBJ_SWAP_MAX,  // 2
  OBJ_PAL_POS_S_MAX              = OBJ_PAL_POS_S_FIELD + OBJ_PAL_NUM_S_FIELD,               // 3  // ���������
};


// ������
#define STRBUF_TIME_LENGTH       (  8)  // ?'??"


// ProcMain�̃V�[�P���X
enum
{
  SEQ_START                = 0,
  SEQ_FADE_CHANGE_BEFORE,
  SEQ_PREPARE,
  SEQ_FADE_IN,
  SEQ_MAIN,
  SEQ_FADE_OUT,
  SEQ_FADE_CHANGE_AFTER,
  SEQ_END,
};

// �I������
typedef enum
{
  END_CMD_NONE,
  END_CMD_INSIDE,
  END_CMD_OUTSIDE,
}
END_CMD;


// BGM
typedef enum
{
  BGM_STATE_PLAY,      // ���ʂɍĐ���
  BGM_STATE_FADE_OUT,  // �t�F�[�h�A�E�g��
  BGM_STATE_PUSH,      // push��
  BGM_STATE_FADE_IN,   // �t�F�[�h�C����
}
BGM_STATE;


// OBJ
enum
{
  OBJ_RES_NUMBER_NCG,
  OBJ_RES_NUMBER_NCL,
  OBJ_RES_NUMBER_NCE,
  OBJ_RES_FIELD_NCG,
  OBJ_RES_FIELD_NCL,
  OBJ_RES_FIELD_NCE,
  OBJ_RES_MAX,
};
enum
{
  OBJ_CELL_TIME_START   = 0,
  OBJ_CELL_NUMBER0      = OBJ_CELL_TIME_START,
  OBJ_CELL_NUMBER1,
  OBJ_CELL_NUMBER2,
  OBJ_CELL_NUMBER3,
  OBJ_CELL_DASH1,
  OBJ_CELL_DASH2,
  OBJ_CELL_TIME_END,

  OBJ_CELL_FIELD        = OBJ_CELL_TIME_END,
  OBJ_CELL_MAX,
};
static const u8 obj_setup_info[OBJ_CELL_MAX][9] =
{
  //  pos_x,  pos_y,  anmseq,  softpri,  bgpri,                ncg,                ncl,                nce,                surface
  {    25*8,   15*8,      28,        0,  BG_FRAME_PRI_M_PANEL, OBJ_RES_NUMBER_NCG, OBJ_RES_NUMBER_NCL, OBJ_RES_NUMBER_NCE, CLSYS_DEFREND_MAIN },
  {    26*8,   15*8,      28,        0,  BG_FRAME_PRI_M_PANEL, OBJ_RES_NUMBER_NCG, OBJ_RES_NUMBER_NCL, OBJ_RES_NUMBER_NCE, CLSYS_DEFREND_MAIN },
  {    28*8,   15*8,      28,        0,  BG_FRAME_PRI_M_PANEL, OBJ_RES_NUMBER_NCG, OBJ_RES_NUMBER_NCL, OBJ_RES_NUMBER_NCE, CLSYS_DEFREND_MAIN },
  {    29*8,   15*8,      28,        0,  BG_FRAME_PRI_M_PANEL, OBJ_RES_NUMBER_NCG, OBJ_RES_NUMBER_NCL, OBJ_RES_NUMBER_NCE, CLSYS_DEFREND_MAIN },
  {    27*8,   15*8,      29,        0,  BG_FRAME_PRI_M_PANEL, OBJ_RES_NUMBER_NCG, OBJ_RES_NUMBER_NCL, OBJ_RES_NUMBER_NCE, CLSYS_DEFREND_MAIN },
  {    30*8,   15*8,      30,        0,  BG_FRAME_PRI_M_PANEL, OBJ_RES_NUMBER_NCG, OBJ_RES_NUMBER_NCL, OBJ_RES_NUMBER_NCE, CLSYS_DEFREND_MAIN },
  {     128,     96,       0,        0,  BG_FRAME_PRI_S_REAR,  OBJ_RES_FIELD_NCG,  OBJ_RES_FIELD_NCL,  OBJ_RES_FIELD_NCE,  CLSYS_DEFREND_SUB  },
};

// �|�P�����̖��A�j��(�c�X�P�[���g��)
#define POKE_CLWK_SIZE         (96)    // �|�P�O���̏c���s�N�Z���T�C�Y
#define POKE_CLWK_BASE_POS_X   (128)
#define POKE_CLWK_BASE_POS_Y   (128)
#define POKE_CLWK_BASE_SCALE_X (1.0f)
#define POKE_CLWK_BASE_SCALE_Y (1.0f)
#define POKE_CLWK_SCALE_Y_MAX  (1.1f)   // �|�P���������Ƃ��̃X�P�[���̍ő�l
#define POKE_CLWK_SCALE_Y_INC  (0.01f)  // �|�P���������Ƃ��̃X�P�[����1�t���[���̑�����
#define POKE_CLWK_SCALE_Y_DEC  (0.01f)  // �|�P���������Ƃ��̃X�P�[����1�t���[���̌�����
typedef enum
{
  POKE_CLWK_STATE_SILENT,
  POKE_CLWK_STATE_CRY_OPEN,
  POKE_CLWK_STATE_CRY_STOP,
  POKE_CLWK_STATE_CRY_CLOSE,
}
POKE_CLWK_STATE;


#ifndef USE_SGRAPH

// �O���t
#define GRAPH_BITMAP_CHAR_SIZE_X    (30)
#define GRAPH_BITMAP_CHAR_SIZE_Y    (15)

#define GRAPH_DATA_INTERVAL_X       (3)  // ���ꂾ��������X���W�����  // 4�̏ꍇ�Ax 0 0 0 x 0 0 0 x �ƂȂ�
#define GRAPH_DATA_MOVE_WAIT        (2)////////(0)  // ���̃t���[�������҂���GRAPH_DATA_MOVE_SPEED�����ړ�����  // 0�̏ꍇ�Ax x x  // 2�̏ꍇ�Ax 0 0 x 0 0 x
#define GRAPH_DATA_MOVE_SPEED       (6)////////(2)//(1)  // �ړ�����ۂ̈ړ���  // [x] = [x+GRAPH_DATA_MOVE_SPEED] �ƂȂ�

#define GRAPH_DATA_SIZE_X           (GRAPH_DATA_MOVE_SPEED+GRAPH_DATA_INTERVAL_X+ 30*8 +GRAPH_DATA_MOVE_SPEED+GRAPH_DATA_INTERVAL_X)
                                            // GRAPH_DATA_SIZE_X�͕`�悷��f�[�^��(GRAPH_DATA_MIN_X, GRAPH_DATA_MAX_X���狁�߂�)���
                                            // �O�ɂ����ɂ��������Ă�������(�ŏ��ƍŌ�̃f�[�^�����ꂢ�ɏ�������)
#define GRAPH_DATA_SIZE_Y           (15*8)

                                          // �r�b�g�}�b�v�̍����(0,0)�Ƃ����Ƃ��̃r�b�g�}�b�v���W
#define GRAPH_BITMAP_ORIGIN_X  (   0)     // (0,       y          )
#define GRAPH_BITMAP_ORIGIN_Y  ( 7*8)     // (x,       7*8 +0     )

#define GRAPH_DATA_MAX_X       (GRAPH_DATA_MOVE_SPEED+GRAPH_DATA_INTERVAL_X+ 30*8 -1)
                                          // (30*8 -1, y          )  // �����܂�(�܂�)�����Ă���  // GRAPH_BITMAP_ORIGIN_X��0�Ƃ����Ƃ��̍ő�l
#define GRAPH_DATA_MIN_X       (GRAPH_DATA_MOVE_SPEED+GRAPH_DATA_INTERVAL_X+       0)
                                          // (0,       y          )  // �����܂�(�܂�)�����Ă���  // GRAPH_BITMAP_ORIGIN_X��0�Ƃ����Ƃ��̍ŏ��l

#define GRAPH_DATA_MAX_Y       ( 7*8)     // (x,       7*8 -7*8   )  // �����܂�(�܂�)�����Ă���  // GRAPH_BITMAP_ORIGIN_Y��0�Ƃ����Ƃ��̍ő�l
#define GRAPH_DATA_MIN_Y       (-7*8)     // (x,       7*8 -(-7*8))  // �����܂�(�܂�)�����Ă���  // GRAPH_BITMAP_ORIGIN_Y��0�Ƃ����Ƃ��̍ŏ��l

#define GRAPH_DATA_SATURATION_MAX_Y  ( 500)  // ������傫�Ȓl�́A���̒l�ɂ��Ă���
#define GRAPH_DATA_SATURATION_MIN_Y  (-500)  // ������傫�Ȓl�́A���̒l�ɂ��Ă���

// �f�[�^���W����r�b�g�}�b�v�̍����(0,0)�Ƃ����Ƃ��̃r�b�g�}�b�v���W�����߂�
#define GRAPH_DATA_X_TO_BITMAP_X(x)  (GRAPH_BITMAP_ORIGIN_X+(x)-GRAPH_DATA_MIN_X)  // ( x<GRAPH_DATA_MIN_X || GRAPH_DATA_MAX_X<x ) �̂Ƃ��͎g�p���Ȃ�����
#define GRAPH_DATA_Y_TO_BITMAP_Y(y)  (GRAPH_BITMAP_ORIGIN_Y-(y))                   // ( y<GRAPH_DATA_MIN_Y || GRAPH_DATA_MAX_Y<y ) �̂Ƃ��͎g�p���Ȃ�����

#define GRAPH_COLOR (15)  // �O���t�̕`��F

// wave�f�[�^�̍ő�l
#define WAVE_DATA_VALUE_MAX                (128)                     // wave_data_begin[i] * WAVE_DATA_VALUE_MAX_TO_GRAPH_DATA / WAVE_DATA_VALUE_MAX
#define WAVE_DATA_VALUE_MAX_TO_GRAPH_DATA  ( GRAPH_DATA_MAX_Y -8 )   // �Ƃ��ăO���t�ɕ\������

// X���W�����Ƃ���wave�f�[�^���ǂꂾ���i�߂邩
#define WAVE_DATA_INTERVAL(data_num_per_frame)  ((data_num_per_frame)*GRAPH_DATA_INTERVAL_X*(GRAPH_DATA_MOVE_WAIT+1)/GRAPH_DATA_MOVE_SPEED)
                                                     // data_num_per_frame�͍Đ�������1�t���[���Ői�ރf�[�^��
#define WAVE_DATA_SIZE_MAX    (26000)    // pokemon_wb/prog/src/sound/pm_voice.c  PMVOICE_WAVESIZE_MAX �Q�l


#ifdef GRAPH_GATHER
  #define GRAPH_DATA_FULL_SIZE_X_MAX  (30*8*4)//(GRAPH_DATA_MAX_X-GRAPH_DATA_MIN_X+1)  // �O�����ėp�ӂ��Ă������S�ȃO���t�̍ő剡�s�N�Z����(��ʏ�ɏ����鉡�͈͂��傫���Ă�����)
                                                                             // �c��GRAPH_BITMAP_CHAR_SIZE_Y�҂�����ɂ��Ă���

  #define WAVE_DATA_SPEED_ONE  (32768)  // NNS_SndWaveOutStart  �Đ��X�s�[�hspeed�́A�Đ����鑬�����w�肵�܂��Bspeed��32768�{�̑����ōĐ�����܂��B
  #define WAVE_DATA_PLAY_FPS   (60)     // 60�t���[���ōĐ�
#endif


#ifdef GRAPH_SWAP_BMPWIN
typedef enum
{
  GRAPH_SWAP_BMPWIN_IDX_0,
  GRAPH_SWAP_BMPWIN_IDX_1,
  GRAPH_SWAP_BMPWIN_IDX_MAX,
}
GRAPH_SWAP_BMPWIN_IDX;
#endif

#endif  // #ifndef USE_SGRAPH


#ifdef USE_SGRAPH

// SGRAPH, SWAVE
#define SGRAPH_BITMAP_CHAR_SIZE_X            (31)  // �r�b�g�}�b�v�̃L�����T�C�Y
#define SGRAPH_BITMAP_CHAR_SIZE_Y            (15)
#define SGRAPH_BITMAP_DISP_CHAR_SIZE_X       (30)  // �����Ă���r�b�g�}�b�v�̃L�����T�C�Y
#define SGRAPH_BITMAP_DISP_CHAR_SIZE_Y       (15)  // SGRAPH_BITMAP_DISP_CHAR_SIZE_? <= SGRAPH_BITMAP_CHAR_SIZE_?
#define SGRAPH_BITMAP_DISP_LAST_PIXEL_NUM_X  (6)   // �����Ă���r�b�g�}�b�v�̍Ō�̃L�����́A�����Ă���s�N�Z����
#define SGRAPH_BITMAP_DISP_LAST_PIXEL_NUM_Y  (8)   // [SGRAPH_BITMAP_DISP_CHAR_SIZE_? -1]�̉��s�N�Z���������Ă��邩
#define SGRAPH_BITMAP_PIXEL_SIZE_X           ( SGRAPH_BITMAP_CHAR_SIZE_X *8 )                                                    // �r�b�g�}�b�v�̃s�N�Z����
#define SGRAPH_BITMAP_DISP_PIXEL_SIZE_X      ( ( SGRAPH_BITMAP_DISP_CHAR_SIZE_X -1 ) *8 + SGRAPH_BITMAP_DISP_LAST_PIXEL_NUM_X )  // �����Ă���r�b�g�}�b�v�̃s�N�Z����
#define SGRAPH_BITMAP_SCREEN_SCROLL_BASE_X   (-1)  // �X�N���[�����ǂꂾ���X�N���[���������Ƃ������Ƃ��邩
#define SGRAPH_BITMAP_SCREEN_SCROLL_BASE_Y   ( 0)  // ����̓r�b�g�}�b�v��[0][0]�ƍ��킹�����̂ŁA�X�N���[�������Ă���
#define SGRAPH_BITMAP_SCREEN_POS_X           (1)   // �X�N���[���̈ʒu�L�������W
#define SGRAPH_BITMAP_SCREEN_POS_Y           (2)

#define SGRAPH_DATA_MOVE_WAIT    (0)//(0)  // ���̃t���[�������҂���SGRAPH_DATA_MOVE_SPEED�����ړ�����  // 0�̏ꍇ�Ax x x  // 2�̏ꍇ�Ax 0 0 x 0 0 x
#define SGRAPH_DATA_MOVE_SPEED   (2)//(2)(1)  // �ړ�����ۂ̈ړ���  // [x] = [x+SGRAPH_DATA_MOVE_SPEED] �ƂȂ�  // 1,2,4,8�̂ǂꂩ��

#define SGRAPH_DATA_MOVE_SPEED_SCROLL_LIMIT  (8)  // �X�N���[���Ƃ̌��ˍ������痈��ړ��ʂ̌��E�l

                                          // �r�b�g�}�b�v�̍����(0,0)�Ƃ����Ƃ��̃r�b�g�}�b�v���W
#define SGRAPH_BITMAP_ORIGIN_X     ( 0)   // ( 0,  y )
#define SGRAPH_BITMAP_ORIGIN_Y     (55)   // ( x, 55 )

#define SGRAPH_DATA_MAX_Y          ( 55)  // ( x, 55 -( 55) )  // �����܂�(�܂�)�����Ă���  // SGRAPH_BITMAP_ORIGIN_Y��0�Ƃ����Ƃ��̍ő�l
#define SGRAPH_DATA_MIN_Y          (-55)  // ( x, 55 -(-55) )  // �����܂�(�܂�)�����Ă���  // SGRAPH_BITMAP_ORIGIN_Y��0�Ƃ����Ƃ��̍ŏ��l

#define SGRAPH_DATA_SATURATION_MAX_Y  ( 500)  // ������傫�Ȓl�́A���̒l�ɂ��Ă���
#define SGRAPH_DATA_SATURATION_MIN_Y  (-500)  // ������傫�Ȓl�́A���̒l�ɂ��Ă���

// �f�[�^���W����r�b�g�}�b�v�̍����(0,0)�Ƃ����Ƃ��̃r�b�g�}�b�v���W�����߂�
#define SGRAPH_DATA_Y_TO_BITMAP_Y(y)  (SGRAPH_BITMAP_ORIGIN_Y-(y))  // ( y<SGRAPH_DATA_MIN_Y || SGRAPH_DATA_MAX_Y<y ) �̂Ƃ��͎g�p���Ȃ�����

#define SGRAPH_COLOR (15)

// wave�f�[�^�̍ő�l
#define SWAVE_DATA_VALUE_MAX                (128)                    // wave_data_begin[i] * SWAVE_DATA_VALUE_MAX_TO_SGRAPH_DATA / SWAVE_DATA_VALUE_MAX
#define SWAVE_DATA_VALUE_MAX_TO_SGRAPH_DATA ( SGRAPH_DATA_MAX_Y -8 ) // �Ƃ��ăO���t�ɕ\������

#define SWAVE_DATA_SIZE_MAX    (26000)    // pokemon_wb/prog/src/sound/pm_voice.c  PMVOICE_WAVESIZE_MAX �Q�l

#define SGRAPH_DATA_FULL_SIZE_X_MAX  (30*8*4)  // �O�����ėp�ӂ��Ă������S�ȃO���t�̍ő剡�s�N�Z����(��ʏ�ɏ����鉡�͈͂��傫���Ă�����)
                                               // �c�� SGRAPH_BITMAP_CHAR_SIZE_Y *8 �҂�����ɂ��Ă���

#define SWAVE_DATA_SPEED_ONE  (32768)  // NNS_SndWaveOutStart  �Đ��X�s�[�hspeed�́A�Đ����鑬�����w�肵�܂��Bspeed��32768�{�̑����ōĐ�����܂��B
#define SWAVE_DATA_PLAY_FPS   (60)     // 60�t���[���ōĐ�

#endif  // #ifdef USE_SGRAPH


#ifdef DEBUG_SOUND_TEST  // ���ꂪ��`����Ă���Ƃ��A�T�E���h�̃e�X�g���\
static int speed;
static int volume;
#endif


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// PROC ���[�N
//=====================================
typedef struct
{
  // ���̂Ƃ��납��ؗp�������
  GFL_CLUNIT*                 clunit;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  // �����ō쐬�������
  GFL_BMPWIN*                 name_bmpwin;
  BOOL                        name_trans;  // �]������K�v������ꍇTRUE

#ifdef TIME_BMPWIN
  GFL_BMPWIN*                 time_bmpwin;
  GFL_MSGDATA*                time_msgdata;
#endif


#ifdef USE_SGRAPH
  u16*                        graph_screen;                     // �X�N���[��

  s16                         graph_x_scroll;                   // �X�N���[��  // 0<= <8�ŁA8�܂ł�������0�ɖ߂�
  s16                         graph_x_chara_offset;             // �X�N���[����x����0�L�����ڂɂ͉��L�����ڂ̃f�[�^��������
  BOOL                        graph_req;                        // TRUE�̂Ƃ�VBlank�œ]������

  GFL_BMPWIN*                 graph_bmpwin;
  
  u8                          graph_data_move_wait_count;       // �O���t���ړ�����܂łɑ҂t���[����(0�̂Ƃ����t���[���i��)
  
  u16                         full_monsno;                      // �O�����ėp�ӂ��Ă������S�ȃO���t�̎������monsno(0�̂Ƃ��Ȃ�)
  u16                         full_formno;                      // �O�����ėp�ӂ��Ă������S�ȃO���t�̎������formno
  GFL_BMP_DATA*               graph_full_bmp_data;              // �O�����ėp�ӂ��Ă������S�ȃO���t
  s16                         graph_data_full_size;             // �O�����ėp�ӂ��Ă������S�ȃf�[�^�͉����s�N�Z�����邩
  s16                         graph_data_full_print;            // ���ɏ������s�N�Z���̈ʒu(0=���ꂩ�珑��; ��3=3�s�N�Z���ڂ��珑��; graph_data_full_size=�����I����Ă���;)
  
  const s8*                   wave_data_begin;                  // wave�f�[�^
  u32                         wave_full_millisec;               // �O�����ėp�ӂ��Ă���wave�f�[�^�̑�����

#else  // #ifdef USE_SGRAPH

#ifdef GRAPH_SWAP_BMPWIN
  GRAPH_SWAP_BMPWIN_IDX       graph_swap_bmpwin_idx;  // ���ꂩ��`�悷��r�b�g�}�b�v�E�B���h�E
  GFL_BMPWIN*                 graph_bmpwin[GRAPH_SWAP_BMPWIN_IDX_MAX];
#else
  GFL_BMPWIN*                 graph_bmpwin;
#endif
  s16                         graph_data_y[GRAPH_DATA_SIZE_X];  // �f�[�^���Wy = graph_data_y[�f�[�^���Wx] 
  s16                         graph_data_prev_x;                // �O�Ƀf�[�^���������Ƃ���
  u8                          graph_data_move_wait_count;       // �O���t���ړ�����܂łɑ҂t���[����(0�̂Ƃ����t���[���i��)
#ifdef GRAPH_GATHER
  u16                         full_monsno;                      // �O�����ėp�ӂ��Ă������S�ȃO���t�̎������monsno(0�̂Ƃ��Ȃ�)
  u16                         full_formno;                      // �O�����ėp�ӂ��Ă������S�ȃO���t�̎������formno
  GFL_BMP_DATA*               graph_full_bmp_data;              // �O�����ėp�ӂ��Ă������S�ȃO���t
  s16                         graph_data_full_size;             // �O�����ėp�ӂ��Ă������S�ȃf�[�^�͉����s�N�Z�����邩
  s16                         graph_data_full_print;            // ���ɏ������s�N�Z���̈ʒu(0=���ꂩ�珑��; ��3=3�s�N�Z���ڂ��珑��; graph_data_full_size=�����I����Ă���;)
#endif

  const s8*                   wave_data_begin;                  // wave�f�[�^
  u32                         wave_data_curr_pos;               // wave�f�[�^�̂��ꂩ�猩��ʒu
  u32                         wave_data_interval;               // wave�f�[�^�̌���ʒu�̊Ԋu
#ifdef GRAPH_GATHER
  u32                         wave_full_millisec;               // �O�����ėp�ӂ��Ă���wave�f�[�^�̑�����
#endif

#endif  // #ifdef USE_SGRAPH


  u32                         poke_ncl[OBJ_SWAP_MAX];   // poke_clwk[i]��NULL�łȂ��Ƃ�poke_ncl[i]�͗L��
  u32                         poke_ncg[OBJ_SWAP_MAX];   // poke_clwk[i]��NULL�łȂ��Ƃ�poke_ncg[i]�͗L��
  u32                         poke_nce[OBJ_SWAP_MAX];   // poke_clwk[i]��NULL�łȂ��Ƃ�poke_nce[i]�͗L��
  GFL_CLWK*                   poke_clwk[OBJ_SWAP_MAX];  // �g���Ă��Ȃ��Ƃ���poke_clwk[i]��NULL�ɂ��Ă���
  OBJ_SWAP                    poke_swap_curr;           // ���\�����Ă���poke_clwk��poke_clwk[poke_swap_curr] 
  POKE_CLWK_STATE             poke_state;

  u32                         voice_idx;
  BOOL                        voice_play;  // ���Ԃ��X�V����TRUE�ɂ��Ă���(�Ȃ̂ŁA�{�C�X�̍Đ����I����Ă�TRUE�̂܂܂̂��Ƃ�����)
  OSTick                      voice_start;
  u16                         voice_disp_millisec;
  u16                         voice_prev_disp_millisec;

  GFL_ARCUTIL_TRANSINFO       panel_m_tinfo;

  ZKNDTL_COMMON_REAR_WORK*    rear_wk_m;
  ZKNDTL_COMMON_REAR_WORK*    rear_wk_s;

  // OBJ
  u32                         obj_res[OBJ_RES_MAX];
  GFL_CLWK*                   obj_cell[OBJ_CELL_MAX];

  // BGM
  BGM_STATE                   bgm_state;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // �t�F�[�h
  ZKNDTL_COMMON_FADE_WORK*    fade_wk_m;
  ZKNDTL_COMMON_FADE_WORK*    fade_wk_s;
  // �p���b�g�t�F�[�h
  ZKNDTL_COMMON_PF_WORK*      pf_wk;

  // �I������
  END_CMD                     end_cmd;

  // ���͉s��
  BOOL                        input_enable;  // ���͉̂Ƃ�TRUE
}
ZUKAN_DETAIL_VOICE_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Zukan_Detail_Voice_VBlankFunc( GFL_TCB* tcb, void* wk );

// BGM
static void Zukan_Detail_Voice_BgmInit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_BgmExit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_BgmMain( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// OBJ
static void Zukan_Detail_Voice_ObjInit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_ObjExit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_ObjTimeDisplay( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                               u16 disp_sec, u16 disp_milli );

// �|�P����2D
static void Zukan_Detail_Voice_CreatePoke( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn, OBJ_SWAP swap_idx );
static void Zukan_Detail_Voice_DeletePoke( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn, OBJ_SWAP swap_idx );
// �|�P�����̖��A�j��(�c�X�P�[���g��)
static void Zukan_Detail_Voice_PokeCryMain( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_PokeCryStart( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_PokeCryEnd( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �|�P������
static void Zukan_Detail_Voice_CreateNameBase( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_DeleteNameBase( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_CreateName( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_DeleteName( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_MainName( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �{�C�X�Đ�����
static void Zukan_Detail_Voice_CreateTimeBase( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_DeleteTimeBase( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_CreateTime( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                           u16 disp_sec, u16 disp_milli );
static void Zukan_Detail_Voice_DeleteTime( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_UpdateTime( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_ResetTime( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �|�P�����ύX
static void Zukan_Detail_Voice_ChangePoke( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �Đ��{�^���̃L�[or�^�b�`
static void Zukan_Detail_Voice_KeyTouchPlayButton( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �O���t
static void Zukan_Detail_Voice_GraphInit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_GraphExit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_GraphMain( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_GraphReset( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_GraphDrawBitmap( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_WaveDataSetup( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn, u16 monsno, u16 formno );

#ifdef USE_SGRAPH
static void Zukan_Detail_Voice_GraphMakeTransScreen_VBlank( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
#endif  // #ifdef USE_SGRAPH


// �A���t�@�ݒ�
static void Zukan_Detail_Voice_AlphaInit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_AlphaExit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Voice_ProcInit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static ZKNDTL_PROC_RESULT Zukan_Detail_Voice_ProcExit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static ZKNDTL_PROC_RESULT Zukan_Detail_Voice_ProcMain( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Voice_CommandFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn, ZKNDTL_COMMAND cmd );

const ZKNDTL_PROC_DATA    ZUKAN_DETAIL_VOICE_ProcData =
{
  Zukan_Detail_Voice_ProcInit,
  Zukan_Detail_Voice_ProcMain,
  Zukan_Detail_Voice_ProcExit,
  Zukan_Detail_Voice_CommandFunc,
  NULL,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void             ZUKAN_DETAIL_VOICE_InitParam(
                            ZUKAN_DETAIL_VOICE_PARAM*  param,
                            HEAPID                     heap_id )
{
  param->heap_id = heap_id;
}


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Voice_ProcInit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_VOICE_PARAM*    param    = (ZUKAN_DETAIL_VOICE_PARAM*)pwk;
  ZUKAN_DETAIL_VOICE_WORK*     work;

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_Voice_ProcInit\n" );
  }
#endif

  // �q�[�v
  {
    work = ZKNDTL_PROC_AllocWork(proc, sizeof(ZUKAN_DETAIL_VOICE_WORK), param->heap_id);
    GFL_STD_MemClear( work, sizeof(ZUKAN_DETAIL_VOICE_WORK) ); 
  }

  // ���̂Ƃ��납��ؗp�������
  {
    ZUKAN_DETAIL_GRAPHIC_WORK* graphic = ZKNDTL_COMMON_GetGraphic(cmn);
    work->clunit      = ZUKAN_DETAIL_GRAPHIC_GetClunit(graphic);
    work->font        = ZKNDTL_COMMON_GetFont(cmn);
    work->print_que   = ZKNDTL_COMMON_GetPrintQue(cmn);
  }


#ifdef USE_SGRAPH
  // �O���t
  work->graph_req = FALSE;
#endif  // #ifdef USE_SGRAPH

  
  // �{�C�X
  work->voice_play = FALSE;
  work->voice_disp_millisec = 0;
  work->voice_prev_disp_millisec = 1;  // �ŏ��̕`�悪�����悤�ɁAvoice_disp_millisec�ƈقȂ�l�ŏ��������Ă���

  // BGM
  work->bgm_state = BGM_STATE_PLAY;
  Zukan_Detail_Voice_BgmInit( param, work, cmn );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Detail_Voice_VBlankFunc, work, 1 );

  // �t�F�[�h
  {
    work->fade_wk_m = ZKNDTL_COMMON_FadeInit( param->heap_id );
    work->fade_wk_s = ZKNDTL_COMMON_FadeInit( param->heap_id );
   
    ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_m );
    ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_s );

    ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_M, work->fade_wk_m );
    ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_S, work->fade_wk_s );
  }
  // �p���b�g�t�F�[�h
  {
    work->pf_wk = ZKNDTL_COMMON_PfInit( param->heap_id );
  }

  // �I�����
  work->end_cmd = END_CMD_NONE;

  // ���͉s��
  work->input_enable = TRUE;

  // �|�P����2D
  {
    // NULL�ŏ�����
    u8 i;
    for( i=0; i<OBJ_SWAP_MAX; i++ )
    {
      work->poke_clwk[i] = NULL;
    }
    work->poke_swap_curr = OBJ_SWAP_0;
  }

  return ZKNDTL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Voice_ProcExit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_VOICE_PARAM*    param    = (ZUKAN_DETAIL_VOICE_PARAM*)pwk;
  ZUKAN_DETAIL_VOICE_WORK*     work     = (ZUKAN_DETAIL_VOICE_WORK*)mywk;

  // �O���t
  Zukan_Detail_Voice_GraphExit( param, work, cmn );
  // OBJ
  Zukan_Detail_Voice_ObjExit( param, work, cmn );
  // �Ŕw��
  ZKNDTL_COMMON_RearExit( work->rear_wk_s );
  ZKNDTL_COMMON_RearExit( work->rear_wk_m );
  // BG�p�l�� 
  ZKNDTL_COMMON_PanelDelete(
           BG_FRAME_M_PANEL,
           work->panel_m_tinfo );
  // �|�P����2D
  Zukan_Detail_Voice_DeletePoke( param, work, cmn, work->poke_swap_curr );
  // �|�P������
  Zukan_Detail_Voice_DeleteName( param, work, cmn );
  Zukan_Detail_Voice_DeleteNameBase( param, work, cmn );
  // �{�C�X�Đ�����
  Zukan_Detail_Voice_DeleteTime( param, work, cmn );
  Zukan_Detail_Voice_DeleteTimeBase( param, work, cmn );

  // �p���b�g�t�F�[�h
  {
    ZKNDTL_COMMON_PfExit( work->pf_wk );
  }
  // �t�F�[�h
  {
    ZKNDTL_COMMON_FadeExit( work->fade_wk_s );
    ZKNDTL_COMMON_FadeExit( work->fade_wk_m );
  }

  // BGM
  if( work->bgm_state != BGM_STATE_FADE_IN )  // Main�Ńt�F�[�h�C�����Ă���̂ő��v�̂͂������A�O�̂��߂����ł��Ă�ł���
  {
    Zukan_Detail_Voice_BgmExit( param, work, cmn );
  }

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �q�[�v
  ZKNDTL_PROC_FreeWork( proc );
 
#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_Voice_ProcExit\n" );
  }
#endif

  return ZKNDTL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Voice_ProcMain( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_VOICE_PARAM*    param    = (ZUKAN_DETAIL_VOICE_PARAM*)pwk;
  ZUKAN_DETAIL_VOICE_WORK*     work     = (ZUKAN_DETAIL_VOICE_WORK*)mywk;

  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  ZUKAN_DETAIL_HEADBAR_WORK*   headbar  = ZKNDTL_COMMON_GetHeadbar(cmn);
 
  switch(*seq)
  {
  case SEQ_START:
    {
      *seq = SEQ_FADE_CHANGE_BEFORE;

      // BG
      {
        // �N���A
        u8 i;
        for( i=GFL_BG_FRAME0_M; i<=GFL_BG_FRAME3_S; i++ )
        {
          if(    i != ZKNDTL_BG_FRAME_M_TOUCHBAR
              && i != ZKNDTL_BG_FRAME_S_HEADBAR )
          {
            GFL_BG_SetScroll( i, GFL_BG_SCROLL_X_SET, 0 );
            GFL_BG_SetScroll( i, GFL_BG_SCROLL_Y_SET, 0 );
			      GFL_BG_ClearFrame(i);
          }
        }

        // ���C��BG
        GFL_BG_SetPriority( BG_FRAME_M_PANEL, BG_FRAME_PRI_M_PANEL );
        GFL_BG_SetPriority( BG_FRAME_M_TIME,  BG_FRAME_PRI_M_TIME );
        GFL_BG_SetPriority( BG_FRAME_M_REAR,  BG_FRAME_PRI_M_REAR );
        
        // �T�uBG
        GFL_BG_SetPriority( BG_FRAME_S_NAME, BG_FRAME_PRI_S_NAME );
        GFL_BG_SetPriority( BG_FRAME_S_REAR, BG_FRAME_PRI_S_REAR );
      }

      // �|�P����2D
      Zukan_Detail_Voice_CreatePoke( param, work, cmn, work->poke_swap_curr );
      GFL_CLACT_WK_SetObjMode( work->poke_clwk[work->poke_swap_curr], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
      // �|�P������
      Zukan_Detail_Voice_CreateNameBase( param, work, cmn );
      Zukan_Detail_Voice_CreateName( param, work, cmn );
      // �{�C�X�Đ�����
      Zukan_Detail_Voice_CreateTimeBase( param, work, cmn );
      Zukan_Detail_Voice_CreateTime( param, work, cmn, 0, 0 );
      // BG�p�l�� 
      work->panel_m_tinfo = ZKNDTL_COMMON_PanelCreate(
                              ZKNDTL_COMMON_PANEL_CREATE_TYPE_TRANS_PAL_CHARA,
                              param->heap_id,
                              BG_FRAME_M_PANEL,
                              BG_PAL_NUM_M_PANEL,
                              BG_PAL_POS_M_PANEL,
                              5,
                              ARCID_ZUKAN_GRA,
                              NARC_zukan_gra_info_info_bgd_NCLR,
                              NARC_zukan_gra_info_info_bgd_NCGR,
                              NARC_zukan_gra_info_voicewin_bgd_NSCR,
                              0 );
      // �Ŕw��
      work->rear_wk_m = ZKNDTL_COMMON_RearInit( param->heap_id, ZKNDTL_COMMON_REAR_TYPE_VOICE,
          BG_FRAME_M_REAR, BG_PAL_POS_M_REAR +0, BG_PAL_POS_M_REAR +1 );
      work->rear_wk_s = ZKNDTL_COMMON_RearInit( param->heap_id, ZKNDTL_COMMON_REAR_TYPE_VOICE,
          BG_FRAME_S_REAR, BG_PAL_POS_S_REAR +0, BG_PAL_POS_S_REAR +1 );
      // OBJ
      Zukan_Detail_Voice_ObjInit( param, work, cmn );
      Zukan_Detail_Voice_ObjTimeDisplay( param, work, cmn, 0, 0 );
      // �O���t
      Zukan_Detail_Voice_GraphInit( param, work, cmn );

      // �p���b�g�t�F�[�h
      {
        ZKNDTL_COMMON_PfSetPaletteDataFromVram( work->pf_wk );
        ZKNDTL_COMMON_PfSetBlackImmediately( work->pf_wk );
      }
    }
    break;
  case SEQ_FADE_CHANGE_BEFORE:
    {
      *seq = SEQ_PREPARE;
      
      // �t�F�[�h
      ZKNDTL_COMMON_FadeSetColorlessImmediately( ZKNDTL_COMMON_FADE_DISP_M, work->fade_wk_m );
      ZKNDTL_COMMON_FadeSetColorlessImmediately( ZKNDTL_COMMON_FADE_DISP_S, work->fade_wk_s );
     
      // ZKNDTL_COMMON_FadeSetColorlessImmediately��G2_SetBlendBrightnessExt��G2S_SetBlendBrightnessExt��ݒ肵�Ă���̂ŁA
      // ���̌��Zukan_Detail_Voice_AlphaInit���ĂԂ���
      Zukan_Detail_Voice_AlphaInit( param, work, cmn );
    }
    break;
  case SEQ_PREPARE:
    {
      *seq = SEQ_FADE_IN;

      //// �t�F�[�h
      //ZKNDTL_COMMON_FadeSetBlackToColorless( work->fade_wk_m );
      //ZKNDTL_COMMON_FadeSetBlackToColorless( work->fade_wk_s );
      
      // �p���b�g�t�F�[�h
      {
        ZKNDTL_COMMON_PfSetBlackToColorless( work->pf_wk );
      }
 
      // �^�b�`�o�[
      if( ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) != ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR )
      {
        ZUKAN_DETAIL_TOUCHBAR_SetType(
            touchbar,
            ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
            ZUKAN_DETAIL_TOUCHBAR_DISP_VOICE );
        ZUKAN_DETAIL_TOUCHBAR_Appear( touchbar, ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE );
      }
      else
      {
        ZUKAN_DETAIL_TOUCHBAR_SetDispOfGeneral(
            touchbar,
            ZUKAN_DETAIL_TOUCHBAR_DISP_VOICE );
      }
      ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, FALSE );  // ZUKAN_DETAIL_TOUCHBAR_SetType�̂Ƃ���Unlock��ԂȂ̂�
      {
        GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
        ZUKAN_DETAIL_TOUCHBAR_SetCheckFlipOfGeneral(
            touchbar,
            GAMEDATA_GetShortCut( gamedata, SHORTCUT_ID_ZUKAN_VOICE ) );
      }
      // �^�C�g���o�[
      if( ZUKAN_DETAIL_HEADBAR_GetState( headbar ) != ZUKAN_DETAIL_HEADBAR_STATE_APPEAR )
      {
        ZUKAN_DETAIL_HEADBAR_SetType(
            headbar,
            ZUKAN_DETAIL_HEADBAR_TYPE_VOICE );
        ZUKAN_DETAIL_HEADBAR_Appear( headbar );
      }
    }
    break;
  case SEQ_FADE_IN:
    {
      if( //   (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_m ))
          //&& (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_s ))
             (!ZKNDTL_COMMON_PfIsExecute( work->pf_wk ))
          && ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) == ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR
          && ZUKAN_DETAIL_HEADBAR_GetState( headbar ) == ZUKAN_DETAIL_HEADBAR_STATE_APPEAR )
      {
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
        ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, TRUE );  // ZUKAN_DETAIL_TOUCHBAR_SetType�̂Ƃ���Unlock��ԂȂ̂�

        //Zukan_Detail_Voice_AlphaInit( param, work, cmn );

        *seq = SEQ_MAIN;
      }
    }
    break;
  case SEQ_MAIN:
    {
      if( work->end_cmd != END_CMD_NONE )
      {
        //Zukan_Detail_Voice_AlphaExit( param, work, cmn );
        
        *seq = SEQ_FADE_OUT;

        //// �t�F�[�h
        //ZKNDTL_COMMON_FadeSetColorlessToBlack( work->fade_wk_m );
        //ZKNDTL_COMMON_FadeSetColorlessToBlack( work->fade_wk_s );

        // �p���b�g�t�F�[�h
        {
          ZKNDTL_COMMON_PfSetColorlessToBlack( work->pf_wk );
        }

        // �^�C�g���o�[
        ZUKAN_DETAIL_HEADBAR_Disappear( headbar );
        
        if( work->end_cmd == END_CMD_OUTSIDE )
        {
          // �^�b�`�o�[
          ZUKAN_DETAIL_TOUCHBAR_Disappear( touchbar, ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE );
        }

        // BGM
        Zukan_Detail_Voice_BgmExit( param, work, cmn );
      }
      else
      {
        // �Đ��{�^���̃L�[or�^�b�`
        Zukan_Detail_Voice_KeyTouchPlayButton( param, work, cmn );
      }
    }
    break;
  case SEQ_FADE_OUT:
    {
      BOOL b_next_seq = FALSE;

      if( //   (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_m ))
          //&& (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_s ))
             (!ZKNDTL_COMMON_PfIsExecute( work->pf_wk ))
          && ZUKAN_DETAIL_HEADBAR_GetState( headbar ) == ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR )
      {
        if( work->end_cmd == END_CMD_OUTSIDE )
        {
          if( ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) == ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR )
          {
            b_next_seq = TRUE;
          }
        }
        else
        {
          b_next_seq = TRUE;
        }
      }

      if( b_next_seq )
      {
        *seq = SEQ_FADE_CHANGE_AFTER;
      }
    }
    break;
  case SEQ_FADE_CHANGE_AFTER:
    {
      *seq = SEQ_END;

      // ZKNDTL_COMMON_FadeSetColorlessImmediately��G2_SetBlendBrightnessExt��G2S_SetBlendBrightnessExt��ݒ肵�Ă���̂ŁA
      // ���̑O��Zukan_Detail_Voice_AlphaExit���ĂԂ���
      Zukan_Detail_Voice_AlphaExit( param, work, cmn );
      
      // �t�F�[�h
      ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_M, work->fade_wk_m );
      ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_S, work->fade_wk_s );
    }
    break;
  case SEQ_END:
    {
      return ZKNDTL_PROC_RES_FINISH;
    }
    break;
  }


#ifdef DEBUG_SOUND_TEST  // ���ꂪ��`����Ă���Ƃ��A�T�E���h�̃e�X�g���\
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    OS_Printf( "speed = %d, volume = %d, size = %d, rate = %d\n",
        PMV_GetSpeed(work->voice_idx), PMV_GetVolume(work->voice_idx), PMV_GetWaveSize(work->voice_idx), PMV_GetWaveRate(work->voice_idx) );
  }
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
  }
#endif


  if( *seq >= SEQ_FADE_IN )
  {
    // �{�C�X�̍Đ�����
    Zukan_Detail_Voice_UpdateTime( param, work, cmn );
    // �O���t 
    Zukan_Detail_Voice_GraphMain( param, work, cmn );
    // �|�P�����̖��A�j��(�c�X�P�[���g��)
    Zukan_Detail_Voice_PokeCryMain( param, work, cmn );
  }

  if( *seq >= SEQ_PREPARE )
  {
    // �|�P������
    Zukan_Detail_Voice_MainName( param, work, cmn );
    
    // �Ŕw��
    ZKNDTL_COMMON_RearMain( work->rear_wk_m );
    ZKNDTL_COMMON_RearMain( work->rear_wk_s );
  }

  // �t�F�[�h
  ZKNDTL_COMMON_FadeMain( work->fade_wk_m, work->fade_wk_s );
  // �p���b�g�t�F�[�h
  {
    ZKNDTL_COMMON_PfMain( work->pf_wk );
  }

  // BGM
  Zukan_Detail_Voice_BgmMain( param, work, cmn );

  return ZKNDTL_PROC_RES_CONTINUE;
}

//-------------------------------------
/// PROC ���ߏ���
//=====================================
static void Zukan_Detail_Voice_CommandFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn, ZKNDTL_COMMAND cmd )
{
  if( mywk )
  {
    ZUKAN_DETAIL_VOICE_PARAM*    param    = (ZUKAN_DETAIL_VOICE_PARAM*)pwk;
    ZUKAN_DETAIL_VOICE_WORK*     work     = (ZUKAN_DETAIL_VOICE_WORK*)mywk;

    ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
    
    BOOL b_valid_cmd = FALSE;  // cmd��ZKNDTL_CMD_NONE�ȊO��ZKNDTL_CMD_???�̂Ƃ�TRUE�BZKNDTL_CMD_NONE��ZKNDTL_SCMD_???�̂Ƃ�FALSE�B

    // ���͕s��
    switch( cmd )
    {
    case ZKNDTL_SCMD_CLOSE:
    case ZKNDTL_SCMD_RETURN:
    case ZKNDTL_SCMD_INFO:
    case ZKNDTL_SCMD_MAP:
    case ZKNDTL_SCMD_FORM:
    case ZKNDTL_SCMD_CUR_D:
    case ZKNDTL_SCMD_CUR_U:
    case ZKNDTL_SCMD_CHECK:
      {
        work->input_enable = FALSE;
      }
      break;
    }
    // ���͉�
    switch( cmd )
    {
    case ZKNDTL_CMD_CLOSE:
    case ZKNDTL_CMD_RETURN:
    case ZKNDTL_CMD_INFO:
    case ZKNDTL_CMD_MAP:
    case ZKNDTL_CMD_FORM:
    case ZKNDTL_CMD_CUR_D:
    case ZKNDTL_CMD_CUR_U:
    case ZKNDTL_CMD_CHECK:
      {
        work->input_enable = TRUE;
        b_valid_cmd = TRUE;
      }
      break;
    }

    // �R�}���h
    switch( cmd )
    {
    case ZKNDTL_CMD_NONE:
      {
      }
      break;
    case ZKNDTL_CMD_CLOSE:
    case ZKNDTL_CMD_RETURN:
      {
        work->end_cmd = END_CMD_OUTSIDE;
      }
      break;
    case ZKNDTL_CMD_INFO:
    case ZKNDTL_CMD_MAP:
    case ZKNDTL_CMD_FORM:
      {
        work->end_cmd = END_CMD_INSIDE;
      }
      break;
    case ZKNDTL_CMD_CUR_D:
      {
        u16 monsno_curr;
        u16 monsno_go;
        monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
        ZKNDTL_COMMON_GoToNextPoke(cmn);
        monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
        if( monsno_curr != monsno_go )
        {
          Zukan_Detail_Voice_ChangePoke(param, work, cmn);
        }
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_CUR_U:
      {
        u16 monsno_curr;
        u16 monsno_go;
        monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
        ZKNDTL_COMMON_GoToPrevPoke(cmn);
        monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
        if( monsno_curr != monsno_go )
        {
          Zukan_Detail_Voice_ChangePoke(param, work, cmn);
        }
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_CHECK:
      {
        GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
        GAMEDATA_SetShortCut(
            gamedata,
            SHORTCUT_ID_ZUKAN_VOICE,
            ZUKAN_DETAIL_TOUCHBAR_GetCheckFlipOfGeneral( touchbar ) );
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    default:
      {
        if( b_valid_cmd )
        {
          ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
        }
      }
      break;
    }
  }
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void Zukan_Detail_Voice_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_DETAIL_VOICE_WORK*     work     = (ZUKAN_DETAIL_VOICE_WORK*)wk;

  if( work->graph_req )
  {
    GFL_BMPWIN_TransVramCharacter( work->graph_bmpwin );
 
    GFL_BG_SetScroll( BG_FRAME_M_TIME, GFL_BG_SCROLL_X_SET, SGRAPH_BITMAP_SCREEN_SCROLL_BASE_X + work->graph_x_scroll );
    GFL_BG_SetScroll( BG_FRAME_M_TIME, GFL_BG_SCROLL_Y_SET, SGRAPH_BITMAP_SCREEN_SCROLL_BASE_Y );
    
    GFL_BG_LoadScreenReq( BG_FRAME_M_TIME );
    
    work->graph_req = FALSE;
  }

  // �p���b�g�t�F�[�h
  {
    ZKNDTL_COMMON_PfTrans( work->pf_wk );
  }
}

//-------------------------------------
/// BGM
//=====================================
static void Zukan_Detail_Voice_BgmInit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  PMSND_FadeOutBGM( FSND_FADE_SHORT );
  work->bgm_state = BGM_STATE_FADE_OUT;
}
static void Zukan_Detail_Voice_BgmExit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->bgm_state == BGM_STATE_PUSH )
  {
    PMSND_PopBGM();
    PMSND_PauseBGM( FALSE );
  }
  PMSND_FadeInBGM( FSND_FADE_NORMAL );
  work->bgm_state = BGM_STATE_FADE_IN;
}
static void Zukan_Detail_Voice_BgmMain( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->bgm_state == BGM_STATE_FADE_OUT )
  {
    if( !PMSND_CheckFadeOnBGM() )
    {
      PMSND_PauseBGM( TRUE );
      PMSND_PushBGM();
      work->bgm_state = BGM_STATE_PUSH;
    }
  }
}

//-------------------------------------
/// OBJ
//=====================================
static void Zukan_Detail_Voice_ObjInit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, param->heap_id );

    work->obj_res[OBJ_RES_NUMBER_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
                                     handle,
                                     NARC_zukan_gra_info_info_obj_NCLR,
                                     CLSYS_DRAW_MAIN,
                                     OBJ_PAL_POS_M_NUMBER*0x20,
                                     0,
                                     OBJ_PAL_NUM_M_NUMBER,
                                     param->heap_id );	
    work->obj_res[OBJ_RES_NUMBER_NCG] = GFL_CLGRP_CGR_Register(
                                     handle,
                                     NARC_zukan_gra_info_info_obj_NCGR,
                                     FALSE,
                                     CLSYS_DRAW_MAIN,
                                     param->heap_id );
    work->obj_res[OBJ_RES_NUMBER_NCE] = GFL_CLGRP_CELLANIM_Register(
                                     handle,
                                     NARC_zukan_gra_info_info_obj_NCER,
                                     NARC_zukan_gra_info_info_obj_NANR,
                                     param->heap_id );

    work->obj_res[OBJ_RES_FIELD_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
                                     handle,
                                     NARC_zukan_gra_info_voicewin_obj_NCLR,
                                     CLSYS_DRAW_SUB,
                                     OBJ_PAL_POS_S_FIELD*0x20,
                                     0,
                                     OBJ_PAL_NUM_S_FIELD,
                                     param->heap_id );	
    work->obj_res[OBJ_RES_FIELD_NCG] = GFL_CLGRP_CGR_Register(
                                     handle,
                                     NARC_zukan_gra_info_voicewin_obj_NCGR,
                                     FALSE,
                                     CLSYS_DRAW_SUB,
                                     param->heap_id );
    work->obj_res[OBJ_RES_FIELD_NCE] = GFL_CLGRP_CELLANIM_Register(
                                     handle,
                                     NARC_zukan_gra_info_voicewin_obj_NCER,
                                     NARC_zukan_gra_info_voicewin_obj_NANR,
                                     param->heap_id );

    GFL_ARC_CloseDataHandle( handle );
  }

  // CLWK�쐬
  {
    u8 i;
    GFL_CLWK_DATA cldata;

    for( i=0; i<OBJ_CELL_MAX; i++ )
    {
      GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
      cldata.pos_x      = obj_setup_info[i][0];
      cldata.pos_y      = obj_setup_info[i][1];
      cldata.anmseq     = obj_setup_info[i][2];
      cldata.softpri    = obj_setup_info[i][3];
      cldata.bgpri      = obj_setup_info[i][4];

      work->obj_cell[i] = GFL_CLACT_WK_Create(
                             work->clunit,
                             work->obj_res[obj_setup_info[i][5]], work->obj_res[obj_setup_info[i][6]], work->obj_res[obj_setup_info[i][7]],
                             &cldata,
                             obj_setup_info[i][8],
                             param->heap_id );

      GFL_CLACT_WK_SetDrawEnable( work->obj_cell[i], TRUE );
      GFL_CLACT_WK_SetObjMode( work->obj_cell[i], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    }

    GFL_CLACT_WK_SetAutoAnmFlag( work->obj_cell[OBJ_CELL_FIELD], TRUE );
  }
}
static void Zukan_Detail_Voice_ObjExit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // CLWK�j��
  {
    u8 i;
    for( i=0; i<OBJ_CELL_MAX; i++ )
    {
      GFL_CLACT_WK_Remove( work->obj_cell[i] );
    }
  }

  // ���\�[�X�j��
  {
    GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_RES_NUMBER_NCL] );
    GFL_CLGRP_CGR_Release( work->obj_res[OBJ_RES_NUMBER_NCG] );
    GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_RES_NUMBER_NCE] );

    GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_RES_FIELD_NCL] );
    GFL_CLGRP_CGR_Release( work->obj_res[OBJ_RES_FIELD_NCG] );
    GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_RES_FIELD_NCE] );
  }
}
static void Zukan_Detail_Voice_ObjTimeDisplay( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                               u16 disp_sec, u16 disp_milli )
{
  // ���̊֐��̌Ăяo�����ŁA�l�̌ܓ���������Ɏ��߂���͂���Ă���Ă���
  u8  cell_idx[4] =
  {
    OBJ_CELL_NUMBER0,
    OBJ_CELL_NUMBER1,
    OBJ_CELL_NUMBER2,
    OBJ_CELL_NUMBER3,
  };
  u16 n[4];
  u8  i;

  n[0] = disp_sec / 10;
  if( n[0] > 9 ) n[0] = 9;
  n[1] = disp_sec % 10;

  n[2] = disp_milli /10;
  if( n[2] > 9 ) n[2] = 9;
  n[3] = disp_milli %10;

  for( i=0; i<4; i++ )
  {
    GFL_CLACT_WK_SetAnmIndex( work->obj_cell[ cell_idx[i] ], n[i] );
  }
}


//-------------------------------------
/// �|�P����2D
//=====================================
static void Zukan_Detail_Voice_CreatePoke( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn, OBJ_SWAP swap_idx )
{
  u16 monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);

  u32 sex;
  BOOL rare;
  u32 form;
  u32 personal_rnd = 0;

  GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
  ZUKAN_SAVEDATA* zkn_sv = GAMEDATA_GetZukanSave( gamedata );
  ZUKANSAVE_GetDrawData(  zkn_sv, monsno, &sex, &rare, &form, param->heap_id );

  if( monsno == MONSNO_PATTIIRU )  // personal_rnd��monsno==MONSNO_PATTIIRU�̂݃Z�[�u���Ă���
  {
    personal_rnd = ZUKANSAVE_GetPokeRandomFlag( zkn_sv, ZUKANSAVE_RANDOM_PACHI );
  }

  // ���\�[�X��ǂݍ���
  {
    ARCHANDLE*            handle;

    CLSYS_DRAW_TYPE draw_type = CLSYS_DRAW_SUB;

    // �n���h��
    handle = POKE2DGRA_OpenHandle( param->heap_id );
    // ���\�[�X�ǂ݂���
    work->poke_ncg[swap_idx] = POKE2DGRA_OBJ_CGR_Register(
                         handle,
                         (int)monsno, (int)form, (int)sex, (int)rare,
                         POKEGRA_DIR_FRONT, FALSE,
                         personal_rnd,
                         draw_type, param->heap_id );
    work->poke_ncl[swap_idx] = POKE2DGRA_OBJ_PLTT_Register(
                         handle,
                         (int)monsno, (int)form, (int)sex, (int)rare,
                         POKEGRA_DIR_FRONT, FALSE,
                         draw_type,
                         ( OBJ_PAL_POS_S_POKE + OBJ_PAL_NUM_S_POKE * swap_idx ) * 0x20, param->heap_id );
    work->poke_nce[swap_idx] = POKE2DGRA_OBJ_CELLANM_Register(
                         (int)monsno, (int)form, (int)sex, (int)rare,
                         POKEGRA_DIR_FRONT, FALSE,
                         ZKNDTL_OBJ_MAPPING_S,
                         draw_type, param->heap_id );

    // �p���b�g�t�F�[�h
    {
      ARCDATID arcdatid = POKEGRA_GetPalArcIndex(
          POKEGRA_GetArcID(), 
          (int)monsno, (int)form, (int)sex, (int)rare,
          POKEGRA_DIR_FRONT, FALSE );

      ZKNDTL_COMMON_PfSetPaletteDataFromArchandle(
          work->pf_wk,
          handle,
          arcdatid,
          param->heap_id,
          FADE_SUB_OBJ,
          OBJ_PAL_NUM_S_POKE * 0x20,
          ( OBJ_PAL_POS_S_POKE + OBJ_PAL_NUM_S_POKE * swap_idx ) * 16,  // �J���[�P�ʂȂ̂ŁA���o�C�g�ڂ��ł͂Ȃ��A���ڂ̐F���ł���B
          0 );
    }

    GFL_ARC_CloseDataHandle( handle );
  }

#if 0
  // OBJ�𐶐�����
  {
    GFL_CLWK_DATA cldata;

    CLSYS_DEFREND_TYPE defrend_type = CLSYS_DEFREND_SUB;

    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x = 128;
    cldata.pos_y = 128;
    work->poke_clwk = GFL_CLACT_WK_Create( work->clunit, 
                                           work->poke_ncg, work->poke_ncl, work->poke_nce,
                                           &cldata, defrend_type, param->heap_id );
  
    GFL_CLACT_WK_SetObjMode( work->poke_clwk, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
  }
#endif

  // �g�k�\��OBJ�𐶐�����
  {
    GFL_CLWK_AFFINEDATA claffinedata;
    GFL_STD_MemClear( &claffinedata, sizeof(GFL_CLWK_AFFINEDATA) );

    claffinedata.clwkdata.pos_x    = POKE_CLWK_BASE_POS_X;
    claffinedata.clwkdata.pos_y    = POKE_CLWK_BASE_POS_Y;
    claffinedata.clwkdata.anmseq   = 0;
    claffinedata.clwkdata.softpri  = 0;
    claffinedata.clwkdata.bgpri    = 0;

    claffinedata.affinepos_x    = 0;
    claffinedata.affinepos_y    = 0;
    claffinedata.scale_x        = FX_F32_TO_FX32( POKE_CLWK_BASE_SCALE_X );
    claffinedata.scale_y        = FX_F32_TO_FX32( POKE_CLWK_BASE_SCALE_Y );
    claffinedata.rotation       = 0;  // ��]�p�x(0�`0xffff 0xffff��360�x)
    claffinedata.affine_type    = CLSYS_AFFINETYPE_DOUBLE;

    work->poke_clwk[swap_idx] = GFL_CLACT_WK_CreateAffine(
        work->clunit,
        work->poke_ncg[swap_idx],
        work->poke_ncl[swap_idx],
        work->poke_nce[swap_idx],
        &claffinedata,
        CLSYS_DEFREND_SUB,
        param->heap_id );

    //GFL_CLACT_WK_SetObjMode( work->poke_clwk, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    GFL_CLACT_WK_SetObjMode( work->poke_clwk[swap_idx], GX_OAM_MODE_NORMAL );  // BG�ƂƂ��ɂ���OBJ���Â��������̂́A�t�F�[�h�C���ƃt�F�[�h�A�E�g�̂Ƃ������ł���A
                                                                     // �|�P������ύX���������̂Ƃ��́A�Â������蔼�����ɂ����肵�����Ȃ��B
                                                                     // BG�ƂƂ��ɂ���OBJ���Â��������Ƃ��́A���̊֐��̌�ɐݒ肷�邱�ƁB
  }

  work->poke_state = POKE_CLWK_STATE_SILENT;
}
static void Zukan_Detail_Voice_DeletePoke( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn, OBJ_SWAP swap_idx )
{
  if( work->poke_clwk[swap_idx] )
  {
    // OBJ��j������
    {
      GFL_CLACT_WK_Remove( work->poke_clwk[swap_idx] );
      work->poke_clwk[swap_idx] = NULL;
    }

    // ���\�[�X��j������
    {
      GFL_CLGRP_PLTT_Release( work->poke_ncl[swap_idx] );
      GFL_CLGRP_CGR_Release( work->poke_ncg[swap_idx] );
      GFL_CLGRP_CELLANIM_Release( work->poke_nce[swap_idx] );
    }
  }
}

// �|�P�����̖��A�j��(�c�X�P�[���g��)
static void Zukan_Detail_Voice_PokeCryMain( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  GFL_CLACTPOS  pos;
  GFL_CLSCALE   scale;
  f32           sy;

  switch( work->poke_state )
  {
  case POKE_CLWK_STATE_SILENT:
    {
    }
    break;
  case POKE_CLWK_STATE_CRY_OPEN:
    {
      GFL_CLACT_WK_GetPos( work->poke_clwk[work->poke_swap_curr], &pos, CLSYS_DEFREND_SUB );
      GFL_CLACT_WK_GetScale( work->poke_clwk[work->poke_swap_curr], &scale );
      sy = FX_FX32_TO_F32( scale.y );
      sy += POKE_CLWK_SCALE_Y_INC;
      if( sy >= POKE_CLWK_SCALE_Y_MAX )
      {
        sy = POKE_CLWK_SCALE_Y_MAX;
        work->poke_state = POKE_CLWK_STATE_CRY_STOP;
      }
      pos.y = POKE_CLWK_BASE_POS_Y - (s16)( POKE_CLWK_SIZE * ( sy - 1.0f ) / 2.0f );
      scale.y = FX_F32_TO_FX32( sy );

      GFL_CLACT_WK_SetPos( work->poke_clwk[work->poke_swap_curr], &pos, CLSYS_DEFREND_SUB );
      GFL_CLACT_WK_SetScale( work->poke_clwk[work->poke_swap_curr], &scale );
    }
    break;
  case POKE_CLWK_STATE_CRY_STOP:
    {
    }
    break;
  case POKE_CLWK_STATE_CRY_CLOSE:
    {
      GFL_CLACT_WK_GetPos( work->poke_clwk[work->poke_swap_curr], &pos, CLSYS_DEFREND_SUB );
      GFL_CLACT_WK_GetScale( work->poke_clwk[work->poke_swap_curr], &scale );
      sy = FX_FX32_TO_F32( scale.y );
      sy -= POKE_CLWK_SCALE_Y_DEC;
      if( sy <= POKE_CLWK_BASE_SCALE_Y )
      {
        sy = POKE_CLWK_BASE_SCALE_Y;
        work->poke_state = POKE_CLWK_STATE_SILENT;
        pos.y = POKE_CLWK_BASE_POS_Y;
      }
      else
      {
        pos.y = POKE_CLWK_BASE_POS_Y - (s16)( POKE_CLWK_SIZE * ( sy - 1.0f ) / 2.0f );
      }
      scale.y = FX_F32_TO_FX32( sy );

      GFL_CLACT_WK_SetPos( work->poke_clwk[work->poke_swap_curr], &pos, CLSYS_DEFREND_SUB );
      GFL_CLACT_WK_SetScale( work->poke_clwk[work->poke_swap_curr], &scale );
    }
    break;
  }
}
static void Zukan_Detail_Voice_PokeCryStart( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  GFL_CLACTPOS  pos;
  GFL_CLSCALE   scale;

  pos.x = POKE_CLWK_BASE_POS_X;
  pos.y = POKE_CLWK_BASE_POS_Y;
  scale.x = FX_F32_TO_FX32( POKE_CLWK_BASE_SCALE_X );
  scale.y = FX_F32_TO_FX32( POKE_CLWK_BASE_SCALE_Y );

  GFL_CLACT_WK_SetPos( work->poke_clwk[work->poke_swap_curr], &pos, CLSYS_DEFREND_SUB );
  GFL_CLACT_WK_SetScale( work->poke_clwk[work->poke_swap_curr], &scale );

  work->poke_state = POKE_CLWK_STATE_CRY_OPEN;
}
static void Zukan_Detail_Voice_PokeCryEnd( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->poke_state == POKE_CLWK_STATE_CRY_OPEN || work->poke_state == POKE_CLWK_STATE_CRY_STOP )
  {
    work->poke_state = POKE_CLWK_STATE_CRY_CLOSE;
  }
}

//-------------------------------------
/// �|�P������
//=====================================
static void Zukan_Detail_Voice_CreateNameBase( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �p���b�g
  PALTYPE paltype = PALTYPE_SUB_BG;

  GFL_ARC_UTIL_TransVramPaletteEx( ARCID_FONT, NARC_font_default_nclr,
       paltype,
       0,
       BG_PAL_POS_S_NAME * 0x20,
       BG_PAL_NUM_S_NAME * 0x20,
       param->heap_id );

  // �r�b�g�}�b�v�E�B���h�E
  work->name_bmpwin = GFL_BMPWIN_Create( BG_FRAME_S_NAME,
                                         10, 5, 12, 4,
                                         BG_PAL_POS_S_NAME,
                                         GFL_BMP_CHRAREA_GET_B );
  work->name_trans = FALSE;

  // �N���A
  Zukan_Detail_Voice_DeleteName( param, work, cmn );

  GFL_BMPWIN_TransVramCharacter( work->name_bmpwin );  // �������������Ȃ��̂ŁA�R�����g�A�E�g���Ă����̂ŁA�����œ]��
}
static void Zukan_Detail_Voice_DeleteNameBase( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  work->name_trans = FALSE;
  PRINTSYS_QUE_Clear( work->print_que );
  GFL_BMPWIN_Delete( work->name_bmpwin );
}
static void Zukan_Detail_Voice_CreateName( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u16 monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);

  STRBUF* strbuf = GFL_MSG_CreateString( GlobalMsg_PokeName, monsno );

  u16 str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
  u16 bmp_width = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(work->name_bmpwin) );
  u16 x = ( bmp_width - str_width ) / 2;

/*
  u16 str_height = (u16)( PRINTSYS_GetStrHeight( strbuf, work->font ) );
  u16 bmp_height = GFL_BMP_GetSizeY( GFL_BMPWIN_GetBmp(work->name_bmpwin) );
  u16 y = ( bmp_height - str_height ) / 2;
*/
  //u16 y = 10;  // �v�Z�ŏo�����ʒu�͒��S����Ɍ�����̂�
  u16 y = 1;  // OBJ_CELL_FIELD�ɍ��킹�� 

  PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->name_bmpwin ),
                          x, y, strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );

  GFL_STR_DeleteBuffer( strbuf );

  work->name_trans = TRUE;

  // �ς�ł��Ȃ���������Ȃ����A1�x�Ă�ł���
  Zukan_Detail_Voice_MainName( param, work, cmn );
}
static void Zukan_Detail_Voice_DeleteName( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  work->name_trans = FALSE;
  PRINTSYS_QUE_Clear( work->print_que );
  
  // �N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->name_bmpwin ), 0 );

  //GFL_BMPWIN_TransVramCharacter( work->name_bmpwin );  // �������������Ȃ��̂ŁA�R�����g�A�E�g
}
static void Zukan_Detail_Voice_MainName( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �X�N���[���]��
  if( work->name_trans )
  {
    if( !PRINTSYS_QUE_IsExistTarget( work->print_que, GFL_BMPWIN_GetBmp(work->name_bmpwin) ) )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( work->name_bmpwin );
      work->name_trans = FALSE;
    }
  }
}


//-------------------------------------
/// �{�C�X�Đ�����
//=====================================
static void Zukan_Detail_Voice_CreateTimeBase( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �p���b�g
  PALTYPE paltype = PALTYPE_MAIN_BG;

  GFL_ARC_UTIL_TransVramPaletteEx( ARCID_FONT, NARC_font_default_nclr,
       paltype,
       0,
       BG_PAL_POS_M_TIME * 0x20,
       BG_PAL_NUM_M_TIME * 0x20,
       param->heap_id );

#ifdef TIME_BMPWIN
  // �r�b�g�}�b�v�E�B���h�E
  work->time_bmpwin = GFL_BMPWIN_Create( BG_FRAME_M_TIME,
                                         26, 18, 5, 2,
                                         BG_PAL_POS_M_TIME,
                                         GFL_BMP_CHRAREA_GET_B );
 
  // �N���A
  Zukan_Detail_Voice_DeleteTime( param, work, cmn );

  // ���b�Z�[�W
  work->time_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
                           ARCID_MESSAGE, NARC_message_zkn_dat,
                           param->heap_id );
#endif
}
static void Zukan_Detail_Voice_DeleteTimeBase( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
#ifdef TIME_BMPWIN
  GFL_MSG_Delete( work->time_msgdata );
  GFL_BMPWIN_Delete( work->time_bmpwin );
#endif
}
static void Zukan_Detail_Voice_CreateTime( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                           u16 disp_sec, u16 disp_milli )
{
#ifdef TIME_BMPWIN
  WORDSET* wordset = WORDSET_Create( param->heap_id );
  STRBUF* src_strbuf = GFL_MSG_CreateString( work->time_msgdata, ZKN_VOICE_TEXT_01 );
  STRBUF* strbuf = GFL_STR_CreateBuffer( STRBUF_TIME_LENGTH, param->heap_id );
    
  WORDSET_RegisterNumber( wordset, 0, disp_sec, 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( wordset, 1, disp_milli, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
  WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
   
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(work->time_bmpwin), 0, 1, strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );  // PRINTSYS_PrintQueColor

  GFL_STR_DeleteBuffer( strbuf );
  GFL_STR_DeleteBuffer( src_strbuf );
  WORDSET_Delete( wordset );

  GFL_BMPWIN_MakeTransWindow_VBlank( work->time_bmpwin );
#endif
}
static void Zukan_Detail_Voice_DeleteTime( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
#ifdef TIME_BMPWIN
  // �N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->time_bmpwin ), 0 );

  GFL_BMPWIN_TransVramCharacter( work->time_bmpwin );
#endif
}

#ifdef GRAPH_GATHER

static void Zukan_Detail_Voice_UpdateTime( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->voice_play )
  {
    {
      OSTick now = OS_GetTick();  // MachineSystem_Init�֐���OS_InitTick�͌Ă΂�Ă���
      OSTick diff;
      u64 diff_millisec;
      if( now >= work->voice_start )
      {
        diff = now - work->voice_start;
      }
      else
      {
        diff = now + ( 0xFFFFFFFFFFFFFFFF - work->voice_start );
      }
      diff_millisec =OS_TicksToMilliSeconds( diff );
      if( diff_millisec > 9990 ) diff_millisec = 9990;
      work->voice_disp_millisec = (u16)diff_millisec;
    }
    if( work->voice_disp_millisec >= work->wave_full_millisec )
    {
      work->voice_disp_millisec = work->wave_full_millisec;
      work->voice_play = FALSE;
      // �|�P�����̖��A�j��(�c�X�P�[���g��)
      Zukan_Detail_Voice_PokeCryEnd( param, work, cmn );
    }
  }

  // �\��
  if( work->voice_disp_millisec != work->voice_prev_disp_millisec )
  {
    u16 disp_sec = work->voice_disp_millisec / 1000;
    u16 disp_milli = work->voice_disp_millisec % 1000;
    disp_milli = disp_milli / 10 + ( (disp_milli%10 < 5) ? (0) : (1) );  // �l�̌ܓ�

    Zukan_Detail_Voice_DeleteTime( param, work, cmn );
    Zukan_Detail_Voice_CreateTime( param, work, cmn, disp_sec, disp_milli );
    Zukan_Detail_Voice_ObjTimeDisplay( param, work, cmn, disp_sec, disp_milli );

    work->voice_prev_disp_millisec = work->voice_disp_millisec;
  }
}

#else

static void Zukan_Detail_Voice_UpdateTime( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->voice_play )
  {
    if( !PMV_CheckPlay() )
    {
      work->voice_play = FALSE;

#ifdef DEBUG_SOUND_TEST  // ���ꂪ��`����Ă���Ƃ��A�T�E���h�̃e�X�g���\
      OS_Printf( "millisec=%d\n", work->voice_prev_disp_millisec );
#endif

    }
    {
      OSTick now = OS_GetTick();  // MachineSystem_Init�֐���OS_InitTick�͌Ă΂�Ă���
      OSTick diff;
      u64 diff_millisec;
      if( now >= work->voice_start )
      {
        diff = now - work->voice_start;
      }
      else
      {
        diff = now + ( 0xFFFFFFFFFFFFFFFF - work->voice_start );
      }
      diff_millisec =OS_TicksToMilliSeconds( diff );
      if( diff_millisec > 9990 ) diff_millisec = 9990;
      work->voice_disp_millisec = (u16)diff_millisec;
    }
  }

  // �\��
  if( work->voice_disp_millisec != work->voice_prev_disp_millisec )
  {
    u16 disp_sec = work->voice_disp_millisec / 1000;
    u16 disp_milli = work->voice_disp_millisec % 1000;
    disp_milli = disp_milli / 10 + ( (disp_milli%10 < 5) ? (0) : (1) );  // �l�̌ܓ�

    Zukan_Detail_Voice_DeleteTime( param, work, cmn );
    Zukan_Detail_Voice_CreateTime( param, work, cmn, disp_sec, disp_milli );
    Zukan_Detail_Voice_ObjTimeDisplay( param, work, cmn, disp_sec, disp_milli );

    work->voice_prev_disp_millisec = work->voice_disp_millisec;
  }
}

#endif

static void Zukan_Detail_Voice_ResetTime( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->voice_play )
  {
    if( PMV_CheckPlay() )
    {
      PMV_StopVoice();
    }
    work->voice_play = FALSE;
  }

  work->voice_disp_millisec = 0;
}

//-------------------------------------
/// �|�P�����ύX
//=====================================
static void Zukan_Detail_Voice_ChangePoke( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // OBJ���݂̕\���ؑ�
  if( work->poke_clwk[work->poke_swap_curr] )
  {
    GFL_CLACT_WK_SetDrawEnable( work->poke_clwk[work->poke_swap_curr], FALSE );
  }

  // ���\�����Ă�����̂�ύX����
  work->poke_swap_curr = ( work->poke_swap_curr +1 ) %OBJ_SWAP_MAX;

  Zukan_Detail_Voice_DeleteName( param, work, cmn );
  Zukan_Detail_Voice_DeletePoke( param, work, cmn, work->poke_swap_curr );

  Zukan_Detail_Voice_CreatePoke( param, work, cmn, work->poke_swap_curr );
  Zukan_Detail_Voice_CreateName( param, work, cmn );

  Zukan_Detail_Voice_ResetTime( param, work, cmn );

  Zukan_Detail_Voice_GraphReset( param, work, cmn );
}

//-------------------------------------
/// �Đ��{�^���̃L�[or�^�b�`
//=====================================
static void Zukan_Detail_Voice_KeyTouchPlayButton( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  BOOL b_push = FALSE;
  u32  x, y;

  if( work->input_enable )
  {
    // �L�[����
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      b_push = TRUE;
    }
    // �^�b�`����
    else if( GFL_UI_TP_GetPointTrg(&x, &y) )
    {
      if(    0<=x && x<256
          && 0<=y && y<168 )
      {
        GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
        b_push = TRUE;
      }
    }
  }

  if( b_push )
  {
      ZUKAN_SAVEDATA* zkn_sv = GAMEDATA_GetZukanSave( ZKNDTL_COMMON_GetGamedata(cmn) );
      u16  monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);
      u32  formno;
      u32  sex;
      BOOL rare;

      // ���ݕ\������p
      ZUKANSAVE_GetDrawData(  zkn_sv, monsno, &sex, &rare, &formno, param->heap_id );

      work->voice_idx = PMV_PlayVoice( monsno, formno );
      work->voice_disp_millisec = 0;
      work->voice_start = OS_GetTick();  // MachineSystem_Init�֐���OS_InitTick�͌Ă΂�Ă���
      work->voice_play = TRUE;

      Zukan_Detail_Voice_WaveDataSetup( param, work, cmn, monsno, (u16)formno );

      // �|�P�����̖��A�j��(�c�X�P�[���g��)
      Zukan_Detail_Voice_PokeCryStart( param, work, cmn );
  }
}

//-------------------------------------
/// �O���t
//=====================================
static void Zukan_Detail_Voice_GraphInit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
#ifdef USE_SGRAPH

  work->graph_screen = GFL_HEAP_AllocClearMemory( param->heap_id, sizeof(u16) * SGRAPH_BITMAP_CHAR_SIZE_X * SGRAPH_BITMAP_CHAR_SIZE_Y );

  work->graph_bmpwin = GFL_BMPWIN_Create(
                           BG_FRAME_M_TIME,
                           SGRAPH_BITMAP_SCREEN_POS_X, SGRAPH_BITMAP_SCREEN_POS_Y,
                           SGRAPH_BITMAP_CHAR_SIZE_X, SGRAPH_BITMAP_CHAR_SIZE_Y,
                           BG_PAL_POS_M_TIME,
                           GFL_BMP_CHRAREA_GET_B );
  Zukan_Detail_Voice_GraphReset( param, work, cmn );

  work->graph_full_bmp_data = GFL_BMP_Create( SGRAPH_DATA_FULL_SIZE_X_MAX /8 +1, SGRAPH_BITMAP_CHAR_SIZE_Y, GFL_BMP_16_COLOR, param->heap_id );

#else  // #ifdef USE_SGRAPH

#ifdef GRAPH_SWAP_BMPWIN
  u8 i;
  for( i=0; i<GRAPH_SWAP_BMPWIN_IDX_MAX; i++ )
  {
    work->graph_bmpwin[i] = GFL_BMPWIN_Create(
                                BG_FRAME_M_TIME,
                                1, 2,
                                GRAPH_BITMAP_CHAR_SIZE_X, GRAPH_BITMAP_CHAR_SIZE_Y,
                                BG_PAL_POS_M_TIME,
                                GFL_BMP_CHRAREA_GET_B );
  }
  Zukan_Detail_Voice_GraphReset( param, work, cmn );
  work->graph_swap_bmpwin_idx = GRAPH_SWAP_BMPWIN_IDX_0; 
  GFL_BMPWIN_MakeTransWindow_VBlank( work->graph_bmpwin[work->graph_swap_bmpwin_idx] );
#else
  work->graph_bmpwin = GFL_BMPWIN_Create(
                           BG_FRAME_M_TIME,
                           1, 2,
                           GRAPH_BITMAP_CHAR_SIZE_X, GRAPH_BITMAP_CHAR_SIZE_Y,
                           BG_PAL_POS_M_TIME,
                           GFL_BMP_CHRAREA_GET_B );
  Zukan_Detail_Voice_GraphReset( param, work, cmn );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->graph_bmpwin );
#endif

#ifdef GRAPH_GATHER
  work->graph_full_bmp_data = GFL_BMP_Create( GRAPH_DATA_FULL_SIZE_X_MAX /8 +1, GRAPH_BITMAP_CHAR_SIZE_Y, GFL_BMP_16_COLOR, param->heap_id );
#endif

#endif  // #ifdef USE_SGRAPH
}
static void Zukan_Detail_Voice_GraphExit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
#ifdef USE_SGRAPH

  GFL_BMP_Delete( work->graph_full_bmp_data );
  GFL_BMPWIN_Delete( work->graph_bmpwin );
  GFL_HEAP_FreeMemory( work->graph_screen );
  
#else  // #ifdef USE_SGRAPH

#ifdef GRAPH_GATHER
  GFL_BMP_Delete( work->graph_full_bmp_data );
#endif

#ifdef GRAPH_SWAP_BMPWIN
  {
    u8 i;
    for( i=0; i<GRAPH_SWAP_BMPWIN_IDX_MAX; i++ )
    {
      GFL_BMPWIN_Delete( work->graph_bmpwin[i] );
    }
  }
#else
  GFL_BMPWIN_Delete( work->graph_bmpwin );
#endif

#endif  // #ifdef USE_SGRAPH
}

#ifdef USE_SGRAPH

static void Zukan_Detail_Voice_GraphMain( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->graph_data_move_wait_count > 0 )
  {
    work->graph_data_move_wait_count--;
  }
  else
  {
    Zukan_Detail_Voice_GraphDrawBitmap( param, work, cmn );
    work->graph_data_move_wait_count = SGRAPH_DATA_MOVE_WAIT;
  }
}

#else  // #ifdef USE_SGRAPH

#ifdef GRAPH_GATHER

static void Zukan_Detail_Voice_GraphMain( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->graph_data_move_wait_count > 0 )
  {
    work->graph_data_move_wait_count--;
  }
  else
  {
    Zukan_Detail_Voice_GraphDrawBitmap( param, work, cmn );
    work->graph_data_move_wait_count = GRAPH_DATA_MOVE_WAIT;
  }
}

#else

static void Zukan_Detail_Voice_GraphMain( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->graph_data_move_wait_count > 0 )
  {
    work->graph_data_move_wait_count--;
  }
  else
  {
    // GRAPH_DATA_INTERVAL_X���Ƃ̃L�[�t���[���ɂ����f�[�^�͓���Ȃ�
    while( work->graph_data_prev_x <= GRAPH_DATA_MAX_X + GRAPH_DATA_MOVE_SPEED )  // GRAPH_DATA_MOVE_SPEED��GRAPH_DATA_INTERVAL_X���傫���ꍇ��1�x��2�񏑂����Ƃ�����
    {
      s16 curr_x = work->graph_data_prev_x + GRAPH_DATA_INTERVAL_X;
      s16 value;
      // ���ꂩ�猩��f�[�^
      if( PMV_CheckPlay() && work->wave_data_curr_pos<WAVE_DATA_SIZE_MAX )
      {
        value = ( (s16)(work->wave_data_begin[work->wave_data_curr_pos]) * WAVE_DATA_VALUE_MAX_TO_GRAPH_DATA ) / WAVE_DATA_VALUE_MAX;
        value = MATH_CLAMP( value, GRAPH_DATA_SATURATION_MIN_Y, GRAPH_DATA_SATURATION_MAX_Y );
        work->wave_data_curr_pos += work->wave_data_interval;
      }
      else
      {
        value = 0;
      }
      work->graph_data_y[curr_x] = value;
      work->graph_data_prev_x = curr_x;
    }

    // �ړ�������
    {
      s16 i;
      for( i=0; i<GRAPH_DATA_SIZE_X -GRAPH_DATA_MOVE_SPEED; i++ )
      {
        work->graph_data_y[i] = work->graph_data_y[i +GRAPH_DATA_MOVE_SPEED];
      }
      work->graph_data_prev_x -= GRAPH_DATA_MOVE_SPEED;
    }

    work->graph_data_move_wait_count = GRAPH_DATA_MOVE_WAIT;
    Zukan_Detail_Voice_GraphDrawBitmap( param, work, cmn );
  }
}

#endif

#endif  // #ifdef USE_SGRAPH

#ifdef USE_SGRAPH

static void Zukan_Detail_Voice_GraphReset( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  work->graph_x_scroll        = 0;
  work->graph_x_chara_offset  = 0;

  work->graph_data_move_wait_count = SGRAPH_DATA_MOVE_WAIT;
  
  work->full_monsno             = 0;
  work->full_formno             = 0;
  work->graph_data_full_size    = 0;
  work->graph_data_full_print   = work->graph_data_full_size;  // �����I����Ă���

  work->wave_full_millisec      = 0;

  {
    // ���꒼���������ď�����
    GFL_BMP_DATA* bmp_data = GFL_BMPWIN_GetBmp( work->graph_bmpwin );
   
    s16 bitmap_min_x  = 0;  // �܂�
    s16 bitmap_max_x  = bitmap_min_x + SGRAPH_BITMAP_DISP_PIXEL_SIZE_X -1;  // �܂�
    s16 bitmap_size_x = bitmap_max_x - bitmap_min_x +1;

    GFL_BMP_Clear( bmp_data, 0 );  // �N���A���Ă���
    GFL_BMP_Fill( bmp_data, bitmap_min_x, SGRAPH_DATA_Y_TO_BITMAP_Y(0), bitmap_size_x, 1, SGRAPH_COLOR );
    //GFL_BMPWIN_TransVramCharacter( work->graph_bmpwin );
  }

  Zukan_Detail_Voice_GraphMakeTransScreen_VBlank( param, work, cmn );
}

#else  // #ifdef USE_SGRAPH

#ifdef GRAPH_GATHER

static void Zukan_Detail_Voice_GraphReset( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  work->graph_data_move_wait_count = GRAPH_DATA_MOVE_WAIT;
  
  work->full_monsno             = 0;
  work->full_formno             = 0;
  work->graph_data_full_size    = 0;
  work->graph_data_full_print   = work->graph_data_full_size;  // �����I����Ă���
  work->wave_full_millisec      = 0;

#ifdef GRAPH_SWAP_BMPWIN
  {
    u8 i;

    s16 bitmap_min_x  = GRAPH_DATA_X_TO_BITMAP_X( GRAPH_DATA_MIN_X );  // �܂�
    s16 bitmap_max_x  = GRAPH_DATA_X_TO_BITMAP_X( GRAPH_DATA_MAX_X );  // �܂�
    s16 bitmap_size_x = bitmap_max_x - bitmap_min_x +1;

    for( i=0; i<GRAPH_SWAP_BMPWIN_IDX_MAX; i++ )
    {
      // ���꒼���������ď�����
      GFL_BMP_DATA* bmp_data = GFL_BMPWIN_GetBmp( work->graph_bmpwin[i] );
      GFL_BMP_Clear( bmp_data, 0 );  // �N���A���Ă���
      GFL_BMP_Fill( bmp_data, bitmap_min_x, GRAPH_DATA_Y_TO_BITMAP_Y(0), bitmap_size_x, 1, GRAPH_COLOR );
      GFL_BMPWIN_TransVramCharacter( work->graph_bmpwin[i] );
    }
  }
#else
  {
    // ���꒼���������ď�����
    GFL_BMP_DATA* bmp_data = GFL_BMPWIN_GetBmp( work->graph_bmpwin );
   
    s16 bitmap_min_x  = GRAPH_DATA_X_TO_BITMAP_X( GRAPH_DATA_MIN_X );  // �܂�
    s16 bitmap_max_x  = GRAPH_DATA_X_TO_BITMAP_X( GRAPH_DATA_MAX_X );  // �܂�
    s16 bitmap_size_x = bitmap_max_x - bitmap_min_x +1;

    GFL_BMP_Clear( bmp_data, 0 );  // �N���A���Ă���
    
    GFL_BMP_Fill( bmp_data, bitmap_min_x, GRAPH_DATA_Y_TO_BITMAP_Y(0), bitmap_size_x, 1, GRAPH_COLOR );
  
    GFL_BMPWIN_TransVramCharacter( work->graph_bmpwin );
  }
#endif
}

#else

static void Zukan_Detail_Voice_GraphReset( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  s16 i;
  GFL_STD_MemClear( &work->graph_data_y, sizeof(work->graph_data_y) );
  for( i=0; i<GRAPH_DATA_SIZE_X; i++ )
  {
    work->graph_data_y[i] = 0;
  }
  work->graph_data_prev_x = GRAPH_DATA_MAX_X +1;
  work->graph_data_move_wait_count = GRAPH_DATA_MOVE_WAIT;
  Zukan_Detail_Voice_GraphDrawBitmap( param, work, cmn );
}

#endif

#endif  // #ifdef USE_SGRAPH

#ifdef USE_SGRAPH

static void Zukan_Detail_Voice_GraphDrawBitmap( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �������Ă���O���t�����̂܂�SGRAPH_DATA_MOVE_SPEED�������炷
  // ���炵�Ăł����󂫂�work->graph_full_bmp_data������
  // work->graph_data_full_print���������������i�߂�
  // �c�Ƃ������Ƃ��������Ƃ��ɂ��̊֐����Ăяo�����ƁB

  GFL_BMP_DATA* bmp_data = GFL_BMPWIN_GetBmp( work->graph_bmpwin );

  s16 draw_start_pixel;  // ����̋󂫂̊J�n���s�N�Z���ʒu(�g�[�^��(0<= <SGRAPH_BITMAP_CHAR_SIZE_X*8)�Ō����s�N�Z���ʒu)
  s16 draw_size;         // ����̋󂫂ɏ����I��������s�N�Z����

  // ���炷�O�ɁA����̋󂫂��m�F���Ă���
  {
    s16 pixel_offset = work->graph_x_chara_offset *8 + work->graph_x_scroll;
    draw_start_pixel = ( pixel_offset + SGRAPH_BITMAP_DISP_PIXEL_SIZE_X ) % SGRAPH_BITMAP_PIXEL_SIZE_X;
    draw_size = 0;
  }

  // �������Ă���O���t�����̂܂�SGRAPH_DATA_MOVE_SPEED�������炷
  work->graph_x_scroll += SGRAPH_DATA_MOVE_SPEED;
  while( work->graph_x_scroll >= SGRAPH_DATA_MOVE_SPEED_SCROLL_LIMIT )
  {
    // ���炵�Č��ɉ�邱�ƂɂȂ����������N���A����
    GFL_BMP_Fill( bmp_data, work->graph_x_chara_offset *8, 0, 8, SGRAPH_BITMAP_CHAR_SIZE_Y *8, 0 );

    // ���炷
    work->graph_x_chara_offset = ( work->graph_x_chara_offset +1 ) % SGRAPH_BITMAP_CHAR_SIZE_X;
    work->graph_x_scroll -= SGRAPH_DATA_MOVE_SPEED_SCROLL_LIMIT;
  }

  // ���炵�Ăł����O���̂͂ݏo���������N���A����
  if( work->graph_x_scroll > 0 )
  {
    GFL_BMP_Fill( bmp_data, work->graph_x_chara_offset *8, 0, work->graph_x_scroll, SGRAPH_BITMAP_CHAR_SIZE_Y *8, 0 );
  }

  // work->graph_full_bmp_data��������Ƃ�
  while( draw_size < SGRAPH_DATA_MOVE_SPEED )
  {
    s16 print_size_max = work->graph_data_full_size - work->graph_data_full_print;
    s16 owari = draw_start_pixel + SGRAPH_DATA_MOVE_SPEED - draw_size;  // draw_start_pixel<= <owari
    if( print_size_max <= 0 )
    {
      break;
    }
    if( owari > SGRAPH_BITMAP_PIXEL_SIZE_X )
    {
      owari = SGRAPH_BITMAP_PIXEL_SIZE_X;
    }
    if( owari - draw_start_pixel > print_size_max )
    {
      owari = draw_start_pixel + print_size_max;
    }
    GFL_BMP_Print(
        work->graph_full_bmp_data,
        bmp_data,
        work->graph_data_full_print, 0,
        draw_start_pixel, 0,
        owari - draw_start_pixel, SGRAPH_BITMAP_CHAR_SIZE_Y*8,  // y�����͑S�������Ă����B�������Ⴂ���Ȃ��Ƃ���͓����ɂ��Ă���̂ŁB
        GF_BMPPRT_NOTNUKI );
    draw_size += owari - draw_start_pixel;
    owari = owari % SGRAPH_BITMAP_PIXEL_SIZE_X;
    draw_start_pixel = owari;
    work->graph_data_full_print += draw_size;
  }

  // work->graph_full_bmp_data�������Ȃ��Ƃ��A�������́A������������Ȃ��Ƃ�
  while( draw_size < SGRAPH_DATA_MOVE_SPEED )
  {
    s16 owari = draw_start_pixel + SGRAPH_DATA_MOVE_SPEED - draw_size;  // draw_start_pixel<= <owari
    if( owari > SGRAPH_BITMAP_PIXEL_SIZE_X )
    {
      owari = SGRAPH_BITMAP_PIXEL_SIZE_X;
    }
    GFL_BMP_Fill( bmp_data, draw_start_pixel, SGRAPH_DATA_Y_TO_BITMAP_Y(0), owari - draw_start_pixel, 1, SGRAPH_COLOR );
    draw_size += owari - draw_start_pixel;
    owari = owari % SGRAPH_BITMAP_PIXEL_SIZE_X;
    draw_start_pixel = owari;
  }

  //GFL_BMPWIN_TransVramCharacter( work->graph_bmpwin );

  Zukan_Detail_Voice_GraphMakeTransScreen_VBlank( param, work, cmn );
}

#else  // #ifdef USE_SGRAPH

#ifdef GRAPH_GATHER

static void Zukan_Detail_Voice_GraphDrawBitmap( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �������Ă���O���t�����̂܂�GRAPH_DATA_MOVE_SPEED�������炷
  // ���炵�Ăł����󂫂�work->graph_full_bmp_data������
  // work->graph_data_full_print���������������i�߂�
  // �c�Ƃ������Ƃ��������Ƃ��ɂ��̊֐����Ăяo�����ƁB

#ifdef GRAPH_SWAP_BMPWIN
  GRAPH_SWAP_BMPWIN_IDX   graph_swap_bmpwin_idx_prev = ( work->graph_swap_bmpwin_idx == GRAPH_SWAP_BMPWIN_IDX_0 )?( GRAPH_SWAP_BMPWIN_IDX_1 ):( GRAPH_SWAP_BMPWIN_IDX_0 );
  GFL_BMP_DATA* bmp_data = GFL_BMPWIN_GetBmp( work->graph_bmpwin[work->graph_swap_bmpwin_idx] );
#else
  GFL_BMP_DATA* bmp_data = GFL_BMPWIN_GetBmp( work->graph_bmpwin );
#endif

  s16 bitmap_min_x  = GRAPH_DATA_X_TO_BITMAP_X( GRAPH_DATA_MIN_X );  // �܂�
  s16 bitmap_max_x  = GRAPH_DATA_X_TO_BITMAP_X( GRAPH_DATA_MAX_X );  // �܂�
  s16 bitmap_size_x = bitmap_max_x - bitmap_min_x +1;

  s16 draw_start = bitmap_max_x - GRAPH_DATA_MOVE_SPEED +1;  // �󂫂̊J�n�ʒu
  s16 draw_size = 0;                                         // �󂫂ɏ��������s�N�Z����

#ifdef GRAPH_SWAP_BMPWIN
  // �������Ă���O���t�����̂܂�GRAPH_DATA_MOVE_SPEED�������炷
  {
    GFL_BMP_DATA* bmp_data_prev = GFL_BMPWIN_GetBmp( work->graph_bmpwin[graph_swap_bmpwin_idx_prev] );
    GFL_BMP_Print( 
        bmp_data_prev, bmp_data,
        bitmap_min_x + GRAPH_DATA_MOVE_SPEED, 0,
        bitmap_min_x, 0,
        bitmap_size_x - GRAPH_DATA_MOVE_SPEED, GRAPH_BITMAP_CHAR_SIZE_Y*8,  // y�����͑S�������Ă����B�������Ⴂ���Ȃ��Ƃ���͓����ɂ��Ă���̂ŁB
        GF_BMPPRT_NOTNUKI );
  }
#else
  // �������Ă���O���t�����̂܂�GRAPH_DATA_MOVE_SPEED�������炷
  GFL_BMP_Print( 
      bmp_data, bmp_data,
      bitmap_min_x + GRAPH_DATA_MOVE_SPEED, 0,
      bitmap_min_x, 0,
      bitmap_size_x - GRAPH_DATA_MOVE_SPEED, GRAPH_BITMAP_CHAR_SIZE_Y*8,  // y�����͑S�������Ă����B�������Ⴂ���Ȃ��Ƃ���͓����ɂ��Ă���̂ŁB
      GF_BMPPRT_NOTNUKI );
#endif

  // ���炵�Ăł����󂫂��N���A����
  GFL_BMP_Fill( bmp_data, draw_start, 0, GRAPH_DATA_MOVE_SPEED, GRAPH_BITMAP_CHAR_SIZE_Y*8, 0 );

  // work->graph_full_bmp_data��������Ƃ�
  if( work->graph_data_full_print < work->graph_data_full_size )
  {
    draw_size = work->graph_data_full_size - work->graph_data_full_print;
    if( draw_size > GRAPH_DATA_MOVE_SPEED ) draw_size = GRAPH_DATA_MOVE_SPEED;
    
    GFL_BMP_Print(
        work->graph_full_bmp_data, bmp_data,
        work->graph_data_full_print, 0,
        draw_start, 0,
        draw_size, GRAPH_BITMAP_CHAR_SIZE_Y*8,  // y�����͑S�������Ă����B�������Ⴂ���Ȃ��Ƃ���͓����ɂ��Ă���̂ŁB
        GF_BMPPRT_NOTNUKI );

    work->graph_data_full_print += draw_size;
  }

  // work->graph_full_bmp_data�������Ȃ��Ƃ��A�������́A������������Ȃ��Ƃ�
  if( draw_size < GRAPH_DATA_MOVE_SPEED )
  {
    GFL_BMP_Fill( bmp_data, draw_start + draw_size, GRAPH_DATA_Y_TO_BITMAP_Y(0), GRAPH_DATA_MOVE_SPEED - draw_size, 1, GRAPH_COLOR );
  }
  
#ifdef GRAPH_SWAP_BMPWIN
  GFL_BMPWIN_MakeTransWindow_VBlank( work->graph_bmpwin[work->graph_swap_bmpwin_idx] );
  work->graph_swap_bmpwin_idx = graph_swap_bmpwin_idx_prev;
#else
  GFL_BMPWIN_TransVramCharacter( work->graph_bmpwin );
#endif
}

#else

static void Zukan_Detail_Voice_GraphDrawBitmap( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  s16 i;

  s16 x0;
  s16 x1;
  s16 y0;
  s16 y1;
  s16 x_width;
  s16 y_width;
  s16 x;
  s16 y;
  
  GFL_BMP_DATA* bmp_data = GFL_BMPWIN_GetBmp( work->graph_bmpwin );
  GFL_BMP_Clear( bmp_data, 0 );

  // GRAPH_DATA_INTERVAL_X���Ƃ̃L�[�t���[���ɂ����f�[�^�͓����Ă��Ȃ�
  i = work->graph_data_prev_x - GRAPH_DATA_INTERVAL_X;
  while( i >= 0 )  // �L�[�t���[����O�֑O�ւ����̂ڂ��Ă���
  {
    x0 = i;
    x1 = i + GRAPH_DATA_INTERVAL_X;  // �K��x0<x1�ƂȂ�
    y0 = work->graph_data_y[x0];
    y1 = work->graph_data_y[x1];
    x_width = MATH_ABS( x1 - x0 );
    y_width = MATH_ABS( y1 - y0 );
    
    // x0����x1�܂Ōq��
    if( x_width > y_width )
    {
      if( y0 < y1 )
      {
        for( x=x0; x<=x1; x++ )
        {
          y = y_width * ( x - x0 ) / x_width + y0;  // ����Z���ɂ���Ă��܂��ƁA�؂�̂Ă��Ă���|���邱�ƂɂȂ�̂ŁA�悭�Ȃ�
          if( GRAPH_DATA_MIN_X<=x && x<=GRAPH_DATA_MAX_X && GRAPH_DATA_MIN_Y<=y && y<=GRAPH_DATA_MAX_Y)
          {
            GFL_BMP_Fill( bmp_data, GRAPH_DATA_X_TO_BITMAP_X(x), GRAPH_DATA_Y_TO_BITMAP_Y(y), 1, 1, GRAPH_COLOR );
          }
        }
      }
      else
      {
        for( x=x0; x<=x1; x++ )
        {
          y = - y_width * ( x - x0 ) / x_width + y0;
          if( GRAPH_DATA_MIN_X<=x && x<=GRAPH_DATA_MAX_X && GRAPH_DATA_MIN_Y<=y && y<=GRAPH_DATA_MAX_Y)
          {
            GFL_BMP_Fill( bmp_data, GRAPH_DATA_X_TO_BITMAP_X(x), GRAPH_DATA_Y_TO_BITMAP_Y(y), 1, 1, GRAPH_COLOR );
          }
        }
      }
    }
    else
    {
      if( y0 < y1 )
      {
        for( y=y0; y<=y1; y++ )  // GRAPH_DATA_SATURATION_MIN_Y����GRAPH_DATA_SATURATION_MAX_Y�܂Ń��[�v�����\��������A���ʂ�������
        {
          x = x_width * ( y - y0 ) / y_width + x0;
          if( GRAPH_DATA_MIN_X<=x && x<=GRAPH_DATA_MAX_X && GRAPH_DATA_MIN_Y<=y && y<=GRAPH_DATA_MAX_Y)
          {
            GFL_BMP_Fill( bmp_data, GRAPH_DATA_X_TO_BITMAP_X(x), GRAPH_DATA_Y_TO_BITMAP_Y(y), 1, 1, GRAPH_COLOR );
          }
        }
      }
      else
      {
        for( y=y0; y>=y1; y-- )  // GRAPH_DATA_SATURATION_MIN_Y����GRAPH_DATA_SATURATION_MAX_Y�܂Ń��[�v�����\��������A���ʂ�������
        {
          x = - x_width * ( y - y1 ) / y_width + x1;
          if( GRAPH_DATA_MIN_X<=x && x<=GRAPH_DATA_MAX_X && GRAPH_DATA_MIN_Y<=y && y<=GRAPH_DATA_MAX_Y)
          {
            GFL_BMP_Fill( bmp_data, GRAPH_DATA_X_TO_BITMAP_X(x), GRAPH_DATA_Y_TO_BITMAP_Y(y), 1, 1, GRAPH_COLOR );
          }
        }
      }
    }

    i -= GRAPH_DATA_INTERVAL_X;
  }

  GFL_BMPWIN_TransVramCharacter( work->graph_bmpwin );
}

#endif

#endif  // #ifdef USE_SGRAPH

static void Zukan_Detail_Voice_WaveDataSetup( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn, u16 monsno, u16 formno )
{
#ifdef USE_SGRAPH

  work->wave_data_begin = PMV_GetWave( work->voice_idx );

  work->graph_data_full_print = 0;  // ���ꂩ�珑��

  // �|�P������(�t�H������)�ς���Ă���ꍇ�A�V���ɗp�ӂ���
  if( monsno != work->full_monsno || formno != work->full_formno )
  {
    work->full_monsno = monsno;
    work->full_formno = formno;

    work->wave_full_millisec = (u32)(                                                      \
          (f32)PMVOICE_GetWaveSize( work->voice_idx ) * (f32)SWAVE_DATA_SPEED_ONE          \
        / (f32)PMV_GetWaveRate( work->voice_idx ) / (f32)PMV_GetSpeed( work->voice_idx )   \
        * 1000.0f                                                                          \
        + 0.5f );  // �l�̌ܓ�
    // wave�f�[�^�̑�����[millisec] = �f�[�^�� / ���[�g(����1�b������̃f�[�^��) / ���x(�Ⴆ�Α��x2�{�Ȃ玞�Ԃ�1/2�{) * 1000

    GFL_BMP_Clear( work->graph_full_bmp_data, 0 );

    {
      u32 dot_idx = 0;
      u32 data_idx_start = 0;  // data_idx_start<=  <data_idx_end
      u32 data_idx_end;

      // 1�t���[���ŏ�����f�[�^�� = ���[�g(����1�b������̃f�[�^��) * ���x(�Ⴆ�Α��x2�{�Ȃ������̂�2�{) / FPS
      // 1�h�b�g�ړ�����̂Ɋ|����t���[���� = (SGRAPH_DATA_MOVE_WAIT+1) / SGRAPH_DATA_MOVE_SPEED
      // 1�h�b�g�ɏ������f�[�^�� = 1�t���[���ŏ�����f�[�^�� * 1�h�b�g�ړ�����̂Ɋ|����t���[����

      while( data_idx_start < PMVOICE_GetWaveSize( work->voice_idx ) )
      {
        data_idx_end =                                                                                                  \
            (f32)PMV_GetWaveRate( work->voice_idx ) * (f32)PMV_GetSpeed( work->voice_idx ) / (f32)SWAVE_DATA_SPEED_ONE  \
          / (f32)SWAVE_DATA_PLAY_FPS                                                                                    \
          * (f32)(SGRAPH_DATA_MOVE_WAIT+1) / (f32)SGRAPH_DATA_MOVE_SPEED                                                \
          * (f32)(dot_idx+1);
      
        if( data_idx_end > PMVOICE_GetWaveSize( work->voice_idx ) )  data_idx_end = PMVOICE_GetWaveSize( work->voice_idx );

        // �`��
        if( dot_idx < SGRAPH_DATA_FULL_SIZE_X_MAX )
        {
          u32  i;
          s16  min = SGRAPH_DATA_SATURATION_MAX_Y;
          s16  max = SGRAPH_DATA_SATURATION_MIN_Y;
          s16  value;
          for( i=data_idx_start; i<data_idx_end; i++ )
          {
            value =                                                                                                                       \
                  (s16)( ( (s32)(work->wave_data_begin[i]) * SWAVE_DATA_VALUE_MAX_TO_SGRAPH_DATA * PMVOICE_GetVolume( work->voice_idx ) ) \
                / SWAVE_DATA_VALUE_MAX / SWAVE_DATA_VALUE_MAX );
            // work->wave_data_begin[i]�́A�����ȉ����傫�ȉ����֌W�Ȃ��A�ő�ŏ����S�|�P�����ő�����ꂽ�f�[�^�������Ă���(�Ǝv����)�B
            // ���̃f�[�^���APMVOICE_GetVolume�ŏ�����������傫�������肵�čĐ����Ă���(�Ǝv����)�B
            // ������A�g�`�̑傫�������̑傫���ƍ��킹��ɂ́A
            // (���̑傫�����l�������g�`�̑傫��) = work->wave_data_begin[i] * PMVOICE_GetVolume / SWAVE_DATA_VALUE_MAX
            // �Ƃ��Ă��(SWAVE_DATA_VALUE_MAX��work->wave_data_begin[i]����蓾��ő�l)�B
            // ������X�ɁA�`��͈͂̏c�����Ɏ��܂�悤�ɂ������̂ŁA
            // (���̑傫�����l�������g�`�̑傫��) * SWAVE_DATA_VALUE_MAX_TO_SGRAPH_DATA / SWAVE_DATA_VALUE_MAX
            // �Ƃ��Ă��(SWAVE_DATA_VALUE_MAX_TO_SGRAPH_DATA�͕`��͈͂̏c�����̍ő�l)�B
            value = MATH_CLAMP( value, SGRAPH_DATA_SATURATION_MIN_Y, SGRAPH_DATA_SATURATION_MAX_Y );
            if( value < min ) min = value;
            if( value > max ) max = value;
          }
          if( min <= max )
          {
            if( min < SGRAPH_DATA_MIN_Y ) min = SGRAPH_DATA_MIN_Y;  // �c�̓h��ׂ����s�N�Z������max-min+1�ƂȂ�
            if( max > SGRAPH_DATA_MAX_Y ) max = SGRAPH_DATA_MAX_Y;
            GFL_BMP_Fill( work->graph_full_bmp_data, dot_idx, SGRAPH_DATA_Y_TO_BITMAP_Y(max), 1, max-min+1, SGRAPH_COLOR );
          }

          {
            // �����ŁAdot_idx>0�̂Ƃ��A[dot_idx-1]�͌��̃f�[�^1�ƌq���A[dot_idx]�͑O�̃f�[�^1�ƌq�����Ƃ�����
          }

        }
        else
        {
          GF_ASSERT_MSG( dot_idx < SGRAPH_DATA_FULL_SIZE_X_MAX, "ZUKAN_DETAIL_VOICE : wave�f�[�^���r�b�g�}�b�v�̉��͈͓��ɕ`�悵����܂���B\n" );
        }

        // ����
        dot_idx++;
        data_idx_start = data_idx_end +1;
      }

      if( dot_idx > SGRAPH_DATA_FULL_SIZE_X_MAX ) dot_idx = SGRAPH_DATA_FULL_SIZE_X_MAX;
      work->graph_data_full_size = dot_idx;

      // work->graph_full_bmp_data��work->graph_data_full_size����̃h�b�g�ɂ͉���������Ă��Ȃ�
    }
  }

#else  // #ifdef USE_SGRAPH

  work->wave_data_begin = PMV_GetWave( work->voice_idx );
  work->wave_data_curr_pos = 0;
  work->wave_data_interval = WAVE_DATA_INTERVAL( PMV_GetWaveRate( work->voice_idx ) / 60 );

#ifdef GRAPH_GATHER
  work->graph_data_full_print = 0;  // ���ꂩ�珑��

  // �|�P������(�t�H������)�ς���Ă���ꍇ�A�V���ɗp�ӂ���
  if( monsno != work->full_monsno || formno != work->full_formno )
  {
    work->full_monsno = monsno;
    work->full_formno = formno;

    work->wave_full_millisec = (u32)(                                                      \
          (f32)PMVOICE_GetWaveSize( work->voice_idx ) * (f32)WAVE_DATA_SPEED_ONE           \
        / (f32)PMV_GetWaveRate( work->voice_idx ) / (f32)PMV_GetSpeed( work->voice_idx )   \
        * 1000.0f                                                                          \
        + 0.5f );  // �l�̌ܓ�
    // wave�f�[�^�̑�����[millisec] = �f�[�^�� / ���[�g(����1�b������̃f�[�^��) / ���x(�Ⴆ�Α��x2�{�Ȃ玞�Ԃ�1/2�{) * 1000

    GFL_BMP_Clear( work->graph_full_bmp_data, 0 );

    {
      u32 dot_idx = 0;
      u32 data_idx_start = 0;  // data_idx_start<=  <data_idx_end
      u32 data_idx_end;

      // 1�t���[���ŏ�����f�[�^�� = ���[�g(����1�b������̃f�[�^��) * ���x(�Ⴆ�Α��x2�{�Ȃ������̂�2�{) / FPS
      // 1�h�b�g�ړ�����̂Ɋ|����t���[���� = (GRAPH_DATA_MOVE_WAIT+1) / GRAPH_DATA_MOVE_SPEED
      // 1�h�b�g�ɏ������f�[�^�� = 1�t���[���ŏ�����f�[�^�� * 1�h�b�g�ړ�����̂Ɋ|����t���[����

      // GRAPH_DATA_INTERVAL_X�͎g�p���Ȃ��B�������������ď����̂ŁB

      while( data_idx_start < PMVOICE_GetWaveSize( work->voice_idx ) )
      {
        data_idx_end =                                                                                                  \
            (f32)PMV_GetWaveRate( work->voice_idx ) * (f32)PMV_GetSpeed( work->voice_idx ) / (f32)WAVE_DATA_SPEED_ONE   \
          / (f32)WAVE_DATA_PLAY_FPS                                                                                     \
          * (f32)(GRAPH_DATA_MOVE_WAIT+1) / (f32)GRAPH_DATA_MOVE_SPEED                                                  \
          * (f32)(dot_idx+1);
      
        if( data_idx_end > PMVOICE_GetWaveSize( work->voice_idx ) )  data_idx_end = PMVOICE_GetWaveSize( work->voice_idx );

        // �`��
        if( dot_idx < GRAPH_DATA_FULL_SIZE_X_MAX )
        {
          u32  i;
          s16  min = GRAPH_DATA_SATURATION_MAX_Y;
          s16  max = GRAPH_DATA_SATURATION_MIN_Y;
          s16  value;
          for( i=data_idx_start; i<data_idx_end; i++ )
          {
            value =                                                                                                                     \
                  (s16)( ( (s32)(work->wave_data_begin[i]) * WAVE_DATA_VALUE_MAX_TO_GRAPH_DATA * PMVOICE_GetVolume( work->voice_idx ) ) \
                / WAVE_DATA_VALUE_MAX / WAVE_DATA_VALUE_MAX );
            // work->wave_data_begin[i]�́A�����ȉ����傫�ȉ����֌W�Ȃ��A�ő�ŏ����S�|�P�����ő�����ꂽ�f�[�^�������Ă���(�Ǝv����)�B
            // ���̃f�[�^���APMVOICE_GetVolume�ŏ�����������傫�������肵�čĐ����Ă���(�Ǝv����)�B
            // ������A�g�`�̑傫�������̑傫���ƍ��킹��ɂ́A
            // (���̑傫�����l�������g�`�̑傫��) = work->wave_data_begin[i] * PMVOICE_GetVolume / WAVE_DATA_VALUE_MAX
            // �Ƃ��Ă��(WAVE_DATA_VALUE_MAX��work->wave_data_begin[i]����蓾��ő�l)�B
            // ������X�ɁA�`��͈͂̏c�����Ɏ��܂�悤�ɂ������̂ŁA
            // (���̑傫�����l�������g�`�̑傫��) * WAVE_DATA_VALUE_MAX_TO_GRAPH_DATA / WAVE_DATA_VALUE_MAX
            // �Ƃ��Ă��(WAVE_DATA_VALUE_MAX_TO_GRAPH_DATA�͕`��͈͂̏c�����̍ő�l)�B
            value = MATH_CLAMP( value, GRAPH_DATA_SATURATION_MIN_Y, GRAPH_DATA_SATURATION_MAX_Y );
            if( value < min ) min = value;
            if( value > max ) max = value;
          }
          if( min <= max )
          {
            if( min < GRAPH_DATA_MIN_Y ) min = GRAPH_DATA_MIN_Y;  // �c�̓h��ׂ����s�N�Z������max-min+1�ƂȂ�
            if( max > GRAPH_DATA_MAX_Y ) max = GRAPH_DATA_MAX_Y;
            GFL_BMP_Fill( work->graph_full_bmp_data, dot_idx, GRAPH_DATA_Y_TO_BITMAP_Y(max), 1, max-min+1, GRAPH_COLOR );
          }

          {
            // �����ŁAdot_idx>0�̂Ƃ��A[dot_idx-1]�͌��̃f�[�^1�ƌq���A[dot_idx]�͑O�̃f�[�^1�ƌq�����Ƃ�����
          }

        }
        else
        {
          GF_ASSERT_MSG( dot_idx < GRAPH_DATA_FULL_SIZE_X_MAX, "ZUKAN_DETAIL_VOICE : wave�f�[�^���r�b�g�}�b�v�̉��͈͓��ɕ`�悵����܂���B\n" );
        }

        // ����
        dot_idx++;
        data_idx_start = data_idx_end +1;
      }

      if( dot_idx > GRAPH_DATA_FULL_SIZE_X_MAX ) dot_idx = GRAPH_DATA_FULL_SIZE_X_MAX;
      work->graph_data_full_size = dot_idx;

      // work->graph_full_bmp_data��work->graph_data_full_size����̃h�b�g�ɂ͉���������Ă��Ȃ�
    }
  }
#endif

#endif  // #ifdef USE_SGRAPH
}

#ifdef USE_SGRAPH
static void Zukan_Detail_Voice_GraphMakeTransScreen_VBlank( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u16 chr_num = GFL_BMPWIN_GetChrNum( work->graph_bmpwin );
  u16 h = 0;
  u16 i, j;

  for( j=0; j<SGRAPH_BITMAP_CHAR_SIZE_Y; j++ )
  {
    for( i=0; i<SGRAPH_BITMAP_CHAR_SIZE_X; i++ )
    {
      u16 chara_name = chr_num + ( SGRAPH_BITMAP_CHAR_SIZE_X * j ) + ( ( i + work->graph_x_chara_offset ) % SGRAPH_BITMAP_CHAR_SIZE_X );
      u16 flip_h     = 0;
      u16 flip_v     = 0;
      u16 pal        = BG_PAL_POS_M_TIME;
      work->graph_screen[h] = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
      h++;
    }
  }

  GFL_BG_WriteScreen( BG_FRAME_M_TIME, work->graph_screen, 
                      SGRAPH_BITMAP_SCREEN_POS_X, SGRAPH_BITMAP_SCREEN_POS_Y,
                      SGRAPH_BITMAP_CHAR_SIZE_X, SGRAPH_BITMAP_CHAR_SIZE_Y );

  //GFL_BG_SetScrollReq( BG_FRAME_M_TIME, GFL_BG_SCROLL_X_SET, SGRAPH_BITMAP_SCREEN_SCROLL_BASE_X + work->graph_x_scroll );
  //GFL_BG_SetScrollReq( BG_FRAME_M_TIME, GFL_BG_SCROLL_Y_SET, SGRAPH_BITMAP_SCREEN_SCROLL_BASE_Y );

  //GFL_BG_LoadScreenV_Req( BG_FRAME_M_TIME );

  work->graph_req = TRUE;
}
#endif  // #ifdef USE_SGRAPH


//-------------------------------------
/// �A���t�@�ݒ�
//=====================================
static void Zukan_Detail_Voice_AlphaInit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �������ɂ��Ȃ�OBJ�̐ݒ��ύX����
  {
    u8 i;

    for( i=0; i<OBJ_SWAP_MAX; i++ )
    {
      if( work->poke_clwk[i] )
      {
        GFL_CLACT_WK_SetObjMode( work->poke_clwk[i], GX_OAM_MODE_NORMAL );  // �A���t�@�A�j���[�V�����̉e�����󂯂Ȃ��悤�ɂ���
      }
    }

    for( i=OBJ_CELL_TIME_START; i<OBJ_CELL_TIME_END; i++ )
    {
      GFL_CLACT_WK_SetObjMode( work->obj_cell[i], GX_OAM_MODE_NORMAL );  // �A���t�@�A�j���[�V�����̉e�����󂯂Ȃ��悤�ɂ���
    }
  }

  {
    int ev1 = 16;
    int ev2 = 8;
    G2_SetBlendAlpha(
        GX_BLEND_PLANEMASK_BG2,
        GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
        ev1,
        ev2 );  // ev1+ev2=16�ɂȂ�Ȃ����ǁA�f�U�C�i�[����w��̒l�Ȃ̂ŁB
  }
  {
    int ev1 = 12;
    G2S_SetBlendAlpha(
        GX_BLEND_PLANEMASK_NONE,
        GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
        ev1,
        16 - ev1 );
  }
}
static void Zukan_Detail_Voice_AlphaExit( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �������ɂ��Ȃ�OBJ�̐ݒ�����ɖ߂�
  {
    u8 i;

    for( i=0; i<OBJ_SWAP_MAX; i++ )
    {
      if( work->poke_clwk[i] )
      {
        GFL_CLACT_WK_SetObjMode( work->poke_clwk[i], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
      }
    }

    for( i=OBJ_CELL_TIME_START; i<OBJ_CELL_TIME_END; i++ )
    {
      GFL_CLACT_WK_SetObjMode( work->obj_cell[i], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    }
  }

  // �ꕔ���t�F�[�h�̐ݒ�����ɖ߂�
  ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_m );
  // �ꕔ���t�F�[�h�̐ݒ�����ɖ߂�
  ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_s );
}


/*
//-------------------------------------
/// BG�p�l��
//=====================================
static void Zukan_Detail_Voice_CreatePanelM( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  HEAPID heap_id     = param->heap_id;
  u8 bg_frame        = BG_FRAME_M_PANEL;
  u8 pal_num         = BG_PAL_NUM_M_PANEL;
  u8 pal_pos         = BG_PAL_POS_M_PANEL;
  u8 pal_ofs         = 0;
  ARCDATID data_ncl  = NARC_zukan_gra_info_info_bgd_NCLR;
  ARCDATID data_ncg  = NARC_zukan_gra_info_info_bgd_NCGR;
  ARCDATID data_nsc  = NARC_zukan_gra_info_voicewin_bgd_NSCR;


  GFL_ARCUTIL_TRANSINFO  tinfo;
  ARCHANDLE* handle;

  // BG�t���[�����烁�C�����T�u�����肷��
  PALTYPE   paltype;
  
  if( bg_frame < GFL_BG_FRAME0_S )
  {
    paltype = PALTYPE_MAIN_BG;
  }
  else
  {
    paltype = PALTYPE_SUB_BG;
  }

  // �ǂݍ���œ]��
  handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, heap_id );

  GFL_ARCHDL_UTIL_TransVramPaletteEx(
                               handle,
                               data_ncl,
                               paltype,
                               pal_ofs * 0x20,
                               pal_pos * 0x20,
                               pal_num * 0x20,
                               heap_id );

  tinfo = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
                            handle,
                            data_ncg,
                            bg_frame,
                            0,
                            FALSE,
                            heap_id );
  GF_ASSERT_MSG( tinfo != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_DETAIL_ : BG�L�����̈悪����܂���ł����B\n" );

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(
        handle,
        data_nsc,
        bg_frame,
        0,
        GFL_ARCUTIL_TRANSINFO_GetPos( tinfo ),
        0,
        FALSE,
        heap_id );
  GFL_BG_ChangeScreenPalette( bg_frame, 0, 0, 32, 24, pal_pos );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenV_Req( bg_frame );


  work->panel_m_tinfo = tinfo;
}
static void Zukan_Detail_Voice_DeletePanelM( ZUKAN_DETAIL_VOICE_PARAM* param, ZUKAN_DETAIL_VOICE_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  GFL_BG_FreeCharacterArea( BG_FRAME_M_PANEL,
                            GFL_ARCUTIL_TRANSINFO_GetPos( work->panel_m_tinfo ),
                            GFL_ARCUTIL_TRANSINFO_GetSize( work->panel_m_tinfo ) );
}
*/
