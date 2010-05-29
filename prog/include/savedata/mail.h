/**
 *  @file mail.h
 *  @brief  ���[���f�[�^��`
 *          ���̃w�b�_�ɂ�MAILDATA�ɃA�N�Z�X���邽�߂�
 *          ����������ǉ����Ă��������B�Z�[�u�f�[�^�Ȃǂ�
 *          mail_util.h�ŁB
 *
 *  @date 09.11.16
 *  @author Miyuki Iwasawa
 *
 *  ���f�[�^���̂��`����ӏ��ȊO�ŎQ�Ƃ��Ă͂����܂���
 *  ���z�|�C���^�o�R�Ńf�[�^�A�N�Z�X�ł���Ηǂ��ӏ��ł�
 *  savedata/mail_util.h��include���Ă�������
 */

#pragma once

#include <strbuf.h>
#include "buflen.h"
#include "system/pms_data.h"
///�萔��`
#include "savedata/mail_def.h"


//------------------------------------------------------------
/**
 * @brief ���[���f�[�^�Ǘ��\���ւ̕s���S�^�|�C���^
 *
 * ���g�͌����Ȃ����ǃ|�C���^�o�R�ŎQ�Ƃ͂ł���
 */
//------------------------------------------------------------
typedef struct _MAIL_DATA MAIL_DATA;

//=============================================================
/**
 *  ���[���f�[�^�A�N�Z�X�n�֐�
 */
//=============================================================
/**
 *  @brief  ���[���f�[�^�T�C�Y�擾
 *
 *  �����[���f�[�^��ʂ̃T�C�Y
 */
extern int MailData_GetDataWorkSize(void);

/**
 *  @brief  ���[���f�[�^�N���A(�����f�[�^�Z�b�g)
 */
extern void MailData_Clear(MAIL_DATA* dat);

/**
 *  @brief  ���[���f�[�^���L�����ǂ����Ԃ�
 *  @retval FALSE ����
 *  @retval TRUE  �L��
 */
extern BOOL MailData_IsEnable(MAIL_DATA* dat);

/**
 *  @brief  ���[���f�[�^�̃��[�N���擾���ĕԂ�
 *
 *  ���Ăяo�������ӔC�����ĉ�����邱��
 *  
 */
extern MAIL_DATA* MailData_CreateWork(HEAPID heapID);

/**
 *  @brief  ���[���f�[�^�̍\���̃R�s�[
 */
extern void MailData_Copy(MAIL_DATA* src,MAIL_DATA* dest);

/**
 *  @brief  ���[���f�[�^�̓��e��r
 *
 * @retval  BOOL    ��v���Ă�����TRUE��Ԃ�
 */
extern BOOL MailData_Compare(MAIL_DATA* src1,MAIL_DATA* src2);

/**
 *  @brief  ���[���f�[�^�@�g���[�i�[ID�A�N�Z�X
 */
extern u32  MailData_GetWriterID(const MAIL_DATA* dat);
extern void MailData_SetWriterID(MAIL_DATA* dat,u32 id);

/**
 *  @brief  ���[���f�[�^�@���C�^�[���A�N�Z�X
 */
extern STRCODE* MailData_GetWriterName(MAIL_DATA* dat);
extern void MailData_SetWriterName(MAIL_DATA* dat,STRCODE* name);

/**
 *  @brief  ���[���f�[�^�@���C�^�[�̐��ʃA�N�Z�X
 */
extern u8 MailData_GetWriterSex(const MAIL_DATA* dat);
extern void MailData_SetWriterSex(MAIL_DATA* dat,const u8 sex);

/**
 *  @brief  ���[���f�[�^�@�f�U�C��No�A�N�Z�X
 */
extern u8 MailData_GetDesignNo(const MAIL_DATA* dat);
extern void MailData_SetDesignNo(MAIL_DATA* dat,const u8 design);

/**
 *  @brief  ���[���f�[�^�@���R�[�h�A�N�Z�X
 */
extern u8 MailData_GetCountryCode(const MAIL_DATA* dat);
extern void MailData_SetCountryCode(MAIL_DATA* dat,const u8 code);

/**
 *  @brief  ���[���f�[�^�@�J�Z�b�g�o�[�W�����A�N�Z�X
 */
extern u8 MailData_GetCasetteVersion(const MAIL_DATA* dat);
extern void MailData_SetCasetteVersion(MAIL_DATA* dat,const u8 version);


/**
 *  @brief  ���[���f�[�^�@�ȈՕ��擾(�C���f�b�N�X�w���)
 */
extern PMS_DATA*  MailData_GetMsgByIndex(MAIL_DATA* dat,u8 index);
extern void MailData_SetMsgByIndex(MAIL_DATA* dat, const PMS_DATA* pms,u8 index);

/**
 *  @brief  ���[���f�[�^�@�ȈՉ�b�P��i�ȈՉ�b�ł͂Ȃ��j
 */
extern u16  MailData_GetPmsWord(const MAIL_DATA* dat);
extern void MailData_SetPmsWord( MAIL_DATA* dat, u16 word );
