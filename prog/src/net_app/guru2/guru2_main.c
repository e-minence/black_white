//******************************************************************************
/**
 * 
 * @file    guru2_main.c
 * @brief   ���邮������@�Q�[�����C��
 * @author  Akito Mori(�ڐA�j / kagaya 
 * @data    2010.01.20
 *
 */
//******************************************************************************
#include <gflib.h>
#include <calctool.h>

#define WINDOW_SAVE_ICON  

#include "system/main.h"
#include "arc/arc_def.h"
#include "sound/pm_sndsys.h"
#include "system/wipe.h"
#include "system/palanm.h"
#include "system/bmp_winframe.h"
#include "savedata/mystatus.h"
#include "savedata/etc_save.h"
#include "net/net_save.h"     // NET_SAVE_Init,
#include "system/net_err.h"
#include "system/time_icon.h"

#include "print/printsys.h"
#include "print/wordset.h"
#include "item/itemsym.h"

#include "system/bmp_menulist.h"

#include "arc/message.naix"
#include "msg/msg_guru2.h"

#include "poke_tool/pokeparty.h"
#include "savedata/myitem_savedata.h"
#include "poke_tool/poke_memo.h"

//#include "..\..\demo\egg\data\egg_data_def.h" //���A�[�J�C�u�C���f�b�N�X

#include "guru2_local.h"
#include "guru2_snd.h"
#include "guru2_2d.naix" //��p2D���\�[�X�A�[�J�C�u�ǉ�
#include "winframe.naix"
#include "font/font.naix"

//==============================================================================
//  define
//==============================================================================
//--------------------------------------------------------------
//  debug
//--------------------------------------------------------------
//----PM_DEBUG
#ifdef PM_DEBUG

//#define GURU2_DEBUG_ON
//----GURU2_DEBUG_ON
#ifdef GURU2_DEBUG_ON
#define DEBUG_DISC_NO (3)
//#define DEBUG_DISP_CHECK_VANISH
//EGG_3D_USE
#endif
//----GURU2_DEBUG_ON END

//----DEBUG_ONLY_FOR_kagaya
#ifdef DEBUG_ONLY_FOR_kagaya
#define DEBUG_GURU2_PRINTF      //��`��OS_Printf()�L��
#endif
//----DEBUG_ONLY_FOR_kagaya END

#define DEBUG_GURU2_PRINTF_FORCE  //���̊��ł��o��Printf()
#define DEBUG_GURU2_PRINTF_COMM // �ʐM�󋵂��v�����g

#endif
//----PM_DEBUG END

//--------------------------------------------------------------
//  �Q�[���V���{��
//--------------------------------------------------------------
#define FRAME_SEC (30)  ///<�b


#define GURU2_GAME_FRAME   (FRAME_SEC*20)
#define GURU2_GAME_FRAME_H (GURU2_GAME_FRAME/2)

#define COMM_WAIT_ERROR_FRAME (FRAME_SEC*30)

#define G2M_TEMPWORK_SIZE (32)

#define GURU2_MSG_WAIT_FRAME (FRAME_SEC*6)
#define GURU2_RESULT_NAME_WAIT_FRAME (FRAME_SEC*4)
#define GURU2_RESULT_MSG_WAIT_FRAME (FRAME_SEC*4)

#define GURU2_KINOMI_MAX (1)  //�؂̎��ő�

///a�t���[����b�ɓ��B���鑬�x��c�t���[���ŒB����ׂ̉����x������o��
#define ACCEL_ARRIVAL_FX(a,b,c) (((NUM_FX32(b)/a)/c)
///a�t���[����b�ɓ��B���鑬�x���o��
#define ACCEL_FRAME_FX(a,b) ((NUM_FX32(b)/a))

//--------------------------------------------------------------
/// VRAM�}�l�[�W��
//--------------------------------------------------------------
#define GURU2_VRAMTRANSFER_MANAGER_NUM (8)  //<VRAM�]���}�l�[�W���ő吔

#define CHAR_MANAGER_MAX (32)
#define PLTT_MANAGER_MAX (16) 
#define CHAR_VRAMTRANS_M_SIZE (0x4000)  //512
#define CHAR_VRAMTRANS_S_SIZE (0x4000)  //512

//--------------------------------------------------------------
/// 3D�J���[
//--------------------------------------------------------------
#define EDGE_COLOR_MAX (8)  ///<3D�@�G�b�W�J���[
#define EDGE_COLOR_R (4)  ///<3D�@�G�b�W�J���[R
#define EDGE_COLOR_G (4)
#define EDGE_COLOR_B (4)

#define CLEAR_COLOR_R (31)  ///<3D�@�N���A�J���[R
#define CLEAR_COLOR_G (31)
#define CLEAR_COLOR_B (31)

#define MSG_WIN_SX    ( 27 )
#define MSG_WIN_SY    ( 4 )



//--------------------------------------------------------------
/// BG
//--------------------------------------------------------------
#define BGF_M_3D    (GFL_BG_FRAME0_M)
#define BGF_M_KAIWA   (GFL_BG_FRAME1_M)
#define BGF_M_BG    (GFL_BG_FRAME3_M)
#define BGF_S_BG    (GFL_BG_FRAME3_S)

#define BGF_PLTT_M_SIZE (32*2)  ///<BG�p���b�g
#define BGF_PLTT_M_OFFS (32*0)
#define BGF_PLTT_S_SIZE (32*2)
#define BGF_PLTT_S_OFFS (32*0)

#define BGF_BG3_M_CHAR_OFFS (32*0)  ///<BG�L�����I�t�Z�b�g
#define BGF_BG2_S_CHAR_OFFS (32*0)
#define BGF_BG3_S_CHAR_OFFS (32*0)

#define BGF_PANO_MENU_WIN (15)  ///< �E�B���h�E�p���b�g
#define BGF_PANO_TALK_WIN (14)  ///< ��b�E�B���h�E�p���b�g

#define BGF_CHSIZE_NAME (4*10)    ///<���O���̓I�t�Z�b�g

///<���j���[�E�B���h�E�L�����J�n�ʒu
#define BGF_CHARNO_MENU (1) 
///<��b�E�B���h�E�L�����J�n�ʒu
#define BGF_CHARNO_TALK (BGF_CHARNO_MENU+MENU_WIN_CGX_SIZ)
///<�r�b�g�}�b�v�L�����x�[�X
#define BGF_BMPCHAR_BASE (BGF_CHARNO_TALK+TALK_WIN_CGX_SIZ)

///��b�E�B���h�E�r�b�g�}�b�v
#define BGF_BMPCHAR_TALK (BGF_BMPCHAR_BASE)
///��b�E�B���h�E�r�b�g�}�b�v�T�C�Y
#define BGF_BMPCHAR_SIZE (MSG_WIN_SX*MSG_WIN_SY)

///�l�[���E�B���h�E�r�b�g�}�b�v
#define BGF_BMPCHAR_NAME_BASE (BGF_BMPCHAR_TALK+BGF_BMPCHAR_SIZE)

///�l�[���E�B���h�E�Z�o
#define BGF_BMPCHAR_NAME(a) (BGF_BMPCHAR_NAME_BASE+(BGF_CHSIZE_NAME*(a)))

#define DOTCHAR(a)  (((a)/8)*8) ///<�h�b�g�T�C�Y->�L�����T�C�Y

//--------------------------------------------------------------
/// BMP
//--------------------------------------------------------------
#define BMPSIZE_NAME_X (8)
#define BMPSIZE_NAME_Y (2)

#define CCX (16)
#define CCY (12)

#define BMPPOS_NAME_X_2_0 (CCX-4)
#define BMPPOS_NAME_Y_2_0 (CCY+6)
#define BMPPOS_NAME_X_2_1 (CCX-4)
#define BMPPOS_NAME_Y_2_1 (CCY-6)

#define BMPPOS_NAME_X_3_0 (CCX-4)
#define BMPPOS_NAME_Y_3_0 (CCY+6)
#define BMPPOS_NAME_X_3_1 (CCX-11)
#define BMPPOS_NAME_Y_3_1 (CCY-2)
#define BMPPOS_NAME_X_3_2 (CCX+3)
#define BMPPOS_NAME_Y_3_2 (CCY-2)

#define BMPPOS_NAME_X_4_0 (CCX-4) //ok
#define BMPPOS_NAME_Y_4_0 (CCY+6)
#define BMPPOS_NAME_X_4_1 (CCX-12)  //ok
#define BMPPOS_NAME_Y_4_1 (CCY+1)
#define BMPPOS_NAME_X_4_2 (CCX-4) //ok
#define BMPPOS_NAME_Y_4_2 (CCY-6)
#define BMPPOS_NAME_X_4_3 (CCX+4) //ok
#define BMPPOS_NAME_Y_4_3 (CCY+1)

#define BMPPOS_NAME_X_5_0 (CCX-4)
#define BMPPOS_NAME_Y_5_0 (CCY+7)

#define BMPPOS_NAME_X_5_1 (CCX-12)
#define BMPPOS_NAME_Y_5_1 (CCY+2)

#define BMPPOS_NAME_X_5_2 (CCX-10)
#define BMPPOS_NAME_Y_5_2 (CCY-3)

#define BMPPOS_NAME_X_5_3 (CCX+2)
#define BMPPOS_NAME_Y_5_3 (CCY-3)

#define BMPPOS_NAME_X_5_4 (CCX+4)
#define BMPPOS_NAME_Y_5_4 (CCY+2)

#define NAME_COL      ( PRINTSYS_LSB_Make( 1, 2,0) )
#define NAME_COL_MINE ( PRINTSYS_LSB_Make( 3, 4,0) )

// BMP���N���A�J���[
#define BMP_COL_WHITE   ( 15 )
#define BMP_COL_NULL    (  0 )

//--------------------------------------------------------------
/// �Z��ID
//--------------------------------------------------------------
enum
{
  EDID_EGG_NCGR = 0,
  EDID_EGG_NCLR,
  EDID_EGG_NCER,
  EDID_EGG_NANR,
  EDID_CDOWN_NCGR,
  EDID_CDOWN_NCLR,
  EDID_CDOWN_NCER,
  EDID_CDOWN_NANR,
};

//--------------------------------------------------------------
/// ��b�E�B���h�E
//--------------------------------------------------------------
enum              ///<�l�[���E�B���h�E
{
  GURU2NAME_WIN_NAME_0,
  GURU2NAME_WIN_NAME_1,
  GURU2NAME_WIN_NAME_2,
  GURU2NAME_WIN_NAME_3,
  GURU2NAME_WIN_NAME_4,
  GURU2NAME_WIN_MAX,
};

#define STR_BUF_SIZE (0x0100) ///<�����o�b�t�@�T�C�Y

//--------------------------------------------------------------
/// �J����
//--------------------------------------------------------------
#define CAMERA_TARGET_X (FX32_ONE*0)    ///<�J�����^�[�Q�b�g
#define CAMERA_TARGET_Y (FX32_ONE*0)
#define CAMERA_TARGET_Z (FX32_ONE*0)

#define CM_ANGLE_X (-32)          ///<�J�����A���O��
#define CM_ANGLE_Y (0)
#define CM_ANGLE_Z (0)
//#define CM_PERSP (10)           ///<�J�����p�[�X
#define CM_PERSP (6)            ///<�J�����p�[�X
#define CM_DISTANCE (0x103)

#define CAMERA_ANGLE_X (FX_GET_ROTA_NUM(CM_ANGLE_X))
#define CAMERA_ANGLE_Y (FX_GET_ROTA_NUM(CM_ANGLE_Y))
#define CAMERA_ANGLE_Z (FX_GET_ROTA_NUM(CM_ANGLE_Z))
#define CAMERA_PERSP (FX_GET_ROTA_NUM(CM_PERSP))
#define CAMERA_DISTANCE (CM_DISTANCE<<FX32_SHIFT)

//--------------------------------------------------------------
/// �^�b�`�p�l��
//--------------------------------------------------------------

#define TP_BTN_CX  (  128  )  ///< �{�^�����S���W
#define TP_BTN_CY  (   96  )  ///< �{�^�����S���W
#define TP_BTN_R   (   64  )  ///< ���a
#define TP_BTN_HSX ( 128/2 )  ///< �{�^���T�C�Y1/2
#define TP_BTN_HSY ( 128/2 )  ///< �{�^���T�C�Y1/2

//--------------------------------------------------------------
/// �p�x
//--------------------------------------------------------------
#define ROT16_ONE (0x10000/256)
#define ROT16_360 (ROT16_ONE*256)
#define ROT16_AG(a) (ROT16_ONE*(a))
#define ROT16_AG_FX(a) NUM_FX32(ROT16_AG(a))


//--------------------------------------------------------------
/// 3D���\�[�X��`
//--------------------------------------------------------------
#define GURU2_3DRES_NUM ( 20 )    // �ǂݍ��݃��\�[�X��
#define GURU2_3DOBJ_MAX ( 11 )    // RENDEROBJ��


//--------------------------------------------------------------
//  �M
//--------------------------------------------------------------

#define DISC_POS_X_FX32             ( NUM_FX32(0) )     ///<�M�����ʒu
#define DISC_POS_Y_FX32             ( NUM_FX32(-36) )
#define DISC_POS_Z_FX32             ( NUM_FX32(0) )

#define DISC_ROTATE_X               ( 0 )               ///<�M��]�p�xX
#define DISC_ROTATE_Y               ( 0 )
#define DISC_ROTATE_Z               ( 0 )
#define DISC_ROTATE_DRAW_OFFS_FX_2  ( NUM_FX32(-72) )
#define DISC_ROTATE_DRAW_OFFS_FX_3  ( NUM_FX32(0) )
#define DISC_ROTATE_DRAW_OFFS_FX_4  ( NUM_FX32(-181) )
#define DISC_ROTATE_DRAW_OFFS_FX_5  ( NUM_FX32(0) )

#define DISC_ROTATE_SPEED_MAX_FX    ( NUM_FX32(-4) )  ///<��]���x
#define EGG_ROTATE_SPEED_MAX_FX     ( NUM_FX32(4) )   ///<��]���x
#define DISC_ROTATE_SPEED_LOW_FX    ( NUM_FX32(-1) )  ///<��]���x
#define EGG_ROTATE_SPEED_LOW_FX     ( NUM_FX32(1) )   ///<��]���x

#define EGG_ATARI_HABA_L_FX         ( NUM_FX32(-4) )
#define EGG_ATARI_HABA_R_FX         ( NUM_FX32(5) )

//#define DISC_TOP_SPEED      ( 14 )
#define DISC_TOP_SPEED      ( 7 )
#define DISC_TOP_SPEED_FX   ( NUM_FX32(DISC_TOP_SPEED ))
#define DISC_ACCEL_FX       ( ACCEL_FRAME_FX(GURU2_GAME_FRAME_H,DISC_TOP_SPEED) )
#define DISC_LOW_SPEED      ( 4 )
#define DISC_LOW_SPEED_FX   ( NUM_FX32(DISC_LOW_SPEED) )
#define DISC_LAST_RANGE_FX  ( NUM_FX32(16) )

//--------------------------------------------------------------
//  ��
//--------------------------------------------------------------
#define EGG_DISC_CX_FX (NUM_FX32(0))      ///<���M���S�ʒu
#define EGG_DISC_CY_FX (NUM_FX32(-6))
#define EGG_DISC_CZ_FX (NUM_FX32(0))
#define EGG_DISC_CXS (22)           ///<���M���a�T�C�Y
#define EGG_DISC_CZS (22)

#define EGG_START_OFFS_Y_FX (NUM_FX32(192))   ///<���o��I�t�Z�b�g
#define EGG_ADD_NEXT_FRAME (15)         ///<���ǉ����̑҂�����
#define EGG_ADD_END_WAIT (30)         ///<���ǉ���̑҂�����

#define EGG_JUMP_TOPSPEED     (4)
#define EGG_JUMP_TOPSPEED_FX  NUM_FX32(EGG_JUMP_TOPSPEED)
#define EGG_JUMP_FLIGHT_FRAME   (30)
#define EGG_JUMP_FLIGHT_FRAME_HL  (EGG_JUMP_FLIGHT_FRAME/2)
#define EGG_JUMP_ACCLE_FX (EGG_JUMP_TOPSPEED_FX/EGG_JUMP_FLIGHT_FRAME_HL)
#define EGG_JUMP_ROTATE_ACCLE_FX (ROT16_AG_FX(32*4)/EGG_JUMP_FLIGHT_FRAME)

#define EGG_FRONT_ANGLE (90)

#define EGG_SPIN_TOPSPEED (24)
#define EGG_SPIN_TOPSPEED_FX NUM_FX32(EGG_SPIN_TOPSPEED)
#define EGG_SPIN_STOPSPEED (4)
#define EGG_SPIN_STOPSPEED_FX NUM_FX32(EGG_SPIN_STOPSPEED)
#define EGG_SPIN_DOWNSPEED (2)
#define EGG_SPIN_DOWNSPEED_FX NUM_FX32(EGG_SPIN_DOWNSPEED)

#define EGG_SHAKE_HABA (30)
#define EGG_SHAKE_HABA_FX (NUM_FX32(EGG_SHAKE_HABA))
#define EGG_SHAKE_TOPSPEED (20)
#define EGG_SHAKE_TOPSPEED_FX NUM_FX32(EGG_SHAKE_TOPSPEED)

//--------------------------------------------------------------
//  �J�E���g�_�E�����b�Z�[�W
//--------------------------------------------------------------
#define CDOWN_CX (128)
#define CDOWN_CY (100)
#define CDOWN_WAIT_FRAME (FRAME_SEC*3+10)
#define CDOWN_SE_FRAME (30)

//--------------------------------------------------------------
//  �{�^��
//--------------------------------------------------------------
#define BTN_ANM_FRAME_START (2)
#define BTN_ANM_FRAME_ON  (4)
#define BTN_ANM_FRAME_BACK  (2)

//--------------------------------------------------------------
//  ���b�Z�[�W
//--------------------------------------------------------------
#define MSG_WAIT                    ( msg_guru2_00  )
#define MSG_NICKNAME                ( msg_guru2_01  )
#define MSG_EGG_GET                 ( msg_guru2_02  )
#define MSG_OMAKE_AREA              ( msg_guru2_03  )
#define MSG_ZANNEN                  ( msg_guru2_04  )
#define MSG_COMM_TAIKI              ( msg_guru2_05  )
#define MSG_COMM_ERROR_MEMBER       ( msg_guru2_06  )
#define MSG_COMM_ERROR_CANCEL_OYA   ( msg_guru2_07  )
#define MSG_COMM_ERROR              ( msg_guru2_08  ) 
#define MSG_COMM_ERROR_JOIN_CLOSE   ( msg_guru2_09  )
#define MSG_SAVE                    ( msg_guru2_11  )
#define MSG_COMM_WAIT               ( msg_guru2_10  )
#define MSG_COMM_ERROR_DAME_TAMAGO  ( msg_guru2_12  )

//--------------------------------------------------------------
//  �{�^��
//--------------------------------------------------------------
typedef enum
{
  BTN_OFF = 0,        ///<�{�^��������Ă��Ȃ�
  BTN_ON,             ///<�{�^��������Ă���
}BTN;

///�{�^�����������ۂ̃E�F�C�g�t���[��
#define BTN_PUSH_WAIT_FRAME (FRAME_SEC*3)

//--------------------------------------------------------------
/// ���C���֐��߂�l
//--------------------------------------------------------------
typedef enum
{
  RET_NON = 0,  ///<���ɂȂ�
  RET_CONT,     ///<�p��
  RET_END,      ///<�I��
}RET;

//--------------------------------------------------------------
/// ���C�������V�[�P���X
//--------------------------------------------------------------
enum
{
  SEQNO_MAIN_INIT = 0,
  SEQNO_MAIN_FADEIN_WAIT,
  
  SEQNO_MAIN_OYA_SIGNAL_JOIN_WAIT,
  SEQNO_MAIN_OYA_SEND_JOIN_CLOSE,
  SEQNO_MAIN_OYA_CONNECT_NUM_CHECK,
  SEQNO_MAIN_OYA_SEND_PLAY_MAX,
  SEQNO_MAIN_OYA_SEND_PLAY_NO,
  SEQNO_MAIN_OYA_SIGNAL_EGG_ADD_START,
  
  SEQNO_MAIN_KO_SEND_SIGNAL_JOIN,
  SEQNO_MAIN_KO_EGG_ADD_START_WAIT,
  
  SEQNO_MAIN_EGG_DATA_SEND_INIT,
  SEQNO_MAIN_EGG_DATA_SEND_TIMING_WAIT,
  SEQNO_MAIN_EGG_DATA_TRADE_POS_SEND,
  SEQNO_MAIN_EGG_DATA_SEND,
  SEQNO_MAIN_EGG_DATA_RECV_WAIT,
  
  SEQNO_MAIN_EGG_DATA_CHECK_WAIT,
  
  SEQNO_MAIN_EGG_ADD_INIT,
  SEQNO_MAIN_EGG_ADD,
  SEQNO_MAIN_EGG_ADD_WAIT,
  SEQNO_MAIN_EGG_ADD_END_WAIT,
  
  SEQNO_MAIN_SEND_GAME_START_FLAG,
  SEQNO_MAIN_RECV_GAME_START_FLAG,
  
#ifdef DEBUG_DISP_CHECK
  SEQNO_MAIN_DEBUG_DISP_CHECK,
#endif
  
  SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_INIT,
  SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_WAIT,
  SEQNO_MAIN_COUNTDOWN_INIT,
  SEQNO_MAIN_COUNTDOWN,
  
  SEQNO_MAIN_GAME_INIT,
  SEQNO_MAIN_GAME_OYA,
  
  SEQNO_MAIN_GAME_END_INIT,
  SEQNO_MAIN_GAME_END_ERROR_ROTATE,
  SEQNO_MAIN_GAME_END_LAST_ROTATE,
  SEQNO_MAIN_GAME_END_TIMING_INIT,
  SEQNO_MAIN_GAME_END_TIMING_WAIT,
  SEQNO_MAIN_GAME_END_OYA_DATA_SEND,
  SEQNO_MAIN_GAME_END_KO_DATA_RECV,
  
  SEQNO_MAIN_RESULT_INIT,
  SEQNO_MAIN_RESULT_NAME_WAIT,
  SEQNO_MAIN_RESULT_MSG_WAIT,
  
  SEQNO_MAIN_OMAKE_CHECK,
  SEQNO_MAIN_OMAKE_MSG_WAIT,
  SEQNO_MAIN_OMAKE_ERROR_MSG_START_WAIT,
  SEQNO_MAIN_OMAKE_ERROR_MSG_WAIT,
  
  SEQNO_MAIN_SAVE_BEFORE_TIMING_INIT,
  SEQNO_MAIN_SAVE_BEFORE_TIMING_WAIT,
  SEQNO_MAIN_SAVE,
  
  #if 0
  SEQNO_MAIN_COMM_ERROR_MSG,
  SEQNO_MAIN_COMM_ERROR_MEMBER_MSG,
  SEQNO_MAIN_COMM_ERROR_OYA_CANCEL_MSG,
  SEQNO_MAIN_COMM_ERROR_JOIN_CLOSE_MSG,
  #endif
  SEQNO_MAIN_COMM_ERROR_DAME_TAMAGO_MSG,
  
  SEQNO_MAIN_MSG_WAIT_NEXT_END,
  
  SEQNO_MAIN_END_TIMING_SYNC_INIT,
  SEQNO_MAIN_END_TIMING_SYNC,
  
  SEQNO_MAIN_END_CONNECT_CHECK,
  
  SEQNO_MAIN_END_FADEOUT_START,
  SEQNO_MAIN_END_FADEOUT,
  SEQNO_MAIN_END,
  
  SEQNO_MAIN_MAX,
};

//--------------------------------------------------------------
/// ���o�ꏈ���V�[�P���X
//--------------------------------------------------------------
enum
{
  SEQNO_EGGSTART_INIT = 0,
  SEQNO_EGGSTART_FALL,
  SEQNO_EGGSTART_FALL_END,
  SEQNO_EGGSTART_MAX,
};

//--------------------------------------------------------------
/// �^�X�N�v���C�I���e�B
//--------------------------------------------------------------
enum
{
  TCBPRI_EGG_DISC_JUMP = 0xff,
  TCBPRI_EGG_JUMP,
  TCBPRI_EGG_SPIN,
  TCBPRI_EGG_SHAKE,
  TCBPRI_BTN_ANM,
  TCBPRI_OMAKE_JUMP,
};

//==============================================================================
//  typedef
//==============================================================================

#define VTCBPRI_FRO_TEXBIND (0xffff)    ///<�e�N�X�`���o�C���hVBlankTCBPriorty

//--------------------------------------------------------------
/// loop?
//--------------------------------------------------------------
typedef enum
{
  ANMLOOP_OFF = 0,
  ANMLOOP_ON,
}ANMLOOPTYPE;

//==============================================================================
//  typedef
//==============================================================================
//--------------------------------------------------------------
//  typedef
//--------------------------------------------------------------
typedef struct _TAG_FRO_MDL FRO_MDL;
typedef struct _TAG_FRO_ANM FRO_ANM;
typedef struct _TAG_FRO_OBJ FRO_OBJ;

//--------------------------------------------------------------
/// ROTATE
//--------------------------------------------------------------
typedef struct
{
  s16 x;
  s16 y;
  s16 z;
  u16 dmy;
}ROTATE;

//--------------------------------------------------------------
/// FRO_MDL 
//--------------------------------------------------------------
struct _TAG_FRO_MDL
{
  BOOL bind_tex_flag;
  NNSG3dResFileHeader *pResFileHeader;
  NNSG3dResMdlSet *pResMdlSet;
  NNSG3dResMdl *pResMdl;
  NNSG3dResTex *pResTex;
};

//--------------------------------------------------------------
/// FRO_ANM
//--------------------------------------------------------------
struct _TAG_FRO_ANM
{
  u32 status_bit;
  fx32 frame;
  void *pResAnm;
  void *pResAnmIdx;
  NNSG3dAnmObj *pAnmObj;
  NNSFndAllocator Allocator;
};

//--------------------------------------------------------------
/// FRO_OBJ
//--------------------------------------------------------------
struct _TAG_FRO_OBJ
{
  NNSG3dRenderObj RenderObj;
};
//--------------------------------------------------------------
//  DEBUG
//--------------------------------------------------------------
#ifdef GURU2_DEBUG_ON
typedef struct
{
  int type;
  int dummy;
  int angle_x;
  int angle_y;
  int angle_z;
  int persp;
  int distance;
}DEBUGWORK;
#endif

//--------------------------------------------------------------
/// DISCWORK
//--------------------------------------------------------------
typedef struct
{
  VecFx32 pos;
  VecFx32 draw_pos;
  VecFx32 scale;
  VecFx32 offs;   //�M�ɂ̂݉e����^����I�t�Z�b�g
  VecFx32 offs_egg; //���ɂ��e����^����I�t�Z�b�g
  fx32  rotate_fx;
  fx32  rotate_offs_fx;
  fx32  rotate_draw_offs_fx;
  fx32  speed_fx;
  ROTATE  rotate;
  FRO_MDL rmdl;
  FRO_OBJ robj;
  u32     unitIndex;  // ���f�����O���\�[�XID
}DISCWORK;

//--------------------------------------------------------------
/// EGGACTOR
//--------------------------------------------------------------
typedef struct
{
  int use_flag;
  int draw_flag;
  int set_flag;
  int comm_id;
  int play_no;
  int name_no;
  int shake_count;
  fx32 angle;
  fx32 offs_angle;
  ROTATE rotate;
  VecFx32 rotate_fx;
  VecFx32 scale;
  VecFx32 pos;
  VecFx32 offs;
  fx32 joffs; // �W�����v�p�I�t�Z�b�g
//  CATS_ACT_PTR cap; //EGG_3D_USE
  FRO_OBJ robj;
}EGGACTOR;

//--------------------------------------------------------------
/// EGGKAGE
//--------------------------------------------------------------
typedef struct
{
  int use_flag;
  ROTATE rotate;
  VecFx32 scale;
  VecFx32 pos;
  FRO_OBJ robj;
  EGGACTOR *eact;
}EGGKAGE;

//--------------------------------------------------------------
/// EGGCURSOR
//--------------------------------------------------------------
typedef struct
{
  int use_flag;
  VecFx32 pos;
  FRO_OBJ robj;
  EGGACTOR *eact;
}EGGCURSOR;

//--------------------------------------------------------------
/// EGGWORK
//--------------------------------------------------------------
typedef struct
{
  FRO_MDL m_rmdl;
  FRO_MDL m_rmdl_kage;
  FRO_MDL m_rmdl_cursor[G2MEMBER_MAX];
  FRO_ANM m_ranm_cursor[G2MEMBER_MAX];
  EGGACTOR eact[G2MEMBER_MAX];
  EGGKAGE ekage[G2MEMBER_MAX];
  EGGCURSOR ecursor[G2MEMBER_MAX];
  u32     unitIndex_egg;
  u32     unitIndex_kage;
  u32     unitIndex_cursor[G2MEMBER_MAX];
}EGGWORK;

//--------------------------------------------------------------
/// EGGSTARTWORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  int frame;
  int fall_se_play;
  fx32 fall_speed;
  EGGACTOR *eact;
  GURU2MAIN_WORK *g2m;
}EGGSTARTWORK;

//--------------------------------------------------------------
/// MSGWORK
//--------------------------------------------------------------
typedef struct 
{
  int win_name_max;
  
  GFL_FONT    *font;
  GFL_MSGDATA *msgman;
  WORDSET *wordset;
  GFL_BMPWIN *bmpwin_talk;
  GFL_BMPWIN *bmpwin_name[GURU2NAME_WIN_MAX];

  PRINT_UTIL printUtilTalk;
  PRINT_UTIL printUtilName[GURU2NAME_WIN_MAX];
  PRINT_QUE  *printQue;
  void *strbuf;
}MSGWORK;

//--------------------------------------------------------------
/// EGGADDWORK
//--------------------------------------------------------------
typedef struct
{
  int count;
  int offs;
  int add;
  int id;
  int no;
  int max;
}EGGADDWORK;

//--------------------------------------------------------------
/// CAMERAWORK
//--------------------------------------------------------------
typedef struct
{
  fx32 distance;
  u32 persp;
  VecFx32 target_pos;
  GFL_G3D_CAMERA *gf_camera;
}CAMERAWORK;


