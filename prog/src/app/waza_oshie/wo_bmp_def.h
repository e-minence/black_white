/**
 *  @file wo_bmp_def.h
 *  @brief  �Z������ʃ��e������`
 *  @author Miyuki Iwasawa , Horoyuki Nakamura
 *  @date 08.04.04
 */



//////////////////////////////////////////////////////////////////////////////////////
//BMP�E�B���h�E�֘A��`


enum {
  WO_PAL_YESNO_WIN = 11,  // YesNo�E�B���h�E�p���b�g
  WO_PAL_YESNO_WIN2,    // YesNo�E�B���h�E�p���b�g2
  WO_PAL_TALK_WIN,    // �i�s�E�B���h�E�p���b�g
  WO_PAL_TALK_FONT,   // �i�s�t�H���g�p���b�g
  WO_PAL_SYS_FONT,    // �V�X�e���t�H���g�p���b�g
};

// ���j���[�E�B���h�E�]���ʒu
#define WO_YESNO_WIN_CGX    ( 1 )
#define WO_YESNO_WIN_PX     (32-6)
#define WO_YESNO_WIN_PY     (10)

// �i�s�E�B���h�E�]���ʒu
#define WO_TALK_WIN_CGX   ( WO_YESNO_WIN_CGX )

#define W_PARAM_PX    (17)
#define W_PARAM_PY    (6)

// �u�Ԃ�邢�v
#define WIN_STR_CLASS_PX  ( 1 )
#define WIN_STR_CLASS_PY  ( W_PARAM_PY )
#define WIN_STR_CLASS_SX  ( 6 )
#define WIN_STR_CLASS_SY  ( 2 )
#define WIN_STR_CLASS_PAL ( WO_PAL_SYS_FONT )
#define WIN_STR_CLASS_CGX ( WO_TALK_WIN_CGX+TALK_WIN_CGX_SIZ )

// �u����傭�v
#define WIN_STR_ATTACK_PX ( 8 )
#define WIN_STR_ATTACK_PY ( W_PARAM_PY )
#define WIN_STR_ATTACK_SX ( 6 )
#define WIN_STR_ATTACK_SY ( 2 )
#define WIN_STR_ATTACK_PAL  ( WO_PAL_SYS_FONT )
#define WIN_STR_ATTACK_CGX  ( WIN_STR_CLASS_CGX + WIN_STR_CLASS_SX * WIN_STR_CLASS_SY )

// �З͒l
#define WIN_PRM_ATTACK_PX ( WIN_STR_ATTACK_PX+6 )
#define WIN_PRM_ATTACK_PY ( WIN_STR_ATTACK_PY )
#define WIN_PRM_ATTACK_SX ( 3 )
#define WIN_PRM_ATTACK_SY ( 2 )
#define WIN_PRM_ATTACK_PAL  ( WO_PAL_SYS_FONT )
#define WIN_PRM_ATTACK_CGX  ( WIN_STR_HIT_CGX + WIN_STR_HIT_SX * WIN_STR_HIT_SY )

// �u�߂����イ�v
#define WIN_STR_HIT_PX    ( 19)
#define WIN_STR_HIT_PY    ( W_PARAM_PY )
#define WIN_STR_HIT_SX    ( 8 )
#define WIN_STR_HIT_SY    ( 2 )
#define WIN_STR_HIT_PAL   ( WO_PAL_SYS_FONT )
#define WIN_STR_HIT_CGX   ( WIN_STR_ATTACK_CGX + WIN_STR_ATTACK_SX * WIN_STR_ATTACK_SY )


// �����l
#define WIN_PRM_HIT_PX    ( WIN_STR_HIT_PX+8 )
#define WIN_PRM_HIT_PY    ( WIN_STR_HIT_PY )
#define WIN_PRM_HIT_SX    ( 3 )
#define WIN_PRM_HIT_SY    ( 2 )
#define WIN_PRM_HIT_PAL   ( WO_PAL_SYS_FONT )
#define WIN_PRM_HIT_CGX   ( WIN_PRM_ATTACK_CGX + WIN_PRM_ATTACK_SX * WIN_PRM_ATTACK_SY )

