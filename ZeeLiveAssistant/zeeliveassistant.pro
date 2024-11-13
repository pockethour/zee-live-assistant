QT       += core gui opengl gui-private network
macx: QT += macextras
win32:QT += winextras testlib
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zeeliveassistant
TEMPLATE = app

THIRDPARTY_PATH = $$PWD/../thirdparty
CONFIG(debug, debug|release) {
    DEPLOY_TYPE = debug
} else {
    DEPLOY_TYPE = release
}

OUTPUT_PWD=$$PWD/../$${DEPLOY_TYPE}
win32{
    BIN_DIR = $$OUTPUT_PWD
    LIB_DIR = $$OUTPUT_PWD
}
macx{
    BIN_DIR = $$OUTPUT_PWD
    LIB_DIR = $$OUTPUT_PWD
}

contains(TEMPLATE, "lib") {
    DLLDESTDIR = $$BIN_DIR
    DESTDIR = $$LIB_DIR
} else {
    DESTDIR = $$BIN_DIR
}

macx {
#QMAKE_LFLAGS = -Wl,-rpath,@executable_path/../Library/,-rpath,@executable_path/../Library/lib/,-rpath,@executable_path/../Library/plugins/,-rpath,@executable_path/../Frameworks/
}

macx {
    QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@rpath/
    CONFIG += unversioned_libname unversioned_soname
}

CONFIG(release, debug|release){
macx {
    DEFINES += NDEBUG
    QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_OBJECTIVE_CFLAGS_RELEASE = $$QMAKE_OBJECTIVE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
}
}

win32 {
    QMAKE_CXXFLAGS_RELEASE += /Zi
    QMAKE_CXXFLAGS_RELEASE += /Od
    QMAKE_LFLAGS_RELEASE += /DEBUG
}

win32 {
    INCLUDEPATH += \
    $$THIRDPARTY_PATH/opencv/win/release/include \
    $$THIRDPARTY_PATH/ffmpeg/win/ffmpeg-4.2.8-win64-dev/include \
    $$THIRDPARTY_PATH/imageeffect/include \
    $$THIRDPARTY_PATH/eeocamera/include \
    $$THIRDPARTY_PATH/sdl2/win/include
}

macx {
    INCLUDEPATH += \
    $$THIRDPARTY_PATH/ffmpeg/macx/include \
    $$THIRDPARTY_PATH/opencv/macx/include/opencv4  \
    $$THIRDPARTY_PATH/eeocamera/include \
    $$THIRDPARTY_PATH/sdl2/macx/include
}

CONFIG(debug, debug|release) {
    win32 {
        QMAKE_LFLAGS_DEBUG += /SAFESEH:NO
        LIBS += -L$$THIRDPARTY_PATH/opencv/win/debug/x64/vc16/lib \
        -lopencv_imgproc430d \
        -lopencv_imgcodecs430d \
        -lopencv_core430d \
        -lopencv_highgui430d \
        -lopencv_video430d \
        -lopencv_calib3d430d

        LIBS += -L$$THIRDPARTY_PATH/arithmetic_dnnl/win/debug \
            -leeodnnl
        LIBS += -L$$THIRDPARTY_PATH/sdl2/win/lib \
            -lSDL2
        LIBS += -L$$THIRDPARTY_PATH/sdl2/win/lib \
            -lSDL2main
    }
    macx {
    LIBS += -L$$THIRDPARTY_PATH/opencv/macx/lib \
    -lopencv_imgproc \
    -lopencv_imgcodecs \
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_video \
    -lopencv_calib3d

    LIBS += -L$$THIRDPARTY_PATH/arithmetic_dnnl/mac/debug \
    -leeodnnl
    }
} else{
    win32 {
        QMAKE_LFLAGS_RELEASE += /SAFESEH:NO
        LIBS += -L$$THIRDPARTY_PATH/opencv/win/release/x64/vc16/lib \
        -lopencv_imgproc430 \
        -lopencv_imgcodecs430 \
        -lopencv_core430 \
        -lopencv_highgui430 \
        -lopencv_video430 \
        -lopencv_calib3d430

         LIBS += -L$$THIRDPARTY_PATH/imageeffect/win/release \
            -limage_effect
        LIBS += -L$$THIRDPARTY_PATH/arithmetic_dnnl/win/release \
            -leeodnnl
        LIBS += -L$$THIRDPARTY_PATH/eeocamera/win/lib \
            -lEeoCamera
        LIBS += -lShcore
    }
    macx {
    LIBS += -L$$THIRDPARTY_PATH/opencv/macx/lib \
    -lopencv_imgproc \
    -lopencv_imgcodecs \
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_video \
    -lopencv_calib3d

    LIBS += -L$$THIRDPARTY_PATH/arithmetic_dnnl/mac/release \
             -leeodnnl
    LIBS += -L$$THIRDPARTY_PATH/eeocamera/macx/lib \
             -lEeoCamera
    LIBS += -F$$THIRDPARTY_PATH/sdl2/macx/
    LIBS += -framework SDL2
    }
}


