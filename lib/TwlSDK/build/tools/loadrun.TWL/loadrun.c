/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tools - loadrun.TWL
  File:     loadrun.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-07-15#$
  $Rev: 7370 $
  $Author: yada $
 *---------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>

#include "isd_api.h"

#define  SDK_BOOL_ALREADY_DEFINED_
#include <twl_win32.h>
#include <nitro/os/common/system.h>

extern const unsigned long SDK_DATE_OF_LATEST_FILE;

//---- �A�v���P�[�V������
#define EXEC_NAME          "loadrun.TWL"
#define DEBUGGER_NAME      "IS-TWL-DEBUGGER"


//---- �o�[�W����������
#define VERSION_STRING     " 1.7  Copyright 2007-2008 Nintendo. All right reserved."
// 1.7 HYBRID��NITRO���[�h�œ���ł���悤�ɂ��� (-N�ǉ�)
// 1.6 �ŏI�G���[��\���ł���悤�ɂ���
// 1.5 �f�o�b�K�֎~�t���O���Q�Ƃ���悤�ɂ���
// 1.4 �V���A���w��Ŗ����݂̂̃}�b�`�ɑΉ�����
// 1.3 ISTD_DownloadGo() �����Ή�
// 1.2 print�R�[���o�b�N�̕s��C��
// 1.1 IS-TWL-DEBUGGER Ver.0.41 �ւ̑Ή�
// 1.0 ���J

//---- exit �ԍ�
#define EXIT_NUM_NO_ERROR               207     // �����I��(�A���A����͂���ɂȂ邱�Ƃ͂Ȃ�)
#define EXIT_NUM_USER_SIGNAL            206     // ���[�U�ɂ�鋭���I��(ctrl-C)
#define EXIT_NUM_EXEC_TIME_OUT          205     // ���s�^�C���A�E�g�ŋ����I��
#define EXIT_NUM_TIME_OUT               204     // �\���^�C���A�E�g�ŋ����I��
#define EXIT_NUM_SHOW_DEVICES           203     // �f�o�C�X���X�g�\���ŏI��
#define EXIT_NUM_SHOW_USAGE             202     // �w���v�\���ŏI��
#define EXIT_NUM_SHOW_VERSION           201     // �o�[�W�����\���ŏI��
#define EXIT_NUM_STRING_ABORT           200     // ������ɂ�鋭���I��

#define EXIT_NUM_NO_DEVICE              -1      // �g�p�\�ȃf�o�C�X���Ȃ�
#define EXIT_NUM_UNKNOWN_OPTION         -2      // �z��O�̃I�v�V�������w�肳�ꂽ
#define EXIT_NUM_ILLEGAL_OPTION         -3      // �I�v�V�����̎g�p���@���Ⴄ
#define EXIT_NUM_NO_INPUT_FILE          -4      // �w��̃t�@�C�����Ȃ�or�I�[�v���ł��Ȃ�
#define EXIT_NUM_NOT_CONNECT            -5      // �f�o�C�X�ւ̐ڑ����s
#define EXIT_NUM_CANNOT_USE_CARTRIDGE   -6      // �J�[�g���b�W�̃��b�N�Ɏ��s
#define EXIT_NUM_CANNOT_USE_CARD        -7      // �J�[�h�̃��b�N�Ɏ��s
#define EXIT_NUM_PRINTF_ERROR           -8      // printf�f�[�^�������̃G���[
#define EXIT_NUM_LOADING_ERROR          -9      // ���[�h���̃G���[
#define EXIT_NUM_EXEC_PROHIBITION       -10     // ���s��������Ă��Ȃ�
#define EXIT_NUM_NO_HYBRID_NITRO        -11     // HYBRID �� NITRO �Ŏ��s�ł��Ȃ�

//---- �ő�R���\�[����
#define PRINT_CONSOLE_MAX   4

//---- ROM �w�b�_���
#define RH_FLAG_OFFSET					0x1c	// ROM�w�b�_�㏔���t���O
#define RH_FLAG_DEBUGGER_PROHIBITION	(1<<3)	// ���̃r�b�g�������Ă���ƃf�o�b�K���s�o���Ȃ�

//---- �f�o�C�X�w��p
char   *gDeviceName[] = {
    "TWLEMU", "TWLDEB", NULL
};
int     gDeviceTypeArray[] = {
    ISTD_DEVICE_IS_TWL_EMULATOR,
    ISTD_DEVICE_IS_TWL_DEBUGGER,
};

//---- ���샂�[�h
BOOL    gQuietMode = FALSE;            // quiet ���[�h
BOOL    gVerboseMode = FALSE;          // verbose ���[�h
BOOL    gDebugMode = FALSE;            // debug ���[�h

BOOL    gStdinMode = FALSE;            // stdin ���[�h

BOOL    gIsForceNitro = FALSE;         // hybrid �� NITRO���[�h�Ƃ��邩

