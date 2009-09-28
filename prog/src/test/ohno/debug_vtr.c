



//=============================================================================
/**
 * @file	  debug_vtr.c
 * @brief	  ���̘^���ƍĐ�
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/04/30
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
#include "arc_def.h"

#include "system/main.h"  //HEAPID
#include "message.naix"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"
#include "sound/pm_sndsys.h"
#include "system/wipe.h"
#include "net/network_define.h"
#include "savedata/wifilist.h"
#include "msg\msg_d_ohno.h"
#include "sound/snd_strm.h"


// �}�C�N�Q�C����ݒ�
#define MY_AMPGAIN PM_AMPGAIN_160

int micrate[]={
	MIC_SAMPLING_RATE_8K,
	MIC_SAMPLING_RATE_11K,
	MIC_SAMPLING_RATE_16K,
	MIC_SAMPLING_RATE_22K,
	MIC_SAMPLING_RATE_32K};



static int _SAMPLING_RATE = MIC_SAMPLING_RATE_8K;
#define _SAMPLING_SIZE (0xb000)

typedef struct _VOICETR_WORK VOICETR_WORK;

typedef void (StateFunc)(VOICETR_WORK* pState);



static void _changeState(VOICETR_WORK* pWork,StateFunc* state);
static void _changeStateDebug(VOICETR_WORK* pWork,StateFunc* state, int line);

#define _NET_DEBUG (1)

#ifdef _NET_DEBUG
#define   _CHANGE_STATE(state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(pWork ,state)
#endif //_NET_DEBUG



struct _VOICETR_WORK {
	GFL_FONT		  *fontHandle;
	GFL_MSGDATA		*mm;
	GFL_BMPWIN		*win;
	GFL_BMP_DATA	*bmp;
	HEAPID heapID;
	STRBUF			*strbuf;
	STRBUF			*strbufEx;
  WORDSET			*WordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[

	SAVE_CONTROL_WORK* pSaveData;
	StateFunc* state;      ///< �n���h���̃v���O�������
	vu32 count;

	void* buffer;
	int bufferSize;
	int hz;
	int req;
	int getdataCount;
	BOOL bEnd;
	DWCGHTTPPost post;
	u8 mac[6];
//	u8 buffer[0x102];
};


static void _micKeyWait(VOICETR_WORK* pWork);




static GFLNetInitializeStruct aGFLNetInit = {
  NULL,  // ��M�֐��e�[�u��
  0, // ��M�e�[�u���v�f��
  NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
  NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
  NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  NULL,  // �r�[�R���f�[�^�擾�֐�
  NULL,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
  NULL,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
  NULL,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
  NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
  NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  NULL,   ///< DWC�`���̗F�B���X�g
  NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  GFL_NET_DWC_HEAPSIZE + 0x80000,   ///< DWC�ւ�HEAP�T�C�Y
  TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
  0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_NETWORK,  //IRC�p��create�����HEAPID
  GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
  1,     // �ő�ڑ��l��
  48,    // �ő呗�M�o�C�g��
  32,    // �ő�r�[�R�����W��
  TRUE,     // CRC�v�Z
  FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIFI,  //�ʐM���
  TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_WIFICLUB,  //GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
    0,//MP�e�ő�T�C�Y 512�܂�
    0,//dummy
};




static void _msgPrint(VOICETR_WORK* pWork,int msg)
{
	GFL_BMP_Clear( pWork->bmp, 0xff );
	GFL_MSG_GetString(pWork->mm, msg, pWork->strbuf);
	PRINTSYS_Print( pWork->bmp, 0, 0, pWork->strbuf, pWork->fontHandle);
	GFL_BMPWIN_TransVramCharacter( pWork->win );
}

static void _msgPrintNo(VOICETR_WORK* pWork,int msg,int no)
{
	GFL_BMP_Clear( pWork->bmp, 0xff );
	GFL_MSG_GetString(pWork->mm, msg, pWork->strbufEx);

	WORDSET_RegisterNumber(pWork->WordSet, 0, no,
												 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
	WORDSET_ExpandStr( pWork->WordSet, pWork->strbuf,pWork->strbufEx );


	PRINTSYS_Print( pWork->bmp, 0, 0, pWork->strbuf, pWork->fontHandle);
	GFL_BMPWIN_TransVramCharacter( pWork->win );
}


//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(VOICETR_WORK* pWork,StateFunc state)
{
	pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef _NET_DEBUG
static void _changeStateDebug(VOICETR_WORK* pWork,StateFunc state, int line)
{
#ifdef DEBUG_ONLY_FOR_ohno
	OS_TPrintf("ghttp: %d\n",line);
#endif
	_changeState(pWork, state);
}
#endif




//--------------------------------------------------------------
/**
 * @brief	�y���b�v�̘^�������f�[�^���Đ�
 *
 * @param	perap	PERAPVOICE�^�̃|�C���^
 * @param	sex		����
 * @param	vol		�{�����[��
 * @param	pan		�p��
 *
 * @retval	"�Đ�����=TRUE�A���s=FALSE"
 */
