#############################################################################
# Makefile for building: lipstick-socam
# Generated by qmake (2.01a) (Qt 4.8.3) on: Tue Oct 8 07:15:31 2013
# Project:  lipstick-socam.pro
# Template: subdirs
# Command: /usr/lib/qt4/bin/qmake -o Makefile lipstick-socam.pro
#############################################################################

first: make_default
MAKEFILE      = Makefile
QMAKE         = /usr/lib/qt4/bin/qmake
DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
INSTALL_FILE  = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
INSTALL_DIR   = $(COPY_DIR)
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
SUBTARGETS    =  \
		sub-src \
		sub-plugin \
		sub-tools

src/$(MAKEFILE): 
	@$(CHK_DIR_EXISTS) src/ || $(MKDIR) src/ 
	cd src/ && $(QMAKE) /home/ramoncin/socam/svn/socam/trunk/UX/lipstick/src/src.pro -o $(MAKEFILE)
sub-src-qmake_all:  FORCE
	@$(CHK_DIR_EXISTS) src/ || $(MKDIR) src/ 
	cd src/ && $(QMAKE) /home/ramoncin/socam/svn/socam/trunk/UX/lipstick/src/src.pro -o $(MAKEFILE)
sub-src: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE)
sub-src-make_default-ordered: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) 
sub-src-make_default: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) 
sub-src-make_first-ordered: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) first
sub-src-make_first: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) first
sub-src-all-ordered: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) all
sub-src-all: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) all
sub-src-clean-ordered: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) clean
sub-src-clean: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) clean
sub-src-distclean-ordered: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) distclean
sub-src-distclean: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) distclean
sub-src-install_subtargets-ordered: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) install
sub-src-install_subtargets: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) install
sub-src-uninstall_subtargets-ordered: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) uninstall
sub-src-uninstall_subtargets: src/$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) uninstall
plugin/$(MAKEFILE): 
	@$(CHK_DIR_EXISTS) plugin/ || $(MKDIR) plugin/ 
	cd plugin/ && $(QMAKE) /home/ramoncin/socam/svn/socam/trunk/UX/lipstick/plugin/plugin.pro -o $(MAKEFILE)
sub-plugin-qmake_all:  FORCE
	@$(CHK_DIR_EXISTS) plugin/ || $(MKDIR) plugin/ 
	cd plugin/ && $(QMAKE) /home/ramoncin/socam/svn/socam/trunk/UX/lipstick/plugin/plugin.pro -o $(MAKEFILE)
sub-plugin: plugin/$(MAKEFILE) FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE)
sub-plugin-make_default-ordered: plugin/$(MAKEFILE) sub-src-make_default-ordered  FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) 
sub-plugin-make_default: plugin/$(MAKEFILE) FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) 
sub-plugin-make_first-ordered: plugin/$(MAKEFILE) sub-src-make_first-ordered  FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) first
sub-plugin-make_first: plugin/$(MAKEFILE) FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) first
sub-plugin-all-ordered: plugin/$(MAKEFILE) sub-src-all-ordered  FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) all
sub-plugin-all: plugin/$(MAKEFILE) FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) all
sub-plugin-clean-ordered: plugin/$(MAKEFILE) sub-src-clean-ordered  FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) clean
sub-plugin-clean: plugin/$(MAKEFILE) FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) clean
sub-plugin-distclean-ordered: plugin/$(MAKEFILE) sub-src-distclean-ordered  FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) distclean
sub-plugin-distclean: plugin/$(MAKEFILE) FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) distclean
sub-plugin-install_subtargets-ordered: plugin/$(MAKEFILE) sub-src-install_subtargets-ordered  FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) install
sub-plugin-install_subtargets: plugin/$(MAKEFILE) FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) install
sub-plugin-uninstall_subtargets-ordered: plugin/$(MAKEFILE) sub-src-uninstall_subtargets-ordered  FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) uninstall
sub-plugin-uninstall_subtargets: plugin/$(MAKEFILE) FORCE
	cd plugin/ && $(MAKE) -f $(MAKEFILE) uninstall
tools/$(MAKEFILE): 
	@$(CHK_DIR_EXISTS) tools/ || $(MKDIR) tools/ 
	cd tools/ && $(QMAKE) /home/ramoncin/socam/svn/socam/trunk/UX/lipstick/tools/tools.pro -o $(MAKEFILE)
