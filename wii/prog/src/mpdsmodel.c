/*---------------------------------------------------------------------------*
  Project:  Revolution MPDS demo
  File:	 mpdsmodel.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mpdsmodel.c,v $
  Revision 1.21  2008/06/05 09:50:49  seiki_masashi
  MPWaitPseudoVBlank �֐��̒ǉ�

  Revision 1.20  2008/06/03 05:03:00  seiki_masashi
  Pseudo VBlank �ʒm�𗘗p����悤�ɕύX

  Revision 1.19  2007/11/29 03:20:27  seiki_masashi
  const �C���q�̒ǉ�

  Revision 1.18  2007/11/21 06:53:14  yosizaki
  ���[���o�b�N�������e���Ĕ��f�B

  Revision 1.16  2007/10/27 11:20:46  seiki_masashi
  small fix

  Revision 1.15  2007/10/26 12:15:00  yosizaki
  �{�[���\����ǉ��B

  Revision 1.14  2007/10/26 09:31:17  seiki_masashi
  �R�[�h�̐���

  Revision 1.13  2007/10/26 07:29:56  seiki_masashi
  �R�[�h�̐���

  Revision 1.12  2007/10/26 07:05:28  seiki_masashi
  �����N���x���\���̏C��

  Revision 1.10  2007/10/25 03:20:53  seiki_masashi
  �q�@�̃j�b�N�l�[���̕\���Ɋւ��āAROM �t�H���g�̃G���R�[�h�ɉ������ϊ��ւ̑Ή�

  Revision 1.8  2007/10/24 13:50:49  seiki_masashi
  Lobby �ł̃j�b�N�l�[���\���@�\�̒ǉ�

  Revision 1.5  2007/10/24 01:38:51  seiki_masashi
  �ڑ���Ԃ̕\�����@���C��

  Revision 1.3  2007/10/23 13:43:34  seiki_masashi
  �p�b�h���͂𑗐M����悤�ɏC��

  Revision 1.2  2007/10/23 13:08:49  seiki_masashi
  �}���`�X���b�h��

  Revision 1.1  2007/10/23 00:05:57  seiki_masashi
  mpdsmodel �f���̒ǉ�

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//
// NITRO-SDK �� demos/wm/dataShare-Model �f���ƒʐM���s���T���v���ł��B
//
// �q�@����̐V�K�ڑ������r�[��ʂŎ󂯕t���A�V�K�ڑ�����ߐ؂��Ă���
// �ΐ���J�n����Ƃ����V�[�N�G���X�̃f���ɂȂ��Ă��܂��B
//

#include <string.h>
#include <revolution.h>
#include <revolution/mp.h>
#include <revolution/mpds.h>
#include <revolution/mem.h>
#include <revolution/enc.h>

#include "rexdemo/demokpad.h"
#include "rexdemo/graphic.h"

#include "draw.h"

#include "ball.h"
#include "mpdsmodel.h"
#include "ds_net.h"
#include "common_def.h"
#include "net_system.h"

#define	 USERHEAP_SIZE ( 0x20000 )

/*===========================================================================*/

// �A�v���ŗL�� UserGameInfo
// DS �ɍ��킹�� Little Endian �ŏ�������K�v�����邱�Ƃɒ���
#pragma pack(push,1)
typedef struct MyGameInfo
{
	u8	  nickName[10 * 2];
	u8	  nickNameLength;
	u8	  entryCount;
	u16	 periodicalCount;
}
MyGameInfo;
#pragma pack(pop)

enum
{
	SHARECMD_NONE = 0,				 // ���ɖ����i�m�[�}���j
	SHARECMD_EXITLOBBY,				// ���r�[��ʏI���̍��}
	SHARECMD_NUM
};

enum
{
	STATE_IDLE = 0,
	STATE_GOING_TO_LOBBY,
	STATE_LOBBY,
	STATE_GOING_TO_COMMUNICATION,
	STATE_COMMUNICATION,
	STATE_GOING_TO_IDLE
};

enum
{
	RESULT_OK = 0,
	RESULT_ERROR = -1,
	RESULT_QUIT = -2
};

/*===========================================================================*/

static const GXColor white = { 0xFF, 0xFF, 0xFF, };
static const GXColor yellow = { 0xFF, 0xFF, 0x00, };
static const GXColor gray = { 0x80, 0x80, 0x80, };
static const GXColor black = { 0x00, 0x00, 0x00, };
static const GXColor red = { 0xFF, 0x00, 0x18 };
static const GXColor green = { 0x00, 0xFF, 0x00 };

static MEMHeapHandle	sUserHeap;

static ShareData sRecvData[MY_MAX_NODES+1];
static BOOL sRecvFlag[MY_MAX_NODES+1];
static BOOL sConnectedFlag[MY_MAX_NODES+1];
static u8 sConnectedMacAddress[MY_MAX_NODES+1][MP_SIZE_MACADDRESS];
static u16 sConnectedNickName[MY_MAX_NODES+1][10 + 1];

static u32 sFrameCount = 0;
static s32 sState = STATE_IDLE;
static OSThreadQueue sStateChangeThreadQueue;
static BOOL sQuitFlag = FALSE;

static OSMutex sGlobalMutex;

static OSThread sStateManagementThread;
static OSThread sDataSharingThread;
static OSThread sUpdateGameInfoThread;

static u8 sStateManagementThreadStack[STACK_SIZE] ATTRIBUTE_ALIGN(32);
static u8 sDataSharingThreadStack[STACK_SIZE] ATTRIBUTE_ALIGN(32);
static u8 sUpdateGameInfoThreadStack[STACK_SIZE] ATTRIBUTE_ALIGN(32);

static MPDSContext sDSContext;
static DS_COMM_USERDATAINFO sMyGameInfo;
static ENCContext sEncContext;

/*===========================================================================*/

static void* StateManagementThreadFunc( void* arg );
static void* DataSharingThreadFunc( void* arg );
static void* UpdateGameInfoThreadFunc( void* arg );

static void Initialize( void );
static void MainLoop( void );

static void Draw( void );
static void SetState( s32 state );
static inline s32 GetState( void );
static s32 WaitForStateChange( void );
static void QuitMP( void );
static BOOL IsQuitting( void );
static s32 TransToLobby( void );
static s32 TransToCommunication( void );
static s32 TransToIdle( void );
static s32 DoLobbyMode( void );
static s32 DoCommunicationMode( void );
static void DataSharingCallback( s32 type, MPPortCallbackInfo* info );
static void IndicationCallback( s32 type, MPIndicationInfo* info );

static u16 ConvKPadKeyToDSPad(u32 mixedKey);