//--------------------------------------------------------------
#if 1
#define WAVEOUT_CH_NORMAL	(14)							//�g�`�Ŏg�p����`�����l��NO
#define WAVEOUT_PLAY_SPDx1 (32768)


//�g�`�f�[�^�Đ��ɕK�v�ȃf�[�^�\����
typedef struct{
	NNSSndWaveOutHandle	handle;					//�g�`�Đ��n���h��
	NNSSndWaveFormat		format;					//�g�`�f�[�^�t�H�[�}�b�g
	const void*				dataaddr;				//�g�`�f�[�^�̐擪�A�h���X
	BOOL					loopFlag;				//���[�v�t���O
	int						loopStartSample;		//���[�v�J�n�T���v���ʒu
	int						samples;				//�g�`�f�[�^�̃T���v����
	int						sampleRate;				//�g�`�f�[�^�̃T���v�����O���[�g
	int						volume;					//����
	int						speed;					//�Đ��X�s�[�h
	int						pan;					//�p��(0-127)
}WAVEOUT_WORK;

static BOOL Snd_PerapVoicePlaySub( const void* perap, int vol, int pan )
{
	u16 add_spd;
	int ret,wave_pan;
	NNSSndWaveOutHandle handle;
//	u8* perap_play_flag		= Snd_GetParamAdrs( SND_W_ID_PERAP_PLAY_FLAG );


	//�g�`�Đ��p�`�����l�����m�ۂ���
	handle = NNS_SndWaveOutAllocChannel( WAVEOUT_CH_NORMAL );

	//�����_���ɉ�����ς���
	//gf_srand( sys.vsync_counter );
	//add_spd = ( gf_rand() % PERAP_WAVEOUT_SPD_RAND );


	{
		WAVEOUT_WORK waveout_wk;
		WAVEOUT_WORK* p;
		
		waveout_wk.handle			= handle;	//�g�`�Đ��n���h��
		waveout_wk.format			= NNS_SND_WAVE_FORMAT_PCM8;		//�g�`�f�[�^�t�H�[�}�b�g
		waveout_wk.dataaddr			= perap;			//�g�`�f�[�^�̐擪�A�h���X
		waveout_wk.loopFlag			= FALSE;						//���[�v�t���O
		waveout_wk.loopStartSample	= 0;							//���[�v�J�n�T���v���ʒu
		waveout_wk.samples			= _SAMPLING_SIZE;			//�g�`�f�[�^�̃T���v����
		waveout_wk.sampleRate		= _SAMPLING_RATE;			//�g�`�f�[�^�̃T���v�����O���[�g
		waveout_wk.volume			= vol;							//����
		waveout_wk.speed			= WAVEOUT_PLAY_SPDx1;	//�Đ��X�s�[�h
		waveout_wk.pan				= 64;						//�p��(0-127)
//		ret = Snd_WaveOutStart( &waveout_wk, WAVEOUT_CH_NORMAL );

		NNS_SndWaveOutSetVolume( handle, 127 );
		p = &waveout_wk;
		ret = NNS_SndWaveOutStart( p->handle, p->format, p->dataaddr, p->loopFlag, p->loopStartSample,
								p->samples, p->sampleRate, p->volume, p->speed, p->pan );

	}


	return ret;
}

#endif


static  void _endCallback(MICResult	result, void*	arg )
{
	VOICETR_WORK* pWork=arg;
	vu32 ans = OS_GetVBlankCount() - pWork->count;

	OS_TPrintf("�T���v�����O����%d %d\n",result, ans/60);
	_CHANGE_STATE(_micKeyWait);
}


//--------------------------------------------------------------
/**
 * @brief	�^���J�n
 * @param	none
 * @retval	"MIC_RESULT_SUCCESS		����������Ɋ���"
 * @retval	"����ȊO				���炩�̌����Ŏ��s"
 */
//--------------------------------------------------------------
static MICResult Snd_PerapVoiceRecStart( VOICETR_WORK* pWork )
{
	MICAutoParam mic;	//�}�C�N�p�����[�^
	MICResult ret;


	mic.type			= MIC_SAMPLING_TYPE_SIGNED_8BIT;	//�T���v�����O���

	//�o�b�t�@��32�o�C�g�A���C�����ꂽ�A�h���X�łȂ��ƃ_���I
	mic.size = pWork->bufferSize;
	mic.buffer = pWork->buffer;


	if( (mic.size&0x1f) != 0 ){
		mic.size &= 0xffffffe0;
	}

	//��\�I�ȃT���v�����O���[�g��ARM7�̃^�C�}�[�����Ɋ��Z�����l�̒�`
	//mic.rate			= MIC_SAMPLING_RATE_8K;
	mic.rate			= _SAMPLING_RATE;
//	mic.rate			= HW_CPU_CLOCK_ARM7 / PERAP_SAMPLING_RATE;

	//�A���T���v�����O���Ƀo�b�t�@�����[�v������t���O
	mic.loop_enable		= FALSE;

	//�o�b�t�@���O�a�����ۂɌĂяo���R�[���o�b�N�֐��ւ̃|�C���^
	mic.full_callback	= _endCallback;

	//�o�b�t�@���O�a�����ۂɌĂяo���R�[���o�b�N�֐��֓n������
	mic.full_arg		= pWork;

	pWork->count = OS_GetVBlankCount();

	
	ret = MIC_StartAutoSampling( &mic );

	return ret;
}


//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _noneState(VOICETR_WORK* pWork)
{

}

static void _cancelState(VOICETR_WORK* pWork)
{
	switch(GFL_UI_KEY_GetTrg())
	{
	case PAD_BUTTON_Y:
		SND_STRM_Stop();
		_CHANGE_STATE(_micKeyWait);
		break;
	}
}



