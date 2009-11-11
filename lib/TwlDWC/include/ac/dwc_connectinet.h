/*---------------------------------------------------------------------------*
  Project:  TwlDWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_CONNECTINET_H_
#define DWC_CONNECTINET_H_

#ifdef __cplusplus
extern "C"
{
#endif


    /* -------------------------------------------------------------------------
            define
       ------------------------------------------------------------------------- */

// WEP �Í������[�h��� (�݊����̂��ߎc����Ă��܂�)
#define DWC_WEPMODE_NONE        DWC_APINFO_WSEC_NONE                  // �Í����Ȃ�
#define DWC_WEPMODE_40          DWC_APINFO_WSEC_WEPMODE_40            // RC4 (  40 �r�b�g ) �Í������[�h
#define DWC_WEPMODE_104         DWC_APINFO_WSEC_WEPMODE_104           // RC4 ( 104 �r�b�g ) �Í������[�h
#define DWC_WEPMODE_128         DWC_APINFO_WSEC_WEPMODE_128           // RC4 ( 128 �r�b�g ) �Í������[�h

// �j���e���h�[Wi-Fi�X�e�[�V�����̏ꏊ���̒���
#define DWC_SIZE_ZONEINFO				9
#define DWC_SIZE_SPOTINFO 				DWC_SIZE_ZONEINFO				// nintendo spot�݊��p

#define	DWC_APINFO_TYPE_NINTENDOSPOT	DWC_APINFO_TYPE_NINTENDOZONE	// nintendo spot�݊��p

