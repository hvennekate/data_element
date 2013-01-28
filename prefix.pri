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
	build_pass:CONFIG(release) {
		TARGET = data_element-release
	}
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
	build_pass:CONFIG(release) {
		TARGET = data_element-release.exe
	}
}



DEFINES += FILECHECKRANDOMNUMBER=469222828 FILECHECKCOMPRESSNUMBER=469333828
