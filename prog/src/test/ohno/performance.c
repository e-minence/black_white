//===================================================================
/**
 * @file	performance
 * @brief	�p�t�H�[�}���X���[�^�[�@�f�o�b�O�p
 * @author	GAME FREAK Inc.
 * @date	08.08.01
 */
//===================================================================

#ifdef PM_DEBUG

#include <nitro.h>
#include <nnsys.h>
#include "gflib.h"
#include "performance.h"


//==============================================================================
//	�萔��`
//==============================================================================
///���[�^�[�^�C�v
enum{
	METER_TYPE_START,	///<�������׊J�n���[�^�[
	METER_TYPE_END,		///<�������׏I�����[�^�[
};

///���[�^�[�̃A�j���p�^�[����
#define METER_ANM_PATERN		(2)
///���[�^�[��CGX�W�J�ʒu(�L�����N�^�ԍ�)
#define METER_CHARNO			(1024 - METER_ANM_PATERN)
///���[�^�[�̏�����������CGX�W�J�ʒu(�L�����N�^�ԍ�)
#define METER_DELAY_CHARNO		(METER_CHARNO + 1)
///���[�^�[�̃p���b�g�ԍ�
#define METER_PALNO				(15)


//OBJ16K���[�h�̎�
#define METER_ICON_CHAR_OFFSET16  ( (512-METER_ANM_PATERN)*32 )	
//OBJ32K���[�h�̎�
#define METER_ICON_CHAR_OFFSET32  ( (1024-METER_ANM_PATERN)*32 )	
//OBJ64K���[�h�̎�
#define METER_ICON_CHAR_OFFSET64 ( (1024-METER_ANM_PATERN)*32 )	
//OBJ80K���[�h�̎�
#define METER_ICON_CHAR_OFFSET80 ( (2560-METER_ANM_PATERN)*32 )	
//OBJ128K���[�h�̎�
#define METER_ICON_CHAR_OFFSET128 ( (4096-METER_ANM_PATERN)*32 )	
//OBJ256K���[�h�̎�
#define METER_ICON_CHAR_OFFSET256 ( (8192-METER_ANM_PATERN)*32 )	


//==============================================================================
//	�\���̒�`
//==============================================================================
///�p�t�H�[�}���X�A�v�����[�N�\����
typedef struct{
	u32 start_vblank_count;		//�v���J�n����V�u�����N�J�E���^�̒l
	s16 start_vcount;			//�v���J�n����V�J�E���^�̒l
}PFM_APP_WORK;

///�p�t�H�[�}���X�V�X�e���\����
typedef struct{
	PFM_APP_WORK app[PERFORMANCE_ID_MAX];
	BOOL on_off;
}PERFORMANCE_SYSTEM;


//==============================================================================
//	�ϐ���`
//==============================================================================
///�p�t�H�[�}���X�V�X�e�����[�N
static PERFORMANCE_SYSTEM pfm_sys;


//==============================================================================
//	�f�[�^
//==============================================================================
///���[�^�[�\�����WY
static const s32 MeterPosY[] = {
	192-16,			//PERFORMANCE_ID_MAIN
	192-8,			//PERFORMANCE_ID_VBLANK
	0,				//PERFORMANCE_ID_USER_A
	8,				//PERFORMANCE_ID_USER_VBLANK_A
	24,				//PERFORMANCE_ID_USER_B
	32,				//PERFORMANCE_ID_USER_VBLANK_B
};
SDK_COMPILER_ASSERT(NELEMS(MeterPosY) == PERFORMANCE_ID_MAX);


//NcgOutCharText�R���o�[�^ Version:1.01
//�L�����N�^�� = 0x2(2)
static const u8 performance_meter[] = {
//0x0
	0x0,0x0,0x0,0x44,0x0,0x40,0x44,0x41,
	0x40,0x14,0x11,0x41,0x14,0x11,0x11,0x41,
	0x40,0x14,0x11,0x41,0x0,0x40,0x44,0x41,
	0x0,0x0,0x0,0x44,0x0,0x0,0x0,0x0,
//0x1
	0x0,0x0,0x0,0x44,0x0,0x40,0x44,0x43,
	0x40,0x34,0x33,0x43,0x34,0x33,0x33,0x43,
	0x40,0x34,0x33,0x43,0x0,0x40,0x44,0x43,
	0x0,0x0,0x0,0x44,0x0,0x0,0x0,0x0,
};