//--------------------------------------------------------------
/// BTNFADE_WORK
//--------------------------------------------------------------
typedef struct
{
  GURU2MAIN_WORK *g2m;
  int light_flag;
  u32 evy;
}BTNFADE_WORK;

//--------------------------------------------------------------
/// OMAKE_AREA
//--------------------------------------------------------------
typedef struct
{
  u16 start;
  u16 end;
}OMAKE_AREA;

//--------------------------------------------------------------
/// OMAKE_AREA_TBL
//--------------------------------------------------------------
typedef struct
{
  int max;
  const OMAKE_AREA *area;
}OMAKE_AREA_TBL;

//--------------------------------------------------------------
/// EGGJUMP_WORK
//--------------------------------------------------------------
typedef struct
{
  int use_flag;
  int seq_no;
  int frame;
  int turn_flag;
  fx32 add_y;
  fx32 accle_y;
  fx32 accle_r;
  EGGACTOR *eact;
}EGGJUMP_WORK;

//--------------------------------------------------------------
/// EGGJUMPTCB_WORK
//--------------------------------------------------------------
typedef struct
{
  EGGJUMP_WORK jump_work[G2MEMBER_MAX];
}EGGJUMPTCB_WORK;

//--------------------------------------------------------------
/// EGGSPIN_WORK
//--------------------------------------------------------------
typedef struct
{
  int use_flag;
  int seq_no;
  int count;
  fx32 add_fx;
  EGGACTOR *eact;
}EGGSPIN_WORK;

//--------------------------------------------------------------
/// EGGSPINTCB_WORK
//--------------------------------------------------------------
typedef struct
{
  EGGSPIN_WORK spin_work[G2MEMBER_MAX];
}EGGSPINTCB_WORK;

//--------------------------------------------------------------
/// EGGSHAKE_WORK
//--------------------------------------------------------------
typedef struct
{
  int use_flag;
  int seq_no;
  fx32 haba_fx;
  fx32 add_fx;
  fx32 rad;
  EGGACTOR *eact;
}EGGSHAKE_WORK;

//--------------------------------------------------------------
/// EGGSHAKETCB_WORK
//--------------------------------------------------------------
typedef struct
{
  EGGSHAKE_WORK shake_work[G2MEMBER_MAX];
}EGGSHAKETCB_WORK;

//--------------------------------------------------------------
/// EGGDISCJUMPTCB_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  int jump_flag;    //�W�����v��
  int jump_proc_flag; //�W�����v������
  int shake_count;
  fx32 shake_fx;
}EGGDISCJUMPTCB_WORK;

//--------------------------------------------------------------
/// BTNANMTCB_WORK
//--------------------------------------------------------------
typedef struct
{
  u16 seq_no;
  u16 push_flag;
  int frame;
}BTNANMTCB_WORK;

//--------------------------------------------------------------
/// OMAKEJUMP_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  int count;
  int wait;
  EGGACTOR *eact;
}OMAKEJUMP_WORK;

//--------------------------------------------------------------
/// OMAKEJUMPTCB_WORK
//--------------------------------------------------------------
typedef struct
{
  OMAKEJUMP_WORK omake_jump_work[G2MEMBER_MAX];
}OMAKEJUMPTCB_WORK;

typedef struct {
  u32 frame;
  u8  x,y,w,h;
  u16 pal, chr;
}BMPWIN_DAT;

//--------------------------------------------------------------
/// COMMDATA
//--------------------------------------------------------------
typedef struct
{
  int my_play_no;
  int play_max;
  int push_btn;
  int send_btn;
  int game_data_send_flag;
  int game_result_get_flag;
  int trade_no_tbl[G2MEMBER_MAX];
  GURU2COMM_GAMEDATA game_data;
  GURU2COMM_GAMERESULT game_result;
  GURU2COMM_PLAYNO play_no_tbl[G2MEMBER_MAX];
  const MYSTATUS *my_status[G2MEMBER_MAX];
  STRBUF *my_name_buf[G2MEMBER_MAX];
}COMMDATA;

enum {
  GURU2MAIN_CLACT_RES_CHR,
  GURU2MAIN_CLACT_RES_PLTT,
  GURU2MAIN_CLACT_RES_CELL,
  GURU2MAIN_CLACT_RES_MAX,
};
//--------------------------------------------------------------
/// GURU2MAIN_WORK
//--------------------------------------------------------------
struct GURU2MAIN_WORK
{
  int seq_no;
  int save_seq;
  int game_frame;
  int frame;
  int comm_wait_frame;
  int play_send_count;
  int force_end_flag;
  u32 omake_bit;
  BOOL end_flag;
  
  POKEPARTY *my_poke_party;
    
  EGGACTOR     *front_eggact;
  
  COMMDATA comm;
  
  u8 temp_work[G2M_TEMPWORK_SIZE];
  
  GURU2PROC_WORK *g2p;
  GURU2COMM_WORK *g2c;
  
  DISCWORK disc;
  EGGWORK egg;
  GFL_G3D_UTIL *g3dUtil;     // 3D���\�[�X�Ǘ�UTIL
  
  EGGJUMPTCB_WORK egg_jump_tcb_work;
  EGGSPINTCB_WORK egg_spin_tcb_work;
  EGGSHAKETCB_WORK egg_shake_tcb_work;
  EGGDISCJUMPTCB_WORK egg_disc_jump_tcb_work;
  BTNANMTCB_WORK btn_anm_tcb_work;
  OMAKEJUMPTCB_WORK omake_jump_tcb_work;
  
  MSGWORK msgwork;
  CAMERAWORK camera;
  ARCHANDLE *arc_handle;
  
  NNSG2dScreenData    *bg_pScr;
  NNSG2dCharacterData *bg_pChar;
  NNSG2dPaletteData   *bg_pPltt;
  
//  CATS_SYS_PTR csp;
//  CATS_RES_PTR crp;
  GFL_CLUNIT  *clUnit;
  u32         resobj[GURU2MAIN_CLACT_RES_MAX];
  
  PALETTE_FADE_PTR pfd;
  
  GFL_TCB* tcb_egg_jump;
  GFL_TCB* tcb_egg_spin;
  GFL_TCB* tcb_egg_shake;
  GFL_TCB* tcb_egg_disc_jump;
  GFL_TCB* tcb_btn_anm;
  GFL_TCB* tcb_omake_jump;
  
  void        *tcbSysWork;  // �^�X�N�V�X�e���p���[�N
  GFL_TCBSYS  *tcbSys;      // �^�X�N�V�X�e��
  GFL_TCB     *vintr_tcb;   // Vblank���荞��TCB

  NET_SAVE_WORK *NetSaveWork;   // �ʐM�����Z�[�u�p���[�N

  u32         unitIndex_bg;     // �w�i�RD�f�[�^���\�[�X�C���f�b�N�X
  TIMEICON_WORK *TimeIconWork;  // ���ԃA�C�R��

#ifdef GURU2_DEBUG_ON
  DEBUGWORK debug;
#endif
};

//==============================================================================
//  proto
//==============================================================================
static void guru2Main_FrameWorkClear( GURU2MAIN_WORK *g2m );
static void guru2Main_Delete( GURU2MAIN_WORK *g2m );

static RET (* const DATA_Guru2ProcTbl[SEQNO_MAIN_MAX])( GURU2MAIN_WORK *g2m );

static void guru2_VBlankFunc( GFL_TCB *tcb, void * work );

static void guru2_DrawInit( GURU2MAIN_WORK *g2m );
static void guru2_DrawDelete( GURU2MAIN_WORK *g2m );
static void guru2_DrawProc( GURU2MAIN_WORK *g2m );

static void guru2_DispInit( GURU2MAIN_WORK *g2m );
static void guru2_DispON( GURU2MAIN_WORK *g2m );
static void guru2_DispOFF( GURU2MAIN_WORK *g2m );

static void guru2_3DDrawInit( GURU2MAIN_WORK *g2m );

static void guru2_BGInit( GURU2MAIN_WORK *g2m );
static void guru2_BGResLoad( GURU2MAIN_WORK *g2m );
static void guru2_BGDelete( GURU2MAIN_WORK *g2m );
static void Guru2BG_ButtonONSet( GURU2MAIN_WORK *g2m );
static void Guru2BG_ButtonOFFSet( GURU2MAIN_WORK *g2m );
static void Guru2BG_ButtonMiddleSet( GURU2MAIN_WORK *g2m );

static void guru2_PlttFadeInit( GURU2MAIN_WORK *g2m );
static void guru2_PlttFadeDelete( GURU2MAIN_WORK *g2m );
static void Guru2PlttFade_BtnFade( GURU2MAIN_WORK *g2m, u32 evy );

static void guru2_ClActInit( GURU2MAIN_WORK *g2m );
static void guru2_ClActResLoad( GURU2MAIN_WORK *g2m );
static void guru2_ClActDelete( GURU2MAIN_WORK *g2m );

static void guru2_TalkWinFontInit( GURU2MAIN_WORK *g2m );
static void guru2_TalkWinFontDelete( GURU2MAIN_WORK *g2m );
static void Guru2TalkWin_Write( GURU2MAIN_WORK *g2m, u32 msgno );
static void Guru2TalkWin_WritePlayer(
  GURU2MAIN_WORK *g2m, u32 msgno, const MYSTATUS *status );
static void Guru2TalkWin_WriteItem( GURU2MAIN_WORK *g2m, u32 msgno, u32 id );
static void Guru2TalkWin_Clear( GURU2MAIN_WORK *g2m );
static void Guru2NameWin_Init( GURU2MAIN_WORK *g2m, int max );
static void Guru2NameWin_Delete( GURU2MAIN_WORK *g2m );
static void Guru2NameWin_Write(
  GURU2MAIN_WORK *g2m, STRBUF *name, int no, u32 col );
static void Guru2NameWin_WriteIDColor(
  GURU2MAIN_WORK *g2m, STRBUF *name, int no, int id );
static void Guru2NameWin_Clear( GURU2MAIN_WORK *g2m, int no );

static void guru2_CameraInit( GURU2MAIN_WORK *g2m );
static void guru2_CameraDelete( GURU2MAIN_WORK *g2m );
static void guru2_CameraSet( GURU2MAIN_WORK *g2m );

static BOOL Guru2TP_ButtonHitTrgCheck( GURU2MAIN_WORK *g2m );

static void Disc_Init( GURU2MAIN_WORK *g2m );
static void Disc_Delete( GURU2MAIN_WORK *g2m );
static void Disc_Update( GURU2MAIN_WORK *g2m );
static void Disc_Draw( GURU2MAIN_WORK *g2m );
static void Disc_Rotate( DISCWORK *disc, fx32 add );

static void Egg_Init( GURU2MAIN_WORK *g2m );
static void Egg_Delete( GURU2MAIN_WORK *g2m );
static void Egg_MdlActInit( GURU2MAIN_WORK *g2m, EGGACTOR *act );

static void Bg3D_Init( GURU2MAIN_WORK *g2m );
static void Bg3D_Delete( GURU2MAIN_WORK *g2m );
static void Bg3D_Draw( GURU2MAIN_WORK *g2m );


static void EggAct_Update( GURU2MAIN_WORK *g2m );
static void EggAct_Draw( GURU2MAIN_WORK *g2m );
static void EggAct_Rotate( GURU2MAIN_WORK *g2m, fx32 add );
static void EggAct_AnglePosSet( EGGACTOR *eact, const VecFx32 *offs, fx32 fa, fx32 joffs );
static EGGACTOR * EggAct_FrontEggActGet( GURU2MAIN_WORK *g2m );

static void EggKage_Init( GURU2MAIN_WORK *g2m, EGGACTOR *act );
static void EggKage_Update( GURU2MAIN_WORK *g2m, EGGKAGE *ekage, fx32 joffs );

static void EggCursor_Init( GURU2MAIN_WORK *g2m, EGGACTOR *act );
static void EggCursor_Update( GURU2MAIN_WORK *g2m, EGGCURSOR *ecs, int i );

static GFL_TCB* EggAct_StartTcbSet(
  GURU2MAIN_WORK *g2m, int id, int no, int name_no,
  int angle, int offs, EGGACTOR *eact );
static void EggActStartTcb( GFL_TCB* tcb, void *wk );
static RET (* const DATA_EggStartTbl[SEQNO_EGGSTART_MAX])( EGGSTARTWORK *work );

static void EggJumpTcb_Init( GURU2MAIN_WORK *g2m );
static void EggJumpTcb_Delete( GURU2MAIN_WORK *g2m );
static void EggJumpTcb_EggSetAll( GURU2MAIN_WORK *g2m );
static BOOL EggJumpTcb_JumpCheck( GURU2MAIN_WORK *g2m );

static void EggSpinTcb_Init( GURU2MAIN_WORK *g2m );
static void EggSpinTcb_Delete( GURU2MAIN_WORK *g2m );
static void EggSpinTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act );

static void EggShakeTcb_Init( GURU2MAIN_WORK *g2m );
static void EggShakeTcb_Delete( GURU2MAIN_WORK *g2m );
static void EggShakeTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act );

static void EggDiscJumpTcb_Init( GURU2MAIN_WORK *g2m );
static void EggDiscJumpTcb_Delete( GURU2MAIN_WORK *g2m );
static void EggDiscJumpTcb_JumpSet( GURU2MAIN_WORK *g2m );
static BOOL EggDiscJumpTcb_JumpCheck( GURU2MAIN_WORK *g2m );
static BOOL EggDiscJumpTcb_JumpProcCheck( GURU2MAIN_WORK *g2m );

static void BtnAnmTcb_Init( GURU2MAIN_WORK *g2m );
static void BtnAnmTcb_Delete( GURU2MAIN_WORK *g2m );
static BOOL BtnAnmTcb_PushCheck( GURU2MAIN_WORK *g2m );
static void BtnAnmTcb_PushSet( GURU2MAIN_WORK *g2m );

static void OmakeEggJumpTcb_Init( GURU2MAIN_WORK *g2m );
static void OmakeEggJumpTcb_Delete( GURU2MAIN_WORK *g2m );
static void OmakeEggJumpTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act );
static void OmakeEggJumpTcb_OmakeBitCheckSet( GURU2MAIN_WORK *g2m );

static GFL_TCB* guru2_CountDownTcbSet( GURU2MAIN_WORK *g2m );

static void BtnFadeTcbAdd( GURU2MAIN_WORK *g2m, int light );

static void AngleAdd( fx32 *angle, fx32 add );
static void DiscRotateEggMove( GURU2MAIN_WORK *gm, fx32 speed );
static BOOL Guru2GameTimeCount( GURU2MAIN_WORK *g2m );
static void * Guru2Arc_DataLoad( GURU2MAIN_WORK *g2m, u32 idx, BOOL fb );

static BOOL Guru2Pad_TrgCheck( u32 pad );
static BOOL Guru2Pad_ContCheck( u32 pad );
static void * Guru2MainTempWorkInit( GURU2MAIN_WORK *g2m, u32 size );
static void * Guru2MainTempWorkGet( GURU2MAIN_WORK *g2m );
static BOOL Guru2MainCommSignalCheck(GURU2MAIN_WORK *g2m, u16 bit);
//static BOOL Guru2MainCommJoinCloseCheck( GURU2MAIN_WORK *g2m );
static BOOL Guru2MainCommJoinNumCheck( GURU2MAIN_WORK *g2m );
static int Guru2MainCommJoinNumGet( GURU2MAIN_WORK *g2m );
//static BOOL Guru2MainCommOyaCancelCheck( GURU2MAIN_WORK *g2m );
static BOOL Guru2MainCommIDPlayCheck( GURU2MAIN_WORK *g2m, int id );
static void Guru2MainFriendEggExchange( GURU2MAIN_WORK *g2m, int id );
static int Guru2MainCommEggDataNumGet( GURU2MAIN_WORK *g2m );
static BOOL Guru2MainOmakeZoneCheck( GURU2MAIN_WORK *g2m, fx32 pos, int max );
static BOOL Guru2MainPokePartyDameTamagoCheck( POKEPARTY *ppty );
static BOOL Guru2MainDameTamagoCheck( GURU2MAIN_WORK *g2m );
static int Guru2MainCommEggDataOKCountCheck( GURU2MAIN_WORK *g2m );
static BOOL Guru2MainCommEggDataNGCheck( GURU2MAIN_WORK *g2m );

static const BMPWIN_DAT DATA_Guru2BmpTalkWinList;

static const BMPWIN_DAT * const DATA_Guru2BmpNameWinTbl[G2MEMBER_MAX+1];
static const u16 DATA_DiscOffsetAngle[G2MEMBER_MAX+1][G2MEMBER_MAX];
static const u16 DATA_EggStartAngle[G2MEMBER_MAX+1][G2MEMBER_MAX];
//static const u16 DATA_GameEndAngle[G2MEMBER_MAX+1];
static const OMAKE_AREA_TBL DATA_OmakeAreaTbl[G2MEMBER_MAX+1];
static const u32 DATA_KinomiTbl[G2MEMBER_MAX+1][2];
static const fx32 DATA_DiscRotateDrawOffset[G2MEMBER_MAX+1];
static BOOL  _me_end_check( void );
static void  _me_play( int seq_bgm );
static void _comm_friend_func( GURU2MAIN_WORK *g2m );

static void _comm_parent_rotate( GURU2MAIN_WORK *g2m, fx32 speed );

#ifdef GURU2_DEBUG_ON
static void DEBUG_WorkInit( GURU2MAIN_WORK *g2m );
static void DEBUG_Proc( GURU2MAIN_WORK *g2m );
#endif

#ifdef DEBUG_DISP_CHECK
extern void DEBUG_DiscTest( GURU2MAIN_WORK *g2m );
#endif

static void print_all_egg_data( GURU2MAIN_WORK* g2m );

#define GURU2_TCB_MAX   ( 32 )

//==============================================================================
//  ���邮������@���C������
//==============================================================================
//--------------------------------------------------------------
/**
 * ���邮�郁�C������������
 * @param proc  GFL_PROC *
 * @param seq   seq
 * @retval  GFL_PROC_RESULT GFL_PROC_RES_CONTINUE,GFL_PROC_RES_FINISH
 */
//--------------------------------------------------------------
GFL_PROC_RESULT Guru2MainProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  GURU2MAIN_WORK *g2m;
  GURU2PROC_WORK *g2p = (GURU2PROC_WORK *)pwk;
  
  //�ؒf�֎~
  GFL_NET_SetAutoErrorCheck( TRUE );      

  //�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_GURU2, GURU2_HEAPSIZE );
  
  //���[�N�m��
  g2m = GFL_PROC_AllocWork( proc, sizeof(GURU2MAIN_WORK), HEAPID_GURU2 );
  GFL_STD_MemFill( g2m, 0, sizeof(GURU2MAIN_WORK) );
  
  g2p->g2m = g2m;
  g2m->g2p = g2p;
  g2m->g2c = g2p->g2c;
  
  OS_Printf("g2m=%08x, g2p=%08x, g2c=%08x\n", (u32)g2m, (u32)g2p, (u32)g2p->g2c);
  
  //�|�P�����p�[�e�B
  g2m->my_poke_party = GAMEDATA_GetMyPokemon( g2m->g2p->param->gamedata );
  
  //�A�[�J�C�u�n���h���I�[�v��
  g2m->arc_handle = GFL_ARC_OpenDataHandle( ARCID_GURU2, HEAPID_GURU2 );
  
  //�O���t�B�b�N������
  guru2_DrawInit( g2m );
  OS_Printf("g2m=%08x, g2p=%08x, g2c=%08x\n", (u32)g2m, (u32)g2p, (u32)g2p->g2c);
  
  //VBlank�Z�b�g
  //  sys_VBlankFuncChange( guru2_VBlankFunc, g2m );
  g2m->vintr_tcb = GFUser_VIntr_CreateTCB( guru2_VBlankFunc, g2m, 1 );

  // ���f�����O���\�[�X�Ǘ�UTIL������
  g2m->g3dUtil = GFL_G3D_UTIL_Create( GURU2_3DRES_NUM, GURU2_3DOBJ_MAX, HEAPID_GURU2 );

  // 3D�w�i������
  Bg3D_Init( g2m );
  
  //�A�N�^�[������
  Egg_Init( g2m );
  Disc_Init( g2m );
  OS_Printf("g2m=%08x, g2p=%08x, g2c=%08x\n", (u32)g2m, (u32)g2p, (u32)g2p->g2c);
  
  { //���݂�ID�ƎQ���l���Ńf�B�X�N�p�x�Z�b�g
    int id = 0, count = 0;
    int my_id = GFL_NET_SystemGetCurrentID();
    DISCWORK *disc = &g2m->disc;
    
    do{
      if( (g2m->g2p->receipt_bit & (1 << id)) ){
        if( id == my_id ){
          break;
        }
        
        count++;
      }
      
      id++;
    }while( id < G2MEMBER_MAX );
    
    disc->rotate_offs_fx = NUM_FX32(
      DATA_DiscOffsetAngle[g2m->g2p->receipt_num][count] );
  }
  OS_Printf("g2m=%08x, g2p=%08x, g2c=%08x\n", (u32)g2m, (u32)g2p, (u32)g2p->g2c);
  
  //�^�X�N������
  {
    g2m->tcbSysWork = GFL_HEAP_AllocMemoryLo( HEAPID_GURU2, 
                                              GFL_TCB_CalcSystemWorkSize( GURU2_TCB_MAX ) );
    g2m->tcbSys = GFL_TCB_Init( GURU2_TCB_MAX, g2m->tcbSysWork);
  }
  // �^�X�N�o�^
  EggJumpTcb_Init( g2m );
  EggSpinTcb_Init( g2m );
  EggShakeTcb_Init( g2m );
  EggDiscJumpTcb_Init( g2m );
  BtnAnmTcb_Init( g2m );
  OmakeEggJumpTcb_Init( g2m );
  OS_Printf("g2m=%08x, g2p=%08x, g2c=%08x\n", (u32)g2m, (u32)g2p, (u32)g2p->g2c);
  
  //�f�o�b�O
  #ifdef GURU2_DEBUG_ON
  DEBUG_WorkInit( g2m );
  #endif
  
  //���C�v�J�n
  WIPE_SYS_Start( WIPE_PATTERN_WMS,
    WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
    WIPE_FADE_BLACK, 8, 1, HEAPID_GURU2 );
  OS_Printf("g2m=%08x, g2p=%08x, g2c=%08x\n", (u32)g2m, (u32)g2p, (u32)g2p->g2c);

  // �q�@�����Ȃ��ꍇ�̓G���[�ɂȂ�B
  GFL_NET_SetNoChildErrorCheck(TRUE);
  
  return( GFL_PROC_RES_FINISH );
}

//--------------------------------------------------------------
/**
 * ���邮�郁�C�������@�I��
 * @param proc  GFL_PROC *
 * @param seq   seq
 * @retval  GFL_PROC_RESULT GFL_PROC_RES_CONTINUE,GFL_PROC_RES_FINISH
 */
//--------------------------------------------------------------
GFL_PROC_RESULT Guru2MainProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  GURU2MAIN_WORK *g2m = (GURU2MAIN_WORK *)mywk;

  
  // �v�����g�L���[�폜
  PRINTSYS_QUE_Clear( g2m->msgwork.printQue );
  PRINTSYS_QUE_Delete( g2m->msgwork.printQue );
  
  // ���ԃA�C�R�����c���Ă���ꍇ�͍폜
  if(g2m->TimeIconWork){
    TIMEICON_Exit( g2m->TimeIconWork );
  }
  
  //�^�X�N�폜
  EggJumpTcb_Delete( g2m );
  EggSpinTcb_Delete( g2m );
  EggShakeTcb_Delete( g2m );
  EggDiscJumpTcb_Delete( g2m );
  BtnAnmTcb_Delete( g2m );
  OmakeEggJumpTcb_Delete( g2m );
  
  // �^�X�N�V�X�e���I��
  GFL_TCB_Exit( g2m->tcbSys );
  GFL_HEAP_FreeMemory( g2m->tcbSysWork );
  
  // 3D�w�i�폜
  Bg3D_Delete( g2m );
  //�A�N�^�[�폜
  Disc_Delete( g2m );
  Egg_Delete( g2m );
  
  // 3D���\�[�XUTIL�폜
  GFL_G3D_UTIL_Delete( g2m->g3dUtil );

  GFL_G3D_Exit();

  //�O���t�B�b�N�폜
  guru2_DrawDelete( g2m );
  GFL_TCB_DeleteTask( g2m->vintr_tcb );  // VBlank�֐�����

  //�A�[�J�C�u�N���[�Y
  GFL_ARC_CloseDataHandle( g2m->arc_handle );
  
  //���[�N�J��
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_GURU2 );
  
  //�\�t�g���Z�b�g�L���ɖ߂�
  GFL_UI_SoftResetEnable(GFL_UI_SOFTRESET_USER);
  
  return( GFL_PROC_RES_FINISH );
}

//--------------------------------------------------------------
/**
 * ���C������
 * @param proc  GFL_PROC *
 * @param seq   seq
 * @retval  GFL_PROC_RESULT GFL_PROC_RES_CONTINUE,GFL_PROC_RES_FINISH
 */
//--------------------------------------------------------------
GFL_PROC_RESULT Guru2MainProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  RET ret;
  GURU2MAIN_WORK *g2m = (GURU2MAIN_WORK *)mywk;
  