// �퓬����
#define WIN_BTL_INFO_PX   ( 1 )
#define WIN_BTL_INFO_PY   ( 0 )
#define WIN_BTL_INFO_SX   ( 30 )
#define WIN_BTL_INFO_SY   ( 6 )
#define WIN_BTL_INFO_PAL  ( WO_PAL_SYS_FONT )
#define WIN_BTL_INFO_CGX  ( WIN_PRM_HIT_CGX + WIN_PRM_HIT_SX * WIN_PRM_HIT_SY )

// ���b�Z�[�W�E�B���h�E
#define WIN_MSG_PX    ( 2 )
#define WIN_MSG_PY    ( 19 )
#define WIN_MSG_SX    ( 27 )
#define WIN_MSG_SY    ( 4 )
#define WIN_MSG_PAL   ( WO_PAL_TALK_FONT )
#define WIN_MSG_CGX   ( WIN_BTL_INFO_CGX + WIN_BTL_INFO_SX * WIN_BTL_INFO_SY )

// ��������E�B���h�E
#define WIN_EXP_PX    (1)
#define WIN_EXP_PY    (0)
#define WIN_EXP_SX    (30)
#define WIN_EXP_SY    (3)
#define WIN_EXP_PAL   (WO_PAL_SYS_FONT)
#define WIN_EXP_CGX   ( WIN_MSG_CGX + WIN_MSG_SX * WIN_MSG_SY )

// ����{�^���E�B���h�E
#define WIN_ABTN_PX   (17)
#define WIN_ABTN_PY   (21)
#define WIN_ABTN_SX   (6)
#define WIN_ABTN_SY   (3)
#define WIN_ABTN_PAL  (WO_PAL_SYS_FONT)
#define WIN_ABTN_CGX  ( WIN_EXP_CGX + WIN_EXP_SX * WIN_EXP_SY )

// �߂�{�^���E�B���h�E
#define WIN_BACK_PX   (25)
#define WIN_BACK_PY   (21)
#define WIN_BACK_SX   (6)
#define WIN_BACK_SY   (3)
#define WIN_BACK_PAL  (WO_PAL_SYS_FONT)
#define WIN_BACK_CGX  ( WIN_ABTN_CGX + WIN_ABTN_SX * WIN_ABTN_SY )

// �Z���X�g
#define WIN_LIST_PX   ( 8 )
#define WIN_LIST_PY   ( 9 )
#define WIN_LIST_SX   ( 21 )
#define WIN_LIST_SY   ( 11 )
#define WIN_LIST_PAL  ( WO_PAL_SYS_FONT )
#define WIN_LIST_CGX  ( WIN_BACK_CGX + WIN_BACK_SX * WIN_BACK_SY )

// �͂�/������
#define WIN_YESNO_PX  ( 23 )
#define WIN_YESNO_PY  ( 13 )
#define WIN_YESNO_SX  ( 7 )
#define WIN_YESNO_SY  ( 4 )
#define WIN_YESNO_PAL ( WO_PAL_TALK_FONT )
#define WIN_YESNO_CGX ( WIN_LIST_CGX + WIN_LIST_SX * WIN_LIST_SY )

// ��@�^�C�g��
#define WIN_TITLE_PX  ( 1 )
#define WIN_TITLE_PY  ( 0 )
#define WIN_TITLE_SX  ( 20 )
#define WIN_TITLE_SY  ( 3 )
#define WIN_TITLE_PAL ( WO_PAL_SYS_FONT )
#define WIN_TITLE_CGX ( 1 )

// ��@�Z���X�g
#define WIN_MWAZA_PX  (  7 )
#define WIN_MWAZA_PY  (  5 )
#define WIN_MWAZA_SX  ( 11 )
#define WIN_MWAZA_SY  ( 16 )
#define WIN_MWAZA_PAL ( WO_PAL_SYS_FONT )
#define WIN_MWAZA_CGX ( WIN_TITLE_CGX + WIN_TITLE_SX*WIN_TITLE_SY )

// ��@�|�P������
#define WIN_MPRM_PX ( 21 )
#define WIN_MPRM_PY (  4 )
#define WIN_MPRM_SX (  9 )
#define WIN_MPRM_SY (  8 )
#define WIN_MPRM_PAL  ( WO_PAL_SYS_FONT )
#define WIN_MPRM_CGX  ( WIN_MWAZA_CGX + WIN_MWAZA_SX*WIN_MWAZA_SY )

