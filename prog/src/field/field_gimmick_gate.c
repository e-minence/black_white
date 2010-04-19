//////////////////////////////////////////////////////////////////////////////////////////// 
/** 
 *
 * @brief  �M�~�b�N�o�^�֐�(�Q�[�g) 
 * @file   field_gimmick_gate.c 
 * @author obata 
 * @date   2009.10.21 
 *
 * [�d���f���̒ǉ����@]
 * �@�z�u�������]�[���ŃM�~�b�N�����삷��悤��, fld_gimmick_assign.xls �ɓo�^
 * �A�o�^�����]�[���ł̃M�~�b�N����֐���, field_gimmick.c �ɓo�^
 * �B�z�u����]�[���̓d���f���f�[�^���쐬��, �d���f���f�[�^�o�^�e�[�u���ɒǉ�
 *
 */ 
//////////////////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h> 
#include "field_gimmick_gate.h" 

#include "field/gimmick_obj_elboard.h"
#include "field_gimmick_def.h"
#include "field/zonedata.h"
#include "field/enc_pokeset.h"  // for ENCPOKE_GetGenerateZone

#include "savedata/gimmickwork.h"
#include "savedata/misc.h" // for MISC_xxxx
#include "savedata/save_tbl.h"
#include "savedata/dendou_save.h"
#include "savedata/config.h"

#include "gmk_tmp_wk.h"
#include "gimmick_obj_elboard.h"
#include "elboard_zone_data.h"
#include "elboard_spnews_data.h" 
#include "field_task_manager.h"
#include "field_task.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_target_offset.h"
#include "move_pokemon.h"

#include "arc/arc_def.h" 
#include "arc/gate.naix"
#include "arc/message.naix" 
#include "msg/msg_gate.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"
#include "../../../resource/fldmapdata/flagwork/flag_define.h"


//==========================================================================================
// ���萔
//========================================================================================== 
// ��x�ɕ\������n����̐�
#define LOCAL_INFO_NUM (3)

// �g���I�u�W�F�N�g�E���j�b�g�ԍ�
#define EXPOBJ_UNIT_ELBOARD (1)

// �\���\�Ȉ��ʂɕ\���\�ȕ�����
#define DISPLAY_SIZE (8)

// �j���[�X�Ԃ̊Ԋu(������)
#define NEWS_INTERVAL (3)

// ��x�ɕ\������W�����̍ő吔
#define GYM_NEWS_MAX_NUM (4)

// �M�~�b�N���[�N�̃A�T�C��ID
#define GIMMICK_WORK_ASSIGN_ID (0)


//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
// ���\�[�X�C���f�b�N�X
typedef enum {
  RES_ELBOARD_NSBMD,        // �f���̃��f��
  RES_ELBOARD_NSBTX,        // �f���̃e�N�X�`��
  RES_ELBOARD_NSBTA_1,      // �j���[�X�E�X�N���[���E�A�j���[�V����1
  RES_ELBOARD_NSBTA_2,      // �j���[�X�E�X�N���[���E�A�j���[�V����2
  RES_ELBOARD_NSBTA_3,      // �j���[�X�E�X�N���[���E�A�j���[�V����3
  RES_ELBOARD_NSBTA_4,      // �j���[�X�E�X�N���[���E�A�j���[�V����4
  RES_ELBOARD_NSBTA_5,      // �j���[�X�E�X�N���[���E�A�j���[�V����5
  RES_ELBOARD_NSBTA_6,      // �j���[�X�E�X�N���[���E�A�j���[�V����6
  RES_ELBOARD_NSBTA_7,      // �j���[�X�E�X�N���[���E�A�j���[�V����7
  RES_ELBOARD_NSBTA_1_FAST, // �j���[�X�E�X�N���[���E�A�j���[�V����1 ( �� )
  RES_ELBOARD_NSBTA_2_FAST, // �j���[�X�E�X�N���[���E�A�j���[�V����2 ( �� )
  RES_ELBOARD_NSBTA_3_FAST, // �j���[�X�E�X�N���[���E�A�j���[�V����3 ( �� )
  RES_ELBOARD_NSBTA_4_FAST, // �j���[�X�E�X�N���[���E�A�j���[�V����4 ( �� )
  RES_ELBOARD_NSBTA_5_FAST, // �j���[�X�E�X�N���[���E�A�j���[�V����5 ( �� )
  RES_ELBOARD_NSBTA_6_FAST, // �j���[�X�E�X�N���[���E�A�j���[�V����6 ( �� )
  RES_ELBOARD_NSBTA_7_FAST, // �j���[�X�E�X�N���[���E�A�j���[�V����7 ( �� )
  RES_MONITOR_NSBTP_BROKEN, // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���<�̏�> )
  RES_MONITOR_NSBTP_C01,    // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C01 )
  RES_MONITOR_NSBTP_C02,    // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C02 )
  RES_MONITOR_NSBTP_C03,    // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C03 )
  RES_MONITOR_NSBTP_C04,    // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C04 )
  RES_MONITOR_NSBTP_C05,    // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C05 )
  RES_MONITOR_NSBTP_C08W,   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C08W )
  RES_MONITOR_NSBTP_C08B,   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C08B )
  RES_MONITOR_NSBTP_TG,     // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���TG )
  RES_MONITOR_NSBTP_ST,     // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���ST )
  RES_MONITOR_NSBTP_WF,     // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���WF )
  RES_MONITOR_NSBTP_BC,     // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���BC )
  RES_MONITOR_NSBTP_D03,    // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���D03 )
  RES_NUM                   // ����
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[ RES_NUM ] = 
{
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_nsbmd, GFL_G3D_UTIL_RESARC },        // �f���̃��f��
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_nsbtx, GFL_G3D_UTIL_RESARC },        // �f���̃e�N�X�`��
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_1_nsbta, GFL_G3D_UTIL_RESARC },      // �j���[�X�E�X�N���[���E�A�j���[�V����1
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_2_nsbta, GFL_G3D_UTIL_RESARC },      // �j���[�X�E�X�N���[���E�A�j���[�V����2
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_3_nsbta, GFL_G3D_UTIL_RESARC },      // �j���[�X�E�X�N���[���E�A�j���[�V����3
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_4_nsbta, GFL_G3D_UTIL_RESARC },      // �j���[�X�E�X�N���[���E�A�j���[�V����4
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_5_nsbta, GFL_G3D_UTIL_RESARC },      // �j���[�X�E�X�N���[���E�A�j���[�V����5
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_6_nsbta, GFL_G3D_UTIL_RESARC },      // �j���[�X�E�X�N���[���E�A�j���[�V����6
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_7_nsbta, GFL_G3D_UTIL_RESARC },      // �j���[�X�E�X�N���[���E�A�j���[�V����7
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_1_nsbta, GFL_G3D_UTIL_RESARC }, // �j���[�X�E�X�N���[���E�A�j���[�V����1 ( �� )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_2_nsbta, GFL_G3D_UTIL_RESARC }, // �j���[�X�E�X�N���[���E�A�j���[�V����2 ( �� )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_3_nsbta, GFL_G3D_UTIL_RESARC }, // �j���[�X�E�X�N���[���E�A�j���[�V����3 ( �� )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_4_nsbta, GFL_G3D_UTIL_RESARC }, // �j���[�X�E�X�N���[���E�A�j���[�V����4 ( �� )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_5_nsbta, GFL_G3D_UTIL_RESARC }, // �j���[�X�E�X�N���[���E�A�j���[�V����5 ( �� )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_6_nsbta, GFL_G3D_UTIL_RESARC }, // �j���[�X�E�X�N���[���E�A�j���[�V����6 ( �� )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_7_nsbta, GFL_G3D_UTIL_RESARC }, // �j���[�X�E�X�N���[���E�A�j���[�V����7 ( �� )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv00_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���<�̏�> )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv01_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C01 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv02_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C02 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv03_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C03 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv04_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C04 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv05_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C05 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv08_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C08W )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv09_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���C08B )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv10_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���TG )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv11_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���ST )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv12_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���WF )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv13_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���BC )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv14_nsbtp, GFL_G3D_UTIL_RESARC },   // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V���� ( ���j�^���D03 )
};

// �A�j���C���f�b�N�X
typedef enum {
  ANM_ELBOARD_DATE,         // �f���j���[�X�E�X�N���[���E���t
  ANM_ELBOARD_WEATHER,      // �f���j���[�X�E�X�N���[���E�V�C
  ANM_ELBOARD_PROPAGATION,  // �f���j���[�X�E�X�N���[���E��ʔ���
  ANM_ELBOARD_INFO_A,       // �f���j���[�X�E�X�N���[���E���A
  ANM_ELBOARD_INFO_B,       // �f���j���[�X�E�X�N���[���E���B
  ANM_ELBOARD_INFO_C,       // �f���j���[�X�E�X�N���[���E���C
  ANM_ELBOARD_CM,           // �f���j���[�X�E�X�N���[���E�ꌾCM
  ANM_MONITOR_C01,          // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C01 )
  ANM_MONITOR_C02,          // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C02 )
  ANM_MONITOR_C03,          // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C03 )
  ANM_MONITOR_C04,          // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C04 )
  ANM_MONITOR_C05,          // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C05 )
  ANM_MONITOR_C08W,         // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C08W )
  ANM_MONITOR_C08B,         // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C08B )
  ANM_MONITOR_TG,           // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���TG )
  ANM_MONITOR_ST,           // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���ST )
  ANM_MONITOR_WF,           // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���WF )
  ANM_MONITOR_BC,           // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���BC )
  ANM_MONITOR_D03,          // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���D03 )
  ANM_MONITOR_BROKEN,       // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���<�̏�> )
  ANM_NUM                   // ����
} ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table[ ANM_NUM ] = 
{
  // �A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ELBOARD_NSBTA_1, 0 },     // �f���j���[�X�E�X�N���[���E���t
  { RES_ELBOARD_NSBTA_2, 0 },     // �f���j���[�X�E�X�N���[���E�V�C
  { RES_ELBOARD_NSBTA_3, 0 },     // �f���j���[�X�E�X�N���[���E��ʔ���
  { RES_ELBOARD_NSBTA_4, 0 },     // �f���j���[�X�E�X�N���[���E���A
  { RES_ELBOARD_NSBTA_5, 0 },     // �f���j���[�X�E�X�N���[���E���B
  { RES_ELBOARD_NSBTA_6, 0 },     // �f���j���[�X�E�X�N���[���E���C
  { RES_ELBOARD_NSBTA_7, 0 },     // �f���j���[�X�E�X�N���[���E�ꌾCM
  { RES_MONITOR_NSBTP_C01, 0 },   // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C01 )
  { RES_MONITOR_NSBTP_C02, 0 },   // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C02 )
  { RES_MONITOR_NSBTP_C03, 0 },   // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C03 )
  { RES_MONITOR_NSBTP_C04, 0 },   // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C04 )
  { RES_MONITOR_NSBTP_C05, 0 },   // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C05 )
  { RES_MONITOR_NSBTP_C08W, 0 },  // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C08W )
  { RES_MONITOR_NSBTP_C08B, 0 },  // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C08B )
  { RES_MONITOR_NSBTP_TG, 0 },    // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���TG )
  { RES_MONITOR_NSBTP_ST, 0 },    // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���ST )
  { RES_MONITOR_NSBTP_WF, 0 },    // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���WF )
  { RES_MONITOR_NSBTP_BC, 0 },    // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���BC )
  { RES_MONITOR_NSBTP_D03, 0 },   // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���D03 )
  { RES_MONITOR_NSBTP_BROKEN, 0 },// ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���<�̏�> )
};
static const GFL_G3D_UTIL_ANM anm_table_fast[ ANM_NUM ] = 
{
  // �A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ELBOARD_NSBTA_1_FAST, 0 }, // �f���j���[�X�E�X�N���[���E���t ( �� )
  { RES_ELBOARD_NSBTA_2_FAST, 0 }, // �f���j���[�X�E�X�N���[���E�V�C ( �� )
  { RES_ELBOARD_NSBTA_3_FAST, 0 }, // �f���j���[�X�E�X�N���[���E��ʔ��� ( �� )
  { RES_ELBOARD_NSBTA_4_FAST, 0 }, // �f���j���[�X�E�X�N���[���E���A ( �� )
  { RES_ELBOARD_NSBTA_5_FAST, 0 }, // �f���j���[�X�E�X�N���[���E���B ( �� )
  { RES_ELBOARD_NSBTA_6_FAST, 0 }, // �f���j���[�X�E�X�N���[���E���C ( �� )
  { RES_ELBOARD_NSBTA_7_FAST, 0 }, // �f���j���[�X�E�X�N���[���E�ꌾCM ( �� )
  { RES_MONITOR_NSBTP_C01, 0 },    // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C01 )
  { RES_MONITOR_NSBTP_C02, 0 },    // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C02 )
  { RES_MONITOR_NSBTP_C03, 0 },    // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C03 )
  { RES_MONITOR_NSBTP_C04, 0 },    // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C04 )
  { RES_MONITOR_NSBTP_C05, 0 },    // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C05 )
  { RES_MONITOR_NSBTP_C08W, 0 },   // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C08W )
  { RES_MONITOR_NSBTP_C08B, 0 },   // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���C08B )
  { RES_MONITOR_NSBTP_TG, 0 },     // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���TG )
  { RES_MONITOR_NSBTP_ST, 0 },     // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���ST )
  { RES_MONITOR_NSBTP_WF, 0 },     // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���WF )
  { RES_MONITOR_NSBTP_BC, 0 },     // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���BC )
  { RES_MONITOR_NSBTP_D03, 0 },    // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���D03 )
  { RES_MONITOR_NSBTP_BROKEN, 0 },    // ���j�^�[�E�e�N�X�`���E�A�j���[�V���� ( ���j�^���<�̏�> )
};