BOOL    gIsTypeSpecified = FALSE;      // �f�o�C�X�^�C�v�w�肠�肩�H
int     gSpecifiedType;                // ����ꍇ�̃f�o�C�X�^�C�v

BOOL    gIsSerialSpecified = FALSE;    // �V���A���ԍ��w�肠�肩�H
int     gSpecifiedSerial;              // ����ꍇ�̃V���A���ԍ�

BOOL    gIsCartridgeLocked = FALSE;    // �J�[�g���b�W�X���b�g�����b�N���邩
BOOL    gIsCardLocked = FALSE;         // �J�[�h�X���b�g�����b�N���邩

int     gTimeOutTime = 0;              // �^�C���A�E�g����(�b) (0 �͂Ȃ�)
BOOL    gTimeOutOccured = FALSE;       // �^�C���A�E�g������������

int     gExecTimeOutTime = 0;          // ���s�^�C���A�E�g����(�b) (0 �͂Ȃ�)
int     gExecTimeOutOccured = FALSE;   // ���s�^�C���A�E�g������������

char   *gAbortString = NULL;           // �����I��������
BOOL    gStringAborted = FALSE;        // �����I��������ɂ���ďI��������ǂ���

BOOL    gExitAborted = FALSE;          // OS_Exit() �I��
int     gExitStatusNum = EXIT_NUM_STRING_ABORT; // OS_Exit()�ɂ���ďI������Ƃ��̕Ԃ�l
int     gExitStrLength;                // �I��������̃T�C�Y

BOOL    gShowArchitecture = FALSE;      // �A�[�L�e�N�`���\�����邩
BOOL    gShowConsoleNum = FALSE;        // �R���\�[���ԍ��\�����邩


//---- TWL���C�u�����p
HINSTANCE gDllInstance;
ISDDEVICEHANDLE gDeviceHandle;
ISDDEVICEID gDeviceId;

//---- �f�o�C�X�ɐڑ�������(�X���b�g�̃X�C�b�`�p)
BOOL    gDeviceConnected = FALSE;


//---- �f�o�C�X���X�g
#define DEVICE_MAX_NUM      256
#define DEVICE_SERIAL_NONE  0x7fffffff // means no specified
ISTDDevice gDeviceList[DEVICE_MAX_NUM];
int     gCurrentDevice = -1;
int     gConnectedDeviceNum = 0;

int     gDeviceTypeSpecified = ISTD_DEVICE_NONE;
int     gDeviceSerialSpecified = DEVICE_SERIAL_NONE;    // means no specified
#define DEVICE_SERIAL_STRING_MAX_SIZE 256
char	gDeviceSerialString[DEVICE_SERIAL_STRING_MAX_SIZE];
int     gDeviceSerialStringLen = 0;

//---- ���̓t�@�C��
#define FILE_NAME_MAX_SIZE  1024
FILE   *gInputFile;
char    gInputFileNameString[FILE_NAME_MAX_SIZE];
BOOL    gIsInputFileOpened = FALSE;

//---- ����
time_t  gStartTime = 0;                // �X�^�[�g����
BOOL    gIsLineHead[PRINT_CONSOLE_MAX] = {TRUE, TRUE, TRUE, TRUE}; // �s�����ǂ���
BOOL    gShowLapTime = FALSE;

//---- �V�O�i��
BOOL    gStoppedByUser = FALSE;        // ���[�U�Ɏ~�߂�ꂽ���ǂ���

//---- �\���������ǂ���
volatile BOOL    gIsOutputString = FALSE;


#define printfIfNotQuiet(str)    do{if(!gQuietMode){fputs(str,stdout);}}while(0)

void    displayErrorAndExit(int exitNum, char *message);
BOOL    outputString(TWLArch arch, int console, char *buf, int bufSize);

static void printCallback( LPVOID user, ISDCPUARCH arch, DWORD console, const void* recvBuf, DWORD size );
void checkFileFlag(void);

/*---------------------------------------------------------------------------*
  Name:         myExit

  Description:  similar to exit()

  Arguments:    exitNum : exit()�ԍ�

  Returns:      None
 *---------------------------------------------------------------------------*/
void myExit(int exitNum)
{
    //---- �J�[�g���b�W�ƃJ�[�h�̃X���b�g���I�t��
    if (gDeviceConnected)
    {
//        (void)ISNTD_CartridgeSlotPower(gDeviceHandle, FALSE);
//        (void)ISNTD_CardSlotPower(gDeviceHandle, FALSE);
    }

    //---- DLL�̈���
//    ISNTD_FreeDll();
    ISTD_FreeDll();

    if (!gQuietMode)
    {
        if (exitNum == EXIT_NUM_USER_SIGNAL)
        {
            printf("\n*** %s: stopped by user.\n", EXEC_NAME);
        }
        else if (exitNum == EXIT_NUM_TIME_OUT)
        {
            printf("\n*** %s: stopped by print timeout.\n", EXEC_NAME);
        }
        else if (exitNum == EXIT_NUM_EXEC_TIME_OUT)
        {
            printf("\n*** %s: stopped by exec timeout.\n", EXEC_NAME);
        }
        else if (gStringAborted)
        {
            printf("\n*** %s: stopped by aborting string.\n", EXEC_NAME);
        }
        else if (gExitAborted)
        {
            exitNum = gExitStatusNum;
        }
    }

    exit(exitNum);
}

