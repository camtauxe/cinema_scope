#!/usr/bin/bash
set -e

case $TRAVIS_OS_NAME in
    linux)
        # Download and run qt ifw installer
        # (Thanks https://github.com/sgsaenger/vipster for example!)
        travis_wait wget https://download.qt.io/official_releases/qt-installer-framework/3.0.6/QtInstallerFramework-linux-x64.run -q -O qt_ifw_installer.run
        chmod +x qt_ifw_installer.run
        travis_wait ./qt_ifw_installer.run -platform minimal --script $TRAVIS_BUILD_DIR/travis/navigate_qt_ifw_installer.qs
        # Add installer framework to path
        export PATH="$HOME/Qt/QtIFW-3.0.6/bin:$PATH"
        ;;
    osx)
        brew install qt5
        brew install qt-ifw
        ;;
    windows)
        # Download and run qt installer
        travis_wait wget "http://download.qt.io/official_releases/online_installers/qt-unified-windows-x86-online.exe" -q -O qt_installer.exe
        travis_wait ./qt_installer.exe --script $TRAVIS_BUILD_DIR/travis/navigate_qt_installer_windows.qs
        # Add Mingw and qt to path
        export MWDIR="/c/Users/travis/Qt/Tools/mingw730_64"
        export QTDIR="/c/Users/travis/Qt/5.12.0/mingw73_64"
        export QTIFW="/c/Users/travis/Qt/Tools/QtInstallerFramework/3.0"
        export PATH="$MWDIR/bin:$QTDIR/bin:$QTIFW/bin:$PATH"
        ;;
esac