inline void EnterCriticalSection( void )
{
	(void)OSLockMutex( &sGlobalMutex );
}

void LeaveCriticalSection( void )
{
	(void)OSUnlockMutex( &sGlobalMutex );
}

/*===========================================================================*/

// GGID �̓A�v���P�[�V�������Ɋ��蓖�Ă��� MP �ʐM�p�̃��j�[�N ID
//   ���A�v���P�[�V�����ł́A�C�V������� GGID �̊��蓖�Ă��󂯂�K�v������܂��B
//   �����ł� NITRO-SDK �� dataShare-Model �f���Ɠ���� GGID ���g�p���܂��B
#define MY_GGID	 (0x346)//0x003fff13

// MP �ʐM�̐ݒ�
static MPConfig sMpConfig =
{
	mpAlloc,
	mpFree,

	8,				  // threadPriority; ���C���X���b�h���4�ȏ㍂���D��x�ɐݒ肷��

	MP_MODE_PARENT,	 // mode

	MY_GGID,			// ggid
	MP_TGID_AUTO,	   // tgid

	MP_CHANNEL_AUTO,	// channel; �ʏ�� MP_CHANNEL_AUTO

	MP_LIFETIME_DEFAULT,// lifeTime; �ʏ�� MP_LIFETIME_DEFAULT

	MP_BEACON_PERIOD_AUTO,  // beaconPeriod; �ʏ�� MP_BEACON_PERIOD_AUTO
	MY_MAX_NODES,	   // maxNodes
	MY_PARENT_MAX_SIZE, // parentMaxSize
	MY_DS_LENGTH,	   // childMaxSize
	TRUE,			   // entryFlag
	FALSE,			  // multiBootFlag; �ʏ�� FALSE

	1,				  // frequency

	0,				  // userGameInfoLength
	{ 0, },			 // userGameInfo; �v���O������ MyGameInfo ��ݒ肷��

	NULL,			   // indicationCallbackFunction

	/// ...			 // port �ݒ� (MPSetPortConfig �Őݒ�\)
};

// DataSharing �̐ݒ�
static MPDSConfig sMpdsConfig =
{
	MY_DS_LENGTH,	   // dataLength

	MY_DS_PORT,		 // port (12�`15 �� Sequential �ʐM�p�|�[�g���g���K�v������)
	MP_PRIORITY_HIGH,   // priority

	TRUE,			   // isParent; ��� TRUE
	(1 << MY_DS_NODES)-1,   // aidBits (MY_DS_NODES ��1������2�i���̒l)
	TRUE,			   // isDoubleMode
	TRUE,			   // isAutoStart; ��� TRUE
	DataSharingCallback // mpdsCallback; �s�v�Ȃ� NULL
};


void main()
{
	OSReport("test program started.\n");

	Initialize();
	
	MainLoop();
}

void Initialize( void )
{
#if 1
	/* initialize OS and memory heap */
	REXDEMOKPadInit();
//	REXDEMOInitScreen( FALSE );
	REXDEMOSetGroundColor( black );
	REXDEMOSetFontSize( 10, 20 );
//	REXDEMOBeginRender();
//	REXDEMOWaitRetrace();
	(void)PADInit();
#endif

	/* Initialize heap for MP library */
	{
		void*   heapAddress;

		heapAddress = OSGetMEM2ArenaLo();
		OSSetMEM2ArenaLo( (void*)OSRoundUp32B( (u32)heapAddress + USERHEAP_SIZE ) );
		sUserHeap   = MEMCreateExpHeapEx( heapAddress, USERHEAP_SIZE, MEM_HEAP_OPT_THREAD_SAFE );
		if( sUserHeap == NULL )
		{
			OSHalt( "Could not create heap.\n" );
		}
	}
	MPDS_Init();

	OSInitMutex( &sGlobalMutex );
	OSInitThreadQueue( &sStateChangeThreadQueue );

#if 0
	// ������\���� ROM �t�H���g���g�p���Ă��邽��
	// ROM �t�H���g�̃G���R�[�h�ɍ��킹�� ENCContext ����������
	(void)ENCInitContext(&sEncContext);
	(void)ENCSetExternalEncoding(&sEncContext,
								 ( OSGetFontEncode() == OS_FONT_ENCODE_SJIS )
								 ? (const u8*)"Shift_JIS" : (const u8*)"ISO-8859-1");
	(void)ENCSetBreakType(&sEncContext, ENC_BR_KEEP);
	(void)ENCSetAlternativeCharacter(&sEncContext, L'?', L'?');
#endif

	sState = STATE_IDLE;

	// ��ԊǗ��X���b�h�̍쐬
	(void)OSCreateThread( &sStateManagementThread, StateManagementThreadFunc, NULL,
						  (void*)((u32)sStateManagementThreadStack + STACK_SIZE), STACK_SIZE,
						  THREAD_PRIORITY, OS_THREAD_ATTR_DETACH );
	(void)OSResumeThread( &sStateManagementThread );
	
	InitDrawSystem();
}

void MainLoop( void )
{
	while (TRUE)
	{

		// ���̃X���b�h�Ƌ��L����O���[�o���ϐ��ɐG��ۂɂ͔r�����s��
		EnterCriticalSection();
		REXDEMOKPadRead();
		LeaveCriticalSection();

		if ( OSIsThreadTerminated(&sStateManagementThread) )
		{
			// �S�ẴX���b�h���I�������̂Ń��C�����[�v�𔲂���
			OSReport("All threads are terminated; exit from MainLoop\n");
			break;
		}

//		Draw();

		UpdateDrawSystem();

//		REXDEMOWaitRetrace();
		sFrameCount++;
	}
}

