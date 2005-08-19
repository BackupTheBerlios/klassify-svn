bin_PROGRAMS = klassify
INCLUDES = $(all_includes)
klassify_LDFLAGS = $(all_libraries) $(KDE_RPATH)
klassify_LDADD = -lmk4 $(LIB_KIO)
klassify_SOURCES = main.cpp klassify.cpp klassify.skel classifier.cpp \
		dcop_classifier.cpp naive_bayesian_classifier.cpp filemanager.cpp classification_task.cpp \
		classifier_factory.cpp dbacl_classifier.cpp


noinst_HEADERS = klassify.h classifier.h dcop_classifier.h \
			naive_bayesian_classifier.h filemanager.h classification_task.h classifier_factory.h \
	dbacl_classifier.h

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/klassify.pot


service_DATA = klassify.desktop
servicedir = $(kde_servicesdir)


