
echo off

REM ============================================
REM     通常版を作成
REM ============================================
call %PROJECT_ROOT%prompt.bat cmd mkgl.bat %1
call %PROJECT_ROOT%twl_prompt.bat cmd mkgl.bat %1


REM ============================================
REM     ROM版を作るために現状の定義を保存
REM ============================================
SET TMP_PM_DEBUG=%PM_DEBUG%
SET TMP_NITRO_FINALROM=%NITRO_FINALROM%
SET TMP_TWL_FINALROM=%TWL_FINALROM%


REM ============================================
REM     ROM用定義指定
REM ============================================
SET PM_DEBUG=no
SET NITRO_FINALROM=yes
SET TWL_FINALROM=yes


REM ============================================
REM     ROM版作成
REM ============================================
call %PROJECT_ROOT%prompt.bat cmd mkgl.bat %1
call %PROJECT_ROOT%twl_prompt.bat cmd mkgl.bat %1


REM ============================================
REM     定義を元に戻す
REM ============================================
SET PM_DEBUG=%TMP_PM_DEBUG%
SET NITRO_FINALROM=%TMP_NITRO_FINALROM%
SET TWL_FINALROM=%TMP_TWL_FINALROM%
