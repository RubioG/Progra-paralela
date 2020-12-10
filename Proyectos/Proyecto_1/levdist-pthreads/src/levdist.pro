TEMPLATE = app
QT -= core
CONFIG += console
CONFIG -= qt app_bundle
QMAKE_CXXFLAGS += -std=c++11 -pthread
LIBS += -pthread

HEADERS += \
	arguments.h \
	concurrency.h \
	dir.h \
	levdist.h \
	queue.h \
    levenshtein.h \
    tree.h

SOURCES += \
	arguments.c \
	concurrency.c \
	dir.c \
	levdist.c \
	main.c \
	queue.c \
    levenshtein.c \
    tree.c
