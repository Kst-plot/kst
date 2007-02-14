
add_subdirectory( src )
add_subdirectory( misc )
add_subdirectory( tests )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############




#original Makefile.am contents follow:

#SUBDIRS = src misc tests
#
#EXTRA_DIST = AUTHORS COPYING COPYING.LGPL ChangeLog INSTALL README TODO RELEASE.NOTES kst.lsm 
#
#changes:
#	(cd $(srcdir) && svn log --xml | xsltproc svn2cl.xsl - > ChangeLog)
#
#svnchanges: changes
#	(cd $(srcdir) && svn commit -m 'update changelog' ChangeLog)
#
