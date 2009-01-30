//==============================================================================
/**
 * @file	huge_beacon.c
 * @brief	�r�[�R�����g�p���ċ���f�[�^�𑗎�M
 * @author	matsuda
 * @date	2009.01.23(��)
 */
//==============================================================================
#include <gflib.h>
#include "huge_beacon.h"


//==============================================================================
//	�萔��`
//==============================================================================
///�r�[�R���ň�x�ɑ��郆�[�U�[�f�[�^�T�C�Y
#define HB_SEND_DATA_SIZE		(64)

///�q�@���[�h�ł����鎞��
#define HB_CHILD_MODE_TIME		(60 * 2)
///�q�@���[�h�Ń��b�N�I�����Ă���e�@����̃^�C���A�E�g����
#define HB_ROCKON_TIMEOUT		(60 * 9)

///�e�@���[�h�ł����鎞��
#define HB_PARENT_MODE_TIME		(60 * 6)
///�e�@���[�h�œ����f�[�^�����񑗐M�����玟�̃f�[�^�֐؂�ւ��邩
#define HB_PARENT_SEND_LOOP		(10)

//--------------------------------------------------------------
//	�f�o�b�O��`
//--------------------------------------------------------------
///�`�����l�����Œ肵�ăr�[�R����z�M����
#define CHANNEL_KOTEI		(1)	//�P���Œ�
///�e�@�����������ScanEx�ł͂Ȃ��AScan�ŃT�[�`����
#define SCAN_USE			(0)	//1=Scan���g��
///�e�q���Œ肷�郂�[�h
#define PARENTCHILD_KOTEI	(0)	//1=�Œ�

//==============================================================================
//	�\���̒�`
//==============================================================================
///���M�f�[�^�̃w�b�_
typedef struct{
	u16 data_no;		///<�f�[�^�ԍ�
	u8 size;			///<���M����Ă������[�U�[�f�[�^�T�C�Y(�w�b�_�͊܂܂Ȃ�)
	u8 padding;
}HB_USER_HEADER;

///���M�f�[�^
typedef struct{
	HB_USER_HEADER head;
	u8 data[HB_SEND_DATA_SIZE];
}HB_SEND_DATA;

///�q�@���[�h���̓���p���[�N
typedef struct{
	BOOL parent_lockon;			///<TRUE:���̐e�@�Ƀ��b�N�I�����Ă���(��M��p���[�h�œ���)
	BOOL parent_hit;			///<TRUE:�e�@�����Ńq�b�g����
	BOOL receive_finish;		///<TRUE:�S�Ă̎�M������
	u8 parent_macAddress[6];	///<�e��MAC�A�h���X
	u8 parent_link_level;		///<���b�N�I�����Ă���e�̓d�g���x��
	u8 padding;
	u16 errcode;
	WMStartScanExCallback wsec;
	WMStartScanCallback wsc;
	WMScanExParam scan_ex_param;
	WMScanParam scan_param;
}HB_CHILD_MODE;

///�e�@���[�h���̓���p���[�N
typedef struct{
	WMMeasureChannelCallback mcc;
	WMParentParam ppara;
	WMStartParentCallback wpc;
	u16 errcode;
	u16 send_loop;
	s16 add_data_count;
	u16 padding;
}HB_PARENT_MODE;

