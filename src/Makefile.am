# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/src/common $(all_includes) \
	-I$(top_builddir)/src/common

# these are the headers for your project
noinst_HEADERS = atmosphere.h devicewidget.h snmpthread.h interfacestats.h interfacethread.h cliwidget.h eventslogwidget.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/atmosphere.pot

KDE_ICON = atmosphere

bin_PROGRAMS = atmosphere

atmosphere_SOURCES = main.cpp atmosphere.cpp devicewidget.cpp snmpthread.cpp \
	interfacestats.cpp interfacethread.cpp cliwidget.cpp eventslogwidget.cpp uidevicewidget.ui \
	uiinterfacestats.ui uicliwidget.ui uisnmpsummary.ui uisnmpinterfaces.ui uisnmpip.ui \
	uisnmpipstats.ui uidevicelog.ui uieventslog.ui
atmosphere_LDFLAGS = $(KDE_RPATH) $(all_libraries)
atmosphere_LDADD = $(top_builddir)/src/common/libatmosphere.la $(KSNMP_LIBS) \
	$(LIB_KDEUI)
atmosphere_DEPENDENCIES = kcm/kcm_kcmatmosphere.la

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = atmosphere.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/atmosphere
shellrc_DATA = atmosphereui.rc eventsrc

SUBDIRS = common kcm . atmosd
