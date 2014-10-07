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

GITSHA1HASHVALUE = $$system(git rev-parse HEAD)
DEFINES += GITSHA1HASH=$$sprintf("%1", $$GITSHA1HASHVALUE)

#### make solution for updating git hash value -- does not work under windows
#### (unfortunately, this always triggers the link process, because "rmappwindowobjectfile" is a phony target)
#rmappwindowobjectfile.target = rmappwindowobjectfile
#rmappwindowobjectfile.commands = [ `cat $$OBJECTS_DIR/gitsha1hashvalue` = $$GITSHA1HASHVALUE ] \
#		 || rm $$OBJECTS_DIR/specappwindow.o ;\
#		 echo $$GITSHA1HASHVALUE > $$OBJECTS_DIR/gitsha1hashvalue

#QMAKE_EXTRA_TARGETS += rmappwindowobjectfile
#PRE_TARGETDEPS += rmappwindowobjectfile
