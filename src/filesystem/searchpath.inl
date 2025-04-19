#ifndef SEARCHPATH_INL
#define SEARCHPATH_INL

/*
==================
CSearchPath::SetPathID
==================
*/
FORCEINLINE void CSearchPath::SetPathID( const achar* pNewPathID )
{
    pathID = pNewPathID;
}

/*
==================
CSearchPath::SetPath
==================
*/
FORCEINLINE void CSearchPath::SetPath( const achar* pNewPath )
{
    path = pNewPath;
}

/*
==================
CSearchPath::GetPathID
==================
*/
FORCEINLINE const std::string& CSearchPath::GetPathID() const
{
    return pathID;
}

/*
==================
CSearchPath::GetPath
==================
*/
FORCEINLINE const std::string& CSearchPath::GetPath() const
{
    return path;
}


/*
==================
CSearchPathIterator::operator++
==================
*/
FORCEINLINE void CSearchPathIterator::operator++()
{
    if ( currentIndex + 1 <= searchPaths.size() )
    {
        ++currentIndex;
    }
}

/*
==================
CSearchPathIterator::operator+=
==================
*/
FORCEINLINE void CSearchPathIterator::operator+=( uint32 offset )
{
    while ( offset-- )
    {
        operator++();
    }
}

/*
==================
CSearchPathIterator::operator*
==================
*/
FORCEINLINE CSearchPath* CSearchPathIterator::operator*() const
{
    return GetSearchPath();
}

/*
==================
CSearchPathIterator::operator->
==================
*/
FORCEINLINE CSearchPath* CSearchPathIterator::operator->() const
{
    return GetSearchPath();
}

/*
==================
CSearchPathIterator::operator bool
==================
*/
FORCEINLINE CSearchPathIterator::operator bool() const
{
    return currentIndex < searchPaths.size();
}

/*
==================
CSearchPathIterator::operator !
==================
*/
FORCEINLINE bool CSearchPathIterator::operator !() const
{
    return !operator bool();
}

/*
==================
CSearchPathIterator::GetSearchPath
==================
*/
FORCEINLINE CSearchPath* CSearchPathIterator::GetSearchPath() const
{
    return !searchPaths.empty() ? searchPaths[currentIndex] : NULL;
}
#endif // !SEARCHPATH_INL