sub-tools-qmake_all:  FORCE
	@$(CHK_DIR_EXISTS) tools/ || $(MKDIR) tools/ 
	cd tools/ && $(QMAKE) /home/ramoncin/socam/svn/socam/trunk/UX/lipstick/tools/tools.pro -o $(MAKEFILE)
sub-tools: tools/$(MAKEFILE) sub-src FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE)
sub-tools-make_default-ordered: tools/$(MAKEFILE) sub-plugin-make_default-ordered  FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) 
sub-tools-make_default: tools/$(MAKEFILE) sub-src-make_default FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) 
sub-tools-make_first-ordered: tools/$(MAKEFILE) sub-plugin-make_first-ordered  FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) first
sub-tools-make_first: tools/$(MAKEFILE) sub-src-make_first FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) first
sub-tools-all-ordered: tools/$(MAKEFILE) sub-plugin-all-ordered  FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) all
sub-tools-all: tools/$(MAKEFILE) sub-src-all FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) all
sub-tools-clean-ordered: tools/$(MAKEFILE) sub-plugin-clean-ordered  FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) clean
sub-tools-clean: tools/$(MAKEFILE) sub-src-clean FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) clean
sub-tools-distclean-ordered: tools/$(MAKEFILE) sub-plugin-distclean-ordered  FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) distclean
sub-tools-distclean: tools/$(MAKEFILE) sub-src-distclean FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) distclean
sub-tools-install_subtargets-ordered: tools/$(MAKEFILE) sub-plugin-install_subtargets-ordered  FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) install
sub-tools-install_subtargets: tools/$(MAKEFILE) sub-src-install_subtargets FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) install
sub-tools-uninstall_subtargets-ordered: tools/$(MAKEFILE) sub-plugin-uninstall_subtargets-ordered  FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) uninstall
sub-tools-uninstall_subtargets: tools/$(MAKEFILE) sub-src-uninstall_subtargets FORCE
	cd tools/ && $(MAKE) -f $(MAKEFILE) uninstall

Makefile: lipstick-socam.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_phonon.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf
	$(QMAKE) -o Makefile lipstick-socam.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_phonon.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
qmake: qmake_all FORCE
	@$(QMAKE) -o Makefile lipstick-socam.pro

qmake_all: sub-src-qmake_all sub-plugin-qmake_all sub-tools-qmake_all FORCE

make_default: sub-src-make_default-ordered sub-plugin-make_default-ordered sub-tools-make_default-ordered FORCE
make_first: sub-src-make_first-ordered sub-plugin-make_first-ordered sub-tools-make_first-ordered FORCE
all: sub-src-all-ordered sub-plugin-all-ordered sub-tools-all-ordered FORCE
clean: sub-src-clean-ordered sub-plugin-clean-ordered sub-tools-clean-ordered FORCE
	-$(DEL_FILE) build-stamp
	-$(DEL_FILE) configure-stamp
	-$(DEL_FILE) artifacts/*.deb
	-$(DEL_FILE) *.log.xml
	-$(DEL_FILE) *.log
distclean: sub-src-distclean-ordered sub-plugin-distclean-ordered sub-tools-distclean-ordered FORCE
	-$(DEL_FILE) Makefile
	-$(DEL_FILE) build-stamp configure-stamp *.log.xml *.log
install_subtargets: sub-src-install_subtargets-ordered sub-plugin-install_subtargets-ordered sub-tools-install_subtargets-ordered FORCE
uninstall_subtargets: sub-src-uninstall_subtargets-ordered sub-plugin-uninstall_subtargets-ordered sub-tools-uninstall_subtargets-ordered FORCE

sub-src-check_ordered: src/$(MAKEFILE)
	cd src/ && $(MAKE) -f $(MAKEFILE) check
sub-plugin-check_ordered: plugin/$(MAKEFILE) sub-src-check_ordered 
	cd plugin/ && $(MAKE) -f $(MAKEFILE) check
sub-tools-check_ordered: tools/$(MAKEFILE) sub-plugin-check_ordered 
	cd tools/ && $(MAKE) -f $(MAKEFILE) check
check: sub-src-check_ordered sub-plugin-check_ordered sub-tools-check_ordered

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all
install: install_subtargets  FORCE

uninstall:  uninstall_subtargets FORCE

FORCE:

