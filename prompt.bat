@ECHO OFF

SET LANG=C-SJIS
SET HOME_TMP=%~dp0
SET PROJECT_ROOT=%HOME_TMP:\=/%
SET PROJECT_PROGDIR=%PROJECT_ROOT%prog/
SET PROJECT_ARCDIR=%PROJECT_PROGDIR%arc/
SET PROJECT_RSCDIR=%PROJECT_ROOT%resource/

REM SET PATH=%CYGNUSDIR%/native-99r1/H-i686-cygwin32/bin;%AGBDIR%/bin;%PATH%;.
REM SET PATH=%CYGNUSDIR%/thumbelf-000512/H-i686-cygwin32/bin;%PATH%
REM SET PATH=%AGBDIR%/bin;%PATH%

REM ##################################################
REM		�f�B���N�g���p�X��`
REM ##################################################
SET WBLIBDIR=%PROJECT_ROOT%\lib
SET CWFolder_NITRO=C:\Program Files\Freescale\CW for NINTENDO DS V2.0
SET CW_NITROSDK_ROOT=%WBLIBDIR%\NitroSDK
SET LM_LICENSE_FILE=C:\Program Files\Freescale\CW for NINTENDO DS V2.0\license.dat
SET NITROSDK_ROOT=%WBLIBDIR%\NitroSDK
SET NITROSYSTEM_ROOT=%WBLIBDIR%\NitroSystem
SET NITROWIFI_ROOT=%WBLIBDIR%\NitroWifi
SET NITRODWC_ROOT=%WBLIBDIR%\NitroDWC
SET NITROVCT_ROOT=%WBLIBDIR%\libVCT
SET NITROLIBVCT_ROOT=%WBLIBDIR%\libVCT


SET SVN_EDITOR=c:\tools\vim\gvim

REM ##################################################
REM		�p�X�̒ǉ�
REM ##################################################
PATH=%PROJECT_ROOT%;c:\cygwin\bin;%PATH%;c:\tools\vim;C:\tools;%NITROSDK_ROOT%\tools\bin;%NITROSYSTEM_ROOT%\tools\win\bin;

REM ##################################################
REM		�}�N����`
REM ##################################################
doskey alias=doskey /macros:all
doskey his=doskey /history
doskey vi=c:\tools\vim\gvim $*
doskey he=c:\progra~1\hidemaru\hidemaru $*
doskey ms = make ./bin/ARM9-TS/Release/main.srl

REM ##################################################
REM		���[�U�[�ʐݒ�̌Ăяo��
REM ##################################################
if exist "tools\personal\%USERNAME%_prompt.bat" call tools\personal\%USERNAME%_prompt.bat

cmd /F:ON
