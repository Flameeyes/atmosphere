INCLUDES = -I$(top_srcdir)/src/common -I$(top_builddir)/src/common \
	$(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kcm_kcmatmosphere.la

kcm_kcmatmosphere_la_SOURCES = kcmatmosphere.cpp kcmmain.cpp adddevice.cpp \
	uiadddevice.ui uikcmmain.ui kcmatmosphere.skel
kcm_kcmatmosphere_la_LIBADD = $(top_builddir)/src/common/libatmosphere.la $(LIB_KIO) $(LIB_KDEUI)
kcm_kcmatmosphere_la_LDFLAGS = -avoid-version -module -no-undefined $(all_libraries)

xdg_apps_DATA = kcmatmosphere.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmatmosphere.pot