defineTest(delFile) {
    TARGET_FILE = $${1}
    win32 {
        TARGET_FILE ~= s,/,\\,g
    }
    win32 {
        QMAKE_POST_LINK += $$quote($${QMAKE_DEL_FILE} $${TARGET_FILE}$$escape_expand(\n\t))
    }
    macx {
        QMAKE_POST_LINK += $$quote($${QMAKE_DEL_FILE} $${TARGET_FILE}$$escape_expand(\n\t))
    }
    export(QMAKE_POST_LINK)
    return(true)
}

defineTest(copyFile) {
    SRC_FILE = $${1}
    TARGET_FILE = $${2}
    win32 {
        SRC_FILE ~= s,/,\\,g
        TARGET_FILE ~= s,/,\\,g
    }
    win32:QMAKE_POST_LINK += $$quote($${QMAKE_COPY_FILE} $${SRC_FILE} $${TARGET_FILE}$$escape_expand(\n\t))
    macx:QMAKE_POST_LINK += $$quote($${QMAKE_COPY_FILE} -a $${SRC_FILE} $${TARGET_FILE}$$escape_expand(\n\t))
    export(QMAKE_POST_LINK)
    return(true)
}

defineTest(copyDir) {
    SRC_DIR = $${1}
    TARGET_DIR = $${2}
    win32 {
        SRC_DIR ~= s,/,\\,g
        TARGET_DIR ~= s,/,\\,g
    }
    macx {
        SRC_DIR = $${SRC_DIR}
    }
    win32:QMAKE_POST_LINK += $$quote($${QMAKE_COPY_DIR} $${SRC_DIR} $${TARGET_DIR}$$escape_expand(\n\t))
    macx: QMAKE_POST_LINK += $$quote($${QMAKE_COPY_DIR} -a $${SRC_DIR} $${TARGET_DIR}$$escape_expand(\n\t))
    export(QMAKE_POST_LINK)
    return(true)
}

defineTest(copyMacDir) {
    SRC_DIR = $${1}
    TARGET_DIR = $${2}
    win32 {
        SRC_DIR ~= s,/,\\,g
        TARGET_DIR ~= s,/,\\,g
    }
    macx {
        SRC_DIR = $${SRC_DIR}/
    }
    QMAKE_POST_LINK += $$quote($${QMAKE_COPY_DIR} $${SRC_DIR} $${TARGET_DIR}$$escape_expand(\n\t))
    export(QMAKE_POST_LINK)
    return(true)
}

defineTest(rmdir) {
    TARGET_DIR = $${1}
    win32 {
        TARGET_DIR ~= s,/,\\,g
    }
    QMAKE_POST_LINK += $$quote($$QMAKE_DEL_TREE $${TARGET_DIR}$$escape_expand(\n\t))
    export(QMAKE_POST_LINK)
    return(true)
}

defineTest(mkdir) {
    TARGET_DIR = $${1}
    win32 {
        TARGET_DIR ~= s,/,\\,g
        QMAKE_POST_LINK += $$quote($$QMAKE_CHK_DIR_EXISTS $${TARGET_DIR} $$QMAKE_MKDIR $${TARGET_DIR}$$escape_expand(\n\t))
    }
    macx {
        QMAKE_POST_LINK += $$quote($$QMAKE_MKDIR $${TARGET_DIR}$$escape_expand(\n\t))
    }
    export(QMAKE_POST_LINK)
    return(true)
}