///����r�[�R������M�Ǘ����[�N
typedef struct{
	int heap_id;
	u16 seq;
	u16 tgid;

	HB_CHILD_MODE cm;
	HB_PARENT_MODE pm;
	
	BOOL scan_callback;
	int timeout;
	u8 scan_buf[WM_SIZE_SCAN_EX_BUF] ATTRIBUTE_ALIGN(32);
	HB_SEND_DATA beacon_send_data;
	
	u32 *receive_bit;			///<��M�����f�[�^�ԍ���bit�Ǘ�
	const u8 *send_data;		///<���M�f�[�^�ւ̃|�C���^
	u8 *receive_buf;			///<��M�f�[�^����惏�[�N�ւ̃|�C���^
	u32 send_data_size;			///<���M�f�[�^�T�C�Y
	u16 data_no_max;			///<�����f�[�^��
	u16 receive_data_count;		///<��M�����f�[�^�̐�
	
	u16 parent_channel;
	u16 use_channel_list;
	u16 use_channel_level;
	u16 use_channel;

#if PARENTCHILD_KOTEI
	BOOL pc_kotei;		///<TRUE:�e�ŌŒ�@FALSE:�q�ŌŒ�
#endif
}HUGEBEACON_SYS;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static int _HUGEBEACON_Wait(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ChildStartInit(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ChildStart(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ChildStartWait(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ChildEnd(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ChildEndWait(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ParentStartInit(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ParentStartInitWait(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ParentStart(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ParentStartWait(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ParentEnd(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ParentEndWait(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ParentSendChange(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_ParentSendChangeWait(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_UseChannelSelectInit(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_UseChannelSelectMain(HUGEBEACON_SYS *hb);
static int _HUGEBEACON_UseChannelSelectWait(HUGEBEACON_SYS *hb);
static void _Callback_ChildScan( void* arg );
static void _Callback_ChildEndScan(void *arg);
static void _Callback_MeasureChannel(void *arg);
static void _Callback_ParentParam(void *arg);
static void _Callback_StartParent(void *arg);
static void _Callback_EndParent(void *arg);
static void _Callback_ParentSendChange(void *arg);
static void _HB_TOOL_SendDataMake(HUGEBEACON_SYS *hb);
static void _HB_TOOL_ReceiveDataSet(HUGEBEACON_SYS *hb, const void *receive_data);


//==============================================================================
//	�V�[�P���X�e�[�u��
//==============================================================================
///����r�[�R������M�p�V�[�P���X�e�[�u���̊֐��^
typedef int (*HUGEBEACON_FUNC)(HUGEBEACON_SYS *);

///����r�[�R������M�F�V�[�P���X�e�[�u��
static const HUGEBEACON_FUNC HugeBeaconSeqTbl[] = {
	_HUGEBEACON_Wait,
	_HUGEBEACON_ChildStartInit,
	_HUGEBEACON_ChildStart,
	_HUGEBEACON_ChildStartWait,
	_HUGEBEACON_ChildEnd,
	_HUGEBEACON_ChildEndWait,
	_HUGEBEACON_UseChannelSelectInit,
	_HUGEBEACON_UseChannelSelectMain,
	_HUGEBEACON_UseChannelSelectWait,
	_HUGEBEACON_ParentStartInit,
	_HUGEBEACON_ParentStartInitWait,
	_HUGEBEACON_ParentStart,
	_HUGEBEACON_ParentStartWait,
	_HUGEBEACON_ParentEnd,
	_HUGEBEACON_ParentEndWait,
	_HUGEBEACON_ParentSendChange,
	_HUGEBEACON_ParentSendChangeWait,
};

//HugeBeaconSeqTbl�ƕ��т𓯂��ɂ��Ă������ƁI
enum{
	HBSEQ_WAIT,
	HBSEQ_CHILD_START_INIT,
	HBSEQ_CHILD_START,
	HBSEQ_CHILD_START_WAIT,
	HBSEQ_CHILD_END,
	HBSEQ_CHILD_END_WAIT,
	HBSEQ_USE_CHANNEL_SELECT_INIT,
	HBSEQ_USE_CHANNEL_SELECT_MAIN,
	HBSEQ_USE_CHANNEL_SELECT_WAIT,
	HBSEQ_PARENT_START_INIT,
	HBSEQ_PARENT_START_INIT_WAIT,
	HBSEQ_PARENT_START,
	HBSEQ_PARENT_START_WAIT,
	HBSEQ_PARENT_END,
	HBSEQ_PARENT_END_WAIT,
	HBSEQ_PARENT_SEND_CHANGE,
	HBSEQ_PARENT_SEND_CHANGE_WAIT,
	
	HBSEQ_CONTINUE = -1,
	HBSEQ_END = -2,
	HBSEQ_ERROR = -3,
};


//==============================================================================
//	���[�J���ϐ�
//==============================================================================
///�R�[���o�b�N�֐����Ŏg�p����ׁA�O���[�o����
static HUGEBEACON_SYS *HbSys = NULL;


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ����r�[�R������M�V�X�e�����쐬
 *
 * @param   heap_id				�q�[�vID
 * @param   send_data_size		���M����f�[�^�̃T�C�Y
 * @param   send_data			���M����f�[�^�ւ̃|�C���^
 * @param   receive_buf			��M�f�[�^��������o�b�t�@�ւ̃|�C���^
 *
 * @retval  ����r�[�R������M�V�X�e�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void HUGEBEACON_SystemCreate(int heap_id, u32 send_data_size, const void *send_data, void *receive_buf)
{
	u32 bit_num;		//�K�v�ȊǗ�bit��
	u32 bit_array;		//�Ǘ�bit�̃��[�N��
	
	GF_ASSERT(HbSys == NULL);
	
	bit_num = send_data_size / HB_SEND_DATA_SIZE;
	if(send_data_size % HB_SEND_DATA_SIZE){
		bit_num++;
	}
	bit_array = bit_num / sizeof(u32);
	if(bit_num % sizeof(u32)){
		bit_array++;
	}
	
	HbSys = GFL_HEAP_AllocClearMemory(heap_id, sizeof(HUGEBEACON_SYS));
	HbSys->heap_id = heap_id;
	HbSys->receive_bit = GFL_HEAP_AllocClearMemory(heap_id, bit_array * sizeof(u32));
	HbSys->send_data_size = send_data_size;
	HbSys->send_data = send_data;
	HbSys->receive_buf = receive_buf;
	HbSys->data_no_max = bit_num;
	HbSys->seq = HBSEQ_WAIT;

#if PARENTCHILD_KOTEI
	if(GFL_UI_KEY_GetCont() & PAD_BUTTON_L){
		OS_TPrintf("�e�q�F�Œ胂�[�h�F�����Ɛe\n");
		HbSys->pc_kotei = TRUE;
	}
	else{
		OS_TPrintf("�e�q�F�Œ胂�[�h�F�����Ǝq\n");
		HbSys->pc_kotei = FALSE;
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ����r�[�R������M�V�X�e�����I��
 */
//--------------------------------------------------------------
void HUGEBEACON_SystemExit(void)
{
	GF_ASSERT(HbSys != NULL);
	
	GFL_HEAP_FreeMemory(HbSys->receive_bit);
	GFL_HEAP_FreeMemory(HbSys);
	HbSys = NULL;
}

//--------------------------------------------------------------
/**
 * @brief   ����r�[�R������M�V�X�e���̓���J�n
 */
//--------------------------------------------------------------
void HUGEBEACON_Start(void)
{
	GF_ASSERT(HbSys != NULL);
	HbSys->seq = HBSEQ_CHILD_START_INIT;
#if PARENTCHILD_KOTEI
	if(HbSys->pc_kotei == TRUE){
		HbSys->seq = HBSEQ_PARENT_START_INIT;
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ����r�[�R������M�V�X�e���̃��C��
 */
//--------------------------------------------------------------
void HUGEBEACON_Main(void)
{
	HUGEBEACON_SYS *hb = HbSys;
	int ret;
	
	GF_ASSERT(HbSys != NULL);
	if(hb->seq == HBSEQ_END || hb->seq == HBSEQ_ERROR){
		return;
	}
	
	ret = HugeBeaconSeqTbl[hb->seq](hb);
	switch(ret){
	case HBSEQ_CONTINUE:
		break;
	case HBSEQ_ERROR:
		GF_ASSERT(0);	//�Ƃ肠�������̓A�T�[�g�Œ�~ ��check
		break;	//�G���[�����֗���
	default:
		hb->seq = ret;
		break;
	}
}


//==============================================================================
//	�V�[�P���X
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�ҋ@
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_Wait(HUGEBEACON_SYS *hb)
{
	return HBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�q�@�Ƃ��ē���J�n(���[�N�̏�����)
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ChildStartInit(HUGEBEACON_SYS *hb)
{
	hb->scan_callback = 0;
	hb->cm.parent_link_level = 0;
	hb->cm.parent_lockon = 0;
	hb->cm.parent_hit = 0;
	hb->cm.receive_finish = 0;
	hb->timeout = 0;
	return HBSEQ_CHILD_START;
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�q�@�Ƃ��ē���J�n
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ChildStart(HUGEBEACON_SYS *hb)
{
	WMErrCode err_code;
	u16 channel_list;
	int i;
	
	channel_list = WM_GetAllowedChannel();
	if(channel_list == 0){
		OS_TPrintf("�����ʐM���Ă͂����Ȃ�\n");
		return HBSEQ_ERROR;
	}
	
	GF_ASSERT(WM_SIZE_SCAN_EX_BUF == sizeof(hb->scan_buf));	//�f�o�b�O�p�`�F�b�N�@��ŏ���

	hb->scan_callback = 0;
	
#if SCAN_USE
	if(hb->cm.parent_lockon == FALSE){
#else
	if(1){
#endif
		GFL_STD_MemClear(&hb->cm.scan_ex_param, sizeof(WMScanExParam));
		hb->cm.scan_ex_param.scanBuf = (void*)hb->scan_buf;
		hb->cm.scan_ex_param.scanBufSize = sizeof(hb->scan_buf);
		hb->cm.scan_ex_param.maxChannelTime = WM_GetDispersionScanPeriod();
		if(hb->cm.parent_lockon == FALSE){
			hb->cm.scan_ex_param.channelList = channel_list;
			for(i = 0; i < 6; i++){
				hb->cm.scan_ex_param.bssid[i] = 0xff;
			}
		}
		else{
		#if CHANNEL_KOTEI
			GF_ASSERT(hb->parent_channel != 0);
			hb->cm.scan_ex_param.channelList = 1 << (hb->parent_channel - 1);
		#else	//�e�q��؂�ւ��ĒʐM���Ă���Ɛe�̃`�����l�����ς���Ă����̂�
			hb->cm.scan_ex_param.channelList = channel_list;
		#endif
			OS_TPrintf("�q�@�F�e�@�w��ŒT���ɍs���F");
			for(i = 0; i < 6; i++){
				hb->cm.scan_ex_param.bssid[i] = hb->cm.parent_macAddress[i];
				OS_TPrintf("%02x:", hb->cm.scan_ex_param.bssid[i]);
			}
			OS_TPrintf("\n");
		}
		hb->cm.scan_ex_param.scanType = WM_SCANTYPE_PASSIVE;

		err_code = WM_StartScanEx(_Callback_ChildScan, &hb->cm.scan_ex_param );
		if(err_code != WM_ERRCODE_OPERATING){
			OS_TPrintf("StartScanEx�Ɏ��s�@�G���[:%d\n", err_code);
			return HBSEQ_ERROR;
		}
	}
	else{
		GFL_STD_MemClear(&hb->cm.scan_param, sizeof(WMScanParam));
		hb->cm.scan_param.scanBuf = (void*)hb->scan_buf;
		hb->cm.scan_param.maxChannelTime = WM_GetDispersionScanPeriod();
		hb->cm.scan_param.channel = hb->parent_channel;
		if(hb->cm.parent_lockon == FALSE){
			for(i = 0; i < 6; i++){
				hb->cm.scan_param.bssid[i] = 0xff;
			}
		}
		else{
			GF_ASSERT(hb->parent_channel != 0);
			OS_TPrintf("�q�@�F�e�@�w��ŒT���ɍs���F");
			for(i = 0; i < 6; i++){
				hb->cm.scan_param.bssid[i] = hb->cm.parent_macAddress[i];
				OS_TPrintf("%02x:", hb->cm.scan_param.bssid[i]);
			}
			OS_TPrintf("\n");
		}

		err_code = WM_StartScan(_Callback_ChildScan, &hb->cm.scan_param );
		if(err_code != WM_ERRCODE_OPERATING){
			OS_TPrintf("StartScan�Ɏ��s�@�G���[:%d\n", err_code);
			return HBSEQ_ERROR;
		}
	}
	
	return HBSEQ_CHILD_START_WAIT;
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�q�@����R�[���o�b�N�҂�
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ChildStartWait(HUGEBEACON_SYS *hb)
{
	int i;
	BOOL before_lockon;
	u16 state, errcode;
	
	hb->timeout++;
	if(hb->scan_callback == 0){
		return HBSEQ_CONTINUE;	//�R�[���o�b�N����������܂ő҂�
	}

	hb->cm.parent_hit = 0;
	before_lockon = hb->cm.parent_lockon;
	
#if SCAN_USE
	if(before_lockon == FALSE){
#else
	if(1){
#endif
		state = hb->cm.wsec.state;
		errcode = hb->cm.wsec.errcode;
	}
	else{
		state = hb->cm.wsc.state;
		errcode = hb->cm.wsc.errcode;
	}
	
	if(state == WM_STATECODE_PARENT_FOUND){
		//���ȏ�̐e�@����������
		switch(errcode){
		case WM_ERRCODE_SUCCESS:
		#if SCAN_USE
			if(before_lockon == FALSE){
		#else
			if(1){
		#endif
				OS_TPrintf("�q�@�F���������e�@�̐� = %d\n", hb->cm.wsec.bssDescCount);
				if ( hb->cm.wsec.bssDescCount > 0 ){
					// BssDesc�̏��ARM7�����珑�����܂�Ă��邽��
					// �o�b�t�@�ɐݒ肳�ꂽBssDesc�̃L���b�V����j��
					DC_InvalidateRange(hb->scan_buf, sizeof(hb->scan_buf));
				}

				for ( i = 0; i < hb->cm.wsec.bssDescCount; i++ ){
					WMBssDesc* bd = hb->cm.wsec.bssDesc[i];
					
					OS_TPrintf("length = %d, WMBssDesc�̃T�C�Y=%d\n", bd->length, sizeof(WMBssDesc));	//��check
					//GF_ASSERT(sizeof(WMBssDesc) == bd->length);
					
					OS_TPrintf(" Ch%2d %02x:%02x:%02x:%02x:%02x:%02x ",
							   bd->channel,
							   bd->bssid[0], bd->bssid[1], bd->bssid[2],
							   bd->bssid[3], bd->bssid[4], bd->bssid[5]);

					// �܂��AWMBssDesc.gameInfoLength ���m�F���A
					// ggid �ɗL���Ȓl�������Ă��邱�Ƃ��璲�ׂ�K�v������܂��B
					if (WM_IsValidGameInfo(&bd->gameInfo, bd->gameInfoLength)){
						// DS �e�@
						OS_TPrintf("GGID=%08x TGID=%04x\n", bd->gameInfo.ggid, bd->gameInfo.tgid);
						if (bd->gameInfo.ggid != GGID_HUGE_DATA){
							// GGID������Ă���Ζ�������
							OS_TPrintf("GGID���Ⴄ\n");
							continue;
						}
					}
					else{
						OS_TPrintf("�L���łȂ��e�@ %d\n", i);
						continue;
					}

					// �������r�[�R���̏����L�^
					if(before_lockon == TRUE){
						//�ꉞ������MacAddress����v���Ă��邩�m�F
						for(i = 0; i < 6; i++){
							if(hb->cm.parent_macAddress[i] != bd->bssid[i]){
								break;
							}
						}
						if(i < 6){
							OS_TPrintf("���b�N�I�����Ă���MacAddress�ƕs��v\n");
							continue;
						}
						
						hb->cm.parent_hit = TRUE;
						//���[�U�[���[�N��ǂݎ��
						_HB_TOOL_ReceiveDataSet(hb, bd->gameInfo.userGameInfo);
					}
					else{
						//�d�g�󋵂��ǂ��̂�D��
						if(hb->cm.parent_link_level <= hb->cm.wsec.linkLevel[i]){
							OS_TPrintf("�q�@�F�ڑ���̐e�@���X�V\n");
							hb->cm.parent_link_level = hb->cm.wsec.linkLevel[i];
							hb->cm.parent_lockon = TRUE;
							hb->cm.parent_hit = TRUE;
							for(i = 0; i < 6; i++){
								hb->cm.parent_macAddress[i] = bd->bssid[i];
							}
							hb->parent_channel = bd->channel;
							//���[�U�[���[�N��ǂݎ��
							_HB_TOOL_ReceiveDataSet(hb, bd->gameInfo.userGameInfo);
						}
					}
				}
			}
			else{
				WMStartScanCallback *wsc = &hb->cm.wsc;
				
				OS_TPrintf("�q�@�FScanBuf�Ŕ���\n");

				// BssDesc�̏��ARM7�����珑�����܂�Ă��邽��
				// �o�b�t�@�ɐݒ肳�ꂽBssDesc�̃L���b�V����j��
				DC_InvalidateRange(hb->scan_buf, sizeof(hb->scan_buf));

				OS_TPrintf(" Ch%2d %02x:%02x:%02x:%02x:%02x:%02x ",
						   wsc->channel,
						   wsc->macAddress[0], wsc->macAddress[1], wsc->macAddress[2],
						   wsc->macAddress[3], wsc->macAddress[4], wsc->macAddress[5]);

				// �܂��AWMBssDesc.gameInfoLength ���m�F���A
				// ggid �ɗL���Ȓl�������Ă��邱�Ƃ��璲�ׂ�K�v������܂��B
				if (WM_IsValidGameInfo(&wsc->gameInfo, wsc->gameInfoLength)){
					// DS �e�@
					OS_TPrintf("GGID=%08x TGID=%04x\n", wsc->gameInfo.ggid, wsc->gameInfo.tgid);
					if (wsc->gameInfo.ggid != GGID_HUGE_DATA){
						// GGID������Ă���Ζ�������
						OS_TPrintf("GGID���Ⴄ\n");
						break;
					}
				}
				else{
					OS_TPrintf("�L���łȂ��e�@\n");
					break;
				}

				//�ꉞ������MacAddress����v���Ă��邩�m�F
				for(i = 0; i < 6; i++){
					if(hb->cm.parent_macAddress[i] != wsc->macAddress[i]){
						break;
					}
				}
				if(i < 6){
					OS_TPrintf("���b�N�I�����Ă���MacAddress�ƕs��v\n");
					break;
				}
				
				hb->cm.parent_hit = TRUE;
				//���[�U�[���[�N��ǂݎ��
				_HB_TOOL_ReceiveDataSet(hb, wsc->gameInfo.userGameInfo);
			}
			break;
		default:	//�ꉞ(�}�j���A����ł͏�L�G���[�R�[�h�ȊO�͔������Ȃ�)
			break;
		}
	}
	else{	//WM_STATECODE_PARENT_NOT_FOUND
		//�e�@��������Ȃ�����
		switch(errcode){
		case WM_ERRCODE_SUCCESS:
		case WM_ERRCODE_FAILED:
		case WM_ERRCODE_ILLEGAL_STATE:
		case WM_ERRCODE_INVALID_PARAM:
		case WM_ERRCODE_FIFO_ERROR:
		default:	//�ꉞ(�}�j���A����ł͏�L�G���[�R�[�h�ȊO�͔������Ȃ�)
			OS_TPrintf("�q�@�F�e�@��������Ȃ�\n");
			break;
		}
	}

	
	if(hb->cm.parent_hit == TRUE){	//�e�@���������Ă���
		hb->timeout = 0;
		if(hb->cm.receive_finish == TRUE){
			return HBSEQ_CHILD_END;
		}
	}
	else{
		int max_timeout;
	
	#if PARENTCHILD_KOTEI
		hb->timeout = 0;	//�Œ胂�[�h�ׁ̈A�^�C���A�E�g�����Ȃ�
	#endif
		max_timeout = hb->cm.parent_lockon ? HB_ROCKON_TIMEOUT : HB_CHILD_MODE_TIME;

		if(hb->timeout > max_timeout){
			hb->timeout = 0;
			return HBSEQ_CHILD_END;
		}
	}

	return HBSEQ_CHILD_START;
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�q�@����I��
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ChildEnd(HUGEBEACON_SYS *hb)
{
	WMErrCode err_code;
	
	hb->scan_callback = 0;
	err_code = WM_EndScan(_Callback_ChildEndScan);
	if(err_code != WM_ERRCODE_OPERATING){
		OS_TPrintf("EndScan�Ɏ��s�@�G���[:%d\n", err_code);
		return HBSEQ_CONTINUE;
	}
	
	hb->scan_callback = 0;
	return HBSEQ_CHILD_END_WAIT;
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�q�@����I���̃R�[���o�b�N�҂�
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ChildEndWait(HUGEBEACON_SYS *hb)
{
	if(hb->scan_callback == 0){
		return HBSEQ_CONTINUE;
	}
	
	switch(hb->cm.errcode){
	case WM_ERRCODE_SUCCESS:
		if(hb->cm.receive_finish == TRUE){
			OS_TPrintf("����r�[�R���F�I��\n");
			return HBSEQ_END;
		}
		OS_TPrintf("�q�@���[�h�I��\n");
		hb->timeout = 0;
		return HBSEQ_PARENT_START_INIT;
	case WM_ERRCODE_FAILED:
	case WM_ERRCODE_ILLEGAL_STATE:
	case WM_ERRCODE_FIFO_ERROR:
	default:
		OS_TPrintf("Child EndScanCb failed: %d\n", hb->cm.errcode);
		return HBSEQ_CHILD_END;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�e�@�Ƃ��ē��삷��ۂ̎g�p�`�����l������
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_UseChannelSelectInit(HUGEBEACON_SYS *hb)
{
	u16 channel_list;
	int channel_no;

	channel_list = WM_GetAllowedChannel();
	if(channel_list == 0){
		OS_TPrintf("�����ʐM���Ă͂����Ȃ�\n");
		return HBSEQ_ERROR;
	}
	
	OS_TPrintf("�g�p�o����`�����l�� = %x\n", channel_list);
	hb->use_channel_list = channel_list;
	hb->use_channel_level = 101;	//����͂ǂ�ł��Z�b�g�����悤�ɔ͈͊O��ݒ�
	return HBSEQ_USE_CHANNEL_SELECT_MAIN;
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�e�@�Ƃ��ē��삷��ۂ̎g�p�`�����l�������F���C��
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_UseChannelSelectMain(HUGEBEACON_SYS *hb)
{
#define WH_MEASURE_TIME         30     // 1�t���[���Ɉ��ʐM���Ă���d�g���E���邾���̊Ԋu(ms)
#define WH_MEASURE_CS_OR_ED     3      // �L�����A�Z���X��ED�l�̘_���a
#define WH_MEASURE_ED_THRESHOLD 17     // �����f�[�^�ɂ��o���I�ɗL���Ǝv���邨����ED臒l

	WMErrCode errcode;
	int i, channel_no;
	
	hb->scan_callback = 0;
	
	channel_no = 1;
	for(i = 0; i < 14; i++){
		if(hb->use_channel_list & (1 << i)){
			hb->use_channel_list ^= (1 << i);
			break;
		}
		channel_no++;
	}
	
	/*
	 * �d�g�g�p���擾�p�����[�^�Ƃ��āA
	 * �����ɂ��o���I�ɗL���Ǝv����l�����Ă��܂��B
	 */
	errcode = WM_MeasureChannel(_Callback_MeasureChannel,	   /* �R�[���o�b�N�ݒ� */
					 WH_MEASURE_CS_OR_ED,		/* CS or ED */
					 WH_MEASURE_ED_THRESHOLD,	/* ��2�������L�����A�Z���X�݂̂̏ꍇ�͖��� */
					 channel_no,  /* ����̌����`�����l�� */
					 WH_MEASURE_TIME);	/*�P�`�����l��������̒�������[ms] */
	switch(errcode){
	case WM_ERRCODE_OPERATING:
		return HBSEQ_USE_CHANNEL_SELECT_WAIT;
	case WM_ERRCODE_ILLEGAL_STATE:
	case WM_ERRCODE_FIFO_ERROR:
	default:
		return HBSEQ_ERROR;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�e�@�Ƃ��ē��삷��ۂ̎g�p�`�����l�������̃R�[���o�b�N�҂�
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_UseChannelSelectWait(HUGEBEACON_SYS *hb)
{
	WMMeasureChannelCallback *mcc = &hb->pm.mcc;
	
	if(hb->scan_callback == 0){
		return HBSEQ_CONTINUE;
	}
	
	switch(mcc->errcode){
	case WM_ERRCODE_SUCCESS:
		OS_TPrintf("WM_MeasureChannel�R�[���o�b�N����\n");
		break;
	case WM_ERRCODE_FAILED:
	case WM_ERRCODE_ILLEGAL_STATE:
	case WM_ERRCODE_FIFO_ERROR:
	default:
		OS_TPrintf("WM_MeasureChannel�̃R�[���o�b�N�ŃG���[���� : %d\n", mcc->errcode);
		return HBSEQ_ERROR;
	}
	
	OS_TPrintf("�q�b�g�����`�����l�� = %d, �d�g�g�p��=%d\n", mcc->channel, mcc->ccaBusyRatio);
	if(mcc->ccaBusyRatio <= hb->use_channel_level){
		hb->use_channel_level = mcc->ccaBusyRatio;
		hb->use_channel = mcc->channel;
		OS_TPrintf("��L�`�����l���ōX�V����܂���\n");
	}
	
	if(hb->use_channel_list == 0){
		return HBSEQ_PARENT_START_INIT;
	}
	return HBSEQ_USE_CHANNEL_SELECT_MAIN;
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�e�@�Ƃ��ē���J�n����O�̊�{���ݒ�
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ParentStartInit(HUGEBEACON_SYS *hb)
{
	WMErrCode err_code;
	u16 channel_list;
	
	hb->scan_callback = 0;

	if(hb->use_channel == 0){	//�܂��g�p�`�����l�������܂��Ă��Ȃ�
		return HBSEQ_USE_CHANNEL_SELECT_INIT;
	}
	
	channel_list = WM_GetAllowedChannel();
	if(channel_list == 0){
		OS_TPrintf("�����ʐM���Ă͂����Ȃ�\n");
		return HBSEQ_ERROR;
	}
	OS_TPrintf("�g�p�o����`�����l�� = 0x%x\n", channel_list);
	
	if(hb->pm.send_loop == 0){
		//���M�f�[�^�쐬
		hb->pm.add_data_count = 1;
		_HB_TOOL_SendDataMake(hb);
	}
	hb->tgid = WM_GetNextTgid();
	
	//�e�@���̐ݒ�
	GFL_STD_MemClear(&hb->pm.ppara, sizeof(WMParentParam));
	hb->pm.ppara.userGameInfo = (u16*)(&hb->beacon_send_data);
	hb->pm.ppara.userGameInfoLength = sizeof(HB_SEND_DATA);
	hb->pm.ppara.ggid = GGID_HUGE_DATA;
	hb->pm.ppara.tgid = hb->tgid;
	hb->pm.ppara.entryFlag = 0;
	hb->pm.ppara.maxEntry = 0;
	hb->pm.ppara.multiBootFlag = 0;
	hb->pm.ppara.KS_Flag = 0;
	hb->pm.ppara.CS_Flag = 0;
	hb->pm.ppara.beaconPeriod = WM_GetDispersionBeaconPeriod();
	hb->pm.ppara.channel = hb->use_channel;
	hb->pm.ppara.parentMaxSize = 4;	//�g�p���Ȃ��̂ŏ��Ȃ�
	hb->pm.ppara.childMaxSize = 4;		//�g�p���Ȃ��̂ŏ��Ȃ�
	err_code = WM_SetParentParameter( _Callback_ParentParam, &hb->pm.ppara);

	if(err_code != WM_ERRCODE_OPERATING){
		OS_TPrintf("WM_SetParentParameter�Ɏ��s�@�G���[:%d\n", err_code);
		return HBSEQ_ERROR;
	}
	OS_TPrintf("ParentStartInit����\n");
	return HBSEQ_PARENT_START_INIT_WAIT;
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�e�@�̊�{���ݒ�̃R�[���o�b�N�҂�
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ParentStartInitWait(HUGEBEACON_SYS *hb)
{
	if(HbSys->scan_callback == FALSE){
		return HBSEQ_CONTINUE;
	}
	
	switch(hb->pm.errcode){
	case WM_ERRCODE_SUCCESS:
		OS_TPrintf("ParentStartInitWait����\n");
		return HBSEQ_PARENT_START;
	case WM_ERRCODE_FAILED:
	case WM_ERRCODE_INVALID_PARAM:
	case WM_ERRCODE_FIFO_ERROR:
	default:
		OS_TPrintf("Parent EndScanCb failed: %d\n", hb->pm.errcode);
		return HBSEQ_PARENT_START_INIT;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�e�@�Ƃ��ē���J�n
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ParentStart(HUGEBEACON_SYS *hb)
{
	WMErrCode err_code;
	u16 channel_list;
	
	channel_list = WM_GetAllowedChannel();
	if(channel_list == 0){
		OS_TPrintf("�����ʐM���Ă͂����Ȃ�\n");
		return HBSEQ_ERROR;
	}
	
	hb->scan_callback = 0;
#if 1
	err_code = WM_StartParent(_Callback_StartParent);

	if(err_code != WM_ERRCODE_OPERATING){
		OS_TPrintf("WM_StartParent�Ɏ��s�@�G���[:%d\n", err_code);
		return HBSEQ_ERROR;
	}
#else
{
#define WH_MEASURE_TIME         30     // 1�t���[���Ɉ��ʐM���Ă���d�g���E���邾���̊Ԋu(ms)
#define WH_MEASURE_CS_OR_ED     3      // �L�����A�Z���X��ED�l�̘_���a
#define WH_MEASURE_ED_THRESHOLD 17     // �����f�[�^�ɂ��o���I�ɗL���Ǝv���邨����ED臒l

    /*
     * �d�g�g�p���擾�p�����[�^�Ƃ��āA
     * �����ɂ��o���I�ɗL���Ǝv����l�����Ă��܂��B
     */
    return WM_MeasureChannel(NULL,     /* �R�[���o�b�N�ݒ� */
                             WH_MEASURE_CS_OR_ED,       /* CS or ED */
                             WH_MEASURE_ED_THRESHOLD,   /* ��2�������L�����A�Z���X�݂̂̏ꍇ�͖��� */
                             channel_list,  /* ����̌����`�����l�� */
                             WH_MEASURE_TIME);  /*�P�`�����l��������̒�������[ms] */
}
#endif
	OS_TPrintf("WM_StartParent����\n");
	return HBSEQ_PARENT_START_WAIT;
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�e�@����̃R�[���o�b�N�҂�
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ParentStartWait(HUGEBEACON_SYS *hb)
{
	hb->timeout++;
	if(hb->scan_callback == 0){
		return HBSEQ_CONTINUE;	//�R�[���o�b�N����������܂ő҂�
	}
	
#if 0	//�ǂ�state�ł����Ă�SUCCESS�����߂��Ă��Ȃ�
	switch(hb->pm.wpc.state){
	case WM_STATECODE_PARENT_START:
		OS_TPrintf("PARENT CALLBACK STATE : START\n");
		break;
	case WM_STATECODE_BEACON_SENT:
		OS_TPrintf("PARENT CALLBACK STATE : BEACON SET\n");
		break;
	case WM_STATECODE_CONNECTED:
		OS_TPrintf("PARENT CALLBACK STATE : CONNECTED\n");
		break;
	case WM_STATECODE_DISCONNECTED:
		OS_TPrintf("PARENT CALLBACK STATE : DISCONNECTED\n");
		break;
	case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
		OS_TPrintf("PARENT CALLBACK STATE : DISCONNECTED_FROM_MYSELF\n");
		break;
	default:
		OS_TPrintf("PARENT CALLBACK STATE : default �ʏ킠�肦�Ȃ�\n");
		break;
	}
	return HBSEQ_CONTINUE;
#endif

#if PARENTCHILD_KOTEI
	hb->timeout = 0;	//�e�q�Œ胂�[�h�ׁ̈A�^�C���A�E�g�����Ȃ�
#endif

	hb->pm.send_loop++;
	if(hb->pm.send_loop >= HB_PARENT_SEND_LOOP){
		hb->pm.send_loop = 0;
		if(hb->timeout > HB_PARENT_MODE_TIME){
			hb->timeout = 0;
			OS_TPrintf("�e�@�I��\n");
			return HBSEQ_PARENT_END;
		}
		return HBSEQ_PARENT_SEND_CHANGE;
	}
//	return HBSEQ_PARENT_START;
	return HBSEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�e�@����I��
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ParentEnd(HUGEBEACON_SYS *hb)
{
	WMErrCode err_code;
	s32 data_no;
	
	//�e�q�؂�ւ����Ƀf�[�^����R��������ׁA���̑��M���ɏ����O�̃f�[�^�i���o�[����
	//���M����悤�ɂ���
	data_no = hb->beacon_send_data.head.data_no;
	data_no -= hb->pm.add_data_count / 2;
	if(data_no < 0){
		hb->beacon_send_data.head.data_no = hb->data_no_max + data_no;
	}
	else{
		hb->beacon_send_data.head.data_no = data_no;
	}
	
	hb->scan_callback = 0;
	err_code = WM_EndParent(_Callback_EndParent);
	switch(err_code){
	case WM_ERRCODE_OPERATING:
		return HBSEQ_PARENT_END_WAIT;
	case WM_ERRCODE_ILLEGAL_STATE:
	case WM_ERRCODE_FIFO_ERROR:
	default:
		OS_TPrintf("WM_EndParent�Ɏ��s�@�G���[:%d\n", err_code);
	#if 0
		return HBSEQ_ERROR;
	#else
		return HBSEQ_CHILD_START_INIT;
	#endif
	}
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�e�@����I���̃R�[���o�b�N�҂�
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ParentEndWait(HUGEBEACON_SYS *hb)
{
	if(hb->scan_callback == 0){
		return HBSEQ_CONTINUE;
	}
	
	switch(hb->pm.errcode){
	case WM_ERRCODE_SUCCESS:
	#if CHANNEL_KOTEI
		;
	#else
		hb->use_channel = 0;	//�e�@�ɂȂ邽�тɃ`�����l�������������邽�߁A0�N���A
	#endif
		OS_TPrintf("�e�@�I��\n");
		return HBSEQ_CHILD_START_INIT;
	case WM_ERRCODE_FAILED:
	case WM_ERRCODE_ILLEGAL_STATE:
	case WM_ERRCODE_FIFO_ERROR:
	default:
		OS_TPrintf("ParentEnd�̃R�[���o�b�N�ŃG���[���������܂��� errcode=%d\n", hb->pm.errcode);
		return HBSEQ_PARENT_END;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�e�@�̑��M�f�[�^�̓��e��ύX����
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ParentSendChange(HUGEBEACON_SYS *hb)
{
	WMErrCode err_code;

//	return HBSEQ_PARENT_START_INIT;
	
	//���M�f�[�^�쐬
	hb->pm.add_data_count++;
	_HB_TOOL_SendDataMake(hb);
	hb->tgid = WM_GetNextTgid();
	
	hb->scan_callback = 0;
	err_code = WM_SetGameInfo(_Callback_ParentSendChange, 
		(u16*)(&hb->beacon_send_data), sizeof(HB_SEND_DATA), GGID_HUGE_DATA, hb->tgid, 0);

	switch(err_code){
	case WM_ERRCODE_OPERATING:
		OS_TPrintf("WM_SetGameInfo�ɐ���\n");
		return HBSEQ_PARENT_SEND_CHANGE_WAIT;
	case WM_ERRCODE_ILLEGAL_STATE:
	case WM_ERRCODE_INVALID_PARAM:
	case WM_ERRCODE_FIFO_ERROR:
	default:
		OS_TPrintf("WM_SetGameInfo�Ɏ��s�@�G���[:%d\n", err_code);
//		return HBSEQ_ERROR;
		return HBSEQ_CONTINUE;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ����V�[�P���X�F�e�@�̑��M�f�[�^�ύX�̃R�[���o�b�N�҂�
 * @param   hb		
 * @retval  ���̃V�[�P���X�ԍ�
 */
//--------------------------------------------------------------
static int _HUGEBEACON_ParentSendChangeWait(HUGEBEACON_SYS *hb)
{
	if(hb->scan_callback == 0){
		return HBSEQ_CONTINUE;
	}
	
	switch(hb->pm.errcode){
	case WM_ERRCODE_SUCCESS:
//		return HBSEQ_PARENT_START;
		return HBSEQ_PARENT_START_WAIT;
	case WM_ERRCODE_FAILED:
	case WM_ERRCODE_FIFO_ERROR:
	default:
		return HBSEQ_PARENT_SEND_CHANGE;
	}
}


//==============================================================================
//	�R�[���o�b�N�֐�
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   WM_StartScanEx�֐��R�[���o�b�N
 * @param   arg		
 */
//--------------------------------------------------------------
static void _Callback_ChildScan( void* arg )
{
	WMStartScanExCallback *ex = arg;
	WMStartScanCallback *scan = arg;

	HbSys->scan_callback = TRUE;
#if SCAN_USE
	if(HbSys->cm.parent_lockon == FALSE){
#else
	if(1){
#endif
		HbSys->cm.wsec = *ex;
	}
	else{
		HbSys->cm.wsc = *scan;
	}
}

//--------------------------------------------------------------
/**
 * @brief   WM_EndScan�R�[���o�b�N�֐�
 * @param   arg		
 */
//--------------------------------------------------------------
static void _Callback_ChildEndScan(void *arg)
{
	WMCallback *cb = arg;
	
	HbSys->cm.errcode = cb->errcode;
	HbSys->scan_callback = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   WM_MeasureChannel�R�[���o�b�N�֐�
 * @param   arg		
 */
//--------------------------------------------------------------
static void _Callback_MeasureChannel(void *arg)
{
	WMMeasureChannelCallback *mcc = arg;

	HbSys->pm.mcc = *mcc;
	HbSys->scan_callback = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   WM_SetParentParameter�R�[���o�b�N�֐�
 * @param   arg		
 */
//--------------------------------------------------------------
static void _Callback_ParentParam(void *arg)
{
	WMCallback *cb = arg;

	HbSys->pm.errcode = cb->errcode;
	HbSys->scan_callback = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   WM_StartParent�R�[���o�b�N�֐�
 * @param   arg		
 */
//--------------------------------------------------------------
static void _Callback_StartParent(void *arg)
{
	WMStartParentCallback *wpc = arg;
	
	HbSys->pm.wpc = *wpc;
	HbSys->scan_callback = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   WM_EndParent�R�[���o�b�N�֐�
 * @param   arg		
 */
//--------------------------------------------------------------
static void _Callback_EndParent(void *arg)
{
	WMCallback *cb = arg;
	
	HbSys->pm.errcode = cb->errcode;
	HbSys->scan_callback = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   WM_SetGameInfo�R�[���o�b�N�֐�
 * @param   arg		
 */
//--------------------------------------------------------------
static void _Callback_ParentSendChange(void *arg)
{
	WMCallback *cb = arg;
	
	HbSys->pm.errcode = cb->errcode;
	HbSys->scan_callback = TRUE;
}


//==============================================================================
//	�c�[��
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ���M�f�[�^���쐬����
 * @param   hb		
 */
//--------------------------------------------------------------
static void _HB_TOOL_SendDataMake(HUGEBEACON_SYS *hb)
{
	HB_SEND_DATA *hsd = &hb->beacon_send_data;
	
	hsd->head.data_no++;
	if(hsd->head.data_no >= hb->data_no_max){
		hsd->head.data_no = 0;
	}
	
	OS_TPrintf("���M�f�[�^�쐬�@data_no = %d\n", hsd->head.data_no);
	if(hb->data_no_max <= hsd->head.data_no+1){
		//�K������beacon_send_data��HB_SEND_DATA_SIZE�̔{���ł͂Ȃ��̂ŁA�Ō�̃f�[�^�����[�����o��
		hsd->head.size = hb->send_data_size % HB_SEND_DATA_SIZE;
	}
	else{
		hsd->head.size = HB_SEND_DATA_SIZE;
	}
	
	GFL_STD_MemCopy(
		&hb->send_data[HB_SEND_DATA_SIZE * hsd->head.data_no], hsd->data, hsd->head.size);
}

//--------------------------------------------------------------
/**
 * @brief   ��M�f�[�^����M�o�b�t�@�փZ�b�g
 *
 * @param   hb					
 * @param   receive_data		��M�f�[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void _HB_TOOL_ReceiveDataSet(HUGEBEACON_SYS *hb, const void *receive_data)
{
	const HB_SEND_DATA *rcd = receive_data;
	u32 array_no, bit_no;
	
	if(rcd->head.data_no >= HbSys->data_no_max){
		OS_TPrintf("�f�[�^��M�Fdata_no_max�𒴂����f�[�^�i���o�[����M���ꂽ\n");
		GF_ASSERT(0);
		return;
	}
	
	array_no = rcd->head.data_no / sizeof(u32);
	bit_no = rcd->head.data_no % sizeof(u32);
	if(HbSys->receive_bit[array_no] & (1 << bit_no)){
		OS_TPrintf("�f�[�^��M�F���Ɏ�M�ς� data_no = %d\n", rcd->head.data_no);
		return;
	}
	OS_TPrintf("�f�[�^��M�F data_no = %d", rcd->head.data_no);
	
	GFL_STD_MemCopy(
		rcd->data, &HbSys->receive_buf[HB_SEND_DATA_SIZE * rcd->head.data_no], rcd->head.size);
	HbSys->receive_bit[array_no] |= 1 << bit_no;
	HbSys->receive_data_count++;
	if(HbSys->receive_data_count >= HbSys->data_no_max){
		HbSys->cm.receive_finish = TRUE;
		OS_TPrintf("�f�[�^��M�F�S�Ă̎�M������\n");
	}
	OS_TPrintf("�c��f�[�^��=%d, ���݂܂ł̎�M�f�[�^�� %d\%\n", HbSys->data_no_max - HbSys->receive_data_count, HbSys->receive_data_count * 100 / HbSys->data_no_max);
}
