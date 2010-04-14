//=============================================================================
/**
 * @file	backup_system.h
 * @brief	�o�b�N�A�b�v�V�X�e���E�Z�[�u�f�[�^�p�w�b�_
 * @author	tamada	GAME FREAK Inc.
 * @date	2007.06.14
 *
 *
 * ���̃w�b�_�̓Z�[�u�f�[�^�̐��������A�o�b�N�A�b�v�̈�ւ̃A�N�Z�X�Ȃǂ�
 * ���ڃZ�[�u�f�[�^�ɂ�����镔���ɂ̂ݕK�v�B
 *
 * �ʏ��SAVEDATA�^�isavedata_def.h�Œ�`�j���o�R���Ċe�f�[�^�ɃA�N�Z�X�ł���΂悢���߁A
 * ���̃w�b�_���C���N���[�h����ӏ��͌��肳���B
 */
//=============================================================================
#pragma	once

#include "savedata_def.h"
#include <heap.h>

#ifdef __cplusplus
extern "C" {
#endif

//============================================================================================
//
//			�Z�[�u�f�[�^�\�z�p��`
//
//============================================================================================
///---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef u32 (*FUNC_GET_SIZE)(void);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef void (*FUNC_INIT_WORK)(void *);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef u32 GFL_SVDT_ID;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//�Z�[�u���s�G���[�̎w��
typedef enum {
	GFL_SAVEERROR_DISABLE_WRITE = 0,		///<�������ݎ��s�̎�
	GFL_SAVEERROR_DISABLE_READ,				///<�������ݑO�̃��[�h�`�F�b�N���s�̂Ƃ�
}GFL_SAVEERROR_DISABLE;

//�t���b�V���G���[(���[�h��)�����������ꍇ�ɌĂ΂��R�[���o�b�N�֐��^
typedef void (*GFL_BACKUP_LOAD_ERROR_FUNC)( void );
//�t���b�V���G���[(�Z�[�u��)�����������ꍇ�ɌĂ΂��R�[���o�b�N�֐��^
typedef void (*GFL_BACKUP_SAVE_ERROR_FUNC)( GFL_SAVEERROR_DISABLE err_disable );


//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���e�f�[�^�p�̍\���̒�`
 */
//---------------------------------------------------------------------------
typedef struct {
	GFL_SVDT_ID		gmdataID;		///<�Z�[�u�f�[�^����ID
	FUNC_GET_SIZE	get_size;		///<�Z�[�u�f�[�^�T�C�Y�擾�֐�
	FUNC_INIT_WORK	init_work;		///<�Z�[�u�f�[�^�������֐�
}GFL_SAVEDATA_TABLE;

//---------------------------------------------------------------------------
///	�Z�[�u�f�[�^��`�\����
//---------------------------------------------------------------------------
typedef struct {
	const GFL_SAVEDATA_TABLE * table;	///<�Z�[�u�f�[�^��`�e�[�u���̃A�h���X
	u32 table_max;						      ///<�Z�[�u�f�[�^��`�e�[�u���̗v�f��
	u32 savearea_top_address;		  	///<�g�p����o�b�N�A�b�v�̈�̐擪�A�h���X
	u32 savearea_mirror_address;		///<�g�p����~���[�����O�̈�̐擪�A�h���X
	                                ///<�~���[�����O�����̏ꍇ��savearea_top_address��[�����l]���w��
	u32 savearea_size;					///<�g�p����o�b�N�A�b�v�̈�̑傫��
	u32 magic_number;					///<�g�p����}�W�b�N�i���o�[
}GFL_SVLD_PARAM;



//============================================================================================
//
//				�o�b�N�A�b�v����֐�
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	�o�b�N�A�b�v�V�X�e���N��
 * @param	heap_save_id
 *
 */
//---------------------------------------------------------------------------
extern void GFL_BACKUP_Init(u32 heap_save_id, GFL_BACKUP_LOAD_ERROR_FUNC load_error_func, GFL_BACKUP_SAVE_ERROR_FUNC save_error_func);

//---------------------------------------------------------------------------
/**
 * @brief	�o�b�N�A�b�v�V�X�e���I��
 */
//---------------------------------------------------------------------------
extern void GFL_BACKUP_Exit(void);
//---------------------------------------------------------------------------
/**
 * @brief	�o�b�N�A�b�v�t���b�V�����݃`�F�b�N
 * @retval	TRUE	�t���b�V�������݂���
 * @retval	FALSE	�t���b�V�������݂��Ȃ�
 */
//---------------------------------------------------------------------------
extern BOOL GFL_BACKUP_IsEnableFlash(void);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�F���[�h����
 * @param	sv
 * @param heap_temp_id    ���̊֐����ł̂݃e���|�����Ƃ��Ďg�p����q�[�vID
 * @return	LOAD_RESULT
 */
//---------------------------------------------------------------------------
extern LOAD_RESULT GFL_BACKUP_Load(GFL_SAVEDATA * sv, u32 heap_temp_id);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�F�Z�[�u����
 * @param	sv
 * @return	SAVE_RESULT
 */
//---------------------------------------------------------------------------
extern SAVE_RESULT GFL_BACKUP_Save(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�F��������
 * @param	sv			        �Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param heap_temp_id    ���̊֐����ł̂݃e���|�����Ƃ��Ďg�p����q�[�vID
 */
//---------------------------------------------------------------------------
extern BOOL GFL_BACKUP_Erase(GFL_SAVEDATA * sv, u32 heap_temp_id);

//==================================================================
/**
 * �t���b�V�����H��o�׏�Ԃ̒l�ŃN���A����
 *
 * @param   heap_id		    �e���|�����q�[�v
 * @param	  start_address	�����J�n�ʒu(�t���b�V���̃A�h���X�i�O�`�j���Z�N�^�w��ł͂Ȃ�)
 * @param   erase_size		��������T�C�Y(byte)
 */
//==================================================================
extern void GFL_BACKUP_FlashErase(HEAPID heap_id, u32 start_address, u32 erase_size);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�F�����Z�[�u�F������
 */
//---------------------------------------------------------------------------
extern void GFL_BACKUP_SAVEASYNC_Init(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�F�����Z�[�u�F���C��
 */
//---------------------------------------------------------------------------
extern SAVE_RESULT GFL_BACKUP_SAVEASYNC_Main(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�F�����Z�[�u�F�L�����Z��
 */
//---------------------------------------------------------------------------
extern void GFL_BACKUP_SAVEASYNC_Cancel(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	�O���Z�[�u����(�O���Ɠ�����Жʂ����݂ɃZ�[�u���s��)
 * @param	sv			      �����Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	extra_sv			�O���Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param link_ptr      �����N���Ƃ��ĊO���Z�[�u�̃O���[�o���J�E���^��
 *                      �Жʂ̊O���Z�[�u�������������_�ł����̃|�C���^�̉ӏ���
 *                      �V�X�e��������Z�b�g���܂��B
 *                      �����Z�[�u�̔C�ӂ̏ꏊ�Ƀ����N���ۑ��p�̃G���A��p�ӂ�
 *                      ���̃|�C���^��n���Ă�������
 *                      �����[�h���ɃV�X�e�����Ń����N���̃`�F�b�N�͂��Ȃ��ׁA
 *                        �Q�[�����Ń����N��񂪈�v���Ă��邩�`�F�b�N���Ă�������
 *                          GFL_BACKUP_GetGlobalCounter( extra_sv ) == *link_ptr
 *
 *                      ���|�P�����̂悤�ɃZ�[�u�������ԂŁu�������傩��v���ł���ƁA
 *                        �O���[�o���J�E���^����v���Ă��܂��ꍇ������܂��B
 *                        �����̃Z�[�u�f�[�^�ƌ�F����Ȃ��悤�ɂ��邽�߁A
 *                        �Q�[�����Łu�O���Z�[�u���������Ƃ�����v
 *                        �̂悤�ȃt���O�Ȃ�}�W�b�N�L�[�Ȃ�������A���[�h�������̂��̂�
 *                        ���Ȃ��悤�ɉ�����Ă��������B
 * 
 * @retval	TRUE		�������ݐ���
 * @retval	FALSE		�������ݎ��s
 *
 * �O���Ɠ����̃����N���͂��炩���ߊO���Ō���ł�����
 * �Z�[�u�V�X�e���̓����N�̊֘A�t���܂ł͂��܂���
 *
 * Cancel�͋֎~�ł�
 *
 * PL,GS�܂ł̊O���Z�[�u���ʂ�������A�����Z�[�u�A�Ƒ�������
 * �O���Z�[�u�Ƃ̃����N��񂪐؂�Ȃ��Ȃ�ׁA�O���Z�[�u�������Ɛ����c��
 */
//---------------------------------------------------------------------------
extern void GFL_BACKUP_SAVEASYNC_EXTRA_Init(GFL_SAVEDATA * sv, GFL_SAVEDATA * extra_sv, u32 *link_ptr);

//---------------------------------------------------------------------------
/**
 * @brief	�O���Z�[�u�F�����Z�[�u���C������(�O���Ɠ�����Жʂ����݂ɃZ�[�u���s��)
 * @param	sv			      �����Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	extra_sv			�O���Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	SAVE_RESULT
 */
//---------------------------------------------------------------------------
extern SAVE_RESULT GFL_BACKUP_SAVEASYNC_EXTRA_Main(GFL_SAVEDATA * sv, GFL_SAVEDATA * extra_sv);

//---------------------------------------------------------------------------
/**
 * @brief	�O���Z�[�u�̃��[�h����
 * @param	extra_sv			�O���Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param heap_temp_id    ���̊֐����ł̂݃e���|�����Ƃ��Ďg�p����q�[�vID
 * @param link          �����Z�[�u�Ƃ̃����N���
 * @retval	TRUE		�ǂݍ��ݐ���
 * @retval	FALSE		�ǂݍ��ݎ��s
 *
 * ���[�h�����̓t���b�V���A�N�Z�X���Ԃ��\���������߁A�����֐��͗p�ӂ��Ă��Ȃ�
 */
//---------------------------------------------------------------------------
extern LOAD_RESULT GFL_BACKUP_Extra_Load(GFL_SAVEDATA * sv, u32 heap_temp_id, u32 link);

//==================================================================
/**
 * GFL_BACKUP_SAVEASYNC_Main�ŉ��o�C�g�ڂ܂ł��������񂾂��𒲂ׂ�
 *
 * @param   sv		
 *
 * @retval  u32		�������݃T�C�Y(Save_GetActualSize��actual_size * 2(���ʃT�C�Y))
 */
//==================================================================
extern u32 GFL_BACKUP_SAVEASYNC_Main_WritingSize(GFL_SAVEDATA * sv);

//==================================================================
/**
 * �����Z�[�u���A�J�[�h�����b�N����Ă��邩���ׂ�
 *
 * @param   none		
 *
 * @retval  BOOL		TRUE:�J�[�h�����b�N����Ă���
 * @retval  BOOL		FALSE:���b�N����Ă��Ȃ�
 */
//==================================================================
extern BOOL GFL_BACKUP_SAVEASYNC_CheckCardLock(void);


//============================================================================================
//
//				�Z�[�u�f�[�^�A�N�Z�X�p�֐�
//
//============================================================================================
//==================================================================
/**
 * �Z�[�u�f�[�^�V�X�e���쐬(�Z�[�u���[�N�̎��̂�n���ꂽ�q�[�v�Ŋm��)
 *
 * @param   sv_param		
 * @param   heap_save_id		�Z�[�u�V�X�e���ƃZ�[�u���[�N���̗̂����Ŏg�p
 *
 * @retval  GFL_SAVEDATA *		
 */
//==================================================================
extern GFL_SAVEDATA * GFL_SAVEDATA_Create(const GFL_SVLD_PARAM * sv_param, u32 heap_save_id);

//==================================================================
/**
 * �Z�[�u�f�[�^�V�X�e���쐬(�Z�[�u���[�N�̎��̂͊O���œn���Ă��炤�@���O���Z�[�u�ł̎g�p��z��
 *
 * @param   sv_param		
 * @param   heap_save_id		�Z�[�u�V�X�e���Ŏg�p
 * @param   svwk_adrs       �Z�[�u���[�N���̂Ƃ��Ďg�p���郏�[�N�̃|�C���^
 * @param   svwk_size       svwk_adrs�̃T�C�Y
 * @param   svwk_clear      svwk_adrs��NULL�łȂ��ꍇ�A
 *                            TRUE:�o�b�t�@�̏��������s��
 *                            FALSE:�o�b�t�@�̏������͍s��Ȃ�
 *
 * @retval  GFL_SAVEDATA *		
 */
//==================================================================
extern GFL_SAVEDATA * GFL_SAVEDATA_CreateEx(const GFL_SVLD_PARAM * sv_param, 
  u32 heap_save_id, void *svwk_adrs, u32 svwk_size, BOOL svwk_clear );

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�������
 * @param	sv		�Z�[�u�f�[�^�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
extern void GFL_SAVEDATA_Delete(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^������
 * @param	sv		�Z�[�u�f�[�^�ւ̃|�C���^
 *
 * �o�b�N�A�b�v�ւ̏������݂͍s�킸�A�f�[�^�̏����������݂̂��s��
 */
//---------------------------------------------------------------------------
extern void GFL_SAVEDATA_Clear(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	�ŏ��̓ǂݍ��ݎ��̃Z�[�u�f�[�^��Ԏ擾
 * @param	sv		�Z�[�u�f�[�^�ւ̃|�C���^
 * @return	LOAD_RESULT		���[�h���ʏ��
 */
//---------------------------------------------------------------------------
extern LOAD_RESULT GFL_SAVEDATA_GetLoadResult(const GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	�f�[�^�����݂��邩�ǂ����̔��ʃt���O�擾
 * @param	sv		�Z�[�u�f�[�^�ւ̃|�C���^
 * @retval	TRUE	���݂���
 * @retval	FALSE	���݂��Ȃ�
 */
//---------------------------------------------------------------------------
extern BOOL GFL_SAVEDATA_GetExistFlag(const GFL_SAVEDATA * sv);

//==================================================================
/**
 * �Z�[�u�f�[�^���[�N�̐擪�A�h���X���擾
 * @param   sv		�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param   size  ���Z�[�u�f�[�^�T�C�Y
 * @retval  const void *		�Z�[�u�f�[�^���[�N�̐擪�A�h���X
 */
//==================================================================
extern const void * GFL_SAVEDATA_GetSaveWorkAdrs(GFL_SAVEDATA * sv, u32 *size);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̊e�v�f�擾
 * @param	gmdataID	�f�[�^�h�c
 * @return	void * �Ƃ��Ċe�v�f�ւ̃|�C���^��Ԃ�
 */
//---------------------------------------------------------------------------
extern void * GFL_SAVEDATA_Get(GFL_SAVEDATA * sv, GFL_SVDT_ID gmdataID);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̊e�v�f�擾
 * @param	gmdataID	�f�[�^�h�c
 * @return	void * �Ƃ��Ċe�v�f�ւ̃|�C���^��Ԃ�
 *
 * �ǂݍ��ݐ�p�Ƃ��Ĉ��������ꍇ��GFL_SAVEDATA_Get�Ɛ؂蕪���Ďg�p����
 */
//---------------------------------------------------------------------------
extern const void * GFL_SAVEDATA_GetReadOnly(const GFL_SAVEDATA * sv, GFL_SVDT_ID gmdataID);

//--------------------------------------------------------------
/**
 * �Z�[�u�����s�����ꍇ�̃Z�[�u�T�C�Y���擾����
 *
 * @param   sv		        �Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param   actual_size		�Z�[�u�������T�C�Y(CRC�e�[�u���Ȃǂ̃V�X�e���n�̃f�[�^�͏����܂�)
 * @param   total_size		�Z�[�u�S�̂̃T�C�Y(CRC�e�[�u���Ȃǂ̃V�X�e���n�̃f�[�^�͏����܂�)
 *
 * ������r�ׁ̈A�t���b�V���A�N�Z�X���܂��B
 */
//--------------------------------------------------------------
extern void GFL_SAVEDATA_GetActualSize(GFL_SAVEDATA *sv, u32 *actual_size, u32 *total_size);

//--------------------------------------------------------------
/**
 * @brief   �u���b�N��CRC�����݂̃f�[�^�ōX�V����
 *
 * @param   sv				�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param   gmdataid		�Z�[�u�f�[�^ID
 *
 * @retval  �������ꂽCRC�l
 */
//--------------------------------------------------------------
extern u16 GFL_BACKUP_BlockCRC_Set(GFL_SAVEDATA *sv, GFL_SVDT_ID gmdataid);

//==================================================================
/**
 * �t���b�V�����ڏ������݁F�ꊇ��������
 *
 * @param	flash_address		�t���b�V���̃A�h���X�i�O�`�j���Z�N�^�w��ł͂Ȃ�
 * @param	src         		�������ރf�[�^�̃A�h���X
 * @param	len		          �������ރf�[�^�̒���
 * @return	BOOL	TRUE�Ő����AFALSE�Ŏ��s
 */
//==================================================================
extern BOOL GFL_BACKUP_DirectFlashSave(u32 flash_address, const void * src, u32 len);

//==================================================================
/**
 * �t���b�V�����ڏ������݁F�����������݁F������
 *
 * @param	flash_address		�t���b�V���̃A�h���X�i�O�`�j���Z�N�^�w��ł͂Ȃ�
 * @param	src         		�������ރf�[�^�̃A�h���X
 * @param	len		          �������ރf�[�^�̒���
 * @return	u16 lock_id
 */
//==================================================================
extern u16 GFL_BACKUP_DirectFlashSaveAsyncInit(u32 flash_address, const void * src, u32 len);

//==================================================================
/**
 * �t���b�V�����ڏ������݁F�����������݁F���C��
 *
 * @param   lock_id		    
 * @param   err_result		����������@TRUE:����ɏ�������ŏI���@FALSE:�G���[�ŏI��
 *
 * @retval  BOOL		      TRUE:���������@FALSE:�����p����
 *
 * �߂�l��TRUE���Ԃ��Ă����̂��m�F���Ă���Aerr_result�ŃG���[��������Ă�������
 */
//==================================================================
extern BOOL GFL_BACKUP_DirectFlashSaveAsyncMain(u16 lock_id, BOOL *err_result);

//==================================================================
/**
 * �t���b�V�����ړǂݏo��
 *
 * @param	src		�t���b�V���̃A�h���X�i�O�`�j���Z�N�^�w��ł͂Ȃ�
 * @param	dst		�ǂݍ��ݐ�A�h���X
 * @param	len		�ǂݍ��ރf�[�^�̒���
 * @return	BOOL	TRUE�Ő����AFALSE�Ŏ��s
 */
//==================================================================
extern BOOL GFL_BACKUP_DirectFlashLoad(u32 src, void * dst, u32 len);

//==================================================================
/**
 * �w��ID�̕����������t���b�V�����璼�ڃ��[�h����
 *
 * @param   sv            
 * @param   gmdataid		  ���[�h����Ώۂ�ID
 * @param   side_a_or_b		0:A�ʂ��烍�[�h�@�@1:B�ʂ��烍�[�h
 * @param   dst		        �f�[�^�W�J��
 * @param   load_size     ���[�h����o�C�g�T�C�Y
 *
 * @retval  BOOL		TRUE:���[�h�����@FALSE:���[�h���s
 */
//==================================================================
extern BOOL GFL_BACKUP_Page_FlashLoad(GFL_SAVEDATA *sv, GFL_SVDT_ID gmdataid, BOOL side_a_or_b, void *dst, u32 load_size);




//==============================================================================
//	�f�o�b�O�p�֐�
//==============================================================================
#ifdef PM_DEBUG
extern void DEBUG_GFL_BACKUP_BlockSaveFlagGet(const GFL_SAVEDATA *sv, GFL_SVDT_ID gmdataid, BOOL crctable, BOOL footer, BOOL *ret_a, BOOL *ret_b);
extern void DEBUG_BACKUP_DataWrite(GFL_SAVEDATA *sv, GFL_SVDT_ID gmdataid, void *data, int write_offset, int size, int save_a, int save_b, int save_crc, int save_footer);
extern BOOL DEBUG_BACKUP_FlashSave(u32 flash_address, const void * src, u32 len);
extern BOOL DEBUG_BACKUP_FlashLoad(u32 src, void * dst, u32 len);
#endif


#ifdef __cplusplus
}/* extern "C" */
#endif


