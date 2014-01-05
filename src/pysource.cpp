#include "pysource.h"
#include "pycsft.h"

#define PYSOURCE_DEBUG 1
//#define PYSOURCE_DEBUG 0

#define LOC_ERROR2(_msg,_arg,_arg2)		{ sError.SetSprintf ( _msg, _arg, _arg2 ); return false; }

CSphSource_Python2::CSphSource_Python2 ( const char * sName, PyObject *obj)
            : CSphSource_Document ( sName )
            , m_iMultiAttr(0)
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

ISphHits * CSphSource_Python2::getHits ()
{
    if ( m_tState.m_bDocumentDone )
        return NULL;

    return &m_tHits;
}

bool CSphSource_Python2::Setup ( const CSphConfigSection & hSource){
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] Setup .\n");
#endif
    int nRet = py_source_setup(_obj, hSource);
    _bAttributeConfigured =  (nRet == 0);
    // TODO: check the error code.
    {
        /*
        0   OK;
        -1  python script error.
        -2  some required method missing.
        -100 unknown error
        */
    }
    return _bAttributeConfigured;
}

// DataSouce Interface

bool CSphSource_Python2::Connect ( CSphString & sError ) {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] Connect .\n");
#endif
    // update capablity of m_tHits
    // call pysource connect
    if(py_source_connected(_obj, m_tSchema) != 0)
        return false;

    // init schema storage.
    m_dStrAttrs.Resize ( m_tSchema.GetAttrsCount() );
    m_tHits.m_dData.Reserve ( m_iMaxHits ); // from sqlsource
    // update plain (not join) field count.
    m_iPlainFieldsLength = 0;
    ARRAY_FOREACH ( i, m_tSchema.m_dFields ) {
        if(m_tSchema.m_dFields[i].m_iIndex!=-1)
            m_iPlainFieldsLength ++;
    }
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
    m_tSchema.Reset ();  //reset m_tSchema is unecessary, and will cause bugs when deal join fields.
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
    // call before index
    if(py_source_before_index(_obj) != 0)
        return false;

    // check has join field  fields -> m_baseFields , m_joinFields ?

    // check fieldMVA , embed multi-value attribute. m_dAttrToFieldMVA ?

    return true;
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
    // printf("begin mva %d , %d.\n", iAttr, m_tSchema.GetAttrsCount() );
    if ( iAttr<0 || iAttr>=m_tSchema.GetAttrsCount() )
        return false;

    const CSphColumnInfo & tAttr = m_tSchema.GetAttr(iAttr);
    // printf("%s ---> %d, %d\n", tAttr.m_sName.cstr(), 1, tAttr.m_eSrc == SPH_ATTRSRC_QUERY);
    if ( !(tAttr.m_eAttrType==SPH_ATTR_UINT32SET || tAttr.m_eAttrType==SPH_ATTR_INT64SET ) )
        return false;
    switch ( tAttr.m_eSrc )
    {
    case SPH_ATTRSRC_FIELD:
        return false;
    case SPH_ATTRSRC_QUERY:
        {
            //FIXME: should check feedMultiValueAttribute existance.
            //printf("---------------");
            m_iMultiAttr = iAttr;
            return true;
        }
    default:
        sError.SetSprintf ( "INTERNAL ERROR: unknown multi-valued attr source type %d", tAttr.m_eSrc );
        return false;
    } // end of switch
}

/// get next multi-valued (id,attr-value) or (id, offset) for mva64 tuple to m_tDocInfo
bool CSphSource_Python2::IterateMultivaluedNext () {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] IterateMultivaluedNext .\n");
#endif
    uint64_t docid = 0;
    int64_t v = 0;
    const CSphColumnInfo & tAttr = m_tSchema.GetAttr(m_iMultiAttr);

    if( py_source_get_join_mva(_obj, tAttr.m_sName.cstr(), &docid, &v) == 0){
        if(!docid)
            return false;

        m_tDocInfo.m_iDocID = (SphDocID_t)docid;
        if ( tAttr.m_eAttrType==SPH_ATTR_UINT32SET )
            m_dMva.Add ( (DWORD)v );
        else
            sphAddMva64 ( m_dMva, v );
        return true;
    }else
        return false;
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
void CSphSource_Python2::PostIndex () {
    CSphSource_Document::PostIndex();
    if(py_source_index_finished(_obj) != 0)
        return ; //what can I do...
}

BYTE ** CSphSource_Python2::NextDocument ( CSphString & sError ) {
#if PYSOURCE_DEBUG
    fprintf(stderr, "[DEBUG][PYSOURCE] NextDocument .\n");
#endif

    unsigned int iPrevHitPos = 0;

    memset(m_dFields,0,sizeof(m_dFields));

    // printf("row size %d\n", m_tSchema.GetRowSize());
    m_tDocInfo.Reset ( m_tSchema.GetRowSize() );

    // save prev hit position.
    m_tHits.m_dData.Resize( 0 );
    iPrevHitPos = m_tHits.m_dData.GetLength();

    // call nextDocument -> feed
    int nRet = py_source_next(_obj);
    bool bHasMoreDoc = (nRet == 0 ); //-1 have exception; 1 normal exit
    // reset docid for newly append hits (which by python hitcollector)

    // check is index finished  -> call afterIndex
    if(!bHasMoreDoc) {
        // might be has exception... if nRet != -1
        if(py_source_after_index(_obj, nRet == 1) != 0) {
            // if u wanna exit, set docid not 0 & return NULL.
            m_tDocInfo.m_iDocID = -1;
            return NULL; // this will cause IterateDocument return false.
        }
    } // end moreDoc check.

    // process docInfo
    {
        // check attribute
        // check MVA field (embed listed mva)
        // check fields. -> in pysource v1 , this job has taken in CSphSource_Python::SetAttr( int iIndex, PyObject* v)
    }
    return m_dFields;
}

// end of file
