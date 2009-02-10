//==============================================================================
/**
 * @file	compati_comm.c
 * @brief	�����`�F�b�N�F�ʐM
 * @author	matsuda
 * @date	2009.02.10(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include "system\main.h"
#include "net\network_define.h"
#include "system\gfl_use.h"



//==============================================================================
//	�f�[�^
//==============================================================================
///�ʐM�R�}���h�e�[�u��
static const NetRecvFuncTable _CommPacketTbl[] = {
    {_RecvMoveData,         NULL},    ///NET_CMD_MOVE
    {_RecvHugeData,         NULL},    ///NET_CMD_HUGE
    {_RecvKeyData,          NULL},    ///NET_CMD_KEY
    {_RecvMyProfile,        NULL},    ///NET_CMD_MY_PROFILE
};

enum{
	NET_CMD_MOVE = GFL_NET_CMD_COMPATI_CHECK,
	NET_CMD_HUGE,
	NET_CMD_KEY,
	NET_CMD_MY_PROFILE,
};

#define _MAXNUM   (4)         // �ő�ڑ��l��
#define _MAXSIZE  (32)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��


static const GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // ��M�֐��e�[�u��
    NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    _endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g	
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
    0x532,//0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_IRC,   //��check�@�ԊO���ʐM�p��create�����HEAPID
    GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_WIRELESS,//GFL_NET_TYPE_IRC,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_DEBUG_MATSUDA_SERVICEID,  //GameServiceID
};

//--------------------------------------------------------------
/**
 * @brief   �ԊO���ʐM���C��
 *
 * @param   cs		
 * @param   irc		
 *
 * @retval  TRUE:�q�̃G���g���[�������� or �e�̃v���t�B�[�����󂯎����
 */
//--------------------------------------------------------------
static BOOL DM_IrcMain(COMPATI_SYS *cs, DM_IRC_WORK *irc)
{
	BOOL ret = 0;
	
	if(irc->initialize == TRUE){
		IRC_Move();
	}
	
	if(irc->connect == TRUE){
		if(IRC_IsConnect() == FALSE){
			irc->connect = FALSE;
			OS_TPrintf("�ؒf���ꂽ\n");
			irc->seq = 0;
		}
	}
	else{
		if(IRC_IsConnect() == TRUE){
			irc->connect = TRUE;
			irc->connect_wait = 0;
			OS_TPrintf("�ڑ�����\n");
		}
		else{
			if(cs->oya == MY_CHILD){
				irc->connect_wait++;
				if(irc->connect_wait > 60){
					irc->connect_wait = 0;
					irc->seq = 0;
					OS_TPrintf("�Ȃ��Ȃ��ڑ����Ȃ��̂ōēx���������Đڑ��ɍs��\n");
				}
			}
		}
	}
	
	if(irc->success == TRUE && irc->connect == FALSE){
		irc->success = FALSE;
		return TRUE;
	}
	
	switch( irc->seq ){
	case 0:
		OS_TPrintf("IRC������\n");
		GFL_STD_MemClear(&cs->irc, sizeof(DM_IRC_WORK));
		IRC_Init();
		IRC_SetRecvCallback(DM_IRC_ReceiveCallback);
		if(cs->oya == MY_CHILD){	//�q���ʐM���n�߂�B�e�͏�Ɏ�M��
			IRC_Connect();
		}
		irc->initialize = TRUE;
		irc->seq++;
		break;
	case 1:
		if(irc->connect == TRUE){
			//if(cs->oya == MY_PARENT){	//��M���ł���e����ʐM�J�n
			if(IRC_IsSender() == FALSE){	//��M���͒ʏ�A�e
				IRC_Send((u8*)&cs->my_profile, sizeof(IRC_MATCH_ENTRY_PARAM), CMD_PARENT_PROFILE, 0);
			}
			irc->seq++;
		}
		break;
	case 2:
		if(irc->success == TRUE && irc->connect == FALSE){
			irc->seq = 0;
		}
		break;
	}
	
	return ret;
}


static void _endCallBackSeq(void* vwk)
{
    COMPATI_SYS *cs = vwk;
    cs->seq++;
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO���ʐM�e�X�g
 *
 * @param   cs		
 *
 * @retval  TRUE:�I���B�@FALSE:�����p����
 */
//--------------------------------------------------------------
static BOOL DebugMatsuda_IrcMatch(COMPATI_SYS *cs)
{
	BOOL ret, irc_ret = 0;
	int msg_id;
	
	GF_ASSERT(cs);

    OS_TPrintf("-- %d --\n",cs->seq);
	switch(cs->seq){
    case 0:
		{
			GFLNetInitializeStruct net_ini_data;
			
			net_ini_data = aGFLNetInit;
            net_ini_data.bNetType = GFL_NET_TYPE_IRC_WIRELESS;
			GFL_NET_Init(&net_ini_data, _initCallBack, cs);	//�ʐM������
		}
		cs->seq++;
        break;
	case 1:
		{
			OSOwnerInfo info;
			int i;
			
			OS_GetOwnerInfo( &info );
			for(i = 0; i < 11; i++){
				cs->my_profile.name[i] = info.nickName[i];
			}
			cs->my_profile.name_len = info.nickNameLength;
			OS_TPrintf("���O�̒���=%d\n", cs->my_profile.name_len);
			OS_GetMacAddress(cs->my_profile.parent_MacAddress);
			OS_TPrintf("MAC: %d, %d, %d, %d, %d, %d\n", cs->my_profile.parent_MacAddress[0], cs->my_profile.parent_MacAddress[1], cs->my_profile.parent_MacAddress[2], cs->my_profile.parent_MacAddress[3], cs->my_profile.parent_MacAddress[4], cs->my_profile.parent_MacAddress[5]);
		}

		OS_TPrintf("�e��A�{�^���A�@�q��X�{�^��\n");
		GFL_MSG_GetString( cs->mm, DM_MSG_MATCH001, cs->strbuf );
		cs->printStream = PRINTSYS_PrintStream( cs->drawwin[CCBMPWIN_MAX].win, 0, 0,
						cs->strbuf, cs->fontHandle, 0, cs->tcbl, 0, HEAPID_COMPATI, 0xff );

		cs->seq++;
		break;
	case 2:
		if( PRINTSYS_PrintStreamGetState(cs->printStream) == PRINTSTREAM_STATE_DONE ){
			PRINTSYS_PrintStreamDelete( cs->printStream );
			cs->seq++;
		}
		break;

	case 3:
		switch(GFL_UI_KEY_GetTrg()){
		case PAD_BUTTON_A:
			cs->oya = MY_PARENT;
			cs->my_profile.oya_child = MY_PARENT;
			msg_id = DM_MSG_MATCH002;
			GFL_MSG_GetString( cs->mm, msg_id, cs->strbuf );
			CCLocal_MessagePut(cs, CCBMPWIN_MAX, cs->strbuf, 0, 0);
			cs->seq++;
			break;
		case PAD_BUTTON_X:
			cs->oya = MY_CHILD;
			cs->my_profile.oya_child = MY_CHILD;
			msg_id = DM_MSG_MATCH003;
			GFL_MSG_GetString( cs->mm, msg_id, cs->strbuf );
			CCLocal_MessagePut(cs, CCBMPWIN_MAX, cs->strbuf, 0, 0);
			cs->seq++;
			break;
		default:
			break;
		}
		break;
	case 4:
		if(cs->oya == MY_PARENT && ((GFL_UI_KEY_GetTrg() & PAD_BUTTON_START) || cs->connect_max == TRUE)){
			IRC_Shutdown();
			cs->seq++;
			break;
		}
		
		if((cs->oya == MY_CHILD && cs->irc.success == FALSE) 
				|| (cs->oya == MY_PARENT && cs->connect_max == FALSE)){
			irc_ret = DM_IrcMain(cs, &cs->irc);
            OS_TPrintf("DM_IrcMain%d \n",irc_ret);
			if(irc_ret == TRUE && cs->entry_num >= CHILD_MAX){
				cs->connect_max = TRUE;
			}
		}
		
		if(irc_ret == TRUE){
			if(cs->oya == MY_CHILD){
				OS_TPrintf("�q�F���C�����X�ʐM�J�n\n");
				GFL_STR_SetStringCodeOrderLength(cs->strbuf, cs->parent_profile.name, cs->parent_profile.name_len+1);
				CCLocal_MessagePut(cs, 0, cs->strbuf, 0, 0);
                GFL_NET_Exit(_endCallBackSeq);	//�ʐM�I��
				cs->seq++;
			}
			else{
				OS_TPrintf("�G���g���[��������\n");
				GFL_STR_SetStringCodeOrderLength(cs->strbuf, cs->child_profile[cs->entry_num - 1].name, cs->child_profile[cs->entry_num - 1].name_len+1);
				CCLocal_MessagePut(cs, cs->entry_num, cs->strbuf, 0, 0);
			}
			break;
		}
		break;
      case 5:
        //_endCallBackSeq�҂�
        break;
	default:
		return TRUE;	//���C�����X�ʐM������
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ���C�����X�ʐM�F�e��MAC�A�h���X���w�肵�Čq����
 *
 * @param   cs		
 *
 * @retval  TRUE:�I���B�@FALSE:�����p����
 */
//--------------------------------------------------------------
static BOOL DebugMatsuda_Wireless(COMPATI_SYS *cs)
{
	BOOL ret;
	
	GF_ASSERT(cs);

	switch( cs->seq ){
	case 0:
		if(cs->oya == MY_PARENT){
			GFL_MSG_GetString( cs->mm, DM_MSG_MATCH006, cs->strbuf );
			CCLocal_MessagePut(cs, CCBMPWIN_MAX, cs->strbuf, 0, 0);
		}
		else{
			GFL_MSG_GetString( cs->mm, DM_MSG_MATCH004, cs->strbuf );
			CCLocal_MessagePut(cs, CCBMPWIN_MAX, cs->strbuf, 0, 0);
		}
		
		{
			GFLNetInitializeStruct net_ini_data;
			
			net_ini_data = aGFLNetInit;
            
			GFL_NET_Init(&net_ini_data, _initCallBack, cs);	//�ʐM������
		}
		cs->seq++;
		break;
	case 1:
		if(GFL_NET_IsInit() == TRUE){	//�������I���҂�
			cs->seq++;
		}
		break;
	case 2:
		if(cs->oya == MY_PARENT){
			GFL_NET_InitServer();
			OS_TPrintf("�e�@�ɂȂ��ă��C�����X�J�n\n");
		}
		else{
			GFL_NET_InitClientAndConnectToParent(cs->parent_profile.parent_MacAddress);
			OS_TPrintf("�q�@�ɂȂ��ă��C�����X�J�n\n");
		}
		cs->seq++;
		break;
	case 3:	//���C�����X�ڑ��҂�
		//��check�@����ڑ����������f�����i���Ȃ��̂ŁA�b��I�Ƀ^�C�}�[�Ōq���������ɂ���
		cs->timer++;
		if(cs->timer > 30){
			cs->timer = 0;
			//�l�S�V�G�[�V�����J�n
			OS_TPrintf("�l�S�V�G�[�V�������M\n");
			if(cs->oya == MY_CHILD){
				if(GFL_NET_HANDLE_RequestNegotiation() == TRUE){
					cs->seq++;
				}
			}
			else{
				cs->seq++;	//�e�@�͍Ō�Ƀl�S�V�G�[�V�������M������
			}
		}
		break;
	case 4:	//�l�S�V�G�[�V���������҂�
		if(cs->oya == MY_PARENT){
			if(GFL_NET_HANDLE_GetNumNegotiation() >= cs->entry_num){
				cs->timer++;
				if(cs->timer > 30){
					cs->timer = 0;
					if(GFL_NET_HANDLE_RequestNegotiation() == TRUE){
						OS_TPrintf("�q�̃l�S�V�G�[�V����������\n");
					 //   cs->connect_ok = TRUE;
					    cs->seq++;
					}
				}
			}
		}
		else{
			cs->seq++;
		}
		break;

	case 5:
		if(cs->oya == MY_PARENT){
			if(GFL_NET_HANDLE_GetNumNegotiation() >= cs->entry_num + 1){
				OS_TPrintf("�e�̃l�S�V�G�[�V����������\n");
				cs->connect_ok = TRUE;
				cs->seq++;
			}
		}
		else{
			cs->seq++;
		}
		break;
	case 6:		//�^�C�~���O�R�}���h���s
		if(cs->oya == MY_PARENT){
			cs->timer++;
			if(cs->timer < 30){
				break;
			}
			cs->timer = 0;
		}
		GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,15);
		cs->seq++;
		break;
	case 7:
		if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),15) == TRUE){
			OS_TPrintf("�^�C�~���O��萬��\n");
			OS_TPrintf("�ڑ��l�� = %d\n", GFL_NET_GetConnectNum());
			GFL_MSG_GetString( cs->mm, DM_MSG_MATCH008, cs->strbuf );
			CCLocal_MessagePut(cs, CCBMPWIN_MAX, cs->strbuf, 0, 0);
			cs->seq++;
		}
		break;
	case 8:
		//netID���ɔz��ɖ��ߍ��ވׁA�ēx�������g�̃v���t�B�[����S���ɑ��M
		if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_MY_PROFILE, sizeof(IRC_MATCH_ENTRY_PARAM), &cs->my_profile) == TRUE){
			OS_TPrintf("�����̃v���t�B�[����S�Ẵv���C���[�ɑ��M����\n");
			OS_TPrintf("MAC: %d, %d, %d, %d, %d, %d\n", cs->my_profile.parent_MacAddress[0], cs->my_profile.parent_MacAddress[1], cs->my_profile.parent_MacAddress[2], cs->my_profile.parent_MacAddress[3], cs->my_profile.parent_MacAddress[4], cs->my_profile.parent_MacAddress[5]);
			cs->seq++;
		}
		else{
			OS_TPrintf("�����v���t�B�[�����M���s\n");
		}
		break;
	case 9:	//�v���t�B�[���𑗐M���������̂ōēx�������
		GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,16);
		cs->seq++;
		break;
	case 10:
		if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),16) == TRUE){
			OS_TPrintf("�^�C�~���O��萬�� 2���\n");
			cs->seq++;
		}
		break;
	
	case 11:	//�L�[�𑗐M������
		ret = -1;
		cs->send_key = GFL_UI_KEY_GetRepeat();
		if(cs->send_key & PAD_KEY_UP){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &cs->send_key);
		}
		if(cs->send_key & PAD_KEY_DOWN){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &cs->send_key);
		}
		if(cs->send_key & PAD_KEY_LEFT){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &cs->send_key);
		}
		if(cs->send_key & PAD_KEY_RIGHT){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &cs->send_key);
		}
		
		if(cs->send_key & PAD_BUTTON_B){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_HUGE, 0x1000, &cs->huge_data);
		}
		
		if(ret == FALSE){
			OS_TPrintf("���M���s\n");
		}
		
		if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT){
//			cs->seq++;
		}
		break;
	case 12:	//�ʐM�I��
		if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_CMD_EXIT_REQ, 0, NULL)){
			//GFL_NET_Exit(_endCallBack);	//�ʐM�I��
			cs->seq++;
		}
		break;
	case 13:	//�ʐM�I���҂�
		if(cs->connect_ok == FALSE){
			cs->seq++;
		}
		break;
	default:
		return TRUE;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   IRC��M���ɌĂ΂��R�[���o�b�N
 *
 * @param   data		
 * @param   size		
 * @param   command		
 * @param   value		
 */
//--------------------------------------------------------------
static void DM_IRC_ReceiveCallback(u8 *data, u8 size, u8 command, u8 value)
{
	COMPATI_SYS *cs = glb_d_matsu;
	int send_id;
	IRC_MATCH_ENTRY_PARAM *recv = (IRC_MATCH_ENTRY_PARAM *)data;
	
	//�ԊO����p�̃V�X�e���R�}���h����
	switch(command){
	case 0xf4:		//�ԊO�����C�u�����̐ؒf�R�}���h
		OS_TPrintf("IRC�ؒf�R�}���h����M\n");
		return;
	}
	
	//�q���瑗�M���Ă���f�[�^
	switch(command){
	case CMD_CHILD_PROFILE:		//�q�̃v���t�B�[����M
		if(cs->my_profile.oya_child == MY_CHILD){
			OS_TPrintf("������������q�ł���\n");
			IRC_Shutdown();
			return;
		}
		CCLocal_EntryAdd(cs, recv);
		IRC_Send(NULL, 0, CMD_PARENT_SUCCESS, 0);
		cs->irc.success = TRUE;
		break;
	}
	
	//�e���瑗�M���Ă���f�[�^
	switch(command){
	case CMD_PARENT_PROFILE:	//�e�̃v���t�B�[����M
		CCLocal_ParentProfileSet(cs, recv);
		IRC_Send((u8*)&cs->my_profile, sizeof(IRC_MATCH_ENTRY_PARAM), CMD_CHILD_PROFILE, 0);
		break;
	case CMD_PARENT_SUCCESS:		//�e����󂯎�����ƕԎ�����M
		IRC_Shutdown();
		cs->irc.success = TRUE;
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �q�̃v���t�B�[����o�^
 *
 * @param   cs		
 * @param   profile		
 */
//--------------------------------------------------------------
static void CCLocal_EntryAdd(COMPATI_SYS *cs, IRC_MATCH_ENTRY_PARAM *profile)
{
	GF_ASSERT(cs->entry_num < CHILD_MAX);
	GFL_STD_MemCopy(profile, &cs->child_profile[cs->entry_num], sizeof(IRC_MATCH_ENTRY_PARAM));
	cs->entry_num++;
	OS_TPrintf("�q���G���g���[:%d�l��\n", cs->entry_num);
}

//--------------------------------------------------------------
/**
 * @brief   �e�̃v���t�B�[����o�^
 *
 * @param   cs		
 * @param   profile		
 */
//--------------------------------------------------------------
static void CCLocal_ParentProfileSet(COMPATI_SYS *cs, IRC_MATCH_ENTRY_PARAM *profile)
{
	GFL_STD_MemCopy(profile, &cs->parent_profile, sizeof(IRC_MATCH_ENTRY_PARAM));
	OS_TPrintf("�e�̃v���t�B�[���o�^\n");
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W��\������
 *
 * @param   cs			
 * @param   strbuf		�\�����郁�b�Z�[�W�f�[�^
 * @param   x			X���W
 * @param   y			Y���W
 */
//--------------------------------------------------------------
static void CCLocal_MessagePut(COMPATI_SYS *cs, int win_index, STRBUF *strbuf, int x, int y)
{
	GFL_BMP_Clear( cs->drawwin[win_index].bmp, 0xff );
	PRINTSYS_PrintQue(cs->printQue, cs->drawwin[win_index].bmp, x, y, strbuf, cs->fontHandle);
	cs->drawwin[win_index].message_req = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R���̋��ʃf�[�^�o�^
 *
 * @param   cs		
 */
//--------------------------------------------------------------
static void CCLocal_PokeIconCommonDataSet(COMPATI_SYS *cs)
{
	//�p���b�g
	NNS_G2dInitImagePaletteProxy( &cs->icon_pal_proxy );
	GFL_ARCHDL_UTIL_TransVramPaletteMakeProxy(ARCID_POKEICON, POKEICON_GetPalArcIndex(), 
		NNS_G2D_VRAM_TYPE_2DMAIN, D_MATSU_ICON_PALNO, HEAPID_COMPATI, &cs->icon_pal_proxy);
	
	//�Z��
	cs->icon_cell_res = GFL_ARCHDL_UTIL_LoadCellBank(ARCID_POKEICON, POKEICON_GetCellArcIndex(), 
		FALSE, &cs->icon_cell_data, HEAPID_COMPATI);
	
	//�Z���A�j��
	cs->icon_anm_res = GFL_ARCHDL_UTIL_LoadAnimeBank(ARCID_POKEICON , POKEICON_GetAnmArcIndex(),
		FALSE, &cs->icon_anm_data, HEAPID_COMPATI);
}

//--------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R���̋��ʃf�[�^�j��
 *
 * @param   cs		
 */
//--------------------------------------------------------------
static void CCLocal_PokeIconCommonDataFree(COMPATI_SYS *cs)
{
	int i;
	
	for(i = 0; i < CCBMPWIN_MAX; i++){
		if(cs->clwk[i] != NULL){
			GFL_CLACT_WK_Remove(cs->clwk[i]);
		}
	}
	GFL_HEAP_FreeMemory(cs->icon_cell_res);
	GFL_HEAP_FreeMemory(cs->icon_anm_res);
}

//--------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R���o�^
 *
 * @param   cs		
 * @param   monsno		�|�P�����ԍ�
 * @param   net_id		�l�b�gID
 */
//--------------------------------------------------------------
static void CCLocal_PokeIconAdd(COMPATI_SYS *cs, int monsno, int net_id)
{
	GFL_CLWK_DATA clwk_data;
	GFL_CLWK_RES clwk_res;
	NNSG2dImageProxy imgProxy;
	u32 icon_index;
	u32 vram_offset = 0;	//byte�P��
	
	GF_ASSERT(cs->clwk[net_id] == NULL);

	vram_offset = POKEICON_SIZE_CGX * net_id;
	
	OS_TPrintf("monsno = %d\n", monsno);
	
	//�L�����N�^�ݒ�
	NNS_G2dInitImageProxy(&imgProxy);
	icon_index = POKEICON_GetCgxArcIndexByMonsNumber(monsno, 0, FALSE);
	//VRAM�]��&�C���[�W�v���L�V�쐬
	GFL_ARCHDL_UTIL_TransVramCharacterMakeProxy(ARCID_POKEICON, icon_index, FALSE, CHAR_MAP_1D, 0, 
		NNS_G2D_VRAM_TYPE_2DMAIN, vram_offset, HEAPID_COMPATI, &imgProxy);
	
	//�Z���A�j���p���\�[�X�f�[�^�쐬
	GFL_CLACT_WK_SetCellResData(&clwk_res, &imgProxy, &cs->icon_pal_proxy, 
		cs->icon_cell_data, cs->icon_anm_data);
	
	//�A�N�^�[�o�^
	clwk_data = PokeIconClwkData;
	clwk_data.pos_x = 120;
	clwk_data.pos_y = 24 + net_id*32;
	cs->clwk[net_id] = GFL_CLACT_WK_Add(
		cs->clunit, &clwk_data, &clwk_res, CLWK_SETSF_NONE, HEAPID_COMPATI);
	
	//�A�j���I�[�g�ݒ�
	GFL_CLACT_WK_SetAutoAnmSpeed(cs->clwk[net_id], FX32_ONE);
	GFL_CLACT_WK_SetAutoAnmFlag(cs->clwk[net_id], TRUE);
	
	//�p���b�gNo�ݒ�
	GFL_CLACT_WK_SetPlttOffs(cs->clwk[net_id], POKEICON_GetPalNum(monsno, 0, FALSE));
}


//==============================================================================
//	
//==============================================================================
typedef struct{
    int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { WB_NET_DEBUG_MATSUDA_SERVICEID };

//--------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�擾�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

static void* _netBeaconGetFunc(void* pWork)
{
	return &_testBeacon;
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int _netBeaconGetSizeFunc(void* pWork)
{
	return sizeof(_testBeacon);
}

///< �r�[�R���f�[�^�擾�֐�
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
    if(myNo != beaconNo){
        return FALSE;
    }
    return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �����������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
static void _initCallBack(void* pWork)
{
	OS_TPrintf("�����������R�[���o�b�N���Ăяo���ꂽ\n");
	return;
}

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
static void _connectCallBack(void* pWork)
{
	COMPATI_SYS *cs = pWork;
	
    OS_TPrintf("�l�S�V�G�[�V��������\n");
    cs->connect_ok = TRUE;
}

static void _endCallBack(void* pWork)
{
	COMPATI_SYS *cs = pWork;

    NET_PRINT("endCallBack�I��\n");
    cs->connect_ok = FALSE;
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ړ��R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

static void _RecvMoveData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
}

//--------------------------------------------------------------
/**
 * @brief   ����f�[�^�R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

static void _RecvHugeData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	COMPATI_SYS *cs = pWork;
	int i;
	const u8 *data = pData;
	u16 cs = 0;
	
	OS_TPrintf("����f�[�^��M netid = %d, size = 0x%x\n", netID, size);
	for(i = 0; i < size; i++){
		cs += data[i];
		//OS_TPrintf("%d, ", data[i]);
	//	if(i % 32 == 0){
	//		OS_TPrintf("\n");
	//	}
	}
	OS_TPrintf("����f�[�^��M�`�F�b�N�T��=0x%x\n", cs);
}

static u8 * _RecvHugeBuffer(int netID, void* pWork, int size)
{
	COMPATI_SYS *cs = pWork;
	return cs->receive_huge_data[netID - 1];
}


//--------------------------------------------------------------
/**
 * @brief   �L�[���R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _RecvKeyData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	COMPATI_SYS *cs = pWork;
	int key_data;
	int dx = 0, dy = 0;
	
	if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	
	key_data = *((int *)pData);
	OS_TPrintf("�f�[�^��M�FnetID=%d, size=%d, data=%d\n", netID, size, key_data);
	if(key_data & PAD_KEY_UP){
		OS_TPrintf("��M�F��\n");
		dy -= 4;
	}
	if(key_data & PAD_KEY_DOWN){
		OS_TPrintf("��M�F��\n");
		dy += 4;
	}
	if(key_data & PAD_KEY_LEFT){
		OS_TPrintf("��M�F��\n");
		dx -= 4;
	}
	if(key_data & PAD_KEY_RIGHT){
		OS_TPrintf("��M�F�E\n");
		dx += 4;
	}

#if 0	//���O�ł͂Ȃ��|�P�����A�C�R���𓮂����悤�ɕύX
	cs->msg_pos_x[netID] += dx;
	cs->msg_pos_y[netID] += dy;
	if(netID == 0){
		if(cs->oya == MY_PARENT){
			GFL_STR_SetStringCodeOrderLength(cs->strbuf_win[netID], cs->my_profile.name, cs->my_profile.name_len+1);
		}
		else{
			GFL_STR_SetStringCodeOrderLength(cs->strbuf_win[netID], cs->parent_profile.name, cs->parent_profile.name_len+1);
		}
	}
	else{
		GFL_STR_SetStringCodeOrderLength(cs->strbuf_win[netID], cs->child_profile[netID-1].name, cs->child_profile[netID-1].name_len+1);
	}
	CCLocal_MessagePut(cs, netID, 
		cs->strbuf_win[netID], cs->msg_pos_x[netID], cs->msg_pos_y[netID])
#else
	if(cs->clwk[netID] != NULL){
		GFL_CLACTPOS pos;
		GFL_CLACT_WK_GetPos(cs->clwk[netID], &pos, CLWK_SETSF_NONE);
		pos.x += dx;
		pos.y += dy;
		GFL_CLACT_WK_SetPos(cs->clwk[netID], &pos, CLWK_SETSF_NONE);
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �L�[���R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _RecvMyProfile(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	COMPATI_SYS *cs = pWork;
	const IRC_MATCH_ENTRY_PARAM *recv = pData;
	int i;
	
	OS_TPrintf("RecvChildProfile, netID=%d, size=%d\n", netID, size);
	
	if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	
	if(netID == 0){
		GFL_STD_MemCopy(recv, &cs->parent_profile, size);
	}
	else{
		GFL_STD_MemCopy(recv, &cs->child_profile[netID - 1], size);
	}
	GFL_STR_SetStringCodeOrderLength(
		cs->strbuf_win[netID], recv->name, recv->name_len+1);
	CCLocal_MessagePut(cs, netID, cs->strbuf_win[netID], 0, 0);

	CCLocal_PokeIconAdd(cs, recv->parent_MacAddress[5], netID);
}




//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA CompatiCheckProcData = {
	CompatiCheck_ProcInit,
	CompatiCheck_ProcMain,
	CompatiCheck_ProcEnd,
};