/*---------------------------------------------------------------------------*
  Name:         listDevice

  Description:  �f�o�C�X�̈ꗗ�\�����ďI���

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void listDevice(void)
{
    int     n;

    //---- �f�o�C�X�ǂݍ���
    gConnectedDeviceNum = ISTD_GetDeviceList(&gDeviceList[0], DEVICE_MAX_NUM);
    if (gConnectedDeviceNum < 0)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "Cannot access devices.");
    }

    printf("---- Connected devices:\n");

    for (n = 0; n < gConnectedDeviceNum; n++)
    {
        switch (gDeviceList[n].type)
        {
			case ISTD_DEVICE_IS_TWL_EMULATOR:
				printf("%3d: [IS-TWL-DEBUGGER] serial:%8d\n", n, gDeviceList[n].serial);
				break;
			case ISTD_DEVICE_IS_TWL_DEBUGGER:
				printf("%3d: [%s] serial:%8d\n", n, DEBUGGER_NAME, gDeviceList[n].serial);
				break;
			case ISTD_DEVICE_UNKNOWN:
				printf("%3d: unknown device %x:%x\n", n, (int)gDeviceList[n].id.eType, (int)gDeviceList[n].id.nSerial );
				break;
			default:
				printf("Illegal device\n");
				break;
        }
    }

    //---- ��������
    printf("Found %d device%s.\n", gConnectedDeviceNum, (gConnectedDeviceNum<=1)?"":"s" );

    myExit(EXIT_NUM_SHOW_DEVICES);
}

/*---------------------------------------------------------------------------*
  Name:         searchDevice

  Description:  search device

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void searchDevice(void)
{
	int		tmpMatch = -1;
	int		tmpMatchCount = 0;

    //---- �f�o�C�X�����������ꍇ
    if (gConnectedDeviceNum <= 0)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "found no device.");
    }

	//---- �����w�肵�ĂȂ��Ȃ�ŏ��̂���
    if (gDeviceTypeSpecified == ISTD_DEVICE_NONE && gDeviceSerialSpecified == DEVICE_SERIAL_NONE)
	{
        gCurrentDevice = 0;
	}
	//---- �f�o�C�X�w�肩�A�V���A���w�肪����
	else
	{
        int     n;
		char    tmpStr[DEVICE_SERIAL_STRING_MAX_SIZE];
		char    tmpStrLen;

        gCurrentDevice = -1;
        for (n = 0; n < gConnectedDeviceNum; n++)
        {
            //---- �w��f�o�C�X�Ƃ̃}�b�`����
            if (gDeviceTypeSpecified != ISTD_DEVICE_NONE && gDeviceTypeSpecified != gDeviceList[n].type)
            {
                continue;
            }

            //---- �w��V���A���Ƃ̃}�b�`����
            if (gDeviceSerialSpecified != DEVICE_SERIAL_NONE )
			{
				//---- ������
				if ( gDeviceSerialSpecified == gDeviceList[n].serial )
				{
					gCurrentDevice = n;
					break;
				}

				//---- �������Ȃ��̂ŕ����}�b�`����
				sprintf( tmpStr, "%d", gDeviceList[n].serial );
				tmpStrLen = strlen(tmpStr);

				if ( ( tmpStrLen > gDeviceSerialStringLen ) && 
					 ( ! strcmp( &tmpStr[ tmpStrLen - gDeviceSerialStringLen ], gDeviceSerialString ) ) )
				{
					tmpMatch = n;
					tmpMatchCount ++;
				}
            }
        }
    }

	//---- �����}�b�`�������̂�������
	if ( gCurrentDevice < 0 )
	{
		if ( tmpMatchCount == 1 )
		{
			gCurrentDevice = tmpMatch;
		}
		else if ( tmpMatchCount >= 2 )
		{
			displayErrorAndExit(EXIT_NUM_NO_DEVICE, "two or more devices matched to sperified serial number." );
		}
	}

    //---- �w�肵�����̂��Ȃ��A�܂��͊Ԉ���Ă���
    if (gCurrentDevice < 0
        || gDeviceList[gCurrentDevice].type == ISTD_DEVICE_NONE
        || gDeviceList[gCurrentDevice].type == ISTD_DEVICE_UNKNOWN)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "illegal device.");
    }
}

/*---------------------------------------------------------------------------*
  Name:         displayUsage

  Description:  �g������\������

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayUsage(void)
{
    fprintf(stderr,
            "TWL-SDK Development Tool - %s - Execute TWL ROM image\n"
            "Build %lu\n\n"
            "Usage: %s [OPTION] <SrlFile>\n"
            "\tdownload TWL srl file to %s and execute.\n\n"
            "Options:\n"
            "  --version                   : Show %s version.\n"
            "  --debuggerVersion           : Show debugger version.\n"
            "  -h, --help                  : Show this help.\n"
            "  -q, --quiet                 : Quiet mode.\n"
            "  -v, --verbose               : Verbose mode.\n"
            "  -L, --list                  : List connecting device.\n"
            "  -N, --nitro                 : Run hybrid rom as NITRO mode\n"
            "  -l, --lap                   : Show lap time at each line.\n"
//            "  -d, --type=DEVICE           : Specify device type.\n"
//            "                                DEVICE=TWLEMU|TWLDEB.\n"
            "  -A, --architecture          : Show architecture at each line.\n"
            "  -n, --console               : Show console number at each line.\n"
            "  -s, --serial=SERIAL         : Specify serial number.\n"
            "  -t, --timeout=SECOND        : Specify quit time after last print.\n"
            "  -T, --exec-timeout=SECOND   : Specify quit time after execute program.\n"
            "  -a, --abort-string=STRING   : Specify aborting string.\n"
            "  -c, --card-slot=SWITCH      : Card      slot SWITCH=ON|OFF, default OFF.\n"
            "  -C, --cartridge-slot=SWITCH : Cartridge slot SWITCH=ON|OFF, default OFF.\n",
//            "  --stdin, --standard-input   : Read data from stdin instead of <SrlFile>.\n\n",
			EXEC_NAME,
            SDK_DATE_OF_LATEST_FILE,
			EXEC_NAME,
			DEBUGGER_NAME,
			EXEC_NAME );
}

/*---------------------------------------------------------------------------*
  Name:         displayVersion

  Description:  �o�[�W�����\������

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayVersion(void)
{
    printf("*** %s: %s\n", EXEC_NAME, VERSION_STRING);
}

/*---------------------------------------------------------------------------*
  Name:         displayDebuggerVersion

  Description:  �f�o�b�K�o�[�W�����\������

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayDebuggerVersion(void)
{
	BOOL r;
	DWORD major;
	DWORD minor;
	DWORD build;
	DWORD revision;
	r = ISTD_GetVersion( &major, &minor, &build, &revision);

	if (r)
	{
		printf("*** %s: %s version is %d.%d.%04d.%04d\n",
			   EXEC_NAME, DEBUGGER_NAME, (int)major, (int)minor, (int)build, (int)revision);
	}
	else
	{
		printf("*** %s: failed to get %s version.\n", EXEC_NAME, DEBUGGER_NAME );
	}
	myExit(EXIT_NUM_NO_ERROR);
}

/*---------------------------------------------------------------------------*
  Name:         displayErrorAndExit

  Description:  �G���[��\������

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayErrorAndExit(int exitNum, char *message)
{
	if (gDebugMode)
	{
		printf("last error=%lx\n", ISTD_GetLastError());
	}

    printf("*** %s: Error: %s\n", EXEC_NAME, message);

    //---- DLL�̈���
    ISTD_FreeDll();

    exit(exitNum);
}

/*---------------------------------------------------------------------------*
  Name:         parseOption

  Description:  parse option line

  Arguments:    argc : argument count
                argv : argument vector

  Returns:      result. less than 0 if error.
 *---------------------------------------------------------------------------*/
