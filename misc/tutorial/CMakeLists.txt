
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############




#original Makefile.am contents follow:

#
#
#all-local: gyrodata.dat
#gyrodata.dat: $(srcdir)/gyrodata.dat.gz
#	gzip -d < $(srcdir)/gyrodata.dat.gz > gyrodata.dat
#
#install-data-local: gyrodata.dat
#	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/kst/tutorial
#	$(INSTALL_DATA) "$(srcdir)/all.kst" "$(DESTDIR)$(kde_datadir)/kst/tutorial/all.kst"
#	$(INSTALL_DATA) "$(srcdir)/boomB.dat" "$(DESTDIR)$(kde_datadir)/kst/tutorial/boomB.dat"
#	$(INSTALL_DATA) "$(srcdir)/cbi.dat" "$(DESTDIR)$(kde_datadir)/kst/tutorial/cbi.dat"
#	$(INSTALL_DATA) "$(srcdir)/dasi.dat" "$(DESTDIR)$(kde_datadir)/kst/tutorial/dasi.dat"
#	$(INSTALL_DATA) "$(srcdir)/boomAB.dat" "$(DESTDIR)$(kde_datadir)/kst/tutorial/boomAB.dat"
#	$(INSTALL_DATA) "$(srcdir)/boom.dat" "$(DESTDIR)$(kde_datadir)/kst/tutorial/boom.dat"
#	$(INSTALL_DATA) "$(srcdir)/combined.dat" "$(DESTDIR)$(kde_datadir)/kst/tutorial/combined.dat"
#	$(INSTALL_DATA) "$(srcdir)/demo.kst" "$(DESTDIR)$(kde_datadir)/kst/tutorial/demo.kst"
#	$(INSTALL_DATA) "$(srcdir)/maxima.dat" "$(DESTDIR)$(kde_datadir)/kst/tutorial/maxima.dat"
#	$(INSTALL_DATA) "$(srcdir)/boomA.dat" "$(DESTDIR)$(kde_datadir)/kst/tutorial/boomA.dat"
#	$(INSTALL_DATA) "$(srcdir)/boom_lce.dat" "$(DESTDIR)$(kde_datadir)/kst/tutorial/boom_lce.dat"
#	$(INSTALL_DATA) gyrodata.dat "$(DESTDIR)$(kde_datadir)/kst/tutorial/gyrodata.dat"
#	$(INSTALL_DATA) "$(srcdir)/vsa.dat" "$(DESTDIR)$(kde_datadir)/kst/tutorial/vsa.dat"
#
#
