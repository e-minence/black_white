/**
 *  @file mailview.h
 *  @brief  ���[���`�惋�[�`���@���[�J���w�b�_
 *  @author MiyukiIwasawa
 *  @date 06.02.08
 */

#ifndef _H_MAIL_VIEW_H_
#define _H_MAIL_VIEW_H_

#define MAIL_KEY_IN (APP_END_KEY)
#define MAIL_TOUCH_IN (APP_END_TOUCH)

typedef enum{
  MAILMODE_CREATE,  ///<�쐬����
  MAILMODE_VIEW_D,  ///<�f�U�C�����݂�
  MAILMODE_VIEW,    ///<�쐬�ς݂̃��[�����݂�
}MAILMODE_ENUM;

enum{
 VIEW_END_LINE0 = 0,
 VIEW_END_LINE1,
 VIEW_END_LINE2,
 VIEW_END_WORD,
 VIEW_END_DECIDE,
 VIEW_END_CANCEL = 0xffff,
};

///���[���f�[�^�e���|����
typedef struct _MAIL_TMP_DATA{
  u16 val;
  u8  cntNo;
  u8  flags;
//  CONFIG* configSave; ///<�R���t�B�O�Z�[�u�f�[�^
//  KEYTOUCH_STATUS* kt_status; ///<�L�[or�^�b�`�X�e�[�^�X

  u32 writerID;
  u8  sex;
  u8  lang;
  u8  ver;
  u8  design;
  STRBUF  *name;
  MAIL_ICON icon[MAILDAT_ICONMAX];
  PMS_DATA  msg[MAILDAT_MSGMAX];
  u8  pms_condition[4];             // �ȈՉ�b���̓^�C�v���w��i0:���R 1:��^���Ȃ̂ŕ��͕ύX�s�� 2:���͕s��)
  PMS_WORD  pmsword;
}MAIL_TMP_DATA;

extern GFL_PROC_RESULT MailViewProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT MailViewProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT MailViewProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk );

#endif  //_H_MAIL_VIEW_H_