#define BG_WLIST_PX (0)
#define BG_WLIST_PY (WIN_LIST_PY)
#define BG_WLIST_SX (16)
#define BG_WLIST_SY (4)

#define BG_WLIST_SPX  (0)
#define BG_WLIST_SPY  (0)

#define BG_ABTN_PX    (16)
#define BG_ABTN_PY    (20)
#define BG_ABTN_SX    (8)
#define BG_ABTN_SY    (4)
#define BG_ABTN_OX    (0)
#define BG_ABTN_OY    (12)

// ���ʂ�PP�\���̗p�̈ʒu�w��
#define U_WLIST_PP1_X   ( 8 )
#define U_WLIST_PP2_X   ( U_WLIST_PP1_X + 24)

// ����ʂ�PP�\���p�̈ʒu�w��
#define F_WLIST_PP1_OX  (12*8)
#define F_WLIST_PP2_OX  (F_WLIST_PP1_OX+3*8)

#define F_WLIST_NAME_OX (0)
#define F_WLIST_LEVEL_OX  (8)

//////////////////////////////////////////////////////////////////////////////////////
//
// ���b�Z�[�W�e�[�u��ID
enum {
  MSG_SEL_WAZA = 0,   // @0�Ɂ@�ǂ̂킴���@���ڂ�������H
  MSG_SET_CHECK,      // @1�@��  ���ڂ������܂����H
  MSG_END_CHECK,      // @0�Ɂ@�킴��  ���ڂ�������̂��@������߂܂����H
  MSG_SET_WAZA,     // @0�́@�����炵���@@1���@���ڂ����I��
  MSG_CHG_CHECK,      // @0�́@�����炵���@@1���@���ڂ������c�c���@�������@�`
  MSG_DEL_WAZA,     // �P�@�Q�́c�c�@�|�J���I���@@0�́@@1�� �����������@�`
  MSG_CHG_WAZA,     // @0�́@�����炵�� @1���@���ڂ����I��
  MSG_DEL_CHECK,      // ����ł́c�c�@@1���@���ڂ���̂��@������߂܂����H
  MSG_CANCEL,       // @0�́@@1���@���ڂ����Ɂ@��������I��
  MSG_SUB_ITEM,     // @2�́@���ꂢ�Ɂ@�n�[�g�̃E���R���@�P�܂��@�������I��
  MSG_DEL_ENTER,      // @0���@�킷�ꂳ���ā@��낵���ł��ˁH"
};

// �͂��E�����������e�[�u��ID
enum {
  YESNO_SET_CHECK = 0,  // @1�@��  ���ڂ������܂����H
  YESNO_END_CHECK,    // @0�Ɂ@�킴��  ���ڂ�������̂��@������߂܂����H
  YESNO_CHG_CHECK,    // @0�́@�����炵���@@1���@���ڂ������c�c���@�������@�`
  YESNO_DEL_CHECK,    // ����ł́c�c�@@1���@���ڂ���̂��@������߂܂����H
  YESNO_DEL_ENTER,    // @0���@�킷�ꂳ���ā@��낵���ł��ˁH"
};

// �����W�J���[�h
enum {
  STR_MODE_LEFT = 0,  // ���l��
  STR_MODE_RIGHT,   // �E�l��
  STR_MODE_CENTER,  // ����
};

#define TMP_MSGBUF_SIZ  ( 256 )   // �ėp������W�J�̈�T�C�Y

#define WOFCOL_N_BLACK   PRINTSYS_LSB_Make( 1,  2, 0 )    // �t�H���g�J���[�F��
#define WOFCOL_N_WHITE   PRINTSYS_LSB_Make( 15, 2, 0 )  // �t�H���g�J���[�F��
#define WOFCOL_W_BLACK   PRINTSYS_LSB_Make( 1,  2, 15 )     // �t�H���g�J���[�F��

#define HEART_PUT_PX  ( 34 )    // �n�[�g��������X���W
#define HEART_PUT_PY  ( 5 )   // �n�[�g��������Y���W
#define HEART_MAX   ( 6 )   // �n�[�g��

#define HEART_POINT_CHR ( 0x0e )  // �n�[�g�L�����ԍ��i�|�C���g����j
#define HEART_NONE_CHR  ( 0x12 )  // �n�[�g�L�����ԍ��i�|�C���g�Ȃ��j


// �L�������\�[�XID
enum {
  WO_CHR_ID_APP_COMMON = 0,   // ���X�g�J�[�\��
  WO_CHR_ID_CURSOR,           // �I���J�[�\��
  WO_CHR_ID_KIND,             // ���ރA�C�R��
  WO_CHR_ID_TYPE1,            // �^�C�v�A�C�R��1
  WO_CHR_ID_TYPE2,            // �^�C�v�A�C�R��2
  WO_CHR_ID_TYPE3,            // �^�C�v�A�C�R��3
  WO_CHR_ID_TYPE4,            // �^�C�v�A�C�R��4
  WO_CHR_ID_TYPE5,            // �^�C�v�A�C�R��5
  WO_CHR_ID_TYPE6,            // �^�C�v�A�C�R��6
  WO_CHR_ID_TYPE7,            // �^�C�v�A�C�R��7
  WO_CHR_ID_TYPE8,            // �^�C�v�A�C�R��8
  WO_CHR_ID_TYPE_MAX=WO_CHR_ID_TYPE8,
  WO_CHR_ID_POKEGRA,          // �|�P�������ʊG
  WO_CHR_ID_MAX               // �ő�
};

// �p���b�g���\�[�XID
enum {
  WO_PAL_ID_APP_COMMON = 0, // �^�b�`�o�[���ʑf�ރp���b�g
  WO_PAL_ID_OBJ,            // ���낢��
  WO_PAL_ID_TYPE,           // �^�C�v�E���ރA�C�R��
  WO_PAL_ID_TYPE_M,         // �^�C�v�E���ރA�C�R��
  WO_PAL_ID_POKEGRA,        // �|�P�������ʊG
  WO_PAL_ID_MAX             // �ő�
};

// �Z�����\�[�XID
enum {
  WO_CEL_ID_APP_COMMON = 0,   // �^�b�`�o�[���ʑf��
  WO_CEL_ID_CURSOR,           // �I���J�[�\��
  WO_CEL_ID_TYPE,             // �^�C�v�A�C�R��
  WO_CEL_ID_POKEGRA,          // �|�P�������ʊG
  WO_CEL_ID_MAX               // �ő�
};

/*
// �Z���A�j�����\�[�XID
enum {
  WO_ANM_ID_APP_COMMON = 0,     // ���X�g�J�[�\��
  WO_ANM_ID_CURSOR,           // �I���J�[�\��
  WO_ANM_ID_TYPE,             // �^�C�v�A�C�R��
  WO_ANM_ID_MAX = WO_ANM_ID_TYPE - WO_ANM_ID_APP_COMMON + 1   // �ő�
};
*/

// �A�j���i���o�[
enum{
  ANMDW_CURSOR=0,
  ANMDW_ARROW_DT=2,    // ���^�b�`�ł���
  ANMDW_ARROW_DF=16,   // ���^�b�`�ł��Ȃ�
  ANMDW_ARROW_UT=3,    // ��^�b�`�ł���
  ANMDW_ARROW_UF=17,   // ��^�b�`�ł��Ȃ�
  ANMDW_ARROW_DTA=10,  // ���A�j����I��
  ANMDW_ARROW_DFA=23,  // ���A�j����I�t
  ANMDW_ARROW_UTA=11,  // ��A�j����I��
  ANMDW_ARROW_UFA=24,  // ��A�j����I�t
  ANMDW_EXIT=0,
};



// �p���b�gNo
enum{
  PALDW_01,
  PALDW_02,
  PALDW_ARROW,
  PALDW_CURSOR,
};

///�p���b�g��
#define DW_ACTPAL_NUM (6)
#define TICON_ACTPAL_IDX  (DW_ACTPAL_NUM)
#define TICON_ACTPAL_IDX_M  (0)