void Draw( void )
{
	s32 state;
	s32 i;

	REXDEMOBeginRender();

	{
		// ���̃X���b�h�Ƌ��L����O���[�o���ϐ��ɐG��ۂɂ͔r�����s��
		EnterCriticalSection(); // �r���J�n
		state = GetState();

		switch ( state )
		{
		case STATE_LOBBY:
			REXDEMOSetTextColor(green);
			REXDEMOPrintf(40, 8, 0, "Lobby mode");
			REXDEMOSetTextColor(white);
			REXDEMOPrintf(4, 400, 0, "push <A> button to start.");

			for (i = 0; i < MY_MAX_NODES+1; i++)
			{
				if (sConnectedFlag[i])
				{
					char nickName[20+1];
					{
						ENCContext convCtx;
						s32 dstlen, srclen;
						dstlen = sizeof(nickName) - 1 /* NULL termination */;
						srclen = -1; // NULL �I�[�܂ł�ϊ�����
						(void)NETMemSet(nickName, 0, sizeof(nickName));
						(void)ENCDuplicateContext(&convCtx, &sEncContext);
						(void)ENCConvertFromInternalEncoding(&convCtx, (u8*)nickName, &dstlen, sConnectedNickName[i], &srclen);
					}
					REXDEMOSetTextColor(yellow);
					REXDEMOPrintf(40, (s16)(80 + i * 20), 0,
								  "AID(%02d): entry %02X:%02X:%02X:%02X:%02X:%02X %s",
								  i,
								  sConnectedMacAddress[i][0],
								  sConnectedMacAddress[i][1],
								  sConnectedMacAddress[i][2],
								  sConnectedMacAddress[i][3],
								  sConnectedMacAddress[i][4],
								  sConnectedMacAddress[i][5],
								  nickName);
				}
				else
				{
					REXDEMOSetTextColor(gray);
					REXDEMOPrintf(40, (s16)(80 + i * 20), 0, "AID(%02d): --------", i);
				}
			}
			break;

		case STATE_COMMUNICATION:
			REXDEMOSetTextColor(green);
			REXDEMOPrintf(40, 8, 0, "Parent mode");
			REXDEMOSetTextColor(white);
			REXDEMOPrintf(4, 400, 0, "push <B> button to restart.");

			REXDEMOSetTextColor(yellow);
			REXDEMOPrintf(4, 30, 0, "Send:	 0x%04X-0x%04X", 0, sFrameCount & 0xffff);
			REXDEMOPrintf(4, 52, 0, "Receive:");

			for (i = 0; i < MY_MAX_NODES+1; i++)
			{
				if (sRecvFlag[i])
				{
				/*
					REXDEMOSetTextColor(yellow);
					REXDEMOPrintf(40, (s16)(80 + i * 20), 0,
								  "AID(%02d): %04X-%04X",
								  i,
								  MPMPToH16((u16)sRecvData[i].key), MPMPToH16((u16)(sRecvData[i].count & 0xffff)) );
				*/
				}
				else
				{
					REXDEMOSetTextColor(red);
					REXDEMOPrintf(40, (s16)(80 + i * 20), 0, "No Data");
				}
			}
			{
				static const GXColor	paletteTable[] =
				{
					{ 0x80, 0x80, 0x80, },  // gray
					{ 0x00, 0xFF, 0x00, },  // green
					{ 0xFF, 0xFF, 0x00, },  // yellow
				};
				s16	 x   = 320;
				s16	 y   = 10;
				s16	 z   = -1023;
				for (i = 0; i < BALL_PLAYER_MAX; ++i)
				{
					REXDEMOSetTextColor(paletteTable[shared->ball[i].plt]);
					REXDEMOPrintf(x + shared->ball[i].x, y + shared->ball[i].y, 0, "%c", shared->ball[i].chr);
				}
				{
					static const u32 font[] ATTRIBUTE_ALIGN(32) =
					{
						0xFFFFFFFF,
						0xFFFFFFFF,
						0xFFFFFFFF,
						0xFFFFFFFF,
						0xFFFFFFFF,
						0xFFFFFFFF,
						0xFFFFFFFF,
						0xFFFFFFFF,
					};
					static const u16 pal[16] ATTRIBUTE_ALIGN(32) = 
					{
					GXPackedRGB5A3(0x00, 0x00, 0x00, 0xFF), /* 0 : black */ 
					GXPackedRGB5A3(0x80, 0x00, 0x00, 0xFF), /* 1 : dark-red */ 
					GXPackedRGB5A3(0x00, 0x80, 0x00, 0xFF), /* 2 : dark-green */ 
					GXPackedRGB5A3(0x80, 0x80, 0x00, 0xFF), /* 3 : dark-yellow */ 
					GXPackedRGB5A3(0x00, 0x00, 0x80, 0xFF), /* 4 : dark-blue */ 
					GXPackedRGB5A3(0x80, 0x00, 0x80, 0xFF), /* 5 : dark-purple */ 
					GXPackedRGB5A3(0x00, 0x80, 0x80, 0x00), /* 6 : (clear) */ 
					GXPackedRGB5A3(0x80, 0x80, 0x80, 0xFF), /* 7 : gray */ 
					GXPackedRGB5A3(0xC0, 0xC0, 0xC0, 0xFF), /* 8 : light-gray */ 
					GXPackedRGB5A3(0xFF, 0x00, 0x00, 0xFF), /* 9 : red */ 
					GXPackedRGB5A3(0x00, 0xFF, 0x00, 0xFF), /* A : green */ 
					GXPackedRGB5A3(0xFF, 0xFF, 0x00, 0xFF), /* B : yellow */ 
					GXPackedRGB5A3(0x00, 0x00, 0xFF, 0xFF), /* C : blue */ 
					GXPackedRGB5A3(0xFF, 0x00, 0xFF, 0xFF), /* D : purple */ 
					GXPackedRGB5A3(0x00, 0xFF, 0xFF, 0xFF), /* E : cyan */ 
					GXPackedRGB5A3(0xFF, 0xFF, 0xFF, 0xFF), /* F : white */ 
					};
					static GXTlutObj tlut;
					Mtx mtx;
					GXTexObj obj;
					GXInitTlutObj(&tlut, (void *)pal, GX_TL_RGB5A3, 16);
					GXLoadTlut(&tlut, GX_TLUT0);
					GXInitTexObjCI(&obj, (void *)font,
								   8, 8, GX_TF_C4,
								   GX_CLAMP, GX_CLAMP, GX_FALSE, GX_TLUT0);
					GXInitTexObjLOD(&obj, GX_NEAR, GX_NEAR,
									0.0f, 0.0f, 0.0f, GX_DISABLE,
									GX_FALSE, GX_ANISO_1);
					GXLoadTexObj(&obj, GX_TEXMAP4);
					MTXScale(mtx, 1.0f / (float)8, 1.0f / (float)8, 1.0f);
					GXLoadTexMtxImm(mtx, GX_TEXMTX4, GX_MTX2x4);
					// REXDEMOBeginRender �ݒ���ꎞ�I�ɕύX
					GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX4);
					GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP4, GX_COLOR0A0);
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					{
						GXPosition3s16((s16) (x), (s16) (y), (s16) z);
						GXTexCoord2s16((s16) (0), (s16) (0));
						GXPosition3s16((s16) (x + BALL_FIELD_WIDTH), (s16) (y), (s16) z);
						GXTexCoord2s16((s16) (8), (s16) (0));
						GXPosition3s16((s16) (x + BALL_FIELD_WIDTH), (s16) (y + BALL_FIELD_HEIGHT), (s16) z);
						GXTexCoord2s16((s16) (8), (s16) (8));
						GXPosition3s16((s16) (x), (s16) (y + BALL_FIELD_HEIGHT), (s16) z);
						GXTexCoord2s16((s16) (0), (s16) (8));
					}
					GXEnd();
					// REXDEMOBeginRender �ݒ�𕜌�
					GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
					GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				}
			}

			break;

		default:
			REXDEMOSetTextColor(green);
			REXDEMOPrintf(40, 8, 0, "Working...");
			break;
		}

		// Wii �̃K�C�h���C���ł́A�����N���x���̕\���͕K�{�ł͂���܂���B
		REXDEMOSetTextColor(green);
		{
			s32 linkLevel = MPGetLinkLevel();
			REXDEMOPrintf(480, 400, 0, "Link Level: %d", (linkLevel >= 0) ? linkLevel : 0);
		}

		REXDEMOSetTextColor(white);

		LeaveCriticalSection(); // �r���I��
	}
}