// �I�u�W�F�N�g�C���f�b�N�X
typedef enum {
  OBJ_ELBOARD, // �d���f����
  OBJ_NUM      // ����
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[ OBJ_NUM ] = 
{
  // ���f�����\�[�XID, 
  // ���f���f�[�^ID(���\�[�X����INDEX), 
  // �e�N�X�`�����\�[�XID,
  // �A�j���e�[�u��, 
  // �A�j�����\�[�X��
  { RES_ELBOARD_NSBMD, 0, RES_ELBOARD_NSBTX, anm_table, ANM_NUM }, // �d���f����
}; 
static const GFL_G3D_UTIL_OBJ obj_table_fast[ OBJ_NUM ] = 
{
  // ���f�����\�[�XID, 
  // ���f���f�[�^ID(���\�[�X����INDEX), 
  // �e�N�X�`�����\�[�XID,
  // �A�j���e�[�u��, 
  // �A�j�����\�[�X��
  { RES_ELBOARD_NSBMD, 0, RES_ELBOARD_NSBTX, anm_table_fast, ANM_NUM }, // �d���f���� ( �� )
}; 

// ���j�b�g�C���f�b�N�X
typedef enum {
  UNIT_ELBOARD_NORMAL, // �d���f���� + ���j�^�[
  UNIT_ELBOARD_FAST,   // �d���f����(��) + ���j�^�[
  UNIT_NUM             // ����
} UNIT_INDEX;
static const GFL_G3D_UTIL_SETUP unit[ UNIT_NUM ] =
{
  { res_table, RES_NUM, obj_table, OBJ_NUM }, // �d���f���� + ���j�^�[
  { res_table, RES_NUM, obj_table_fast, OBJ_NUM }, // �d���f����(��) + ���j�^�[
};


//==========================================================================================
// ���j���[�X�p�����[�^
//==========================================================================================
// �j���[�X�^�C�v
typedef enum {
  NEWS_TYPE_NULL,         // ��
  NEWS_TYPE_DATE,         // ���t
  NEWS_TYPE_WEATHER,      // �V�C
  NEWS_TYPE_PROPAGATION,  // ��ʔ���
  NEWS_TYPE_INFO_A,       // �n����A
  NEWS_TYPE_INFO_B,       // �n����B
  NEWS_TYPE_INFO_C,       // �n����C
  NEWS_TYPE_CM,           // �ꌾCM 
  NEWS_TYPE_SPECIAL,      // �Վ��j���[�X
  NEWS_TYPE_CHAMPION,     // �`�����s�I�����
  NEWS_TYPE_NUM,
  NEWS_TYPE_MAX = NEWS_TYPE_NUM - 1
} NEWS_TYPE;

// �j���[�X�o�^�ԍ�
typedef enum {
  NEWS_INDEX_DATE,         // ���t
  NEWS_INDEX_WEATHER,      // �V�C
  NEWS_INDEX_PROPAGATION,  // ��ʔ���
  NEWS_INDEX_INFO_A,       // �n����A
  NEWS_INDEX_INFO_B,       // �n����B
  NEWS_INDEX_INFO_C,       // �n����C
  NEWS_INDEX_CM,           // �ꌾCM
  NEWS_INDEX_NUM,
  NEWS_INDEX_MAX = NEWS_INDEX_NUM -1
} NEWS_INDEX; 

// �X�N���[���E�A�j���[�V�����E�C���f�b�N�X
static u16 news_anm_index[NEWS_INDEX_NUM] = 
{
  ANM_ELBOARD_DATE,        // ���t
  ANM_ELBOARD_WEATHER,     // �V�C
  ANM_ELBOARD_PROPAGATION, // ��ʔ���
  ANM_ELBOARD_INFO_A,      // �n����A
  ANM_ELBOARD_INFO_B,      // �n����B
  ANM_ELBOARD_INFO_C,      // �n����C
  ANM_ELBOARD_CM,          // �ꌾCM
};
// �e�N�X�`����
static char* news_tex_name[NEWS_INDEX_NUM] =
{
  "gelboard_1",
  "gelboard_2",
  "gelboard_3",
  "gelboard_4",
  "gelboard_5",
  "gelboard_6",
  "gelboard_7",
};
// �p���b�g��
static char* news_plt_name[NEWS_INDEX_NUM] =
{
  "gelboard_1_pl",
  "gelboard_2_pl",
  "gelboard_3_pl",
  "gelboard_4_pl",
  "gelboard_5_pl",
  "gelboard_6_pl",
  "gelboard_7_pl",
};
// �V�C�Ɏg�p���郁�b�Z�[�W
u32 str_id_weather[ WEATHER_NO_NUM ] = 
{
  msg_gate_sunny,        // ����
  msg_gate_snow,         // ��
  msg_gate_rain,         // �J
  msg_gate_storm,        // ����
  msg_gate_snowstorm,    // ����
  msg_gate_arare,        // �����
  msg_gate_raikami,      // ���C�J�~
  msg_gate_kazakami,     // �J�U�J�~
  msg_gate_diamond_dust, // �_�C�A�����h�_�X�g
  msg_gate_mist,         // ��
  msg_gate_mist,         // �p���XWhite�p ��
  msg_gate_mist,         // �p���XBlack�p ��
};

// �W�����̓o�^�ꏊ
static const NEWS_INDEX gym_news_idx[ GYM_NEWS_MAX_NUM ] = 
{ 
  NEWS_INDEX_PROPAGATION, 
  NEWS_INDEX_INFO_A, 
  NEWS_INDEX_INFO_B, 
  NEWS_INDEX_INFO_C
};

// ���A�V�C�ꗗ
static const u8 RareWeatherNo[] = 
{
  WEATHER_NO_RAIKAMI,
  WEATHER_NO_KAZAKAMI,
  WEATHER_NO_DIAMONDDUST,
};

// ���A�V�C�̃`�F�b�N�Ώۃ]�[���ꗗ
static const u16 RareWeatherCheckZoneID[] = 
{
  ZONE_ID_C07,
};


//==========================================================================================
// �����j�^�[�p�����[�^
//==========================================================================================
// �A�j���[�V�����ԍ�
typedef enum {
  MONITOR_ANIME_C01,    // ( ���j�^���C01 )
  MONITOR_ANIME_C02,    // ( ���j�^���C02 )
  MONITOR_ANIME_C03,    // ( ���j�^���C03 )
  MONITOR_ANIME_C04,    // ( ���j�^���C04 )
  MONITOR_ANIME_C05,    // ( ���j�^���C05 )
  MONITOR_ANIME_C08W,   // ( ���j�^���C08W )
  MONITOR_ANIME_C08B,   // ( ���j�^���C08B )
  MONITOR_ANIME_TG,     // ( ���j�^���TG )
  MONITOR_ANIME_ST,     // ( ���j�^���ST )
  MONITOR_ANIME_WF,     // ( ���j�^���WF )
  MONITOR_ANIME_BC,     // ( ���j�^���BC )
  MONITOR_ANIME_D03,    // ( ���j�^���D03 )
  MONITOR_ANIME_BROKEN, // ( ���j�^���<�̏�> )
  MONITOR_ANIME_NUM,    // ����
  MONITOR_ANIME_MAX = MONITOR_ANIME_NUM - 1
} MONITOR_ANIME_INDEX;

// ���A�j���[�V�����ԍ�
static u16 monitor_anime[ MONITOR_ANIME_NUM ] = 
{ 
  ANM_MONITOR_C01,    // ( ���j�^���C01 )
  ANM_MONITOR_C02,    // ( ���j�^���C02 )
  ANM_MONITOR_C03,    // ( ���j�^���C03 )
  ANM_MONITOR_C04,    // ( ���j�^���C04 )
  ANM_MONITOR_C05,    // ( ���j�^���C05 )
  ANM_MONITOR_C08W,   // ( ���j�^���C08W )
  ANM_MONITOR_C08B,   // ( ���j�^���C08B )
  ANM_MONITOR_TG,     // ( ���j�^���TG )
  ANM_MONITOR_ST,     // ( ���j�^���ST )
  ANM_MONITOR_WF,     // ( ���j�^���WF )
  ANM_MONITOR_BC,     // ( ���j�^���BC )
  ANM_MONITOR_D03,    // ( ���j�^���D03 )
  ANM_MONITOR_BROKEN, // ( ���j�^���<�̏�> )
};


//==========================================================================================
// ���V�C�j���[�X�̐����p�����[�^
//==========================================================================================
typedef struct 
{
  u16 zoneID[ WEATHER_ZONE_NUM ]; // �]�[��ID
  u8  weatherNo[ WEATHER_ZONE_NUM ]; // �]�[���̓V�C

} WEATHER_NEWS_PARAM; 

//==========================================================================================
// ���j���[�X�o�^��
//==========================================================================================
typedef struct
{
  u8        newsNum;                      // �o�^�����j���[�X�̐�
  NEWS_TYPE newsType[ NEWS_INDEX_NUM ];   // �o�^�����j���[�X
  u32       spNewsFlag[ NEWS_INDEX_NUM ]; // �o�^�����Վ��j���[�X�ɑΉ�����t���O

} NEWS_ENTRY_DATA;

//==========================================================================================
// ���M�~�b�N �Z�[�u���[�N
//==========================================================================================
typedef struct
{
  BOOL            firstSetupFlag;   // ����Z�b�g�A�b�v���������Ă��邩�ǂ���
  u32             recoveryFrame;    // ���A�t���[��
  u16             champNewsMinutes; // �Q�[�g�ɓ��������̃`�����v�j���[�X�c�莞��
  NEWS_ENTRY_DATA newsEntryData;    // �j���[�X�o�^��

} SAVEWORK; 

//==========================================================================================
// ���M�~�b�N�Ǘ����[�N
//==========================================================================================
typedef struct
{ 
  HEAPID         heapID;     // �g�p����q�[�vID
  FIELDMAP_WORK* fieldmap;   // �t�B�[���h�}�b�v
  GAMEDATA*      gameData;   // �Q�[���f�[�^
  GAMESYS_WORK*  gameSystem; // �Q�[���V�X�e��

  BOOL firstSetupFlag; // �P��ڂ̃Z�b�g�A�b�v���������Ă��邩�ǂ���

  GOBJ_ELBOARD*        elboard;        // �d���f���Ǘ��I�u�W�F�N�g
  u32                  recoveryFrame;  // ���A�t���[��
  ELBOARD_ZONE_DATA*   gateData;       // �Q�[�g�f�[�^
  ELBOARD_SPNEWS_DATA* spNewsData;     // �Վ��j���[�X�f�[�^
  u8                   spNewsDataNum;  // �Վ��j���[�X�f�[�^��
  NEWS_ENTRY_DATA      newsEntryData;  // �j���[�X�o�^��
  u16                  champNewsMinutes; // �Q�[�g�ɓ��������̃`�����v�j���[�X�c�莞��

} GATEWORK;


//==========================================================================================
// ���֐��C���f�b�N�X
//==========================================================================================
// �Z�[�u���[�N�A�N�Z�X
SAVEWORK* GetGimmickSaveWork( FIELDMAP_WORK* fieldmap ); // �M�~�b�N�̃Z�[�u���[�N���擾����
// �M�~�b�N�̕ۑ��ƕ��A
static void SaveGimmick( const GATEWORK* work ); // �M�~�b�N��Ԃ��Z�[�u����
static void LoadGimmick( GATEWORK* work ); // �M�~�b�N��Ԃ����[�h����
static void RecoverChampNewsTimer( GATEWORK* work ); // �`�����s�I���j���[�X�̎c��\�����Ԃ𕜋A����
static void RecoverSpNewsFlags( GATEWORK* work ); // �Վ��j���[�X�̃t���O��Ԃ𕜋A����
static void RecoverElboardStatus( GATEWORK* work ); // �d���f���̏�Ԃ𕜋A����
// �M�~�b�N�Ǘ����[�N�̐����E�j��
static GATEWORK* CreateGateWork( FIELDMAP_WORK* fieldmap ); // �M�~�b�N�Ǘ����[�N�𐶐�����
static void DeleteGateWork( GATEWORK* work ); // �M�~�b�N�Ǘ����[�N��j������
static void FirstSetupGateWork( GATEWORK* work ); // �M�~�b�N�Ǘ����[�N�̏���Z�b�g�A�b�v���s��
// �Q�[�g�f�[�^
static void LoadGateData( GATEWORK* work ); // �Q�[�g�f�[�^��ǂݍ���
static void DeleteGateData( GATEWORK* work ); // �Q�[�g�f�[�^��j������
// ���j�^�[
static void StartMonitorAnime( GATEWORK* work ); // ���j�^�[�̃A�j���[�V�������J�n����
// �d���f����
static void SetElboardPos( GATEWORK* work ); // �d���f����z�u����
// �V�C�j���[�X
static int GetValidWeatherNewsNum( WEATHER_NEWS_PARAM* param ); // �L���ȓV�C���̐����擾����
static void InitWeatherNewsParam( WEATHER_NEWS_PARAM* param ); // �V�C�j���[�X�p�����[�^������������
static void GetWeatherNewsParam( const GATEWORK* work, WEATHER_NEWS_PARAM* dest ); // �\�����ׂ��V�C�j���[�X���擾����
static void GetRareWeather( const GATEWORK* work, WEATHER_NEWS_PARAM* dest ); // �������V�C�j���[�X���擾����
static void GetMovePokeWeather( const GATEWORK* work, WEATHER_NEWS_PARAM* dest ); // �ړ��|�P�����Ɋւ���V�C�j���[�X���擾����
// �`�����s�I���j���[�X
static BOOL CheckChampionNews( const GATEWORK* work ); // �`�����s�I���j���[�X�����邩�ǂ����𔻒肷��
static BOOL CheckChampionDataExist( const GATEWORK* work ); // �`�����s�I���f�[�^�����݂��邩�ǂ����𔻒肷��
static BOOL CheckFirstClearDataExist( const GATEWORK* work ); //�u�͂��߂ẴN���A�v�f�[�^�����݂��邩�ǂ����𔻒肷��
static BOOL CheckDendouDataExist( const GATEWORK* work ); //�u�a������v�f�[�^�����݂��邩�ǂ����𔻒肷��
static void GetChampMonsNo_byDendouData( const GATEWORK* work, int* destMonsNo, int* destMonsNum ); //�u�a������v�f�[�^�̃����X�^�[�ԍ����擾����
static void GetChampMonsNo_byFirstClear( const GATEWORK* work, int* destMonsNo, int* destMonsNum ); //�u�͂��߂ăN���A�v�f�[�^�̃����X�^�[�ԍ����擾����
// �Վ��j���[�X
static void LoadSpNewsData( GATEWORK* work ); // �Վ��j���[�X�f�[�^��ǂݍ���
static void DeleteSpNewsData( GATEWORK* work ); // �Վ��j���[�X�f�[�^��j������
static const ELBOARD_SPNEWS_DATA* SearchTopNews( const GATEWORK* work ); // �ł��D�揇�ʂ̍����Վ��j���[�X����������
static const ELBOARD_SPNEWS_DATA* SearchGymNews( const GATEWORK* work ); // �ł��D�揇�ʂ̍����W���j���[�X����������
static const ELBOARD_SPNEWS_DATA* SearchFlagNews( const GATEWORK* work, u32 flag ); // �w�肵���t���O�ɑΉ�����Վ��j���[�X����������
static BOOL CheckSpecialNews( const GATEWORK* work ); // �Վ��j���[�X�����邩�ǂ������`�F�b�N����
// �j���[�X�o�^
static void SetupElboardNews( GATEWORK* work ); // �f���ŕ\������j���[�X���Z�b�g�A�b�v����
static void SetupElboardNews_Normal( GATEWORK* work ); // �f���ŕ\������j���[�X�𕽏�j���[�X�ō\������
static void SetupElboardNews_Champion( GATEWORK* work ); // �f���ŕ\������j���[�X���`�����s�I���j���[�X�ō\������
static void SetupElboardNews_Special( GATEWORK* work ); // �f���ŕ\������j���[�X��Վ��j���[�X�ō\������
static void EntryNews( GATEWORK* work, const NEWS_PARAM* news, NEWS_TYPE type, const ELBOARD_SPNEWS_DATA* sp_data ); // �j���[�X��o�^����
static void AddNewsEntryData( GATEWORK* work, NEWS_TYPE newsType, u32 spNewsFlag ); // �o�^�󋵂��X�V����
// ����j���[�X�ǉ�
static void AddNews_DATE( GATEWORK* work ); // ���t����ǉ�����
static void AddNews_WEATHER( GATEWORK* work ); // �V�C����ǉ�����
static void AddNews_PROPAGATION( GATEWORK* work ); // ��ʔ�������ǉ�����
static void AddNews_INFO_A( GATEWORK* work ); // �n����A��ǉ�����
static void AddNews_INFO_B( GATEWORK* work ); // �n����B��ǉ�����
static void AddNews_INFO_C( GATEWORK* work ); // �n����C��ǉ�����
static void AddNews_CM( GATEWORK* work ); // �ꌾCM��ǉ�����
// �`�����s�I���j���[�X�ǉ�
static void AddNews_CHAMPION( GATEWORK* work ); // �`�����s�I������ǉ�����
// �Վ��j���[�X�ǉ�
static void AddNews_SPECIAL( GATEWORK* work ); // �Վ�����ǉ�����
static void AddSpNews_DIRECT( GATEWORK* work, const ELBOARD_SPNEWS_DATA* spnews, NEWS_INDEX news_idx ); // �Վ���� ( ���b�Z�[�W���̂܂� ) ��ǉ�����
static void AddSpNews_GYM( GATEWORK* work ); // �Վ���� ( �W����� ) ��ǉ�����
// �f�o�b�O
static void DebugPrint_SAVEWORK( const SAVEWORK* save ); // �Z�[�u���[�N���o�͂���


//==========================================================================================
// ���M�~�b�N�o�^�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  GATEWORK* work = NULL;  // GATE�M�~�b�N�Ǘ����[�N
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  {
    GAMESYS_WORK* gameSystem;
    GAMEDATA* gameData;
    SAVE_CONTROL_WORK* save;
    MOJIMODE mojiMode;
    UNIT_INDEX index;
    CONFIG* config;
    
    gameSystem = FIELDMAP_GetGameSysWork( fieldmap );
    gameData = GAMESYSTEM_GetGameData( gameSystem );
    save = GAMEDATA_GetSaveControlWork( gameData );
    config = SaveData_GetConfig( save );
    mojiMode = CONFIG_GetMojiMode( config );
    switch( mojiMode ) {
    case MOJIMODE_KANJI:    index = UNIT_ELBOARD_NORMAL; break;
    case MOJIMODE_HIRAGANA: index = UNIT_ELBOARD_FAST;   break;
    default: GF_ASSERT(0);
    }
    FLD_EXP_OBJ_AddUnit( exobj_cnt, &unit[ index ], EXPOBJ_UNIT_ELBOARD );
  }

  // �M�~�b�N�Ǘ����[�N���쐬
  work = CreateGateWork( fieldmap );

  // �e��f�[�^�ǂݍ���
  LoadGateData( work );   // �Q�[�g
  LoadSpNewsData( work ); // �Վ��j���[�X
  LoadGimmick( work );    // �M�~�b�N�̃Z�[�u�f�[�^

  // ����Z�b�g�A�b�v
  if( work->firstSetupFlag == FALSE ) { 
    FirstSetupGateWork( work ); 
  }
  else {
    RecoverChampNewsTimer( work ); // �`�����s�I���j���[�X�̎c�莞�ԕ��A
    RecoverSpNewsFlags( work );    // �t���O���A
  }

  // �j���[�X���Z�b�g�A�b�v
  SetupElboardNews( work );

  // �f���E���j�^�[���Z�b�g�A�b�v
  RecoverElboardStatus( work );  // �f���̏�ԕ��A
  SetElboardPos( work ); // �d���f����z�u
  StartMonitorAnime( work ); // ���j�^�[�E�A�j���[�V�����J�n

  // DEBUG:
  OBATA_Printf( "GIMMICK-GATE: setup\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  GATEWORK* work = (GATEWORK*)GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );

  // �M�~�b�N��Ԃ��Z�[�u
  SaveGimmick( work );
  
  // �d���f���Ǘ����[�N��j��
  if( work->elboard ){ GOBJ_ELBOARD_Delete( work->elboard ); }

  // �M�~�b�N�Ǘ����[�N��j��
  DeleteGateWork( work );

  // DEBUG:
  OBATA_Printf( "GIMMICK-GATE: end\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief ����֐�
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  GATEWORK* work = (GATEWORK*)GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );
  static fx32 animeSpeed = FX32_ONE;

#if 0
  // TEST: �������葬�x�̒���
  {
    int key = GFL_UI_KEY_GetCont();
    int trg = GFL_UI_KEY_GetTrg();
    //if( key & PAD_BUTTON_DEBUG )
    {
      if( trg & PAD_BUTTON_R )
      {
        animeSpeed += FX_F32_TO_FX32( 0.1f );
        OS_Printf( "animeSpeed = %f\n", FX_FX32_TO_F32(animeSpeed) );
      }
      if( trg & PAD_BUTTON_L )
      {
        animeSpeed -= FX_F32_TO_FX32( 0.1f );
        OS_Printf( "animeSpeed = %f\n", FX_FX32_TO_F32(animeSpeed) );
      }
      if( trg & PAD_BUTTON_Y )
      {
        animeSpeed -= FX_F32_TO_FX32( 0.01f );
        OS_Printf( "animeSpeed = %f\n", FX_FX32_TO_F32(animeSpeed) );
      }
      if( trg & PAD_BUTTON_B )
      {
        animeSpeed += FX_F32_TO_FX32( 0.01f );
        OS_Printf( "animeSpeed = %f\n", FX_FX32_TO_F32(animeSpeed) );
      }
      if( trg & PAD_BUTTON_START )
      {
        animeSpeed = FX_F32_TO_FX32( 1.0f );
        OS_Printf( "animeSpeed = %f\n", FX_FX32_TO_F32(animeSpeed) );
      }
    }
  }
#endif

  // �d���f�����C������
  GOBJ_ELBOARD_Main( work->elboard, animeSpeed );

  // ���j�^�[�A�j���[�V�����Đ�
  {
    FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
    FLD_EXP_OBJ_PlayAnime( exobj_cnt );
  }
}


//------------------------------------------------------------------------------------------
/**
 * @brief �d���f���̌������擾����
 *
 * @param fieldmap
 *
 * @return �d���f���̌���( DIR_xxxx )
 */
//------------------------------------------------------------------------------------------
u8 GATE_GIMMICK_GetElboardDir( FIELDMAP_WORK* fieldmap )
{ 
  GATEWORK* work = (GATEWORK*)GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );

  return work->gateData->dir;
}


