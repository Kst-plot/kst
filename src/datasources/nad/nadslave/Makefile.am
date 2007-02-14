INCLUDES = $(all_includes) -I$(src_dir)/..
kde_module_LTLIBRARIES = kio_nad.la

kio_nad_la_SOURCES = nadslave.cpp
kio_nad_la_LIBADD = $(LIB_KIO)
kio_nad_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) 
noinst_HEADERS = nadslave.h

METASOURCES = AUTO

kdelnkdir = $(kde_servicesdir)
kdelnk_DATA = nad.protocol

KDE_ICON = piolib dirfile

mimedir = $(kde_mimedir)/application
mime_DATA = x-piolib.desktop x-dirfile.desktop

