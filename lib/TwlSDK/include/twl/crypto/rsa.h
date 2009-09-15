/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include
  File:     crypto/rsa.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::$
  $Rev:$
  $Author:$
 *---------------------------------------------------------------------------*/

#ifndef NITRO_CRYPTO_RSA_H_
#define NITRO_CRYPTO_RSA_H_

#ifdef __cplusplus
extern "C" {
#endif


#define CRYPTO_RSA_VERIFY    // RSA�������؋@�\��L���ɂ������ꍇ�͂������`����


/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/

#define CRYPTO_RSA_CONTEXT_SIZE         (4 * 1)
#define CRYPTO_RSA_SIGN_CONTEXT_SIZE    (4 * 1)



/*---------------------------------------------------------------------------*
    �^��`
 *---------------------------------------------------------------------------*/

// RSA �����R���e�L�X�g
//   CRYPTORSAContext_local�ƃT�C�Y����v����K�v������
//   �����o��ǉ��E�폜�����Ƃ��͑Ή����邱��(CRYPTO_RSA_CONTEXT_SIZE)
typedef struct CRYPTORSAContext
{
/* private: */
    u8      mem[CRYPTO_RSA_CONTEXT_SIZE];
}
CRYPTORSAContext;

// CRYPTO_RSA_EncryptInit I/O�X�g���[���p�����[�^
typedef struct CRYPTORSAEncryptInitParam
{
    void    *key;       // [in]  ���J��������
    u32     key_len;    // [in]  ���J��������
}
CRYPTORSAEncryptInitParam;

// CRYPTO_RSA_EncryptInit_PrivateKey I/O�X�g���[���p�����[�^
typedef struct CRYPTORSAEncryptInitPKParam
{
    void    *privkey;       // [in]  �閧��������
    u32     privkey_len;    // [in]  �閧��������
}
CRYPTORSAEncryptInitPKParam;

// CRYPTO_RSA_Encrypt I/O�X�g���[���p�����[�^
typedef struct CRYPTORSAEncryptParam
{
    void    *in;        // [in]  �Í���������
    u32     in_len;     // [in]  �Í���������
    void    *out;       // [out] �o�͕�����o�b�t�@
    u32     out_size;   // [in]  �o�͕�����o�b�t�@�T�C�Y
}
CRYPTORSAEncryptParam;

// CRYPTO_RSA_DecryptInit I/O�X�g���[���p�����[�^
typedef struct CRYPTORSADecryptInitParam
{
    void    *key;       // [in]  ���J��������
    u32     key_len;    // [in]  ���J��������
}
CRYPTORSADecryptInitParam;

// CRYPTO_RSA_Decrypt I/O�X�g���[���p�����[�^
typedef struct CRYPTORSADecryptParam
{
    void    *in;        // [in]  ������������
    u32     in_len;     // [in]  ������������
    void    *out;       // [out] �o�͕�����o�b�t�@
    u32     out_size;   // [in]  �o�͕�����o�b�t�@�T�C�Y
}
CRYPTORSADecryptParam;

// RSA �d�q�����n�����p�R���e�L�X�g
//   CRYPTORSASignContext_local�ƃT�C�Y����v����K�v������
//   �����o��ǉ��E�폜�����Ƃ��͑Ή����邱��(CRYPTO_RSA_SIGN_CONTEXT_SIZE)
typedef struct CRYPTORSASignContext
{
/* private: */
    u8      mem[CRYPTO_RSA_SIGN_CONTEXT_SIZE];
}
CRYPTORSASignContext;

// CRYPTO_RSA_SignInit I/O�X�g���[���p�����[�^
typedef struct CRYPTORSASignInitParam
{
    void    *key;       // [in]  �閧��������
    u32     key_len;    // [in]  �閧��������
}
CRYPTORSASignInitParam;

// CRYPTO_RSA_Sign I/O�X�g���[���p�����[�^
typedef struct CRYPTORSASignParam
{
    void    *in;        // [in]  �����Ώە�����
    u32     in_len;     // [in]  �����Ώە�����
    void    *out;       // [out] �o�͏���������o�b�t�@
    u32     out_size;   // [in]  �o�͏���������o�b�t�@�T�C�Y
}
CRYPTORSASignParam;

#if defined(CRYPTO_RSA_VERIFY)
// CRYPTO_RSA_VerifyInit I/O�X�g���[���p�����[�^
typedef struct CRYPTORSAVerifyInitParam
{
    void    *key;       // [in]  ���J��������
    u32     key_len;    // [in]  ���J��������
}
CRYPTORSAVerifyInitParam;

// CRYPTO_RSA_Verify I/O�X�g���[���p�����[�^
typedef struct CRYPTORSAVerifyParam
{
    void    *in;        // [in]  ���؂��镶����
    u32     in_len;     // [in]  ���؂��镶����
    void    *sign;      // [in]  ����������
    u32     sign_len;   // [in]  ����������
}
CRYPTORSAVerifyParam;
#endif


/*---------------------------------------------------------------------------*
    ��l�\���̐錾
 *---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*
    �֐��錾
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_EncryptInit

  Description:  RSA �Í����̂��߂̏���������

  Arguments:    context:    ���C�u�����R���e�L�X�g
                param:      I/O�X�g���[���p�����[�^

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_EncryptInit(CRYPTORSAContext *context, CRYPTORSAEncryptInitParam *param);

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_EncryptInit_PrivateKey

  Description:  RSA �Í����̂��߂̏��������� (�閧�����g�p����)

  Arguments:    context:    ���C�u�����R���e�L�X�g
                param:      I/O�X�g���[���p�����[�^

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_EncryptInit_PrivateKey(CRYPTORSAContext *context, CRYPTORSAEncryptInitPKParam *param);

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_Encrypt

  Description:  RSA �Í�������

  Arguments:    context:    ���C�u�����R���e�L�X�g
                param:      I/O�X�g���[���p�����[�^

  Returns:      �� - �Í�������������, -1 - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_Encrypt(CRYPTORSAContext *context, CRYPTORSAEncryptParam *param);

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_EncryptTerminate

  Description:  RSA �Í����̂��߂̏I������

  Arguments:    context:    ���C�u�����R���e�L�X�g

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_EncryptTerminate(CRYPTORSAContext *context);

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_DecryptInit

  Description:  RSA �������̂��߂̏���������

  Arguments:    context:    ���C�u�����R���e�L�X�g
                param:      I/O�X�g���[���p�����[�^

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_DecryptInit(CRYPTORSAContext *context, CRYPTORSADecryptInitParam *param);

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_Decrypt

  Description:  RSA ����������

  Arguments:    context:    ���C�u�����R���e�L�X�g
                param:      I/O�X�g���[���p�����[�^

  Returns:      �� - �Í�������������, -1 - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_Decrypt(CRYPTORSAContext *context, CRYPTORSADecryptParam *param);

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_DecryptTerminate

  Description:  RSA �������̂��߂̏I������

  Arguments:    context:    ���C�u�����R���e�L�X�g

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_DecryptTerminate(CRYPTORSAContext *context);


/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_SignInit

  Description:  RSA �����̂��߂̏���������

  Arguments:    context:    ���C�u�����R���e�L�X�g
                param:      I/O�X�g���[���p�����[�^

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_SignInit(CRYPTORSASignContext *context, CRYPTORSASignInitParam *param);

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_Sign

  Description:  RSA �����쐬����

  Arguments:    context:    ���C�u�����R���e�L�X�g
                param:      I/O�X�g���[���p�����[�^

  Returns:      �� - �쐬��������������, -1 - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_Sign(CRYPTORSASignContext *context, CRYPTORSASignParam *param);

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_SignTerminate

  Description:  RSA �����̂��߂̏I������

  Arguments:    context:    ���C�u�����R���e�L�X�g

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_SignTerminate(CRYPTORSASignContext *context);


#if defined(CRYPTO_RSA_VERIFY)
/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_VerifyInit

  Description:  RSA �������؂̂��߂̏���������

  Arguments:    context:    ���C�u�����R���e�L�X�g
                param:      I/O�X�g���[���p�����[�^

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_VerifyInit(CRYPTORSASignContext *context, CRYPTORSAVerifyInitParam *param);

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_Verify

  Description:  RSA ��������

  Arguments:    context:    ���C�u�����R���e�L�X�g
                param:      I/O�X�g���[���p�����[�^

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_Verify(CRYPTORSASignContext *context, CRYPTORSAVerifyParam *param);

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_RSA_VerifyTerminate

  Description:  RSA �������؂̂��߂̏I������

  Arguments:    context:    ���C�u�����R���e�L�X�g

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_RSA_VerifyTerminate(CRYPTORSASignContext *context);
#endif


/* for internal use */



#ifdef __cplusplus
}
#endif

#endif //NITRO_CRYPTO_RSA_H_
