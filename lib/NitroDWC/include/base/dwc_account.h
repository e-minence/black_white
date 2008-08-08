/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - account
  File:     dwc_account.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.23 $
  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_ACCOUNT_H_
#define DWC_ACCOUNT_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/** --------------------------------------------------------------------
  ���[�U�f�[�^�i�[�p�̍\����
  ----------------------------------------------------------------------*/
// �t���O�`�F�b�N�p�̍\����
typedef struct DWCstAccFlag
{
    u32         flags;
    u32         reserved;
    u32         reserved1;
} DWCAccFlag;

// ���O�C��ID�i�[�p�̍\����
typedef struct DWCstAccLoginId
{
    u32         id_data;
    u32         userid_lo32;
    u32         playerid;
} DWCAccLoginId;

// �F�B�o�^���p�̍\����
typedef struct DWCstAccFriendKey
{
    u32         id_data;
    u32         friendkey_lo32;
    u32         friendkey_hi32;
} DWCAccFriendKey;

// GS Profile ID�p�̍\����
typedef struct DWCstAccGsProfileId
{
    u32         id_data;
    int         id;
    u32         reserved;
} DWCAccGsProfileId;

// �F�B���i�[�p�̋��p�� 12Bytes
typedef union DWCstAccFriendData
{
    DWCAccFlag          flags;
    DWCAccLoginId       login_id;
    DWCAccFriendKey     friend_key;
    DWCAccGsProfileId   gs_profile_id;
} DWCAccFriendData;

// �����̃f�[�^�i�[�p�̍\���� 64Bytes
typedef struct DWCstAccUserData
{
    int                 size;               // 4
    DWCAccLoginId       pseudo;             // 12
    DWCAccLoginId       authentic;          // 12
    int                 gs_profile_id;      // 4
    int                 flag;               // 4
    u32                 gamecode;           // 4

    int                 reserved[5];        // 20
    u32                 crc32;              // 4
} DWCAccUserData;

// DWCUserData�̒�`
typedef DWCAccUserData DWCUserData;
typedef DWCAccFriendData DWCFriendData;

/** --------------------------------------------------------------------
  �萔��`
  ----------------------------------------------------------------------*/
// FriendData�p�̒�`
#define DWC_ACC_USERDATA_BUFSIZE            64

#define DWC_ACC_USERNAME_STRING_LENGTH      20
#define DWC_ACC_USERNAME_GSBRCD_OFFSET      9
#define DWC_ACC_USERNAME_STRING_BUFSIZE     (DWC_ACC_USERNAME_STRING_LENGTH + 1)
#define DWC_ACC_FRIENDKEY_STRING_LENGTH     12
#define DWC_ACC_FRIENDKEY_STRING_BUFSIZE    (DWC_ACC_FRIENDKEY_STRING_LENGTH + 1)

#define DWC_ACC_MASKBITS(bits)          (((1u<<bits)-1))

#define DWC_ACC_USERID_BITS             43
#define DWC_ACC_PLAYERID_BITS           32

#define DWC_ACC_USERID_HI32_SHIFT       0
#define DWC_ACC_USERID_HI32_BITS        11
#define DWC_ACC_USERID_HI32_MASK        DWC_ACC_MASKBITS(DWC_ACC_USERID_HI32_BITS)
#define DWC_ACC_FLAGS_SHIFT             11
#define DWC_ACC_FLAGS_BITS              21
#define DWC_ACC_FLAGS_MASK              DWC_ACC_MASKBITS(DWC_ACC_FLAGS_BITS)

#define DWC_ACC_FRIENDDATA_NODATA           0
#define DWC_ACC_FRIENDDATA_LOGIN_ID         1
#define DWC_ACC_FRIENDDATA_FRIEND_KEY       2
#define DWC_ACC_FRIENDDATA_GS_PROFILE_ID    3
#define DWC_ACC_FRIENDDATA_MASK             3

#define DWC_ACC_FRIENDDATA_ISBUDDY          (1u << 2)
#define DWC_ACC_FRIENDDATA_ISBUDDY_MASK     (1u << 2)
#define DWC_ACC_FRIENDDATA_IS_REVERSEBUDDY         (1u << 3)
#define DWC_ACC_FRIENDDATA_IS_REVERSEBUDDY_MASK    (1u << 3)
#define DWC_ACC_FRIENDDATA_EXP_ISBUDDYFRIEND       (1u << 4)
#define DWC_ACC_FRIENDDATA_EXP_ISBUDDYFRIEND_MASK  (1u << 4)
#define DWC_ACC_FRIENDDATA_SENT_BUDDYREQ           (1u << 5)
#define DWC_ACC_FRIENDDATA_SENT_BUDDYREQ_MASK      (1u << 5)

#define DWC_ACC_USERDATA_DIRTY                    (1u)
#define DWC_ACC_USERDATA_DIRTY_MASK               (1u)

// �F�B�R�[�h�̐����A���S���Y���񋓌^
typedef enum
{
    DWCi_FRIENDKEY_GENERATOR_CRC,   // CRC�ɂ�鐶�����@(���o�[�W����)
    DWCi_FRIENDKEY_GENERATOR_MD5,   // MD5�ɂ�鐶�����@(�V�o�[�W���� 3.0�ȍ~)

    DWCi_FRIENDKEY_GENERATOR_NUM    // �J�E���^
} DWCiFriendKeyGenMethod;

/** --------------------------------------------------------------------
  ���J��`
  ----------------------------------------------------------------------*/
#define DWC_FRIENDDATA_NODATA        DWC_ACC_FRIENDDATA_NODATA
#define DWC_FRIENDDATA_LOGIN_ID      DWC_ACC_FRIENDDATA_LOGIN_ID
#define DWC_FRIENDDATA_FRIEND_KEY    DWC_ACC_FRIENDDATA_FRIEND_KEY
#define DWC_FRIENDDATA_GS_PROFILE_ID DWC_ACC_FRIENDDATA_GS_PROFILE_ID
#define DWC_FRIENDKEY_STRING_BUFSIZE DWC_ACC_FRIENDKEY_STRING_BUFSIZE

/** --------------------------------------------------------------------
  �֐��̊O����`
  ----------------------------------------------------------------------*/
// ���[�U�f�[�^�̍쐬�^�m�F�p�B
void    DWC_CreateUserData            ( DWCAccUserData* userdata, u32 gamecode );
BOOL    DWC_CheckUserData             ( const DWCAccUserData* userdata );
BOOL    DWC_CheckHasProfile           ( const DWCAccUserData* userdata );
BOOL    DWC_CheckValidConsole         ( const DWCAccUserData* userdata );
BOOL    DWC_CheckDirtyFlag            ( const DWCAccUserData* userdata );
void    DWC_ClearDirtyFlag            ( DWCAccUserData* userdata );

// �F�B�֌W
u64     DWC_GetFriendKey              ( const DWCAccFriendData* frienddata );
u64     DWC_CreateFriendKey           ( const DWCAccUserData*   userdata );
u64     DWC_CreateFriendKeyLegacy     ( const DWCAccUserData*   userdata );
void    DWC_CreateExchangeToken       ( const DWCAccUserData*   userdata, DWCAccFriendData* token );
void    DWC_CreateFriendKeyToken      ( DWCAccFriendData* token,      u64 friend_key    );
void    DWC_SetGsProfileId            ( DWCAccFriendData* frienddata, int gs_profile_id );
BOOL    DWC_CheckFriendKey            ( const DWCAccUserData*   userdata, u64 friend_key );
BOOL    DWC_CheckFriendKeyLegacy      ( const DWCAccUserData*   userdata, u64 friend_key );
BOOL    DWC_IsEqualFriendData         ( const DWCAccFriendData* frienddata1, const DWCAccFriendData* frienddata2 );
BOOL    DWC_IsValidFriendData         ( const DWCAccFriendData* frienddata );
BOOL    DWC_IsBuddyFriendData         ( const DWCAccFriendData* frienddata );
BOOL    DWC_WasBuddyFriendData    ( const DWCAccFriendData* frienddata ); // �F�B���F�B���X�g���玄���폜���Ă��Ȃ����m�F����B
int     DWC_GetFriendDataType         ( const DWCAccFriendData* frienddata ); // ReverseBuddy��Buddy�̃t���O�𗼕����Ĕ��肷��
void    DWC_ClearBuddyFlagFriendData  ( DWCAccFriendData*       frienddata );

// �F�B�����p
int     DWC_GetGsProfileId            ( const DWCAccUserData* userdata, const DWCAccFriendData* frienddata );
void    DWC_LoginIdToUserName         ( const DWCAccUserData* userdata, const DWCAccFriendData* frienddata, char* username );

// �f�o�b�O�p
void    DWC_ReportFriendData          ( const DWCAccUserData* userdata, const DWCAccFriendData* frienddata );
void    DWC_ReportUserData            ( const DWCAccUserData* userdata );

// �F�B�o�^���֌W
u64     DWC_StringToFriendKey         ( const char* string );
void    DWC_FriendKeyToString         ( char* string, u64 friendKey );

// �����֐��B
BOOL    DWCi_IsBuddyFriendData( const DWCAccFriendData* frienddata );         // DWC_IsBuddyFriendData �ƈႢ�ABuddy�t���O�݂̂�����
BOOL    DWCi_IsReverseBuddyFriendData ( const DWCAccFriendData* frienddata ); // �Ўv���̗F�B�𒲂ׂ�ׂ̊֐�
BOOL    DWCi_IsBuddyMsgAlreadySent( const DWCAccFriendData* frienddata );     // 1��̃��O�C���ɑ΂��A1�l/1���b�Z�[�W�̐����������邽�߂̃t���O
void    DWCi_Acc_CreateUserData       ( DWCAccUserData* userdata, u32 gamecode );
void    DWCi_Acc_CreateTempLoginId    ( DWCAccLoginId* loginid );

BOOL    DWC_Acc_CheckFriendKey        ( u64 friendkey, u32 gamecode, DWCiFriendKeyGenMethod method );
u64     DWC_Acc_CreateFriendKey       ( int gs_profile_id, u32 gamecode, DWCiFriendKeyGenMethod method );
int     DWC_Acc_FriendKeyToGsProfileId( u64 friendkey, u32 gamecode );

u64     DWCi_Acc_GetUserId            ( const DWCAccLoginId*     data );
u32     DWCi_Acc_GetPlayerId          ( const DWCAccLoginId*     data );
u64     DWCi_Acc_GetFriendKey         ( const DWCAccFriendKey*   data );
int     DWCi_Acc_GetGsProfileId       ( const DWCAccGsProfileId* data );
void    DWCi_Acc_SetUserId            ( DWCAccLoginId*     data, u64 userid );
void    DWCi_Acc_SetPlayerId          ( DWCAccLoginId*     data, u32 playerid );
void    DWCi_Acc_SetFriendKey         ( DWCAccFriendKey*   data, u64 friend_key );
void    DWCi_Acc_SetGsProfileId       ( DWCAccGsProfileId* data, int gs_profile_id );

void    DWCi_Acc_LoginIdToUserName    ( const DWCAccLoginId* loginid, u32 gamecode, char* username );
void    DWCi_Acc_SetLoginIdToUserData ( DWCAccUserData* userdata, const DWCAccLoginId* loginid, int gs_profile_id );
BOOL    DWCi_Acc_CheckConsoleUserId   ( const DWCAccLoginId* loginid );

BOOL    DWCi_Acc_IsDirty              ( const DWCAccUserData* userdata );
void    DWCi_Acc_ClearDirty           ( DWCAccUserData* userdata );
BOOL    DWCi_Acc_IsValidLoginId       ( const DWCAccLoginId* loginid );
BOOL    DWCi_Acc_IsAuthentic          ( const DWCAccUserData* userdata );
BOOL    DWCi_Acc_IsValidFriendData    ( const DWCAccFriendData* frienddata );

void    DWCi_SetBuddyFriendData       ( DWCAccFriendData* frienddata );
void    DWCi_SetReverseBuddyFlag      ( DWCAccFriendData* frienddata, BOOL setflag );
void    DWCi_SetExpIsBuddyFriendFlag  ( DWCAccFriendData* frienddata, BOOL setflag );
void    DWCi_SetSentBuddyReqFlag      ( DWCAccFriendData* frienddata, BOOL setflag );

void
DWCi_Acc_TestFlagFunc( void );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DWC_ACCOUNT_H_
