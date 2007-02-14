!define VER_MAJOR 1
!define VER_MINOR 0
!define VER_FILE 10
!define VER_DISPLAY "1.0 ${__TIMESTAMP__}"
!define APP_NAME QJSEmbed
!define APP_NAME_LC qjsembed


Outfile ${APP_NAME}-${VER_FILE}.exe
SetCompressor lzma

InstallDir $PROGRAMFILES\${APP_NAME}
InstallDirRegKey HKLM Software\SourceXtreme\${APP_NAME} ""

!include "Sections.nsh"
!include "MUI.nsh"

Name "${APP_NAME}"
Caption "${APP_NAME} ${VER_DISPLAY} Setup"
XPStyle on

!define MUI_ABORTWARNING
!define MUI_HEADERIMAGE 

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "lgpl.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
 
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"



Section "${APP_NAME} SDK (Required)" secSDK
        SectionIn RO
  SetDetailsPrint textonly
  DetailPrint "${APP_NAME} static library for embedding into Qt applications."
  SetDetailsPrint listonly

  SetOutPath "$WINDIR"
  

  SetOutPath "$INSTDIR"
  File lgpl.txt
  File ReadMe.txt
  SetOutPath "$INSTDIR"
  File kjsembed\qjsembed.dll
  SetOutPath "$INSTDIR\lib"
  File kjsembed\qjsembed.lib
  SetOutPath "$INSTDIR\include"
  File kjs\completion.h
  File kjs\list.h  
  File kjs\reference.h
  File kjs\ustring.h
  File kjs\function.h
  File kjs\lookup.h
  File kjs\reference_list.h
  File kjs\value.h
  File kjs\global.h
  File kjs\object.h
  File kjs\scope_chain.h
  File kjs\identifier.h
  File kjs\operations.h
  File kjs\simple_number.h
  File kjs\interpreter.h
  File kjs\property_map.h
  File kjs\types.h

  File kjsembed\kjsembedpart.h
  File kjsembed\jsconsolewidget.h
  File kjsembed\jssecuritypolicy.h


  ; Write the installation path into the registry
  WriteRegStr HKLM Software\SourceXtreme\${APP_NAME} "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayName" "${APP_NAME} ${VER_DISPLAY}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayVersion" "${VER_DISPLAY}"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoRepair" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "VersionMajor" "${VER_MAJOR}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "VersionMinor" "${VER_MINOR}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "URLInfoAbout" "http://www.sourcextreme.com/projects/${APP_NAME_LC}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "Publisher" "SourceXtreme, Inc"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "HelpLink" "oss@sourcextreme.com"

  WriteUninstaller "uninstall.exe"

SectionEnd

Section "${APP_NAME} Runtime" secRuntime
  SetOutPath "$INSTDIR"
  File qjscmd\qjscmd.exe
  File qjscmdw\qjscmdw.exe
  File stdlib\cmdline.js
  File "$%QTDIR%\bin\qt-mt*.dll"
SectionEnd

Section "${APP_NAME} Source" secSrc
  SetOutPath "$INSTDIR\source"
  File *.pro
  File *.pri
  File lgpl.txt
  File readme.txt
  File todo.txt


  SetOutPath "$INSTDIR\source\pcre"
  File pcre\*.pro
  File pcre\*.c
  File pcre\*.h
  
  SetOutPath "$INSTDIR\source\kjs"
  File kjs\*.pro
  File kjs\*.cpp
  File kjs\*.h

  SetOutPath "$INSTDIR\source\builtins"
  File builtins\*.pro
  File builtins\*.cpp
  File builtins\*.h

  SetOutPath "$INSTDIR\source\qtbindings"
  File qtbindings\*.pro
  File qtbindings\*.cpp
  File qtbindings\*.h

  SetOutPath "$INSTDIR\source\kjsembed"
  File kjsembed\*.pro
  File kjsembed\*.cpp
  File kjsembed\*.h

  SetOutPath "$INSTDIR\source\qjscmd"
  File qjscmd\*.pro
  File qjscmd\*.cpp

  
  SetOutPath "$INSTDIR\source\stdlib"
  File stdlib\*.js

  SetOutPath "$INSTDIR\source\tests"
  File tests\*.js
  File tests\*.ui
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts" secMenu

  SetOutPath "$INSTDIR"
  CreateDirectory "$SMPROGRAMS\${APP_NAME}"
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\ReadMe.lnk" "$INSTDIR\ReadMe.txt" "" "$INSTDIR\ReadMe.txt" 0
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\JSConsole.lnk" "$INSTDIR\qjscmdw.exe" "cmdline.js"


; Create internet links
  WriteINIStr "$SMPROGRAMS\${APP_NAME}\${APP_NAME} Webpage.url" "InternetShortcut" "URL" "http://www.sourcextreme.com/projects/${APP_NAME_LC}/"

SectionEnd

;Descriptions

LangString DESC_secSDK 		${LANG_ENGLISH} "${APP_NAME} static library for embedding into Qt applications."
LangString DESC_secRuntime 	${LANG_ENGLISH} "${APP_NAME} runtime for running scripts standalone."
LangString DESC_secMenu 	${LANG_ENGLISH} "Start menu shortcuts."
LangString DESC_secSrc 		${LANG_ENGLISH} "Source for building ${APP_NAME}."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${secSDK} $(DESC_secSDK)
!insertmacro MUI_DESCRIPTION_TEXT ${secRuntime} $(DESC_secRuntime)
!insertmacro MUI_DESCRIPTION_TEXT ${secMenu} $(DESC_secMenu)
!insertmacro MUI_DESCRIPTION_TEXT ${secSrc} $(DESC_secSrc)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; Uninstaller

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
  DeleteRegKey HKLM SOFTWARE\SourceXtreme\${APP_NAME}

  ; Remove files and uninstaller
  Delete "$INSTDIR\*.*"

  ; Remove source
  Delete "$INSTDIR\include\*.*"
  Delete "$INSTDIR\lib\*.*"
  Delete "$INSTDIR\source\pcre\*.*"
  Delete "$INSTDIR\source\kjsembed\*.*"
  Delete "$INSTDIR\source\kjs\*.*"
  Delete "$INSTDIR\source\qtbindings\*.*"
  Delete "$INSTDIR\source\builtins\*.*"
  Delete "$INSTDIR\source\qjscmd\*.*"
  Delete "$INSTDIR\source\tests\*.*"
  Delete "$INSTDIR\source\stdlib\*.*"
  Delete "$INSTDIR\source\*.*"

  Delete "$INSTDIR\src\*.*"
  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\${APP_NAME}\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\${APP_NAME}"
  RMDir "$INSTDIR\lib"
  RMDir "$INSTDIR\include"
  RMDir "$INSTDIR\source\pcre"
  RMDir "$INSTDIR\source\kjsembed"
  RMDir "$INSTDIR\source\kjs"
  RMDir "$INSTDIR\source\qtbindings"
  RMDir "$INSTDIR\source\builtins"
  RMDir "$INSTDIR\source\qjscmd"
  RMDir "$INSTDIR\source\tests"
  RMDir "$INSTDIR\source\stdlib"
  RMDir "$INSTDIR\source"

  RMDir "$INSTDIR"

SectionEnd
