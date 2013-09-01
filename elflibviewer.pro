
OTHER_FILES += \
    README \
    locationbar-erase.png \
    elflibviewer.png \
    elflibviewer.desktop \
    document-open.png \
    application-exit.png \
    README.md

HEADERS += \
    MainWindowImpl.h \
    LibrariesInfo.h

SOURCES += \
    MainWindowImpl.cpp \
    main.cpp \
    LibrariesInfo.cpp

FORMS += \
    elflibviewer.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS += translations/i18n_ru.ts
