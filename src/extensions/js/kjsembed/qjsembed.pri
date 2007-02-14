unix:OBJECTS_DIR = unix_obj
unix:MOC_DIR = unix_moc

win32:OBJECTS_DIR = win_obj
win32:MOC_DIR = win_moc

DEFINES += QT_ONLY
win32:DEFINES += WIN32 _WIN32 KJSEMBED_DLL

#DEBUG {
#CONFIG += debug warn_on
#}

#RELEASE {
CONFIG += release warn_off
#}

CONFIG += thread rtti