void SetState( s32 state )
{
	// ���̃X���b�h�Ƌ��L����O���[�o���ϐ��ɐG��ۂɂ͔r�����s��
	EnterCriticalSection(); // �r���J�n
	sState = state;
	LeaveCriticalSection(); // �r���I��
	OSWakeupThread( &sStateChangeThreadQueue );
}

inline s32 GetState( void )
{
	return sState;
}

s32 WaitForStateChange( void )
{
	OSSleepThread( &sStateChangeThreadQueue );
	// �D��x�̍����X���b�h�� SetState ��������Ă΂ꂽ�ꍇ�Ȃǂɂ����āA
	// sState �̕ω��͍Ō��1�������m�ł��Ȃ����Ƃɒ���
	return GetState();
}

void QuitMP( void )
{
	sQuitFlag = TRUE;
}

BOOL IsQuitting( void )
{
	return sQuitFlag;
}

/////////////////////////////////////////////////////////////////////
// �Q�[����Ԃ̊Ǘ��X���b�h
void* StateManagementThreadFunc( void* arg )
{
#pragma unused( arg )
	s32 result;

	///////////////////////////////////////////////////////
	// STATE_IDLE
	///////////////////////////////////////////////////////
state_idle:
	SetState( STATE_IDLE );

	///////////////////////////////////////////////////////
	// STATE_GOING_TO_LOBBY
	///////////////////////////////////////////////////////
state_going_to_lobby:
	SetState( STATE_GOING_TO_LOBBY );

	result = TransToLobby();

	switch ( result )
	{
	case RESULT_OK:
		// go to next state
		break;

	case RESULT_ERROR:
		goto state_error;

	case RESULT_QUIT:
		goto state_going_to_idle;
	}

	///////////////////////////////////////////////////////
	// STATE_LOBBY
	///////////////////////////////////////////////////////
state_lobby:
	SetState( STATE_LOBBY );

	result = DoLobbyMode();

	switch ( result )
	{
	case RESULT_OK:
		// go to next state
		break;

	case RESULT_ERROR:
		goto state_error;

	case RESULT_QUIT:
		goto state_going_to_idle;
	}

	///////////////////////////////////////////////////////
	// STATE_GOING_TO_COMMUNICATION
	///////////////////////////////////////////////////////
state_going_to_communication:
	SetState( STATE_GOING_TO_COMMUNICATION );

	(void)TransToCommunication();

	///////////////////////////////////////////////////////
	// STATE_COMMUNICATION
	///////////////////////////////////////////////////////
state_communication:
	SetState( STATE_COMMUNICATION );

	result = DoCommunicationMode();

	switch ( result )
	{
	case RESULT_OK:
		// go to next state
		goto state_going_to_idle;

	case RESULT_ERROR:
		goto state_error;

	case RESULT_QUIT:
		goto state_going_to_idle;
	}

	///////////////////////////////////////////////////////
	// STATE_GOING_TO_IDLE
	///////////////////////////////////////////////////////
state_going_to_idle:
	SetState( STATE_GOING_TO_IDLE );

	result = TransToIdle();

	switch ( result )
	{
	case RESULT_ERROR:
		goto state_error;
	}

	goto state_idle;

state_error:
	OSReport("Error occurred.\n");

	return NULL;
}

