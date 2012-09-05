unix {
	OBJECTS_DIR = ../build/objects
	MOC_DIR = ../build/moc
	DESTDIR = ../build/target
	DLLDESTDIR = ../build/target
	UI_DIR = ../build/uic
	UI_HEADERS_DIR = ../build/uic-headers
	UI_SOURCES_DIR = ../build/uic-src
	LIBS += -L../build/target
	RCC_DIR = ../build/resources
}
win32 {
	OBJECTS_DIR = ../winbuild/objects
	MOC_DIR = ../winbuild/moc
	DESTDIR = ../winbuild/target
	DLLDESTDIR = ../winbuild/target
	UI_DIR = ../winbuild/uic
	UI_HEADERS_DIR = ../winbuild/uic-headers
	UI_SOURCES_DIR = ../winbuild/uic-src
	LIBS += -L../winbuild/target
	RCC_DIR = ../winbuild/resources
}



DEFINES += FILECHECKRANDOMNUMBER=469222828
