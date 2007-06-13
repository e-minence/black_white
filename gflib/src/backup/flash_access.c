//============================================================================================
/**
 * @file	flash_access.c
 * @brief
 * @author	tamada GAME FREAK inc.
 * @date	2007.06.12
 */
//============================================================================================

#include <nitro.h>
#include <nnsys.h>

#include "assert.h"

#include "flash_access.h"

typedef	u16	CARD_ID;
//============================================================================================
//
//
//			���C�u����I/O����
//
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	�o�b�N�A�b�v�t���b�V���̓���m�F
 * @retval	TRUE	�o�b�N�A�b�v�t���b�V��������ɓ��삷��
 * @retval	FALSE	���퓮�삵�Ȃ��i���݂��Ȃ��A�Ⴄ��ނ̃o�b�N�A�b�v�f�o�C�X���j
 */
//---------------------------------------------------------------------------
BOOL GFL_FLASH_Init(void)
{
	s32 lock_id;
	BOOL result;
	//u32 read_buf;

	lock_id = OS_GetLockID();
	GF_ASSERT(lock_id != OS_LOCK_ID_ERROR);
	CARD_LockBackup((CARD_ID)lock_id);

	if (CARD_IdentifyBackup(CARD_BACKUP_TYPE_FLASH_4MBITS)) {
		result = CARD_BACKUP_TYPE_FLASH_4MBITS;
	} else
	if (CARD_IdentifyBackup(CARD_BACKUP_TYPE_FLASH_2MBITS)) {
		result = CARD_BACKUP_TYPE_FLASH_2MBITS;
	} else {
		result = CARD_BACKUP_TYPE_NOT_USE;
	}

	CARD_UnlockBackup((CARD_ID)lock_id);
	OS_ReleaseLockID((CARD_ID)lock_id);

	if (result == CARD_BACKUP_TYPE_FLASH_4MBITS) {
		OS_TPrintf("Identified 4M FLASH\t");
	}else if (result == CARD_BACKUP_TYPE_FLASH_2MBITS) {
		OS_TPrintf("Identified 2M FLASH\t");
	}else{
		OS_TPrintf("No FLASH Identified\t");
	}

	OS_TPrintf("CardResult %d\n",CARD_GetResultCode());
	//4Mbit FLASH SectorSize == 0x10000 PageSize == 0x100 TotalSize == 0x80000
	//OS_TPrintf("SECTOR SIZE %x\n",CARD_GetBackupSectorSize());
	//OS_TPrintf("PAGE SIZE %x\n",CARD_GetBackupPageSize());
	//OS_TPrintf("TOTAL SIZE %x\n", CARD_GetBackupTotalSize());

	return (result != CARD_BACKUP_TYPE_NOT_USE);
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u
 * @param	src		�t���b�V���̃A�h���X�i�O�`�j���Z�N�^�w��ł͂Ȃ�
 * @param	dst		�������ރf�[�^�̃A�h���X
 * @param	len		�������ރf�[�^�̒���
 * @return	BOOL	TRUE�Ő����AFALSE�Ŏ��s
 */
//---------------------------------------------------------------------------
BOOL GFL_FLASH_Save(u32 src, void * dst, u32 len)
{
	u16 lock_id;
	BOOL result;
	lock_id = GFL_FLASH_SAVEASYNC_Init(src, dst, len);
	while (GFL_FLASH_SAVEASYNC_Main(lock_id, &result) == FALSE) {
		/* Do Nothing */
	}
	return result;
#if 0
	s32 lock_id;
	BOOL result;

	lock_id = OS_GetLockID();
	GF_ASSERT(lock_id != OS_LOCK_ID_ERROR);
	CARD_LockBackup((CARD_ID)lock_id);

//	result = CARD_WriteAndVerifyFlash(src, dst, len);
	CARD_WriteAndVerifyFlashAsync(src, dst, len, NULL, NULL);
	result = CARD_WaitBackupAsync();

	CARD_UnlockBackup((CARD_ID)lock_id);
	OS_ReleaseLockID((CARD_ID)lock_id);

	return result;
#endif
}
//---------------------------------------------------------------------------
/**
 * @brief	���[�h
 * @param	src		�t���b�V���̃A�h���X�i�O�`�j���Z�N�^�w��ł͂Ȃ�
 * @param	dst		�ǂݍ��ݐ�A�h���X
 * @param	len		�ǂݍ��ރf�[�^�̒���
 * @return	BOOL	TRUE�Ő����AFALSE�Ŏ��s
 */
//---------------------------------------------------------------------------
BOOL GFL_FLASH_Load(u32 src, void * dst, u32 len)
{
	s32 lock_id;
	BOOL result;

	lock_id = OS_GetLockID();
	GF_ASSERT(lock_id != OS_LOCK_ID_ERROR);
	CARD_LockBackup((CARD_ID)lock_id);

	//result = CARD_ReadFlash(src, dst, len);
	CARD_ReadFlashAsync(src, dst, len, NULL, NULL);
	result = CARD_WaitBackupAsync();

	CARD_UnlockBackup((CARD_ID)lock_id);
	OS_ReleaseLockID((CARD_ID)lock_id);

#ifndef	DISABLE_FLASH_CHECK		//�o�b�N�A�b�v�t���b�V���Ȃ��ł�����
	if (!result) {
		//GFL_HEAP_FreeMemory(SvPointer);
		//GF_ASSERT(0);
		//BackupErrorWarningCall(HEAPID_BASE_SAVE);
	}
#endif

	return result;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static BOOL SaveEndFlag;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void SaveCallBack(void * src)
{
	SaveEndFlag = TRUE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
u16 GFL_FLASH_SAVEASYNC_Init(u32 src, void * dst, u32 len)
{
	s32 lock_id;
	u32 buf;
	BOOL result;

	lock_id = OS_GetLockID();
	GF_ASSERT(lock_id != OS_LOCK_ID_ERROR);
	CARD_LockBackup((CARD_ID)lock_id);

#ifndef	DISABLE_FLASH_CHECK
	result = CARD_ReadFlash(0, &buf, sizeof(buf));
	if (!result) {
		/* �Z�[�u�f�[�^���ǂ߂Ȃ����ڐG�s�� */
		//PMSVLD_SaveError(lock_id, SAVEERROR_MSG_DISABLE_READ);
	}
#endif
	SaveEndFlag = FALSE;
	CARD_WriteAndVerifyFlashAsync(src, dst, len, SaveCallBack, NULL);
	return (CARD_ID)lock_id;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static BOOL GFL_FLASH_SAVEASYNC_Main(u16 lock_id, BOOL * result)
{
	if (SaveEndFlag == TRUE) {
		CARD_UnlockBackup(lock_id);
		OS_ReleaseLockID(lock_id);

#ifdef	DISABLE_FLASH_CHECK
		*result = TRUE;
#else
		switch (CARD_GetResultCode()) {
		case CARD_RESULT_SUCCESS:
			*result = TRUE;
			break;
		default:
			GF_ASSERT(0);
		case CARD_RESULT_TIMEOUT:
			*result = FALSE;
			/* �Z�[�u�f�[�^���������߂Ȃ����t���b�V���������̏� */
			//PMSVLD_SaveError(lock_id, SAVEERROR_MSG_DISABLE_WRITE);
		case CARD_RESULT_NO_RESPONSE:
			*result = FALSE;
			/* CARD_RESULT_NO_RESPONSE�����ڐG�s�� */
			//PMSVLD_SaveError(lock_id, SAVEERROR_MSG_DISABLE_READ);
			break;
		}
#endif
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------
/**
 * @brief
 * @param	lock_id
 *
 */
//---------------------------------------------------------------------------
void GFL_FLASH_SAVEASYNC_Cancel(u16 lock_id)
{
    if(!CARD_TryWaitBackupAsync()){
        CARD_CancelBackupAsync();		//�񓯊������L�����Z���̃��N�G�X�g

        CARD_UnlockBackup(lock_id);
        OS_ReleaseLockID(lock_id);
    }
}


//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u���s�ɂ��J�[�h�ُ�
 * @param	lock_id		�o�X���b�N�ŃV�X�e�����Ԃ���ID
 */
//---------------------------------------------------------------------------
static void PMSVLD_SaveError(u16 lock_id, int error_msg_id)
{
	//���b�N���Ă���o�X���J������i�������ǂ߂Ȃ��Ȃ�̂Łj
	CARD_UnlockBackup(lock_id);
	OS_ReleaseLockID(lock_id);

	//�Z�[�u���s��ʂŃZ�[�u�q�[�v���g�p�ł���悤�ɊJ������
	//GFL_HEAP_FreeMemory(SvPointer);

	//�Z�[�u���s��ʌĂяo��
	GF_ASSERT(0);
	//SaveErrorWarningCall(HEAPID_BASE_SAVE, error_msg_id);
}

