//==============================================================================
/**
 * @file	gf_overlay.c
 * @brief	�I�[�o�[���C����
 * @author	GAME FREAK inc.
 * @date	2006.03.06
 */
//==============================================================================
#include <nitro.h>
#include <nitro/fs.h>
#include "gflib.h"
#include "gf_overlay.h"
#include "assert.h"


#if 0
NitroStaticInit���g�p����ɂ�
/*
 * NitroStaticInit() �� static initializer �Ɏw�肷��ɂ�
 * ���̃w�b�_���C���N���[�h���܂�.
 */
#include <nitro/sinit.h>

#endif


//==============================================================================
//	�萔��`
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief	�I�[�o�[���C�̓W�J�������G���A
 */
//--------------------------------------------------------------
typedef enum{
	GF_OVERLAY_AREA_MAIN = 0,			///<���C��������
	GF_OVERLAY_AREA_ITCM,				///<����TCM
	GF_OVERLAY_AREA_DTCM,				///<�f�[�^TCM
}GF_OVERLAY_AREA;

//--------------------------------------------------------------
//	�f�o�b�O�p��`
//--------------------------------------------------------------
#ifdef PM_DEBUG
///��`���L���̏ꍇ�̓I�[�o�[���C�̃A�����[�h���s���ɃA�����[�h�����̈���N���A����
#define DEBUG_OVELAY_UNLOAD_CLEAR
#endif


//==============================================================================
//	�\���̒�`
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief	�I�[�o�[���C�p���[�N��`
 */
//--------------------------------------------------------------
typedef struct{
	FSOverlayID loaded_id;
	BOOL loaded;
}OVERLAY_WORK;

//--------------------------------------------------------------
/**
 * @brief	�I�[�o�[���C����V�X�e���p���[�N��`
 */
//--------------------------------------------------------------
typedef struct{
	u8 main_num;
	u8 itcm_num;
	u8 dtcm_num;
	OVERLAY_WORK * main;
	OVERLAY_WORK * itcm;
	OVERLAY_WORK * dtcm;
}OVERLAY_SYSTEM;

//==============================================================================
//	�O���[�o���ϐ��錾
//==============================================================================
static OVERLAY_SYSTEM * OverlaySys;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static void Overlay_Unload(OVERLAY_WORK *ovwork);
static BOOL CheckCrossArea(const FSOverlayID id);
static OVERLAY_WORK * GetWorkPointer(GF_OVERLAY_AREA memory_area);
static int GetWorkSize(GF_OVERLAY_AREA memory_area);
static BOOL GetOverlayAddress(const FSOverlayID id, u32 *start, u32 *end);
static GF_OVERLAY_AREA GetMemoryAreaID(const FSOverlayID id);
static void UnloadEachWork(OVERLAY_WORK * ov_wk, int count);


//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
void GFL_OVERLAY_boot(HEAPID heapID, int main_num, int itcm_num, int dtcm_num)
{
	int size;
	size = sizeof(OVERLAY_SYSTEM) + sizeof(OVERLAY_WORK) * (main_num + itcm_num + dtcm_num);

	GF_ASSERT(main_num >= 0);
	GF_ASSERT(itcm_num >= 0);
	GF_ASSERT(dtcm_num >= 0);
	OverlaySys = GFL_HEAP_AllocMemory(heapID, size);
	GFL_STD_MemClear32(OverlaySys, size);

	OverlaySys->main_num = main_num;
	OverlaySys->itcm_num = itcm_num;
	OverlaySys->dtcm_num = dtcm_num;
	OverlaySys->main = (OVERLAY_WORK *)((u8*)OverlaySys + sizeof(OVERLAY_SYSTEM));
	OverlaySys->itcm = OverlaySys->main + main_num;
	OverlaySys->dtcm = OverlaySys->itcm + itcm_num;
}

//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static void UnloadEachWork(OVERLAY_WORK * ov_wk, int count)
{
	int i;
	
	for(i = 0; i < count; i++){
		if(ov_wk[i].loaded == TRUE){
			GF_ASSERT(0);
			Overlay_Unload(&ov_wk[i]);
		}
	}
}
//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
void GFL_OVERLAY_Exit(void)
{
	UnloadEachWork(OverlaySys->main, OverlaySys->main_num);

	UnloadEachWork(OverlaySys->itcm, OverlaySys->itcm_num);

	UnloadEachWork(OverlaySys->dtcm, OverlaySys->dtcm_num);

	GFL_HEAP_FreeMemory(OverlaySys);
}

//--------------------------------------------------------------
/**
 * @brief   �I�[�o�[���C���A�����[�h���܂�
 * @param   ovwork		�I�[�o�[���C���[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Overlay_Unload(OVERLAY_WORK *ovwork)
{
	//���ݓǂݍ��܂�Ă���I�[�o�[���C���W���[�����A�����[�h���܂�
	BOOL ret;

	GF_ASSERT(ovwork->loaded == TRUE);

	ret = FS_UnloadOverlay(MI_PROCESSOR_ARM9, ovwork->loaded_id);
	OS_Printf("overlay unload id = 0x%08X\n", ovwork->loaded_id);
	GF_ASSERT(ret == TRUE);
	ovwork->loaded = FALSE;

#ifdef DEBUG_OVELAY_UNLOAD_CLEAR
	{
		u32 start, end;
		//�A�����[�h�����I�[�o�[���C�̈��0�N���A���邽�߂ɉ������O�ɃA�h���X�擾
		GetOverlayAddress(ovwork->loaded_id, &start, &end);
		//�A�����[�h�����I�[�o�[���C�̈���N���A����
		OS_TPrintf("overlay unload start address = 0x%08X\n", start);
		OS_TPrintf("overlay unload end address = 0x%08X\n", end);
		MI_CpuFill8((void*)start, 0xff, end - start);
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �w�肵���I�[�o�[���CID�̃I�[�o�[���C���A�����[�h���܂�
 *
 * @param   id		�I�[�o�[���CID
 *
 */
//--------------------------------------------------------------
void GFL_OVERLAY_Unload(const FSOverlayID id)
{
	OVERLAY_WORK *ovwork;
	int memory_area;
	int size;
	int i;
	
	GF_ASSERT_MSG(OverlaySys != NULL, "Overlay System������������Ă��Ȃ�\n");

	if (id == GFL_OVERLAY_BLANK_ID) {
		OS_TPrintf("�s�v��overlay unload �Ăяo��\n");
		return;
	}

	memory_area = GetMemoryAreaID(id);
	ovwork = GetWorkPointer(memory_area);
	size = GetWorkSize(memory_area);
	
	for(i = 0; i < size; i++){
		if(ovwork[i].loaded == TRUE && ovwork[i].loaded_id == id){
			//���ݓǂݍ��܂�Ă���I�[�o�[���C���W���[�����A�����[�h���܂�
			Overlay_Unload(&ovwork[i]);
			return;
		}
	}
	OS_TPrintf("�G���[�F�ǂݍ���ł��Ȃ�OverlayID�ւ�Unload�v��(%d)\n",ovwork[i].loaded_id);
	GF_ASSERT(0);
}

//--------------------------------------------------------------
/**
 * @brief   �w�肵���I�[�o�[���C�����[�h����
 *
 * @param   id				���[�h����I�[�o�[���CID
 *
 * @retval  TRUE:����
 * @retval  FALSE:���s
 *
 * �����̃I�[�o�[���C�����s���Ă��鎞�A�̈悪������ꍇ�̓I�[�o�[���C�͎��s���܂��B
 * (�f�o�b�O����GF_ASSERT�Œ�~���܂�)
 *
 * ���[�h��FS_SetDefaultDMA(or FS_Init)�Őݒ肳��Ă�����@�ōs���܂��B
 * �g�p����DMA,CPU��ύX�������ꍇ�́A���̊֐����s�O��FS_SetDefaultDMA�Őݒ���s���Ă��������B
 * ITCM,DTCM�ւ̃��[�h�͓����ňꎞ�I��CPU�ɕύX���ă��[�h���s���܂��B(���[�h��͌��̐ݒ�ɖ߂��܂�)
 */
//--------------------------------------------------------------
BOOL GFL_OVERLAY_Load(const FSOverlayID id)
{
	BOOL ret;
	int memory_area;
	u32 dma_bak = FS_DMA_NOT_USE;
	OVERLAY_WORK *ovwork;
	int size;
	int i;
	
	GF_ASSERT_MSG(OverlaySys != NULL, "Overlay System������������Ă��Ȃ�\n");

	if (id == GFL_OVERLAY_BLANK_ID) {
		OS_TPrintf("�s�v��overlay load �Ăяo��\n");
		return TRUE;
	}
	if(CheckCrossArea(id) == FALSE){
		return FALSE;
	}

	//�󂫃��[�N�T�[�`
	memory_area = GetMemoryAreaID(id);
	ovwork = GetWorkPointer(memory_area);
	size = GetWorkSize(memory_area);
	for(i = 0; i < size; i++){
		if(ovwork[i].loaded == FALSE){
			ovwork = &ovwork[i];
			ovwork->loaded = TRUE;
			ovwork->loaded_id = id;
			break;
		}
	}
	if(i >= size){
		GF_ASSERT(0);	//���������G���A���ɃI�[�o�[���C�o���鐔�𒴂��Ă���
		return FALSE;
	}
	
	if(memory_area == GF_OVERLAY_AREA_ITCM || memory_area == GF_OVERLAY_AREA_DTCM){
		//TCM�ւ̏������݂�DMA�ł͏o���Ȃ��̂ňꎞ�I��CPU�֕ύX
		dma_bak = FS_SetDefaultDMA(FS_DMA_NOT_USE);
	}
	
	ret = FS_LoadOverlay(MI_PROCESSOR_ARM9, id);

	if(memory_area == GF_OVERLAY_AREA_ITCM || memory_area == GF_OVERLAY_AREA_DTCM){
		//�ꎞ�I�ɕύX�����̂����ɖ߂�
		FS_SetDefaultDMA(dma_bak);
	}

	if(ret == FALSE){	//���[�h���s
		OS_Printf("overlay 0x%08X is not available!\n", id);
		GF_ASSERT(0);
		return FALSE;
	}

	OS_Printf("overlay load id = 0x%08X\n", id);
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �w�肵���I�[�o�[���CID�̗̈悪���ݓW�J����Ă���I�[�o�[���C�̈�ƃA�h���X��
 *          ����Ă��Ȃ����`�F�b�N����
 *
 * @param   id		�I�[�o�[���CID
 *
 * @retval  TRUE:����Ă��Ȃ��B�@FALSE:����Ă���(�G���[�̏ꍇ���L)
 */
//--------------------------------------------------------------
static BOOL CheckCrossArea(const FSOverlayID id)
{
	GF_OVERLAY_AREA memory_area;
	u32 start, end, c_start, c_end;
	BOOL ret;
	OVERLAY_WORK *ovwork;
	int size;
	int i;
	
	ret = GetOverlayAddress(id, &start, &end);
	if(ret == FALSE){
		return FALSE;
	}
	
	memory_area = GetMemoryAreaID(id);
	ovwork = GetWorkPointer(memory_area);
	size = GetWorkSize(memory_area);
	for(i = 0; i < size; i++){
		if(ovwork[i].loaded == TRUE){
			ret = GetOverlayAddress(ovwork[i].loaded_id, &c_start, &c_end);
			if(ret == TRUE){
				if((start >= c_start && start < c_end) || (end > c_start && end <= c_end)
						|| (start <= c_start && end >= c_end)){
					OS_Printf("�I�[�o�[���C�̈�ɏd�����������܂����I\n");
					OS_Printf("�W�J�ς�ID=%d�A�V�K�ɓW�J���悤�Ƃ���ID=%d\n", 
						ovwork[i].loaded_id, id);
					GF_ASSERT(0);
					return FALSE;
				}
			}
		}
	}
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �w�胁�����G���A���Q�Ƃ���I�[�o�[���C���[�N�̐擪�A�h���X���擾����
 * @param   memory_area		�������G���A(GF_OVERLAY_AREA_???)
 * @retval  �w�胁�����ɑΉ������I�[�o�[���C���[�N�̐擪�A�h���X
 */
//--------------------------------------------------------------
static OVERLAY_WORK * GetWorkPointer(GF_OVERLAY_AREA memory_area)
{
	OVERLAY_WORK *ovwork;
	
	switch(memory_area){
	default:
		GF_ASSERT_MSG(0, "error:overlay:����`�̃������G���A�w��(%d)\n", memory_area);
		/* FALL THROUGH */
	case GF_OVERLAY_AREA_MAIN:
		ovwork = OverlaySys->main;
		break;
	case GF_OVERLAY_AREA_ITCM:
		ovwork = OverlaySys->itcm;
		break;
	case GF_OVERLAY_AREA_DTCM:
		ovwork = OverlaySys->dtcm;
		break;
	}
	return ovwork;
}

//--------------------------------------------------------------
/**
 * @brief	�w�胁�����G���A���Q�Ƃ���I�[�o�[���C���[�N�̐����擾����
 * @param   memory_area		�������G���A(GF_OVERLAY_AREA_???)
 * retval	int	�ێ����Ă��郏�[�N�̐�
 */
//--------------------------------------------------------------
static int GetWorkSize(GF_OVERLAY_AREA memory_area)
{
	switch (memory_area){
	case GF_OVERLAY_AREA_MAIN:
		return OverlaySys->main_num;
	case GF_OVERLAY_AREA_ITCM:
		return OverlaySys->itcm_num;
	case GF_OVERLAY_AREA_DTCM:
		return OverlaySys->dtcm_num;
	}
	GF_ASSERT_MSG(0, "error:overlay:����`�̃������G���A�w��(%d)\n", memory_area);
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   �w��I�[�o�[���CID�̓W�J�惁�����G���A���擾����
 * @param   id		�I�[�o�[���CID
 * @retval  �W�J�惁�����G���A(GF_OVERLAY_AREA_???)
 */
//--------------------------------------------------------------
static GF_OVERLAY_AREA GetMemoryAreaID(const FSOverlayID id)
{
	FSOverlayInfo info;
	BOOL ret;
	u32 ram_address;
	
	ret = FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, id);
	GF_ASSERT(ret == TRUE);
	
	ram_address = (u32)FS_GetOverlayAddress(&info);
	if(ram_address <= HW_ITCM_END && ram_address >= HW_ITCM_IMAGE){
		return GF_OVERLAY_AREA_ITCM;
	}
	else if(ram_address <= HW_DTCM_END && ram_address >= HW_DTCM){
		return GF_OVERLAY_AREA_DTCM;
	}
	return GF_OVERLAY_AREA_MAIN;
}

//--------------------------------------------------------------
/**
 * @brief   �w��I�[�o�[���CID�̃��[�h�J�n�A�h���X�ƃ��[�h�I���A�h���X���擾����
 *
 * @param   id			�I�[�o�[���CID
 * @param   start		�擾�������[�h�J�n�A�h���X�����
 * @param   end			�擾�������[�h�I���A�h���X�����
 *
 * @retval  TRUE:����I���B�@FALSE:���s
 */
//--------------------------------------------------------------
static BOOL GetOverlayAddress(const FSOverlayID id, u32 *start, u32 *end)
{
	FSOverlayInfo info;
	BOOL ret;
	
	ret = FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, id);
	if(ret == FALSE){
		GF_ASSERT(0);
		return FALSE;
	}
	
	*start = (u32)FS_GetOverlayAddress(&info);
	*end = *start + FS_GetOverlayTotalSize(&info);
	
	return TRUE;
}