////////////////////////////////////////////
// �A�C�h����Ԃ��烍�r�[���[�h�ɏ�ԑJ��
s32 TransToLobby( void )
{
	s32 result;
	int i;

	// MP �ʐM�̊J�n�O�̂��߁A�����̃O���[�o���ϐ���G��X���b�h�͑��ɂ���܂���B
	// �r�������ŏ��������s���܂��B
	for ( i = 0; i < MY_MAX_NODES+1; i++ )
	{
		sRecvFlag[i] = FALSE;
		sConnectedFlag[i] = FALSE;
		(void)NETMemSet(sConnectedMacAddress[i], 0, MP_SIZE_MACADDRESS);
		(void)NETMemSet(sConnectedNickName[i], 0, sizeof(sConnectedNickName[0]));
	}
	// �������g�̐ݒ�
	sConnectedFlag[0] = TRUE;
	(void)NETGetWirelessMacAddress(sConnectedMacAddress[0]);
	(void)NETMemSet(sConnectedNickName[0], 0, sizeof(sConnectedNickName[0]));
	(void)NETMemCpy(sConnectedNickName[0], MY_NICKNAME, MY_NICKNAME_LENGTH * 2);
	ASSERT(MY_NICKNAME_LENGTH * 2 <= sizeof(sConnectedNickName[0]) - 2);

	sQuitFlag = FALSE;


	// �Z�b�V�����S�������[�J���ɕێ����ׂ��Q�[����������������
	// (MP�ʐM����ē��͏����������A�������[���œ��e���X�V���Ă���)
	InitBall(NULL, 0, 1, 2);


	// UserGameInfo �̏�����
	/*
	(void)NETMemSet(&sMyGameInfo, 0, sizeof(sMyGameInfo));
	sMyGameInfo.nickNameLength = MY_NICKNAME_LENGTH;
	//DS �ւ̃f�[�^�̓G���f�B�A����ϊ����Ȃ���΂Ȃ�Ȃ����Ƃɒ���
	NETSwapAndCopyMemory16(sMyGameInfo.nickName, MY_NICKNAME, MY_NICKNAME_LENGTH*sizeof(u16));
	sMyGameInfo.entryCount = 0;
	sMyGameInfo.periodicalCount = 0;
	(void)NETMemCpy( sMpConfig.userGameInfo, &sMyGameInfo, sizeof(sMyGameInfo) );
	sMpConfig.userGameInfoLength = sizeof(MyGameInfo);
	*/
	
	(void)NETMemSet(&sMyGameInfo,0,sizeof(DS_COMM_USERDATAINFO));
	NETMemCpy(sMyGameInfo.FixHead, "POKEWB", _BEACON_FIXHEAD_SIZE*sizeof(u8));
	sMyGameInfo.serviceNo = 30;
	sMyGameInfo.connectNum = 0;
	(void)NETMemCpy( sMpConfig.userGameInfo, &sMyGameInfo, sizeof(DS_COMM_USERDATAINFO) );
	sMpConfig.userGameInfoLength = sizeof(DS_COMM_USERDATAINFO);
	
	MPSetIndicationConfig( &sMpConfig, IndicationCallback );

	result = MPDSInit( &sDSContext, &sMpdsConfig );
	if ( result < MP_RESULT_OK )
	{
		OSReport( "MPDSInit returns %08x\n", result );
		return RESULT_ERROR;
	}

	result = MPDSSetupPortConfig( &sDSContext, &sMpConfig );
	if ( result < MP_RESULT_OK )
	{
		OSReport( "MPDSSetupPortConfig returns %08x\n", result );
		return RESULT_ERROR;
	}

	// config �ɏ]���� MP �ʐM��Ԃ��J�n
	//   MP ��ԂɂȂ�̂�҂��߃u���b�N���܂��B
	OSReport("MPStartup()\n");
	result = MPStartup( &sMpConfig );
	if( result != MP_RESULT_OK )
	{
		OSReport( "MPStartup returns %08x\n", result );
		return RESULT_ERROR;
	}

	// �ʐM�p�X���b�h�̍쐬
	(void)OSCreateThread( &sDataSharingThread, DataSharingThreadFunc, NULL,
						  (void*)((u32)sDataSharingThreadStack + STACK_SIZE), STACK_SIZE,
						  THREAD_PRIORITY, 0 );
	(void)OSResumeThread( &sDataSharingThread );
	(void)OSCreateThread( &sUpdateGameInfoThread, UpdateGameInfoThreadFunc, NULL,
						  (void*)((u32)sUpdateGameInfoThreadStack + STACK_SIZE), STACK_SIZE,
						  THREAD_PRIORITY, 0 );
	(void)OSResumeThread( &sUpdateGameInfoThread );

	return IsQuitting() ? RESULT_QUIT : RESULT_OK;
}

////////////////////////////////////////////
// ���r�[���[�h����ʐM���[�h�ɏ�ԑJ��
s32 TransToCommunication( void )
{
	s32 result;
	int i;

	// �V�K�ڑ�����ߐ؂�
	result = MPSetEntryFlag( FALSE );
	if ( result < MP_RESULT_OK )
	{
		OSReport( "MPSetEntryFlag returns %08x\n", result );
		return RESULT_ERROR;
	}

	// beacon ���X�V���āA�ڑ����ߐ؂�����m
	//   ���̃r�[�R�������M�����܂Œ����ԃu���b�N���܂��B
	result = MPUpdateBeacon();
	if ( result < MP_RESULT_OK )
	{
		OSReport( "MPUpdateBeacon returns %08x\n", result );
		return RESULT_ERROR;
	}

	// ���̃X���b�h�Ƌ��L����O���[�o���ϐ��ɐG��ۂɂ͔r�����s��
	EnterCriticalSection();
	for ( i = 0; i < MY_MAX_NODES+1; i++ )
	{
		sRecvFlag[i] = FALSE;
	}
	LeaveCriticalSection();

	return IsQuitting() ? RESULT_QUIT : RESULT_OK;
}

////////////////////////////////////////////
// �A�C�h����Ԃ֏�ԑJ��
s32 TransToIdle( void )
{
	s32 result;

	// �X���b�h�ɏI���̎w�߂��o��
	QuitMP();

	// MP �ʐM��Ԃ��I��
	//   �S�Ă̎q�@���ؒf�����̂�҂��߂ɒ����ԃu���b�N���܂��B
	OSReport("MPCleanup()\n");
	result = MPCleanup();
	if ( result < MP_RESULT_OK )
	{
		OSReport( "MPCleanup returns %08x\n", result );
		return RESULT_ERROR;
	}

	(void)OSJoinThread( &sDataSharingThread, NULL );
	(void)OSJoinThread( &sUpdateGameInfoThread, NULL );

	return RESULT_OK;
}

////////////////////////////////////////////
// Lobby Mode ���̏�ԑJ�ڏ���
s32 DoLobbyMode( void )
{
	BOOL fBreak = FALSE;

	// �q�@�̐V�K�ڑ��̎�t
	{
		while ( !fBreak )
		{
			// ���̃X���b�h�Ƌ��L����O���[�o���ϐ��ɐG��ۂɂ͔r�����s��
			EnterCriticalSection();
			if( REXDEMOGetAnyMixedPadTrigger() & (KPAD_BUTTON_A | (PAD_BUTTON_A << 16)) )
			{
				// A �{�^���Ŏ��̏�Ԃ֑J��
				fBreak = TRUE;
			}
			LeaveCriticalSection();

			VIWaitForRetrace();
		}
	}

	return RESULT_OK;
}

////////////////////////////////////////////
// Communication Mode ���̏�ԑJ�ڏ���
s32 DoCommunicationMode( void )
{
	BOOL fBreak = FALSE;

	// �ʐM��
	{
		while ( !fBreak )
		{
			// ���̃X���b�h�Ƌ��L����O���[�o���ϐ��ɐG��ۂɂ͔r�����s��
			EnterCriticalSection();
			if( REXDEMOGetAnyMixedPadTrigger() & (KPAD_BUTTON_B | (PAD_BUTTON_B << 16)) )
			{
				// B �{�^���Ŏ��̏�Ԃ֑J��
				fBreak = TRUE;
			}
			LeaveCriticalSection();

			VIWaitForRetrace();
		}
	}

	return RESULT_OK;
}