//NcgOutCharText�R���o�[�^ Version:1.01
static const u16 performance_meter_ncl[] = {
	0x5c1f,0x0667,0x1e7f,0x187b,0x0000,0x0000,0x0000,0x0000,	//0
	0x0000,0x0000,0x0000,0x5ad6,0x5ad6,0x0000,0x18c6,0x7fff,
};


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static void Performance_Draw(int meter_type, PERFORMANCE_ID id, s32 v_count, BOOL delay);
static void Performance_CGXTrans(void);
static void MeterCGX_OffsetGet(u32 *offset, u32 *anm_offset);



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �p�t�H�[�}���X���[�^�[�V�X�e���̏�����
 */
//--------------------------------------------------------------
void DEBUG_PerformanceInit(void)
{
	GFL_STD_MemClear(&pfm_sys, sizeof(PERFORMANCE_SYSTEM));
	pfm_sys.on_off = TRUE;	//�f�t�H���g�ŕ\��ON
}

//--------------------------------------------------------------
/**
 * @brief   �p�t�H�[�}���X���[�^�V�X�e���̃��C������
 *
 * ���̏��p�t�H�[�}���X���[�^�[��ON,OFF�؂�ւ��̊Ď�����
 */
//--------------------------------------------------------------
void DEBUG_PerformanceMain(void)
{
	int debugButtonTrg = GFL_UI_KEY_GetTrg()&PAD_BUTTON_DEBUG;
	
	if(debugButtonTrg){
		pfm_sys.on_off ^= TRUE;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �p�t�H�[�}���X���[�^�[�̃X�^�[�g���C���`��
 *
 * @param   id		�p�t�H�[�}���X���[�^�[ID
 */
//--------------------------------------------------------------
void DEBUG_PerformanceStartLine(PERFORMANCE_ID id)
{
	PFM_APP_WORK *app;
	
	GF_ASSERT(id < PERFORMANCE_ID_MAX);
	
	app = &pfm_sys.app[id];
	app->start_vblank_count = OS_GetVBlankCount();
	
	Performance_Draw(METER_TYPE_START, id, GX_GetVCount(), FALSE);
}

//--------------------------------------------------------------
/**
 * @brief   �p�t�H�[�}���X���[�^�[�̏I�[���C���`��
 *
 * @param   id		�p�t�H�[�}���X���[�^�[ID
 */
//--------------------------------------------------------------
void DEBUG_PerformanceEndLine(PERFORMANCE_ID id)
{
	PFM_APP_WORK *app;
	u32 end_vblank_count;
	
	GF_ASSERT(id < PERFORMANCE_ID_MAX);
	
	app = &pfm_sys.app[id];
	end_vblank_count = OS_GetVBlankCount();
	Performance_Draw(METER_TYPE_END, id, GX_GetVCount(), app->start_vblank_count != end_vblank_count);
}




//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �p�t�H�[�}���X���[�^�[�`��
 *
 * @param   meter_type			���[�^�[�^�C�v
 * @param   id					�p�t�H�[�}���X���[�^�[ID
 * @param   v_count				V�J�E���^
 * @param   delay				�x���t���O(TRUE:�x�����Ă���)
 */
//--------------------------------------------------------------
static void Performance_Draw(int meter_type, PERFORMANCE_ID id, s32 v_count, BOOL delay)
{
	GXOamAttr *meter_oam;
	GXOamEffect effect_type;
	s32 x, char_no;
	s32 calc_v_count;
	
	meter_oam = &(((GXOamAttr *)HW_OAM)[127 - id*2 - meter_type]);
	
	calc_v_count = v_count;
	if(calc_v_count < 192){	//VBlank���Ԃ���ʏ㍶�[�ɂ���̂Ŋ��ԓ��Ɗ��ԊO�̐��l�𔽓]����
		calc_v_count += 262-192;
	}
	else{
		calc_v_count -= 192;
	}
	x = 255 * calc_v_count / 262;	//V�J�E���^����ʉ����T�C�Y�̊����ɕϊ�
	if(id & 1){	//VBlank���Ԃ̏������ׂ𒲂ׂ�ID
		if(v_count < 192){
			delay = TRUE;	//VBlank���ԊO
		}
	}
	else{	//���C�����[�v���̏������ׂ𒲂ׂ�ID
		if(meter_type == METER_TYPE_START){
			if(v_count > 191){
			//	x = 0;	//Start��VBlank���ԓ��̏ꍇ�͏������ׂ̌��Ղ����l������X�ʒu��0�ɂ���
						//��VBlank���荞�݂̏����́A������AVBlank���Ԃ̏I�������Ă��Ȃ��̂ŁA
						//  �������I���΂����Ƀ��C�����[�v�̏���������
			}
		}
	}
	
	if(meter_type == METER_TYPE_START){
		effect_type = GX_OAM_EFFECT_NONE;
	}
	else{
		effect_type = GX_OAM_EFFECT_FLIP_H;
		x -= 8;
	}
	
	if(x > 259){	//������Ɖ�ʒ[�ɐH������ł�����������ׂ����₷���̂ŁB
		x %= 256;
	}
	char_no = (delay == TRUE) ? METER_DELAY_CHARNO : METER_CHARNO;
	switch(GX_GetBankForOBJ()){
	case GX_VRAM_OBJ_16_F:
	case GX_VRAM_OBJ_16_G:
		char_no -= 512;
		break;
	}
	
	//�L�����f�[�^�]��
	Performance_CGXTrans();
	
	//OAM�Z�b�g
	G2_SetOBJAttr(
		meter_oam,
		x,							//x
		MeterPosY[id],				//y
		0,							//priority
		GX_OAM_MODE_NORMAL,			//mode
		0,							//mosaic
		effect_type,				//effect
		GX_OAM_SHAPE_8x8,			//shape
		GX_OAM_COLORMODE_16,		//color
		char_no,					//charName
		METER_PALNO,				//cParam
		0							//rsParam
	);
	
#if 0
	{
		PFM_APP_WORK *app;
		app = &pfm_sys.app[id];
		if(id==0){
			if(meter_type==0){
				OS_TPrintf("�J�n���C��=%d, v_count=%d, start_v=%d, end_v=%d\n", x, v_count, app->start_vblank_count, OS_GetVBlankCount());
			}
			else{
				OS_TPrintf("�I�����C��=%d, v_count=%d, start_v=%d, end_v=%d\n", x, v_count, app->start_vblank_count, OS_GetVBlankCount());
			}
		}
		else if(id==1){
			if(meter_type==0){
				OS_TPrintf("VBlank�J�n���C��=%d, v_count=%d, start_v=%d, end_v=%d\n", x, v_count, app->start_vblank_count, OS_GetVBlankCount());
			}
			else{
				OS_TPrintf("VBlank�I�����C��=%d, v_count=%d, start_v=%d, end_v=%d\n", x, v_count, app->start_vblank_count, OS_GetVBlankCount());
			}
		}
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ���[�^�[CGX�]��
 */
//--------------------------------------------------------------
static void Performance_CGXTrans(void)
{
	u32 offset, anm_offset;
	
	MeterCGX_OffsetGet(&offset, &anm_offset);
	GFL_STD_MemCopy32(performance_meter, (void*)(HW_OBJ_VRAM + offset), 0x20);
	GFL_STD_MemCopy32(&performance_meter[0x20], (void*)(HW_OBJ_VRAM + offset + anm_offset), 0x20);
	//�p���b�g
	GFL_STD_MemCopy16(&performance_meter_ncl, (void*)(HW_OBJ_PLTT + 0x20*METER_PALNO), 0x20);
}

//--------------------------------------------------------------
/**
 * @brief   ���[�^�[CGX�̓]���ʒu�ƃA�j�����̃I�t�Z�b�g�A�h���X��Ԃ�
 *
 * @param   offset			���[�^�[CGX�]���ʒu
 * @param   anm_offset		�A�j�����̃I�t�Z�b�gCGX�]���ʒu
 */
//--------------------------------------------------------------
static void MeterCGX_OffsetGet(u32 *offset, u32 *anm_offset)
{
#if 0
	int objBank;
	
	objBank = GX_GetBankForOBJ();
	switch( objBank ){
	case GX_VRAM_OBJ_16_F:
	case GX_VRAM_OBJ_16_G:
		*offset = METER_ICON_CHAR_OFFSET16;
		*anm_offset = 0x20;
		break;
	case GX_VRAM_OBJ_32_FG:
		*offset = METER_ICON_CHAR_OFFSET32;
		*anm_offset = 0x20;
		break;
	case GX_VRAM_OBJ_64_E:
		*offset = METER_ICON_CHAR_OFFSET64;
		*anm_offset = 0x40;
		break;
	case GX_VRAM_OBJ_80_EF:
	case GX_VRAM_OBJ_80_EG:
		*offset = METER_ICON_CHAR_OFFSET80;
		*anm_offset = 0x80;
		break;
	case GX_VRAM_OBJ_96_EFG:
	case GX_VRAM_OBJ_128_A:
	case GX_VRAM_OBJ_128_B:
		*offset = METER_ICON_CHAR_OFFSET128;
		*anm_offset = 0x80;
		break;
	case GX_VRAM_OBJ_256_AB:
	default:
		*offset = METER_ICON_CHAR_OFFSET256;
		*anm_offset = 0x100;
		break;
	}
#else
	int objBank, obj_vram_mode;
	
	objBank = GX_GetBankForOBJ();
	switch( objBank ){
	case GX_VRAM_OBJ_16_F:
	case GX_VRAM_OBJ_16_G:
		*offset = 512*0x20;
		break;
	case GX_VRAM_OBJ_32_FG:
		*offset = 1024*0x20;
		break;
	case GX_VRAM_OBJ_64_E:
		*offset = 2048*0x20;
		break;
	case GX_VRAM_OBJ_80_EF:
	case GX_VRAM_OBJ_80_EG:
		*offset = 2560*0x20;
		break;
	case GX_VRAM_OBJ_96_EFG:
		*offset = 3072*0x20;
		break;
	case GX_VRAM_OBJ_128_A:
	case GX_VRAM_OBJ_128_B:
		*offset = 4096*0x20;
		break;
	case GX_VRAM_OBJ_256_AB:
	default:
		*offset = 8192*0x20;
		break;
	}

	obj_vram_mode = GX_GetOBJVRamModeChar();
	switch( obj_vram_mode ){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
	default:
		*anm_offset = 0x20;
		if(*offset > 1024*0x20){
			*offset = 1024*0x20;
		}
		break;
	case GX_OBJVRAMMODE_CHAR_1D_64K:
		*anm_offset = 0x40;
		if(*offset > 2048*0x20){
			*offset = 2048*0x20;
		}
		break;
	case GX_OBJVRAMMODE_CHAR_1D_128K:
		*anm_offset = 0x80;
		if(*offset > 4096*0x20){
			*offset = 4096*0x20;
		}
		break;
	case GX_OBJVRAMMODE_CHAR_1D_256K:
		*anm_offset = 0x100;
		break;
	}
	*offset -= (*anm_offset) * METER_ANM_PATERN;
#endif
}





#if 0
//------------------------------------------------------------------
/**
 * @brief		�p�t�H�[�}���X���[�^�[�̃X�^�[�g 
 */
//------------------------------------------------------------------

void DEBUG_PerformanceStart(void)
{
    int debugButtonTrg = GFL_UI_KEY_GetTrg()&PAD_BUTTON_DEBUG;
    
    if(debugButtonTrg){
        Onoff = Onoff ? FALSE
            : TRUE;
        if(Onoff){
            G2_SetWnd0InsidePlane(0,FALSE);
            G2_SetWnd1InsidePlane(0,FALSE);
            G2_SetWndOutsidePlane(0xff,FALSE);
            G2_SetWnd0Position(0, 0, 3, 192);
            G2_SetWnd1Position(263/2-1, 178, 263/2+1, 180);
            GX_SetVisibleWnd(3);
        }
    }
    if(Onoff){
        CountKeep = OS_GetVBlankCount();
    }
}

//------------------------------------------------------------------
/**
 * @brief		�p�t�H�[�}���X���[�^�[�̕\��
 */
//------------------------------------------------------------------

void DEBUG_PerformanceDisp(void)
{
    if(Onoff){
        u32 vn,cnt;

        cnt = OS_GetVBlankCount() - CountKeep;
        vn = GX_GetVCount() + (cnt * 263);
        vn = vn / 2;
        if(vn > 263){
            vn = 263;
        }
        G2_SetWnd0Position(0, 180, vn, 182);
//        OS_TPrintf("%d %d\n",vn,CountKeep);
    }
}
#endif

#endif //PM_DEBUG
