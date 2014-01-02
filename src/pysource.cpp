#include "pysource.h"
#include "pycsft.h"

CSphSource_Python2::CSphSource_Python2 ( const char * sName )
            : CSphSource_Document ( sName )
{
    // ...
}

CSphSource_Python2::~CSphSource_Python2 ()
{
    // ...
}

bool CSphSource_Python2::Setup ( const CSphConfigSection & hSource){
    py_source_setup(NULL, hSource);
    return false;
}

// DataSouce Interface

bool CSphSource_Python2::Connect ( CSphString & sError ) {
    return false;
}

/// disconnect from the source
void CSphSource_Python2::Disconnect () {

}

/// check if there are any attributes configured
/// note that there might be NO actual attributes in the case if configured
/// ones do not match those actually returned by the source
bool CSphSource_Python2::HasAttrsConfigured () {
    return false;
}

/// begin indexing this source
/// to be implemented by descendants
bool CSphSource_Python2::IterateStart ( CSphString & sError ) {
    return false;
}

/// get next document
/// to be implemented by descendants
/// returns false on error
/// returns true and fills m_tDocInfo on success
/// returns true and sets m_tDocInfo.m_iDocID to 0 on eof -> by CSphSource_Document
// virtual bool						IterateDocument ( CSphString & sError ) = 0;

/// get next hits chunk for current document
/// to be implemented by descendants
/// returns NULL when there are no more hits
/// returns pointer to hit vector (with at most MAX_SOURCE_HITS) on success
/// fills out-string with error message on failure ->CSphSource_Document
// virtual ISphHits *					IterateHits ( CSphString & sError ) = 0;

/// get joined hits from joined fields (w/o attached docinfos)
/// returns false and fills out-string with error message on failure
/// returns true and sets m_tDocInfo.m_uDocID to 0 on eof
/// returns true and sets m_tDocInfo.m_uDocID to non-0 on success ->...?
// virtual ISphHits *					IterateJoinedHits ( CSphString & sError );

/// begin iterating values of out-of-document multi-valued attribute iAttr
/// will fail if iAttr is out of range, or is not multi-valued
/// can also fail if configured settings are invalid (eg. SQL query can not be executed)
bool CSphSource_Python2::IterateMultivaluedStart ( int iAttr, CSphString & sError )
{
    return false;
}

/// get next multi-valued (id,attr-value) or (id, offset) for mva64 tuple to m_tDocInfo
bool CSphSource_Python2::IterateMultivaluedNext () {
    return false;
}

/// begin iterating values of multi-valued attribute iAttr stored in a field
/// will fail if iAttr is out of range, or is not multi-valued
SphRange_t CSphSource_Python2::IterateFieldMVAStart ( int iAttr ) {
    SphRange_t tRange;
    tRange.m_iStart = tRange.m_iLength = 0;
    return tRange;
}

/// begin iterating kill list
bool CSphSource_Python2::IterateKillListStart ( CSphString & sError ) {
    return false;
}

/// get next kill list doc id
bool CSphSource_Python2::IterateKillListNext ( SphDocID_t & tDocId ) {
    return false;
}

/// post-index callback
/// gets called when the indexing is succesfully (!) over
// virtual void						PostIndex () {}

BYTE ** CSphSource_Python2::NextDocument ( CSphString & sError ) {
    return NULL;
}

// end of file
