/**
 *	@file	mail.c
 *	@brief	���[���V�X�e��
 *	@author	Miyuki Iwasaw
 *	@date	06.02.07
 */

#include <gflib.h>
#include "poke_tool/poke_tool.h"
#include "savedata/mail_util.h"
#include "app/mailtool.h"
#include "item/itemsym.h"
#include "item/item.h"

#if 0
#include "system/procsys.h"
#include "system/snd_tool.h"
#include "system/fontproc.h"
#include "system/pm_str.h"
#include "system/buflen.h"
#include "system/palanm.h"
#include "system/savedata.h"
#include "itemtool/itemsym.h"
#include "itemtool/item.h"
#include "gflib/strbuf_family.h"

#include "savedata/config.h"
#include "savedata/record.h"
#include "application/mail/mail_param.h"
#include "application/mailtool.h"
#include "application/mail/mailview.h"
#include "application/pms_input.h"
#include "mail_snd_def.h"

/**
 *	@brief	���[����ʌĂяo�����[�N�쐬(Create)
 *
 *	@retval	NULL	�V�K�쐬�̈悪�Ȃ�
 *	@retval ���[�N�|�C���^	���[���쐬��ʌĂяo��
 */
MAIL_PARAM* MailSys_GetWorkCreate(SAVEDATA* savedata,
		MAILBLOCK_ID blockID,u8 poke_pos,u8 design,int heapID)
{
	MAIL_PARAM* wk;
	MAIL_BLOCK* pBlock;
	int id;
	
	//���[���Z�[�u�f�[�^�u���b�N�擾
	pBlock = SaveData_GetMailBlock(savedata); 

	//���W���[���Ăяo���p�̃��[�N���쐬
	wk = sys_AllocMemoryLo(heapID,sizeof(MAIL_PARAM));

	MI_CpuClear8(wk,sizeof(MAIL_PARAM));
	wk->designNo = design;
	wk->poke_pos = poke_pos;
	wk->pBlock = pBlock;

	//�Ăяo�����[�h�ݒ�
	wk->mode = MAIL_MODE_CREATE;
	wk->blockID = blockID;	//�f�[�^�u���b�NID�擾
	wk->dataID = 0;	//ID��0������
	wk->savedata = savedata;

	//�f�[�^�e���|�����쐬
	wk->pDat = MailData_CreateWork(heapID);
	MailData_Clear(wk->pDat);

	//�����f�[�^�i�[(�f�U�C��No�͖����ɂ��ČĂяo��)
	MailData_CreateFromSaveData(wk->pDat,MAIL_DESIGN_NULL,poke_pos,savedata);
	return wk;
}

/**
 *	@brief	���[����ʌĂяo�����[�N�쐬(View)
 *
 *	@retval	NULL	�V�K�쐬�̈悪�Ȃ�
 *	@retval ���[�N�|�C���^	���[���`���ʌĂяo��
 */
MAIL_PARAM* MailSys_GetWorkView(SAVEDATA* savedata,MAILBLOCK_ID blockID,u16 dataID,int heapID)
{
	MAIL_PARAM* wk;
	MAIL_BLOCK* pBlock;

	wk = sys_AllocMemoryLo(heapID,sizeof(MAIL_PARAM));
	MI_CpuClear8(wk,sizeof(MAIL_PARAM));

	//�Ăяo�����[�h�ݒ�
	wk->mode = MAIL_MODE_VIEW;
	wk->blockID = blockID;
	wk->dataID = dataID;
	wk->savedata = savedata;
	
	//���[���Z�[�u�f�[�^�u���b�N�擾
	pBlock = SaveData_GetMailBlock(savedata); 
	wk->pBlock = pBlock;
	
	//�Z�[�u�f�[�^�擾
	wk->pDat = MAIL_AllocMailData(pBlock,blockID,dataID,heapID);
	return wk;
}

/**
 *	@brief	���[����ʌĂяo�����[�N�쐬(View/POKEMON_PARAM)
 */
MAIL_PARAM* MailSys_GetWorkViewPoke(SAVEDATA* savedata,POKEMON_PARAM* poke,int heapID)
{
	MAIL_PARAM* wk;
	
	wk = sys_AllocMemoryLo(heapID,sizeof(MAIL_PARAM));
	MI_CpuClear8(wk,sizeof(MAIL_PARAM));
	
	//�Ăяo�����[�h�ݒ�
	wk->mode = MAIL_MODE_VIEW;
	wk->savedata = savedata;
	
	//���[�N���擾
	wk->pDat = MailData_CreateWork(heapID);
	//���[���f�[�^�擾
	PokeParaGet(poke,ID_PARA_mail_data,wk->pDat);
	return wk;
}

/**
 *	@brief	���[����ʌĂяo�����[�N�쐬(View/�󃁁[���v���r���[)
 */
