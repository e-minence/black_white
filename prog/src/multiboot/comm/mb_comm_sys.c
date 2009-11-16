//======================================================================
/**
 * @file  mb_comm_sys.c
 * @brief �}���`�u�[�g �ʐM�V�X�e��
 * @author  ariizumi
 * @data  09/11/13
 *
 * ���W���[�����FMB_COMM
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "net/network_define.h"

#include "multiboot/comm/mb_comm_sys.h"
#include "multiboot/mb_local_def.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define

//======================================================================
//  enum
//======================================================================
#pragma mark [> enum


//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
struct _MB_COMM_WORK
{
  HEAPID heapId;
};


//======================================================================
//  proto
//======================================================================
#pragma mark [> proto

static void*	MB_COMM_GetBeaconDataDummy(void* pWork);
static int		MB_COMM_GetBeaconSizeDummy(void* pWork);

static const NetRecvFuncTable MultiBootCommPostTable[] = {
  NULL , NULL
};


//--------------------------------------------------------------
//  ���[�N�̍쐬
//--------------------------------------------------------------
MB_COMM_WORK* MB_COMM_CreateSystem( const HEAPID heapId )
{
  MB_COMM_WORK* commWork = GFL_HEAP_AllocMemory( heapId , sizeof( MB_COMM_WORK ) );
  
  commWork->heapId = heapId;
  
  return commWork;
}

//--------------------------------------------------------------
//  ���[�N�̊J��
//--------------------------------------------------------------
void MB_COMM_DeleteSystem( MB_COMM_WORK* commWork )
{
  GFL_HEAP_FreeMemory( commWork );
}

//--------------------------------------------------------------
//  �ʐM�̊J�n
//--------------------------------------------------------------
void MB_COMM_InitComm( MB_COMM_WORK* commWork )
{
  GFLNetInitializeStruct aGFLNetInit = {
    MultiBootCommPostTable, //NetSamplePacketTbl,  // ��M�֐��e�[�u��
    NELEMS(MultiBootCommPostTable), // ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL, // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL, // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    MB_COMM_GetBeaconDataDummy,  // �r�[�R���f�[�^�擾�֐�  
    MB_COMM_GetBeaconSizeDummy,  // �r�[�R���f�[�^�T�C�Y�擾�֐� 
    NULL, // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL, // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� 
    NULL,  //Fatal�G���[���N�������ꍇ
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL, // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g  
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
    MB_DEF_GGID,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    0,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_WIFI,  //�ԊO���p��create�����HEAPID
    GFL_WICON_POSX,GFL_WICON_POSY,  // �ʐM�A�C�R��XY�ʒu
    2,//_MAXNUM,  //�ő�ڑ��l��
    48,//_MAXSIZE,  //�ő呗�M�o�C�g��
    2,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
    TRUE,   // CRC�v�Z
    FALSE,    // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_WIRELESS,    //  NET�ʐM�^�C�v �� wifi�ʐM���s�����ǂ���
    FALSE,    // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_BOX_DOWNLOAD_SERVICEID,//GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD, // �ԊO���^�C���A�E�g����
#endif
    512 - 24*2,//MP�e�ő�T�C�Y 512�܂�
    0,//dummy
  };

  aGFLNetInit.baseHeapID = commWork->heapId;
  GFL_NET_Init( &aGFLNetInit , NULL , (void*)commWork );
}

//--------------------------------------------------------------
// �ʐM�I��
//--------------------------------------------------------------
void MB_COMM_ExitComm( MB_COMM_WORK* work )
{
  GFL_NET_Exit(NULL);
}

//--------------------------------------------------------------
// �����������`�F�b�N
//--------------------------------------------------------------
const BOOL MB_COMM_IsInitComm( MB_COMM_WORK* work )
{
  return GFL_NET_IsInit();
}

//--------------------------------------------------------------
// �I�����������`�F�b�N
//--------------------------------------------------------------
const BOOL MB_COMM_IsFinishComm( MB_COMM_WORK* work )
{
  return GFL_NET_IsExit();
}





//--------------------------------------------------------------
/**
 *	�e��r�[�R���p�R�[���o�b�N�֐�(�_�~�[
 */
//--------------------------------------------------------------
void*	MB_COMM_GetBeaconDataDummy(void* pWork)
{
	static u8 dummy[2];
	return (void*)&dummy;
}
int	MB_COMM_GetBeaconSizeDummy(void* pWork)
{
	return sizeof(u8)*2;
}