void parseOption(int argc, char *argv[])
{
    int     n;
    int     c;
    BOOL    helpFlag = FALSE;

    struct option optionInfo[] = {
        {"help", no_argument, NULL, 'h'},
        {"quiet", no_argument, NULL, 'q'},
        {"verbose", no_argument, NULL, 'v'},
        {"list", no_argument, NULL, 'L'},
        {"nitro", no_argument, NULL, 'N'},
        {"lap", no_argument, NULL, 'l'},
        {"debug", no_argument, NULL, 'D'},      //�B���I�v�V����
        {"version", no_argument, NULL, '1'},
        {"debuggerVersion", no_argument, NULL, '2'},
//        {"stdin", no_argument, NULL, 'I'},
//        {"standard-input", no_argument, NULL, 'I'},
        {"type", required_argument, 0, 'd'},    //�B���I�v�V����
        {"serial", required_argument, 0, 's'},
        {"timeout", required_argument, 0, 't'},
        {"exec-timeout", required_argument, 0, 'T'},
        {"abort-string", required_argument, 0, 'a'},
        {"card-slot", required_argument, 0, 'c'},
        {"cartridge-slot", required_argument, 0, 'C'},
        {"architecture", no_argument, NULL, 'A'},
        {"console", no_argument, NULL, 'n'},
        {NULL, 0, 0, 0}
    };
    int     optionIndex;

    char   *optionStr = NULL;

    //---- suppress error string of getopt_long()
    opterr = 0;

    while (1)
    {
        c = getopt_long(argc, argv, "+hqvLNlDd:s:t:T:a:c:C:An", &optionInfo[0], &optionIndex);

        //printf("optind=%d optopt=%d  %x(%c) \n", optind, optopt, c,c );

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'I':                     //---- �W������
            gStdinMode = TRUE;
            break;
        case 'h':                     //---- �w���v�\��
            helpFlag = TRUE;
            break;
        case 'q':                     //---- quiet���[�h
            gQuietMode = TRUE;
            break;
        case 'v':                     //---- verbose���[�h
            gVerboseMode = TRUE;
            break;
        case 'D':                     //---- debug���[�h
            gDebugMode = TRUE;
            break;
        case '1':                     //---- �v���O�����o�[�W�����\��
            displayVersion();
            myExit(EXIT_NUM_SHOW_VERSION);
            break;
        case '2':                     //---- �f�o�b�K�o�[�W�����\��
			displayDebuggerVersion();
            myExit(EXIT_NUM_SHOW_VERSION);
			break;
        case 'L':                     //---- �f�o�C�X�̈ꗗ
            listDevice();
            break;
        case 'N':                     //---- hybrid �� NITRO���[�h�Ƃ���
            gIsForceNitro = TRUE;
            break;
        case 'l':                     //---- ���b�v�^�C��
            gShowLapTime = TRUE;
            break;
		case 'A':
			gShowArchitecture = TRUE; // �A�[�L�e�N�`���\��
			break;
		case 'n':
			gShowConsoleNum = TRUE;   // �R���\�[���ԍ��\��
			break;
        case 'd':                     //---- �f�o�C�X
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            {
                int     n;
                for (n = 0; gDeviceName[n]; n++)
                {
                    if (!strcmp(optionStr, gDeviceName[n]))
                    {
                        gDeviceTypeSpecified = gDeviceTypeArray[n];
                        break;
                    }
                }

                if (gDeviceTypeSpecified == ISTD_DEVICE_NONE)
                {
                    displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION, "illegal device type.");
                }
            }
            break;
        case 's':                     //---- �V���A���w��
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            gDeviceSerialSpecified = atoi(optionStr);
			strncpy(gDeviceSerialString, optionStr, DEVICE_SERIAL_STRING_MAX_SIZE);
			gDeviceSerialString[DEVICE_SERIAL_STRING_MAX_SIZE-1] = '\0';
			gDeviceSerialStringLen = strlen( gDeviceSerialString );
            break;
        case 'c':                     //---- �J�[�h�X���b�g���b�N
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            if (!strcmp(optionStr, "ON") || !strcmp(optionStr, "on"))
            {
                gIsCardLocked = TRUE;
            }
            else if (!strcmp(optionStr, "OFF") || !strcmp(optionStr, "off"))
            {
                gIsCardLocked = FALSE;
            }
            else
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION, "illegal value for card slot option.");
            }
            break;
        case 'C':                     //---- �J�[�g���b�W�X���b�g���b�N
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            if (!strcmp(optionStr, "ON") || !strcmp(optionStr, "on"))
            {
                gIsCartridgeLocked = TRUE;
            }
            else if (!strcmp(optionStr, "OFF") || !strcmp(optionStr, "off"))
            {
                gIsCartridgeLocked = FALSE;
            }
            else
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                    "illegal value for cartridge slot option.");
            }
            break;
        case 't':                     //---- �ŏI�\������̃^�C���A�E�g����
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            gTimeOutTime = atoi(optionStr);
            if (gTimeOutTime <= 0)
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                    "illegal value for abort timeout option.");
            }
            break;
        case 'T':                     //---- ���s�^�C���A�E�g����
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            gExecTimeOutTime = atoi(optionStr);
            if (gExecTimeOutTime <= 0)
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                    "illegal value for abort exec timeout option.");
            }
            break;
        case 'a':                     //---- �I��������
            gAbortString = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            {
                int     length = strlen(gAbortString);
                if (length <= 0 || length > 256)
                {
                    displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                        "illegal value for abort string option.");
                }
            }
            break;
        default:
            displayErrorAndExit(EXIT_NUM_UNKNOWN_OPTION, "unknown option.");
        }
    }

    //---- �w���v�\��
    {
        BOOL    isDisplayHelp = FALSE;

        if (helpFlag)
        {
            isDisplayHelp = TRUE;
        }
        else if (argc <= optind && !gStdinMode)
        {
            isDisplayHelp = TRUE;
        }
        else if (argc > optind && gStdinMode)
        {
            isDisplayHelp = TRUE;
        }

        if (isDisplayHelp)
        {
            displayUsage();
            exit(EXIT_NUM_SHOW_USAGE);
        }
    }

    //---- ���̓t�@�C����
    if (!gStdinMode)
    {
        strncpy(gInputFileNameString, argv[optind], FILE_NAME_MAX_SIZE);
    }

    if (gVerboseMode)
    {
        if (!gStdinMode)
        {
            printf("Input file is [%s]\n", gInputFileNameString);
        }
        else
        {
            printf("Input file is stdin\n");
        }
    }

    //---- ���\��
    if (gVerboseMode)
    {
        printf("Print time out : %d sec.\n", gTimeOutTime);
        printf("Execute time out : %d sec.\n", gExecTimeOutTime);
        printf("Card lock : %s.\n", (gIsCardLocked) ? "ON" : "OFF");
        printf("Cartridge lock : %s.\n", (gIsCartridgeLocked) ? "ON" : "OFF");

        if (gAbortString)
        {
            printf("Abort string : [%s]\n", gAbortString);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         loadFile

  Description:  load file

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void loadFile(void)
{
    unsigned int address = 0;

    //---- �t�@�C���I�[�v��
    if (gStdinMode)
    {
        gInputFile = stdin;
        _setmode(_fileno(gInputFile), O_BINARY);
    }
    else
    {
        if ((gInputFile = fopen(gInputFileNameString, "rb")) == NULL)
        {
            displayErrorAndExit(EXIT_NUM_NO_INPUT_FILE, "cannot open input file.");
        }
    }
    gIsInputFileOpened = TRUE;

	//---- �t�@�C���`�F�b�N
	checkFileFlag();

    //---- �f�o�C�X�ւ̐ڑ�
    if ((gDeviceHandle = ISTD_OpenEx(gDeviceList[gCurrentDevice].id)) == NULL)
    {
        displayErrorAndExit(EXIT_NUM_NOT_CONNECT, "cannot connect device.");
    }
    gDeviceConnected = TRUE;

    //---- ���Z�b�g�̔��s
    ISTD_Reset(gDeviceHandle, TRUE);
    Sleep(1000);

	//---- �v�����g�R�[���o�b�N�̐ݒ�
	ISTD_PrintfSetCB( gDeviceHandle, printCallback, (void*)0 );

	//---- �n�C�u���b�h�̍ۂ̃��[�h�ݒ�
	if ( ISTD_SetHybridMode )
	{
		ISTD_SetHybridMode( gDeviceHandle, gIsForceNitro? ISDHYBRID_NTR: ISDHYBRID_TWL );
	}
	else
	{
		if ( gIsForceNitro )
		{
			displayErrorAndExit(EXIT_NUM_LOADING_ERROR, "cannot run as NITRO mode. The required version of IS-TWL-DEBUGGER is v0.64 or later.");
		}
	}

	//---- ROM�f�[�^�_�E�����[�h���N��
	if ( ! ISTD_DownloadGo( gDeviceHandle, gInputFileNameString ) )
	{
		displayErrorAndExit(EXIT_NUM_LOADING_ERROR, "troubled while loading input file.");
	}

#if 0
    //---- 16KB���]������
    while (1)
    {
        char    buf[16384];
        size_t  size = fread(buf, 1, sizeof(buf), gInputFile);
        static int progressCount = 0;

        if (!size)
        {
            break;
        }

        //---- �]��
        if (!ISTD_WriteROM(gDeviceHandle, buf, address, size))
        {
            displayErrorAndExit(EXIT_NUM_LOADING_ERROR, "troubled while loading input file.");
        }

        address += size;

        if (gVerboseMode)
        {
            if (!(progressCount++ % 32))
            {
                printf("*");
            }
        }
    }
#endif

    //---- �t�@�C���N���[�Y
    if (gStdinMode)
    {
        _setmode(_fileno(gInputFile), O_TEXT);
    }
    else
    {
        fclose(gInputFile);
    }
    gIsInputFileOpened = FALSE;

    if (gVerboseMode)
    {
        printf("\nInput file size: %d (0x%x) byte\n", address, address);
    }
}

/*---------------------------------------------------------------------------*
  Name:         checkFileFlag

  Description:  ROM�w�b�_�̎��s�t���O�̃`�F�b�N

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void checkFileFlag(void)
{
	char buf[0x100];

	fseek( gInputFile, 0, SEEK_SET );
	fread( buf, 1, sizeof(buf), gInputFile );

	//---- �f�o�b�K���s�\���ǂ���
	if ( buf[RH_FLAG_OFFSET] & RH_FLAG_DEBUGGER_PROHIBITION )
	{
		displayErrorAndExit(EXIT_NUM_EXEC_PROHIBITION, "cannot exec this file. (debugger forbidden)");
	}

	//---- rewind
	rewind( gInputFile );
}

/*---------------------------------------------------------------------------*
  Name:         setSlopPower

  Description:  �X���b�g�̃p���[����

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void setSlotPower(void)
{
    //---- �J�[�g���b�W�X���b�g�����b�N����
    if (gIsCartridgeLocked)
    {
//        if (!ISNTD_CartridgeSlotPower(gDeviceHandle, TRUE))
        if (0)
        {
            displayErrorAndExit(EXIT_NUM_CANNOT_USE_CARTRIDGE, "cannot use cartridge slot.");
        }
    }

    //---- �J�[�h�X���b�g�����b�N����
    if (gIsCardLocked)
    {
//        if (!ISNTD_CardSlotPower(gDeviceHandle, TRUE))
        if (0)
        {
            displayErrorAndExit(EXIT_NUM_CANNOT_USE_CARD, "cannot use card slot.");
        }
    }

#if 0
    //---- ���Z�b�g�̉���
    Sleep(1000);
    ISTD_Reset(gDeviceHandle, FALSE);
#endif
}


/*---------------------------------------------------------------------------*
  Name:         procPrint

  Description:  printf����

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
TWLArch archNum[] = {
    TWLArchARM9,
    TWLArchARM7
};

#define PRINT_ONETIME_SIZE  512
#define PRINT_CONSOLE_MAX   4

//---- �������ĕ\�����邽�߂̃o�b�t�@
static char gConbineBuf[PRINT_CONSOLE_MAX][PRINT_ONETIME_SIZE * 2 + 2] = {"\0", "\0", "\0", "\0" };

static char *gConbineBufPtr[PRINT_CONSOLE_MAX] = {&gConbineBuf[0][0], 
												  &gConbineBuf[1][0], 
												  &gConbineBuf[2][0], 
												  &gConbineBuf[3][0] };

//---- �������ĕ�������r���邽�߂̃o�b�t�@
static char gLineBuf[PRINT_ONETIME_SIZE + 1];


void procPrintf(void)
{
    int     blankTime = 0;

    //---- �I��������̃T�C�Y
    gExitStrLength = strlen(OS_EXIT_STRING_1);

    while (1)
    {
        //---- ���[�U�Ɏ~�߂�ꂽ�ꍇ
        if (gStoppedByUser)
        {
            myExit(EXIT_NUM_USER_SIGNAL);
        }

        //---- �I�����邩
        if (gStringAborted || gExitAborted)
        {
            break;
        }

        //---- �����\�����Ă��Ȃ��ꍇ
        if (!gIsOutputString)
        {
            Sleep(100);
            blankTime += 100;

            //---- �^�C���A�E�g����
            if (gTimeOutTime && blankTime > gTimeOutTime * 1000)
            {
                gTimeOutOccured = TRUE;
                break;
            }
        }
        //---- �����\�������ꍇ
        else
        {
			gIsOutputString = FALSE;
            blankTime = 0;
        }

        //---- �^�C���A�E�g����
        if (gExecTimeOutTime > 0)
        {
            time_t  currentTime;
            (void)time(&currentTime);

            if (currentTime - gStartTime >= gExecTimeOutTime)
            {
                gExecTimeOutOccured = TRUE;
                break;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         printCallback

  Description:  �v�����g�R�[���o�b�N

  Arguments:    user    :
                arch    :
                console :
                recvBuf :
                size    :

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void printCallback( LPVOID user, ISDCPUARCH arch, DWORD console, const void* recvBuf, DWORD size )
{
//printf("%d arch:%d console:%d buf:%x size:%d\n", (int)user, (int)arch, (int)console, (int)recvBuf, (int)size );
	gIsOutputString = TRUE;
	outputString(arch, console, (char*)recvBuf, size);
}

/*---------------------------------------------------------------------------*
  Name:         showLapTime

  Description:  display lap time at line head

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void showLapTime(void)
{
    int     lap;
    time_t  currentTime;

    (void)time(&currentTime);
    lap = currentTime - gStartTime;

    printf("{%d:%02d}", lap / 60, lap % 60);
}

void showAddition(TWLArch arch, int console)
{
	static char* archStr[] = {"A9", "A7"};
	int archIndex = -1;

	if ( gIsLineHead[console] )
	{
		if ( gShowArchitecture )
		{
			//---- �A�[�L�e�N�`������
			if ( arch == TWLArchARM9 )
			{
				archIndex=0;
			}
			else if ( arch == TWLArchARM7 )
			{
				archIndex=1;
			}
		}
		if ( !gShowConsoleNum )
		{
			console = -1;
		}

		//---- �A�[�L�e�N�`���ƃR���\�[���ԍ��̕\��
		if ( archIndex<0 && console<0 )
		{
			// do nothing;
		}
		else if ( archIndex<0 )
		{
			printf("<%d>", console );
		}
		else if ( console<0 )
		{
			printf("<%s>", archStr[archIndex] );
		}
		else
		{
			printf("<%s:%d>", archStr[archIndex], console );
		}

		//---- �����\��
		if (!gQuietMode && gShowLapTime)
		{
			showLapTime();
		}
	}
}

/*---------------------------------------------------------------------------*
  Name:         outputString

  Description:  output string to stdout

  Arguments:    arch    : architecture(TWLArchARM9, TWLArchARM7, None)
                console : console num (0-3, -1)
                buf     : buffer
                bufSize : data size in buffer

  Returns:      FALSE if to do quit.
 *---------------------------------------------------------------------------*/
BOOL outputString(TWLArch arch, int console, char *buf, int bufSize)
{
    char   *bufEnd = buf + bufSize;
    char   *p = buf;

    int     abortStrLength = gAbortString ? strlen(gAbortString) : 0;

	static int prevConsole = -1;

	if ( prevConsole >= 0 && prevConsole != console )
	{
        gIsLineHead[prevConsole] = TRUE;
		printf("\n");
	}
	prevConsole = console;

    while (p < bufEnd)
    {
        char   *crPtr = strchr(p, '\n');

        //---- \n ���Ȃ�
        if (!crPtr)
        {
            //----��r�p�ɕۑ�
            strcat(gConbineBufPtr[console], p);
            gConbineBufPtr[console] += strlen(p);

			//---- �ǉ����
			showAddition( arch, console );
            gIsLineHead[console] = FALSE;

            //---- �\��
            printfIfNotQuiet(p);

            //---- �o�b�t�@�I�[�o�[�Ȃ�j��(\n�����Ă��Ȃ��o�͑��̐ӔC)
            if (gConbineBufPtr[console] - &gConbineBuf[console][0] > PRINT_ONETIME_SIZE)
            {
                gConbineBufPtr[console] = &gConbineBuf[console][0];
                *gConbineBufPtr[console] = '\0';
            }

            break;
        }

        //---- \n �܂ł��R�s�[
        {
            int     n = crPtr - p + 1;

            //---- ��r�p�Ɍ���
            strncpy(gConbineBufPtr[console], p, n);
            gConbineBufPtr[console][n] = '\0';

            //---- �\���p
            strncpy(&gLineBuf[0], p, n);
            gLineBuf[n] = '\0';
        }

		//---- �ǉ����
		showAddition( arch, console );
        gIsLineHead[console] = TRUE;

        //---- �s�\��
        printfIfNotQuiet(gLineBuf);

        //---- �����I��������Ƃ̔�r
        if (gAbortString && !strncmp(gConbineBuf[console], gAbortString, abortStrLength))
        {
            gStringAborted = TRUE;
            return FALSE;
        }

        //---- OS_Exit �ɂ��I��
        if (!strncmp(gConbineBuf[console], OS_EXIT_STRING_1, gExitStrLength))
        {
            gExitAborted = TRUE;
            gExitStatusNum = atoi(gConbineBuf[console] + gExitStrLength);
            return FALSE;
        }

        gConbineBufPtr[console] = &gConbineBuf[console][0];
        *gConbineBufPtr[console] = '\0';

        p = crPtr + 1;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         signalHandler

  Description:  signal handler

  Arguments:    sig
                argv : argument vector

  Returns:      ---
 *---------------------------------------------------------------------------*/
void signalHandler(int sig)
{
    gStoppedByUser = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         main

  Description:  main proc

  Arguments:    argc : argument count
                argv : argument vector

  Returns:      ---
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    //---- DLL ������
    ISTD_InitDll();

    //---- �W���o�͂̃o�b�t�@�����O�̖�����
    setvbuf(stdout, NULL, _IONBF, 0);

    //---- �I�v�V�������
    parseOption(argc, argv);

    //---- �f�o�C�X�ǂݍ���
    gConnectedDeviceNum = ISTD_GetDeviceList(&gDeviceList[0], DEVICE_MAX_NUM);
    if (gConnectedDeviceNum < 0)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "Cannot access devices.");
    }

    //---- �f�o�C�X����
    searchDevice();

    //---- �ǂݍ���
    loadFile();

    //---- �V�O�i���ݒ�
    (void)signal(SIGINT, signalHandler);

    //---- �X���b�g
    setSlotPower();

    //---- �X�^�[�g�����擾
    (void)time(&gStartTime);

    //----printf����
    procPrintf();

    //---- �I��
    if (gExitAborted)                  //---- OS_Exit() �I��
    {
        myExit(gExitStatusNum);
    }
    else if (gStringAborted)           //---- ������I��
    {
        myExit(EXIT_NUM_STRING_ABORT);
    }
    else if (gTimeOutOccured)          //---- �^�C���A�E�g
    {
        myExit(EXIT_NUM_TIME_OUT);
    }
    else if (gExecTimeOutOccured)
    {
        myExit(EXIT_NUM_EXEC_TIME_OUT);
    }
    else                               //---- �ʏ�I��
    {
        myExit(EXIT_NUM_NO_ERROR);
    }
    //---- never reached here

    //---- dummy to avoid warning
    return 0;
}