//  OS_Printf("g2m->comm.play_max=%d\n", g2m->comm.play_max);
//  OS_Printf("join_bit=%d\n", g2m->g2c->comm_game_join_bit);
  // �ʐM�G���[���o

  do{
    #ifdef GURU2_DEBUG_ON
    DEBUG_Proc( g2m );
    #endif
    ret = DATA_Guru2ProcTbl[g2m->seq_no]( g2m );
  }while( ret == RET_CONT );
  
  if( ret == RET_END ){
    return( GFL_PROC_RES_FINISH );
  }
  
  Disc_Update( g2m );
  EggAct_Update( g2m );
  
  guru2_DrawProc( g2m );
  guru2Main_FrameWorkClear( g2m );

  // �^�X�N���C��
  GFL_TCB_Main( g2m->tcbSys );

  // �t�F�[�h������Ȃ����ɒʐM�G���[�`�F�b�N
  if( WIPE_SYS_EndCheck() ){
    if(NetErr_App_CheckError()!=NET_ERR_STATUS_NULL){
      OS_Printf("------------------------�ʐM�G���[--------------------\n");
      return ( GFL_PROC_RES_FINISH );
    }
  }
  return( GFL_PROC_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * 1�t���[�����ɃN���A���郏�[�N
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void guru2Main_FrameWorkClear( GURU2MAIN_WORK *g2m )
{
  g2m->comm.push_btn = 0;
  g2m->comm.game_data_send_flag = 0;
}

//==============================================================================
//  ���邮������@���C������
//==============================================================================
//--------------------------------------------------------------
/**
 * ���C���@������
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_Init( GURU2MAIN_WORK *g2m )
{
  Guru2TalkWin_Write( g2m, MSG_WAIT );
  g2m->seq_no = SEQNO_MAIN_FADEIN_WAIT;
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C�� �e�q���� �t�F�[�h�C���҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_FadeInWait( GURU2MAIN_WORK *g2m )
{
  if( WIPE_SYS_EndCheck() ){
    if( GFL_NET_SystemGetCurrentID() == 0 ){
      OS_Printf("�e�Ƃ��ē���\n");
      g2m->seq_no = SEQNO_MAIN_OYA_SIGNAL_JOIN_WAIT;
    }else{
      OS_Printf("�q�Ƃ��ē���\n");
      g2m->seq_no = SEQNO_MAIN_KO_SEND_SIGNAL_JOIN;
    }

    // ���ԃA�C�R���\��
    g2m->TimeIconWork = TIMEICON_Create( g2m->tcbSys, g2m->msgwork.bmpwin_talk, 15, 
                                            TIMEICON_DEFAULT_WAIT, HEAPID_GURU2 );
    
    return( RET_CONT );
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�e�@�Q���҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_OyaSignalJoinWait( GURU2MAIN_WORK *g2m )
{
  if( Guru2MainCommJoinNumCheck(g2m) ){   //�Q�����`�F�b�N
    #ifdef DEBUG_GURU2_PRINTF
    OS_Printf( "���邮�� �e �Q���҂� �S���Q��\n" );
    #endif
    g2m->seq_no = SEQNO_MAIN_OYA_SEND_JOIN_CLOSE;
    return( RET_CONT );
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�e�@�Q�����ߐ؂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_OyaSendJoinClose( GURU2MAIN_WORK *g2m )
{
  u16 bit = G2COMM_GMSBIT_JOIN_CLOSE;
  
  if( Guru2Comm_SendData( g2m->g2c, G2COMM_GM_SIGNAL,&bit,2) == TRUE ){
    #ifdef DEBUG_GURU2_PRINTF
    OS_Printf( "���邮�� �e �Q�����ߐ؂�\n" );
    #endif
    g2m->seq_no = SEQNO_MAIN_OYA_CONNECT_NUM_CHECK;
    return( RET_CONT );
  }
  
  return( RET_NON );
}

//----------------------------------------------------------------------------------
/**
 * @brief ���j�I�����[���ʐM���[�N�|�C���^�擾
 *
 * @param   wk    
 *
 * @retval  UNION_APP_PTR   
 */
//----------------------------------------------------------------------------------
static UNION_APP_PTR _get_unionwork(GURU2MAIN_WORK *wk)
{
//  OS_Printf("union app adr=%08x\n",(u32)wk->g2p->param.uniapp);
  return wk->g2p->param->uniapp;
}

//--------------------------------------------------------------
/**
 * ���C���@�e�@�ڑ��l����v�҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_OyaConnectNumCheck( GURU2MAIN_WORK *g2m )
{
  int count = Guru2MainCommJoinNumGet( g2m ) + 1; //+1=���g
  
  if( count != Union_App_GetMemberNum(_get_unionwork(g2m)) ){
    return( RET_NON );
  }
  
  g2m->seq_no = SEQNO_MAIN_OYA_SEND_PLAY_MAX;
  return( RET_CONT );
}

//--------------------------------------------------------------
/**
 * ���C���@�e�@�v���C�l���ő��]��
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_OyaSendPlayMax( GURU2MAIN_WORK *g2m )
{
  int ret;
  
  g2m->comm.play_max = Guru2MainCommJoinNumGet(g2m)+1; //+1=���g
//  g2m->comm.play_max = Union_App_GetMemberNum(_get_unionwork(g2m));
  
  ret = Guru2Comm_SendData(
    g2m->g2c, G2COMM_GM_PLAYMAX, &g2m->comm.play_max, 4 );
  
  if( ret == TRUE ){
    g2m->play_send_count = 0;
    g2m->seq_no = SEQNO_MAIN_OYA_SEND_PLAY_NO;
    #ifdef DEBUG_GURU2_PRINTF_FORCE
    OS_Printf( "���邮�� �Q���� max=%d\n", g2m->comm.play_max );
    #endif
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�e�@�Q�[���ԍ��]��
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_OyaSendPlayNo( GURU2MAIN_WORK *g2m )
{
  int ret;
  GURU2COMM_PLAYNO play;
  
  if( g2m->play_send_count == 0 ){  //���� �e
    play.comm_id = 0;
    play.play_no = 0;
    
    ret = Guru2Comm_SendData(g2m->g2c,
        G2COMM_GM_PLAYNO,&play,sizeof(GURU2COMM_PLAYNO) );
    
    if( ret == TRUE ){
      g2m->play_send_count++;
      
      #ifdef DEBUG_GURU2_PRINTF_FORCE
      OS_Printf( "���邮�� �q�֐e�v���C�ԍ�(%d)�]��\n",
        play.comm_id, play.play_no );
      #endif
    }
    
    return( RET_NON );
  }
  
  { //�q�v���C�f�[�^���M
    int id = 1, count = 1;
    u32 join = g2m->g2c->comm_game_join_bit;
    
    do{
      if( (join & (1<<id)) ){
        if( count >= g2m->play_send_count ){
          play.comm_id = id;
          play.play_no = g2m->play_send_count;
          ret = Guru2Comm_SendData(g2m->g2c,
            G2COMM_GM_PLAYNO,&play,sizeof(GURU2COMM_PLAYNO) );
          
          if( ret == TRUE ){
            g2m->play_send_count++;
            #ifdef DEBUG_GURU2_PRINTF_FORCE
            OS_Printf( "���邮�� �q(ID:%d)��", id );
            OS_Printf( "�v���C�ԍ��]�� no=%d\n", count );
            #endif
          }
          
          break;
        }
        
        count++;
      }
      
      id++;
    }while( id < G2MEMBER_MAX );
  }
  
  if( g2m->play_send_count >= g2m->comm.play_max ){
    g2m->seq_no = SEQNO_MAIN_OYA_SIGNAL_EGG_ADD_START;
  }
  
#if 0
  0 1 2 3 4 5
  1 2 3 4 5
#endif
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�e�@�^�}�S�ǉ��J�n�V�O�i���]��
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_OyaSignalEggAddStart( GURU2MAIN_WORK *g2m )
{
  u16 bit = G2COMM_GMSBIT_EGG_ADD_START;
  
  if( Guru2Comm_SendData(g2m->g2c,G2COMM_GM_SIGNAL,&bit,2) == TRUE ){
    g2m->seq_no = SEQNO_MAIN_EGG_DATA_SEND_INIT; //�^�}�S�ǉ���
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�q�@�Q���V�O�i�����M
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_KoSendSignalJoin( GURU2MAIN_WORK *g2m )
{
  int ret;
  
  ret = Guru2Comm_SendData( g2m->g2c, G2COMM_GM_JOIN, NULL, 0 );
  
  if( ret == TRUE ){
    g2m->comm_wait_frame = 0; //�G���[���Ԃ���U�N���A
    g2m->seq_no = SEQNO_MAIN_KO_EGG_ADD_START_WAIT;
    return( RET_NON );
  }
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�q�@�^�}�S�ǉ��J�n�҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_KoEggAddStartWait( GURU2MAIN_WORK *g2m )
{
  if( Guru2MainCommSignalCheck(g2m,G2COMM_GMSBIT_EGG_ADD_START) == TRUE ){
//    GF_ASSERT( g2m->comm.play_max >= 2 );
//    GF_ASSERT( g2m->comm.my_play_no != 0 );
    g2m->comm.play_max = Guru2MainCommJoinNumGet(g2m)+1;
    g2m->seq_no = SEQNO_MAIN_EGG_DATA_SEND_INIT;
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���܂��f�[�^�]���@������
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EggDataSendInit( GURU2MAIN_WORK *g2m )
{
  GFL_NETHANDLE* pNet = GFL_NET_HANDLE_GetCurrentHandle();
  #ifdef DEBUG_GURU2_PRINTF
  OS_Printf( "���邮��@���܂��f�[�^�]���J�n\n" );
  #endif

  TIMEICON_Exit( g2m->TimeIconWork );
  g2m->TimeIconWork=NULL;
  
  GFL_NET_HANDLE_TimeSyncStart( pNet, COMM_GURU2_TIMINGSYNC_NO, WB_NET_GURUGURU );
  g2m->seq_no = SEQNO_MAIN_EGG_DATA_SEND_TIMING_WAIT;
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���܂��f�[�^�]���@�����҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EggDataSendTimingWait( GURU2MAIN_WORK *g2m )
{
  GFL_NETHANDLE* pNet = GFL_NET_HANDLE_GetCurrentHandle();

  if( GFL_NET_HANDLE_IsTimeSync( pNet, COMM_GURU2_TIMINGSYNC_NO, WB_NET_GURUGURU) ){
    g2m->seq_no = SEQNO_MAIN_EGG_DATA_TRADE_POS_SEND;
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�^�}�S�ʒu�𑗐M
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EggDataTradePosSend( GURU2MAIN_WORK *g2m )
{
  if( Guru2Comm_SendData(
      g2m->g2c,G2COMM_GM_TRADE_POS,&g2m->g2p->trade_no,4) ){
    g2m->seq_no = SEQNO_MAIN_EGG_DATA_SEND;
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���܂��f�[�^�]��
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EggDataSend( GURU2MAIN_WORK *g2m )
{
  GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();
  BOOL ret;

//    ret=GFL_NET_SendData( pNet, G2COMM_GM_SEND_EGG_DATA, 
//                        GURU2_WIDEUSE_SENDWORK_SIZE, (void*)g2m->my_poke_party );
  ret=GFL_NET_SendDataExBit( pNet, Union_App_GetMemberNetBit(_get_unionwork(g2m)), 
                             G2COMM_GM_SEND_EGG_DATA, POKEPARTY_SEND_ONCE_SIZE, 
                             (void*)g2m->my_poke_party, TRUE, TRUE, TRUE);

  OS_Printf("my_pokepartyt adr = %08x\n", (u32)g2m->my_poke_party);

  if( ret ){
    g2m->seq_no = SEQNO_MAIN_EGG_DATA_RECV_WAIT;
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���܂��f�[�^��M�҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EggDataRecvWait( GURU2MAIN_WORK *g2m )
{
  OS_Printf("�q�@:play_max = %d\n", g2m->comm.play_max);
  

  if( Guru2MainCommEggDataNumGet(g2m) == g2m->comm.play_max ){
    //�^�}�S�f�[�^��M�����@�_���^�}�S�`�F�b�N
    if( Guru2MainDameTamagoCheck(g2m) == TRUE ){
      Guru2Comm_SendData( g2m->g2c, G2COMM_GM_EGG_DATA_NG, NULL, 0 );
      g2m->seq_no = SEQNO_MAIN_COMM_ERROR_DAME_TAMAGO_MSG;
      #ifdef DEBUG_GURU2_PRINTF
      OS_Printf( "�_���^�}�S�𔭌�\n" );
      #endif
    }else{
      Guru2Comm_SendData( g2m->g2c, G2COMM_GM_EGG_DATA_OK, NULL, 0 );
      g2m->seq_no = SEQNO_MAIN_EGG_DATA_CHECK_WAIT;
      #ifdef DEBUG_GURU2_PRINTF
      OS_Printf( "���邮��@�^�}�S�f�[�^��M����\n" );
      #endif
    }
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�S���̗��`�F�b�N�I���҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_DameTamagoCheckWait( GURU2MAIN_WORK *g2m )
{
  if( Guru2MainCommEggDataNGCheck(g2m) == TRUE ){
    g2m->seq_no = SEQNO_MAIN_COMM_ERROR_DAME_TAMAGO_MSG;
  }else if( Guru2MainCommEggDataOKCountCheck(g2m) == g2m->comm.play_max ){
    g2m->seq_no = SEQNO_MAIN_EGG_ADD_INIT;
  }

  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���ǉ�������
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EggAddInit( GURU2MAIN_WORK *g2m )
{
  int i;
  EGGADDWORK *wk = Guru2MainTempWorkInit( g2m, sizeof(EGGADDWORK) );

#ifdef PM_DEBUG
  OS_TPrintf("*** �����O ***\n");
  print_all_egg_data( g2m );
#endif

  wk->no = g2m->comm.my_play_no;  //���g�̗�����ǉ�
  wk->max = g2m->comm.play_max;
  wk->offs = DATA_DiscOffsetAngle[wk->max][g2m->comm.my_play_no];
  
  for( i = 0; i < NET_GURU2_CONNECT_MAX; i++ ){
    if( Guru2MainCommIDPlayCheck(g2m,i) ){
      g2m->comm.my_status[i] = Union_App_GetMystatus( _get_unionwork(g2m), i );
      MyStatus_CopyNameString(
        g2m->comm.my_status[i], g2m->comm.my_name_buf[i] );
    }
  }
  
  // �ʐM�F�B�o�^����
  _comm_friend_func(g2m);
  
  g2m->disc.rotate_offs_fx = NUM_FX32( wk->offs );
  g2m->seq_no = SEQNO_MAIN_EGG_ADD;
  
  //BGM�؂�ւ�
//  Snd_DataSetByScene( SND_SCENE_GURUGURU, SEQ_GS_GURUGURU, 1 );
  PMSND_PlayBGM( SEQ_BGM_GURUGURU );
  g2m->g2p->param->bgm_change = TRUE;
  
  Guru2TalkWin_Clear( g2m );
  return( RET_CONT );
}

//--------------------------------------------------------------
/**
 * ���C���@���ǉ�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EggAdd( GURU2MAIN_WORK *g2m )
{
  g2m->frame--;
  
  if( g2m->frame > 0 ){
    return( RET_NON );
  }
  
  g2m->frame = EGG_ADD_NEXT_FRAME;
  
  {
    EGGADDWORK *wk = Guru2MainTempWorkGet( g2m );
    u16 angle = DATA_EggStartAngle[wk->max][wk->no];
    int id = g2m->comm.play_no_tbl[wk->no].comm_id;
    
    // �ʐM�f�[�^��������
    g2m->comm.game_data.egg_angle[wk->no] = angle;

    EggAct_StartTcbSet( g2m, id, wk->no, wk->count,
        angle, wk->offs, &g2m->egg.eact[wk->no] );
    
    wk->no++;
    wk->no %= wk->max; 
    wk->count++;              //�^�}�S�ǉ��J�E���g
    
    if( wk->count == wk->max ){
      g2m->frame = 0;
      g2m->seq_no = SEQNO_MAIN_EGG_ADD_WAIT;
    }
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���ǉ��҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EggAddWait( GURU2MAIN_WORK *g2m )
{
  EGGWORK *egg = &g2m->egg;
  int i = 0,count = 0,max = g2m->comm.play_max;
  
  do{
    if( egg->eact[i].set_flag == TRUE ){
      count++;
    }
    
    i++;
  }while( i < max );
  
  if( count == max ){
    g2m->seq_no = SEQNO_MAIN_EGG_ADD_END_WAIT;
    return( RET_CONT );
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���ǉ���̈ꐡ�҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EggAddEndWait( GURU2MAIN_WORK *g2m )
{
  g2m->frame++;
  
  if( g2m->frame > EGG_ADD_END_WAIT ){
    g2m->frame = 0;
    
    if( GFL_NET_SystemGetCurrentID() == 0 ){
      g2m->seq_no = SEQNO_MAIN_SEND_GAME_START_FLAG;
    }else{
      g2m->seq_no = SEQNO_MAIN_RECV_GAME_START_FLAG;
    }
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�e�@�Q�[���J�n�t���O�]��
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_SendGameStartFlag( GURU2MAIN_WORK *g2m )
{
  u16 bit = G2COMM_GMSBIT_GAME_START;
  
  if( Guru2Comm_SendData(g2m->g2c,G2COMM_GM_SIGNAL,&bit,2) == TRUE ){
    #ifdef DEBUG_DISP_CHECK
    g2m->seq_no = SEQNO_MAIN_DEBUG_DISP_CHECK;
    #else
    g2m->seq_no = SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_INIT;
    #endif
  }
  
  return( RET_NON );
}

//----------------------------------------------------------------------------------
/**
 * @brief ���邮��������s���������o�[��ʐM�F�B�Ƃ��ēo�^
 *
 * @param   g2m   
 */
//----------------------------------------------------------------------------------
static void _comm_friend_func( GURU2MAIN_WORK *g2m )
{
  ETC_SAVE_WORK  *etc_save  = SaveData_GetEtc( 
                                GAMEDATA_GetSaveControlWork(g2m->g2p->param->gamedata));
  int i;
  for(i=0;i<G2MEMBER_MAX;i++){
    if( g2m->comm.my_status[i]!=NULL ){   // �ڑ��ł��Ă��āA
      if(i!=GFL_NET_SystemGetCurrentID()){  // �����̒ʐMID����Ȃ����
        EtcSave_SetAcquaintance( etc_save, MyStatus_GetID(g2m->comm.my_status[i]));
        OS_Printf("id=%d��F�B�o�^\n", i);
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * ���C���@�q�@�Q�[���J�n�t���O�҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_RecvGameStartFlag( GURU2MAIN_WORK *g2m )
{
  if( Guru2MainCommSignalCheck(g2m,G2COMM_GMSBIT_GAME_START) == TRUE ){
    #ifdef DEBUG_DISP_CHECK
    {
      int i;
      for( i = 0; i < g2m->comm.play_max; i++ ){
        Guru2NameWin_Clear( g2m, i );
      }
      BtnFadeTcbAdd( g2m, TRUE );
      g2m->seq_no = SEQNO_MAIN_DEBUG_DISP_CHECK;
    }
    #else
    g2m->seq_no = SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_INIT;
    #endif
    return( RET_CONT );
  }
  
  return( RET_NON );
}

#ifdef DEBUG_DISP_CHECK
//--------------------------------------------------------------
/**
 * ���C���@�q�@�f�o�b�O�@��ʃe�X�g
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET GURU2RET
 */
//--------------------------------------------------------------
static RET DEBUG_Guru2Subproc_DispCheck( GURU2MAIN_WORK *g2m )
{
  if( BtnAnmTcb_PushCheck(g2m) == FALSE ){
    if( Guru2TP_ButtonHitTrgCheck(g2m) == TRUE ){
      BtnAnmTcb_PushSet( g2m );
    }
  }
  
  if( Guru2Pad_ContCheck(PAD_BUTTON_START) ){
    g2m->seq_no = SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_INIT;
    return( RET_CONT );
  }
  
  return( RET_NON );
}
#endif

//--------------------------------------------------------------
/**
 * ���C���@�J�E���g�_�E�����O�̓����J�n
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_CountDownBeforeTimingInit( GURU2MAIN_WORK *g2m )
{
  GFL_NETHANDLE* pNet = GFL_NET_HANDLE_GetCurrentHandle();
  GFL_NET_HANDLE_TimeSyncStart( pNet, COMM_GURU2_TIMINGSYNC_NO, WB_NET_GURUGURU );
  g2m->seq_no = SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_WAIT;
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�J�E���g�_�E�����O�̓����҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_CountDownBeforeTimingWait( GURU2MAIN_WORK *g2m )
{
  GFL_NETHANDLE* pNet = GFL_NET_HANDLE_GetCurrentHandle();
  if( GFL_NET_HANDLE_IsTimeSync( pNet, COMM_GURU2_TIMINGSYNC_NO, WB_NET_GURUGURU) ){
    g2m->seq_no = SEQNO_MAIN_COUNTDOWN_INIT;
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�J�E���g�_�E��������
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_CountDownInit( GURU2MAIN_WORK *g2m )
{
  BtnFadeTcbAdd( g2m, TRUE );
  guru2_CountDownTcbSet( g2m );
  
  g2m->seq_no = SEQNO_MAIN_COUNTDOWN;
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�J�E���g�_�E��
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_CountDown( GURU2MAIN_WORK *g2m )
{
  int i;
  
  g2m->frame++;
  
  if( g2m->frame < (30*3+5) ){
    return( RET_NON );
  }
  
  for( i = 0; i < g2m->comm.play_max; i++ ){
    Guru2NameWin_Clear( g2m, i );
  }
  
  g2m->frame = 0;
  g2m->seq_no = SEQNO_MAIN_GAME_INIT;
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�Q�[���@������
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_GameInit( GURU2MAIN_WORK *g2m )
{
  if( GFL_NET_SystemGetCurrentID() == 0 ){
    g2m->seq_no = SEQNO_MAIN_GAME_OYA;
  }else{
    g2m->seq_no = SEQNO_MAIN_GAME_OYA;
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�e�@�Q�[��
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_GameOya( GURU2MAIN_WORK *g2m )
{
  BOOL ret = Guru2GameTimeCount( g2m );
    
  if( ret == TRUE && EggDiscJumpTcb_JumpProcCheck(g2m) == FALSE && g2m->comm.game_data.egg_joffs == 0 ){
    g2m->end_flag = TRUE;
  }

  if( g2m->comm.game_data.end_flag == TRUE )
  {
    g2m->seq_no = SEQNO_MAIN_GAME_END_INIT;
    return( RET_CONT );
  }
  
  if( g2m->comm.push_btn ){
    if( EggDiscJumpTcb_JumpProcCheck(g2m) == FALSE ){
      EggDiscJumpTcb_JumpSet( g2m );
    }
  }
  
  if( ret == FALSE && BtnAnmTcb_PushCheck(g2m) == FALSE ){
    if( Guru2TP_ButtonHitTrgCheck(g2m) == TRUE ){
      if( EggDiscJumpTcb_JumpProcCheck(g2m) == FALSE ){
//        EggDiscJumpTcb_JumpSet( g2m );
        g2m->comm.send_btn = TRUE;
      }
      
      BtnAnmTcb_PushSet( g2m );
    }
  }
  
  if( g2m->comm.send_btn ){
    if( Guru2Comm_SendData(g2m->g2p->g2c,
        G2COMM_GM_BTN,&g2m->comm.send_btn,1) == TRUE ){
      g2m->comm.send_btn = 0;
    }
  }
  
  { //�M��
    fx32 speed;
    DISCWORK *disc = &g2m->disc;
      
    if( GFL_NET_SystemGetCurrentID() == 0 )
    {
      if( g2m->game_frame < GURU2_GAME_FRAME_H ){
        disc->speed_fx += DISC_ACCEL_FX;
        if( disc->speed_fx >= DISC_TOP_SPEED_FX ){
          disc->speed_fx = DISC_TOP_SPEED_FX;
        }
      }else{
        disc->speed_fx -= DISC_ACCEL_FX;
        if( disc->speed_fx < DISC_LOW_SPEED_FX ){
          disc->speed_fx = DISC_LOW_SPEED_FX;
        }
      }
    }
  }
  
  // �e�̂ݑ��M
  if( GFL_NET_SystemGetCurrentID() == 0 )
  {
    int i;
    GURU2COMM_GAMEDATA data;
    
    data.disc_angle = ( g2m->disc.rotate_fx );
    data.end_flag = g2m->end_flag;
    data.egg_joffs = g2m->egg.eact[0].joffs;

    for( i=0; i<g2m->comm.play_max; i++ )
    {
      EGGWORK* egg = &g2m->egg;
      data.egg_angle[i] = FX32_NUM(egg->eact[i].angle);
    }
    
    Guru2Comm_SendData( g2m->g2c,
      G2COMM_GM_GAMEDATA, &data, sizeof(GURU2COMM_GAMEDATA) );
  }
    
  if( GFL_NET_SystemGetCurrentID() == 0 )
  {
    fx32 speed;

    // ��]
    speed = g2m->disc.speed_fx;
//      speed = g2m->comm.game_data.disc_speed; 
    
    if( EggDiscJumpTcb_JumpCheck(g2m) == TRUE ){
      Disc_Rotate( &g2m->disc, (speed)<<1 );
    }else{
      DiscRotateEggMove( g2m, speed );
    }
  }
  
#if 0
  {
    static int d_cnt = 0;
    OS_Printf("[%03d] send=%d push_btn=%d speed_fx=0x%x rotate_fx=0x%x comm_rotate=0x%x \n",
        d_cnt++, g2m->comm.game_data_send_flag,
        g2m->comm.push_btn,
        g2m->disc.speed_fx,
        g2m->disc.rotate_fx,
//        g2m->comm.game_data.disc_speed, 
        g2m->comm.game_data.disc_angle  );
  }
#endif
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�Q�[���I���@������
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_GameEndInit( GURU2MAIN_WORK *g2m )
{
  BtnFadeTcbAdd( g2m, FALSE );
  
  g2m->front_eggact = EggAct_FrontEggActGet( g2m );
  
  if( GFL_NET_SystemGetCurrentID() == 0 )
  {
    if( g2m->front_eggact->comm_id == GFL_NET_SystemGetCurrentID() ){
      g2m->seq_no = SEQNO_MAIN_GAME_END_ERROR_ROTATE;
    }else{
      g2m->seq_no = SEQNO_MAIN_GAME_END_LAST_ROTATE;
    }
  }
  else
  {
    g2m->seq_no =  SEQNO_MAIN_GAME_END_TIMING_INIT;
  }

  PMSND_PlaySE( GURU2_SE_TIMEUP );  

  OS_Printf("GameEndInit\n");
  return( RET_CONT );
}

//--------------------------------------------------------------
/**
 * ���C���@�Q�[���I���@���^�}�ɂ���]
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_GameEndErrorRotate( GURU2MAIN_WORK *g2m )
{
  u32 front;
  EGGACTOR *eact = g2m->front_eggact;

  // ��]����
//  DiscRotateEggMove( g2m, DISC_LOW_SPEED_FX );

  _comm_parent_rotate( g2m, DISC_LOW_SPEED_FX );

  // ���߂̃^�}�S�A�N�^�[���擾
  g2m->front_eggact = EggAct_FrontEggActGet( g2m );
  
  // ��ԋ߂��^�}�S��ID�������̒ʐMID�ƈ�����̂ł���΍Ō�̃^�[����
  if( g2m->front_eggact->comm_id != GFL_NET_SystemGetCurrentID() ){
    g2m->seq_no = SEQNO_MAIN_GAME_END_LAST_ROTATE;
  }

  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�Q�[���I���@�Ō�̉�]
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_GameEndLastRotate( GURU2MAIN_WORK *g2m )
{
  int max,no;
  fx32 angle,front,res,speed;
  EGGACTOR *eact = g2m->front_eggact;
  
  no = g2m->comm.my_play_no;
  max = g2m->comm.play_max;
  speed = DISC_LOW_SPEED_FX;
  
  angle = eact->angle;
  front = NUM_FX32( DATA_EggStartAngle[max][no] );
  
  res = front - angle;
  
  if( res < 0 ){
    res = (front + NUM_FX32(360)) - angle;
  }
  
  if( res <= DISC_LAST_RANGE_FX ){
    speed >>= 1;
  }
  
  if( res < speed ){
    speed = res;
  }
  
//  Disc_Rotate( &g2m->disc, speed );
//  EggAct_Rotate( g2m, speed );

  _comm_parent_rotate( g2m, speed );
  
  AngleAdd( &angle, speed );
  
  if( FX32_NUM(angle) == FX32_NUM(front) ){
    g2m->seq_no = SEQNO_MAIN_GAME_END_TIMING_INIT;
  }

  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�Q�[���I���@�ʐM�����J�n
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_GameEndTimingInit( GURU2MAIN_WORK *g2m )
{
  GFL_NETHANDLE* pNet = GFL_NET_HANDLE_GetCurrentHandle();
  GFL_NET_HANDLE_TimeSyncStart( pNet,  COMM_GURU2_TIMINGSYNC_NO, WB_NET_GURUGURU );
  g2m->seq_no = SEQNO_MAIN_GAME_END_TIMING_WAIT;
  OS_Printf("GameEndTimingInit\n");
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�Q�[���I���@�ʐM����
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_GameEndTimingWait( GURU2MAIN_WORK *g2m )
{
  GFL_NETHANDLE* pNet = GFL_NET_HANDLE_GetCurrentHandle();
  if( GFL_NET_HANDLE_IsTimeSync( pNet, COMM_GURU2_TIMINGSYNC_NO, WB_NET_GURUGURU) ){
    if( GFL_NET_SystemGetCurrentID() == 0 ){
      g2m->seq_no = SEQNO_MAIN_GAME_END_OYA_DATA_SEND;
    }else{
      g2m->seq_no = SEQNO_MAIN_GAME_END_KO_DATA_RECV;
    }
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�e�@�Q�[���I���@�Q�[�����ʂ𑗐M
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_OyaGameEndDataSend( GURU2MAIN_WORK *g2m )
{
  int i,ret;
  EGGACTOR *eact;
  GURU2COMM_GAMERESULT result;
  
//  result.disc_angle = FX32_NUM( g2m->disc.rotate_fx );
  result.disc_angle = FX32_NUM( g2m->comm.game_data.disc_angle );
  
  for( i = 0, g2m->omake_bit = 0; i < g2m->comm.play_max; i++ ){
    eact = &g2m->egg.eact[i];
    GF_ASSERT( eact->use_flag );
//    result.egg_angle[eact->play_no] = (u16)FX32_NUM( eact->angle );
    result.egg_angle[eact->play_no] = g2m->comm.game_data.egg_angle[i];
    
    #ifdef DEBUG_GURU2_PRINTF_FORCE
    OS_Printf( "Guru2Result OYA Egg No %d, play_no %d, comm_id %d, angle %d\n", i, eact->play_no, eact->comm_id, result.egg_angle[eact->play_no] );
    #endif

    OS_Printf("comm_angle=%d, egg_angle=%d \n", g2m->comm.game_data.egg_angle[i], FX32_NUM(eact->angle) );
    
    if( Guru2MainOmakeZoneCheck(
      g2m, NUM_FX32(g2m->comm.game_data.egg_angle[i]), g2m->comm.play_max ) == TRUE ){
//      g2m,eact->angle,g2m->comm.play_max) == TRUE ){
      g2m->omake_bit |= (1 << eact->comm_id);
    }
  }
  
  result.omake_bit = g2m->omake_bit;

  OS_Printf("omake_bit=0x%x\n", g2m->omake_bit);
  
  ret = Guru2Comm_SendData( g2m->g2c,
    G2COMM_GM_GAMERESULT, &result, sizeof(GURU2COMM_GAMERESULT) );
  
  if( ret == TRUE ){
    g2m->seq_no = SEQNO_MAIN_RESULT_INIT;
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�q�@�Q�[���I���@�e����Q�[�����ʎ󂯎��
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_KoGameEndDataRecv( GURU2MAIN_WORK *g2m )
{
  if( g2m->comm.game_result_get_flag == TRUE ){
    int i;
    EGGACTOR *eact;
    GURU2COMM_GAMERESULT *res = &g2m->comm.game_result;
    
    g2m->disc.rotate_fx = NUM_FX32( res->disc_angle );
    g2m->comm.game_data.disc_angle = g2m->disc.rotate_fx;
    
    for( i = 0; i < g2m->comm.play_max; i++ ){
      eact = &g2m->egg.eact[i];
      GF_ASSERT( eact->use_flag );
      eact->angle = NUM_FX32( res->egg_angle[eact->play_no] );
      g2m->comm.game_data.egg_angle[i] = res->egg_angle[eact->play_no];
      
      #ifdef DEBUG_GURU2_PRINTF_FORCE
      OS_Printf( "Guru2Result KO Egg No %d, play_no %d, comm_id %d, angle %d\n", i, eact->play_no, eact->comm_id, res->egg_angle[eact->play_no] );
      #endif
    }
    
    g2m->omake_bit = res->omake_bit;
  
    OS_Printf("omake_bit=0x%x\n", g2m->omake_bit);
    
    Disc_Update( g2m );   //�M�X�V
    EggAct_Update( g2m ); //�^�}�S�X�V
    g2m->front_eggact = EggAct_FrontEggActGet( g2m ); //���ʃ^�}�S�擾
    
    g2m->seq_no = SEQNO_MAIN_RESULT_INIT;
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���ʔ��\�@���O�\��
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_ResultInit( GURU2MAIN_WORK *g2m )
{
  int i,id;
  int no = g2m->front_eggact->play_no;

  for( i = 0; i < g2m->comm.play_max; i++ ){
    id = g2m->egg.eact[no].comm_id;
    Guru2NameWin_WriteIDColor( g2m, g2m->comm.my_name_buf[id], i, id );
    
    no++;
    no %= g2m->comm.play_max;
  }
  
  g2m->frame = 0;
  g2m->seq_no = SEQNO_MAIN_RESULT_NAME_WAIT;
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���ʔ��\�@���O�\���҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_ResultNameWait( GURU2MAIN_WORK *g2m )
{
  g2m->frame++;
  
  if( g2m->frame < GURU2_RESULT_NAME_WAIT_FRAME ){
    return( RET_NON );
  }
  
  g2m->frame = 0;
  
  {
    int i;
    
    for( i = 0; i < g2m->comm.play_max; i++ ){
      Guru2NameWin_Clear( g2m, i );
    }
  }
  
  {
    int id = g2m->front_eggact->comm_id;
    Guru2TalkWin_WritePlayer(
      g2m, msg_guru2_02, g2m->comm.my_status[id] );
  }
  
  { //���擾�t�@���t�@�[��
    _me_play( SEQ_ME_ITEM );
  }
  
  g2m->seq_no = SEQNO_MAIN_RESULT_MSG_WAIT;
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���ʔ��\�@���ʃ��b�Z�[�W�\���҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_ResultMsgWait( GURU2MAIN_WORK *g2m )
{
  if( g2m->frame < GURU2_RESULT_MSG_WAIT_FRAME ){
    g2m->frame++;
  }else if( _me_end_check() == FALSE ){
    g2m->frame = 0;
    g2m->seq_no = SEQNO_MAIN_OMAKE_CHECK;
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���܂��G���A �`�F�b�N
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_OmakeAreaCheck( GURU2MAIN_WORK *g2m )
{
  OmakeEggJumpTcb_OmakeBitCheckSet( g2m );
  
  if( (g2m->omake_bit & (1 << g2m->front_eggact->comm_id)) == 0 ){
    g2m->seq_no = SEQNO_MAIN_SAVE_BEFORE_TIMING_INIT;
    return( RET_NON );
  }
  
  {
    u32 id;
    BOOL ret;
    
    id = DATA_KinomiTbl[g2m->comm.play_max][0];
    id += GFUser_GetPublicRand(GFUSER_RAND_PAST_MAX) % (DATA_KinomiTbl[g2m->comm.play_max][1] - id + 1);
    
    if( id < DATA_KinomiTbl[g2m->comm.play_max][0] ){ //�O�̈�
      #ifdef DEBUG_GURU2_PRINTF       
      OS_Printf( "�؂̎��̒l����\n" );
      #endif
      id = DATA_KinomiTbl[g2m->comm.play_max][0];
    }else if( id > DATA_KinomiTbl[g2m->comm.play_max][1] ){
      #ifdef DEBUG_GURU2_PRINTF       
      OS_Printf( "�؂̎��̒l����\n" );
      #endif
      id = DATA_KinomiTbl[g2m->comm.play_max][1];
    }
    
    Guru2TalkWin_WriteItem( g2m, MSG_OMAKE_AREA, id );
    
    ret = MYITEM_AddItem(
      GAMEDATA_GetMyItem(g2m->g2p->param->gamedata), id, 1, HEAPID_GURU2 ); 
    
    _me_play( SEQ_ME_ITEM );  //�t�@���t�@�[��
    
    if( ret == TRUE ){  //����
      g2m->seq_no = SEQNO_MAIN_OMAKE_MSG_WAIT;
    }else{        //���s
      g2m->seq_no = SEQNO_MAIN_OMAKE_ERROR_MSG_START_WAIT;
    }
  }
  
  return( RET_NON );
}


//--------------------------------------------------------------
/**
 * ���C���@���܂��G���A�@�؂̎��擾��̃��b�Z�[�W�\���҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_OmakeAreaMsgWait( GURU2MAIN_WORK *g2m )
{
  if( g2m->frame < GURU2_MSG_WAIT_FRAME ){
    g2m->frame++;
  }if( _me_end_check() == FALSE ){
    g2m->frame = 0;
    g2m->seq_no = SEQNO_MAIN_SAVE_BEFORE_TIMING_INIT;
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���܂��G���A�@�؂̎��擾���s���b�Z�[�W�\���J�n
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_OmakeAreaErrorMsgStartWait( GURU2MAIN_WORK *g2m )
{
  if( g2m->frame < GURU2_MSG_WAIT_FRAME ){
    g2m->frame++;
  }else if( _me_end_check() == FALSE ){
    g2m->frame = 0;
    g2m->seq_no = SEQNO_MAIN_OMAKE_ERROR_MSG_WAIT;
    Guru2TalkWin_Write( g2m, MSG_ZANNEN );
  } 
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���܂��G���A�@�؂̎��擾���s���b�Z�[�W�\���҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_OmakeAreaErrorMsgWait( GURU2MAIN_WORK *g2m )
{
  if( g2m->frame < GURU2_MSG_WAIT_FRAME ){
    g2m->frame++;
  }else{
    g2m->frame = 0;
    g2m->seq_no = SEQNO_MAIN_SAVE_BEFORE_TIMING_INIT;
  } 
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�Z�[�u�@�Z�[�u�O�̓���������
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_SaveBeforeTimingInit( GURU2MAIN_WORK *g2m )
{
  GFL_NETHANDLE* pNet = GFL_NET_HANDLE_GetCurrentHandle();
  RECORD_Inc( g2m->g2p->param->record, RECID_GURUGURU_COUNT );
  
  Guru2TalkWin_Write( g2m, MSG_SAVE );
  GFL_NET_HANDLE_TimeSyncStart( pNet, COMM_GURU2_TIMINGSYNC_NO, WB_NET_GURUGURU );
  
  g2m->seq_no = SEQNO_MAIN_SAVE_BEFORE_TIMING_WAIT;

  //�\�t�g���Z�b�g�֎~
  GFL_UI_SoftResetDisable(GFL_UI_SOFTRESET_USER);
  
  #ifdef DEBUG_GURU2_PRINTF
  OS_Printf( "���邮������@�Z�[�u�J�n\n" );
  #endif
  return( RET_NON );
}


//--------------------------------------------------------------
/**
 * ���C���@�Z�[�u�@�Z�[�u�O�̓���
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_SaveBeforeTimingWait( GURU2MAIN_WORK *g2m )
{
  GFL_NETHANDLE* pNet = GFL_NET_HANDLE_GetCurrentHandle();
  if( GFL_NET_HANDLE_IsTimeSync( pNet, COMM_GURU2_TIMINGSYNC_NO, WB_NET_GURUGURU) ){
    Guru2MainFriendEggExchange( g2m, g2m->front_eggact->comm_id );
    // �ʐM�����Z�[�u�J�n
    g2m->NetSaveWork = NET_SAVE_Init( HEAPID_GURU2, g2m->g2p->param->gamedata);
#ifdef WINDOW_SAVE_ICON
    // ���ԃA�C�R���\��
    g2m->TimeIconWork = TIMEICON_Create( g2m->tcbSys, g2m->msgwork.bmpwin_talk, 15, 
                                            TIMEICON_DEFAULT_WAIT, HEAPID_GURU2 );

#endif
    g2m->seq_no = SEQNO_MAIN_SAVE;

#ifdef PM_DEBUG
    OS_TPrintf("*** ������ ***\n");
    print_all_egg_data( g2m );
#endif
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�Z�[�u�@�Z�[�u�f�[�^���f
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_Save( GURU2MAIN_WORK *g2m )
{
  BOOL ret = NET_SAVE_Main( g2m->NetSaveWork ); // �ʐM�����Z�[�u���C��

  if(NET_ERR_CHECK_NONE != NetErr_App_CheckError()){
    NetErr_DispCall( TRUE );
    WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
    WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
    return (RET_NON);
  }

  
  if( ret ){
#ifdef WINDOW_SAVE_ICON
  TIMEICON_Exit( g2m->TimeIconWork );
  g2m->TimeIconWork=NULL;
#endif
    NET_SAVE_Exit( g2m->NetSaveWork );          // �ʐM�����Z�[�u�I��
    g2m->seq_no = SEQNO_MAIN_END_TIMING_SYNC_INIT;
  }
  
  return( RET_NON );
}


//--------------------------------------------------------------
/**
 * ���C���@�ʐM�G���[���b�Z�[�W�@�_���^�}�S������
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_CommErrorDameTamagoMsg( GURU2MAIN_WORK *g2m )
{
  Guru2TalkWin_Write( g2m, MSG_COMM_ERROR_DAME_TAMAGO );
  g2m->seq_no = SEQNO_MAIN_MSG_WAIT_NEXT_END;
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@���b�Z�[�W�I���҂� -> �Q�[���I����
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_MsgWaitNextEnd( GURU2MAIN_WORK *g2m )
{
  g2m->frame++;
  
  if( g2m->frame >= GURU2_MSG_WAIT_FRAME ){
    g2m->frame = 0;
    g2m->seq_no = SEQNO_MAIN_END_TIMING_SYNC_INIT;
    return( RET_CONT );
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�I���@�ʐM�����҂��J�n
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EndTimingSyncInit( GURU2MAIN_WORK *g2m )
{
  GFL_NETHANDLE* pNet = GFL_NET_HANDLE_GetCurrentHandle();
  if( g2m->force_end_flag == FALSE ){
    GFL_NET_HANDLE_TimeSyncStart( pNet, COMM_GURU2_TIMINGSYNC_NO, WB_NET_GURUGURU );
    OS_Printf("�ʐM�����J�n\n");
  }
  
  Guru2TalkWin_Write( g2m, MSG_COMM_WAIT );
  OS_Printf("�ʐM�������b�Z�[�W�\��\n");

  OS_Printf("���ԃA�C�R���R���\n");
  g2m->seq_no = SEQNO_MAIN_END_TIMING_SYNC;
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�I���@�ʐM�����҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EndTimingSync( GURU2MAIN_WORK *g2m )
{
  GFL_NETHANDLE* pNet = GFL_NET_HANDLE_GetCurrentHandle();
  if( g2m->force_end_flag == FALSE ){
    OS_Printf("�ʐM������\n");
    if( GFL_NET_HANDLE_IsTimeSync( pNet, COMM_GURU2_TIMINGSYNC_NO, WB_NET_GURUGURU) ){
      // ���ԃA�C�R���\��
      g2m->TimeIconWork = TIMEICON_Create( g2m->tcbSys, g2m->msgwork.bmpwin_talk, 15, 
                                       TIMEICON_DEFAULT_WAIT, HEAPID_GURU2 );
      OS_Printf("�ʐM�ؒf�J�n\n");
      // �ʐM�R�}���h�e�[�u�����
      GFL_NET_SetNoChildErrorCheck(FALSE);
      GFL_NET_DelCommandTable( GFL_NET_CMD_GURUGURU );
      Union_App_Shutdown( _get_unionwork(g2m) );  // �ʐM�ؒf�J�n
      g2m->seq_no = SEQNO_MAIN_END_CONNECT_CHECK;
     
    }
  }else{
    g2m->frame++;
    
    if( g2m->frame >= GURU2_MSG_WAIT_FRAME ){
      g2m->frame = 0;
      
      if( g2m->g2c->comm_psel_oya_end_flag == G2C_OYA_END_FLAG_NON ){
        GFL_NET_SetAutoErrorCheck( FALSE );     //�ؒf�\��  
      }
      
      g2m->seq_no = SEQNO_MAIN_END_CONNECT_CHECK;
    }
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�I���@�ʐM�ؒf�҂�
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EndConnectCheck( GURU2MAIN_WORK *g2m )
{
  // �ʐM�ؒf�҂�
  OS_Printf("�ʐM�ؒf�҂�\n");
  if( Union_App_WaitShutdown(_get_unionwork(g2m)) ){
    TIMEICON_Exit( g2m->TimeIconWork );
    g2m->TimeIconWork=NULL;
    OS_Printf("���ԃA�C�R���폜\n");

    OS_Printf("�ʐM�ؒf�I��\n");
    g2m->seq_no = SEQNO_MAIN_END_FADEOUT_START;
    return( RET_CONT );
  }
  
  return( RET_NON );
} 

//--------------------------------------------------------------
/**
 * ���C���@�I���@�t�F�[�h�A�E�g�J�n
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EndFadeOutStart( GURU2MAIN_WORK *g2m )
{
  WIPE_SYS_Start( WIPE_PATTERN_FSAM,
    WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
    WIPE_FADE_BLACK, 8, 1, HEAPID_GURU2 );
  
  
  g2m->seq_no = SEQNO_MAIN_END_FADEOUT;
  return( RET_NON );
} 

//--------------------------------------------------------------
/**
 * ���C���@�I���@�t�F�[�h�A�E�g
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_EndFadeOut( GURU2MAIN_WORK *g2m )
{
  if( WIPE_SYS_EndCheck() ){
    g2m->seq_no = SEQNO_MAIN_END;
    return( RET_CONT );
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���C���@�I��
 * @param g2m GURU2MAIN_WORK
 * @retval  GURU2RET  GURU2RET
 */
//--------------------------------------------------------------
static RET Guru2Subproc_End( GURU2MAIN_WORK *g2m )
{
  return( RET_END );
}

//--------------------------------------------------------------
/// ���C�������e�[�u��
//--------------------------------------------------------------
static RET (* const DATA_Guru2ProcTbl[SEQNO_MAIN_MAX])( GURU2MAIN_WORK *g2m ) =
{
  Guru2Subproc_Init,                          //  SEQNO_MAIN_INIT = 0,
  Guru2Subproc_FadeInWait,                    //  SEQNO_MAIN_FADEIN_WAIT,
                                              //  
  Guru2Subproc_OyaSignalJoinWait,             //  SEQNO_MAIN_OYA_SIGNAL_JOIN_WAIT,
  Guru2Subproc_OyaSendJoinClose,              //  SEQNO_MAIN_OYA_SEND_JOIN_CLOSE,
  Guru2Subproc_OyaConnectNumCheck,            //  SEQNO_MAIN_OYA_CONNECT_NUM_CHECK,
  Guru2Subproc_OyaSendPlayMax,                //  SEQNO_MAIN_OYA_SEND_PLAY_MAX,
  Guru2Subproc_OyaSendPlayNo,                 //  SEQNO_MAIN_OYA_SEND_PLAY_NO,
  Guru2Subproc_OyaSignalEggAddStart,          //  SEQNO_MAIN_OYA_SIGNAL_EGG_ADD_START,
                                              //  
  Guru2Subproc_KoSendSignalJoin,              //  SEQNO_MAIN_KO_SEND_SIGNAL_JOIN,
  Guru2Subproc_KoEggAddStartWait,             //  SEQNO_MAIN_KO_EGG_ADD_START_WAIT,
                                              //  
  Guru2Subproc_EggDataSendInit,               //  SEQNO_MAIN_EGG_DATA_SEND_INIT,
  Guru2Subproc_EggDataSendTimingWait,         //  SEQNO_MAIN_EGG_DATA_SEND_TIMING_WAIT,
  Guru2Subproc_EggDataTradePosSend,           //  SEQNO_MAIN_EGG_DATA_TRADE_POS_SEND,
  Guru2Subproc_EggDataSend,                   //  SEQNO_MAIN_EGG_DATA_SEND,
  Guru2Subproc_EggDataRecvWait,               //  SEQNO_MAIN_EGG_DATA_RECV_WAIT,
                                              //  
  Guru2Subproc_DameTamagoCheckWait,           //  SEQNO_MAIN_EGG_DATA_CHECK_WAIT,
                                              //  
  Guru2Subproc_EggAddInit,                    //  SEQNO_MAIN_EGG_ADD_INIT,
  Guru2Subproc_EggAdd,                        //  SEQNO_MAIN_EGG_ADD,
  Guru2Subproc_EggAddWait,                    //  SEQNO_MAIN_EGG_ADD_WAIT,
  Guru2Subproc_EggAddEndWait,                 //  SEQNO_MAIN_EGG_ADD_END_WAIT,
                                              //  
  Guru2Subproc_SendGameStartFlag,             //  SEQNO_MAIN_SEND_GAME_START_FLAG,
  Guru2Subproc_RecvGameStartFlag,             //  SEQNO_MAIN_RECV_GAME_START_FLAG,
                                              //  
  #ifdef DEBUG_DISP_CHECK                     //
  DEBUG_Guru2Subproc_DispCheck,               //  SEQNO_MAIN_DEBUG_DISP_CHECK,
  #endif                                      //
                                              //  
  Guru2Subproc_CountDownBeforeTimingInit,     //  SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_INIT,
  Guru2Subproc_CountDownBeforeTimingWait,     //  SEQNO_MAIN_COUNTDOWN_BEFORE_TIMING_WAIT,
  Guru2Subproc_CountDownInit,                 //  SEQNO_MAIN_COUNTDOWN_INIT,
  Guru2Subproc_CountDown,                     //  SEQNO_MAIN_COUNTDOWN,
                                              //  
  Guru2Subproc_GameInit,                      //  SEQNO_MAIN_GAME_INIT,
  Guru2Subproc_GameOya,                       //  SEQNO_MAIN_GAME_OYA,
                                              //  
  Guru2Subproc_GameEndInit,                   //  SEQNO_MAIN_GAME_END_INIT,
  Guru2Subproc_GameEndErrorRotate,            //  SEQNO_MAIN_GAME_END_ERROR_ROTATE,
  Guru2Subproc_GameEndLastRotate,             //  SEQNO_MAIN_GAME_END_LAST_ROTATE,
  Guru2Subproc_GameEndTimingInit,             //  SEQNO_MAIN_GAME_END_TIMING_INIT,
  Guru2Subproc_GameEndTimingWait,             //  SEQNO_MAIN_GAME_END_TIMING_WAIT,
  Guru2Subproc_OyaGameEndDataSend,            //  SEQNO_MAIN_GAME_END_OYA_DATA_SEND,
  Guru2Subproc_KoGameEndDataRecv,             //  SEQNO_MAIN_GAME_END_KO_DATA_RECV,
                                              //  
  Guru2Subproc_ResultInit,                    //  SEQNO_MAIN_RESULT_INIT,
  Guru2Subproc_ResultNameWait,                //  SEQNO_MAIN_RESULT_NAME_WAIT,
  Guru2Subproc_ResultMsgWait,                 //  SEQNO_MAIN_RESULT_MSG_WAIT,
                                              //  
  Guru2Subproc_OmakeAreaCheck,                //  SEQNO_MAIN_OMAKE_CHECK,
  Guru2Subproc_OmakeAreaMsgWait,              //  SEQNO_MAIN_OMAKE_MSG_WAIT,
  Guru2Subproc_OmakeAreaErrorMsgStartWait,    //  SEQNO_MAIN_OMAKE_ERROR_MSG_START_WAIT,
  Guru2Subproc_OmakeAreaErrorMsgWait,         //  SEQNO_MAIN_OMAKE_ERROR_MSG_WAIT,
                                              //  
  Guru2Subproc_SaveBeforeTimingInit,          //  SEQNO_MAIN_SAVE_BEFORE_TIMING_INIT,
  Guru2Subproc_SaveBeforeTimingWait,          //  SEQNO_MAIN_SAVE_BEFORE_TIMING_WAIT,
  Guru2Subproc_Save,                          //  SEQNO_MAIN_SAVE,
                                              //  
  Guru2Subproc_CommErrorDameTamagoMsg,        //  SEQNO_MAIN_COMM_ERROR_DAME_TAMAGO_MSG,
                                              //  
  Guru2Subproc_MsgWaitNextEnd,                //  SEQNO_MAIN_MSG_WAIT_NEXT_END,
                                              //  
  Guru2Subproc_EndTimingSyncInit,             //  SEQNO_MAIN_END_TIMING_SYNC_INIT,
  Guru2Subproc_EndTimingSync,                 //  SEQNO_MAIN_END_TIMING_SYNC,
  Guru2Subproc_EndConnectCheck,               //  SEQNO_MAIN_END_CONNECT_CHECK,
                                              //  
  Guru2Subproc_EndFadeOutStart,               //  SEQNO_MAIN_END_FADEOUT_START,
  Guru2Subproc_EndFadeOut,                    //  SEQNO_MAIN_END_FADEOUT,
  Guru2Subproc_End,                           //  SEQNO_MAIN_END,
};                                            //  
                                                
//==============================================================================
//  VBlank
//==============================================================================
//--------------------------------------------------------------
/**
 * ���邮������@VBlank�֐�
 * @param wk    work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_VBlankFunc( GFL_TCB *tcb, void * work )
{
  GURU2MAIN_WORK *g2m = work;
  
  PaletteFadeTrans( g2m->pfd );

  GFL_CLACT_SYS_VBlankFunc();

  GFL_BG_VBlankFunc();
}

//==============================================================================
//  �O���t�B�b�N
//==============================================================================
//--------------------------------------------------------------
/**
 * �`�揉����
 * @param g2m GURU2MAIN_WORK 
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_DrawInit( GURU2MAIN_WORK *g2m )
{
  guru2_DispInit( g2m );      //��ʏ�����
  guru2_3DDrawInit( g2m );    //3D�`�揉����
//  guru2_CameraInit( g2m );    //�J����������
  guru2_BGInit( g2m );      //BG������
  guru2_PlttFadeInit( g2m );    //�p���b�g�t�F�[�h������
  guru2_ClActInit( g2m );     //�Z���A�N�^�[������
  
  guru2_BGResLoad( g2m );     //BG���\�[�X���[�h
  guru2_TalkWinFontInit( g2m ); //�t�H���g������
  guru2_ClActResLoad( g2m );    //�Z���A�N�^�[���\�[�X���[�h
  guru2_DispON( g2m );      //���ON
}

//--------------------------------------------------------------
/**
 * �`��폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_DrawDelete( GURU2MAIN_WORK *g2m )
{
  guru2_DispOFF( g2m );
  
  guru2_ClActDelete( g2m );
  guru2_TalkWinFontDelete( g2m );
  guru2_BGDelete( g2m );
  guru2_PlttFadeDelete( g2m );
  guru2_CameraDelete( g2m );
}

//--------------------------------------------------------------
/**
 * �`�揈��
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_DrawProc( GURU2MAIN_WORK *g2m )
{
  int i;
  {
    fx32 far = FX32_ONE * 4096;
    GFL_G3D_CAMERA_SetFar( g2m->camera.gf_camera, &far );
  }
  
  //----3D�`��
  GFL_G3D_DRAW_Start();
  
  //�J�����ݒ�
  GFL_G3D_CAMERA_Switching( g2m->camera.gf_camera );
  GFL_G3D_DRAW_SetLookAt();
  
  //���C�g�ݒ�
  NNS_G3dGlbLightVector( 0, 0, -FX32_ONE, 0 );
  NNS_G3dGlbLightColor( 0, GX_RGB(31,31,31) );
  NNS_G3dGlbMaterialColorDiffAmb(
    GX_RGB(31,31,31), GX_RGB(31,31,31), FALSE );
  NNS_G3dGlbMaterialColorSpecEmi(
    GX_RGB(31,31,31), GX_RGB(31,31,31), FALSE );
  
  // �w�i�`��
  Bg3D_Draw( g2m );
  
  //�M�`��
  Disc_Draw( g2m );
  //���`��
  EggAct_Draw( g2m );
  
  //�W�I���g���������_�����O�G���W���֘A�������̃X���b�v
  GFL_G3D_DRAW_End();
  
  //----2D�`��
  GFL_CLACT_SYS_Main();

  // BMP����`��
  PRINTSYS_QUE_Main( g2m->msgwork.printQue );
  PRINT_UTIL_Trans( &g2m->msgwork.printUtilTalk, g2m->msgwork.printQue );
  for(i=0;i<GURU2NAME_WIN_MAX;i++){
    PRINT_UTIL_Trans( &g2m->msgwork.printUtilName[i], g2m->msgwork.printQue );
  }
  
  // ���ԃA�C�R�����C��
  if(g2m->TimeIconWork!=NULL){
    TIMEICON_Main( g2m->TimeIconWork );
  }
}

//==============================================================================
//  ���
//==============================================================================

static const GFL_DISP_VRAM Guru2DispVramDat = {       //�f�B�X�v���C�@�o���N������
    GX_VRAM_BG_128_B,             // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,       // ���C��2D�G���W����BG�g���p���b�g
    GX_VRAM_SUB_BG_128_C,         // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
    GX_VRAM_OBJ_80_EF,            // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,      // ���C��2D�G���W����OBJ�g���p���b�g
    GX_VRAM_SUB_OBJ_16_I,         // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
    GX_VRAM_TEX_0_A,              // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_0_G,          // �e�N�X�`���p���b�g�X���b�g
    GX_OBJVRAMMODE_CHAR_1D_64K,   // ���C����OBJVRAM�T�C�Y
    GX_OBJVRAMMODE_CHAR_1D_32K,   // �T�u��OBJVRAM�T�C�Y
    
};

//--------------------------------------------------------------
/**
 * ��ʏ�����
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_DispInit( GURU2MAIN_WORK *g2m )
{
    
  GFL_DISP_SetBank( &Guru2DispVramDat );
  GFL_BMPWIN_Init( HEAPID_GURU2 );
}

//--------------------------------------------------------------
/**
 * ���ON
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_DispON( GURU2MAIN_WORK *g2m )
{

  GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );

  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
  
  // �|���S���ʂ�BG�v���C�I���e�B��ݒ�
  G2_SetBG0Priority( 1 );
  
  //�A���t�@�ύX
  G2_SetBlendAlpha(
    GX_BLEND_PLANEMASK_BG2,
    GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_BD,
    11, 10 );
}

//--------------------------------------------------------------
/**
 * ���OFF
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_DispOFF( GURU2MAIN_WORK *g2m )
{
  GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
  
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
}


//==============================================================================
//  3D�`��
//==============================================================================

#define GURU2_EDGE_COLOR  GX_RGB( EDGE_COLOR_R, EDGE_COLOR_G, EDGE_COLOR_B )

//----------------------------------------------------------------------------------
/**
 * @brief G3D�V�X�e���ɓn���RD�������֐��Q
 *
 * @param   none    
 */
//----------------------------------------------------------------------------------
static void _g3d_setup( void )
{
  // �G�b�W�}�[�L���O�p
  static  const GXRgb edge_color_table[8]=
  { GURU2_EDGE_COLOR, GURU2_EDGE_COLOR, GURU2_EDGE_COLOR, GURU2_EDGE_COLOR, 
    GURU2_EDGE_COLOR, GURU2_EDGE_COLOR, GURU2_EDGE_COLOR, GURU2_EDGE_COLOR, };

  G3X_SetShading( GX_SHADING_TOON );  //�V�F�[�h
  G3X_AntiAlias(  TRUE );             //�A���`�G�C���A�X
  G3X_AlphaTest(  FALSE, 0 );         //�A���t�@�e�X�g�@
  G3X_AlphaBlend( TRUE );             //�A���t�@�u�����h
  
  G3X_EdgeMarking( TRUE );            // �G�b�W�}�[�L���O�ݒ�
  G3X_SetEdgeColorTable( edge_color_table );

  G3X_SetClearColor(                  //�N���A�J���[
    GX_RGB(CLEAR_COLOR_R,CLEAR_COLOR_G,CLEAR_COLOR_B),
    0,                    //�N���A�J���[�A���t�@�u�����h�l
    0x7fff,               //�N���A�J���[�f�v�X�l
    63,                   //�A�g���r���[�g�o�b�t�@�|���S��ID�����l
    FALSE );              //�A�g���r���[�g�o�b�t�@�t�H�OON,OFF

 
  G3_ViewPort( 0, 0, 255, 191 );       //�r���[�|�[�g

}

//--------------------------------------------------------------
/**
 * 3D�`�揉����
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_3DDrawInit( GURU2MAIN_WORK *g2m )
{
  int i;
  CAMERAWORK *cm = &g2m->camera;
    
//  NNS_G3dInit();            //3D�G���W��������
//  G3X_InitMtxStack();         //�}�g���N�X������
    //�����_�����O�G���W���ւ̃X���b�v
//  G3_SwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
    //�e�N�X�`��VRAM�}�l�[�W��������
//  GF_G3D_InitFrmTexVramManager( 1, TRUE );
    //�p���b�gVRAM�}�l�[�W��������
    //�p���b�gRAM=0x4000=16kb,VRAM�}�l�[�W�����f�t�H���g�Ŏg�p
//  GF_G3D_InitFrmPlttVramManager(0x4000, TRUE);

  GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K, GFL_G3D_VMANLNK, 
                GFL_G3D_PLT16K,  0, HEAPID_GURU2, _g3d_setup );
  
  //GF_G3_RequestSwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);  
  GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
  

  //�J�����ݒ�
  { 
    static const VecFx32 cam_pos    = { 0,0x30*FX32_ONE,0x70*FX32_ONE};
    static const VecFx32 cam_target = { CAMERA_TARGET_X,CAMERA_TARGET_Y,CAMERA_TARGET_Z};
    cm->gf_camera = GFL_G3D_CAMERA_CreateDefault( &cam_pos, &cam_target, HEAPID_GURU2 );
    
    GFL_G3D_CAMERA_Switching( cm->gf_camera );
  }

}



//==============================================================================
//  BG
//==============================================================================
//--------------------------------------------------------------
/**
 * BG������
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_BGInit( GURU2MAIN_WORK *g2m )
{
  GFL_BG_Init( HEAPID_GURU2 );

  GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );
  
  { //BG������
    GFL_BG_SYS_HEADER bg_head = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D };
    GFL_BG_SetBGMode( &bg_head );
  }
  
  { //main BG0 �|���S����
  }
  
  { //main BG1  ��b�E�B���h�E
    GFL_BG_BGCNT_HEADER bg_cnt_header = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000,
      GFL_BG_CHRSIZ_256x128,GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(
       GFL_BG_FRAME1_M, &bg_cnt_header, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME1_M );
    GFL_BG_SetClearCharacter( GFL_BG_FRAME1_M, 32, 0, HEAPID_GURU2 );
  }
    
  { //main BG2  �w�i����1
    GFL_BG_BGCNT_HEADER bg_cnt_header = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000,
      GFL_BG_CHRSIZ_256x128,GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(
       GFL_BG_FRAME2_M, &bg_cnt_header, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME2_M );
  }
  
  { //main BG3 �w�i���̂Q
    GFL_BG_BGCNT_HEADER bg_cnt_header = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000,
      GFL_BG_CHRSIZ_256x128,GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &bg_cnt_header, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME3_M );
  }
  
  { //sub BG2�@�{�^��
    GFL_BG_BGCNT_HEADER bg_cnt_header = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01,
      GFL_BG_CHRSIZ_256x128,2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(
       GFL_BG_FRAME2_S, &bg_cnt_header, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME2_S );
  }
  
  { //sub BG3�@�{�^���w�i
    GFL_BG_BGCNT_HEADER bg_cnt_header = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x1c000, GX_BG_EXTPLTT_01,
      GFL_BG_CHRSIZ_256x128,3, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &bg_cnt_header, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME3_S );
  }
}

//--------------------------------------------------------------
/**
 * BG�O���t�B�b�N���\�[�X���[�h
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_BGResLoad( GURU2MAIN_WORK *g2m )
{
  void *buf;
  
  //BG�p���b�g main
  buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_bg_NCLR, FALSE );
  NNS_G2dGetUnpackedPaletteData( buf, &g2m->bg_pPltt );
  PaletteWorkSet( g2m->pfd, g2m->bg_pPltt->pRawData,
    FADE_MAIN_BG, BGF_PLTT_M_OFFS, BGF_PLTT_M_SIZE );
  GFL_HEAP_FreeMemory( buf );
  
  //BG�p���b�g sub
  buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_sub_NCLR, FALSE );
  NNS_G2dGetUnpackedPaletteData( buf, &g2m->bg_pPltt );
  PaletteWorkSet( g2m->pfd, g2m->bg_pPltt->pRawData,
    FADE_SUB_BG, BGF_PLTT_S_OFFS, BGF_PLTT_S_SIZE );
  GFL_HEAP_FreeMemory( buf );
  
  //BG�p���b�g sub ���O�ɈÂ����Ă���
  ColorConceChangePfd( g2m->pfd, FADE_SUB_BG, 0xffff, 8, 0 );
  
  //BG�L���� MAIN BG3
  buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_bg_NCGR, FALSE );
  NNS_G2dGetUnpackedCharacterData( buf, &g2m->bg_pChar );
  GFL_BG_LoadCharacter(  GFL_BG_FRAME3_M,
    g2m->bg_pChar->pRawData, g2m->bg_pChar->szByte, BGF_BG3_M_CHAR_OFFS );
  GFL_HEAP_FreeMemory( buf );
  
  //BG�L���� SUB BG2
  buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_but_NCGR, FALSE );
  NNS_G2dGetUnpackedCharacterData( buf, &g2m->bg_pChar );
  GFL_BG_LoadCharacter(  GFL_BG_FRAME2_S,
    g2m->bg_pChar->pRawData, g2m->bg_pChar->szByte, BGF_BG2_S_CHAR_OFFS );
  GFL_HEAP_FreeMemory( buf );
  
  //BG�L���� SUB BG3
  buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_sub_bg_NCGR, FALSE );
  NNS_G2dGetUnpackedCharacterData( buf, &g2m->bg_pChar );
  GFL_BG_LoadCharacter(  GFL_BG_FRAME3_S,
    g2m->bg_pChar->pRawData, g2m->bg_pChar->szByte, BGF_BG3_S_CHAR_OFFS );
  GFL_HEAP_FreeMemory( buf );
  
  //BG�X�N���[�� MAIN BG2
  buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_bg1_NSCR, FALSE );
  NNS_G2dGetUnpackedScreenData( buf, &g2m->bg_pScr );
  GFL_BG_LoadScreenBuffer(  GFL_BG_FRAME2_M,
    (void*)g2m->bg_pScr->rawData, g2m->bg_pScr->szByte );
  GFL_BG_LoadScreenReq(  GFL_BG_FRAME2_M );
  GFL_HEAP_FreeMemory( buf );
  
  //BG�X�N���[�� MAIN BG3
  buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_bg0_NSCR, FALSE );
  NNS_G2dGetUnpackedScreenData( buf, &g2m->bg_pScr );
  GFL_BG_LoadScreenBuffer(  GFL_BG_FRAME3_M,
    (void*)g2m->bg_pScr->rawData, g2m->bg_pScr->szByte );
  GFL_BG_LoadScreenReq(  GFL_BG_FRAME3_M );
  GFL_HEAP_FreeMemory( buf );
  
  //BG�X�N���[�� SUB BG2
  buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_but0_NSCR, FALSE );
  NNS_G2dGetUnpackedScreenData( buf, &g2m->bg_pScr );
  GFL_BG_LoadScreenBuffer(  GFL_BG_FRAME2_S,
    (void*)g2m->bg_pScr->rawData, g2m->bg_pScr->szByte );
  GFL_BG_LoadScreenReq(  GFL_BG_FRAME2_S );
  GFL_HEAP_FreeMemory( buf );
  
  //BG�X�N���[�� SUB BG3
  buf = Guru2Arc_DataLoad( g2m, NARC_guru2_guruguru_sub_bg_NSCR, FALSE );
  NNS_G2dGetUnpackedScreenData( buf, &g2m->bg_pScr );
  GFL_BG_LoadScreenBuffer(  GFL_BG_FRAME3_S,
    (void*)g2m->bg_pScr->rawData, g2m->bg_pScr->szByte );
  GFL_BG_LoadScreenReq(  GFL_BG_FRAME3_S );
  GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------
/**
 * BG�R���g���[���폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_BGDelete( GURU2MAIN_WORK *g2m )
{
  GFL_BMPWIN_Exit();

  GFL_BG_FreeBGControl(  GFL_BG_FRAME1_M );
  GFL_BG_FreeBGControl(  GFL_BG_FRAME2_M );
  GFL_BG_FreeBGControl(  GFL_BG_FRAME3_M );
  GFL_BG_FreeBGControl(  GFL_BG_FRAME2_S );
  GFL_BG_FreeBGControl(  GFL_BG_FRAME3_S );
  GFL_BG_Exit();
}

//--------------------------------------------------------------
/**
 * BG SUB �{�^���X�N���[����]��
 * @param g2m GURU2MAIN_WORK
 * @param idx �{�^���C���f�b�N�X
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2BG_ButtonScreenSet( GURU2MAIN_WORK *g2m, u32 idx )
{
  void *buf;
  
  buf = Guru2Arc_DataLoad( g2m, idx, FALSE );
  NNS_G2dGetUnpackedScreenData( buf, &g2m->bg_pScr );
  GFL_BG_LoadScreenBuffer(  GFL_BG_FRAME2_S,
    (void*)g2m->bg_pScr->rawData, g2m->bg_pScr->szByte );
  GFL_BG_LoadScreenReq(  GFL_BG_FRAME2_S );
  GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------
/**
 * BG�@�{�^���X�N���[�� ON��]��
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2BG_ButtonONSet( GURU2MAIN_WORK *g2m )
{
  Guru2BG_ButtonScreenSet( g2m, NARC_guru2_guruguru_but2_NSCR );
}

//--------------------------------------------------------------
/**
 * BG�@�{�^���X�N���[�� OFF��]��
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2BG_ButtonOFFSet( GURU2MAIN_WORK *g2m )
{
  Guru2BG_ButtonScreenSet( g2m, NARC_guru2_guruguru_but0_NSCR );
}

//--------------------------------------------------------------
/**
 * BG�@�{�^���X�N���[�� �����n�߂�]��
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2BG_ButtonMiddleSet( GURU2MAIN_WORK *g2m )
{
  Guru2BG_ButtonScreenSet( g2m, NARC_guru2_guruguru_but1_NSCR );
}

//==============================================================================
//  �p���b�g�t�F�[�h
//==============================================================================
//--------------------------------------------------------------
/**
 * �p���b�g�t�F�[�h������
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_PlttFadeInit( GURU2MAIN_WORK *g2m )
{
  g2m->pfd = PaletteFadeInit( HEAPID_GURU2 );
  PaletteTrans_AutoSet(g2m->pfd,TRUE);
  PaletteFadeWorkAllocSet( g2m->pfd, FADE_MAIN_BG, 0x200, HEAPID_GURU2);
  PaletteFadeWorkAllocSet( g2m->pfd, FADE_SUB_BG,  0x200, HEAPID_GURU2);
}

//--------------------------------------------------------------
/**
 * �p���b�g�t�F�[�h�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_PlttFadeDelete( GURU2MAIN_WORK *g2m )
{
  PaletteFadeWorkAllocFree( g2m->pfd, FADE_MAIN_BG  );
  PaletteFadeWorkAllocFree( g2m->pfd, FADE_SUB_BG   );
  PaletteFadeFree( g2m->pfd );  
}

//--------------------------------------------------------------
/**
 * ����ʃ{�^���@�P�x�Z�b�g
 * @param g2m GURU2MAIN_WORK
 * @param evy EVY
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2PlttFade_BtnFade( GURU2MAIN_WORK *g2m, u32 evy )
{
  ColorConceChangePfd( g2m->pfd, FADE_SUB_BG, 0xffff, evy, 0 );
}

//==============================================================================
//  �Z���A�N�^�[
//==============================================================================
//--------------------------------------------------------------
/**
 * �Z���A�N�^�[������
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_ClActInit( GURU2MAIN_WORK *g2m )
{
  // �Z���A�N�^�[�V�X�e���J�n
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, &Guru2DispVramDat, HEAPID_GURU2 );

  g2m->clUnit = GFL_CLACT_UNIT_Create( 30, 1, HEAPID_GURU2 );

}

//--------------------------------------------------------------
//
/**
 * �Z���A�N�^�[�@���\�[�X���[�h
 * @param g2m
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_ClActResLoad( GURU2MAIN_WORK *g2m )
{
  PALETTE_FADE_PTR pfd = g2m->pfd;
  ARCHANDLE *hdl = g2m->arc_handle;
  
  //�ʐM�A�C�R���p�p���b�g�̈���Ɋm��
//  {
//    CLACT_U_WmIcon_SetReserveAreaPlttManager( NNS_G2D_VRAM_TYPE_2DMAIN );
//  }
  
  //��
  { 
    ARCHANDLE *n_hdl;
    n_hdl = GFL_ARC_OpenDataHandle( ARCID_GURU2_2D, HEAPID_GURU2 );
    
    g2m->resobj[GURU2MAIN_CLACT_RES_CHR] = GFL_CLGRP_CGR_Register( 
                                              n_hdl, NARC_guru2_2d_cook_s_obj0_NCGR, 0, 
                                              CLSYS_DRAW_MAIN, HEAPID_GURU2 );

    g2m->resobj[GURU2MAIN_CLACT_RES_PLTT] =GFL_CLGRP_PLTT_Register( 
                                              n_hdl, NARC_guru2_2d_cook_s_obj0_NCLR,
                                              CLSYS_DRAW_MAIN, 0, HEAPID_GURU2 );

    g2m->resobj[GURU2MAIN_CLACT_RES_CELL] =GFL_CLGRP_CELLANIM_Register( 
                                              n_hdl, 
                                              NARC_guru2_2d_cook_s_obj0_NCER, 
                                              NARC_guru2_2d_cook_s_obj0_NANR, 
                                              HEAPID_GURU2 );
    
//    CATS_LoadResourceCharArcH( csp, crp,
//      n_hdl, NARC_guru2_2d_cook_s_obj0_ncgr, FALSE,
//      NNS_G2D_VRAM_TYPE_2DMAIN, EDID_CDOWN_NCGR );
//    CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp,
//      n_hdl, NARC_guru2_2d_cook_s_obj0_nclr,
//      FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, EDID_CDOWN_NCLR );
//    CATS_LoadResourceCellArcH( csp, crp, n_hdl,
//      NARC_guru2_2d_cook_s_obj0_ncer, FALSE, EDID_CDOWN_NCER );
//    CATS_LoadResourceCellAnmArcH( csp, crp, n_hdl,
//      NARC_guru2_2d_cook_s_obj0_nanr, FALSE, EDID_CDOWN_NANR);
    
    GFL_ARC_CloseDataHandle( n_hdl );
  }
  
  //�ʐM�A�C�R���Z�b�g
  GFL_NET_ReloadIconTopOrBottom( TRUE, HEAPID_GURU2 );

}

//--------------------------------------------------------------
/**
 * �Z���A�N�^�[�@�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_ClActDelete( GURU2MAIN_WORK *g2m )
{
//  CATS_ResourceDestructor_S( g2m->csp, g2m->crp );
//  CATS_FreeMemory( g2m->csp );

  // �L�����j��
  GFL_CLGRP_CGR_Release( g2m->resobj[GURU2MAIN_CLACT_RES_CHR] );

  // �p���b�g�j��
  GFL_CLGRP_PLTT_Release( g2m->resobj[GURU2MAIN_CLACT_RES_PLTT] );

  // �Z���E�A�j���j��
  GFL_CLGRP_CELLANIM_Release( g2m->resobj[GURU2MAIN_CLACT_RES_CELL] );

  // �Z���A�N�^�[�Z�b�g�j��
  GFL_CLACT_UNIT_Delete( g2m->clUnit );

  // �Z���A�N�^�[�V�X�e���I��
  GFL_CLACT_SYS_Delete();
}


//==============================================================================
//  �t�H���g
//==============================================================================
//--------------------------------------------------------------
/**
 * �t�H���g�@������
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_TalkWinFontInit( GURU2MAIN_WORK *g2m )
{
  int i;
  MSGWORK *msg = &g2m->msgwork;
  
  BmpWinFrame_GraphicSet(  BGF_M_KAIWA,
    BGF_CHARNO_MENU, BGF_PANO_MENU_WIN, 0, HEAPID_GURU2 );

  PaletteWorkSet_Arc( g2m->pfd,
    ARCID_FLDMAP_WINFRAME, NARC_winframe_system_NCLR,
    HEAPID_GURU2, FADE_MAIN_BG, 0x20, BGF_PANO_TALK_WIN * 16 );

  PaletteWorkSet_Arc( g2m->pfd,
    ARCID_FONT, NARC_font_default_nclr, 
    HEAPID_GURU2, FADE_MAIN_BG, 0x20, BGF_PANO_MENU_WIN * 16 );
  
  msg->font       = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr ,
                                     GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_GURU2 );
  msg->msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                               NARC_message_guru2_dat, HEAPID_GURU2 );
  
  msg->wordset = WORDSET_Create( HEAPID_GURU2 );

  // BMPWIN�m��
  msg->bmpwin_talk = GFL_BMPWIN_Create( BGF_M_KAIWA, 2, 19, 26, 4,
                                        BGF_PANO_MENU_WIN, GFL_BMP_CHRAREA_GET_B);
  
  msg->strbuf = GFL_STR_CreateBuffer( STR_BUF_SIZE, HEAPID_GURU2 );
  
  for( i = 0; i < G2MEMBER_MAX; i++ ){
    g2m->comm.my_name_buf[i] =
      GFL_STR_CreateBuffer( PERSON_NAME_SIZE+EOM_SIZE, HEAPID_GURU2 ); 
  }
  
  // PrintUtil�֘A�t��
  PRINT_UTIL_Setup( &msg->printUtilTalk, msg->bmpwin_talk );
  msg->printQue = PRINTSYS_QUE_Create( HEAPID_GURU2 );

  Guru2NameWin_Init( g2m, g2m->g2p->receipt_num );
}

//--------------------------------------------------------------
/**
 * �t�H���g�@�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_TalkWinFontDelete( GURU2MAIN_WORK *g2m )
{
  int i;
  MSGWORK *msg = &g2m->msgwork;
  
  GFL_BMPWIN_ClearTransWindow( msg->bmpwin_talk );
  GFL_BMPWIN_Delete( msg->bmpwin_talk );
  
  Guru2NameWin_Delete( g2m );
  
  GFL_MSG_Delete( msg->msgman );
  WORDSET_Delete( msg->wordset );
  GFL_FONT_Delete( msg->font );

  GFL_STR_DeleteBuffer( msg->strbuf );
  
  for( i = 0; i < G2MEMBER_MAX; i++ ){
    GFL_STR_DeleteBuffer( g2m->comm.my_name_buf[i] );
  }
  GFL_BMPWIN_Exit();


}


#define GURU2_DEFAULT_COL  (PRINTSYS_LSB_Make(1,2,0))

//--------------------------------------------------------------
/**
 * ��b�E�B���h�E�\��
 * @param g2m GURU2MAIN_WORK
 * @param msgno ���b�Z�[�W�ԍ�  
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2TalkWin_Write( GURU2MAIN_WORK *g2m, u32 msgno )
{
  MSGWORK *msg = &g2m->msgwork;
  GFL_BMPWIN *bmpwin = msg->bmpwin_talk;
  
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmpwin), BMP_COL_WHITE );
  
  BmpWinFrame_Write( bmpwin,
    WINDOW_TRANS_OFF, BGF_CHARNO_MENU, BGF_PANO_TALK_WIN );
  
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmpwin), BMP_COL_WHITE );
  
  GFL_MSG_GetString( msg->msgman, msgno, msg->strbuf );

  PRINT_UTIL_PrintColor( &msg->printUtilTalk, msg->printQue, 0,0, 
                         msg->strbuf, msg->font, GURU2_DEFAULT_COL );
  
  GFL_BMPWIN_MakeTransWindow_VBlank( bmpwin );
}

//--------------------------------------------------------------
/**
 * ��b�E�B���h�E�\���@�v���C���[�l�[���w��
 * @param g2m GURU2MAIN_WORK
 * @param msgno ���b�Z�[�W�ԍ�  
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2TalkWin_WritePlayer(
  GURU2MAIN_WORK *g2m, u32 msgno, const MYSTATUS *status )
{
  STRBUF *str;
  MSGWORK *msg = &g2m->msgwork;
  GFL_BMPWIN *bmpwin = msg->bmpwin_talk;
  
  //�����񐶐�
  WORDSET_RegisterPlayerName( msg->wordset, 1,
      Union_App_GetMystatus( _get_unionwork(g2m), GFL_NET_SystemGetCurrentID()) );
  WORDSET_RegisterPlayerName( msg->wordset, 2, status );
  
  str = GFL_STR_CreateBuffer( STR_BUF_SIZE, HEAPID_GURU2 );
  GFL_MSG_GetString( msg->msgman, msgno, str );
  WORDSET_ExpandStr( msg->wordset, msg->strbuf, str );
  GFL_STR_DeleteBuffer( str );
  
  BmpWinFrame_Write( bmpwin,
    WINDOW_TRANS_OFF, BGF_CHARNO_MENU, BGF_PANO_TALK_WIN );
  
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmpwin), BMP_COL_WHITE );
  
  PRINT_UTIL_PrintColor( &msg->printUtilTalk, msg->printQue, 0,0, 
                         msg->strbuf, msg->font, GURU2_DEFAULT_COL );
  
  GFL_BMPWIN_MakeTransWindow_VBlank( bmpwin );
}

//--------------------------------------------------------------
/**
 * ��b�E�B���h�E�\���@�A�C�e��ID�w��
 * @param g2m GURU2MAIN_WORK
 * @param msgno ���b�Z�[�W�ԍ�  
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2TalkWin_WriteItem(
    GURU2MAIN_WORK *g2m, u32 msgno, u32 id )
{
  STRBUF *str;
  MSGWORK *msg = &g2m->msgwork;
  GFL_BMPWIN *bmpwin = msg->bmpwin_talk;
  
  //�����񐶐�
  WORDSET_RegisterItemName( msg->wordset, 0, id );
  
  str = GFL_STR_CreateBuffer( STR_BUF_SIZE, HEAPID_GURU2 );
  GFL_MSG_GetString( msg->msgman, msgno, str );
  WORDSET_ExpandStr( msg->wordset, msg->strbuf, str );
  GFL_STR_DeleteBuffer( str );
  
  BmpWinFrame_Write( bmpwin,
    WINDOW_TRANS_OFF, BGF_CHARNO_MENU, BGF_PANO_TALK_WIN );
  
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmpwin), BMP_COL_WHITE );
  
  PRINT_UTIL_PrintColor( &msg->printUtilTalk, msg->printQue, 0,0, 
                         msg->strbuf, msg->font, GURU2_DEFAULT_COL );
  
  GFL_BMPWIN_MakeTransWindow_VBlank( bmpwin );
}

//--------------------------------------------------------------
/**
 * ��b�E�B���h�E�N���A
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2TalkWin_Clear( GURU2MAIN_WORK *g2m )
{
  MSGWORK *msg = &g2m->msgwork;
  GFL_BMPWIN *bmpwin = msg->bmpwin_talk;
  
  BmpWinFrame_Clear( bmpwin, WINDOW_TRANS_ON );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmpwin), BMP_COL_NULL );
  GFL_BMPWIN_MakeTransWindow_VBlank( bmpwin );
}

//--------------------------------------------------------------
/**
 * �l�[���E�B���h�E������
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2NameWin_Init( GURU2MAIN_WORK *g2m, int max )
{
  int i;
  MSGWORK *msg = &g2m->msgwork;
  const BMPWIN_DAT *bmpdata = DATA_Guru2BmpNameWinTbl[max];
  
  msg->win_name_max = max;
  #ifdef DEBUG_GURU2_PRINTF
  OS_Printf( "win_name_max = %d\n", max );
  #endif
  
  for( i = 0; i < max; i++ ){
    msg->bmpwin_name[i] = GFL_BMPWIN_Create( bmpdata[i].frame,
                                    bmpdata[i].x,   bmpdata[i].y,
                                    bmpdata[i].w,   bmpdata[i].h, 
                                    bmpdata[i].pal, GFL_BMP_CHRAREA_GET_B );
    // �֘A�t��
    PRINT_UTIL_Setup( &msg->printUtilName[i], msg->bmpwin_name[i] );
  }
}

//--------------------------------------------------------------
/**
 * �l�[���E�B���h�E�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2NameWin_Delete( GURU2MAIN_WORK *g2m )
{
  int i;
  MSGWORK *msg = &g2m->msgwork;
  
  for( i = 0; i < msg->win_name_max; i++ ){
    GFL_BMPWIN_ClearTransWindow( msg->bmpwin_name[i] );
    GFL_BMPWIN_Delete( msg->bmpwin_name[i] );
  }
}

//--------------------------------------------------------------
/**
 * �l�[���E�B���h�E�\��
 * @param g2m GURU2MAIN_WORK
 * @param name  STRBUF
 * @param no  �E�B���h�E�ԍ�
 * @param col �����F
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2NameWin_Write(
  GURU2MAIN_WORK *g2m, STRBUF *name, int no, u32 col )
{
  MSGWORK *msg = &g2m->msgwork;
  GFL_BMPWIN *bmp = msg->bmpwin_name[no];
  
  BmpWinFrame_Write(
    bmp, WINDOW_TRANS_OFF, BGF_CHARNO_MENU, BGF_PANO_TALK_WIN );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmp), BMP_COL_WHITE );
//  GF_STR_PrintColor( bmp, FONT_SYSTEM, name, 4, 1, MSG_NO_PUT, col, NULL );

  PRINT_UTIL_PrintColor( &msg->printUtilName[no], msg->printQue, 
                         4, 1, name, msg->font, col );


  GFL_BMPWIN_MakeTransWindow_VBlank( bmp );
}

//--------------------------------------------------------------
/**
 * �l�[���E�B���h�E�\���@ID����F�ݒ�
 * @param g2m GURU2MAIN_WORK
 * @param name  STRBUF
 * @param no  �E�B���h�E�ԍ�
 * @param id  �ʐMID
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2NameWin_WriteIDColor(
  GURU2MAIN_WORK *g2m, STRBUF *name, int no, int id )
{
  u32 col = NAME_COL;
  if( id == GFL_NET_SystemGetCurrentID() ){ col = NAME_COL_MINE; }
  Guru2NameWin_Write( g2m, name, no, col );
}

//--------------------------------------------------------------
/**
 * �l�[���E�B���h�E�N���A
 * @param g2m GURU2MAIN_WORK
 * @param no  �E�B���h�E�ԍ�
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2NameWin_Clear( GURU2MAIN_WORK *g2m, int no )
{
  MSGWORK *msg       = &g2m->msgwork;
  GFL_BMPWIN *bmpwin = msg->bmpwin_name[no];
  
  BmpWinFrame_Clear( bmpwin, WINDOW_TRANS_ON );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmpwin), BMP_COL_NULL );
  GFL_BMPWIN_MakeTransWindow_VBlank( bmpwin );
}

//==============================================================================
//  �J����
//==============================================================================

//--------------------------------------------------------------
/**
 * �J�����@�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void guru2_CameraDelete( GURU2MAIN_WORK *g2m )
{
  CAMERAWORK *cm = &g2m->camera;
//  GFC_FreeCamera( cm->gf_camera );
  GFL_G3D_CAMERA_Delete( cm->gf_camera );
}


//==============================================================================
//  �^�b�`�p�l��
//==============================================================================

//--------------------------------------------------------------
/**
 * �^�b�`�p�l���@�{�^���͈͓��g���K�`�F�b�N
 * @param g2m GURU2MAIN_WORK
 * @retval  BOOL  TRUE=�q�b�g
 */
//--------------------------------------------------------------
static BOOL Guru2TP_ButtonHitTrgCheck( GURU2MAIN_WORK *g2m )
{
  static const GFL_UI_TP_HITTBL hittbl[]={
    { GFL_UI_TP_USE_CIRCLE, TP_BTN_CX, TP_BTN_CY, TP_BTN_R },
    { GFL_UI_TP_HIT_END, 0, 0, 0 },   // �I���f�[�^
  };
  u32 ret = GFL_UI_TP_HitTrg( hittbl );

  if(ret!=GFL_UI_TP_HIT_NONE){
    return TRUE;
  }
  return FALSE;
}


//==============================================================================
//  �M
//==============================================================================
//--------------------------------------------------------------
/// �l���ʎM���f���A�[�J�C�uID
//--------------------------------------------------------------
static const u32 DATA_SaraArcIdxTbl[G2MEMBER_MAX+1] =
{
  0,  //0 dummy
  0,  //1 dummy
  NARC_guru2_g_panel02_c_nsbmd,
  NARC_guru2_g_panel03_c_nsbmd,
  NARC_guru2_g_panel04_c_nsbmd,
  NARC_guru2_g_panel05_c_nsbmd,
};

// �M
static const GFL_G3D_UTIL_RES res_table_disc2[] = 
  {  { ARCID_GURU2, NARC_guru2_g_panel02_c_nsbmd, GFL_G3D_UTIL_RESARC },};
static const GFL_G3D_UTIL_RES res_table_disc3[] = 
  {  { ARCID_GURU2, NARC_guru2_g_panel03_c_nsbmd, GFL_G3D_UTIL_RESARC },};
static const GFL_G3D_UTIL_RES res_table_disc4[] = 
  {  { ARCID_GURU2, NARC_guru2_g_panel04_c_nsbmd, GFL_G3D_UTIL_RESARC },};
static const GFL_G3D_UTIL_RES res_table_disc5[] = 
  {  { ARCID_GURU2, NARC_guru2_g_panel05_c_nsbmd, GFL_G3D_UTIL_RESARC },};

static const GFL_G3D_UTIL_OBJ obj_table_disc[] = 
{
  {
    0,    // ���f�����\�[�XID
    0,    // ���f���f�[�^ID(���\�[�X����INDEX)
    0,    // �e�N�X�`�����\�[�XID
    NULL, // �A�j����`
    0,    // �A�j��ID
  },
}; 

static const GFL_G3D_UTIL_SETUP setupDisc[] =
{
  { res_table_disc2, 1, obj_table_disc, 1 },
  { res_table_disc3, 1, obj_table_disc, 1 },
  { res_table_disc4, 1, obj_table_disc, 1 },
  { res_table_disc5, 1, obj_table_disc, 1 },
};


//--------------------------------------------------------------
/**
 * �M������(�ڑ��l���ɂ��킹�ă��f�����O�؂�ւ��j
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Disc_Init( GURU2MAIN_WORK *g2m )
{
  int no;
  u32 idx;
  DISCWORK *disc = &g2m->disc;
  ARCHANDLE *handle = g2m->arc_handle;
  
  no = g2m->g2p->receipt_num;
  #ifdef DEBUG_DISC_NO
  no = DEBUG_DISC_NO;
  #endif  
//  idx = DATA_SaraArcIdxTbl[no];
//  FRO_MDL_ResSetArcLoad( &disc->rmdl, 0, handle, idx, HEAPID_GURU2, 0 );
//  FRO_MDL_TexTransBindVTaskAdd( &disc->rmdl );

  disc->unitIndex = GFL_G3D_UTIL_AddUnit( g2m->g3dUtil, &setupDisc[no-2] );
  

//  FRO_OBJ_InitInMdl( &disc->robj, &disc->rmdl );
  
  disc->rotate_fx = NUM_FX32( DISC_ROTATE_Y );
  disc->rotate_draw_offs_fx = DATA_DiscRotateDrawOffset[no];
  disc->scale.x = FX32_ONE;
  disc->scale.y = FX32_ONE;
  disc->scale.z = FX32_ONE;
  disc->rotate.x = DISC_ROTATE_X;
  disc->rotate.y = DISC_ROTATE_Y;
  disc->rotate.z = DISC_ROTATE_Z;
  disc->pos.x = DISC_POS_X_FX32;
  disc->pos.y = DISC_POS_Y_FX32;
  disc->pos.z = DISC_POS_Z_FX32;
  disc->draw_pos = disc->pos;
}

//--------------------------------------------------------------
/**
 * �M�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Disc_Delete( GURU2MAIN_WORK *g2m )
{
  DISCWORK *disc = &g2m->disc;
 // FRO_MDL_DeleteAll( &disc->rmdl );
 GFL_G3D_UTIL_DelUnit( g2m->g3dUtil, disc->unitIndex );

}

//--------------------------------------------------------------
/**
 * �M�X�V
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Disc_Update( GURU2MAIN_WORK *g2m )
{
  DISCWORK *disc = &g2m->disc;
//  fx32 fa = disc->rotate_fx;
  fx32 fa = g2m->comm.game_data.disc_angle;
  AngleAdd( &fa, disc->rotate_offs_fx );
  AngleAdd( &fa, disc->rotate_draw_offs_fx );
  disc->rotate.y = (360 - FX32_NUM( fa )) % 360;    //���]�������v����
//  OS_Printf("disc->rotate.y = %d\n", disc->rotate.y);

  disc->draw_pos.x = disc->pos.x + disc->offs_egg.x + disc->offs.x;
  disc->draw_pos.y = disc->pos.y + disc->offs_egg.y + disc->offs.y;
  disc->draw_pos.z = disc->pos.z + disc->offs_egg.z + disc->offs.z;
}

// CALC3D_MTX_CreateRot�ɓn�����߂̉�]�P��
#define ROT_RADIAN    ( 65536/360 )

//--------------------------------------------------------------
/**
 * �M�`��
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Disc_Draw( GURU2MAIN_WORK *g2m )
{
  DISCWORK *disc = &g2m->disc;
  
#ifndef DEBUG_DISP_CHECK_VANISH
//  FRO_OBJ_DrawScaleRotate(
//    &disc->robj, &disc->draw_pos, &disc->scale, &disc->rotate );
  GFL_G3D_OBJSTATUS status;
  GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( g2m->g3dUtil, disc->unitIndex );
  
  VEC_Set( &status.trans, disc->draw_pos.x, disc->draw_pos.y, disc->draw_pos.z );
  VEC_Set( &status.scale, disc->scale.x, disc->scale.y, disc->scale.z );
//  MTX_Identity33( &status.scale );
  GFL_CALC3D_MTX_CreateRot( disc->rotate.x, disc->rotate.y*ROT_RADIAN, 
                            disc->rotate.z, &status.rotate );

  GFL_G3D_DRAW_DrawObject( obj, &status );
    
#endif
}


//--------------------------------------------------------------
/**
 * �M�@��]
 * @param disc  DISCWORK
 * @param add   ��]�p�x
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Disc_Rotate( DISCWORK *disc, fx32 add )
{
#ifdef DEBUG_GURU2_PRINTF_COMM
  // �f�B�X�N�̉�]�����v�����g
  fx32 total = disc->rotate_fx + add;
  if( FX32_NUM( total ) >= 360 )
  {
    static int drcnt=0;
    drcnt++; OS_Printf("�f�B�X�N %d ��]��\n",drcnt);
  }
#endif

  AngleAdd( &disc->rotate_fx, add );
}




//==============================================================================
//  �w�i�R�c
//==============================================================================
// �w�i
static const GFL_G3D_UTIL_RES res_table_bg3d[] = 
  {  { ARCID_GURU2, NARC_guru2_g_panel_bg_nsbmd, GFL_G3D_UTIL_RESARC },};

static const GFL_G3D_UTIL_OBJ obj_table_bg3d[] = 
{
  {
    0,    // ���f�����\�[�XID
    0,    // ���f���f�[�^ID(���\�[�X����INDEX)
    0,    // �e�N�X�`�����\�[�XID
    NULL, // �A�j����`
    0,    // �A�j��ID
  },
}; 

static const GFL_G3D_UTIL_SETUP setupBg3d[] =
{
  { res_table_bg3d, 1, obj_table_bg3d, 1 },
};

//----------------------------------------------------------------------------------
/**
 * @brief �w�i�RD������
 *
 * @param   g2m   
 */
//----------------------------------------------------------------------------------
static void Bg3D_Init( GURU2MAIN_WORK *g2m )
{
  // �w�i�R�c���f�����O�f�[�^�ǂݍ���
  g2m->unitIndex_bg  = GFL_G3D_UTIL_AddUnit( g2m->g3dUtil, setupBg3d );

  // �w�i�|���S���ɃG�b�W�}�[�L���O����ʒ[�ɂ��̂���������
  G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,0,FALSE);
}

//----------------------------------------------------------------------------------
/**
 * @brief 3D�w�i�폜
 *
 * @param   g2m   
 */
//----------------------------------------------------------------------------------
static void Bg3D_Delete( GURU2MAIN_WORK *g2m )
{
  // �R�c���j�b�g�폜
  GFL_G3D_UTIL_DelUnit( g2m->g3dUtil, g2m->unitIndex_bg  );
  
}

//--------------------------------------------------------------
/**
 * �w�i�`��
 * @param g2m GURU2MAIN_WORK
 * @retval    none
 */
//--------------------------------------------------------------
static void Bg3D_Draw( GURU2MAIN_WORK *g2m )
{
  GFL_G3D_OBJSTATUS status;

  GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( g2m->g3dUtil, g2m->unitIndex_bg );
  
  VEC_Set( &status.trans, 0,0,0 );
  VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  GFL_CALC3D_MTX_CreateRot( 0, 0, 0, &status.rotate );

  GFL_G3D_DRAW_DrawObject( obj, &status );
    
}


//==============================================================================
//  ��
//==============================================================================
static const u32 DATA_EggCursorMdlArcIDTbl[G2MEMBER_MAX] =
{
  NARC_guru2_g_egg_cursor1_nsbmd,
  NARC_guru2_g_egg_cursor2_nsbmd,
  NARC_guru2_g_egg_cursor3_nsbmd,
  NARC_guru2_g_egg_cursor4_nsbmd,
  NARC_guru2_g_egg_cursor5_nsbmd,
};

static const u32 DATA_EggCursorAnmArcIDTbl[G2MEMBER_MAX] =
{
  NARC_guru2_g_egg_cursor1_nsbca,
  NARC_guru2_g_egg_cursor2_nsbca,
  NARC_guru2_g_egg_cursor3_nsbca,
  NARC_guru2_g_egg_cursor4_nsbca,
  NARC_guru2_g_egg_cursor5_nsbca,
};

static const GFL_G3D_UTIL_RES res_table_g_egg[] = 
{  { ARCID_GURU2, NARC_guru2_g_egg_nsbmd, GFL_G3D_UTIL_RESARC },  };
static const GFL_G3D_UTIL_OBJ obj_table_g_egg[] = 
{
  {
    0,    // ���f�����\�[�XID
    0,    // ���f���f�[�^ID(���\�[�X����INDEX)
    0,    // �e�N�X�`�����\�[�XID
    NULL, // �A�j���e�[�u��(�����w��̂���)
    0,    // �A�j�����\�[�X��
  },
}; 
static const GFL_G3D_UTIL_RES res_table_egg_kage[] = 
{  { ARCID_GURU2, NARC_guru2_g_egg_kage_nsbmd, GFL_G3D_UTIL_RESARC },  };
static const GFL_G3D_UTIL_OBJ obj_table_egg_kage[] = 
{
  {
    0,    // ���f�����\�[�XID
    0,    // ���f���f�[�^ID(���\�[�X����INDEX)
    0,    // �e�N�X�`�����\�[�XID
    NULL, // �A�j���e�[�u��(�����w��̂���)
    0,    // �A�j�����\�[�X��
  },
}; 

// �^�}�S���\�[�X�Z�b�g�A�b�v
static const GFL_G3D_UTIL_SETUP setupEgg[] =
{
  { res_table_g_egg,    1, obj_table_g_egg,    1 },
  { res_table_egg_kage, 1, obj_table_egg_kage, 1 },
};


static const GFL_G3D_UTIL_RES res_table_cursor1[] = 
{
    { ARCID_GURU2, NARC_guru2_g_egg_cursor1_nsbmd, GFL_G3D_UTIL_RESARC },  
    { ARCID_GURU2, NARC_guru2_g_egg_cursor1_nsbca, GFL_G3D_UTIL_RESARC },  
};
static const GFL_G3D_UTIL_RES res_table_cursor2[] = 
{
    { ARCID_GURU2, NARC_guru2_g_egg_cursor2_nsbmd, GFL_G3D_UTIL_RESARC },  
    { ARCID_GURU2, NARC_guru2_g_egg_cursor2_nsbca, GFL_G3D_UTIL_RESARC },  
};
static const GFL_G3D_UTIL_RES res_table_cursor3[] = 
{
    { ARCID_GURU2, NARC_guru2_g_egg_cursor3_nsbmd, GFL_G3D_UTIL_RESARC },  
    { ARCID_GURU2, NARC_guru2_g_egg_cursor3_nsbca, GFL_G3D_UTIL_RESARC },  
};
static const GFL_G3D_UTIL_RES res_table_cursor4[] = 
{
    { ARCID_GURU2, NARC_guru2_g_egg_cursor4_nsbmd, GFL_G3D_UTIL_RESARC },  
    { ARCID_GURU2, NARC_guru2_g_egg_cursor4_nsbca, GFL_G3D_UTIL_RESARC },  
};
static const GFL_G3D_UTIL_RES res_table_cursor5[] = 
{
    { ARCID_GURU2, NARC_guru2_g_egg_cursor5_nsbmd, GFL_G3D_UTIL_RESARC },  
    { ARCID_GURU2, NARC_guru2_g_egg_cursor5_nsbca, GFL_G3D_UTIL_RESARC },  
};

static const GFL_G3D_UTIL_ANM anm_table_cursor[] = 
{
  { 1, 0 },
};


static const GFL_G3D_UTIL_OBJ obj_table_cursor[] = 
{
  {
    0,                        // ���f�����\�[�XID
    0,                        // ���f���f�[�^ID(���\�[�X����INDEX)
    0,                        // �e�N�X�`�����\�[�XID
    anm_table_cursor,         // �A�j���e�[�u��(�����w��̂���)
    NELEMS(anm_table_cursor), // �A�j�����\�[�X��
  },
}; 


static const GFL_G3D_UTIL_SETUP setupCursor[] =
{
  { res_table_cursor1, NELEMS(res_table_cursor1), obj_table_cursor, NELEMS(obj_table_cursor) },
  { res_table_cursor2, NELEMS(res_table_cursor2), obj_table_cursor, NELEMS(obj_table_cursor) },
  { res_table_cursor3, NELEMS(res_table_cursor3), obj_table_cursor, NELEMS(obj_table_cursor) },
  { res_table_cursor4, NELEMS(res_table_cursor4), obj_table_cursor, NELEMS(obj_table_cursor) },
  { res_table_cursor5, NELEMS(res_table_cursor5), obj_table_cursor, NELEMS(obj_table_cursor) },
};

//--------------------------------------------------------------
/**
 * ���@������
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Egg_Init( GURU2MAIN_WORK *g2m )
{
  int i;
  const u32 *mdl_id, *anm_id;
  
  EGGWORK *egg = &g2m->egg;
/*  
  FRO_MDL_ResSetArcLoad(  //���C�����\�[�X�Ƃ��Ĉ���
    &egg->m_rmdl, 0, g2m->arc_handle,
    NARC_guru2_g_egg_nsbmd, HEAPID_GURU2, 0 );
  FRO_MDL_TexTransBindVTaskAdd( &egg->m_rmdl );
  
  FRO_MDL_ResSetArcLoad(  //���C�����\�[�X�Ƃ��Ĉ���
    &egg->m_rmdl_kage, 0, g2m->arc_handle,
    NARC_guru2_g_egg_kage_nsbmd, HEAPID_GURU2, 0 );
  FRO_MDL_TexTransBindVTaskAdd( &egg->m_rmdl_kage );

  mdl_id = DATA_EggCursorMdlArcIDTbl;
  anm_id = DATA_EggCursorAnmArcIDTbl;
  
  for( i = 0; i < G2MEMBER_MAX; i++, mdl_id++, anm_id++ ){
    FRO_MDL_ResSetArcLoad(
      &egg->m_rmdl_cursor[i], 0, g2m->arc_handle,
      *mdl_id, HEAPID_GURU2, 0 );
    FRO_MDL_TexTransBindVTaskAdd( &egg->m_rmdl_cursor[i] );
    
    FRO_ANM_AnmResSetArcLoad(
      &egg->m_ranm_cursor[i], 0, g2m->arc_handle,
      *anm_id, HEAPID_GURU2, 0 );
    
    FRO_ANM_AllocAnmObjInMdl(
      &egg->m_ranm_cursor[i], &egg->m_rmdl_cursor[i], HEAPID_GURU2 );
    FRO_ANM_AnmObjInitInMdl(
      &egg->m_ranm_cursor[i], &egg->m_rmdl_cursor[i] );
  }

*/
  egg->unitIndex_egg  = GFL_G3D_UTIL_AddUnit( g2m->g3dUtil, &setupEgg[0] );
  egg->unitIndex_kage = GFL_G3D_UTIL_AddUnit( g2m->g3dUtil, &setupEgg[1] );
  
  for( i = 0; i < G2MEMBER_MAX; i++){
     egg->unitIndex_cursor[i] = GFL_G3D_UTIL_AddUnit( g2m->g3dUtil, &setupCursor[i] );
  }

}

//--------------------------------------------------------------
/**
 * �����f�����O���\�[�X�@�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Egg_Delete( GURU2MAIN_WORK *g2m )
{
  int i;
  EGGACTOR *eact;
  EGGWORK *egg = &g2m->egg;
  
  for( i = 0; i < G2MEMBER_MAX; i++ ){
    eact = &egg->eact[i];
  }
/*  
  FRO_MDL_DeleteAll( &egg->m_rmdl );
  FRO_MDL_DeleteAll( &egg->m_rmdl_kage );
  
  for( i = 0; i < G2MEMBER_MAX; i++ ){
    FRO_MDL_DeleteAll( &egg->m_rmdl_cursor[i] );
    FRO_ANM_AnmResFree( &egg->m_ranm_cursor[i] );
  }
*/

 for( i = 0; i < G2MEMBER_MAX; i++){
  GFL_G3D_UTIL_DelUnit( g2m->g3dUtil, egg->unitIndex_cursor[i]  );
 }

 GFL_G3D_UTIL_DelUnit( g2m->g3dUtil, egg->unitIndex_egg  );
 GFL_G3D_UTIL_DelUnit( g2m->g3dUtil, egg->unitIndex_kage  );

  
  
}


//--------------------------------------------------------------
/**
 * ���A�N�^�[�@OBJ������
 * @param GURU2MAIN_WORK *g2m
 * @param act EGGACTOR*
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Egg_MdlActInit( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
  EGGWORK *egg = &g2m->egg;
//  FRO_OBJ_InitInMdl( &act->robj, &egg->m_rmdl );
  act->draw_flag = TRUE;
  
  EggKage_Init( g2m, act );
  EggCursor_Init( g2m, act );
}

//==============================================================================
//  ���A�N�^�[
//==============================================================================
//--------------------------------------------------------------
/**
 * ���A�N�^�[�X�V
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggAct_Update( GURU2MAIN_WORK *g2m )
{
  int i = 0,max = g2m->comm.play_max;
  EGGWORK *egg = &g2m->egg;
  
  while( i < max ){
    if( egg->eact[i].use_flag ){
      EggAct_AnglePosSet( &egg->eact[i], 
          &g2m->disc.offs_egg, NUM_FX32(g2m->comm.game_data.egg_angle[i]), g2m->comm.game_data.egg_joffs );
    }
    
    if( egg->ekage[i].use_flag ){
      EggKage_Update( g2m, &egg->ekage[i], g2m->comm.game_data.egg_joffs );
    }
    
    if( egg->ecursor[i].use_flag ){
      EggCursor_Update( g2m, &egg->ecursor[i], i );
    }
    
    i++;
  }
}

//--------------------------------------------------------------
/**
 * ���A�N�^�[�@�`��
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggAct_Draw( GURU2MAIN_WORK *g2m )
{
  int i = 0,max = g2m->comm.play_max;
  EGGWORK *egg = &g2m->egg;
  EGGACTOR *eact = egg->eact;
  EGGKAGE *ekage = egg->ekage;
  EGGCURSOR *ecs = egg->ecursor;
  
  while( i < max ){
    // �^�}�S�`��
    if( eact->draw_flag ){
//      FRO_OBJ_DrawScaleRotate(
//        &eact->robj, &eact->pos, &eact->scale, &eact->rotate );

        GFL_G3D_OBJSTATUS status;
        GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( g2m->g3dUtil, egg->unitIndex_egg );
        
        VEC_Set( &status.trans, eact->pos.x, eact->pos.y, eact->pos.z );
        VEC_Set( &status.scale, eact->scale.x, eact->scale.y, eact->scale.z );
        GFL_CALC3D_MTX_CreateRot( eact->rotate.x, eact->rotate.y, eact->rotate.z, &status.rotate );
      
        GFL_G3D_DRAW_DrawObject( obj, &status );

    }
    
    // �e�`��
    if( ekage->use_flag ){
//      FRO_OBJ_DrawScaleRotate(
//        &ekage->robj, &ekage->pos, &ekage->scale, &ekage->rotate );
        GFL_G3D_OBJSTATUS status;
        GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( g2m->g3dUtil, egg->unitIndex_kage );
        
        VEC_Set( &status.trans, ekage->pos.x, ekage->pos.y, ekage->pos.z );
        VEC_Set( &status.scale, ekage->scale.x, ekage->scale.y, ekage->scale.z );
        GFL_CALC3D_MTX_CreateRot( ekage->rotate.x, ekage->rotate.y, ekage->rotate.z, &status.rotate );
      
        GFL_G3D_DRAW_DrawObject( obj, &status );


    }
    
    // �J�[�\���`��
    if( ecs->use_flag ){
//      FRO_OBJ_DrawPos( &ecs->robj, &ecs->pos );
        GFL_G3D_OBJSTATUS status;
        GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( g2m->g3dUtil, egg->unitIndex_cursor[i] );
        
        VEC_Set( &status.trans, ecs->pos.x, ecs->pos.y, ecs->pos.z );
        VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
        MTX_Identity33( &status.rotate );
      
        GFL_G3D_DRAW_DrawObject( obj, &status );


    }
    
    eact++;
    ekage++;
    ecs++;
    i++;
  }
}

//--------------------------------------------------------------
/**
 * ���A�N�^�[�@��]
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggAct_Rotate( GURU2MAIN_WORK *g2m, fx32 add )
{
  int i = 0,max = g2m->comm.play_max;
  EGGWORK *egg = &g2m->egg;
  
  while( i < max ){
    if( egg->eact[i].use_flag ){
      AngleAdd( &egg->eact[i].angle, add );
    }
    
    i++;
  }
}

//--------------------------------------------------------------
/**
 * ���@�p�x������W�A�g�k������
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void EggAct_AnglePosSet( EGGACTOR *eact, const VecFx32 *offs, fx32 fa, fx32 joffs )
{
  u16 r;
//  fx32 fa;
  
//  fa = eact->angle;
  AngleAdd( &fa, eact->offs_angle );
  r = (u16)FX32_NUM( fa );
  
  eact->pos.x =
    EGG_DISC_CX_FX + eact->offs.x + offs->x + (GFL_CALC_Cos360(r)*EGG_DISC_CXS);
  eact->pos.y =
    EGG_DISC_CY_FX + eact->offs.y + offs->y + joffs;
  eact->pos.z =
    EGG_DISC_CZ_FX + eact->offs.z + offs->z + (GFL_CALC_Sin360(r)*EGG_DISC_CZS);
  
  eact->rotate.x = FX32_NUM( eact->rotate_fx.x );
  eact->rotate.y = FX32_NUM( eact->rotate_fx.y );
  eact->rotate.z = FX32_NUM( eact->rotate_fx.z );
}

//--------------------------------------------------------------
/**
 * ��ʎ�O�̗��A�N�^�[��Ԃ�
 * @param g2m GURU2MAIN_WORK
 * @retval  int �A�N�^�[�z��v�f��
 */
//--------------------------------------------------------------
static EGGACTOR * EggAct_FrontEggActGet( GURU2MAIN_WORK *g2m )
{
  int i,j,a,n;
  fx32 front,angle,res;
  int max = g2m->comm.play_max;
  int buf[G2MEMBER_MAX],no[G2MEMBER_MAX];
  EGGWORK *egg = &g2m->egg;
  
  i = 0;
  // �����̒ʐMID�ɑΉ�������]�p�x�ʒu���擾
  front = NUM_FX32( DATA_EggStartAngle[max][g2m->comm.my_play_no] );
  
  OS_Printf("my_play_no%d\n", g2m->comm.my_play_no);
  OS_Printf("egg0->comm_id=%d, egg1->comm_id=%d\n", egg->eact[0].comm_id,egg->eact[1].comm_id);
  // ���[�N������
  do{no[i]=i,buf[i]=0xffff;}while(++i<max); // no[5]=0,1,2,3,4 buf[5]=0xffff,0xffff,0xffff,0xffff,0xffff,
  
  // �������_���猩�Ċe�ʐMID�̌��݂̉�]�p�x�͂�����buf�Ɋi�[
  for( i = 0; i < max; i++ ){
    angle = egg->eact[i].angle;
    res = front - angle;
    
    if( res < 0 ){
      res = (front + NUM_FX32(360)) - angle;
    }
    
    buf[i] = FX32_NUM( res );
  }
  
  // �����̒ʐMID�̊p�x�ƃ^�}�S�̊p�x���r���Ĉ�ԋ߂�����
  // ���ג���
  for( i = 1; i < max; i++ ){
    n = no[i]; a = buf[n];
    for( j = i - 1; j >= 0 && buf[no[j]] > a; j-- ){
      no[j+1] = no[j];
    }
    no[j+1] = n;
  }
  
  return( &egg->eact[no[0]] );
}

//==============================================================================
//  ���e
//==============================================================================
#define EGGKAGE_OFFS_Y_FX (FX32_ONE*2)

//--------------------------------------------------------------
/**
 * ���e�@�Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void EggKage_Init( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
  EGGWORK *egg = &g2m->egg;
  EGGKAGE *ekage = &egg->ekage[act->play_no];
  
  ekage->use_flag = TRUE;
  ekage->eact = act;
//  FRO_OBJ_InitInMdl( &ekage->robj, &egg->m_rmdl_kage );
  EggKage_Update( g2m, ekage, 0 );
}

//--------------------------------------------------------------
/**
 * ���e�@�X�V
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void EggKage_Update( GURU2MAIN_WORK *g2m, EGGKAGE *ekage, fx32 joffs )
{
  fx32 offs;
  
  ekage->rotate.x = 0;
  ekage->rotate.y = 0;
  ekage->rotate.z = 0;
  ekage->pos = ekage->eact->pos;
  ekage->pos.x -= ekage->eact->offs.x;
  ekage->pos.y -= ( ekage->eact->offs.y + joffs );
  ekage->pos.z -= ekage->eact->offs.z;
  ekage->pos.y += EGGKAGE_OFFS_Y_FX;
  
  offs = ( ekage->eact->offs.y + joffs );
  
  if( offs < 0 ){
    offs = -offs;
  }
  
  offs /= 0x80;
  
  if( offs > 0x800 ){
    offs = 0x800;
  }
  
  ekage->scale.x = FX32_ONE - offs;
  ekage->scale.y = FX32_ONE - offs;
  ekage->scale.z = FX32_ONE - offs;
}

//==============================================================================
//  ���J�[�\��
//==============================================================================
#define EGGCURSOR_OFFS_Y_FX (FX32_ONE*20)

//--------------------------------------------------------------
/**
 * ���J�[�\���@�Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void EggCursor_Init( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
  EGGWORK *egg = &g2m->egg;
  EGGCURSOR *ecs = &egg->ecursor[act->play_no];
  
  ecs->use_flag = TRUE;
  ecs->eact = act;
  
//  FRO_OBJ_InitAll( &ecs->robj,
//    &egg->m_rmdl_cursor[act->play_no],
//    &egg->m_ranm_cursor[act->play_no] );
}

//--------------------------------------------------------------
/**
 * ���J�[�\���@�X�V
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void EggCursor_Update( GURU2MAIN_WORK *g2m, EGGCURSOR *ecs, int i )
{
  EGGWORK *egg = &g2m->egg;
  GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( g2m->g3dUtil, egg->unitIndex_cursor[i] );
  ecs->pos = ecs->eact->pos;
  ecs->pos.y += EGGCURSOR_OFFS_Y_FX;
//  FRO_ANM_Play(
//    &egg->m_ranm_cursor[ecs->eact->play_no], FX32_ONE, ANMLOOP_ON);
  GFL_G3D_OBJECT_LoopAnimeFrame( obj, 0, FX32_ONE );

}

//==============================================================================
//  ���o��^�X�N
//==============================================================================
//--------------------------------------------------------------
/**
 * ���o��^�X�N�ǉ�
 * @param g2m GURU2MAIN_WORK
 * @param id  ��ID
 * @param no  ���ԍ�
 * @param name_no ���O�E�B���h�E�ԍ�
 * @param angle �p�x
 * @param offset  �p�x�I�t�Z�b�g
 * @param eact  EGGACTOR
 * @retval  GFL_TCB* GFL_TCB*
 */
//--------------------------------------------------------------
static GFL_TCB* EggAct_StartTcbSet(
  GURU2MAIN_WORK *g2m, int id, int no, int name_no,
  int angle, int offs, EGGACTOR *eact )
{
  GFL_TCB* tcb;
  EGGSTARTWORK *work;
  
  eact->comm_id = id;
  eact->play_no = no;
  eact->name_no = name_no;
  eact->shake_count = no;
  eact->angle = NUM_FX32( angle );
  eact->offs_angle = NUM_FX32( offs );
  eact->use_flag = TRUE;
  eact->set_flag = FALSE;
  
  work = GFL_HEAP_AllocMemoryLo( HEAPID_GURU2, sizeof(EGGSTARTWORK) );
  GFL_STD_MemFill( work, 0, sizeof(EGGSTARTWORK) );
  work->g2m = g2m;
  work->eact = eact;
  
  tcb = GFL_TCB_AddTask( g2m->tcbSys,  EggActStartTcb, work, 0 );

  return( tcb );
}

//--------------------------------------------------------------
/**
 * ���o��^�X�N
 * @param tcb GFL_TCB*
 * @param wk  tcb work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggActStartTcb( GFL_TCB* tcb, void *wk )
{
  RET ret;
  EGGSTARTWORK *work = wk;
  
  do{
    ret = DATA_EggStartTbl[work->seq_no]( work );
  }while( ret == RET_CONT );
  
  if( ret == RET_END ){
    work->eact->set_flag = TRUE;
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
}

//--------------------------------------------------------------
/**
 * ���o��^�X�N�@������
 * @param work  EGGSTARTWORK
 * @retval  BOOL  TRUE=�ċN
 */
//--------------------------------------------------------------
static RET EggActStart_Init( EGGSTARTWORK *work )
{
  EGGACTOR *eact = work->eact;
  
  eact->scale.x = FX32_ONE;
  eact->scale.y = FX32_ONE;
  eact->scale.z = FX32_ONE;
  eact->offs.y = EGG_START_OFFS_Y_FX;
  Egg_MdlActInit( work->g2m, eact );
  
  work->fall_speed = NUM_FX32( 8 );
  work->seq_no = SEQNO_EGGSTART_FALL;
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���o��^�X�N�@����
 * @param work  EGGSTARTWORK
 * @retval  BOOL  TRUE=�ċA
 */
//--------------------------------------------------------------
static RET EggStart_Fall( EGGSTARTWORK *work )
{
  EGGACTOR *eact = work->eact;
  
  eact->offs.y -= work->fall_speed;
  
  if( work->fall_se_play == FALSE ){    //����SE�炷�^�C�~���O����
    if( eact->offs.y <= NUM_FX32(104) ){
      work->fall_se_play = TRUE;
      PMSND_PlaySE( GURU2_SE_EGG_FALL );
    }
  }
  
  if( eact->offs.y <= NUM_FX32(32) ){
    eact->offs.y = NUM_FX32( 32 );
    work->seq_no = SEQNO_EGGSTART_FALL_END;
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/**
 * ���o��^�X�N�@�����I��
 * @param work  EGGSTARTWORK
 * @retval  BOOL  TRUE=�ċA
 */
//--------------------------------------------------------------
static RET EggStart_FallEnd( EGGSTARTWORK *work )
{
  EGGACTOR *eact = work->eact;
  
  if( work->fall_speed > NUM_FX32(1) ){
    work->fall_speed -= 0x1000;
    
    if( work->fall_speed < NUM_FX32(1) ){
      work->fall_speed = NUM_FX32( 1 );
    }
  }
  
  eact->offs.y -= work->fall_speed;
  
  if( eact->offs.y <= 0 ){
    eact->offs.y = 0;
    
    Guru2NameWin_WriteIDColor(
      work->g2m,
      work->g2m->comm.my_name_buf[eact->comm_id],
      eact->name_no, eact->comm_id );
    
    return( RET_END );
  }
  
  return( RET_NON );
}

//--------------------------------------------------------------
/// ���o��^�X�N�@�����e�[�u��
//--------------------------------------------------------------
static RET (* const DATA_EggStartTbl[SEQNO_EGGSTART_MAX])( EGGSTARTWORK *work ) =
{
  EggActStart_Init,
  EggStart_Fall,
  EggStart_FallEnd,
};

//==============================================================================
//  �������オ��^�X�N
//==============================================================================
//--------------------------------------------------------------
/**
 * �������オ�蓮��
 * @param g2m   GURU2MAIN_WORK
 * @param work  EGGJUMP_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggJump( GURU2MAIN_WORK *g2m, EGGJUMP_WORK *work )
{
  EGGACTOR *eact = work->eact;
  
  switch( work->seq_no ){
  case 0:
    EggSpinTcb_EggSet( g2m, eact );
    work->add_y = EGG_JUMP_TOPSPEED_FX;
    work->accle_y = -EGG_JUMP_ACCLE_FX;
    work->seq_no++;
  case 1:
    eact->joffs += work->add_y;
//    eact->offs.y += work->add_y;
    work->add_y += work->accle_y;
    work->frame++;
    
    if( work->frame < EGG_JUMP_FLIGHT_FRAME_HL  ){
      break;
    }
    
    work->frame = 0;
    work->add_y = 0;
    
    if( work->turn_flag == FALSE ){
      work->turn_flag = TRUE;
      break;
    }
    
    eact->joffs = 0;
//    eact->offs.y = 0;
    EggShakeTcb_EggSet( g2m, eact );
    GFL_STD_MemFill( work, 0, sizeof(EGGJUMP_WORK) );
  }
}

//--------------------------------------------------------------
/**
 * �������オ��^�X�N
 * @param tcb   GFL_TCB*
 * @param wk    tcb work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggJumpTcb( GFL_TCB* tcb, void *wk )
{
  int i;
  GURU2MAIN_WORK *g2m = wk;
  EGGJUMPTCB_WORK *work = &g2m->egg_jump_tcb_work;
  EGGJUMP_WORK *jump = work->jump_work;
  
  for( i = 0; i < G2MEMBER_MAX; i++, jump++ ){
    if( jump->use_flag ){
      EggJump( g2m, jump );
    }
  }
}

//--------------------------------------------------------------
/**
 * �������オ��^�X�N������
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggJumpTcb_Init( GURU2MAIN_WORK *g2m )
{
  EGGJUMPTCB_WORK *work;
  
  work = &g2m->egg_jump_tcb_work;
  GFL_STD_MemFill( work, 0, sizeof(EGGJUMPTCB_WORK) );
  
  g2m->tcb_egg_jump = GFL_TCB_AddTask( g2m->tcbSys,  EggJumpTcb, g2m, TCBPRI_EGG_JUMP );
  GF_ASSERT( g2m->tcb_egg_jump );
}

//--------------------------------------------------------------
/**
 * �������オ��^�X�N�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggJumpTcb_Delete( GURU2MAIN_WORK *g2m )
{
  GFL_TCB_DeleteTask( g2m->tcb_egg_jump );
}

//--------------------------------------------------------------
/**
 * �������オ��^�X�N�ɗ��A�N�^�[�Z�b�g
 * @param g2m GURU2MAIN_WORK
 * @param act EGGACTOR
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggJumpTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
  int i;
  EGGJUMPTCB_WORK *work = &g2m->egg_jump_tcb_work;
  EGGJUMP_WORK *jump = work->jump_work;
  
  for( i = 0; i < G2MEMBER_MAX; i++, jump++ ){
    if( jump->use_flag == FALSE ){
      jump->use_flag = TRUE;
      jump->eact = act;
      return;
    }
  }
  
  GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * �������オ��^�X�N�ɑS�Ă̗��A�N�^�[�Z�b�g
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggJumpTcb_EggSetAll( GURU2MAIN_WORK *g2m )
{
  EGGWORK *egg = &g2m->egg;
  int i = 0, max = g2m->comm.play_max;
  
  do{
    if( egg->eact[i].use_flag ){
      EggJumpTcb_EggSet( g2m, &egg->eact[i] );
    }
    i++;
  }while( i < max );
}

//--------------------------------------------------------------
/**
 * �������オ��^�X�N�`�F�b�N
 * @param g2m GURU2MAIN_WORK
 * @retval  BOOL  TRUE=�W�����v��
 */
//--------------------------------------------------------------
static BOOL EggJumpTcb_JumpCheck( GURU2MAIN_WORK *g2m )
{
  int i;
  EGGJUMPTCB_WORK *work = &g2m->egg_jump_tcb_work;
  EGGJUMP_WORK *jump = work->jump_work;
  
  for( i = 0; i < G2MEMBER_MAX; i++, jump++ ){
    if( jump->use_flag ){
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//==============================================================================
/// ����]�^�X�N
//==============================================================================
//--------------------------------------------------------------
/**
 * ����]
 * @param spin  EGGSPIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggSpin( EGGSPIN_WORK *spin )
{
  fx32 r;
  EGGACTOR *eact = spin->eact;
  
  switch( spin->seq_no ){
  case 0:
    spin->add_fx = EGG_SPIN_TOPSPEED_FX;
    eact->rotate_fx.y = 0;
    spin->seq_no++;
  case 1:
    r = eact->rotate_fx.y;
    AngleAdd( &eact->rotate_fx.y, spin->add_fx );
    
    if( eact->rotate_fx.y < EGG_SPIN_STOPSPEED_FX ){
      spin->add_fx = (u32)(spin->add_fx) >> 1;
      
      if( spin->add_fx < EGG_SPIN_STOPSPEED_FX ){
        eact->rotate_fx.y = 0;
        GFL_STD_MemFill( spin, 0, sizeof(EGGSPIN_WORK) );
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * ����]�^�X�N
 * @param tcb   GFL_TCB*
 * @param wk    tcb work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggSpinTcb( GFL_TCB* tcb, void *wk )
{
  int i;
  GURU2MAIN_WORK *g2m = wk;
  EGGSPINTCB_WORK *work = &g2m->egg_spin_tcb_work;
  EGGSPIN_WORK *spin = work->spin_work;
  
  for( i = 0; i < G2MEMBER_MAX; i++, spin++ ){
    if( spin->use_flag ){
      EggSpin( spin );
    }
  }
}

//--------------------------------------------------------------
/**
 * ����]�^�X�N������
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggSpinTcb_Init( GURU2MAIN_WORK *g2m )
{
  EGGSPINTCB_WORK *work;
  
  work = &g2m->egg_spin_tcb_work;
  GFL_STD_MemFill( work, 0, sizeof(EGGSPINTCB_WORK) );
  
  g2m->tcb_egg_spin = GFL_TCB_AddTask( g2m->tcbSys,  EggSpinTcb, g2m, TCBPRI_EGG_SPIN );
  GF_ASSERT( g2m->tcb_egg_spin );
}

//--------------------------------------------------------------
/**
 * ����]�^�X�N�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggSpinTcb_Delete( GURU2MAIN_WORK *g2m )
{
  GFL_TCB_DeleteTask( g2m->tcb_egg_spin );
}

//--------------------------------------------------------------
/**
 * ����]�^�X�N�ɗ��A�N�^�[�Z�b�g
 * @param g2m GURU2MAIN_WORK
 * @param act EGGACTOR
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggSpinTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
  int i;
  EGGSPINTCB_WORK *work = &g2m->egg_spin_tcb_work;
  EGGSPIN_WORK *spin = work->spin_work;
  
  for( i = 0; i < G2MEMBER_MAX; i++, spin++ ){ //���s���ł���΍ċN��
    if( spin->use_flag == TRUE && spin->eact == act ){
      spin->seq_no = 0;
      return;
    }
  }
  
  for( i = 0, spin = work->spin_work; i < G2MEMBER_MAX; i++, spin++ ){
    if( spin->use_flag == FALSE ){
      spin->use_flag = TRUE;
      spin->eact = act;
      return;
    }
  }
  
  GF_ASSERT( 0 );
}

//==============================================================================
//  ���h��^�X�N
//==============================================================================
//--------------------------------------------------------------
/**
 * ���h��
 * @param tcb GFL_TCB*
 * @param wk  tcb work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggShake( EGGSHAKE_WORK *shake )
{
//  eact->pos.z = EGG_DISC_CZ_FX + eact->offs.z + (Sin360(r)*EGG_DISC_CZS);
  fx32 r;
  EGGACTOR *eact = shake->eact;
  
  switch( shake->seq_no ){
  case 0:
    eact->rotate_fx.z = 0;
    shake->rad = 0;
    shake->haba_fx = EGG_SHAKE_HABA_FX;
    shake->add_fx = EGG_SHAKE_TOPSPEED_FX;
    shake->seq_no++;
  case 1:
    r = GFL_CALC_Sin360( FX32_NUM(shake->rad)) * FX32_NUM(shake->haba_fx );
    eact->rotate_fx.z = 0;
    AngleAdd( &eact->rotate_fx.z, r );
    
    r = shake->rad;
    AngleAdd( &shake->rad, shake->add_fx );
    
    if( shake->rad < r ){
      shake->haba_fx = (u32)(shake->haba_fx) >> 1;
      
      if( shake->haba_fx == 0 ){
        eact->rotate_fx.z = 0;
        GFL_STD_MemFill( shake, 0, sizeof(EGGSHAKE_WORK) );
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * ���h��^�X�N
 * @param tcb GFL_TCB*
 * @param wk  tcb work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggShakeTcb( GFL_TCB* tcb, void *wk )
{
  int i;
  GURU2MAIN_WORK *g2m = wk;
  EGGSHAKETCB_WORK *work = &g2m->egg_shake_tcb_work;
  EGGSHAKE_WORK *shake = work->shake_work;
  
  for( i = 0; i < G2MEMBER_MAX; i++, shake++ ){
    if( shake->use_flag ){
      EggShake( shake );
    }
  }
}

//--------------------------------------------------------------
/**
 * ���h��^�X�N������
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggShakeTcb_Init( GURU2MAIN_WORK *g2m )
{
  EGGSHAKETCB_WORK *work;
  
  work = &g2m->egg_shake_tcb_work;
  GFL_STD_MemFill( work, 0, sizeof(EGGSHAKETCB_WORK) );
  
  g2m->tcb_egg_shake = GFL_TCB_AddTask( g2m->tcbSys,  EggShakeTcb, g2m, TCBPRI_EGG_SHAKE );
  GF_ASSERT( g2m->tcb_egg_shake );
}

//--------------------------------------------------------------
/**
 * ���h��^�X�N�폜
 * @param g2m GURU2MAIN_WORK  
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggShakeTcb_Delete( GURU2MAIN_WORK *g2m )
{
  GFL_TCB_DeleteTask( g2m->tcb_egg_shake );
}

//--------------------------------------------------------------
/**
 * ���h��^�X�N�ɗ��A�N�^�[�Z�b�g
 * @param g2m GURU2MAIN_WORK
 * @param act EGGACTOR
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggShakeTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
  int i;
  EGGSHAKETCB_WORK *work = &g2m->egg_shake_tcb_work;
  EGGSHAKE_WORK *shake = work->shake_work;
  
  for( i = 0; i < G2MEMBER_MAX; i++, shake++ ){ //���s���ł���΍ċN��
    if( shake->use_flag == TRUE && shake->eact == act ){
      shake->seq_no = 0;
      return;
    }
  }
  
  for( i = 0, shake = work->shake_work; i < G2MEMBER_MAX; i++, shake++ ){
    if( shake->use_flag == FALSE ){
      shake->use_flag = TRUE;
      shake->eact = act;
      return;
    }
  }
  
  GF_ASSERT( 0 );
}

//==============================================================================
//  ���M�W�����v�^�X�N
//==============================================================================
//--------------------------------------------------------------
/**
 * ���M�����オ�菈��
 * @param g2m GURU2MAIN_WORK
 * @param work  EGGDISCJUMPTCB_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggDiscJump( GURU2MAIN_WORK *g2m, EGGDISCJUMPTCB_WORK *work )
{
  DISCWORK *disc = &g2m->disc;
  
  switch( work->seq_no ){
  case 0:
    break;
  case 1:
    work->shake_fx = NUM_FX32( 4 );
    work->shake_count = 0;
    work->seq_no++;
  case 2:
    disc->offs_egg.y -= NUM_FX32( 1 );
    if( disc->offs_egg.y <= NUM_FX32(-8) ){
      work->seq_no++;
    }
    break;
  case 3:
    disc->offs_egg.y += NUM_FX32( 2 );
    if( disc->offs_egg.y >= NUM_FX32(0) ){
      disc->offs_egg.y = 0;
      EggJumpTcb_EggSetAll( g2m );
      work->jump_flag = TRUE;
      work->seq_no++;
      
      PMSND_PlaySE( GURU2_SE_EGG_JUMP );
    }
    break;
  case 4:
    disc->offs.y = work->shake_fx;
    work->shake_fx = -work->shake_fx;
    work->shake_count++;
    if( (work->shake_count&0x03) == 0 ){
      if( work->shake_fx <= NUM_FX32(1) ){
        disc->offs.y = 0;
        work->seq_no++;
      }else{
        work->shake_fx >>= 1;
      }
    }
    break;
  case 5:
    if( EggJumpTcb_JumpCheck(g2m) == FALSE ){
      work->jump_proc_flag = FALSE;
      work->jump_flag = FALSE;
      work->seq_no = 0;
      OS_Printf("�^�}�S���n�����Đ�\n");
      PMSND_PlaySE( GURU2_SE_EGG_LANDING );
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * ���M�����オ��^�X�N
 * @param tcb   GFL_TCB*
 * @param wk    tcb work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggDiscJumpTcb( GFL_TCB* tcb, void *wk )
{
  GURU2MAIN_WORK *g2m = wk;
  EGGDISCJUMPTCB_WORK *work = &g2m->egg_disc_jump_tcb_work;
  EggDiscJump( g2m, work );
}

//--------------------------------------------------------------
/**
 * ���M�����オ��W�����v�^�X�N������ 
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggDiscJumpTcb_Init( GURU2MAIN_WORK *g2m )
{
  EGGDISCJUMPTCB_WORK *work;
  
  work = &g2m->egg_disc_jump_tcb_work;
  GFL_STD_MemFill( work, 0, sizeof(EGGDISCJUMPTCB_WORK) );
  
  g2m->tcb_egg_disc_jump = GFL_TCB_AddTask( g2m->tcbSys, 
      EggDiscJumpTcb, g2m, TCBPRI_EGG_DISC_JUMP );
  GF_ASSERT( g2m->tcb_egg_disc_jump );
}

//--------------------------------------------------------------
/**
 * ���M�����オ��^�X�N�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggDiscJumpTcb_Delete( GURU2MAIN_WORK *g2m )
{
  GFL_TCB_DeleteTask( g2m->tcb_egg_disc_jump );
}

//--------------------------------------------------------------
/**
 * ���M�����オ��^�X�N�N��
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void EggDiscJumpTcb_JumpSet( GURU2MAIN_WORK *g2m )
{
  EGGDISCJUMPTCB_WORK *work;
  work = &g2m->egg_disc_jump_tcb_work;
  GF_ASSERT( work->jump_flag == 0 );
  
  work->jump_proc_flag = TRUE;
  work->seq_no = 1;
}

//--------------------------------------------------------------
/**
 * ���M�����オ��^�X�N�@�W�����v�`�F�b�N
 * @param g2m GURU2MAIN_WORK
 * @retval  BOOL  TRUE=�W�����v��
 */
//--------------------------------------------------------------
static BOOL EggDiscJumpTcb_JumpCheck( GURU2MAIN_WORK *g2m )
{
  EGGDISCJUMPTCB_WORK *work;
  work = &g2m->egg_disc_jump_tcb_work;
  return( work->jump_flag );
}

//--------------------------------------------------------------
/**
 * ���M�����オ��^�X�N�@�������`�F�b�N
 * @param g2m GURU2MAIN_WORK
 * @retval  BOOL  TRUE=�W�����v��
 */
//--------------------------------------------------------------
static BOOL EggDiscJumpTcb_JumpProcCheck( GURU2MAIN_WORK *g2m )
{
  EGGDISCJUMPTCB_WORK *work;
  work = &g2m->egg_disc_jump_tcb_work;
  return( work->jump_proc_flag );
}

//==============================================================================
//  �J�E���g�_�E���^�X�N
//==============================================================================

//--------------------------------------------------------------
/// CDOWNWORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  int frame;
  int end;
  int count;
  GURU2MAIN_WORK *g2m;
  GFL_CLWK *cap;
}CDOWNWORK;

//--------------------------------------------------------------
/**
 * �J�E���g�_�E��TCB
 * @param tcb GFL_TCB*
 * @param wk  tcb work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void CountDownTcb( GFL_TCB* tcb, void *wk )
{
  CDOWNWORK *work = wk;
  
  switch( work->seq_no ){
  case 0:
    if( work->count < 4 ){  //3..2..1..START
      int tbl[4] = {GURU2_SE_COUNT_3,
        GURU2_SE_COUNT_2,GURU2_SE_COUNT_1,GURU2_SE_START};
      work->frame--;
      if( work->frame <= 0 ){
        work->frame = CDOWN_SE_FRAME;
        PMSND_PlaySE( tbl[work->count] );
        work->count++;
      }
    }
    
    GFL_CLACT_WK_AddAnmFrame( work->cap, FX32_ONE+(FX32_ONE/2) );
    
    if( GFL_CLACT_WK_CheckAnmActive(work->cap) == FALSE ){
      work->seq_no++;
    }
    break;
  case 1:
//    CATS_ActorPointerDelete_S( work->cap );
    GFL_CLACT_WK_Remove( work->cap );
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
}

//--------------------------------------------------------------
/**
 * �J�E���g�_�E���^�X�N�ǉ�
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static GFL_TCB* guru2_CountDownTcbSet( GURU2MAIN_WORK *g2m )
{
  CDOWNWORK *work = GFL_HEAP_AllocMemoryLo( HEAPID_GURU2, sizeof(CDOWNWORK) );
  GFL_CLWK_DATA prm;
  GFL_STD_MemFill( work, 0, sizeof(CDOWNWORK) );
  
  prm.pos_x   = CDOWN_CX;
  prm.pos_y   = CDOWN_CY;
  prm.anmseq  = 0;
  prm.bgpri   = 0;
  prm.softpri = 0;
  
  work->cap = GFL_CLACT_WK_Create( g2m->clUnit, 
                                   g2m->resobj[GURU2MAIN_CLACT_RES_CHR],
                                   g2m->resobj[GURU2MAIN_CLACT_RES_PLTT],
                                   g2m->resobj[GURU2MAIN_CLACT_RES_CELL],
                                   &prm, CLSYS_DEFREND_MAIN, HEAPID_GURU2 );
  
//  work->cap = Guru2ClAct_Add( g2m,&prm );
  GFL_CLACT_WK_AddAnmFrame( work->cap, FX32_ONE );
  
  work->g2m = g2m;
  {
   GFL_TCB* tcb = GFL_TCB_AddTask( g2m->tcbSys, CountDownTcb,work,0);
   GF_ASSERT( tcb != NULL );
   return( tcb );
  }
}

//==============================================================================
//  �{�^���A�j���^�X�N
//==============================================================================
//--------------------------------------------------------------
/**
 * �{�^���A�j������
 * @param g2m GURU2MAIN_WORK
 * @param work  BTNANMTCB_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void BtnAnm( GURU2MAIN_WORK *g2m, BTNANMTCB_WORK *work )
{
  switch( work->seq_no ){
  case 0: //�ҋ@
    break;
  case 1: //�����J�n
    Guru2BG_ButtonONSet( g2m );
    PMSND_PlaySE( GURU2_SE_BTN_PUSH );
    work->frame = 0;
    work->seq_no++;
  case 2: //�{�^����������
    work->frame++;
    if( work->frame >= BTN_ANM_FRAME_START ){
      work->frame = 0;
      work->seq_no++;
      Guru2BG_ButtonMiddleSet( g2m );
    }
    break;
  case 3: 
    work->frame++;
    if( work->frame >= BTN_ANM_FRAME_ON ){
      Guru2BG_ButtonONSet( g2m );
      work->frame = 0;
      work->seq_no++;
    }
    break;
  case 4:
    work->frame++;
    if( work->frame >= BTN_ANM_FRAME_BACK ){
      Guru2BG_ButtonOFFSet( g2m );
      work->push_flag = FALSE;
      work->seq_no = 0;
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * �{�^���A�j���^�X�N
 * @param tcb   GFL_TCB*
 * @param wk    tcb work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void BtnAnmTcb( GFL_TCB* tcb, void *wk )
{
  GURU2MAIN_WORK *g2m = wk;
  BTNANMTCB_WORK *work = &g2m->btn_anm_tcb_work;
  BtnAnm( g2m, work );
}

//--------------------------------------------------------------
/**
 * �{�^���A�j���^�X�N������
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void BtnAnmTcb_Init( GURU2MAIN_WORK *g2m )
{
  BTNANMTCB_WORK *work;
  work = &g2m->btn_anm_tcb_work;
  GFL_STD_MemFill( work, 0, sizeof(BTNANMTCB_WORK) );
  g2m->tcb_btn_anm = GFL_TCB_AddTask( g2m->tcbSys,  BtnAnmTcb, g2m, TCBPRI_BTN_ANM );
  GF_ASSERT( g2m->tcb_btn_anm );
}

//--------------------------------------------------------------
/**
 * �{�^���A�j���^�X�N�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void BtnAnmTcb_Delete( GURU2MAIN_WORK *g2m )
{
  GFL_TCB_DeleteTask( g2m->tcb_btn_anm );
}

//--------------------------------------------------------------
/**
 * �{�^���A�j���^�X�N�@�������݃`�F�b�N
 * @param g2m GURU2MAIN_WORK
 * @retval  BOOL  TRUE=�������ݒ�
 */
//--------------------------------------------------------------
static BOOL BtnAnmTcb_PushCheck( GURU2MAIN_WORK *g2m )
{
  BTNANMTCB_WORK *work;
  work = &g2m->btn_anm_tcb_work;
  return( work->push_flag );
}

//--------------------------------------------------------------
/**
 * �{�^���A�j���^�X�N�@����
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void BtnAnmTcb_PushSet( GURU2MAIN_WORK *g2m )
{
  BTNANMTCB_WORK *work;
  work = &g2m->btn_anm_tcb_work;
  GF_ASSERT( work->push_flag == FALSE );
  work->push_flag = TRUE;
  work->seq_no = 1;
}

//==============================================================================
//  ���܂��]�[�����W�����v�^�X�N
//==============================================================================
//--------------------------------------------------------------
/**
 * ���܂����W�����v
 * @param g2m GURU2MAIN_WORK
 * @param work  OMAKEJUMP_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void OmakeEggJump( GURU2MAIN_WORK *g2m, OMAKEJUMP_WORK *work )
{
  switch( work->seq_no ){
  case 0:
    break;
  case 1:
    {
      static int offs[16] = {
        4, 6, 8, 10, 11, 12, 12, 12,
        11, 10, 9, 8, 6, 4, 0, 0 };
      
      work->eact->offs.y = NUM_FX32( offs[work->count] );
      work->count++;
    
      if( work->count >= 16 ){
        work->count = 0;
        work->seq_no++;
      }
    }
    break;
  case 2:
    work->wait++;
    
    if( work->wait >= 15 ){
      work->wait = 0;
      work->seq_no = 1;
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * ���܂����W�����v�^�X�N
 * @param tcb   GFL_TCB*
 * @param wk    tcb work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void OmakeEggJumpTcb( GFL_TCB* tcb, void *wk )
{
  int i;
  GURU2MAIN_WORK *g2m = wk;
  OMAKEJUMPTCB_WORK *work = &g2m->omake_jump_tcb_work;
  OMAKEJUMP_WORK *jump = work->omake_jump_work;
  
  for( i = 0; i < G2MEMBER_MAX; i++, jump++ ){
    OmakeEggJump( g2m, jump );
  }
}

//--------------------------------------------------------------
/**
 * ���܂����W�����v�^�X�N������
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void OmakeEggJumpTcb_Init( GURU2MAIN_WORK *g2m )
{
  OMAKEJUMPTCB_WORK *work;
  work = &g2m->omake_jump_tcb_work;
  GFL_STD_MemFill( work,0,sizeof(OMAKEJUMPTCB_WORK) );
  g2m->tcb_omake_jump = GFL_TCB_AddTask( g2m->tcbSys,  OmakeEggJumpTcb, g2m, TCBPRI_OMAKE_JUMP );
  GF_ASSERT( g2m->tcb_omake_jump );
}

//--------------------------------------------------------------
/**
 * ���܂����W�����v�^�X�N�폜
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void OmakeEggJumpTcb_Delete( GURU2MAIN_WORK *g2m )
{
  GFL_TCB_DeleteTask( g2m->tcb_omake_jump );
}

//--------------------------------------------------------------
/**
 * ���܂����W�����v�^�X�N�ɗ��o�^
 * @param g2m GURU2MAIN_WORK
 * @param act EGGACTOR
 * @retval  nothing
 */
//--------------------------------------------------------------
static void OmakeEggJumpTcb_EggSet( GURU2MAIN_WORK *g2m, EGGACTOR *act )
{
  int id = act->comm_id;
  OMAKEJUMPTCB_WORK *work = &g2m->omake_jump_tcb_work;
  OMAKEJUMP_WORK *ojump = &work->omake_jump_work[id];
  
  GF_ASSERT( id < G2MEMBER_MAX );
  GF_ASSERT( ojump->seq_no == 0 );
  
  ojump->seq_no = 1;
  ojump->eact = act;
}

//--------------------------------------------------------------
/**
 * omake_bit�������ɃW�����v�Z�b�g
 * @param 
 * @retval
 */
//--------------------------------------------------------------
static void OmakeEggJumpTcb_OmakeBitCheckSet( GURU2MAIN_WORK *g2m )
{
  u32 bit = g2m->omake_bit;
  int i = 0, max = g2m->comm.play_max;
  EGGWORK *egg = &g2m->egg;
  
  while( i < max ){
    if( egg->eact[i].use_flag && (bit & (1 << egg->eact[i].comm_id)) ){
      OmakeEggJumpTcb_EggSet( g2m, &egg->eact[i] );
    }
    
    i++;
  }
}

#if 0
static void OmakeEggJumpTcb_AllSet( GURU2MAIN_WORK *g2m )
{
  fx32 pos;
  int i = 0, max = g2m->comm.play_max;
  EGGWORK *egg = &g2m->egg;
  
  while( i < max ){
    if( egg->eact[i].use_flag ){
      pos = egg->eact[i].angle;
      
      if( Guru2MainOmakeZoneCheck(g2m,pos,max) == TRUE ){
        OmakeEggJumpTcb_EggSet( g2m, &egg->eact[i] );
      }
    }
    
    i++;
  }
}
#endif

//==============================================================================
//  �{�^������
//==============================================================================
//--------------------------------------------------------------
/**
 * �{�^���𖾈Â���^�X�N
 * @param g2m GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void BtnFadeTcb( GFL_TCB* tcb, void *wk )
{
  int end = FALSE;
  BTNFADE_WORK *work = wk;
  
  if( work->light_flag == TRUE ){   //���邭
    work->evy--;
    
    if( (int)work->evy <= 0 ){
      work->evy = 0;
      end = TRUE;
    }
  }else{
    work->evy++;
    
    if( (int)work->evy >= 8 ){
      work->evy = 8;
      end = TRUE;
    }
  }
  
  Guru2PlttFade_BtnFade( work->g2m, work->evy );
  
  if( end == TRUE ){
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
}

//--------------------------------------------------------------
/**
 * �{�^�����Ã^�X�N�ǉ�
 * @param g2m   GURU2MAIN_WORK
 * @param light TURE=���邭 FALSE=�Â�
 * @retval  nothing
 */
//--------------------------------------------------------------
static void BtnFadeTcbAdd( GURU2MAIN_WORK *g2m, int light )
{
  GFL_TCB* tcb;
  u32 evy[2] = { 0, 8 };
  
  BTNFADE_WORK *work = GFL_HEAP_AllocMemoryLo( HEAPID_GURU2, sizeof(BTNFADE_WORK) );
  
  work->g2m = g2m;
  work->light_flag = light;
  work->evy = evy[light];
  
  tcb = GFL_TCB_AddTask( g2m->tcbSys,  BtnFadeTcb, work, 0 );
  GF_ASSERT( tcb != NULL );
}

//==============================================================================
//  �p�[�c
//==============================================================================
//--------------------------------------------------------------
/**
 * �p�x����
 * @param angle �p�x
 * @param add   �����l
 * @retval  nothing
 */
//--------------------------------------------------------------
static void AngleAdd( fx32 *angle, fx32 add )
{
  (*angle) += add;
  while( (*angle) < 0 ){ (*angle) += NUM_FX32(360); }
  (*angle) %= NUM_FX32( 360 );
}

//--------------------------------------------------------------
/**
 * �M����]�����A���𓮂����B
 * @param gm  GURU2MAIN_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DiscRotateEggMove( GURU2MAIN_WORK *g2m, fx32 speed )
{
  Disc_Rotate( &g2m->disc, speed );
  EggAct_Rotate( g2m, speed );
}

//--------------------------------------------------------------
/**
 * �Q�[�����Ԃ̐i�s
 * @param g2m GURU2MAIN_WORK
 * @retval  BOOL  TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL Guru2GameTimeCount( GURU2MAIN_WORK *g2m )
{
  g2m->game_frame++;
  
  if( g2m->game_frame < GURU2_GAME_FRAME ){
    return( FALSE );
  }
  
  g2m->game_frame = GURU2_GAME_FRAME;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^���o��
 * @param g2m   GURU2MAIN_WORK
 * @param idx   �f�[�^�C���f�b�N�X
 * @param fb    TRUE=�O,FALSE=��납��̈�m��
 * @retval  void* ���[�h�f�[�^
 */
//--------------------------------------------------------------
static void * Guru2Arc_DataLoad( GURU2MAIN_WORK *g2m, u32 idx, BOOL fb )
{
  void *buf;
  u32 size = GFL_ARC_GetDataSizeByHandle( g2m->arc_handle, idx );
  
  if( fb == TRUE ){
    buf = GFL_HEAP_AllocMemory( HEAPID_GURU2, size );
  }else{
    buf = GFL_HEAP_AllocMemoryLo( HEAPID_GURU2, size );
  }
  
  GFL_ARC_LoadDataByHandle( g2m->arc_handle, idx, buf );
  return( buf );
}

//--------------------------------------------------------------
/**
 * �L�[�g���K�`�F�b�N
 * @param pad   PAD_BUTTON_A��
 * @retval  BOOL  TRUE=�q�b�g
 */
//--------------------------------------------------------------
static BOOL Guru2Pad_TrgCheck( u32 pad )
{
  u32 ret = GFL_UI_KEY_GetTrg() & pad;
  if( ret ){ return( TRUE ); }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �L�[�R���e�j���[�`�F�b�N
 * @param pad   PAD_BUTTON_A��
 * @retval  BOOL  TRUE=�q�b�g
 */
//--------------------------------------------------------------
static BOOL Guru2Pad_ContCheck( u32 pad )
{
  u32 ret = GFL_UI_KEY_GetCont() & pad;
  if( ret ){ return( TRUE ); }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �ʐM���瑗���Ă���{�^�������i�[
 * @param g2m   GURU2MAIN_WORK
 * @param btn   �{�^�����
 * @retval  nothing
 */
//--------------------------------------------------------------
void Guru2Main_CommButtonSet( GURU2MAIN_WORK *g2m, u32 btn )
{
  if( btn != 0 ){
    g2m->comm.push_btn = btn;
  }
}

//--------------------------------------------------------------
/**
 * �ʐM���瑗���Ă���Q�[�������i�[
 * @param g2m GURU2MAIN
 * @param data  GURU2COMM_GAMEDATA
 * @retval  nothing
 */
//--------------------------------------------------------------
void Guru2Main_CommGameDataSet(
    GURU2MAIN_WORK *g2m, const GURU2COMM_GAMEDATA *data )
{
  g2m->comm.game_data_send_flag = TRUE;
  g2m->comm.game_data = *data;

#ifdef DEBUG_GURU2_PRINTF_COMM
  {
    static int d_cnt = 0;
    OS_Printf("[%03d] push_btn=%d rotate=0x%x comm_rotate=0x%x comm_egg_angle[0]=0x%x comm_egg_joffs=0x%x \n",
        d_cnt++, 
        g2m->comm.push_btn,
        g2m->disc.rotate_fx, 
        data->disc_angle,
        data->egg_angle[0],
        data->egg_joffs
        );
  }
#endif
}

//--------------------------------------------------------------
/**
 * �e���|�������[�N������
 * @param g2m GURU2MAIN_WORK
 * @retval  void*
 */
//--------------------------------------------------------------
static void * Guru2MainTempWorkInit( GURU2MAIN_WORK *g2m, u32 size )
{
  GF_ASSERT( size < G2M_TEMPWORK_SIZE );
  GFL_STD_MemFill( g2m->temp_work, 0, G2M_TEMPWORK_SIZE );
  return( g2m->temp_work );
}

//--------------------------------------------------------------
/**
 * �e���|�������[�N�擾
 * @param g2m GURU2MAIN_WORK
 * @retval  void*
 */
//--------------------------------------------------------------
static void * Guru2MainTempWorkGet( GURU2MAIN_WORK *g2m )
{
  return( g2m->temp_work );
}

//--------------------------------------------------------------
/**
 * �e������V�O�i���r�b�g�`�F�b�N
 * @param g2m GURU2MAIN_WORK
 * @retval  BOOL  TRUE=���ߐ؂�
 */
//--------------------------------------------------------------
static BOOL Guru2MainCommSignalCheck(GURU2MAIN_WORK *g2m, u16 bit)
{
  u32 signal = g2m->g2c->comm_game_signal_bit;
//  OS_Printf("comm_game_signal_bit=%x\n",signal);

  if( (signal&bit) ){ return( TRUE ); }
  return( FALSE );
}


//--------------------------------------------------------------
/**
 * �Q�[���Q���l���`�F�b�N
 * @param g2m GURU2MAIN_WORK
 * @retval  BOOL  TRUE �Q���l����v
 */
//--------------------------------------------------------------
static BOOL Guru2MainCommJoinNumCheck( GURU2MAIN_WORK *g2m )
{
  int in = 0;
  int max = g2m->g2p->receipt_num-1;  //-1=���g
  u32 bit = g2m->g2c->comm_game_join_bit;
  
  while( bit ){
    in += (bit&0x01);
    bit >>= 1;
  }
  
  if( in >= max ){
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �Q�[���Q���l���擾
 * @param g2m GURU2MAIN_WORK
 * @retval  int TRUE �Q���l����v
 */
//--------------------------------------------------------------
static int Guru2MainCommJoinNumGet( GURU2MAIN_WORK *g2m )
{
  int in = 0;
  u32 bit = g2m->g2c->comm_game_join_bit; //�q�̎Q����
  
  while( bit ){
    in += (bit&0x01);
    bit >>= 1;
  }
  
  return( in );
}


//--------------------------------------------------------------
/**
 * �����Ă����v���C�ԍ����Z�b�g
 * @param g2m   GURU2MAIN_WORK
 * @param no  GURU2COMM_PLAYNO
 * @retval  nothing
 */
//--------------------------------------------------------------
void Guru2Main_CommPlayNoDataSet(
    GURU2MAIN_WORK *g2m, const GURU2COMM_PLAYNO *no )
{
  g2m->comm.play_no_tbl[no->play_no] = *no;
  OS_Printf("play_no=%d comm_id=%d\n", no->play_no, no->comm_id);
  
  if( no->comm_id == GFL_NET_SystemGetCurrentID() ){
    g2m->comm.my_play_no = no->play_no;
    #ifdef DEBUG_GURU2_PRINTF
    OS_Printf( "���邮��@�����̃v���C�ԍ�(%d)��M\n", no->play_no );
    #endif
  }
}

//--------------------------------------------------------------
/**
 * �����Ă����v���C�ő�l�����Z�b�g
 * @param g2m   GURU2MAIN_WORK
 * @param max   �v���C�ő�l��
 * @retval  nothing
 */
//--------------------------------------------------------------
void Guru2Main_CommPlayMaxSet( GURU2MAIN_WORK *g2m, int max )
{
//  g2m->comm.play_max = max;
  g2m->comm.play_max = Union_App_GetMemberNum(_get_unionwork(g2m));
}

//--------------------------------------------------------------
/**
 * �w��ID���Q�[���Q���҂��ǂ����`�F�b�N
 * @param g2m
 * @param id
 * @retval  nothing
 */
//--------------------------------------------------------------
static BOOL Guru2MainCommIDPlayCheck( GURU2MAIN_WORK *g2m, int id )
{
  if( id != 0 ){ //�e�͖������ŎQ��
    u32 bit = 1 << (u32)id;
    if( (g2m->g2c->comm_game_join_bit&bit) == 0 ){
      return( FALSE );
    }
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���܂��f�[�^����
 * @param g2m   GURU2MAIN_WORK
 * @param id    id=�������ID
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Guru2MainFriendEggExchange( GURU2MAIN_WORK *g2m, int id )
{
  int m_pos,f_pos;
  POKEPARTY *m_poke,*f_poke;
  POKEMON_PARAM *m_pp,*f_pp;
//  POKEMON_PARAM *mc_pp,*fc_pp;
  
  m_poke = g2m->my_poke_party;
  f_poke = (POKEPARTY *)Guru2Comm_FriendPokePartyGet( g2m->g2c, id );
  
  m_pos = g2m->g2p->trade_no;
  f_pos = g2m->comm.trade_no_tbl[id];
  
  m_pp = PokeParty_GetMemberPointer( m_poke, m_pos );
  f_pp = PokeParty_GetMemberPointer( f_poke, f_pos );
  
  // �g���[�i�[��������
  POKE_MEMO_SetTrainerMemoPP( f_pp, POKE_MEMO_EGG_TRADE,
                              Union_App_GetMystatus( _get_unionwork(g2m), GFL_NET_SystemGetCurrentID() ), 
                              POKE_MEMO_PLACE_GAME_TRADE, HEAPID_GURU2 );
  
  POKETOOL_CopyPPtoPP( f_pp, m_pp ); //�R�s�[��,�R�s�[��
}

//--------------------------------------------------------------
/**
 * �^�}�S�f�[�^��M���擾
 * @param g2m GURU2MAIN_WORK
 * @retval  int
 */
//--------------------------------------------------------------
static int Guru2MainCommEggDataNumGet( GURU2MAIN_WORK *g2m )
{
  int in = 0;
  u32 bit = g2m->g2c->comm_game_egg_recv_bit;
  
  while( bit ){
    in += (bit&0x01);
    bit >>= 1;
  }
  
  return( in );
}

//--------------------------------------------------------------
/**
 * �����ʒu�Z�b�g
 * @param g2m GURU2MAIN_WORK
 * @retval  int
 */
//--------------------------------------------------------------
void Guru2Main_TradeNoSet( GURU2MAIN_WORK *g2m, int id, int no )
{
  g2m->comm.trade_no_tbl[id] = no;
}

//--------------------------------------------------------------
/**
 * �Q�[�����ʃZ�b�g
 * @param g2m GURU2MAIN_WORK
 * @param result  GURU2COMM_GAMERESULT
 * @retval  int
 */
//--------------------------------------------------------------
void Guru2Main_GameResultSet(
  GURU2MAIN_WORK *g2m, const GURU2COMM_GAMERESULT *result )
{
  g2m->comm.game_result = *result;
  g2m->comm.game_result_get_flag = TRUE;
}

//--------------------------------------------------------------
/**
 * ���܂��]�[���`�F�b�N
 * @param g2m   GURU2MAIN_WORK
 * @param pos_fx  �p�x0-359
 * @param max   �v���C�l���ő�
 * @retval  BOOL  TRUE=������
 */
//--------------------------------------------------------------
static BOOL Guru2MainOmakeZoneCheck(
    GURU2MAIN_WORK *g2m, fx32 pos_fx, int max )
{
  int count;
  fx32 d_pos;
  u32 pos0,pos1;
  const OMAKE_AREA_TBL *tbl;
  const OMAKE_AREA *area;
  
  #ifdef DEBUG_DISC_NO
  max = DEBUG_DISC_NO;
  #endif
  
  AngleAdd( &pos_fx, -g2m->disc.rotate_fx );            //�f�B�X�N�ړ����␳
  AngleAdd( &pos_fx, -g2m->disc.rotate_draw_offs_fx );  //�f�B�X�N�\���I�t�Z�b�g�����炷

  OS_Printf("rotate=0x%x(%d) comm_rotate=0x%x \n", g2m->disc.rotate_fx, FX32_NUM(g2m->disc.rotate_fx), g2m->comm.game_data.disc_angle );
  
  d_pos = pos_fx;
  AngleAdd( &d_pos, EGG_ATARI_HABA_L_FX );
  pos0 = FX32_NUM( d_pos );
  
  d_pos = pos_fx;
  AngleAdd( &d_pos, EGG_ATARI_HABA_R_FX );
  pos1 = FX32_NUM( d_pos );
  
  #ifndef DEBUG_DISP_CHECK
  OS_Printf("���邮�� ���܂��G���A�`�F�b�N " );
  OS_Printf( "pos_fx=%d,pos0=%d,pos1=%d,max=%d\n",FX32_NUM(pos_fx),pos0,pos1,max);
  #endif
  
  tbl = &DATA_OmakeAreaTbl[max];
  count = tbl->max;
  area = tbl->area;
  
  while( count ){
    OS_Printf("area_start=%d area_end=%d result=%d \n",area->start, area->end, 
       ( (pos0 >= area->start && pos0 <= area->end) || (pos1 >= area->start && pos1 <= area->end) ) 
    );
    
    if( (pos0 >= area->start && pos0 <= area->end) ||
      (pos1 >= area->start && pos1 <= area->end) ){
      return( TRUE );
    }
    
    area++;
    count--;
  }
  
  return( FALSE );
}

#if 0 //��_�`�F�b�N�̂�
static BOOL Guru2MainOmakeZoneCheck(
    GURU2MAIN_WORK *g2m, fx32 pos_fx, int max )
{
  u32 pos;
  int count;
  fx32 d_offs;
  const OMAKE_AREA_TBL *tbl;
  const OMAKE_AREA *area;
  
  #ifdef DEBUG_DISC_NO
  max = DEBUG_DISC_NO;
  #endif
  
  AngleAdd( &pos_fx, -g2m->disc.rotate_fx );    //�f�B�X�N�ړ����␳
  pos = FX32_NUM( pos_fx );
  
  #ifndef DEBUG_DISP_CHECK
  OS_Printf("���邮�� ���܂��G���A�`�F�b�N pos=%d,max=%d\n",pos,max);
  #endif
  
  tbl = &DATA_OmakeAreaTbl[max];
  count = tbl->max;
  area = tbl->area;
  
  while( count ){
    if( area->start <= pos && area->end >= pos ){
      return( TRUE );
    }
    
    area++;
    count--;
  }
  
  return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * �_���^�}�S�`�F�b�N 
 * @param ppty  POKEPARTY 
 * @retval  BOOL  TRUE==�_���^�}�S�L��
 */
//--------------------------------------------------------------
static BOOL Guru2MainPokePartyDameTamagoCheck( POKEPARTY *ppty )
{
  int i,max;
  POKEMON_PARAM *pp;
  
  max = PokeParty_GetPokeCount( ppty );
  
  for( i = 0; i < max; i++ ){
    pp = PokeParty_GetMemberPointer( ppty, i );
    
    if( PP_Get(pp,ID_PARA_tamago_flag,NULL) ){
      if( PP_Get(pp,ID_PARA_fusei_tamago_flag,NULL) ){
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �莝���Ƒ���̃_���^�}�S�`�F�b�N
 * @param g2m GURU2MAIN_WORK
 * @retval  BOOL TRUE==�_���^�}�S�L��
 */
//--------------------------------------------------------------
static BOOL Guru2MainDameTamagoCheck( GURU2MAIN_WORK *g2m )
{
  //�莝��
  if( Guru2MainPokePartyDameTamagoCheck(g2m->my_poke_party) == TRUE ){
    return( TRUE );
  }
  
  { //����
    int id;
    POKEPARTY *pty;
    
    for( id = 0; id < G2MEMBER_MAX; id++ ){
      if( Guru2MainCommIDPlayCheck(g2m,id) ){
        pty = Guru2Comm_FriendPokePartyGet( g2m->g2c, id );
        if( Guru2MainPokePartyDameTamagoCheck(pty) == TRUE ){
          return( TRUE );
        }
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���f�[�^�`�F�b�N�@OK�����擾
 * @param g2m GURU2MAIN_WORK *
 * @retval  int OK��M��
 */
//--------------------------------------------------------------
static int Guru2MainCommEggDataOKCountCheck( GURU2MAIN_WORK *g2m )
{
  int count = 0;
  u32 bit = g2m->g2c->comm_game_egg_check_ok_bit;
  
  while( bit ){
    count += bit & 0x01;
    bit >>= 1;
  }
  
  return( count );
}

//--------------------------------------------------------------
/**
 * ���f�[�^�`�F�b�N�@�G���[����
 * @param g2m GURU2MAIN_WORK *
 * @retval  BOOL TRUE=�_���^�}�S���������݂��Ă���
 */
//--------------------------------------------------------------
static BOOL Guru2MainCommEggDataNGCheck( GURU2MAIN_WORK *g2m )
{
  int count = 0;
  u32 bit = g2m->g2c->comm_game_egg_check_error_bit;
  
  if( bit ){
    return( TRUE );
  }
  
  return( FALSE );
}

//----------------------------------------------------------------------------------
/**
 * @brief ME�Đ�
 *
 * @param   seq_bgm   ME�Ƃ��čĐ�������BGM
 */
//----------------------------------------------------------------------------------
static void _me_play( int seq_bgm )
{
  //���݂̂a�f�l���ꎞ��~(TRUE��~,FALSE�ĊJ)
  PMSND_PauseBGM( TRUE );  

  //���݂̂a�f�l��ޔ�
  PMSND_PushBGM();

  PMSND_PlayBGM( seq_bgm );
}

//----------------------------------------------------------------------------------
/**
 * @brief ME�I�����o��BGM���A
 *
 * @param   none    
 *
 * @retval  BOOL    TRUE:�Đ��� FALSE:�I��
 */
//----------------------------------------------------------------------------------
static BOOL _me_end_check( void )
{

  // ME�Ƃ��čĐ����Ă���BGM�̏I�����o
  if(PMSND_CheckPlayBGM()==FALSE){
    //�ޔ������a�f�l�𕜌�
    PMSND_PopBGM();

    //�ꎞ��~����BGM���ĊJ(TRUE��~,FALSE�ĊJ)
    PMSND_PauseBGM( FALSE );  
    return FALSE;
  }

  return TRUE;
}


//-----------------------------------------------------------------------------
/**
 *	@brief  �e�����i�f�B�X�N�A����]�j����]��񑗐M
 *
 *	@param	GURU2MAIN_WORK *g2m
 *	@param	speed 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _comm_parent_rotate( GURU2MAIN_WORK *g2m, fx32 speed )
{
  int i;
  GURU2COMM_GAMEDATA data;

  if( GFL_NET_SystemGetCurrentID() != 0 )
  {
    return;
  }

  DiscRotateEggMove( g2m, speed );
  data.disc_angle = ( g2m->disc.rotate_fx );
  data.egg_joffs = g2m->egg.eact[0].joffs;

  for( i=0; i<g2m->comm.play_max; i++ )
  {
    EGGWORK* egg = &g2m->egg;
    data.egg_angle[i] = FX32_NUM(egg->eact[i].angle);
  }

  Guru2Comm_SendData( g2m->g2c,
    G2COMM_GM_GAMEDATA, &data, sizeof(GURU2COMM_GAMEDATA) );
}






//==============================================================================
//  data
//==============================================================================

//--------------------------------------------------------------
/// �r�b�g�}�b�v�E�B���h�E�@���O�@�l�� 2
//--------------------------------------------------------------
static const BMPWIN_DAT DATA_Guru2BmpNameWin_2[2] =
{
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_2_0,BMPPOS_NAME_Y_2_0,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(0),
  },
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_2_1,BMPPOS_NAME_Y_2_1,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(1),
  },
};

//--------------------------------------------------------------
/// �r�b�g�}�b�v�E�B���h�E�@���O�@�l�� 3
//--------------------------------------------------------------
static const BMPWIN_DAT DATA_Guru2BmpNameWin_3[3] =
{
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_3_0,BMPPOS_NAME_Y_3_0,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(0),
  },
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_3_1,BMPPOS_NAME_Y_3_1,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(1),
  },
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_3_2,BMPPOS_NAME_Y_3_2,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(2),
  },
};

//--------------------------------------------------------------
/// �r�b�g�}�b�v�E�B���h�E�@���O�@�l�� 4
//--------------------------------------------------------------
static const BMPWIN_DAT DATA_Guru2BmpNameWin_4[4] =
{
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_4_0,BMPPOS_NAME_Y_4_0,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(0),
  },
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_4_1,BMPPOS_NAME_Y_4_1,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(1),
  },
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_4_2,BMPPOS_NAME_Y_4_2,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(2),
  },
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_4_3,BMPPOS_NAME_Y_4_3,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(3),
  },
};

//--------------------------------------------------------------
/// �r�b�g�}�b�v�E�B���h�E�@���O�@�l�� 5
//--------------------------------------------------------------
static const BMPWIN_DAT DATA_Guru2BmpNameWin_5[5] =
{
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_5_0,BMPPOS_NAME_Y_5_0,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(0),
  },
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_5_1,BMPPOS_NAME_Y_5_1,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(1),
  },
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_5_2,BMPPOS_NAME_Y_5_2,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(2),
  },
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_5_3,BMPPOS_NAME_Y_5_3,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(3),
  },
  {
    BGF_M_KAIWA,
    BMPPOS_NAME_X_5_4,BMPPOS_NAME_Y_5_4,
    BMPSIZE_NAME_X,BMPSIZE_NAME_Y,
    BGF_PANO_MENU_WIN, BGF_BMPCHAR_NAME(4),
  },
};

//--------------------------------------------------------------
/// �r�b�g�}�b�v�E�B���h�E�@���O�@�l����
//--------------------------------------------------------------
static const BMPWIN_DAT * const DATA_Guru2BmpNameWinTbl[G2MEMBER_MAX+1] =
{
  NULL,         //�_�~�[�f�[�^
  NULL,         //�_�~�[�f�[�^
  DATA_Guru2BmpNameWin_2,
  DATA_Guru2BmpNameWin_3,
  DATA_Guru2BmpNameWin_4,
  DATA_Guru2BmpNameWin_5,
};

//--------------------------------------------------------------
//  �l���ʎM�����p�x
//--------------------------------------------------------------
static const u16 DATA_DiscOffsetAngle[G2MEMBER_MAX+1][G2MEMBER_MAX] =
{
  {0,0,0,0,0},    //0 �_�~�[
  {0,0,0,0,0},    //1 �_�~�[
  {0,180,0,0,0},    //2
  {0,240,120,0,0},  //3
  {0,270,180,90,0}, //4
  {0,288,216,144,72}, //5
};

//--------------------------------------------------------------
//  �l���ʗ������ʒu
//--------------------------------------------------------------
static const u16 DATA_EggStartAngle[G2MEMBER_MAX+1][G2MEMBER_MAX] =
{
  {0,0,0,0,0},      //0 �_�~�[
  {0,0,0,0,0},      //1 �_�~�[
  {90,270,0,0,0},     //2
  {90,210,330,0,0},   //3
  {90,180,270,0,0},   //4 
  {90,162,234,306,18},  //5
};

//--------------------------------------------------------------
/// �l���ʖ��O�\���I�t�Z�b�g
//--------------------------------------------------------------
static const int DATA_NameWinNoOffset[G2MEMBER_MAX+1][G2MEMBER_MAX] =
{
  {0,0,0,0,0},      //0 �_�~�[
  {0,0,0,0,0},      //1 �_�~�[
  {0,1,0,0,0},      //2
  {0,2,1,0,0},      //3
  {0,0,0,0,0},      //4
  {0,0,0,0,0},      //5
};

#if 0
//--------------------------------------------------------------
/// �I���p�x���p�x���嗦
//--------------------------------------------------------------
static const u16 DATA_GameEndAngle[G2MEMBER_MAX+1] =
{
  0,    //0�@�_�~�[
  0,    //1�@�_�~�[
  180,  //2
  120,  //3
  90,   //4
  72,   //5
};
#endif

//--------------------------------------------------------------
/// ���܂��G���A
//--------------------------------------------------------------
static const OMAKE_AREA DATA_OmakeArea2 = ///<��l
{ 0, 180 };

static const OMAKE_AREA DATA_OmakeArea3[2] =  ///<�O�l
{
  { 0, 198 },
  { 342, 359 },
};

static const OMAKE_AREA DATA_OmakeArea4[2] =  ///<�l�l
{
  { 37, 142 },
  { 215, 323 },
};

static const OMAKE_AREA DATA_OmakeArea5[2] =  ///<�ܐl
{
  { 0, 215 },
  { 323, 359 },
};

///���܂��G���A�܂Ƃ�
static const OMAKE_AREA_TBL DATA_OmakeAreaTbl[G2MEMBER_MAX+1] =
{
  { 0, NULL },      //dummy
  { 0, NULL },      //dummy
  { 1, &DATA_OmakeArea2 },  //2
  { 2, DATA_OmakeArea3 }, //3
  { 2, DATA_OmakeArea4 }, //4
  { 2, DATA_OmakeArea5 }, //5
};

//--------------------------------------------------------------
/// �؂̎��e�[�u��
//--------------------------------------------------------------
static const u32 DATA_KinomiTbl[G2MEMBER_MAX+1][2] =
{
  {   0,   0 },  //0 dummy
  {   0,   0 },  //1 dummy
  { 159, 163 },  //2 �t�B���̂݁`�C�A�̂�
  { 159, 163 },  //3 �t�B���̂݁`�C�A�̂�
  { 149, 153 },  //4 �N���{�̂݁`�i�i�V�̂�
  { 169, 174 },  //5 �U�N���̂݁`�}�g�}�̂�
};


//--------------------------------------------------------------
/// �l���ʎM�`��p�x�I�t�Z�b�g
//--------------------------------------------------------------
static const fx32 DATA_DiscRotateDrawOffset[G2MEMBER_MAX+1] =
{
  0,  //0 dummy
  0,  //1 dummy
  DISC_ROTATE_DRAW_OFFS_FX_2,
  DISC_ROTATE_DRAW_OFFS_FX_3,
  DISC_ROTATE_DRAW_OFFS_FX_4,
  DISC_ROTATE_DRAW_OFFS_FX_5,
};

//==============================================================================
//  debug
//==============================================================================
#ifdef DEBUG_DISP_CHECK
static void DEBUG_CameraTest( GURU2MAIN_WORK *g2m );
static void DEBUG_DiscMove( GURU2MAIN_WORK *g2w );
static void DEBUG_OmakeCheck( GURU2MAIN_WORK *g2m );
#endif

#ifdef GURU2_DEBUG_ON

//--------------------------------------------------------------
/**
 * �J�������f
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void guru2_CameraSet( GURU2MAIN_WORK *g2m )
{
  CAMERAWORK *cm = &g2m->camera;
  GFL_G3D_CAMERA_SetTarget( cm->gf_camera, &cm->target_pos );
//  GFC_BindCameraTarget( &cm->target_pos, cm->gf_camera );
//  GFC_SetCameraAngleRev( &cm->angle, cm->gf_camera );
//  GFC_SetCameraDistance( cm->distance, cm->gf_camera );
//  GFC_SetCameraPerspWay( cm->persp, cm->gf_camera );  
}


static void DEBUG_WorkInit( GURU2MAIN_WORK *g2m )
{
  DEBUGWORK *dw = &g2m->debug;
  GFL_STD_MemFill( dw, 0, sizeof(DEBUGWORK) );
  
  dw->angle_x  = CM_ANGLE_X;
  dw->angle_y  = CM_ANGLE_Y;
  dw->angle_z  = CM_ANGLE_Z;
  dw->persp    = CM_PERSP;
  dw->distance = CAMERA_DISTANCE;
}

static void DEBUG_Proc( GURU2MAIN_WORK *g2m )
{
  DEBUGWORK *dw = &g2m->debug;
  
  #ifdef DEBUG_DISP_CHECK
  if( Guru2Pad_TrgCheck(PAD_BUTTON_SELECT) ){
    dw->type++;
    dw->type %=3;
    
    switch( dw->type ){
    case 0: OS_Printf( "���邮��@�J�����e�X�g\n" ); break;
    case 1: OS_Printf( "���邮��@�M�e�X�g\n" ); break;
    case 2: OS_Printf( "���邮��@���܂��G���A�e�X�g\n" );
    }
  }
  
  if( dw->type == 0 ){
    DEBUG_CameraTest( g2m );
  }else if( dw->type == 1 ){
    DEBUG_DiscTest( g2m );
  }else if( dw->type == 2 ){
    DEBUG_OmakeCheck( g2m );
  }
  #endif  //DEBUG_DISP_CHECK
}
#endif //GURU2_DEBUG_ON

#ifdef DEBUG_DISP_CHECK
//--------------------------------------------------------------
//  �J�����e�X�g
//--------------------------------------------------------------
static void DEBUG_CameraTest( GURU2MAIN_WORK *g2m )
{
  DEBUGWORK *dw = &g2m->debug;
  CAMERAWORK *cm = &g2m->camera;
  
  if( Guru2Pad_ContCheck(PAD_BUTTON_Y) ){   //�J�����A���O��
    if( Guru2Pad_ContCheck(PAD_BUTTON_R) ){
      if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
        dw->angle_z -= 1;
      }else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
        dw->angle_z += 1;
      }
    }else{
      if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
        dw->angle_y -= 1;
      }else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
        dw->angle_y += 1;
      }else if( Guru2Pad_ContCheck(PAD_KEY_LEFT) ){
        dw->angle_x -= 1;
      }else if( Guru2Pad_ContCheck(PAD_KEY_RIGHT) ){
        dw->angle_x += 1;
      }
    }
  }else if( Guru2Pad_ContCheck(PAD_BUTTON_A) ){   //�J��������
    if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
      cm->distance -= FX32_ONE;
    }else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
      cm->distance += FX32_ONE;
    }
  }else if( Guru2Pad_ContCheck(PAD_BUTTON_B) ){   //�J�����p�[�X
    if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
      dw->persp -= 1;
    }else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
      dw->persp += 1;
    }
  }
  
  dw->angle_x %= 360;
  dw->angle_y %= 360;
  dw->angle_z %= 360;
  dw->persp %= 360;
  
  if( Guru2Pad_TrgCheck(PAD_BUTTON_X) ){
    OS_Printf( "���邮��@�J�����ݒ� ID=%d ", GFL_NET_SystemGetCurrentID() );
    OS_Printf( "angle X=%d, Y=%d, Z=%d, ",
      dw->angle_x,dw->angle_y,dw->angle_z );
    OS_Printf( "distance 0x%x : ", cm->distance );
    OS_Printf( "persp %d\n", dw->persp ); 
  }
  
  cm->angle.x = FX_GET_ROTA_NUM( dw->angle_x );
  cm->angle.y = FX_GET_ROTA_NUM( dw->angle_y );
  cm->angle.z = FX_GET_ROTA_NUM( dw->angle_z );
  cm->persp = FX_GET_ROTA_NUM( dw->persp );
  
  guru2_CameraSet( g2m );
}
#endif  //DEBUG_DISP_CHECK

#ifdef DEBUG_DISP_CHECK
void DEBUG_DiscTest( GURU2MAIN_WORK *g2m )
{
  DISCWORK *disc = &g2m->disc;
  
  if( Guru2Pad_ContCheck(PAD_BUTTON_Y) ){ //�M�p�x
    if( Guru2Pad_ContCheck(PAD_BUTTON_R) ){
      if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
        disc->rotate.z -= 1;
      }else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
        disc->rotate.z += 1;
      }
    }else{
      if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
        disc->rotate_fx -= NUM_FX32( 1 );
      }else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
        disc->rotate_fx += NUM_FX32( 1 );
      }else if( Guru2Pad_ContCheck(PAD_KEY_LEFT) ){
        disc->rotate.x -= 1;
      }else if( Guru2Pad_ContCheck(PAD_KEY_RIGHT) ){
        disc->rotate.x += 1;
      }
    }
  }else{                    //�M���W
    if( Guru2Pad_ContCheck(PAD_BUTTON_R) ){
      if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
        disc->pos.z -= FX32_ONE;
      }else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
        disc->pos.z += FX32_ONE;
      }
    }else{
      if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
        disc->pos.y -= FX32_ONE;
      }else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
        disc->pos.y += FX32_ONE;
      }else if( Guru2Pad_ContCheck(PAD_KEY_LEFT) ){
        disc->pos.x -= FX32_ONE;
      }else if( Guru2Pad_ContCheck(PAD_KEY_RIGHT) ){
        disc->pos.x += FX32_ONE;
      }
    }
  }
  
  disc->rotate.x %= 360;
  disc->rotate.y %= 360;
  disc->rotate.z %= 360;
  
  if( Guru2Pad_TrgCheck(PAD_BUTTON_X) ){
    OS_Printf( "���邮��@�M�ݒ� ID=%d ", GFL_NET_SystemGetCurrentID() );
    OS_Printf( "disc X=%d, Y=%d, Z=%d, AX=%d, AY=%d, AZ=%d\n",
      disc->pos.x/FX32_ONE,disc->pos.y/FX32_ONE,disc->pos.z/FX32_ONE,
      disc->rotate.x,FX32_NUM(disc->rotate_fx),disc->rotate.z );
  }
}
#endif  //DEBUG_DISP_CHECK

#ifdef DEBUG_DISP_CHECK
static void DEBUG_OmakeCheck( GURU2MAIN_WORK *g2m )
{
  int hit = FALSE;
  fx32 pos;
  EGGACTOR *eact;
  DISCWORK *disc = &g2m->disc;
  
  if( Guru2Pad_ContCheck(PAD_BUTTON_Y) ){   //�M�p�x
    if( Guru2Pad_ContCheck(PAD_KEY_UP) ){
      Disc_Rotate( disc, NUM_FX32(1) );
    }else if( Guru2Pad_ContCheck(PAD_KEY_DOWN) ){
      Disc_Rotate( disc, NUM_FX32(-1) );
    }
  }
  
  eact = EggAct_FrontEggActGet( g2m );
  pos = eact->angle;
  hit = Guru2MainOmakeZoneCheck( g2m, pos, g2m->comm.play_max );
  
  if( Guru2Pad_TrgCheck(PAD_BUTTON_X) ){
    int d_pos,e_pos,h_pos;
    fx32 dmy;
    
    if( hit == TRUE ){
      OS_Printf( "���邮��@���܂��G���A�@������I�@" );
    }else{
      OS_Printf( "���邮��@���܂��G���A�@�͂���c�@" );
    }
    
    d_pos = FX32_NUM( disc->rotate_fx );
    e_pos = FX32_NUM( pos );
    
    dmy = pos;
    AngleAdd( &dmy, -disc->rotate_fx );
    h_pos = FX32_NUM( dmy );
    
    OS_Printf( "ID = %d, disc angle = %d(%d), egg pos = %d(%d,%d)\n",
      GFL_NET_SystemGetCurrentID(),
      d_pos, 360-d_pos, e_pos, 360-e_pos, h_pos );
  }
}
#endif  //DEBUG_DISP_CHECK

#ifdef PM_DEBUG
static void print_all_egg_data( GURU2MAIN_WORK* g2m )
{
  int id;
  int m_pos,f_pos;
  POKEPARTY *m_poke,*f_poke;
  POKEMON_PARAM *m_pp,*f_pp;

  m_poke = g2m->my_poke_party;
  m_pos = g2m->g2p->trade_no;
  m_pp = PokeParty_GetMemberPointer( m_poke, m_pos );
  OS_TPrintf( "�����̗� monsno= %d \n", PP_Get( m_pp, ID_PARA_monsno, NULL ) );

  for( id=0; id<g2m->comm.play_max; id++ )
  {
    f_poke = (POKEPARTY *)Guru2Comm_FriendPokePartyGet( g2m->g2c, id );
    f_pos = g2m->comm.trade_no_tbl[id];
    f_pp = PokeParty_GetMemberPointer( f_poke, f_pos );
    OS_TPrintf( "comm_id[%d]�̗� monsno= %d \n",id, PP_Get( f_pp, ID_PARA_monsno, NULL ) );
  }

}
#endif