//==========================================================================================
// �� ����J�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�̃Z�[�u���[�N���擾����
 *
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �M�~�b�N�̃��[�J���Z�[�u�f�[�^
 */
//------------------------------------------------------------------------------------------
SAVEWORK* GetGimmickSaveWork( FIELDMAP_WORK* fieldmap )
{
  GAMESYS_WORK*   gsys;
  GAMEDATA*      gdata;
  GIMMICKWORK* gmkwork;
  int           gmk_id;
  SAVEWORK*  save_work;

  GF_ASSERT( fieldmap );

  // �M�~�b�N�̃Z�[�u���[�N���擾
  gsys      = FIELDMAP_GetGameSysWork( fieldmap );
  gdata     = GAMESYSTEM_GetGameData( gsys );
  gmkwork   = GAMEDATA_GetGimmickWork( gdata );
  gmk_id    = GIMMICKWORK_GetAssignID( gmkwork );
  save_work = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id ); 
  return save_work;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N��Ԃ�ۑ�����
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void SaveGimmick( const GATEWORK* work )
{
  SAVEWORK* saveWork;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->elboard );

  // �M�~�b�N��Ԃ�ۑ�
  saveWork = GetGimmickSaveWork( work->fieldmap );
  saveWork->firstSetupFlag   = work->firstSetupFlag;
  saveWork->recoveryFrame    = GOBJ_ELBOARD_GetFrame( work->elboard ) >> FX32_SHIFT; 
  saveWork->champNewsMinutes = work->champNewsMinutes;
  saveWork->newsEntryData    = work->newsEntryData;

  // DEBUG: �Z�[�u�o�b�t�@�o��
  DebugPrint_SAVEWORK( saveWork );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�̃Z�[�u�f�[�^��ǂݍ���
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void LoadGimmick( GATEWORK* work )
{
  SAVEWORK* saveWork;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );

  // �Z�[�u�f�[�^�擾
  saveWork = GetGimmickSaveWork( work->fieldmap );
  work->firstSetupFlag   = saveWork->firstSetupFlag;
  work->recoveryFrame    = saveWork->recoveryFrame;  // �f���̕��A�|�C���g
  work->champNewsMinutes = saveWork->champNewsMinutes;

  // DEBUG: �Z�[�u�o�b�t�@�o��
  DebugPrint_SAVEWORK( saveWork );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �`�����s�I���j���[�X�̎c��\�����Ԃ𕜋A����
 *
 * @param work
 */
//------------------------------------------------------------------------------------------
static void RecoverChampNewsTimer( GATEWORK* work )
{
  SAVE_CONTROL_WORK* save;
  MISC* misc;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gameData );

  save = GAMEDATA_GetSaveControlWork( work->gameData );
  misc = SaveData_GetMisc( save ); 
  MISC_SetChampNewsMinutes( misc, work->champNewsMinutes );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �\�����Ă����Վ��j���[�X�ɑΉ�����t���O�𕜋A����
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void RecoverSpNewsFlags( GATEWORK* work )
{
  int i;
  EVENTWORK* evwork;
  SAVEWORK* saveWork;
  NEWS_ENTRY_DATA* entry_data;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gameData );
  GF_ASSERT( work->gameSystem );

  evwork     = GAMEDATA_GetEventWork( work->gameData );
  saveWork   = GetGimmickSaveWork( work->fieldmap );
  entry_data = &saveWork->newsEntryData;

  // �t���O����
  for( i=0; i<entry_data->newsNum; i++)
  {
    if( entry_data->newsType[i] == NEWS_TYPE_SPECIAL ) {
      EVENTWORK_SetEventFlag( evwork, entry_data->spNewsFlag[i] );
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̏�Ԃ𕜋A����
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void RecoverElboardStatus( GATEWORK* work )
{
  GF_ASSERT( work );
  GF_ASSERT( work->elboard );

  // �f���̃A�j���t���[�����A
  GOBJ_ELBOARD_SetFrame( work->elboard, work->recoveryFrame << FX32_SHIFT );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�Ǘ����[�N���쐬����
 *
 * @param fieldmap �ˑ�����t�B�[���h�}�b�v
 *
 * @return �쐬�����M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static GATEWORK* CreateGateWork( FIELDMAP_WORK* fieldmap )
{
  GATEWORK* work;
  HEAPID heapID;
  FLD_EXP_OBJ_CNT_PTR exObjCnt;

  GF_ASSERT( fieldmap );

  heapID   = FIELDMAP_GetHeapID( fieldmap );
  exObjCnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // �M�~�b�N�Ǘ����[�N�𐶐�
  work = (GATEWORK*)GMK_TMP_WK_AllocWork( fieldmap, 
                                          GIMMICK_WORK_ASSIGN_ID, 
                                          heapID, sizeof(GATEWORK) );
  // ���[�N��������
  GFL_STD_MemClear( work, sizeof(GATEWORK) );
  work->heapID         = heapID;
  work->fieldmap       = fieldmap;
  work->gameSystem     = FIELDMAP_GetGameSysWork( fieldmap );
  work->gameData       = GAMESYSTEM_GetGameData( work->gameSystem );
  work->firstSetupFlag = FALSE;

  // �d���f���Ǘ����[�N���쐬
  {
    ELBOARD_PARAM param;
    param.heapID       = heapID;
    param.maxNewsNum   = NEWS_INDEX_NUM;
    param.dispSize     = DISPLAY_SIZE;
    param.newsInterval = NEWS_INTERVAL;
    param.g3dObj       = FLD_EXP_OBJ_GetUnitObj( exObjCnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD );
    work->elboard      = GOBJ_ELBOARD_Create( &param );
  } 
  return work;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�Ǘ����[�N��j������
 *
 * @param work �j�����郏�[�N
 */
//------------------------------------------------------------------------------------------
static void DeleteGateWork( GATEWORK* work )
{
  if( work ) {
    DeleteGateData( work );      // �Q�[�g�f�[�^
    DeleteSpNewsData( work );    // �Վ��j���[�X�f�[�^
    GMK_TMP_WK_FreeWork( work->fieldmap, GIMMICK_WORK_ASSIGN_ID ); // �{��
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�Ǘ����[�N�̏���Z�b�g�A�b�v���s��
 *
 * @param work
 */
//------------------------------------------------------------------------------------------
static void FirstSetupGateWork( GATEWORK* work )
{
  const SAVE_CONTROL_WORK* save;
  const MISC* misc;

  GF_ASSERT( work );
  GF_ASSERT( work->gameData );
  GF_ASSERT( work->firstSetupFlag == FALSE );

  if( work->firstSetupFlag == FALSE ) {
    // �`�����s�I���j���[�X�̎c�莞�Ԃ��擾
    save = GAMEDATA_GetSaveControlWorkConst( work->gameData );
    misc = SaveData_GetMiscConst( save ); 
    work->champNewsMinutes = MISC_GetChampNewsMinutes( misc );

    // ����Z�b�g�A�b�v����
    work->firstSetupFlag = TRUE;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �]�[���ɉ������d���f���f�[�^���擾����
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void LoadGateData( GATEWORK* work )
{
  int datID;
  u16 zoneID;
  u32 dataNum;
  ELBOARD_ZONE_DATA data;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );

  // ���łɓǂݍ��܂�Ă���
  if( work->gateData ){ return; }

  // �]�[��ID���擾
  zoneID = FIELDMAP_GetZoneID( work->fieldmap );

  // ���[�N���m��
  work->gateData = GFL_HEAP_AllocMemory( work->heapID, sizeof(ELBOARD_ZONE_DATA) );
  dataNum        = GFL_ARC_GetDataFileCnt( ARCID_ELBOARD_ZONE );

  // �d���f���f�[�^�z�񂩂�, �Y������f�[�^������
  for( datID=0; datID < dataNum; datID++ )
  {
    // �ǂݍ���
    ELBOARD_ZONE_DATA_Load( &data, ARCID_ELBOARD_ZONE, datID ); 

    // ����( �]�[����v and �o�[�W������v )
    if( data.zoneID == zoneID ) {
      if( (data.version == 0) || (data.version == GetVersion()) ) {
        *(work->gateData) = data;
        return;
      }
    }
  } 

  // ���݂̃]�[���ɑΉ�����d���f���f�[�^���o�^����Ă��Ȃ��ꍇ
  OS_Printf( "error: �d���f���f�[�^���o�^����Ă��܂���B\n" );
  GF_ASSERT(0);
}

//------------------------------------------------------------------------------------------
/**
 * @brief �Q�[�g�f�[�^��j������
 *
 * @param work �j������Q�[�g�f�[�^��ێ����郏�[�N
 */
//------------------------------------------------------------------------------------------
static void DeleteGateData( GATEWORK* work )
{
  if( work->gateData )
  {
    GFL_HEAP_FreeMemory( work->gateData );
    work->gateData = NULL;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���j�^�[�̃A�j���[�V�������J�n����
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void StartMonitorAnime( GATEWORK* work )
{ 
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;
  
  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gateData );

  exobj_cnt = FIELDMAP_GetExpObjCntPtr( work->fieldmap );

  // ���j�^�[�E�A�j���[�V�����J�n
  {
    int anime_idx;
    anime_idx = monitor_anime[ work->gateData->monitorAnimeIndex ];
    FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD, anime_idx, TRUE ); 
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �d���f����z�u����
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void SetElboardPos( GATEWORK* work )
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;
  GFL_G3D_OBJSTATUS* status;
  ELBOARD_ZONE_DATA* gate_data;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gateData );

  exobj_cnt = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
  status    = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD );
  gate_data = work->gateData;

  // ���W��ݒ�
  status->trans.x = gate_data->x << FX32_SHIFT;
  status->trans.y = gate_data->y << FX32_SHIFT;
  status->trans.z = gate_data->z << FX32_SHIFT;

  // ������ݒ�
  {
    u16 rot;
    switch( gate_data->dir ) {
    case DIR_DOWN:  rot = 0;    break;
    case DIR_RIGHT: rot = 90;   break;
    case DIR_UP:    rot = 180;  break;
    case DIR_LEFT:  rot = 270;  break;
    default:        rot = 0;    break;
    }
    rot *= 182;  // 65536��360 = 182.044...
    GFL_CALC3D_MTX_CreateRot( 0, rot, 0, &status->rotate );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �o�^�󋵂��X�V����
 *
 * @param work
 * @param newsType   �ǉ�����j���[�X�̃^�C�v
 * @param spNewsFlag �Վ��j���[�X�̏ꍇ, �Ή�����t���O���w��
 */
//------------------------------------------------------------------------------------------
static void AddNewsEntryData( GATEWORK* work, NEWS_TYPE newsType, u32 spNewsFlag )
{
  NEWS_ENTRY_DATA* entryData = &(work->newsEntryData);
  int              entryNum  = entryData->newsNum;

  // ���łɍő吔���o�^����Ă���
  if( NEWS_INDEX_NUM <= entryNum ){ return; }

  // �o�^
  entryData->newsType[ entryNum ]   = newsType;
  entryData->spNewsFlag[ entryNum ] = spNewsFlag;
  entryData->newsNum                = entryNum + 1;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �`�����s�I���j���[�X�̗L���𒲂ׂ�
 *
 * @param work
 * 
 * @return �`�����s�I����񂪂���ꍇ TRUE, �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------------------
static BOOL CheckChampionNews( const GATEWORK* work )
{ 
  //�u�͂��߂ăN���A�vor�u�a������v�f�[�^�����݂���
  if( (CheckFirstClearDataExist( work ) == TRUE) ||
      (CheckDendouDataExist( work ) == TRUE) )
  {
    // �\���̎c�莞�Ԃ�����
    if( 0 < work->champNewsMinutes ) { 
      return TRUE; 
    }
  }

  return FALSE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �u�͂��߂ăN���A�v�f�[�^�����݂��邩�ǂ����𔻒肷��
 *
 * @param work
 *
 * @return �u�͂��߂ăN���A�v�̃f�[�^�����݂���ꍇ TRUE
 *          �����łȂ��ꍇ FALSE
 */
//------------------------------------------------------------------------------------------ 
static BOOL CheckFirstClearDataExist( const GATEWORK* work )
{
  EVENTWORK* evwork;

  evwork  = GAMEDATA_GetEventWork( work->gameData );

  // �V�i���I�N���A�t���O�����Ă���
  if( EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_GAME_CLEAR ) == TRUE ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �u�a������v�f�[�^�����݂��邩�ǂ����𔻒肷��
 *
 * @param work
 *
 * @return �u�a������v�̃f�[�^�����݂���ꍇ TRUE
 *          �����łȂ��ꍇ FALSE
 */
//------------------------------------------------------------------------------------------ 
static BOOL CheckDendouDataExist( const GATEWORK* work )
{
  EVENTWORK* evwork;

  evwork  = GAMEDATA_GetEventWork( work->gameData );

  // �Q�[���N���A�t���O�������Ă���
  if( EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_CHAMPION_WIN ) == TRUE ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �u�a������v�f�[�^�̃����X�^�[�ԍ����擾����
 *
 * @param work
 * @param destMonsNo  �����X�^�[�ԍ����i�[����z��
 * @param destMonsNum �擾���������X�^�[�ԍ��̐��̊i�[��
 */
//------------------------------------------------------------------------------------------
static void GetChampMonsNo_byDendouData( const GATEWORK* work, int* destMonsNo, int* destMonsNum )
{
  SAVE_CONTROL_WORK* save;
  LOAD_RESULT result;
  DENDOU_SAVEDATA* dendouData;
  int i;

  // �f�[�^�����݂��Ȃ�
  GF_ASSERT( CheckDendouDataExist( work ) == TRUE );

  *destMonsNum = 0;

  // �O���f�[�^�̃��[�h
  save = GAMEDATA_GetSaveControlWork( work->gameData );
  result = SaveControl_Extra_Load( save, SAVE_EXTRA_ID_DENDOU, work->heapID );

  // �ǂݍ��݂ɐ���
  if( (result == LOAD_RESULT_OK) || (result == LOAD_RESULT_NG) )
  {
    // �a���f�[�^���擾
    dendouData = 
      SaveControl_Extra_DataPtrGet( save, SAVE_EXTRA_ID_DENDOU, EXGMDATA_ID_DENDOU );

    // ���R�[�h������
    if( 0 < DendouData_GetRecordCount(dendouData) ) {

      // �����X�^�[�����擾
      *destMonsNum = DendouData_GetPokemonCount( dendouData, 0 );

      // �����X�^�[�ԍ����擾
      for( i=0; i<*destMonsNum; i++ )
      {
        DENDOU_POKEMON_DATA pokeData;
        pokeData.nickname = GFL_STR_CreateBuffer( 64, work->heapID );
        pokeData.oyaname = GFL_STR_CreateBuffer( 64, work->heapID );
        DendouData_GetPokemonData( dendouData, 0, i, &pokeData );
        destMonsNo[i] = pokeData.monsno;
        GFL_STR_DeleteBuffer( pokeData.nickname );
        GFL_STR_DeleteBuffer( pokeData.oyaname );
      }
    } 
    // ���R�[�h���Ȃ�
    else {
      GF_ASSERT(0);
    }
  }
  // �ǂݍ��݂Ɏ��s
  else {
    GF_ASSERT(0);
  }

  // �Z�[�u�f�[�^���
  SaveControl_Extra_Unload( save, SAVE_EXTRA_ID_DENDOU );
}

//------------------------------------------------------------------------------------------
/**
 * @brief�u�͂��߂ăN���A�v�f�[�^�̃����X�^�[�ԍ����擾����
 *
 * @param work
 * @param destMonsNo  �����X�^�[�ԍ����i�[����z��
 * @param destMonsNum �擾���������X�^�[�ԍ��̐��̊i�[��
 */
//------------------------------------------------------------------------------------------
static void GetChampMonsNo_byFirstClear( const GATEWORK* work, int* destMonsNo, int* destMonsNum )
{
  SAVE_CONTROL_WORK* save;
  LOAD_RESULT result;
  DENDOU_RECORD* record;
  int i;

  // �f�[�^�����݂��Ȃ�
  GF_ASSERT( CheckFirstClearDataExist(work) == TRUE );

  record = GAMEDATA_GetDendouRecord( work->gameData );
  *destMonsNum = DendouRecord_GetPokemonCount( record );

  for( i=0; i<*destMonsNum; i++ )
  {
    DENDOU_POKEMON_DATA pokeData;
    pokeData.nickname = GFL_STR_CreateBuffer( 64, work->heapID );
    pokeData.oyaname = GFL_STR_CreateBuffer( 64, work->heapID );
    DendouRecord_GetPokemonData( record, i, &pokeData );
    destMonsNo[i] = pokeData.monsno;
    GFL_STR_DeleteBuffer( pokeData.nickname );
    GFL_STR_DeleteBuffer( pokeData.oyaname );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �Վ��j���[�X�f�[�^��ǂݍ���
 *
 * @param work �ǂݍ��񂾃f�[�^��ێ����郏�[�N
 */
//------------------------------------------------------------------------------------------
static void LoadSpNewsData( GATEWORK* work )
{
  GF_ASSERT( work );

  // ���łɓǂݍ��܂�Ă���
  if( work->spNewsData ){ return; }

  // �Վ��j���[�X�f�[�^��ǂݍ���
  {
    int data_idx;
    int data_num;

    // �f�[�^���擾
    data_num = GFL_ARC_GetDataFileCnt( ARCID_ELBOARD_SPNEWS );

    // �e�f�[�^���擾
    work->spNewsData = GFL_HEAP_AllocMemory( work->heapID, 
                                             sizeof(ELBOARD_SPNEWS_DATA) * data_num );

    for( data_idx=0; data_idx < data_num; data_idx++ )
    {
      ELBOARD_SPNEWS_DATA_Load( &work->spNewsData[data_idx], ARCID_ELBOARD_SPNEWS, data_idx );
    }
    work->spNewsDataNum = data_num;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �Վ��j���[�X�f�[�^��j������
 *
 * @param �j������f�[�^��ێ����郏�[�N
 */
//------------------------------------------------------------------------------------------
static void DeleteSpNewsData( GATEWORK* work )
{
  GF_ASSERT( work );

  if( work->spNewsData ) {
    GFL_HEAP_FreeMemory( work->spNewsData );
    work->spNewsData = NULL;
    work->spNewsDataNum = 0;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �\�����ׂ��j���[�X�̒���, �ł��D�揇�ʂ̍����Վ��j���[�X����������
 *
 * @param work �M�~�b�N�Ǘ����[�N
 *
 * @return �ł��D�揇�ʂ̍���, �\�����ׂ��Վ��j���[�X
 *         �\�����ׂ��j���[�X���Ȃ��ꍇ NULL
 */
//------------------------------------------------------------------------------------------
static const ELBOARD_SPNEWS_DATA* SearchTopNews( const GATEWORK* work )
{
  int i;
  u32        zone_id;
  EVENTWORK* evwork;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gameData );
  GF_ASSERT( work->gameSystem );

  zone_id = FIELDMAP_GetZoneID( work->fieldmap );
  evwork  = GAMEDATA_GetEventWork( work->gameData );

  // �f�[�^�������Ă��Ȃ�
  if( !work->spNewsData ) { return NULL; }

  // �Վ��j���[�X�f�[�^������
  for( i=0; i<work->spNewsDataNum; i++ )
  { 
    BOOL flag_hit = EVENTWORK_CheckEventFlag( evwork, work->spNewsData[i].flag );
    BOOL zone_hit = ELBOARD_SPNEWS_DATA_CheckZoneHit( &work->spNewsData[i], zone_id );
    BOOL version_hit = ELBOARD_SPNEWS_DATA_CheckVersionHit( &work->spNewsData[i] );

    // �o�[�W������v and �t���OON and �]�[����v
    if( version_hit && flag_hit && zone_hit ) { return &work->spNewsData[i]; }
  }
  return NULL;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �\�����ׂ��j���[�X�̒���, �ł��D�揇�ʂ̍����Վ��W���j���[�X����������
 *
 * @param work �M�~�b�N�Ǘ����[�N
 *
 * @return �ł��D�揇�ʂ̍���, �\�����ׂ��Վ��j���[�X
 *         �\�����ׂ��j���[�X���Ȃ��ꍇ NULL
 */
//------------------------------------------------------------------------------------------
static const ELBOARD_SPNEWS_DATA* SearchGymNews( const GATEWORK* work )
{
  int i;
  u32        zoneID;
  EVENTWORK* evwork;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gameData );

  zoneID = FIELDMAP_GetZoneID( work->fieldmap );
  evwork = GAMEDATA_GetEventWork( work->gameData );

  // �f�[�^�������Ă��Ȃ�
  if( work->spNewsData == NULL ) { return NULL; }

  // �Վ��j���[�X�f�[�^������
  for( i=0; i<work->spNewsDataNum; i++ )
  { 
    BOOL flag_hit = EVENTWORK_CheckEventFlag( evwork, work->spNewsData[i].flag );
    BOOL zone_hit = ELBOARD_SPNEWS_DATA_CheckZoneHit( &work->spNewsData[i], zoneID );

    // �t���OON and �]�[����v�̃W�����𔭌�
    if( flag_hit && zone_hit ) {
      if( work->spNewsData[i].newsType == SPNEWS_TYPE_GYM ) {
        return &work->spNewsData[i];
      }
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �w�肵���t���O�ɑΉ�����j���[�X����������
 *
 * @param work �M�~�b�N�Ǘ����[�N
 * @param flag ��������t���O
 *
 * @return �w�肵���t���O�ɑΉ�����j���[�X
 *         �j���[�X���Ȃ��ꍇ NULL
 */
//------------------------------------------------------------------------------------------
static const ELBOARD_SPNEWS_DATA* SearchFlagNews( const GATEWORK* work, u32 flag )
{
  int i;

  GF_ASSERT( work );

  // �f�[�^�������Ă��Ȃ�
  if( !work->spNewsData ){ return NULL; }

  // �Վ��j���[�X�f�[�^������
  for( i=0; i < work->spNewsDataNum; i++ )
  { 
    if( work->spNewsData[i].flag == flag )
    {
      return &work->spNewsData[i];
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �Վ��j���[�X�̗L���𒲂ׂ�
 *
 * @param work �M�~�b�N�Ǘ����[�N
 *
 * @return �\�����ׂ��Վ��j���[�X������ꍇ TRUE, �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------------------
static BOOL CheckSpecialNews( const GATEWORK* work )
{
  return (SearchTopNews( work ) != NULL);
}

//------------------------------------------------------------------------------------------
/**
 * @brief �j���[�X��o�^����
 *
 * @param work    �M�~�b�N�Ǘ����[�N
 * @param news    �o�^����j���[�X�p�����[�^
 * @param type    �o�^����j���[�X�̃^�C�v
 * @param sp_data �o�^����Վ��j���[�X�̃f�[�^( ����j���[�X�̏ꍇ��NULL )
 *
 * ���j���[�X�̒ǉ���, �K�����̊֐�����čs���B
 *  �@�f���փj���[�X��ǉ�
 *  �A�j���[�X�o�^�󋵂̍X�V
 *  �B�t���O���� ( �Վ��j���[�X�̏ꍇ )
 */
//------------------------------------------------------------------------------------------
static void EntryNews( GATEWORK* work, 
                       const NEWS_PARAM* news, NEWS_TYPE type, 
                       const ELBOARD_SPNEWS_DATA* sp_data )
{
  GF_ASSERT( work );

  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( work->elboard, news );

  // �o�^�󋵂��X�V
  if( type == NEWS_TYPE_SPECIAL ) { 
    AddNewsEntryData( work, type, sp_data->flag );
  }
  else {
    AddNewsEntryData( work, type, 0 );
  }

  // �t���O����
  // �Վ��j���[�X��ǉ������ꍇ, ���̃j���[�X�ɐݒ肳�ꂽ�t���O������s��.
  if( type == NEWS_TYPE_SPECIAL ) {
    // �t���O�𗎂Ƃ�
    if( sp_data->flagControl == FLAG_CONTROL_RESET ) { 
      EVENTWORK* evwork = GAMEDATA_GetEventWork( work->gameData ); 
      EVENTWORK_ResetEventFlag( evwork, sp_data->flag );
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �d���f���� �j���[�X�Z�b�g�A�b�v
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void SetupElboardNews( GATEWORK* work )
{
  GF_ASSERT( work );

  // �`�����s�I���j���[�X����
  if( CheckChampionNews( work ) == TRUE ) {
    SetupElboardNews_Champion( work ); 
  }
  // �Վ��j���[�X����
  else if( CheckSpecialNews( work ) ) {
    SetupElboardNews_Special( work ); 
  }
  // �Վ��j���[�X�Ȃ�
  else {
    SetupElboardNews_Normal( work );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���ɕ\��������� ����j���[�X�ō\������
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void SetupElboardNews_Normal( GATEWORK* work )
{
  // ���łɑ��̃j���[�X���o�^����Ă���ꍇ
  if( GOBJ_ELBOARD_GetNewsNum(work->elboard) != 0 ) {
    OS_Printf( "���łɑ��̃j���[�X���ݒ肳��Ă��܂��B\n" );
    GF_ASSERT(0);
    return;
  }

  if( !work->gateData ){ return; } // �f�[�^�������Ă��Ȃ�
  if( !work->gateData->newsDispValidFlag ) { return; } // �j���[�X�\���t���O��OFF

  // �j���[�X��ǉ�
  AddNews_DATE( work );        // ���t
  AddNews_WEATHER( work );     // �V�C
  AddNews_PROPAGATION( work ); // ��ʔ���
  AddNews_INFO_A( work );      // �n����A
  AddNews_INFO_B( work );      // �n����B
  AddNews_INFO_C( work );      // �n����C
  AddNews_CM( work );          // �ꌾCM
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���ɕ\��������� �`�����s�I���j���[�X�ō\������
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void SetupElboardNews_Champion( GATEWORK* work )
{
  // ���łɑ��̃j���[�X���o�^����Ă���ꍇ
  if( GOBJ_ELBOARD_GetNewsNum(work->elboard) != 0 ) {
    OS_Printf( "���łɑ��̃j���[�X���ݒ肳��Ă��܂��B\n" );
    GF_ASSERT(0);
    return;
  }

  if( !work->gateData ){ return; } // �f�[�^�������Ă��Ȃ�
  if( !work->gateData->newsDispValidFlag ) { return; } // �j���[�X�\���t���O��OFF

  // �j���[�X��ǉ�
  AddNews_DATE( work );        // ���t
  AddNews_WEATHER( work );     // �V�C
  AddNews_PROPAGATION( work ); // ��ʔ���
  AddNews_CHAMPION( work );    // �`�����s�I�����
  AddNews_CM( work );          // �ꌾCM
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���ɕ\��������� �Վ��j���[�X�ō\������
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void SetupElboardNews_Special( GATEWORK* work )
{ 
  // ���łɑ��̃j���[�X���o�^����Ă���ꍇ
  if( GOBJ_ELBOARD_GetNewsNum(work->elboard) != 0 ) {
    OS_Printf( "���łɑ��̃j���[�X���ݒ肳��Ă��܂��B\n" );
    GF_ASSERT(0);
    return;
  }

  if( !work->spNewsData ){ return; } // �f�[�^�������Ă��Ȃ�
  if( !work->gateData ){ return; } // �f�[�^�������Ă��Ȃ�
  if( !work->gateData->newsDispValidFlag ) { return; } // �j���[�X�\���t���O��OFF

  // �j���[�X��ǉ�
  AddNews_DATE( work );        // ���t
  AddNews_WEATHER( work );     // �V�C
  AddNews_PROPAGATION( work ); // ��ʔ���
  AddNews_SPECIAL( work );     // �Վ��j���[�X
  AddNews_CM( work );          // �ꌾCM
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(���t)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_DATE( GATEWORK* work )
{
  NEWS_PARAM news;
  WORDSET* wordset;
  RTCDate date;

  // ���[�h�Z�b�g�쐬
  wordset = WORDSET_Create( work->heapID );
  RTC_GetDate( &date );
  WORDSET_RegisterNumber( wordset, 0, date.month, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
  WORDSET_RegisterNumber( wordset, 1, date.day,   2, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );

  // �j���[�X�p�����[�^���쐬
  news.animeIndex = news_anm_index[NEWS_INDEX_DATE];
  news.texName    = news_tex_name[NEWS_INDEX_DATE];
  news.pltName    = news_plt_name[NEWS_INDEX_DATE];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = work->gateData->msgID_date;
  news.wordset    = wordset;

  // �j���[�X�o�^
  EntryNews( work, &news, NEWS_TYPE_DATE, NULL ); 

  // ���[�h�Z�b�g�j��
  WORDSET_Delete( wordset );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�V�C)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_WEATHER( GATEWORK* work )
{
  int i;
  u32 msgStrID;
  NEWS_PARAM news;
  WORDSET* wordset;
  WEATHER_NEWS_PARAM newsParam;

  // �V�C�j���[�X�p�����[�^���擾
  GetWeatherNewsParam( work, &newsParam );

  // ���[�h�Z�b�g�𐶐�
  wordset = WORDSET_CreateEx( WEATHER_ZONE_NUM, 256, work->heapID );

  // ���[�h�Z�b�g�o�^����
  {
    GFL_MSGDATA* msg_place_name;
    GFL_MSGDATA* msg_gate;

    // ���b�Z�[�W�f�[�^ �n���h���I�[�v��
    msg_place_name = GFL_MSG_Create( 
        GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_place_name_dat, work->heapID ); 

    msg_gate = GFL_MSG_Create( 
        GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_gate_dat, work->heapID ); 

    // �e�]�[���̒n���{�V�C��o�^
    for( i=0; i<WEATHER_ZONE_NUM; i++ )
    {
      STRBUF* strbuf_zone;    // �n��
      STRBUF* strbuf_weather; // �V�C
      STRBUF* strbuf_set;     // �n���{�V�C
      u16 zoneID;
      u8 weatherNo;

      // �]�[���w�肪�����l�Ȃ�\�����Ȃ�
      if( newsParam.zoneID[i] == ZONE_ID_MAX ) { continue; }

      zoneID = newsParam.zoneID[i];
      weatherNo = newsParam.weatherNo[i];

      // �n�����擾
      strbuf_zone = 
        GFL_MSG_CreateString( msg_place_name, ZONEDATA_GetPlaceNameID( zoneID ) );
      // �V�C���擾
      strbuf_weather = 
        GFL_MSG_CreateString( msg_gate, str_id_weather[ weatherNo ] );

      // �n���{�V�C�̃Z�b�g���쐬
      {
        WORDSET* wordset_expand = WORDSET_CreateEx( 2, 256, work->heapID );
        STRBUF* strbuf_expand = GFL_MSG_CreateString( msg_gate, msg_gate_weather );
        strbuf_set = GFL_STR_CreateBuffer( 64, work->heapID );
        WORDSET_RegisterWord( wordset_expand, 0, strbuf_zone, 0, TRUE, 0 );
        WORDSET_RegisterWord( wordset_expand, 1, strbuf_weather, 0, TRUE, 0 );
        WORDSET_ExpandStr( wordset_expand, strbuf_set, strbuf_expand );
        GFL_STR_DeleteBuffer( strbuf_expand );
        WORDSET_Delete( wordset_expand );
      }
      // �쐬��������������[�h�Z�b�g�ɓo�^
      WORDSET_RegisterWord( wordset, i, strbuf_set, 0, TRUE, 0 );
      // ��n��
      GFL_STR_DeleteBuffer( strbuf_zone );
      GFL_STR_DeleteBuffer( strbuf_weather );
      GFL_STR_DeleteBuffer( strbuf_set );
    }

    // ���b�Z�[�W�f�[�^�̃n���h�������
    GFL_MSG_Delete( msg_gate );
    GFL_MSG_Delete( msg_place_name );
  }

  // �\���Ɏg�p���郁�b�Z�[�WID��I��
  switch( GetValidWeatherNewsNum( &newsParam ) ) {
  case 1: msgStrID = msg_gate_weather_set_1; break;
  case 2: msgStrID = msg_gate_weather_set_2; break;
  case 3: msgStrID = msg_gate_weather_set_3; break;
  case 4: msgStrID = msg_gate_weather_set_4; break;
  default: GF_ASSERT(0);
  }

  // �j���[�X�p�����[�^���쐬
  news.animeIndex = news_anm_index[ NEWS_INDEX_WEATHER ];
  news.texName    = news_tex_name[ NEWS_INDEX_WEATHER ];
  news.pltName    = news_plt_name[ NEWS_INDEX_WEATHER ];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = msgStrID;
  news.wordset    = wordset;

  // �j���[�X�o�^
  EntryNews( work, &news, NEWS_TYPE_WEATHER, NULL ); 

  // ���[�h�Z�b�g�j��
  WORDSET_Delete( wordset ); 
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(��ʔ���)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_PROPAGATION( GATEWORK* work )
{
  NEWS_PARAM news;
  WORDSET* wordset;
  u16 zoneID;

  // ��ʔ������N���Ă���]�[�����擾
  zoneID = ENCPOKE_GetGenerateZone( work->gameData ); 

  // ��ʔ������N���Ă��Ȃ�
  if( zoneID == 0xFFFF ){ return; }

  // ���[�h�Z�b�g�쐬
  wordset = WORDSET_Create( work->heapID );

  // ���[�h�Z�b�g�ɒn�����Z�b�g
  {
    GFL_MSGDATA* msg_place_name;
    int str_id;
    STRBUF* zone_name;

    msg_place_name = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
                                     ARCID_MESSAGE, NARC_message_place_name_dat, work->heapID ); 
    str_id         = ZONEDATA_GetPlaceNameID( zoneID );
    zone_name      = GFL_MSG_CreateString( msg_place_name, str_id );
    WORDSET_RegisterWord( wordset, 0, zone_name, 0, TRUE, 0 );
    GFL_STR_DeleteBuffer( zone_name );
    GFL_MSG_Delete( msg_place_name );
  }

  // �j���[�X�p�����[�^���쐬
  news.animeIndex = news_anm_index[NEWS_INDEX_PROPAGATION];
  news.texName    = news_tex_name[NEWS_INDEX_PROPAGATION];
  news.pltName    = news_plt_name[NEWS_INDEX_PROPAGATION];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = work->gateData->msgID_propagation;
  news.wordset    = wordset;

  // �j���[�X�o�^
  EntryNews( work, &news, NEWS_TYPE_PROPAGATION, NULL ); 

  // ���[�h�Z�b�g�j��
  WORDSET_Delete( wordset );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�n����A)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_INFO_A( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // �����f�[�^���擾
  RTC_GetDate( &date ); 

  // �j���ɉ���������I��
  switch( date.week ) {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_infoG;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_infoD;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_infoA;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_infoE;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_infoB;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_infoF;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_infoC;  break;
  }

  news.animeIndex = news_anm_index[NEWS_INDEX_INFO_A];
  news.texName    = news_tex_name[NEWS_INDEX_INFO_A];
  news.pltName    = news_plt_name[NEWS_INDEX_INFO_A];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;

  // �j���[�X�o�^
  EntryNews( work, &news, NEWS_TYPE_INFO_A, NULL ); 
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�n����B)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_INFO_B( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // �����f�[�^���擾
  RTC_GetDate( &date ); 

  // �j���ɉ���������I��
  switch( date.week ) {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_infoH;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_infoE;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_infoB;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_infoF;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_infoC;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_infoG;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_infoD;  break;
  }

  news.animeIndex = news_anm_index[NEWS_INDEX_INFO_B];
  news.texName    = news_tex_name[NEWS_INDEX_INFO_B];
  news.pltName    = news_plt_name[NEWS_INDEX_INFO_B];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;

  // �j���[�X�o�^
  EntryNews( work, &news, NEWS_TYPE_INFO_B, NULL ); 
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�n����C)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_INFO_C( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // �����f�[�^���擾
  RTC_GetDate( &date ); 

  // �j���ɉ���������I��
  switch( date.week ) {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_infoI;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_infoF;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_infoC;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_infoG;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_infoD;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_infoH;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_infoE;  break;
  }

  news.animeIndex = news_anm_index[NEWS_INDEX_INFO_C];
  news.texName    = news_tex_name[NEWS_INDEX_INFO_C];
  news.pltName    = news_plt_name[NEWS_INDEX_INFO_C];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;

  // �j���[�X�o�^
  EntryNews( work, &news, NEWS_TYPE_INFO_C, NULL ); 
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�ꌾCM)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_CM( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // �����f�[�^���擾
  RTC_GetDate( &date );

  // �j���ɉ�����CM��I��
  switch( date.week ) {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_cmSun;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_cmMon;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_cmTue;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_cmWed;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_cmThu;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_cmFri;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_cmSat;  break;
  }
  news.animeIndex = news_anm_index[NEWS_INDEX_CM];
  news.texName    = news_tex_name[NEWS_INDEX_CM];
  news.pltName    = news_plt_name[NEWS_INDEX_CM];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;
  
  // �j���[�X�o�^
  EntryNews( work, &news, NEWS_TYPE_CM, NULL ); 
} 

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃`�����s�I���j���[�X��ǉ�����
 *
 * @param work     �M�~�b�N�Ǘ����[�N
 * @param news_idx �j���[�X�̒ǉ��ꏊ���w��
 *
 * @todo �|�P�������𐳂����Z�b�g����
 */
//------------------------------------------------------------------------------------------
static void AddNews_CHAMPION( GATEWORK* work )
{
  HEAPID heap_id; 
  NEWS_PARAM news;
  WORDSET* wordset;
  int monsno[6];
  int monsnum;
  int i;
  u32 strID;

  // ���[�h�Z�b�g�쐬
  heap_id = GOBJ_ELBOARD_GetHeapID( work->elboard );
  wordset = WORDSET_Create( heap_id );

  //�u�a������v�f�[�^�������, �Q�Ƃ���
  if( CheckDendouDataExist( work ) ) {
    GetChampMonsNo_byDendouData( work, monsno, &monsnum );
  }
  //�u�a������v�f�[�^���Ȃ����,�u�͂��߂ăN���A�v�f�[�^���Q�Ƃ���
  else {
    GetChampMonsNo_byFirstClear( work, monsno, &monsnum );
  }

  // �g�p���郁�b�Z�[�W��I��
  switch( monsnum ) {
  case 1: strID = msg_gate_champ_1; break;
  case 2: strID = msg_gate_champ_2; break;
  case 3: strID = msg_gate_champ_3; break;
  case 4: strID = msg_gate_champ_4; break;
  case 5: strID = msg_gate_champ_5; break;
  case 6: strID = msg_gate_champ_6; break;
  default: GF_ASSERT(0);
  }

  // �|�P�����������[�h�Z�b�g�ɓo�^
  for( i=0; i<monsnum; i++ )
  {
    WORDSET_RegisterPokeMonsNameNo( wordset, i, monsno[i] );
  }

  // �j���[�X�p�����[�^���쐬
  news.animeIndex = news_anm_index[NEWS_INDEX_INFO_A];
  news.texName    = news_tex_name[NEWS_INDEX_INFO_A];
  news.pltName    = news_plt_name[NEWS_INDEX_INFO_A];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = strID;
  news.wordset    = wordset;

  // �j���[�X�o�^
  EntryNews( work, &news, NEWS_TYPE_CHAMPION, NULL ); 

  // ���[�h�Z�b�g�j��
  WORDSET_Delete( wordset );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�Վ��j���[�X)
 * 
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_SPECIAL( GATEWORK* work )
{ 
  const ELBOARD_SPNEWS_DATA* news;

  // �Վ��j���[�X���擾
  news = SearchTopNews( work );

  GF_ASSERT( news ) // �Վ��j���[�X���Ȃ�

  // �j���[�X��ǉ�
  switch( news->newsType ) {
  case SPNEWS_TYPE_DIRECT: AddSpNews_DIRECT( work, news, NEWS_INDEX_INFO_A );  break;
  case SPNEWS_TYPE_GYM:    AddSpNews_GYM( work );                              break;
  default: GF_ASSERT(0);
  }
} 

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̗Վ��j���[�X��ǉ�����(���b�Z�[�W���̂܂�)
 *
 * @param work     �M�~�b�N�Ǘ����[�N
 * @param spnews   �Վ��j���[�X�f�[�^
 * @param news_idx �j���[�X�̒ǉ��ꏊ���w��
 */
//------------------------------------------------------------------------------------------
static void AddSpNews_DIRECT( GATEWORK* work, 
                              const ELBOARD_SPNEWS_DATA* spnews, NEWS_INDEX news_idx )
{
  NEWS_PARAM news;

  news.animeIndex = news_anm_index[ news_idx ];
  news.texName    = news_tex_name[ news_idx ];
  news.pltName    = news_plt_name[ news_idx ];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = spnews->msgID;
  news.wordset    = NULL;
  
  // �j���[�X�o�^
  EntryNews( work, &news, NEWS_TYPE_SPECIAL, spnews ); 
}


//------------------------------------------------------------------------------------------
/**
 * @brief �f���̗Վ��j���[�X��ǉ�����(�W�����)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddSpNews_GYM( GATEWORK* work )
{
  // �j���[�X�ǉ��ꏊ
  const ELBOARD_SPNEWS_DATA* news;
  int count = 0;

  // �S�ẴW���j���[�X��\������
  news = SearchGymNews( work ); 
  while( news && count < GYM_NEWS_MAX_NUM )
  {
    // �j���[�X��ǉ�
    AddSpNews_DIRECT( work, news, gym_news_idx[count++] );

    // ���̃W���j���[�X���擾
    news = SearchGymNews( work );
  } 
}

//------------------------------------------------------------------------------------------
/**
 * @brief �L���ȓV�C���̐����擾����
 *
 * @param param �V�C�j���[�X�p�����[�^
 *
 * @return �w�肵���V�C�j���[�X�p�����[�^�Ɋ܂܂��, �L���ȓV�C���̐�
 */
//------------------------------------------------------------------------------------------
static int GetValidWeatherNewsNum( WEATHER_NEWS_PARAM* param )
{
  int i;
  int num = 0;

  for( i=0; i<WEATHER_ZONE_NUM; i++ )
  {
    // �L���ȃf�[�^�𔭌�
    if( (param->zoneID[i] != ZONE_ID_MAX) &&
        (param->weatherNo[i] != WEATHER_NO_NONE) ) { num++; }
  }

  return num;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�C�j���[�X�p�����[�^������������
 *
 * @param param ����������p�����[�^
 */
//------------------------------------------------------------------------------------------
static void InitWeatherNewsParam( WEATHER_NEWS_PARAM* param )
{
  int i;

  // �����l�ŏ���������
  for( i=0; i<WEATHER_ZONE_NUM; i++ )
  {
    param->zoneID[i]    = ZONE_ID_MAX;
    param->weatherNo[i] = WEATHER_NO_NONE;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �\�����ׂ��V�C�j���[�X���擾����
 *
 * @param work
 * @param dest �擾�������̊i�[�� 
 */
//------------------------------------------------------------------------------------------
static void GetWeatherNewsParam( const GATEWORK* work, WEATHER_NEWS_PARAM* dest )
{
  WEATHER_NEWS_PARAM rareWeather;
  WEATHER_NEWS_PARAM movePokeWeather;
  int num = 0;
  int i;

  InitWeatherNewsParam( dest ); // ������
  GetRareWeather( work, &rareWeather ); // ���A�V�C���擾
  GetMovePokeWeather( work, &movePokeWeather ); // �ړ��|�P�����V�C���擾

  // �擾�����ړ��|�P�����V�C��o�^
  for( i=0; i<WEATHER_ZONE_NUM; i++ )
  {
    // �����f�[�^�𔭌�
    if( (movePokeWeather.zoneID[i] == ZONE_ID_MAX) ||
        (movePokeWeather.weatherNo[i] == WEATHER_NO_NONE) ) { break; }

    // ����V�C�̓�������������������
    if( SP_WEATHER_MAX_NUM <= num ) {
      GF_ASSERT(0);
      break;
    }

    // �o�^
    dest->zoneID[num]    = movePokeWeather.zoneID[i];
    dest->weatherNo[num] = movePokeWeather.weatherNo[i];
    num++;
  }

  // �擾��������V�C��o�^
  for( i=0; i<WEATHER_ZONE_NUM; i++ )
  {
    // �����f�[�^�𔭌�
    if( (rareWeather.zoneID[i] == ZONE_ID_MAX) ||
        (rareWeather.weatherNo[i] == WEATHER_NO_NONE) ) { break; }

    // ����V�C�̓�������������������
    if( SP_WEATHER_MAX_NUM <= num ) {
      GF_ASSERT(0);
      break;
    }

    // �o�^
    dest->zoneID[num]    = rareWeather.zoneID[i];
    dest->weatherNo[num] = rareWeather.weatherNo[i];
    num++;
  }

  // ����V�C���Ȃ��ꍇ��, �ʏ�̓V�C��o�^����
  if( num == 0 ) {
    dest->zoneID[0] = work->gateData->zoneID_weather_1;
    dest->zoneID[1] = work->gateData->zoneID_weather_2;
    dest->zoneID[2] = work->gateData->zoneID_weather_3;
    dest->zoneID[3] = work->gateData->zoneID_weather_4;
  }
  for( i=0; i<WEATHER_ZONE_NUM; i++ )
  {
    dest->weatherNo[i] = FIELDMAP_GetZoneWeatherID( work->fieldmap, dest->zoneID[i] );
  } 
}

//------------------------------------------------------------------------------------------
/**
 * @brief �������V�C�j���[�X���擾����
 *
 * @param work
 * @param dest �擾�������̊i�[��
 */
//------------------------------------------------------------------------------------------
static void GetRareWeather( const GATEWORK* work, WEATHER_NEWS_PARAM* dest )
{
  int i, j;
  int num; 

  // �f�[�^�̊i�[����N���A
  InitWeatherNewsParam( dest );

  // �`�F�b�N�Ώۃ]�[���ɂ���, ���A�V�C��T��
  num = 0;
  for( i=0; i<NELEMS(RareWeatherCheckZoneID); i++ )
  {
    // �����̓V�C���擾
    u16 zoneID = RareWeatherCheckZoneID[i];
    u8 weather = FIELDMAP_GetZoneWeatherID( work->fieldmap, zoneID );

    // ���A�V�C���ǂ����𔻒�
    for( j=0; j<NELEMS(RareWeatherNo); j++ )
    {
      // ���A�V�C�𔭌�
      if( weather == RareWeatherNo[j] ) {
        // ���A�V�C�̓�������������������
        if( WEATHER_ZONE_NUM <= num ) {
          GF_ASSERT(0);
          break;
        }
        // �V�C�j���[�X�p�����[�^�ɒǉ�
        dest->zoneID[ num ] = zoneID;
        dest->weatherNo[ num ] = weather;
        num++;
      }
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ړ��|�P�����Ɋւ���V�C�j���[�X���擾����
 *
 * @param work
 * @param dest �擾�������̊i�[��
 */
//------------------------------------------------------------------------------------------
static void GetMovePokeWeather( const GATEWORK* work, WEATHER_NEWS_PARAM* dest )
{
  SAVE_CONTROL_WORK* save;
  ENC_SV_PTR encSave;
  int movePokeID;
  u16 zoneID;
  u8 weatherNo;

  save    = GAMEDATA_GetSaveControlWork( work->gameData );
  encSave = EncDataSave_GetSaveDataPtr( save );

  // �f�[�^�̊i�[����N���A
  InitWeatherNewsParam( dest );

  // �ړ��|�P���������Ȃ�
  if( MP_CheckMovePoke( encSave ) == FALSE ) { return; }

  // �ړ��|�P����ID������
  switch( GetVersion() ) {
  case VERSION_WHITE: movePokeID = MOVE_POKE_RAIKAMI;  break;
  case VERSION_BLACK: movePokeID = MOVE_POKE_KAZAKAMI; break;
  default: GF_ASSERT(0);
  }

  // �V�C������
  switch( movePokeID ) { 
  case MOVE_POKE_RAIKAMI:  weatherNo = WEATHER_NO_RAIKAMI; break;
  case MOVE_POKE_KAZAKAMI: weatherNo = WEATHER_NO_KAZAKAMI; break;
  default: GF_ASSERT(0);
  }

  // �ړ��|�P����������]�[��ID���擾
  zoneID = MP_GetMovePokeZoneID( encSave, movePokeID ); 

  // �V�C�j���[�X�f�[�^�ɒǉ�
  dest->zoneID[0]    = zoneID;
  dest->weatherNo[0] = weatherNo;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �Z�[�u���[�N���o�͂���
 *
 * @param save
 */
//------------------------------------------------------------------------------------------
static void DebugPrint_SAVEWORK( const SAVEWORK* save )
{
  int i;
  OBATA_Printf( "GIMMICK-GATE: gimmick load\n" );
  OBATA_Printf( "-recoveryFrame = %d\n", save->recoveryFrame );
  OBATA_Printf( "-newsEntryData.newsNum = %d\n",  save->newsEntryData.newsNum );
  for( i=0; i<NEWS_INDEX_NUM; i++ )
  {
    OBATA_Printf( "-newsEntryData.newsType[%d] = ", i );
    switch( save->newsEntryData.newsType[i] ) {
    case NEWS_TYPE_NULL:        OBATA_Printf( "NULL\n" );        break;
    case NEWS_TYPE_DATE:        OBATA_Printf( "DATE\n" );        break;
    case NEWS_TYPE_WEATHER:     OBATA_Printf( "WEATHER\n" );     break;
    case NEWS_TYPE_PROPAGATION: OBATA_Printf( "PROPAGATION\n" ); break;
    case NEWS_TYPE_INFO_A:      OBATA_Printf( "INFO_A\n" );      break;
    case NEWS_TYPE_INFO_B:      OBATA_Printf( "INFO_B\n" );      break;
    case NEWS_TYPE_INFO_C:      OBATA_Printf( "INFO_C\n" );      break;
    case NEWS_TYPE_CM:          OBATA_Printf( "CM\n" );          break;
    case NEWS_TYPE_SPECIAL:     OBATA_Printf( "SPECIAL\n" );     break;
    case NEWS_TYPE_CHAMPION:    OBATA_Printf( "CHAMPION\n" );    break;
    default:                    OBATA_Printf( "UNKNOWN\n" );     break;
    }
  }
  for( i=0; i<NEWS_INDEX_NUM; i++ )
  {
    OBATA_Printf( "-newsEntryData.spNewsFlag[%d] = %d\n", 
                  i, save->newsEntryData.spNewsFlag[i] );
  }
}
