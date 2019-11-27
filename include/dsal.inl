/**
 * DAL/DSAL typenamees implementation.
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

//------------------------------- Overwritten methods  ----------------------------------------//
template <typename KeyType, typename DataType, typename KeyTypeLess>
long int DSAL<KeyType, DataType, KeyTypeLess>::search( const KeyType & key ) const
{
/*------------------------------- Binary Search ------------------------------------*/
    long int first = 0;
    long int last = this->m_length;

    while(first != last)
    {
        long int middle = first + (last - first)/2;

        if( this->m_array[middle].first == key )
        {
            return middle;
        }
        else
        {
            if( this->m_array[middle].first > key )
            {
                last = middle;
            }
             else
            {
                first = middle + 1;
            }
        }

    }

    return -1; //<-- key not found!
}

template <typename KeyType, typename DataType, typename KeyTypeLess>
long int DSAL<KeyType, DataType, KeyTypeLess>::search( const KeyType & key, DataType & info ) const
{
/*------------------------------- Binary Search ------------------------------------*/
    long int first = 0;
    long int last = this->m_length;

    while(first != last)
    {
        long int middle = first + (last - first)/2;

        if( this->m_array[middle].first == key )
        {
            return middle;
        }
        else
        {
            if( this->m_array[middle].first > key )
            {
                last = middle;
            }
             else
            {
                first = middle + 1;
            }
        }

    }

    return -1; //<-- key not found!
}


/*----------------------------------------------------------------------------------------------*/

template <typename KeyType, typename DataType, typename KeyTypeLess>
size_t DSAL<KeyType, DataType, KeyTypeLess>::where( const KeyType & key ) const
{
    if( this->m_length == 0 or this->m_capacity == 0 )
    {
        return 0;
    }

    // insert position.
    long int position = 0;

    for( long int i = 0; i < this->m_length + 1; i++ )
    {

        if( this->m_array[i].first > key )
        {
            position = i;
            break;
        }
        else
        {
            position = i;
        }
    }


    // return the bigger smallest position after the key.
    // the insertion will be in the previous position.
    return position;
}

/*----------------------------------------------------------------------------------------------*/

template <typename KeyType, typename DataType, typename KeyTypeLess>
size_t DSAL<KeyType, DataType, KeyTypeLess>::reserve( const size_t SIZE, const KeyType & key )
{
    std::unique_ptr<entry_type []> temp{ new entry_type[SIZE]};


    size_t insertIndex = where( key );

    long int tempIndex = 0;

    for( long int i{0}; i < this->m_length; i++ )
    {
        if( i != insertIndex )
        {
            temp[tempIndex].first = this->m_array[i].first;
            temp[tempIndex].second = this->m_array[i].second;
            tempIndex++;
        }
        else
        {
            tempIndex++; //<-- leaving free the insert space.
            // i on the next space.
            temp[tempIndex].first = this->m_array[i].first;
            temp[tempIndex].second = this->m_array[i].second;
            tempIndex++; //<-- next iteration.
        }
    }

    this->m_array.get_deleter();

    this->m_array.reset(temp.release());
    this->m_capacity = SIZE;

    return insertIndex;
}

/*----------------------------------------------------------------------------------------------*/

template <typename KeyType, typename DataType, typename KeyTypeLess>
bool DSAL<KeyType, DataType, KeyTypeLess>::insert( const KeyType & newKey, const DataType & newInfo )
{
    long int checKey = search( newKey );

    if( checKey != -1 )
    {
        return false;
    }

    if( checKey == -1 )
    {
        if( not (this->m_length < this->m_capacity) )
        {
            // needs more space 
            size_t insert_checKey = reserve( this->m_length + 1, newKey );

            // Inserts the new element on the correct position 
            this->m_array[insert_checKey].first = newKey;
            this->m_array[insert_checKey].second = newInfo;

        }
        else
        {
            // doesnt need more space, we just need to find where to put
            size_t insert_checKey = where( newKey );

            std::unique_ptr< entry_type []> temp{ new entry_type[this->m_capacity]};

            bool IN_FLAG = false; //boolean for insertion

            for( long int i{0}, tempIndex{0}; i < this->m_length + 1 and tempIndex < this->m_capacity;)
            {
                if( i == insert_checKey and !IN_FLAG )
                {
                    temp[tempIndex].first = newKey;
                    temp[tempIndex].second = newInfo;
                    // after insertion the flag will down.
                    IN_FLAG = true;
                    tempIndex++;
                }
                else
                {
                    temp[tempIndex].first = this->m_array[i].first;
                    temp[tempIndex].second = this->m_array[i].second;
                    tempIndex++;
                    i++;
                }
            }


           this->m_array.get_deleter();
           this->m_array.reset(temp.release());

        }

        this->m_length++;
  }
  else
  {
      //the key already exists, so we can overwrite 
      return false;
  }


  return true;
}

/*----------------------------------------------------------------------------------------------*/

template <typename KeyType, typename DataType, typename KeyTypeLess>
bool DSAL<KeyType, DataType, KeyTypeLess>::remove( const KeyType & key, DataType & info )
{
    long int checKey = search( key );

    if( checKey == -1 )
    {
        return false; // The key doesn't exist
    }

    info = this->m_array[checKey].second;

    std::unique_ptr< entry_type [] > temp{ new entry_type [this->m_capacity] };


    for( long int i{0}; i <= this->m_length; i++ )
    {
        if( i >= checKey and i < this->m_capacity - 1 )
        {
            temp[i].first = this->m_array[i+1].first;
            temp[i].second = this->m_array[i+1].second;
        }
        else
        {
            if( i < this->m_capacity )
            {
                temp[i].first = this->m_array[i].first;
                temp[i].second = this->m_array[i].second;
            }
        }

        if( this->m_length > 0 )
        {
            this->m_length--;
        }

      }

    this->m_array.get_deleter();

    this->m_array.reset(temp.release());

    return true;

}


/*----------------------------------------------------------------------------------------------*/

template <typename KeyType, typename DataType, class KeyTypeLess>
KeyType DSAL<KeyType, DataType, KeyTypeLess>::max() const
{
    if( this->m_length == 0 )
    {
        throw std::out_of_range("\x1b[31mERROR\x1b[0m: The dictionary is empty!\n");
    }

    return this->m_array[this->m_length - 1].first;
}



template <typename KeyType, typename DataType, class KeyTypeLess>
KeyType DSAL<KeyType, DataType, KeyTypeLess>::min() const
{
    if( this->m_length == 0 )
    {
        throw std::out_of_range("\x1b[31mERROR\x1b[0m: The dictionary is empty!\n");
    }

    return this->m_array[0].first;
}



template <typename KeyType, typename DataType, class KeyTypeLess>
bool DSAL<KeyType, DataType, KeyTypeLess>::successor( const KeyType & key, KeyType & nextKey ) const
{
    long int checKey = search( key );

    if( checKey + 1 < this->m_length and checKey != -1 )
    {
        nextKey = this->m_array[checKey + 1].first;
        return true;
    }

    return false;
}

template <typename KeyType, typename DataType, typename KeyTypeLess>
bool DSAL<KeyType, DataType, KeyTypeLess>::predecessor( const KeyType & key, KeyType & prevKey ) const
{
    long int checKey = search( key );

    if( checKey - 1 >= 0 and checKey != -1 )
    {
        prevKey = this->m_array[checKey - 1].first;
        return true;
    }

    return false;
}

} //END NAMESPACE
