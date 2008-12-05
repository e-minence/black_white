#include "../include/system.h"
#include "../include/com.h"
/******************************************************************************
******************************************************************************/
#define COM_MAX			(16)
//#define COM_PORT_NAME	"COM8"
//#define BAUD_RATE		(115200)
#define BAUD_RATE		(256000)
#define BYTE_SIZE		(8)			// 8bit
#define PARITY			NOPARITY	// �p���e�B�[ EVENPARITY
#define STOP_BIT		ONESTOPBIT	// �X�g�b�v�r�b�g
#define F_PARITY		FALSE //TRUE		// �t���[����F
/******************************************************************************
******************************************************************************/
#define TIMEOUT			(20)
/******************************************************************************
******************************************************************************/
static bool isConnect;
static int connectIdx;
/******************************************************************************
******************************************************************************/
HANDLE hComm, hEvent;							// �V���A���|�[�g�Ƃ̒ʐM�n���h��
OVERLAPPED overlap;

static TCHAR *port_tbl_l[COM_MAX] = 
{
  L"COM1", L"COM2", L"COM3", L"COM4", L"COM5", L"COM6", L"COM7", L"COM8", 
  L"COM9", L"COM10", L"COM11", L"COM12", L"COM13", L"COM14", L"COM15", L"COM16", 
};

static char *port_tbl[COM_MAX] = 
{
  "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", 
  "COM9", "\\\\.\\COM10", "\\\\.\\COM11", "\\\\.\\COM12", "\\\\.\\COM13", "\\\\.\\COM14", "\\\\.\\COM15", "\\\\.\\COM16", 
};
/******************************************************************************
******************************************************************************/
bool ComIsConnect(void){ return isConnect; }
TCHAR *ComGetPort(void)
{
  if(connectIdx == -1){ return NULL; }
  return port_tbl_l[connectIdx];
}
/******************************************************************************
******************************************************************************/
static int _search_port(void)
{
  HKEY hKey = NULL;
  DWORD dwCount = 0;
  TCHAR cNameBuff[256];
  BYTE byValueBuff[256];
  DWORD dwNameBuffSize = 0;
  DWORD dwValueBuffSize = 0;
  DWORD dwType = 0;
  //LONG ret;

  int i;
  /****************************************************************************
  ****************************************************************************/
  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Enum\\USB\\Vid_ffff&Pid_0015\\00000001\\Device Parameters"), 
				  0, 
				  KEY_READ, 
				  &hKey) != ERROR_SUCCESS){
	//printf("USB�@���GUID�����̂��߂̃��W�X�g���I�[�v�����s\n");
	return (COM_MAX);
  }
  /****************************************************************************
  �v�f���𓾂�
  ****************************************************************************/
  DWORD subKeys;
  DWORD maxSubKeyLen;
  if(RegQueryInfoKey(hKey, NULL, NULL, NULL, &subKeys, &maxSubKeyLen, NULL, &dwCount, // ���W�X�g���G���g���̐�
					 NULL, NULL, NULL, NULL) != ERROR_SUCCESS ){
	//printf("COM�|�[�g�̐��擾�Ɏ��s���܂���\n");
	return (COM_MAX);
  }
  //printf("���W�X�g������ �T�u�L�[�� %d �ő啶���� %d ���W�X�g���� %d\n", subKeys, maxSubKeyLen, dwCount);
  /****************************************************************************
  �v�f�������J��Ԃ�
  ****************************************************************************/
  for(i = 0 ; i < (int)dwCount ; i++){
	dwNameBuffSize = sizeof(cNameBuff);
	dwValueBuffSize = sizeof(byValueBuff);
	
	if(RegEnumValue(hKey, i, &cNameBuff[0], &dwNameBuffSize, NULL, &dwType, // �ꗗ�����o��
					&byValueBuff[0], &dwValueBuffSize) != ERROR_SUCCESS ){
	  //printf("���O�̎擾�Ɏ��s���܂���\n");
	  return (COM_MAX);
	}
	if(wcscmp(L"PortName", cNameBuff) == 0){ /*printf("%ls: %ls\n", cNameBuff, byValueBuff);*/ break;}
#if 0
	dwNameBuffSize = sizeof(keyBuff);
	dwValueBuffSize = sizeof(subBuff);
	ret = RegEnumKeyExA(hKey, i, &keyBuff[0], &dwNameBuffSize, NULL,
					   &subBuff[0], &dwValueBuffSize, &filetime);
	if(ret != ERROR_SUCCESS ){
	  DWORD r;
	  printf("�L�[�̎擾�Ɏ��s���܂��� %d\n", ret);
	  r = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
						NULL, // ���������
						ret,//GetLastError(),
						LANG_USER_DEFAULT, // ����ID
						&cNameBuff[0], 256,
						NULL);
	  printf("�߂�l %d\n", r);
	  MessageBox(g_hWnd, cNameBuff, L"error: RegEnumKeyEx", MB_ICONHAND|MB_OK);
	  return (COM_MAX);
	}
	printf("%ls\n", cNameBuff);