//------------------------------------------------------------------------------
/**
 * @brief   �L�[���͂œ�����ς���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _micKeyWait(VOICETR_WORK* pWork)
{
	switch(GFL_UI_KEY_GetTrg())
	{
	case PAD_BUTTON_X:
		Snd_PerapVoiceRecStart(pWork);
		_msgPrint(pWork, DEBUG_OHNO_MSG0006);
		_CHANGE_STATE(_noneState);
		break;
	case PAD_BUTTON_Y:
//		Snd_PerapVoicePlaySub( pWork->buffer, 127, 0 );
		SND_STRM_SetUpStraightData( SND_STRM_PCM8, pWork->hz,
																 GFL_HEAPID_APP, pWork->buffer,pWork->bufferSize);
		SND_STRM_Play();
		OS_TPrintf("�Đ��J�n\n");
		_CHANGE_STATE(_cancelState);
		break;
	case PAD_BUTTON_L:
		pWork->hz++;
		if(pWork->hz >= SND_STRM_HZMAX){
			pWork->hz = 0;
		}
		OS_TPrintf("%d �ɂȂ���\n",pWork->hz);
		_SAMPLING_RATE = micrate[pWork->hz];
		break;
	case PAD_BUTTON_B:
		_CHANGE_STATE(NULL);
		break;
	}
}

static void _mic_init(VOICETR_WORK* pWork)
{
	MIC_Init();
	PM_Init();

	{
		u32 ret;
		ret = PM_SetAmp(PM_AMP_ON);
		if( ret == PM_RESULT_SUCCESS )
		{
			OS_TPrintf("AMP���I���ɂ��܂����B\n");
		}
		else
		{
			OS_TPrintf("AMP�̏������Ɏ��s�i%d�j", ret);
		}
	}

	{
		u32 ret;
		ret = PM_SetAmpGain(MY_AMPGAIN);
		if( ret == PM_RESULT_SUCCESS )
		{
			OS_TPrintf("AMP�̃Q�C����ݒ肵�܂����B\n");
		}
		else
		{
			OS_TPrintf("AMP�̃Q�C���ݒ�Ɏ��s�i%d�j", ret);
		}
	}
	pWork->buffer = GFL_NET_Align32Alloc(GFL_HEAPID_APP ,_SAMPLING_SIZE);
	pWork->bufferSize = _SAMPLING_SIZE;
	
	_CHANGE_STATE(_micKeyWait);

}




static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_0_F,			// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_0123_E,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_32K,	// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};


static GFL_PROC_RESULT VTRProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PROC, 0x70000 );//�e�X�g

	{
		VOICETR_WORK* pWork = GFL_PROC_AllocWork( proc, sizeof(VOICETR_WORK), HEAPID_PROC );

		pWork->pSaveData = SaveControl_GetPointer();  //�f�o�b�O
		pWork->heapID = HEAPID_PROC;

		GFL_DISP_SetBank( &vramBank );

		//�o�b�N�O���E���h�̐F�����Ă���
		GFL_STD_MemFill16((void*)HW_BG_PLTT, 0x5ad6, 2);
	
		// �e����ʃ��W�X�^������
		G2_BlendNone();

		// BGsystem������
		GFL_BG_Init( pWork->heapID );
		GFL_BMPWIN_Init( pWork->heapID );
		GFL_FONTSYS_Init();

			//�a�f���[�h�ݒ�
	{
		static const GFL_BG_SYS_HEADER sysHeader = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysHeader );
	}

	// �ʃt���[���ݒ�
	{
		static const GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x5800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};

		GFL_BG_SetBGControl( GFL_BG_FRAME0_M,   &bgcntText,   GFL_BG_MODE_TEXT );
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S,   &bgcntText,   GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_OFF );

//		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0x22, wk->heapID );
		GFL_BG_FillCharacter( GFL_BG_FRAME0_M, 0xff, 1, 0 );

//		void GFL_BG_FillScreen( u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode )
		GFL_BG_FillScreen( GFL_BG_FRAME0_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

		GFL_BG_FillCharacter( GFL_BG_FRAME0_S, 0x00, 1, 0 );
		GFL_BG_FillScreen( GFL_BG_FRAME0_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
	}

	// �㉺��ʐݒ�
		GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

		pWork->win = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 4, 12, 24, 16, 0, GFL_BMP_CHRAREA_GET_F );
		pWork->bmp = GFL_BMPWIN_GetBmp( pWork->win);
		GFL_BMP_Clear( pWork->bmp, 0xff );
		GFL_BMPWIN_MakeScreen( pWork->win );
		GFL_BMPWIN_TransVramCharacter( pWork->win );

		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

		pWork->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
																			GFL_FONT_LOADTYPE_FILE, FALSE, pWork->heapID );

		pWork->mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_ohno_dat, pWork->heapID );
		pWork->strbuf = GFL_STR_CreateBuffer(64, pWork->heapID);
		pWork->strbufEx = GFL_STR_CreateBuffer(64, pWork->heapID);
		pWork->WordSet    = WORDSET_Create( pWork->heapID );

	//�t�H���g�p���b�g�]��
		GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
																	0, 0x20, HEAPID_PROC);

		
		_CHANGE_STATE(_mic_init);
	}
  return GFL_PROC_RES_FINISH;
}
static GFL_PROC_RESULT VTRProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	VOICETR_WORK* pWork = mywk;
	StateFunc* state = pWork->state;

	if( state ){
		state( pWork );
		return GFL_PROC_RES_CONTINUE;
	}
	return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT VTRProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	VOICETR_WORK* pWork = mywk;

	GFL_STR_DeleteBuffer(pWork->strbuf);
	GFL_STR_DeleteBuffer(pWork->strbufEx);
	GFL_MSG_Delete(pWork->mm);
	GFL_FONT_Delete(pWork->fontHandle);

	WORDSET_Delete( pWork->WordSet );

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_PROC);  //�e�X�g
  return GFL_PROC_RES_FINISH;
}

// �v���Z�X��`�f�[�^
const GFL_PROC_DATA VTRProcData = {
  VTRProc_Init,
  VTRProc_Main,
  VTRProc_End,
};

