


typedef struct _NET_AUTO_STATE NET_AUTO_STATE ;

/// �R�[���o�b�N�֐��̏��� �����X�e�[�g�J�ڗp
typedef void (*PTRStateFunc)(NET_AUTO_STATE* pState);

struct _NET_AUTO_STATE{
//  NET_PARENTSYS sParent;  ///< �e�̏���ێ�����|�C���^
  PTRStateFunc state;      ///< �n���h���̃v���O�������
  PSyncroCallback pSCallBack;  ///< �����ڑ����ɓ��������������ꍇ�Ă΂��
  u16 timer;           ///< �i�s�^�C�}�[
  u8 autoConnectNum;     ///< �����ڑ��ɕK�v�Ȑl��
};

static NET_AUTO_STATE* _pNetState = NULL;


//==============================================================================
// static�錾
//==============================================================================

// �X�e�[�g�̕ύX
static void _changeStateDebug(NET_AUTO_STATE* pState, PTRStateFunc state, int time, int line);  // �X�e�[�g��ύX����
static void _changeState(NET_AUTO_STATE* pState, PTRStateFunc state, int time);  // �X�e�[�g��ύX����

#ifdef GFL_NET_DEBUG
#define   _CHANGE_STATE(state, time)  _changeStateDebug(_pNetState ,state, time, __LINE__)
#else  //GFL_NET_DEBUG
#define   _CHANGE_STATE(state, time)  _changeState(_pNetState ,state, time)
#endif //GFL_NET_DEBUG


// ���̑���ʓI�ȃX�e�[�g
static void _stateNone(NET_AUTO_STATE* pState);            // �������Ȃ�


//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================

static void _changeState(NET_AUTO_STATE* pState,PTRStateFunc state, int time)
{
    pState->state = state;
    pState->timer = time;
}

//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(NET_AUTO_STATE* pState,PTRStateFunc state, int time, int line)
{
    NET_PRINT("net_state: %d\n",line);
    _changeState(pState, state, time);
}
#endif

#if 0
BOOL KEY_YT_NET_Main(NET_PARAM* pNet)
{
    switch(pNet->seq){
      case _INIT_WAIT_PARENT:
        if(GFL_NET_IsInit()){
            _SEQCHANGE(_CONNECT_WAIT);
        }
        break;
      case _INIT_WAIT_CHILD:
        if(GFL_NET_IsInit()){
            GFL_NET_StartBeaconScan(); // �����ڑ�
            _SEQCHANGE( _SEARCH_CHILD );
        }
        break;
      case _SEARCH_CHILD:
        {
            u8* pData = GFL_NET_GetBeaconMacAddress(0);//�r�[�R�����X�g��0�Ԗڂ𓾂�
            if(pData){
                GFL_NET_ConnectToParent(pData);
                _SEQCHANGE( _CONNECT_WAIT );
            }
        }
        break;
      case _CONNECT_WAIT:
        _SEQCHANGE( _NEGO_START );
        break;
      case _NEGO_START:
        if(KEY_YT_NET_IsParent(pNet)){  //�e�@�̏ꍇ
            if(GFL_NET_HANDLE_GetNumNegotiation() != 0){
                if(GFL_NET_HANDLE_RequestNegotiation()){
                    _SEQCHANGE( _TIM_START );
                }
            }
        }
        else{
//            if(GFL_NET_HANDLE_GetNumNegotiation( pNet->pNetHandle[1]) != 0){
                if(GFL_NET_HANDLE_RequestNegotiation( )){
                    _SEQCHANGE( _TIM_START );
                }
  //          }
        }
        break;
      case _TIM_START:
        if(GFL_NET_HANDLE_GetNumNegotiation()==2){
            pNet->pNetHandle[1] = GFL_NET_HANDLE_GetCurrentHandle();   // �n���h���쐬
            GFL_NET_TimingSyncStart(pNet->pNetHandle[1] , _TEST_TIMING);
            _SEQCHANGE( _TIM_OK );
        }
        break;
      case _TIM_OK:
        if(GFL_NET_IsTimingSync(pNet->pNetHandle[1] , _TEST_TIMING)){
            pNet->bGameStart = TRUE;
#if GFL_NET_WIFI
            GFL_NET_SetWifiBothNet(FALSE);  // WIFI��default�Ŕ񓯊��ڑ��ɂ��Ă���
#endif
            GFL_NET_SetNoChildErrorCheck(TRUE);
            GFL_NET_SetAutoErrorCheck(TRUE);
            _SEQCHANGE( _LOOP );
        }
        break;

    }
    return pNet->bGameStart;
}
#endif


//==============================================================================
/**
 * @brief   ��������ꂽ��R�[���o�b�N���Ă�ŏI���
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _autoInitSyncroServer(GFL_NETSTATE* pState)
{
    GFLNetInitializeStruct* pIni = _GFL_NET_GetNETInitStruct();

    if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle() , _TEST_TIMING)){
        if(_pNetState->pSCallBack){
            _pNetState->pSCallBack(pIni->pWork);
            _CHANGE_STATE(_stateNone, 0);
        }
    }
}

//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�������l������������瓯�����Ƃ�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _autoInitHandlingServer(GFL_NETSTATE* pState)
{
    if(GFL_NET_HANDLE_GetNumNegotiation()==_pNetState->autoConnectNum){
        GFL_NET_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() , _TEST_TIMING);
        _CHANGE_STATE(_autoInitSyncroServer)
    }
}

//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�������Ď����ē����Ԃ�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _autoInitWaitServer(GFL_NETSTATE* pState)
{
    if(GFL_NET_IsInit()){
        if(GFL_NET_HANDLE_GetNumNegotiation() != 0){
            if(GFL_NET_HANDLE_RequestNegotiation()){
                _CHANGE_STATE(_autoInitHandlingServer);
            }
        }
    }
}

//==============================================================================
/**
 * @brief   �����ŃT�[�o�𗧂��グ�� �F�ؓ��͂��ׂĂ��C��
 * @param   num     �ڑ������ɕK�v�Ȑl��
 * @param   num     �l�����̓�������ꂽ�Ƃ��Ă΂��R�[���o�b�N
 * @retval  none
 */
//==============================================================================

void GFL_NET_AutoInitServer(int num,PSyncroCallback pSyncroCallback)
{
    _pNetState->autoConnectNum = num;
    _pNetState->pSCallBack = pSyncroCallback;
    _CHANGE_STATE(_autoInitWaitServer);


}
