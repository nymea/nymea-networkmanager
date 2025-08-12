greaterThan(QT_MAJOR_VERSION, 5) {
    message("Building using Qt6 support")
    CONFIG *= c++17
    QMAKE_LFLAGS *= -std=c++17
    QMAKE_CXXFLAGS *= -std=c++17
} else {
    message("Building using Qt5 support")
    CONFIG *= c++11
    QMAKE_LFLAGS *= -std=c++11
    QMAKE_CXXFLAGS *= -std=c++11
    DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x050F00
}

QMAKE_CXXFLAGS *= -Werror -g

top_srcdir=$$PWD
top_builddir=$$shadowed($$PWD)

VERSION_STRING=$$system('dpkg-parsechangelog | sed -n -e "s/^Version: //p"')
DEFINES += VERSION_STRING=\\\"$${VERSION_STRING}\\\"
