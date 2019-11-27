/**
 * DAL/DSAL classes implementation.
 */

#include "dal.h"

/**
 * ---------------------------------------------------------------------------
 * Common methods from DAL.
 * ---------------------------------------------------------------------------
 */

//----------------------------------------------------------------------------------------
/*! @brief Basic constructor
 *  Creates a dictionary of the requested capacity.
 *
 *  @param capacity_ The requested storage capacity of the dictionary.
 */

namespace ac{
//------------------------------- construtctors ----------------------------------------//
//< Default constructor.
template < typename KeyType, typename DataType, typename KeyTypeLess >
DAL<KeyType,DataType,KeyTypeLess>::DAL ( size_t capacity_ )
    : m_length{ 0 }
    , m_capacity{ capacity_ }
    , m_array{ new entry_type[ m_capacity ] }
{ /* empty */ }


/*
template < typename KeyType, typename DataType, typename KeyTypeLess >
DAL<KeyType,DataType,KeyTypeLess>::DAL ( const DAL & )
*/


//------------------------------- status members --------------------------------------//

template < typename KeyType, typename DataType, typename KeyTypeLess >
size_t const DAL<KeyType,DataType,KeyTypeLess>::capacity() const
{
    return m_capacity;
}

template < typename KeyType, typename DataType, typename KeyTypeLess >
size_t const DAL<KeyType,DataType,KeyTypeLess>::length() const
{
    return m_length;
}

template < typename KeyType, typename DataType, typename KeyTypeLess >
bool DAL<KeyType,DataType,KeyTypeLess>::empty() const
{
    return m_length == 0;
}

template <typename KeyType, typename DataType, class KeyTypeLess>
int DAL<KeyType, DataType, KeyTypeLess>::search( const KeyType & k_ ) const
{

    for( int i = 0; i < m_capacity; i++ )
    {
        if( k_ == m_array[i].first )
        {
            return i;
        }
    }

    return -1; //<-- Not found. ( cannot be a index!).
}

template <typename KeyType, typename DataType, typename KeyTypeLess>
bool DAL<KeyType, DataType, KeyTypeLess>::search( const KeyType & k_, DataType & d_ ) const
{
    for( int i{0}; i < m_capacity; i++ )
    {
        if( k_ == m_array[i].first )
        {
            d_ = m_array[i].second;

            return true;
        }
    }

    return false;
}
//------------------------------- Access members --------------------------------------//

template < typename KeyType, typename DataType, typename KeyTypeLess >
KeyType DAL< KeyType, DataType, KeyTypeLess>::max() const
{
    if( this->empty() )
    {
        throw std::out_of_range("The dictionary is empty!\n");
    }

    KeyType max = m_array[0].first;

    for( int i{1}; i < m_length; i++ )
    {
        if( m_array[i].first > max )
        {
            max = m_array[i].first;
        }
    }

    return max;

}

template < typename KeyType, typename DataType, typename KeyTypeLess >
KeyType DAL< KeyType, DataType, KeyTypeLess>::min() const
{
    if( this->empty() )
    {
        throw std::out_of_range("The dictionary is empty!\n");
    }

    KeyType min = m_array[0].first;

    for( int i{1}; i < m_length; i++ )
    {
        if( m_array[i].first < min )
        {
            min = m_array[i].first;
        }
    }

    return min;

}

template <typename KeyType, typename DataType, class KeyTypeLess>
bool DAL<KeyType, DataType, KeyTypeLess>::successor( const KeyType & key, KeyType & nextKey ) const
{
    // Verify key
    int checKey = search( key );


    if( checKey == -1  or key == max() or checKey + 1 == m_length )
    {
        return false;
    }

    if( checKey + 1 == m_length and m_array[m_length - 1].first > key )
    {
        nextKey = m_array[checKey + 1].first;
    }
    else if( checKey + 1 == m_length )
    {
        return false;
    }

    KeyType aux = m_array[checKey + 1].first;

    bool flagAux = true;

    for( long int i{0}; i < m_length; i++ )
    {
        bool aux_minmax = ( m_array[i].first < aux or flagAux);
        if( m_array[i].first > key and aux_minmax )
        {
            flagAux = false;
            aux = m_array[i].first;
        }
    }

    nextKey = aux;

    return true;
}

template <typename KeyType, typename DataType, typename KeyTypeLess>
bool DAL<KeyType, DataType, KeyTypeLess>::predecessor( const KeyType & key, KeyType & prevKey ) const
{
    int checKey = search( key );

    if( checKey == -1 or key == min() or checKey - 1 < 0  )
    {
       return false;
    }

    KeyType aux = m_array[checKey - 1].first;

    bool flagAux = true;

    for( long int i = m_length - 1 ; i >= 0; i-- )
    {
        bool aux_minmax = (m_array[i].first > aux or flagAux);
        if( m_array[i].first < key and aux_minmax )
        {
            flagAux = false;
            aux = m_array[i].first;
        }
    }

   prevKey  = aux;

   return true;
}

//------------------------------ Modifier members -------------------------------------//


//insert main method.
template < typename KeyType, typename DataType, typename KeyTypeLess >
bool DAL< KeyType, DataType, KeyTypeLess>::insert( const KeyType & newKey, const DataType & newInfo)
{
    //Verify key
    int checKey = search( newKey );

    if( checKey == -1 )
    {
        //verify space.
        if( m_capacity == m_length )
        {
            reserve( m_capacity + 1);
        }

        m_array[m_length].first = newKey;
        m_array[m_length].second = newInfo;

        m_length++;

        return true;
    }

    return false; // This key already exists!

}

//Allocator
template < typename KeyType, typename DataType, typename KeyTypeLess>
bool DAL<KeyType, DataType, KeyTypeLess>::reserve( size_t newSize )
{
    std::unique_ptr<entry_type []> temp{ new entry_type[newSize] };

    for( int i{0}; i < m_capacity; i++ )
    {
        temp[i].first = this->m_array[i].first;
        temp[i].second = this->m_array[i].second;
    }

    m_array.get_deleter();

    m_array.reset(temp.release());

    m_capacity = newSize;

    return true;
}

template < typename KeyType, typename DataType, typename KeyTypeLess >
bool DAL< KeyType, DataType, KeyTypeLess>::remove( const KeyType & oldKey, DataType & info)
{
    int checKey = search( oldKey );

    if( m_length == 0 or checKey == -1 )
    {
        return false; // dict is empty or key doesn't exist.
    }

    info = m_array[checKey].second;

    std::unique_ptr<entry_type []> temp{ new entry_type[m_capacity - 1]};

    int newIndex = 0;

    for( int i{0}; i < m_capacity; i++ )
    {
        if( m_array[i].first == oldKey )
        {
            continue;
        }
        else
        {
            temp[newIndex].first = this->m_array[i].first;
            temp[newIndex].second = this->m_array[i].second;
            newIndex++;
        }
    }

    m_capacity--;
    m_length--;

    m_array.get_deleter();

    m_array.reset(temp.release());

    return true;

}

} // END NAMESPACE.
// TODO: INCOMPLETE!!!