/////////////////////////////////////////////////////////////////////
// DataSharing �̏����X���b�h
void* DataSharingThreadFunc( void* arg )
{
#pragma unused( arg )
//	s32 result;
//	ShareData sendData;
//	MPDSDataSet recvDataSet;
	BOOL fError;
//	s32 i;
	s32 state;

	// Wii �� DS �ł� V-Blank �������قȂ邽�߁A
	// DS �Ԃōs���Ă��� MP �ʐM�̎����� Wii �� V-Blank �����ɍ����܂���B
	// ���̂��߁AWii �� V-Blank �����ɍ��킹�ē����Ă���`��p�̃��C�����[�v�Ƃ͕ʂ�
	// DataSharing �p�� MP �ʐM�̎��� (MPDSStep �֐��������������) �œ���
	// ��p�̃X���b�h���K�v�ɂȂ�܂��B

	// �X���b�h���쐬�����ɁA���C�����[�v���� MPDSTryStep �֐��𗘗p����
	// DataSharing ���s�����Ƃ��ł��܂����A
	// ���̏ꍇ�͐e�q�Ńf�[�^���Z�b�g����^�C�~���O�����킸�A
	// �����p�x�� DataSharing �����s���܂��B

	fError = FALSE;

	while ( !IsQuitting() && fError == FALSE )
	{
		// ���̃X���b�h�Ƌ��L����O���[�o���ϐ��ɐG��ۂɂ͔r�����s��
		EnterCriticalSection(); // �r���J�n

		state = GetState();

		switch ( state )
		{
		case STATE_LOBBY:
		case STATE_COMMUNICATION:
			LeaveCriticalSection(); // �r���I��
    		GFL_NET_HANDLE_MainProc();
    		GFL_NET_UpdateSystem( &sDSContext );
			if( GFL_NET_HANDLE_GetNumNegotiation() != 0 )
			{
				GFL_NET_HANDLE_RequestNegotiation();
			}
			
			break;

#if 0
			// DataSharing �����s���̏�Ԃ̏ꍇ

			// ���݂̃Q�[����Ԃ��瑗�M�f�[�^���쐬
			//   Wii �� DS �ł̓G���f�B�A�����قȂ邱�Ƃɒ���
			(void)NETMemSet( &sendData, 0, sizeof(sendData) );
			
			//sendData.data[_DS_HEADERNO] = _INVALID_HEADER;
			
			/*
			sendData.count = MPHToMP16( (u16)sFrameCount ); // �G���f�B�A���ϊ�
			if ( state == STATE_LOBBY )
			{
				sendData.command = SHARECMD_NONE;
			}
			else
			{
				sendData.command = SHARECMD_EXITLOBBY;
			}
			sendData.key = MPHToMP16(ConvKPadKeyToDSPad(REXDEMOGetAnyMixedPadHold()));
			sendData.level = MPGetLinkLevel();
			*/
			// �r����������ςȂ��Ńu���b�N���Ă͂����Ȃ�
			LeaveCriticalSection(); // �r���I��

			// DataSharing ��p���ăf�[�^�̋��L���s��
			//   MPDSStep �֐��̓f�[�^�������܂Œ����ԃu���b�N���܂��B
			result = MPDSTryStep( &sDSContext, &sendData, &recvDataSet );

			// ��M�f�[�^�����߂��A�Q�[����Ԃ��X�V����
			{
			
				// ���̃X���b�h�Ƌ��L����O���[�o���ϐ��ɐG��ۂɂ͔r�����s��
				EnterCriticalSection(); // �r���J�n
				//if( result == MP_RESULT_OK )
				{
					// ���L�f�[�^�̎�M�ɐ�������
					const u8* data;
					// ���L�f�[�^�̓��e�����߂���
					for ( i = 0; i < MY_MAX_NODES+1; i++ )
					{
						data = MPDSGetData( &sDSContext, &recvDataSet, (u32)i );
						if ( data != NULL )
						{
							// i �Ԗڂ̋��L���肩��̃f�[�^�����L�ł���
							(void)NETMemCpy( (void*)&sRecvData[i], data, MY_DS_LENGTH );
							//sRecvFlag[i] = TRUE;
							MPDS_PostShareData( data , (u8)i );
						}
						else
						{
							// NULL ���A���Ă���ꍇ�ɂ́A�������̃P�[�X������܂��B
							// 1. DataSharing �J�n����̋�ǂ݃f�[�^
							// 2. �Y������ AID �̎q�@���ڑ����Ă��Ȃ�
							// �i3. ���炩�̓����G���[���������Ă���j

							// ���̏����g���A�q�@�̐ڑ�/�ؒf���m�F���邱�Ƃ��\�ł��B
							// �������A�q�@�����R�ɐڑ��ł����Ԃł́A
							// �q�@���ؒf��������ɕʂ̎q�@���ڑ����Ă����ꍇ��
							// ��������o���邱�Ƃ��ł��܂���B
							// �Q�[���J�n��ɐV�K�̎q�@�̐ڑ�����ߐ؂�ɂ�
							// MPSetEntryFlag �֐��� MPUpdateBeacon �֐����g�p���Ă��������B
						}
					}
					
				}
			/*
				if ( state != GetState() )
				{
					// MPDS[Try]Step �֐��̌Ăяo�����ɏ�Ԃ��ω����Ă��܂���
					//   ����͂ǂ̏�Ԃł��f�[�^���߂ɍ��ق͂Ȃ��̂Ŗ������܂��B
					//
					//   ���L���ꂽ�f�[�^�́A�e�q�S�Ăœ��l�ɉ��߂���Ȃ��Ƃ����܂���̂ŁA
					//   �����̓s���ɂ���ẮA���L�f�[�^�̒��ɏ�Ԃ��܂߂Ă����A
					//   ���L���ꂽ�f�[�^�Ɋ܂܂�Ă����Ԃɏ]���ď����𕪊򂵂��ق����悢�ł��傤�B
				}

				for ( i = 0; i < MY_MAX_NODES+1; i++ )
				{
					sRecvFlag[i] = FALSE;
				}

				if( result == MP_RESULT_OK )
				{
					// ���L�f�[�^�̎�M�ɐ�������
					const u8* data;

					// ���L�f�[�^�̓��e�����߂���
					for ( i = 0; i < MY_MAX_NODES+1; i++ )
					{
						data = MPDSGetData( &sDSContext, &recvDataSet, (u32)i );
						if ( data != NULL )
						{
							// i �Ԗڂ̋��L���肩��̃f�[�^�����L�ł���
							(void)NETMemCpy( (void*)&sRecvData[i], data, MY_DS_LENGTH );
							sRecvFlag[i] = TRUE;
						}
						else
						{
							// NULL ���A���Ă���ꍇ�ɂ́A�������̃P�[�X������܂��B
							// 1. DataSharing �J�n����̋�ǂ݃f�[�^
							// 2. �Y������ AID �̎q�@���ڑ����Ă��Ȃ�
							// �i3. ���炩�̓����G���[���������Ă���j

							// ���̏����g���A�q�@�̐ڑ�/�ؒf���m�F���邱�Ƃ��\�ł��B
							// �������A�q�@�����R�ɐڑ��ł����Ԃł́A
							// �q�@���ؒf��������ɕʂ̎q�@���ڑ����Ă����ꍇ��
							// ��������o���邱�Ƃ��ł��܂���B
							// �Q�[���J�n��ɐV�K�̎q�@�̐ڑ�����ߐ؂�ɂ�
							// MPSetEntryFlag �֐��� MPUpdateBeacon �֐����g�p���Ă��������B
						}
					}

					//
					//
					// �q�@�Ɛe�@�Ńf�[�^�̋��L���ł����̂ŁA�Q�[������Ԃ��X�V���܂��B
					// �����ɁA���L�ł����f�[�^���g�p���ăQ�[������Ԃ��X�V���郍�W�b�N��u���܂��B
					//
					// DataSharing �𗘗p����ꍇ�́AMPDS[Try]Step �֐�����擾�ł����f�[�^�݂̂�
					// �g�p���ăQ�[������Ԃ��X�V���邱�ƂŁA�e�q�̊��S�ȓ�������邱�Ƃ��\�ł��B
					// ���݂̃p�b�h���͂Ȃǂ̃��[�J���̏��𒼐ڎg�p����Ɠ���������܂��̂ŁA
					// �����̏��ł����Ă��A�K����x MPDS[Try]Step �֐��ɓn���悤�ɂ��Ă��������B
					//
					//

					for (i = 0; i < BALL_PLAYER_MAX; ++i)
					{
						if ((i < MY_MAX_NODES+1) && sRecvFlag[i])
						{
							InputBallKey((int)i, (int)MPMPToH16((u16)sRecvData[i].key));
						}
					}
					UpdateBalls(0);

				}
				else if ( result == MP_RESULT_NO_DATA )
				{
					// MPDSTryStep �֐����g�p�����ꍇ�A
					// �ʐM��Ԃ����������Ȃǂ̗v���ɂ��f�[�^�������Ă��Ȃ������Ƃ���
					// ���ʂɔ�������G���[�ł��B
					// MPDSStep �֐����g�p�����ꍇ�ɂ͔������܂���B

					//
					// �e�q�Ńf�[�^���L���ł��Ă��Ȃ��Ƃ������ƂȂ̂ŁA
					// �Q�[������Ԃ��X�V�����Ɏ��̃t���[���܂ő҂��܂��B
					//
					// ���̏�Ԃ��������Ԃ͒ʐM�Q���҂ɂ��قȂ�ꍇ������܂��̂ŁA
					// ���x�f�[�^��p���č��W�f�[�^���X�V����A�Ƃ�
					// �����l��ǂݍ��ށA�Ƃ������A�v���O�����̓�����Ԃ�
					// �ύX���鑀����s���Ă͂����܂���B
					//
				}
				else
				{
					// �G���[
					OSReport("MPDSStep failed: %08x\n", result);
					fError = TRUE;
				}
			*/
				LeaveCriticalSection(); // �r���I��
			}
			
			break;
#endif
		default:
			// �ʐM��Ԃł͂Ȃ��ꍇ
			LeaveCriticalSection(); // �r���I��

			// ��Ԃ��ς��܂Ńu���b�N���đ҂�
			(void)WaitForStateChange();
			break;
		}

		// �q�@���������Ă��鉼�z VBlank �����Ƀ��[�v�𓯊�������
		// ������s�����ƂŁA�ʐM�̃��C�e���V���Ⴍ�Ȃ�A
		// �q�@�ɓ͂����M�f�[�^���e�@�̍ŐV�ɋ߂���Ԃ𔽉f����悤�ɂȂ�
		(void)MPWaitPseudoVBlank();
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////
// �r�[�R���X�V�p�X���b�h
void* UpdateGameInfoThreadFunc( void* arg )
{
#pragma unused( arg )
	while ( !IsQuitting() )
	{
		switch ( GetState() )
		{
		case STATE_LOBBY:
		case STATE_COMMUNICATION:
			// �ʐM���͒���I�Ƀr�[�R�����e���X�V����
			sMyGameInfo.connectNum = MPCountPopulation(MPGetConnectedAIDs());
			//sMyGameInfo.periodicalCount++;
			(void)MPSetUserGameInfo(&sMyGameInfo, sizeof(sMyGameInfo));

			// �ݒ肵�� UserGameInfo ���r�[�R���ɔ��f
			//   �X�V���ꂽ�r�[�R�������M�����܂Œ����ԃu���b�N���܂��B
			(void)MPUpdateBeacon();
			break;

		default:
			(void)WaitForStateChange();
			break;
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////
// �C�x���g�ʒm�p�R�[���o�b�N
void DataSharingCallback( s32 type, MPPortCallbackInfo* info )
{
	// MPDS ���C�u������ MP ���C�u��������̒ʒm�� forward ���܂��B
	// ���̃f���ł͐ڑ����Ă����q�@�̏ڍׂȏ����擾���邽�߂�
	// �R�[���o�b�N�𗘗p���Ă��܂����ADataSharing �𗘗p���邾���ł����
	// �K�������R�[���o�b�N�֐���p�ӂ���K�v�͂���܂���B

	// ���̊֐��́AMP ���C�u�����̃R�[���o�b�N�p�X���b�h����Ăяo����܂��B
	// ���荞�݃R�[���o�b�N�ł͂Ȃ����߁A�X���b�h�����p�̊֐����Ăяo�����Ƃ�
	// �\�ł����A���܂蒷���ԃu���b�N���Ă��܂��ƁAMP �ʐM�ɉe����^���܂��B
	// �r�����s�����߂̍Œ���̃u���b�N�ɂƂǂ߂Ă��������B
	// �ꍇ�ɂ���ẮAOSMessageQueue �Ȃǂ𗘗p�����A�u���b�N���Ȃ��悤��
	// �f�[�^�̎󂯓n�����@���������Ă��������B

	u32 aid;

	switch ( type )
	{
	case MP_PORT_CB_TYPE_CONNECTED:
		// �q�@���ڑ�����
		aid = info->connected.fromAid;
		OSReport("ConnectChild[%d]\n",aid);
		EnterCriticalSection();
		(void)NETMemCpy(sConnectedMacAddress[aid], info->connected.macAddress, MP_SIZE_MACADDRESS);
		(void)NETMemSet(sConnectedNickName[aid], 0, sizeof(sConnectedNickName[0]));
		/* DS ����̃f�[�^�̓G���f�B�A�����قȂ邱�Ƃɒ��� */
		NETSwapAndCopyMemory16(sConnectedNickName[aid], info->connected.ssidUserData, info->connected.ssidUserDataLength);
		sConnectedFlag[aid] = TRUE;
		{
			PLAYER_STATE	*plData = MPDS_GetPlayerState( aid );
			plData->isValid_ = FALSE;
		}
		LeaveCriticalSection();
		
		break;

	case MP_PORT_CB_TYPE_DISCONNECTED:
		// �q�@���ؒf����
		aid = info->disconnected.fromAid;

		EnterCriticalSection();
		sConnectedFlag[aid] = FALSE;
		(void)NETMemSet(sConnectedMacAddress[aid], 0, MP_SIZE_MACADDRESS);
		(void)NETMemSet(sConnectedNickName[aid], 0, sizeof(sConnectedNickName[0]));
		
		OSReport("DisConnectChild[%d]\n",aid);
		GFL_NET_InitHandle((int)aid);
		
		LeaveCriticalSection();

		break;
		
	case MP_PORT_CB_TYPE_STARTUP:
	case MP_PORT_CB_TYPE_CLEANUP:
	case MPDS_PORT_CB_TYPE_DATASET_RECEIVED:
	default:
		// �������邱�Ƃ͂���܂���
		break;
	}
}

void IndicationCallback( s32 type, MPIndicationInfo* info )
{
	// MP ���C�u�������瑗����e��̈�ʓI�Ȓʒm���󂯎�邽�߂̃R�[���o�b�N�ł��B
	// Fatal Error �̃n���h�����O�́A���C�u�����֐��̕Ԃ�l�̏����ōs�����Ƃ�
	// �ł��܂��̂ŁA�K�������R�[���o�b�N�֐���p�ӂ���K�v�͂���܂���B

	// ���̊֐��́AMP ���C�u�����̃R�[���o�b�N�p�X���b�h����Ăяo����܂��B
	// ���荞�݃R�[���o�b�N�ł͂Ȃ����߁A�X���b�h�����p�̊֐����Ăяo�����Ƃ�
	// �\�ł����A���܂蒷���ԃu���b�N���Ă��܂��ƁAMP �ʐM�ɉe����^���܂��B
	// �r�����s�����߂̍Œ���̃u���b�N�ɂƂǂ߂Ă��������B
	// �ꍇ�ɂ���ẮAOSMessageQueue �Ȃǂ𗘗p�����A�u���b�N���Ȃ��悤��
	// �f�[�^�̎󂯓n�����@���������Ă��������B

	switch ( type )
	{
	case MP_INDICATION_CB_TYPE_FATAL_ERROR:
		// �v���I�G���[�̔���

		// �������m�ۃG���[�ȂǁA�����ŕ����s�\�ȃG���[�����������ꍇ�ɒʒm����܂��B
		OSReport("Fatal Error occurred: error=%d\n", info->value);

		break;

	case MP_INDICATION_CB_TYPE_PSEUDO_VBLANK:
		// ���z VBlank �ʒm

		// MPConfig.mode �� MP_MODE_PARENT | MP_MODE_ENABLE_PSEUDO_VBLANK_INDICATION ��
		// �w�肷�邱�ƂŁA�q�@���������Ă��鉼�z VBlank �̃^�C�~���O�ł̒ʒm��
		// �󂯂邱�Ƃ��ł���悤�ɂȂ�܂��B
		// ���̃f���ł͎g�p���Ă��܂���B

		break;

	default:
		// �������邱�Ƃ͂���܂���
		break;
	}
}

/*===========================================================================*/

void* mpAlloc( u32 size )
{
	OSReport("allocated %d bytes\n", size);
	return MEMAllocFromExpHeapEx( sUserHeap, size, 32 );
}

void mpFree( void* ptr )
{
	MEMFreeToExpHeap( sUserHeap, ptr );
}

/*===========================================================================*/

// DS �� PAD �萔
#define DSPAD_BUTTON_A			0x0001 // A
#define DSPAD_BUTTON_B			0x0002 // B
#define DSPAD_BUTTON_SELECT	   0x0004 // SELECT
#define DSPAD_BUTTON_START		0x0008 // START
#define DSPAD_KEY_RIGHT		   0x0010 // RIGHT of cross key
#define DSPAD_KEY_LEFT			0x0020 // LEFT  of cross key
#define DSPAD_KEY_UP			  0x0040 // UP	of cross key
#define DSPAD_KEY_DOWN			0x0080 // DOWN  of cross key
#define DSPAD_BUTTON_R			0x0100 // R
#define DSPAD_BUTTON_L			0x0200 // L
#define DSPAD_BUTTON_X			0x0400 // X
#define DSPAD_BUTTON_Y			0x0800 // Y

u16 ConvKPadKeyToDSPad(u32 mixedKey)
{
	u16 key = 0;

	if ( mixedKey & (KPAD_BUTTON_LEFT | (PAD_BUTTON_LEFT << 16)) )
	{
		key |= DSPAD_KEY_LEFT;
	}
	if ( mixedKey & (KPAD_BUTTON_RIGHT | (PAD_BUTTON_RIGHT << 16)) )
	{
		key |= DSPAD_KEY_RIGHT;
	}
	if ( mixedKey & (KPAD_BUTTON_UP | (PAD_BUTTON_UP << 16)) )
	{
		key |= DSPAD_KEY_UP;
	}
	if ( mixedKey & (KPAD_BUTTON_DOWN | (PAD_BUTTON_DOWN << 16)) )
	{
		key |= DSPAD_KEY_DOWN;
	}
	if( mixedKey & (KPAD_BUTTON_A | (PAD_BUTTON_A << 16)) )
	{
		key |= DSPAD_BUTTON_A;
	}
	if( mixedKey & (KPAD_BUTTON_B | (PAD_BUTTON_B << 16)) )
	{
		key |= DSPAD_BUTTON_A;
	}
	if( mixedKey & (KPAD_BUTTON_1 | (PAD_BUTTON_X << 16)) )
	{
		key |= DSPAD_BUTTON_X;
	}
	if( mixedKey & (KPAD_BUTTON_2 | (PAD_BUTTON_Y << 16)) )
	{
		key |= DSPAD_BUTTON_Y;
	}
	if( mixedKey & (KPAD_BUTTON_PLUS | (PAD_TRIGGER_R << 16)) )
	{
		key |= DSPAD_BUTTON_R;
	}
	if( mixedKey & (KPAD_BUTTON_MINUS | (PAD_TRIGGER_L << 16)) )
	{
		key |= DSPAD_BUTTON_L;
	}
	if( mixedKey & (KPAD_BUTTON_HOME | (PAD_BUTTON_START << 16)) )
	{
		key |= DSPAD_BUTTON_START;
	}

	return key;
}
