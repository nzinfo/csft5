#include "pysource.h"
#include "pycsft.h"

#define PYSOURCE_DEBUG 1
//#define PYSOURCE_DEBUG 0

#define LOC_ERROR2(_msg,_arg,_arg2)		{ sError.SetSprintf ( _msg, _arg, _arg2 ); return false; }

CSphSource_Python2::CSphSource_Python2 ( const char * sName, PyObject *obj)
            : CSphSource_Document ( sName )
            , _bAttributeConfigured(false)
            , _obj(obj)
{
    // 检测 DocID 是否为 64bit 的
    assert(sizeof(uint64_t) >= sizeof(SphDocID_t));

    Py_INCREF(_obj); // hold the referenct, it doesn't hurt.
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] Init source '%s'.\n", sName);
#endif
}

CSphSource_Python2::~CSphSource_Python2 ()
{
    // release the obj.
    Py_XDECREF(_obj);
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] Deinit source .\n");
#endif
}

bool CSphSource_Python2::Setup ( const CSphConfigSection & hSource){
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] Setup .\n");
#endif
    int nRet = py_source_setup(_obj, m_tSchema, hSource);
    _bAttributeConfigured =  (nRet == 0);
    return _bAttributeConfigured;
}

// DataSouce Interface

bool CSphSource_Python2::Connect ( CSphString & sError ) {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] Connect .\n");
#endif
    // update capablity of m_tHits
    // call pysource connect
    if(py_source_connected(_obj) != 0)
        return false;

    // init schema storage.
    m_tDocInfo.Reset ( m_tSchema.GetRowSize() );
    m_dStrAttrs.Resize ( m_tSchema.GetAttrsCount() );

    // check it
    if ( m_tSchema.m_dFields.GetLength()>SPH_MAX_FIELDS )
        LOC_ERROR2 ( "too many fields (fields=%d, max=%d); raise SPH_MAX_FIELDS in sphinx.h and rebuild",
            m_tSchema.m_dFields.GetLength(), SPH_MAX_FIELDS );

    return true;
}

/// disconnect from the source
void CSphSource_Python2::Disconnect () {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] Disconnect .\n");
#endif
    m_tSchema.Reset ();
    m_tHits.m_dData.Reset();
    // notify python ?
}

/// check if there are any attributes configured
/// note that there might be NO actual attributes in the case if configured
/// ones do not match those actually returned by the source
bool CSphSource_Python2::HasAttrsConfigured () {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] HasAttrsConfigured .\n");
#endif
    return _bAttributeConfigured;
}

/// begin indexing this source
/// to be implemented by descendants
bool CSphSource_Python2::IterateStart ( CSphString & sError ) {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] IterateStart .\n");
#endif
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
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] IterateMultivaluedStart .\n");
#endif
    return false;
}

/// get next multi-valued (id,attr-value) or (id, offset) for mva64 tuple to m_tDocInfo
bool CSphSource_Python2::IterateMultivaluedNext () {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] IterateMultivaluedNext .\n");
#endif
    return false;
}

/// begin iterating values of multi-valued attribute iAttr stored in a field
/// will fail if iAttr is out of range, or is not multi-valued
SphRange_t CSphSource_Python2::IterateFieldMVAStart ( int iAttr ) {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] IterateFieldMVAStart .\n");
#endif
    SphRange_t tRange;
    tRange.m_iStart = tRange.m_iLength = 0;
    return tRange;
}

/// begin iterating kill list
bool CSphSource_Python2::IterateKillListStart ( CSphString & sError ) {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] IterateKillListStart .\n");
#endif
    return false;
}

/// get next kill list doc id
bool CSphSource_Python2::IterateKillListNext ( SphDocID_t & tDocId ) {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] IterateKillListNext .\n");
#endif
    return false;
}

/// post-index callback
/// gets called when the indexing is succesfully (!) over
// virtual void						PostIndex () {}

BYTE ** CSphSource_Python2::NextDocument ( CSphString & sError ) {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] NextDocument .\n");
#endif
    return NULL;
}

// end of file