contains(CONFIG, static){
}else{
    win32 {

        DEPLOY_TOOL = $${dirname(QMAKE_QMAKE)}/windeployqt
        DEPLOY_TOOL = $$replace(DEPLOY_TOOL, /, \\)

        DEPLOY_TARGET = $${PWD}/../$${DEPLOY_TYPE}/$${TARGET}.exe
        DEPLOY_TARGET=$$replace(DEPLOY_TARGET, /, \\)

        # copy thirdparty dll files
        copyFile($${THIRDPARTY_PATH}/arithmetic_dnnl/win/$${DEPLOY_TYPE}/* , $${OUTPUT_PWD})
        copyFile($${THIRDPARTY_PATH}/ffmpeg/win/ffmpeg-4.2.8-win64-shared/bin/*.dll , $${OUTPUT_PWD})
        copyFile($${THIRDPARTY_PATH}/opencv/win/$${DEPLOY_TYPE}/x64/vc16/bin/*.dll , $${OUTPUT_PWD})
        #copyFile($${THIRDPARTY_PATH}/misc/* , $${OUTPUT_PWD})
        copyFile($${THIRDPARTY_PATH}/eeocamera/win/lib/*.dll , $${OUTPUT_PWD})
        copyFile($${THIRDPARTY_PATH}/imageeffect/win/$${DEPLOY_TYPE}/* , $${OUTPUT_PWD})
        copyFile($${THIRDPARTY_PATH}/qtav/win/$${DEPLOY_TYPE}/QtAV* , $${OUTPUT_PWD})
        msvc {
            QMAKE_POST_LINK+=$$DEPLOY_TOOL --no-angle --no-opengl-sw --$$DEPLOY_TYPE $$DEPLOY_TARGET $$OUTPUT_PWD $$escape_expand(\\n)
            message("cmd is $${QMAKE_POST_LINK}")
        }else{
            QMAKE_POST_LINK+=$$DEPLOY_TOOL --no-angle --no-opengl-sw  $$DEPLOY_TARGET
        }
    }
macx{
    DEPLOY_TOOL = $${dirname(QMAKE_QMAKE)}/macdeployqt
    DEPLOY_TOOL = $$replace(DEPLOY_TOOL, /, \\)
    MAC_OUTPUT_PWD="$$OUTPUT_PWD/$${TARGET}.$${TEMPLATE}/Contents/Library"
    MAC_OUTPUT_RES="$$OUTPUT_PWD/$${TARGET}.$${TEMPLATE}/Contents/Resources"
    MAC_OUTPUT_FRA="$$OUTPUT_PWD/$${TARGET}.$${TEMPLATE}/Contents/Frameworks"
    MAC_OUTPUT_OS="$$OUTPUT_PWD/$${TARGET}.$${TEMPLATE}/Contents/MacOS"
    MAC_OUTPUT_PLG="$$OUTPUT_PWD/$${TARGET}.$${TEMPLATE}/Contents/PlugIns"
    mkdir($${MAC_OUTPUT_PWD})
    mkdir($${MAC_OUTPUT_RES})
    mkdir($${MAC_OUTPUT_FRA})
    mkdir($${MAC_OUTPUT_OS})
    mkdir($${MAC_OUTPUT_PLG})
    # copy thirdparty dylib files
    copyFile($${THIRDPARTY_PATH}/arithmetic_dnnl/mac/$${DEPLOY_TYPE}/*.dylib , $${MAC_OUTPUT_PWD})
    copyFile($${THIRDPARTY_PATH}/ffmpeg/macx/lib/*.dylib , $${MAC_OUTPUT_PWD})
    copyFile($${THIRDPARTY_PATH}/opencv/macx/lib/*.dylib , $${MAC_OUTPUT_PWD})
    #copyFile($${THIRDPARTY_PATH}/misc/* , $${MAC_OUTPUT_RES})
    copyFile($${THIRDPARTY_PATH}/eeocamera/macx/lib/*.dylib , $${MAC_OUTPUT_PWD})
    copyFile($${THIRDPARTY_PATH}/others/macx/lib/*.dylib  $${MAC_OUTPUT_PWD})
    copyFile($${THIRDPARTY_PATH}/others/demo.plist.txt  $${MAC_OUTPUT_PWD}/../Info.plist)
    copyFile($${THIRDPARTY_PATH}/qtav/macx/$${DEPLOY_TYPE}/libQtAV* , $${MAC_OUTPUT_PWD})

    QTDIR = $$[QT_HOST_PREFIX]
    QMAKE_LFLAGS += -Wl,-rpath,@executable_path/../Library/,-rpath,@executable_path/../Frameworks/
    QMAKE_POST_LINK += $$quote(export PATH=$${QTDIR}:$${QTDIR}/bin$$escape_expand(\n\t)) # colon :::
    QMAKE_POST_LINK += $$quote($${QTDIR}/bin/macdeployqt $${OUTPUT_PWD}/$${TARGET}.$${TEMPLATE} -verbose=-1$$escape_expand(\n\t)) # -verbose=-1 is a hack to ignore errors (about sql deployment)
}
}


win32 {
    LIBS += -L$$THIRDPARTY_PATH/ffmpeg/win/ffmpeg-4.2.8-win64-dev/lib
}
macx {
    LIBS += -L$$THIRDPARTY_PATH/ffmpeg/macx/lib
}

LIBS += -lavutil -lavcodec -lavdevice -lswscale -lavformat -lavfilter -lswresample

SOURCES += main.cpp\
    httpserver.cpp \
        player.cpp \
    xvideowidget.cpp \
    rgbrenderer.cpp

HEADERS  += player.h \
    httpserver.h \
    xvideowidget.h \
    rgbrenderer.h

FORMS    += player.ui

RESOURCES +=

DISTFILES +=