MAIL_PARAM* MailSys_GetWorkViewPrev(SAVEDATA* savedata,u8 designNo,int heapID)
{
	MAIL_PARAM* wk;
	
	wk = sys_AllocMemoryLo(heapID,sizeof(MAIL_PARAM));
	MI_CpuClear8(wk,sizeof(MAIL_PARAM));
	
	//�Ăяo�����[�h�ݒ�
	wk->mode = MAIL_MODE_VIEW;
	wk->savedata = savedata;

	//���[�N���擾
	wk->pDat = MailData_CreateWork(heapID);

	//�f�U�C��No�̂ݐݒ�
	MailData_SetDesignNo(wk->pDat,designNo);	
	return wk;
}

/**
 *	@brief	���O�̃��W���[���Ăяo���Ńf�[�^���쐬���ꂽ���ǂ����H
 */
BOOL MailSys_IsDataCreate(MAIL_PARAM* wk)
{
	return wk->ret_val;
}

/**
 *	@brief	���O�̃��W���[���Ăяo���ō쐬���ꂽ�f�[�^���Z�[�u�f�[�^�ɔ��f
 */
int MailSys_PushDataToSave(MAIL_PARAM* wk,MAILBLOCK_ID blockID,u8 dataID)
{
	if(!MailSys_IsDataCreate(wk)){
		return 0;	//�쐬����Ă��Ȃ�
	}
	MAIL_AddMailFormWork(wk->pBlock,blockID,dataID,wk->pDat);
	return 1;
}

/**
 *	@brief	���O�̃��W���[���Ăяo���ō쐬���ꂽ�f�[�^���Z�[�u�f�[�^�ɔ��f(Poke)
 */
int MailSys_PushDataToSavePoke(MAIL_PARAM* wk,POKEMON_PARAM* poke)
{
	if(!MailSys_IsDataCreate(wk)){
		return 0;	//�쐬����Ă��Ȃ�
	}
	PokeParaPut(poke,ID_PARA_mail_data,wk->pDat);
	return 1;
}

/**
 *	@brief	���[�����W���[���Ăяo�����[�N�����
 */
void MailSys_ReleaseCallWork(MAIL_PARAM* wk)
{
	//�f�[�^�e���|�������m�ۂ��Ă����ꍇ�͉��
	if(wk->pDat != NULL){
		sys_FreeMemoryEz(wk->pDat);
	}
	sys_FreeMemoryEz(wk);
}
#endif

/**
 *	@brief	�|�P�������[�����p�\���[���Ɉړ�
 *
 *	@retval	MAILDATA_NULLID	�󂫂��Ȃ��̂œ]���ł��Ȃ�
 *	@retval	"���̑�"�@�]�������f�[�^ID
 */
int	MailSys_MoveMailPoke2Paso(MAIL_BLOCK* block,POKEMON_PARAM* poke,HEAPID heapID)
{
	int id;
	int itemno = ITEM_DUMMY_DATA;
	MAIL_DATA* src = NULL;

	//�󂫗̈挟��
	id = MAIL_SearchNullID(block,MAILBLOCK_PASOCOM);
	if(id == MAILDATA_NULLID){
		return MAILDATA_NULLID;
	}
	//���[�N�쐬
	src = MailData_CreateWork(heapID);

	//�|�P�������烁�[�����擾
	PP_Get(poke,ID_PARA_mail_data,src);

	//�p�\�R���̈�Ƀf�[�^�R�s�[
	MAIL_AddMailFormWork(block,MAILBLOCK_PASOCOM,id,src);

	//�|�P�������烁�[�����O��
	MailData_Clear(src);
	PP_Put(poke,ID_PARA_mail_data,(u32)src);
	PP_Put(poke,ID_PARA_item,itemno);

	//�̈���
	GFL_HEAP_FreeMemory(src);

	return id;
}

/**
 *	@brief	�p�\���[�����|�P�����Ɉړ�
 *
 *	@retval	MAILDATA_NULLID ID���s���Ȃ̂łȂɂ����Ȃ�����
 *	@retval	"���̑�"�@�]�������f�[�^ID
 */
int	MailSys_MoveMailPaso2Poke(MAIL_BLOCK* block,u16 id,POKEMON_PARAM* poke,HEAPID heapID)
{
	int itemno = ITEM_DUMMY_DATA;
	MAIL_DATA* src = NULL;

	//���[���f�[�^���擾
	src = MAIL_AllocMailData(block,MAILBLOCK_PASOCOM,id,heapID);
	if(src == NULL){
		return MAILDATA_NULLID;
	}
	//���[���̃f�U�C��No����A�C�e��No�ɕ���
	itemno = ITEM_MailNumGet(MailData_GetDesignNo(src));
	//�|�P�����Ƀ��[�����ڂ�
	PP_Put(poke,ID_PARA_mail_data,(u32)src);
	PP_Put(poke,ID_PARA_item,itemno);

	//�p�\�R���̈�̃f�[�^������
	MAIL_DelMailData(block,MAILBLOCK_PASOCOM,id);

	//�̈���
	GFL_HEAP_FreeMemory(src);
	return id;
}




//===========================================================================
//�f�o�b�O�p���[�`��
//===========================================================================
