#include <gflib.h>
#include "arc_def.h"
#include "net/network_define.h"
#include "savedata/save_tbl.h"

#include "system/main.h"
#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"
#include "system/bmp_winframe.h"
#include "system/wipe.h"
#include "print/global_font.h"
#include "print/wordset.h"

#include "mystery.naix"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_mystery.h"
#include "msg/msg_wifi_system.h"

#include "savedata/mystery_data.h"
#include "savedata/save_control.h"
#include "savedata/config.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"
#include "test/ariizumi/ari_debug.h"
#include "comm_mystery_func.h"



//--------------------------------------------------------------------------------------------
/**
 * VRAM������
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CommMysteryFunc_VramBankSet(void)
{
	//CLACT �̏��������ς��邱��
	const GFL_DISP_VRAM vramSetTable = {
		GX_VRAM_BG_128_C,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_32_H,			/* �T�u2D�G���W����BG */
		GX_VRAM_SUB_BGEXTPLTT_NONE,		/* �T�u2D�G���W����BG�g���p���b�g */
		GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_128_D,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_0_B,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_01_FG,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_32K,		// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
	};
	GFL_DISP_SetBank( &vramSetTable );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief	BG������
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CommMysteryFunc_BgInit( GFL_BG_INI * ini )
{
	{	/* BG SYSTEM */
		GFL_BG_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &BGsys_data );
	}

	{	/* MAIN DISP BG0 */
		GFL_BG_BGCNT_HEADER MBg0_Data = {
			0, 0, 0x0800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000,0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &MBg0_Data, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_M );
	}

	{	/* MAIN DISP BG1 */
		GFL_BG_BGCNT_HEADER MBg1_Data = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000,0x4000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &MBg1_Data, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME1_M );
	}

	{	/* MAIN DISP BG2 */
		GFL_BG_BGCNT_HEADER MBg2_Data = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x00000,0,
			GX_BG_EXTPLTT_23, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &MBg2_Data, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME2_M );
	}

	{	/* MAIN DISP BG3 */
		GFL_BG_BGCNT_HEADER MBg3_Data = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x0c000,0x4000,
			GX_BG_EXTPLTT_23, 3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &MBg3_Data, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME3_M );
	}
}


//------------------------------------------------------------------
/**
 * @brief	��������̂����炦�邩�`�F�b�N
 * @param	NONE
 * @return	NONE
 */
//------------------------------------------------------------------
int CommMysteryFunc_CheckGetGift(SAVE_CONTROL_WORK *sv, GIFT_COMM_PACK *gcp)
{
	u32 version;
	MYSTERY_DATA *fdata = SaveControl_DataPtrGet(sv,GMDATA_ID_MYSTERYDATA);

#if 0
	// �f�o�b�O�p�̋����G���[�����R�[�h
	return COMMMYSTERYFUNC_ERROR_GROUNDCHILD;
#endif
#ifdef DEBUG_ONLY_FOR_mituhara
	OS_TPrintf("�E�����r�[�R���̃o�[�W������ %08X �ł�\n", gcp->beacon.version);
	OS_TPrintf("��x�����󂯎��Ȃ��t���O: %d\n", gcp->beacon.only_one_flag);
	OS_TPrintf("�J�[�h�t����������̃t���O: %d\n", gcp->beacon.have_card);
	OS_TPrintf("���z�z�t���O: %d\n", gcp->beacon.groundchild_flag);
#endif

	
	version = gcp->beacon.version;
	
	// �f�o�b�O�p�̂ӂ����Ȃ�������̏�������
	if(version == 0xFFFFFFFF && gcp->beacon.event_id == 0xFFFF){
		MYSTERYDATA_Init(fdata);
		//FIXME �Z�[�u����
		//SaveData_SaveParts(sv, SVBLK_ID_NORMAL);
		OS_ResetSystem(0); 
	}


		//100�`152�̂o�a�q�̃f�[�^������v���`�i�Ŏ󂯎�邽�߂Ƀo�[�W�����r�b�g�𑝂₵�Ă���
		// k.ohno  2007/6/20
		if((gcp->beacon.event_id >= 100) && (gcp->beacon.event_id <= 152)){
			  version |= (1<<VERSION_PLATINUM)+(1<<VERSION_GOLD)+(1<<VERSION_SILVER);
		}

	// �Ώۃo�[�W�������O�Ȃ�ΑS�Ẵo�[�W�����Ŏ󂯎���
	if(version == 0)
		version = ~0;

	// �Ώۃo�[�W�����Ɋ܂܂�Ă��邩�`�F�b�N
	if((version & (1 << PM_VERSION)) == FALSE)
		return COMMMYSTERYFUNC_ERROR_VERSION;

	// ��x�����󂯎��Ȃ���������̂́A
	if(gcp->beacon.only_one_flag == TRUE &&
		 // ���łɎ󂯎���Ă��邩���ׂ�
		 MYSTERYDATA_IsEventRecvFlag(fdata, gcp->beacon.event_id) == TRUE)
		return COMMMYSTERYFUNC_ERROR_HAVE;

	// �J�[�h�t����������̂�
	if( gcp->beacon.have_card == TRUE &&
		 // �J�[�h��񂪃Z�[�u�ł��邩�`�F�b�N
		MYSTERYDATA_CheckCardDataSpace(fdata) == FALSE)
	{
#if DEB_ARI
		OS_TPrintf("�{���͂����ς��Ŏ󂯎��Ȃ��I�I\n");
#else
		return COMMMYSTERYFUNC_ERROR_FULLCARD;
#endif
	}

	// ����ɔz�B����񂪊܂܂�Ă��邨������̂�
	// �z�B���̃X���b�g���`�F�b�N����
#if DEB_ARI
	if(MYSTERYDATA_CheckDeliDataSpace(fdata) == FALSE)
		OS_TPrintf("�{���͂����ς��Ŏ󂯎��Ȃ��I�I\n");
#else
	if(MYSTERYDATA_CheckDeliDataSpace(fdata) == FALSE)
		return COMMMYSTERYFUNC_ERROR_FULLGIFT;
#endif
	// ���z�z�̃`�F�b�N
	if(gcp->beacon.groundchild_flag == 1)
		return COMMMYSTERYFUNC_ERROR_GROUNDCHILD;

	return COMMMYSTERYFUNC_ERROR_NONE;
}


/*  */
