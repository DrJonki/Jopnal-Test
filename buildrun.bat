call android update project --target "android-18" --subprojects --path .
call ndk-build NDK_DEBUG=1
call :CHECK_FAIL
call ant clean
call ant debug
call adb install -r bin/NativeActivity-debug.apk
call run.bat

:: /// check if the app has failed
:CHECK_FAIL
@echo off
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)