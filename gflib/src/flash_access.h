//============================================================================================
/**
 * @file	flash_access.h
 * @brief	�t���b�V���A�N�Z�X�p�֐�
 * @author	tamada GAME FREAK inc.
 * @date	2007.06.12
 *
 *
 */
//============================================================================================

#pragma	once

//---------------------------------------------------------------------------
//	�t���b�V���A�N�Z�X�p�֐�
//	���f�o�b�O�p�r�ȊO�ł�SaveData_�`���ĂԂ͂��Ȃ̂ŁA�ʏ�͒��ڎg��Ȃ��B
//---------------------------------------------------------------------------
extern BOOL GFL_FLASH_Init(void);
extern BOOL GFL_FLASH_Save(u32 src, void * dst, u32 len);
extern BOOL GFL_FLASH_Load(u32 src, void * dst, u32 len);
extern u16 GFL_FLASH_SAVEASYNC_Init(u32 src, void * dst, u32 len);
extern BOOL GFL_FLASH_SAVEASYNC_Main(u16 lock_id, BOOL * result);

