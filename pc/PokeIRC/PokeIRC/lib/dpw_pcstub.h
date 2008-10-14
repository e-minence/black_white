/*! @file
	@brief	DPW��WIN32�ł�DS��DWC���g����悤�ɂ��邽�߂̃X�^�u�R�[�h
	
	@author	nakamud

	@version 1.00	initial release.
*/


#ifndef DPW_PCSTUB_H_
#define DPW_PCSTUB_H_


#ifdef __cplusplus
extern "C" {
#endif





//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// �G���[�񋓎q
typedef enum {
    DWC_ERROR_NONE = 0,            // �G���[�Ȃ�
    DWC_ERROR_DS_MEMORY_ANY,       // �{�̃Z�[�u�f�[�^�Ɋւ���G���[
    DWC_ERROR_AUTH_ANY,            // �F�؂Ɋւ���G���[
    DWC_ERROR_AUTH_OUT_OF_SERVICE, // �T�[�r�X�I��
    DWC_ERROR_AUTH_STOP_SERVICE,   // �T�[�r�X�ꎞ���f
    DWC_ERROR_AC_ANY,              // �����ڑ��Ɋւ���G���[
    DWC_ERROR_NETWORK,             // ���̑��̃l�b�g���[�N�G���[
    DWC_ERROR_DISCONNECTED,        // �ؒf���ꂽ
    DWC_ERROR_FATAL,               // �v���I�ȃG���[

    // [arakit] main 051011
    // �V�[�P���X��̌y�x�̃G���[�iDWCErrorType = DWC_ETYPE_LIGHT�j
    DWC_ERROR_FRIENDS_SHORTAGE,    // �w��̐l���̗L���ȗF�B�����Ȃ��̂ɗF�B�w��s�A�}�b�`���C�N���n�߂悤�Ƃ���
    DWC_ERROR_NOT_FRIEND_SERVER,   // �T�[�o�N���C�A���g�}�b�`���C�N�ŁA�T�[�o�Ɏw�肵���F�B�����݂ɗF�B�ł͂Ȃ��A�������̓T�[�o�𗧂ĂĂ��Ȃ�
    DWC_ERROR_MO_SC_CONNECT_BLOCK, // �T�[�o�N���C�A���g�}�b�`���C�N�̃}�b�`���C�N�I�v�V�����ŁA�T�[�o����ڑ������ۂ��ꂽ
    // [arakit] main 051011
    // [arakit] main 051024
    DWC_ERROR_SERVER_FULL,         // �T�[�o�N���C�A���g�}�b�`���C�N�̃T�[�o������I�[�o�[
        
    DWC_ERROR_NUM
} DWCError;

// [arakit] main 051010
// �G���[���o���ɕK�v�ȏ����������G���[�^�C�v
typedef enum {
    DWC_ETYPE_NO_ERROR = 0,  // �G���[�Ȃ�
    DWC_ETYPE_LIGHT,         // �Q�[���ŗL�̕\���݂̂ŁA�G���[�R�[�h�\���͕K�v�Ȃ�
    DWC_ETYPE_SHUTDOWN_FM,   // DWC_ShutdownFriendsMatch()���Ăяo���āAFriendsMatch���C�u�������I������K�v������B
                             // �G���[�R�[�h�̕\�����K�v�B
    DWC_ETYPE_DISCONNECT,    // FriendsMatch�������Ȃ�DWC_ShutdownFriendsMatch()���Ăяo���A�X��DWC_CleanupInet()�ŒʐM�̐ؒf���s���K�v������B
                             //�G���[�R�[�h�̕\�����K�v�B
    DWC_ETYPE_FATAL,         // Fatal Error �����Ȃ̂ŁA�d��OFF�𑣂��K�v������B
                             // �G���[�R�[�h�̕\�����K�v�B
    DWC_ETYPE_NUM
} DWCErrorType;
// [arakit] main 051010

// �f�o�b�O�pdwc_base�G���[�R�[�h�e�v�f
enum {
    // �ǂ̃V�[�P���X���ɋN�������G���[��
    DWC_ECODE_SEQ_LOGIN          = (-60000),  // ���O�C�������ł̃G���[
    DWC_ECODE_SEQ_FRIEND         = (-70000),  // �F�B�Ǘ������ł̃G���[
    DWC_ECODE_SEQ_MATCH          = (-80000),  // �}�b�`���C�N�����ł̃G���[
    DWC_ECODE_SEQ_ETC            = (-90000),  // ��L�ȊO�̏����ł̃G���[

    // GameSpy�̃G���[�̏ꍇ�A�ǂ�SDK���o�����G���[��
    DWC_ECODE_GS_GP              =  (-1000),  // GameSpy GP �̃G���[
    DWC_ECODE_GS_PERS            =  (-2000),  // GameSpy Persistent �̃G���[
    DWC_ECODE_GS_STATS           =  (-3000),  // GameSpy Stats �̃G���[
    DWC_ECODE_GS_QR2             =  (-4000),  // GameSpy QR2 �̃G���[
    DWC_ECODE_GS_SB              =  (-5000),  // GameSpy Server Browsing �̃G���[
    DWC_ECODE_GS_NN              =  (-6000),  // GameSpy Nat Negotiation �̃G���[
    DWC_ECODE_GS_GT2             =  (-7000),  // GameSpy gt2 �̃G���[
    DWC_ECODE_GS_HTTP            =  (-8000),  // GameSpy HTTP �̃G���[
    DWC_ECODE_GS_ETC             =  (-9000),  // GameSpy ���̑���SDK�̃G���[

    // �ʏ�G���[���
    DWC_ECODE_TYPE_NETWORK       =   ( -10),  // �l�b�g���[�N��Q
    DWC_ECODE_TYPE_SERVER        =   ( -20),  // GameSpy�T�[�o��Q
    DWC_ECODE_TYPE_DNS           =   ( -30),  // DNS��Q
    DWC_ECODE_TYPE_DATA          =   ( -40),  // �s���ȃf�[�^����M
    DWC_ECODE_TYPE_SOCKET        =   ( -50),  // �\�P�b�g�ʐM�G���[
    DWC_ECODE_TYPE_BIND          =   ( -60),  // �\�P�b�g�̃o�C���h�G���[
    DWC_ECODE_TYPE_TIMEOUT       =   ( -70),  // �^�C���A�E�g����
    DWC_ECODE_TYPE_PEER          =   ( -80),  // �P�΂P�̒ʐM�ŕs�
    DWC_ECODE_TYPE_CONN_OVER     =   (-100),  // �R�l�N�V�������I�[�o
    DWC_ECODE_TYPE_STATS_AUTH    =   (-200),  // STATS�T�[�o���O�C���G���[
    DWC_ECODE_TYPE_STATS_LOAD    =   (-210),  // STATS�T�[�o�f�[�^���[�h�G���[
    DWC_ECODE_TYPE_STATS_SAVE    =   (-220),  // STATS�T�[�o�f�[�^�Z�[�u�G���[
    DWC_ECODE_TYPE_NOT_FRIEND    =   (-400),  // �w�肳�ꂽ���肪���݂ɓo�^���ꂽ�F�B�ł͂Ȃ�
    DWC_ECODE_TYPE_OTHER         =   (-410),  // ���肪�ʐM�ؒf�����i��ɃT�[�o�N���C�A���g�}�b�`���C�N���j
    DWC_ECODE_TYPE_MUCH_FAILURE  =   (-420),  // NAT�l�S�V�G�[�V�����ɋK��񐔈ȏ㎸�s����
    DWC_ECODE_TYPE_SC_CL_FAIL    =   (-430),  // �T�[�o�N���C�A���g�}�b�`���C�N�E�N���C�A���g�̐ڑ����s
    DWC_ECODE_TYPE_CLOSE         =   (-600),  // �R�l�N�V�����N���[�Y���̃G���[
    DWC_ECODE_TYPE_TRANS_HEADER  =   (-610),  // ���肦�Ȃ���M�X�e�[�g��Reliable�f�[�^����M����
    DWC_ECODE_TYPE_TRANS_BODY    =   (-620),  // ��M�o�b�t�@�I�[�o�[�t���[
    DWC_ECODE_TYPE_AC_FATAL      =   (-700),  // AC��������AC�ł̃G���[�O���[�v�B���̏ꍇ���ʌ��͐�p��state�l�B
    DWC_ECODE_TYPE_OPEN_FILE     =   (-800),  // GHTTP�t�@�C���I�[�v�����s
    DWC_ECODE_TYPE_INVALID_POST  =   (-810),  // GHTTP�����ȑ��M
    DWC_ECODE_TYPE_REQ_INVALID   =   (-820),  // GHTTP�t�@�C����������
    DWC_ECODE_TYPE_UNSPECIFIED   =   (-830),  // GHTTP�ڍוs���̃G���[
    DWC_ECODE_TYPE_BUFF_OVER     =   (-840),  // GHTTP�o�b�t�@�I�[�o�[�t���[
    DWC_ECODE_TYPE_PARSE_URL     =   (-850),  // GHTTP URL�̉�̓G���[
    DWC_ECODE_TYPE_BAD_RESPONSE  =   (-860),  // GHTTP�T�[�o����̃��X�|���X��̓G���[
    DWC_ECODE_TYPE_REJECTED      =   (-870),  // GHTTP�T�[�o����̃��N�G�X�g������
    DWC_ECODE_TYPE_FILE_RW       =   (-880),  // GHTTP���[�J���t�@�C��Read/Write�G���[
    DWC_ECODE_TYPE_INCOMPLETE    =   (-890),  // GHTTP�_�E�����[�h���f
    DWC_ECODE_TYPE_TO_BIG        =   (-900),  // GHTTP�t�@�C���T�C�Y���傫�����ă_�E�����[�h�s��
    DWC_ECODE_TYPE_ENCRYPTION    =   (-910),  // GHTTP�G���N���v�V�����G���[

    // Fatal Error��p���
    DWC_ECODE_TYPE_ALLOC         =     (-1),  // �������m�ێ��s
    DWC_ECODE_TYPE_PARAM         =     (-2),  // �p�����[�^�G���[
    DWC_ECODE_TYPE_SO_SOCKET     =     (-3),  // SO�̃G���[��������GameSpy gt2�̃\�P�b�g�G���[
    DWC_ECODE_TYPE_NOT_INIT      =     (-4),  // ���C�u����������������Ă��Ȃ��B
    DWC_ECODE_TYPE_DUP_INIT      =     (-5),  // ���C�u��������x���������ꂽ�B
    DWC_ECODE_TYPE_WM_INIT       =     (-6),  // WM�̏������Ɏ��s�����B
    DWC_ECODE_TYPE_UNEXPECTED    =     (-9)   // �\�����Ȃ���ԁA��������Unkwon��GameSpy�G���[����
};







    

#define DWC_Printf( dummyarg, fmt, ... )	printf( fmt, __VA_ARGS__ )
#define OS_TVPrintf( fmt, vlist )			vprintf( fmt, vlist )
BOOL DWCi_IsError(void);
void DWCi_SetError(DWCError error, int errorCode);
void DWC_ClearError(void);

#define OS_Sleep		Sleep
#define OS_SPrintf		sprintf

typedef enum
{
    DWC_ALLOCTYPE_AUTH,
    DWC_ALLOCTYPE_AC,
    DWC_ALLOCTYPE_BM,
    DWC_ALLOCTYPE_UTIL,
    DWC_ALLOCTYPE_BASE,
    DWC_ALLOCTYPE_GS,
    DWC_ALLOCTYPE_LAST
} DWCAllocType;
    
void* DWC_Alloc     ( DWCAllocType name, u32 size );
void  DWC_Free      ( DWCAllocType name, void* ptr, u32 size );

void Debug_MemBrowse_begin();
void Debug_MemBrowse_end();

void MI_CpuClear8(void* ptr, int size);


typedef struct
{
    u32     x;                         // �����l
    u32     mul;                       // �搔
    u32     add;                       // ���Z���鐔
}
MATHRandContext16;

typedef struct
{
    u64     x;                         // �����l
    u64     mul;                       // �搔
    u64     add;                       // ���Z���鐔
}
MATHRandContext32;

void MATH_InitRand16(MATHRandContext16 *context, u32 seed);
u16 MATH_Rand16(MATHRandContext16 *context, u16 max);
void MATH_InitRand32(MATHRandContext32 *context, u64 seed);
u32 MATH_Rand32(MATHRandContext32 *context, u32 max);


#ifdef __cplusplus
}
#endif


#endif // DPW_PCSTUB_H_