// �y�[�W�J�[�\����
#define PAGE_CUR_L_PX ( 146 )
#define PAGE_CUR_L_PY ( 8 )
// �y�[�W�J�[�\���E
#define PAGE_CUR_R_PX ( 246 )
#define PAGE_CUR_R_PY ( 8 )
// ���X�g�J�[�\����
#define LIST_CUR_U_PX ( 8*5 )
#define LIST_CUR_U_PY ( 192-24 )
// ���X�g�J�[�\����
#define LIST_CUR_D_PX ( 8 )
#define LIST_CUR_D_PY ( 192-24 )
// �I���J�[�\��
#define SEL_CURSOR_PX ( 127+3 )
#define SEL_CURSOR_PY (  84 )
#define SEL_CURSOR_SY (  24 )

#define SEL_CURSOR_P_PX (0)
#define SEL_CURSOR_P_PY (192-24)
#define SEL_CURSOR_P_SX (40)

#define SEL_CURSOR_B_PX (256-64)
#define SEL_CURSOR_B_PY (192-32)


// ���ރA�C�R��
#define KIND_ICON_PX  ( 33 )
#define KIND_ICON_PY  ( 56 )
// �^�C�v�A�C�R���P(�����)
#define TYPE_ICON1_PX ( 49 )
#define TYPE_ICON1_PY ( 80 )
// �^�C�v�A�C�R���Q
#define TYPE_ICON2_PX ( TYPE_ICON1_PX )
#define TYPE_ICON2_PY ( TYPE_ICON1_PY+24 )
// �^�C�v�A�C�R���R
#define TYPE_ICON3_PX ( TYPE_ICON1_PX )
#define TYPE_ICON3_PY ( TYPE_ICON2_PY+24 )
// �^�C�v�A�C�R���S
#define TYPE_ICON4_PX ( TYPE_ICON1_PX )
#define TYPE_ICON4_PY ( TYPE_ICON3_PY+24 )

// �^�C�v�A�C�R���T
#define TYPE_ICON5_PX ( 38 )
#define TYPE_ICON5_PY ( 48 )
// �^�C�v�A�C�R���U
#define TYPE_ICON6_PX ( TYPE_ICON5_PX )
#define TYPE_ICON6_PY ( TYPE_ICON5_PY+32 )
// �^�C�v�A�C�R���V
#define TYPE_ICON7_PX ( TYPE_ICON5_PX )
#define TYPE_ICON7_PY ( TYPE_ICON6_PY+32 )
// �^�C�v�A�C�R���W
#define TYPE_ICON8_PX ( TYPE_ICON5_PX )
#define TYPE_ICON8_PY ( TYPE_ICON7_PY+32 )

#define POKE_PX (204)
#define POKE_PY (124)

////////////////////////////////////////////////////////////////////
//���X�g
#define LIST_NUM    (4)
#define LIST_NUM_OFS  (LIST_NUM-1)
#define LIST_SY     (24)

#define CPOS_YMAX     (LIST_NUM+1)
#define CPOS_XMAX     (3)

enum{
 CPOS_LIST01,
 CPOS_LIST02,
 CPOS_LIST03,
 CPOS_LIST04,
 CPOS_PAGE_DW,
 CPOS_PAGE_UP,
 CPOS_BACK,
 CPOS_ABTN,
};

/////////////////////////////////////////////////////////////////////
//�^�b�`�p�l��
#define TP_WAZA_PX  (   32 )
#define TP_WAZA_SX  ( 24*8 )
#define TP_WAZA_SY  (   16 )
#define TP_WAZA1_PY (   72 )
#define TP_WAZA2_PY (TP_WAZA1_PY+TP_WAZA_SY)
#define TP_WAZA3_PY (TP_WAZA2_PY+TP_WAZA_SY)
#define TP_WAZA4_PY (TP_WAZA3_PY+TP_WAZA_SY)

#define TP_SB_SX  (5*8)
#define TP_SB_SY  (3*8)
#define TP_SB_PY  (21*8)
#define TP_SBD_PX (0)
#define TP_SBU_PX (TP_SBD_PX+TP_SB_SX)

#define TP_ABTN_PX  (16*8)
#define TP_ABTN_PY  ((21*8)-2)
#define TP_ABTN_SX  (8*8)
#define TP_ABTN_SY  (24+2)

#define TP_BACK_PX  (24*8)
#define TP_BACK_PY  (TP_ABTN_PY)
#define TP_BACK_SX  (TP_ABTN_SX)
#define TP_BACK_SY  (TP_ABTN_SY)
