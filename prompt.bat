@ECHO OFF

SET LANG=C-SJIS
SET HOME_TMP=%~dp0
SET PROJECT_ROOT=%HOME_TMP:\=/%
SET PROJECT_PROGDIR=%PROJECT_ROOT%prog/
SET PROJECT_ARCDIR=%PROJECT_PROGDIR%arc/
SET PROJECT_RSCDIR=%PROJECT_ROOT%resource/
SET WBLIBDIR=C:\home\pokemon_wb\lib
SET TAGFILE=c:\home\pokemon_wb\tags

REM SET PATH=%CYGNUSDIR%/native-99r1/H-i686-cygwin32/bin;%AGBDIR%/bin;%PATH%;.
REM SET PATH=%CYGNUSDIR%/thumbelf-000512/H-i686-cygwin32/bin;%PATH%
REM SET PATH=%AGBDIR%/bin;%PATH%

REM ##################################################
REM		�f�B���N�g���p�X��`
REM ##################################################
SET WBLIBDIR=%PROJECT_ROOT%lib
SET CWFolder_NITRO=C:\Program Files\Freescale\CW for NINTENDO DSi V1.2\
SET CWFOLDER_TWL=C:\Program Files\Freescale\CW for NINTENDO DSi V1.2\
SET CW_NITROSDK_ROOT=%WBLIBDIR%\TwlSDK
SET CW_TWLSDK_ROOT=%WBLIBDIR%\TwlSDK
SET LM_LICENSE_FILE=C:\Program Files\Freescale\CW for NINTENDO DSi V1.2\license.dat
SET NITROSDK_ROOT=%WBLIBDIR%\TwlSDK
SET NITROSYSTEM_ROOT=%WBLIBDIR%\TwlSystem
SET NITROWIFI_ROOT=%WBLIBDIR%\TwlWiFi
SET NITRODWC_ROOT=%WBLIBDIR%\TwlDWC

REM ####################################################
REM �n�C�u���b�h�Ή��̏ꍇ���L�̕�����L���ɂ��Ă������� �n�C�u���b�h�łȂ����͖����ɂ��Ă�������
REM NITRO�Ƃ�����IS-NITRO�p TWL�Ƃ�����TWL�p ���������Ɨ������o���オ��܂�
REM ####################################################
REM SET TWLSDK_PLATFORM=TWL NITRO
REM SET TARGET_PLATFORM=TWL NITRO

REM ####################################################
REM �n�C�u���b�h�łȂ��ꍇ���L�̕�����L���ɂ��Ă������� �n�C�u���b�h�̎��͖����ɂ��Ă�������
REM ####################################################
SET TWLSDK_PLATFORM=NITRO
SET TARGET_PLATFORM=NITRO

REM ####################################################
REM SD�J�[�h�Ƀf�o�b�O���O���o�͂���@�\��ON�ɂ��܂��B�n�C�u���b�h��DSi�̂ݓ����܂�
REM ####################################################
REM SET PM_DEBUG_SD_PRINT=yes


SET TWLSYSTEM_ROOT=%WBLIBDIR%\TwlSystem
SET TWLSDK_ROOT=%WBLIBDIR%\TwlSDK
SET TWLWIFI_ROOT=%WBLIBDIR%\TwlWiFi
SET TWLDWC_ROOT=%WBLIBDIR%\TwlDWC

SET TWLVCT_ROOT=%WBLIBDIR%\libVCT
SET TWLLIBVCT_ROOT=%WBLIBDIR%\libVCT
SET TWLCRYPTO_ROOT=%WBLIBDIR%\NitroCrypto
SET NITROVCT_ROOT=%WBLIBDIR%\libVCT
SET NITROLIBVCT_ROOT=%WBLIBDIR%\libVCT
SET NITROCRYPTO_ROOT=%WBLIBDIR%\NitroCrypto

SET GFL_MAKE_DIR=C:/home/gflib/gflib/
SET GFL_PROJECT_DIR=%PROJECT_ROOT%lib/gflib/
SET BMDS_SDK_ROOT=%PROJECT_ROOT%lib/BMDS_SDK/

SET SVN_EDITOR=c:\tools\vim\gvim

REM ##################################################
REM		�p�X�̒ǉ�
REM ##################################################
PATH=c:\tools;%PROJECT_ROOT%;%PROJECT_ROOT%\tools;c:\tools\subversion\bin;c:\cygwin\bin;c:\tools\vim;C:\tools;%NITROSDK_ROOT%\tools\bin;%NITROSYSTEM_ROOT%\tools\bin;%PATH%;

REM ##################################################
REM		�}�N����`
REM ##################################################
doskey alias=doskey /macros:all
doskey his=doskey /history
doskey vi=c:\tools\vim\gvim $*
doskey he=c:\progra~1\hidemaru\hidemaru $*
doskey gort = cd %PROJECT_ROOT:/=\%
doskey gopr = cd %PROJECT_PROGDIR:/=\%
doskey gors = cd %PROJECT_RSCDIR:/=\%
doskey ms = make ./bin/ARM9-TS/Release/main.srl

REM ##################################################
REM		���[�U�[�ʐݒ�̌Ăяo��
REM ##################################################
if exist "tools\personal\%USERNAME%_prompt.bat" call tools\personal\%USERNAME%_prompt.bat

REM �ʂ̃o�b�`�t�@�C������Ăяo����
REM ���ϐ���p�X�w�肾����L���ɂ���
REM ���̃o�b�`�t�@�C���ɖ߂��悤��cmd /F:ON���X���[�ł���悤�ɂ��Ă܂�
REM prompt.bat cmd hoge.bat�Ƃ���Ɗ��ϐ��ƃp�X�w���WB���ɂ�����ŁA
REM hoge.bat�����s���܂�
if %1.==. GOTO START
if %1==cmd ( call %2 & GOTO END )
:START
cmd /F:ON
:END

