SRC_SPHINX = sphinx.cpp sphinxexcerpt.cpp sphinxquery.cpp \
	sphinxsoundex.cpp sphinxmetaphone.cpp sphinxstemen.cpp sphinxstemru.cpp sphinxstemcz.cpp sphinxstemar.cpp \
	sphinxutils.cpp sphinxstd.cpp sphinxsort.cpp sphinxexpr.cpp sphinxfilter.cpp \
	sphinxsearch.cpp sphinxrt.cpp sphinxjson.cpp sphinxudf.c sphinxaot.cpp

if USE_PYTHON
SRC_PYTHONLAYER = py_layer.cpp pysource.cpp pyiface.cpp pycsft.cxx
endif

noinst_LIBRARIES = libsphinx.a
libsphinx_a_SOURCES = $(SRC_SPHINX) $(SRC_PYTHONLAYER) 

bin_PROGRAMS = indexer searchd search spelldump indextool wordbreaker
noinst_PROGRAMS = tests

indexer_SOURCES = indexer.cpp
searchd_SOURCES = searchd.cpp
search_SOURCES = search.cpp
spelldump_SOURCES = spelldump.cpp
indextool_SOURCES = indextool.cpp
tests_SOURCES = tests.cpp
wordbreaker_SOURCES = wordbreaker.cpp

BUILT_SOURCES = extract-version pycsft.cxx
.PHONY: extract-version
extract-version:
	/bin/sh svnxrev.sh ..

pycsft.cxx: pycsft.pyx
	@cython pycsft.pyx --cplus -o pycsft.cxx


AM_CPPFLAGS = $(LIBRE2_CFLAGS) -DSYSCONFDIR="\"$(sysconfdir)\"" -DDATADIR="\"$(localstatedir)/data\""
COMMON_LIBS = libsphinx.a $(LIBSTEMMER_LIBS) $(MYSQL_LIBS) $(PGSQL_LIBS) $(LIBRE2_LIBS)
LDADD = $(COMMON_LIBS)