#endif 
  }
  /****************************************************************************
  ****************************************************************************/
  for(i=0;i<COM_MAX;i++){
	if(wcscmp(port_tbl_l[i], (TCHAR*)byValueBuff) == 0){ return (i); }
  }
  return (COM_MAX);
}
/******************************************************************************
�V���A���|�[�g���J����
******************************************************************************/
bool ComInit(unsigned char idx)
{
  hEvent = NULL; hComm = NULL;
  isConnect = FALSE; connectIdx = -1;
  /****************************************************************************
  ****************************************************************************/
  hEvent = CreateEvent(NULL, FALSE, FALSE, L"overlap");
  memset(&overlap, 0, sizeof(OVERLAPPED));
  overlap.hEvent = hEvent;
  /****************************************************************************
  ****************************************************************************/
  if(idx == COM_AUTO){ // �ԊO�����j�b�g��T��
	idx = _search_port(); 
	if(idx == COM_MAX){ /*printf("COM�|�[�g��������܂���ł���\n");*/ return false; }
  }
  /****************************************************************************
  �V���A���|�[�g���J����
  ****************************************************************************/
  hComm = CreateFileA(port_tbl[idx],					// �V���A���|�[�g�̕�����
					 GENERIC_READ | GENERIC_WRITE,	// �A�N�Z�X���[�h�F�ǂݏ��� 
					 0,								// ���L���[�h�F������̓A�N�Z�X�s�� 
					 NULL,							// �Z�L�����e�B�����F�n���h���p������ 
					 OPEN_EXISTING,					// �쐬�t���O�F
					 FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,			// �����F
					 NULL);							// �e���v���[�g�̃n���h���F 
  
  
  if(hComm == INVALID_HANDLE_VALUE) {
	//printf("%s���J�����Ƃ��o���܂���ł����B\n", port_tbl[idx]); // ����o���ƍĐڑ��ŗ���܂���
	return false;
  }
  //printf("%s�̃I�[�v�������B\n", port_tbl[idx]);
  isConnect = TRUE; connectIdx = idx;
  /****************************************************************************
  �ʐM������ݒ肷��
  ****************************************************************************/
  DCB dcb;
  GetCommState(hComm, &dcb); /* DCB ���擾 */
  dcb.BaudRate = BAUD_RATE;
  dcb.ByteSize = BYTE_SIZE;
  dcb.Parity = PARITY;
  dcb.StopBits = STOP_BIT;
  dcb.fParity = F_PARITY;
  SetCommState(hComm, &dcb); /* DCB ��ݒ� */
  
  return true;
}
/******************************************************************************
�n���h�������
******************************************************************************/
void ComEnd()
{
  CloseHandle(hComm); CloseHandle(hEvent);
  isConnect = FALSE; connectIdx = -1;
}
/******************************************************************************
******************************************************************************/
bool ComCheck(void)
{
  DWORD dwErrors;  /* �G���[��� */
  COMSTAT ComStat; /* �f�o�C�X�̏�� */

  ClearCommError(hComm, &dwErrors, &ComStat);
#if 0
  switch(dwErrors){
  case CE_BREAK: printf("CE_BREAK\n"); break;
  case CE_DNS: printf("CE_DNS\n"); break;
  case CE_FRAME: printf("CE_FRAME\n"); break;
  case CE_IOE: printf("CE_IOE\n"); break;
  case CE_MODE: printf("CE_MODE\n"); break;
  case CE_OOP: printf("CE_OOP\n"); break;
  case CE_OVERRUN: printf("CE_OVERRUN\n"); break;
  case CE_PTO: printf("CE_PTO\n"); break;
  case CE_RXOVER: printf("CE_RXOVER\n"); break;
  case CE_RXPARITY: printf("CE_RXPARITY\n"); break;
  case CE_TXFULL: printf("CE_TXFULL\n"); break;
  case 0: break;
  default: printf("CE_UNKNOWN %d\n", dwErrors); break;
  }
#else
  if(dwErrors != 0){ return FALSE; }
#endif
  return TRUE;
}
/******************************************************************************
******************************************************************************/
bool WriteData(unsigned char *buff, unsigned int data_size)
{
  BOOL bRet, bLError;
  DWORD dwWritten; /* �|�[�g�֏������񂾃o�C�g�� */
	
  if(hComm == INVALID_HANDLE_VALUE) { return false; }
  
  bRet = WriteFile(hComm, buff, data_size, &dwWritten, &overlap);

  if(!bRet){
	DWORD timeout;
	bLError = GetLastError();
	
	if(bLError == ERROR_IO_PENDING){
	  timeout = GetTickCount();
	  while(1){
		bRet = GetOverlappedResult(hComm, &overlap, &dwWritten, FALSE);
		if(bRet){ break; } // �������ݏI��
		if((GetTickCount() - timeout) > TIMEOUT){ 
		  //printf("�������݃^�C���A�E�g\n"); 
		  return false; 
		}
	  }
	}else{
	  //printf("�������݃G���[\n");
	  return false;
	}
  }
  if (dwWritten!=data_size) {
	//printf("�f�[�^�̑��M�Ɏ��s���܂����Bsize %d -> %d: 0x%X\n", data_size, dwWritten, buff[0]);
	return false;
  }
  return true;
}
/******************************************************************************
******************************************************************************/
int ReadData(unsigned char *buff, unsigned int max_size)
{
  BOOL bRet, bLError;
  DWORD dwErrors;  /* �G���[��� */
  COMSTAT ComStat; /* �f�o�C�X�̏�� */
  DWORD dwCount;   /* ��M�f�[�^�̃o�C�g�� */
  DWORD dwRead;    /* �|�[�g����ǂݏo�����o�C�g�� */
  
  ClearCommError(hComm, &dwErrors, &ComStat);
  dwCount = ComStat.cbInQue;
  
  if (dwCount > max_size){ dwRead = 0; return (dwRead); }
  
  bRet = ReadFile(hComm, buff, dwCount, &dwRead, &overlap);
  if(!bRet){
	DWORD timeout;
	bLError = GetLastError();

	if(bLError == ERROR_IO_PENDING){
	  timeout = GetTickCount();
	  while(1){
		bRet = GetOverlappedResult(hComm, &overlap, &dwRead, FALSE);
		if(bRet){ break; } // �������ݏI��
		if((GetTickCount() - timeout) > TIMEOUT){ 
		  //printf("�ǂݍ��݃^�C���A�E�g\n"); 
		  return false; 
		}
	  }
	}else{
	  //printf("�ǂݍ��݃G���[\n");
	  CloseHandle(hComm); CloseHandle(hEvent);
	  return false;
	}
  }
  
  if(dwCount != dwRead){ dwRead = 0; return (dwRead); }
  
  return dwRead;
}
/******************************************************************************
******************************************************************************/
#if 0
void ComOpen(void)
{
  ComInit(COM_PORT12); // COM12 ���I�[�v��
}
#endif
/******************************************************************************
******************************************************************************/
#if 0
void ComCheck(void)
{
  for(int i=0;i<COM_MAX;i++){ ComInit(i); }
}
#endif