// WDS�p�o�b�t�@�T�C�Y
#define	DWC_WDS_SSID_BUF_SIZE				32
#define DWC_WDS_WEPKEY_BUF_SIZE             32



    /* -------------------------------------------------------------------------
            enum
       ------------------------------------------------------------------------- */

    /**
     * �C���^�[�l�b�g�ڑ����
     *
     */
    typedef enum
    {
        DWC_CONNECTINET_STATE_NOT_INITIALIZED = 0, ///< ����������Ă��Ȃ�
        DWC_CONNECTINET_STATE_IDLE,         ///< �ڑ��J�n����Ă��Ȃ�
        DWC_CONNECTINET_STATE_OPERATING,    ///< ������
        DWC_CONNECTINET_STATE_OPERATED,     ///< ��������
        DWC_CONNECTINET_STATE_CONNECTED,    ///< �C���^�[�l�b�g�֐ڑ��ς�
        DWC_CONNECTINET_STATE_DISCONNECTING,///< �C���^�[�l�b�g����ؒf��
        DWC_CONNECTINET_STATE_DISCONNECTED, ///< �C���^�[�l�b�g����ؒf���ꂽ
        DWC_CONNECTINET_STATE_ERROR,        ///< �G���[����������
        DWC_CONNECTINET_STATE_FATAL_ERROR,  ///< �v���I�ȃG���[����������
        DWC_CONNECTINET_STATE_LAST
    } DWCInetResult;


    /**
     * �F�؃T�[�o�̎��
     * 
     */
    typedef enum
    {
        DWC_CONNECTINET_AUTH_TEST,    ///< �J�������F�؃T�[�o
        DWC_CONNECTINET_AUTH_DEVELOP, ///< ���C�u�����J���p�F�؃T�[�o(�g�p���Ȃ��ŉ�����)
        DWC_CONNECTINET_AUTH_RELEASE, ///< ���i�����F�؃T�[�o
        DWC_CONNECTINET_AUTH_LAST
    } DWCInetAuthType;


    /**
     * �j���e���h�[Wi-Fi�X�e�[�V����/Nintendo Zone�̐ݒu�n��������G���A�R�[�h
     *
     */
    typedef enum
    {
        DWC_APINFO_AREA_JPN = 0, ///< ���{
        DWC_APINFO_AREA_USA,     ///< �k��
        DWC_APINFO_AREA_EUR,     ///< ���B
        DWC_APINFO_AREA_AUS,     ///< ���B
        DWC_APINFO_AREA_KOR,     ///< �؍�
        DWC_APINFO_AREA_UNKNOWN = 0xff ///< �s�� (�j���e���h�[Wi-Fi�X�e�[�V����/Nintendo Zone�ł͂Ȃ�)
    } DWCApInfoArea;


    /**
     * AP�^�C�v
     *
     */
    typedef enum
    {
        DWC_APINFO_TYPE_USER0 = 0, ///< �ڑ���1�̃A�N�Z�X�|�C���g
        DWC_APINFO_TYPE_USER1,     ///< �ڑ���2�̃A�N�Z�X�|�C���g
        DWC_APINFO_TYPE_USER2,     ///< �ڑ���3�̃A�N�Z�X�|�C���g
        DWC_APINFO_TYPE_USB,       ///< �j���e���h�[Wi-Fi USB�R�l�N�^
        DWC_APINFO_TYPE_SHOP,      ///< �j���e���h�[Wi-Fi�X�e�[�V����
        DWC_APINFO_TYPE_FREESPOT,  ///< FREESPOT�i�t���[�X�|�b�g�j
        DWC_APINFO_TYPE_WAYPORT,   ///< Wayport(�k�ăz�b�g�X�|�b�g)�����݂͎g�p�ł��܂���
        DWC_APINFO_TYPE_OTHER,     ///<     (����`)
        DWC_APINFO_TYPE_NINTENDOWFC = DWC_APINFO_TYPE_OTHER, ///<    (�\��)
        DWC_APINFO_TYPE_NINTENDOZONE, ///< Nintendo Zone
#ifdef SDK_TWL
        DWC_APINFO_TYPE_USER3 = 0x0a, ///< �ڑ���4�̃A�N�Z�X�|�C���g�@ ��NITRO ROM�ł͂��̒�`�͂���܂���
        DWC_APINFO_TYPE_USER4,        ///< �ڑ���5�̃A�N�Z�X�|�C���g�@ ��NITRO ROM�ł͂��̒�`�͂���܂���
        DWC_APINFO_TYPE_USER5,        ///< �ڑ���6�̃A�N�Z�X�|�C���g�@ ��NITRO ROM�ł͂��̒�`�͂���܂���
#endif // SDK_TWL
        DWC_APINFO_TYPE_UNKNOWN = 99  ///< �s��
    } DWCApInfoType;


    /**
     * �����Z�L�����e�B�^�C�v
     *
     */
    typedef enum
    {
        DWC_APINFO_WSEC_NONE  = 0x00,  ///< �Í����Ȃ�
        DWC_APINFO_WSEC_WEPMODE_40,    ///< RC4 (  40 �r�b�g ) �Í������[�h
        DWC_APINFO_WSEC_WEPMODE_104,   ///< RC4 ( 104 �r�b�g ) �Í������[�h
        DWC_APINFO_WSEC_WEPMODE_128	   ///< RC4 ( 128 �r�b�g ) �Í������[�h
#ifdef SDK_TWL
        ,DWC_APINFO_WSEC_WPA_PSK_TKIP, ///< WPA-PSK  ( TKIP ) �Í��@ ��NITRO ROM�ł͂��̒�`�͂���܂���
        DWC_APINFO_WSEC_WPA2_PSK_TKIP, ///< WPA2-PSK ( TKIP ) �Í��@ ��NITRO ROM�ł͂��̒�`�͂���܂���
        DWC_APINFO_WSEC_WPA_PSK_AES,   ///< WPA-PSK  ( AES  ) �Í��@ ��NITRO ROM�ł͂��̒�`�͂���܂���
        DWC_APINFO_WSEC_WPA2_PSK_AES   ///< WPA2-PSK ( AES  ) �Í��@ ��NITRO ROM�ł͂��̒�`�͂���܂���
#endif
    } DWCApInfoSecurity;


    /**
     * �C���^�[�l�b�g�Ƃ̐ڑ��e�X�g�̒ʐM��������
     *
     */
    typedef enum
    {
        DWC_TESTINET_NOT_INITIALIZED = 0, ///< DWC_TestInetAsync�֐����Ăяo����Ă��Ȃ�
        DWC_TESTINET_OPERATING,    ///< �ʐM���������s��
        DWC_TESTINET_CONNECTED,    ///< �C���^�[�l�b�g�ɐڑ�����Ă���
        DWC_TESTINET_DISCONNECTED  ///< �C���^�[�l�b�g�ɐڑ�����Ă��Ȃ�
    } DWCTestInetResult;


    /**
     * Proxy �F�،`��
     *
     */
    typedef enum
    {
        DWC_PROXY_AUTHTYPE_NONE  = 0x00,	///< �F�؂Ȃ�
        DWC_PROXY_AUTHTYPE_BASIC 			///< BASIC �F��
    } DWCProxyAuthType;


    // WDS�p�X�e�[�g
    typedef enum
    {
        DWC_WDS_STATE_IDLE      = 0,
        DWC_WDS_STATE_PROCESS   = 1,
        DWC_WDS_STATE_COMPLETED = 2, ///< AP�̃f�[�^���擾�ł����B
        DWC_WDS_STATE_FAILED    = 3, ///< �擾�ł��Ȃ������B
        DWC_WDS_STATE_ERROR     = 4,
        DWC_WDS_STATE_CANCEL    = 5  ///< WDS��������DWC_CleanupInetAsync()���Ă΂ꂽ�̂ŏI��������
    } DWCWDSState;



    /* -------------------------------------------------------------------------
            struct
       ------------------------------------------------------------------------- */


    /**
     * �ڑ����̃A�N�Z�X�|�C���g�Ɋւ�������i�[���邽�߂̍\���̂ł��B
     *
     * �A�N�Z�X�|�C���g�̏����i�[���邽�߂̍\���̂ł��B
     *
     * See also:  DWC_GetApInfo
     *
     */
    typedef struct
    {
        DWCApInfoType		aptype; ///< �A�N�Z�X�|�C���g�̎��
        ///<
        ///< - DWC_APINFO_TYPE_USER0        :  �ڑ���1�̃A�N�Z�X�|�C���g
        ///< - DWC_APINFO_TYPE_USER1        :  �ڑ���2�̃A�N�Z�X�|�C���g
        ///< - DWC_APINFO_TYPE_USER2        :  �ڑ���3�̃A�N�Z�X�|�C���g
        ///< - DWC_APINFO_TYPE_USER3        :  �ڑ���4�̃A�N�Z�X�|�C���g
        ///< - DWC_APINFO_TYPE_USER4        :  �ڑ���5�̃A�N�Z�X�|�C���g
        ///< - DWC_APINFO_TYPE_USER5        :  �ڑ���6�̃A�N�Z�X�|�C���g
        ///< - DWC_APINFO_TYPE_USB          :  �j���e���h�[Wi-Fi USB�R�l�N�^
        ///< - DWC_APINFO_TYPE_SHOP         :  �j���e���h�[Wi-Fi�X�e�[�V����
        ///< - DWC_APINFO_TYPE_FREESPOT     :  FREESPOT�i�t���[�X�|�b�g�j
        ///< - DWC_APINFO_TYPE_WAYPORT      :  Wayport(�k�ăz�b�g�X�|�b�g)�����݂͎g�p�ł��܂���
        ///< - DWC_APINFO_TYPE_NINTENDOZONE : Nintendo Zone

        DWCApInfoArea		area; ///< �j���e���h�[Wi-Fi�X�e�[�V����/nintendo zone�̐ݒu�n��
        ///< ��aptype��DWC_APINFO_TYPE_SHOP/DWC_APINFO_TYPE_NINTENDOZONE�̎��̂ݗL���ł��B
        ///< �����L�̑S�Ă̒n��ɐݒu����Ă���Ƃ͌���܂���B
        ///< - DWC_APINFO_AREA_JPN      :  ���{
        ///< - DWC_APINFO_AREA_USA      :  �k��
        ///< - DWC_APINFO_AREA_EUR      :  ���B
        ///< - DWC_APINFO_AREA_AUS      :  ���B
        ///< - DWC_APINFO_AREA_KOR      :  �؍�
        ///< - DWC_APINFO_AREA_UNKNOWN  : (�j���e���h�[Wi-Fi�X�e�[�V����/nintendo zone�ł͂���܂���)

        DWCApInfoSecurity   security; ///< AP�Ƃ̒ʐM�ɗp���閳���Í�������
        ///<
        ///< - DWC_APINFO_WSEC_NONE            :  �Í����Ȃ�
        ///< - DWC_APINFO_WSEC_WEPMODE_40      :  RC4 (  40 �r�b�g ) �Í������[�h
        ///< - DWC_APINFO_WSEC_WEPMODE_104     :  RC4 ( 104 �r�b�g ) �Í������[�h
        ///< - DWC_APINFO_WSEC_WEPMODE_128     :  RC4 ( 128 �r�b�g ) �Í������[�h
        ///< - DWC_APINFO_WSEC_WPA_PSK_TKIP    :  WPA-PSK  ( TKIP ) �Í�
        ///< - DWC_APINFO_WSEC_WPA2_PSK_TKIP   :  WPA2-PSK ( TKIP ) �Í�
        ///< - DWC_APINFO_WSEC_WPA_PSK_AES     :  WPA-PSK  ( AES  ) �Í�
        ///< - DWC_APINFO_WSEC_WPA2_PSK_AES    :  WPA2-PSK ( AES  ) �Í�

        char				spotinfo[DWC_SIZE_ZONEINFO+1]; ///< �j���e���h�[Wi-Fi�X�e�[�V����/nintendo zone�̏ꏊ���
        ///< ��aptype��DWC_APINFO_TYPE_SHOP/DWC_APINFO_TYPE_NINTENDOZONE�̎��̂ݗL���ł��B
        ///< �e�j���e���h�[Wi-Fi�X�e�[�V����/nintendo zone�Ń��j�[�N��9���̕�����ł��B

        char				essid[WM_SIZE_SSID+1]; ///< �A�N�Z�X�|�C���g��SSID ��aptype��DWC_APINFO_TYPE_USER*�̎��̂ݗL���ł��B
        u8 					bssid[WM_SIZE_BSSID]; ///< �A�N�Z�X�|�C���g��BSSID
    } DWCApInfo;


    /**
     * �ڑ����̃A�N�Z�X�|�C���g�Ɋւ���proxy�ݒ�����擾����֐��ł��B
     *
     * proxy�ݒ�����i�[���邽�߂̍\���̂ł��B
     *
     * See also:  DWC_GetProxySetting
     */
    typedef struct
    {
        u16         authType;					///< proxy�̔F�؃^�C�v
        ///<
        ///< - DWC_PROXY_AUTHTYPE_NONE �F �F�؂Ȃ�
        ///< - DWC_PROXY_AUTHTYPE_BASIC�F BASIC �F��
        
        u16         port;						///< proxy���g�p����|�[�g
        u8          hostName      [ 0x64 ];		///< proxy�T�[�o��URL
        u8          authId        [ 0x20 ];		///< basic�F�؎���ID
        u8          authPass      [ 0x20 ];		///< basic�F�؎��̃p�X���[�h
    } DWCProxySetting;


    // WDS�p�̍\����
    typedef struct tagDWCWDSData
    {
        u8          ssid[ DWC_WDS_SSID_BUF_SIZE ];
        u8          wep[ DWC_WDS_WEPKEY_BUF_SIZE ];
        int         wepMode;
        u16		    mtu;
        char        apnum[10];
    } DWCWDSData;


    typedef struct DWCstInetControl
    {
        volatile int ac_state;
        u16  state;
        u16  online;
        u16  dmaNo;
        u16  powerMode;
        s32  rwin; ///< TCP rwin�T�C�Y
        u8   wontExecuteEULACheck;    ///< EULA�`�F�b�N�����Ȃ��ꍇTRUE
        u8            wontExecuteWDS; ///< nintendo zone�ւ̐ڑ������݂�t�F�[�Y�𒆎~���邩�ǂ����̃t���O
        DWCWDSState   processWDSstate;///< WDS�̏����X�e�[�g
        DWCWDSData    nspotInfo;      ///< WDS�Ŏ擾����ssid,wep,wepmode,apnum���i�[���邽�߂̕ϐ�
#ifndef SDK_FINALROM
        int  connectType;
#endif
    } DWCInetControl;



    /* -------------------------------------------------------------------------
            function - external
       ------------------------------------------------------------------------- */

    typedef void* (* DWCACAlloc )( u32 name, s32 size );				// alloc�֐��ւ̃|�C���^
    typedef void  (* DWCACFree  )( u32 name, void *ptr, s32 size );		// free�֐��ւ̃|�C���^

    void  DWC_InitInet( DWCInetControl* inetCntl );
    void  DWC_InitInetEx( DWCInetControl* inetCntl, u16 dmaNo, u16 powerMode, u16 ssl_priority );
    void  DWC_SetRwinSize( s32 size );

