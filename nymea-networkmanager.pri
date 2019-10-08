QMAKE_CXXFLAGS *= -Werror -std=c++1z -g
QMAKE_LFLAGS *= -std=c++1z

top_srcdir=$$PWD
top_builddir=$$shadowed($$PWD)

VERSION_STRING=$$system('dpkg-parsechangelog | sed -n -e "s/^Version: //p"')
DEFINES += VERSION_STRING=\\\"$${VERSION_STRING}\\\"
