INCLUDES = -I$(top_srcdir)/src/common $(all_includes) \
	-I$(top_builddir)/src/common
METASOURCES = AUTO

services_DATA = atmosd.desktop
servicesdir = $(kde_servicesdir)/kded

kde_module_LTLIBRARIES = kded_atmosd.la
kded_atmosd_la_LDFLAGS = -avoid-version -module $(all_libraries)
noinst_HEADERS = kdedatmos.h
kded_atmosd_la_SOURCES = kdedatmos.cpp kdedatmos.skel
kded_atmosd_la_LIBADD = $(top_builddir)/src/common/libatmosphere.la \
	$(KSNMP_LIBS) $(LIB_KDEUI)