#ifndef SDK_FINALROM
    void         DWC_SetConnectApType( DWCApInfoType type );
#else
#define      DWC_SetConnectApType( type )       ((void)0)
#endif

    void  DWC_ConnectInetAsync( void );
    void  DWC_DebugConnectInetAsync( const void *ssid, const void *wep, int wepMode );
    void  DWC_DebugConnectInetExAsync( const void *ssid, const void *wep, int wepMode, u16 mtu, const char *apSpotInfo, int overrideType );

    BOOL  DWC_CheckInet( void );
    void  DWC_ProcessInet( void );
    DWCInetResult DWC_GetInetStatus( void );

    BOOL DWC_UpdateConnection( void );

    BOOL DWC_GetApInfo(DWCApInfo *apinfo);
    BOOL DWC_GetProxySetting( DWCProxySetting *proxy );
    WMLinkLevel DWC_GetLinkLevel( void );
    int   DWC_GetUdpPacketDropNum( void );

    void  DWC_CleanupInet( void );
    BOOL  DWC_CleanupInetAsync( void );

    BOOL DWC_TestInetAsync(int timeout);
    DWCTestInetResult DWC_TestInetProcess(void);


    void DWC_EnableHotspot(void); // [nakata] �u���E�U�Ή��̊g��
    BOOL DWC_CheckWiFiStation( const void *ssid, u16 len, DWCApInfo* apinfo );
    size_t DWC_GetCheckNintendoZoneWorkSize( void );
    BOOL DWC_CheckNintendoZone( void* work, const WMBssDesc* bssdesc, DWCApInfo* apinfo );
    size_t DWC_GetParseWMBssDescWorkSize( void );
    BOOL DWC_ParseWMBssDesc( void* work, const WMBssDesc* bssdesc, DWCApInfo* apinfo );
    void DWC_SetDisableEulaCheck( void );


#ifdef __cplusplus
}
#endif


#endif // DWC_CONNECTINET_H_
