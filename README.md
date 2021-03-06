# Perl: The Complete (Crap) Reference

A.K.A _Binary Search Revisited_

# Overview

Why would someone take the time to write binary search in this day and age?

Because:

1. The only way to remain a good programmer is to constantly practice the basics, and
2. It is trivial to write. See [Typical crappy Binary Search vs a Clean one
](#typical-crappy-binary-search-vs-a-clean-one) but we're getting ahead of ourselves.

# Backstory

I was cleaning out my old computer books and came across a Perl book.
In [Perl: The Complete Reference](https://www.amazon.com/Perl-Complete-Reference-Martin-Brown/dp/0072120002/),
Chapter 19, _Perl Compiler_, Page 638, the author compares the speed of Perl with C.
He gives various small examples:

* count the number of lines (test2)
* count the number of unique lines (test3)

both in C and Perl.

Here is the `test3` Perl program:

```Perl
    open(DATA,"<words.txt") || die "Can't open file";

    my %words;

    while(<DATA>)
    {
      $words{$_} = 1;
    }
    close(DATA);

    print scalar keys %words," unique lines\n";
```

Here is the author's "equivalent" C version:

```c
    // Perl: The Complete Reference (1st Ed.), Chapter 19, Perl Compiler, Page 638, 1999, ISBN: 0-07-212000-2
    #include <stdio.h>

    char words[32000][260];
    long wordidx = 0;

    void addtolist(char word[255])
    {
      int i=0;
      for(i=0;i<wordidx;i++)
        {
          if (strcmp(word,words[i]) == 0)
              return;
        }

      strcpy(words[++wordidx],word);
    }

    int main()
    {
      char line[255];
      FILE *data;
      long count = 0;

      data = fopen("words.txt","rw");

      while(!feof(data))
        {
          fgets(line,255,data);
          addtolist(line);
        }

      printf( "%ld unique lines\n",wordidx);
    }
```

*facepalm*

**How many _noob mistakes_ can you spot?**

Take a minute.

# Bugs

That _wasn't_ just a rhetorical question by the way.

There are _numerous_ things wrong with the C version.
One of the _best_ ways to become a better programmer is to
**read code** by _finding_ bugs in existing code that already has them.

Once you're ready for all the answers here is the full list of everything wrong
in [src/test3_annotated.c](src/test3_annotated.c)

* https://github.com/Michaelangel007/perl_crap_reference/blob/master/src/test3_annotated.c

And the cleaned up version in [src/test3_opt1.c](src/test3_opt1.c)

*  https://github.com/Michaelangel007/perl_crap_reference/blob/master/src/test3_opt1.c

The 3 biggest problems are:

* The bloody thing doens't even compile !
* Has an off-by-one bug! Did the author _even test it??_
* Not scalable

# Apples to Oranges

Table 19-8 has benchmark times showing that the Perl version is faster then the C version.

|Operation                                 | Test 3|
|:-----------------------------------------|--------:|
|Compile C program once, execute 100 times | 1103.75 |
|Execute interpreted Perl script 100 times |  197.00 |

First, where is the _data file_ to independently verify the timing???

Second, except this an _apples-to-oranges-comparison_ -- **different algorithms** are used!
Obviously the _better algorithm_ in a slower language
is going to "win" against the _worse algorithm_ in a faster language!
Of course the Perl version is faster.
**[No shit, Sherlock!](http://knowyourmeme.com/memes/no-shit-sherlock--2)**

The author gives the _excuse:_

> We could have used a binary tree or even a hashing system,
> but that would have added significantly more lines to an already long C program.

1. _No one give a shit how many lines of code C is_ -- the **entire** point of using
  **C is for SPEED;** not some _bullshit metric!_

 Does that make _-2,000 Lines of Code_ even better?!

 * http://www.folklore.org/StoryView.py?project=Macintosh&story=Negative_2000_Lines_Of_Code.txt&sortOrder=Sort+by+Date&topic=Management

2. Apparently 34 lines of code is long?!?!

  a) Wait till the author finds out how this _toy example_ compares in the _Real World (TM)_
  where there are _hundreds of thousands of lines of code in a project!_

  b) Really?! The book is 1179 pages; apparently including an additional 38 lines for a total of 72 lines was too much work !

/sarcasm _"Hey, that would have been twice the work, right?"_


# Optimizations

There are numerous optimizations that can be done:

1. Read the entire file into memory at once bypassing the [C Run-Time library](http://stackoverflow.com/questions/2766233/what-is-the-c-runtime-library) (CRT, aka `libc`) buffering.
2. Replace the dog slow string comparision with a hash compare such as [FNV1a](https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function)

 ```cpp
    const uint32_t FNV1A_PRIME = 0x01000193; //   16777619
    const uint32_t FNV1A_SEED  = 0x811C9DC5; // 2166136261

    inline uint32_t fnv1a_byte( unsigned char byte, uint32_t hash = FNV1A_SEED )
    {
          return (byte ^ hash) * FNV1A_PRIME;
    }

    inline uint32_t fnv1a_string( char *text )
    {
        uint32_t       hash = FNV1A_SEED;
        unsigned char *p    = (unsigned char *)text;

        while( *p )
            hash = fnv1a_byte( *p++, hash );

        return hash;
    }
 ```

3. Inline the hash generation

 Since FNV1a is so tiny, we can manually inline it:

 ```c
            hash = FNV1A_SEED;
            while( *p != LF )
                hash = (*p++ ^ hash) * FNV1A_PRIME;
 ```

4. Replace the slow _Linear_ string search with a fast _Binary Seach_


 # Typical crappy Binary Search vs a Clean one

 The typical Binary Search is given with this algorithm:

 ```c
    int binarysearch(datatype t, datatype *x, size_t n)
    {
        /* Lower and upper limits and middle test value */
        int l, u, m;

        /* Initialize bounds */
        l = 0;
        u = n - 1;

        /* Are we done yet? */
        while (l <= u)
        {
            /* Halve the range */
            m = (l + u) / 2;
            if (x[m] < t)
            {
                /* Move lower limit */
                l = m + 1;
            }
            else if (x[m] == t)
            {
                /* A match - return its location */
                return m;
            }
            else /* x[m] > t */
            {
                /* Move upper limit */
                u = m - 1;
            }
        }

        /* Failure */
        return -1
    }
 ```

 UGH.

 Notice the crappy code style and variable names:

  * `t` badly named and non-descript of what we are searching for; in contradisction to `key`
  * `x` badly named `data`
  * `l` lowercase `L` is _horrible to read_ in some fonts due to looking one `1` looking almost identical; better would be `lo`
  * `m` is ambigious - does it mean minimum? median? maximum?
  * `u` is OK; better would be `hi`
  * Useless, redundant comments due to bad variable names

 Also the argument order should always be in a consistent fashion:

  * Array Size
  * Array Pointer

 # Code Poet: Binary Search

 Here is a cleaned up version with _good_ variable names:

 ```c
    int BinarySearch( uint32_t needle, int size, uint32_t *haystack )
    {
        int min = 0;
        int mid = 0;
        int max = size-1;

        while( min <= max )
        {
            mid = (min + max) >> 1;

            /**/ if( haystack[ mid ] == needle )   return mid  ;
            else if( haystack[ mid ] >  needle )   max  = mid-1;
            else /*( haystack[ mid ] <  needle )*/ min  = mid+1;
        }
    }
 ```

 We are finding a `needle` in a `haystack`. :-)

 This also explains why _some_ coders like to put the array size _after_ the
 array name, but I digress.

 For performance reasons we need to consider the _common case_.
 The majority of the time we _won't_ be on the key we are looking at.
 Modern CPU's have _branch prediction_ -- by placing the common cases first
 we can leverage this.

 Ergo,

 ```c
    int BinarySearch( uint32_t needle, int size, uint32_t *haystack )
    {
        int min = 0;
        int mid = 0;
        int max = size-1;

        while( min <= max )
        {
            mid = (min + max) >> 1;

            /**/ if( haystack[ mid ] <  needle )   min  = mid+1;
            else if( haystack[ mid ] >  needle )   max  = mid-1;
            else /*( haystack[ mid ] == needle )*/ return mid  ;
        }
    }
 ```

 Looking at the what the code is doing from a higher perspective
 if we are doing a _binary search followed by an insert if not found_
 then we can tweak the binary search to return the _negative position of middle._
 This will allow us to use that as the _starting_  position
 of where the key should be inserted into the array.

 ```c
    // If the key is found , returns position > 0 where key was found
    // If the key not found, returns position < 0 of last best location for insert
    // ========================================================================
    int find_key( uint32_t key )
    {
        int min = 0;
        int mid = 0;
        int max = gnWords - 1;

        while( min <= max )
        {
            mid = (min + max) >> 1;

            /**/ if( gaWords[ mid ] == key )   return mid+1; // normally return mid
            else if( gaWords[ mid ] >  key )   max =  mid-1;
            else /*                 <  key )*/ min =  mid+1;
        }

        return -(mid+1); // normally return false or -1
    }

    // ========================================================================
    void insert_key( uint32_t key, int pos )
    {
        int mid = pos - 1;

        while( (mid < gnWords) && (key > gaWords[ mid ]) )
            mid++;

        /* */ uint32_t *src = &gaWords[ mid ];
        /* */ uint32_t *dst = src + 1;
        const size_t    len = gnWords - mid;

        memmove( dst, src, sizeof( uint32_t ) * len ); // memcpy() can't alias (overlap)

        gaWords[ mid ] = key;
        gnWords++;
}
 ```

 And it is used like this:

 ```c
    void demo()
    {
        uint32_t hash;

        hash = fnva1a_string( ... );

        found = find_key( hash );
        if( found < 0 )
            insert_key( hash, -found );
    }
 ```

 Even better we can _merge_ the two algorithms ...

  * `FindKey()`
  * `InsertKey()`

 ... into a combined version since there is common data.

 ```c
    // If the key is  found, returns position >= 0 where key was found
    // If the key not found, returns -1 but with key inserted
    // ========================================================================
    int find_key_insert( uint32_t key )
    {
        int min = 0;
        int mid = 0;
        int max = gnWords - 1;

        while( min <= max )
        {
            mid = (min + max) >> 1;

            /**/ if( gaWords[ mid ] == key )   return mid  ;
            else if( gaWords[ mid ] >  key )   max  = mid-1;
            else /*                 <  key )*/ min  = mid+1;
        }

        while( (mid < gnWords) && (key > gaWords[ mid ]) )
            mid++;

        /* */ uint32_t *src = &gaWords[ mid ];
        /* */ uint32_t *dst = src + 1;
        const size_t    len = gnWords - mid;

        memmove( dst, src, sizeof( uint32_t ) * len ); // memcpy() can't alias (overlap)
        gaWords[ mid ] = key;
        gnWords++;

        return -1;
    }
 ```

 The full optimized single-threaded version is:

 * [test3 opt5.c](src/test3_opt5.c):


5\. Use multithreading

 Using OpenMP this is pretty trivial using

 * Scatter
  * Divide-and-Conqueor
 * Gather


# Scatter

We don't need to use any mutexes, semaphores, or atomics or worry about deadlocks as
this is a _lock-free_ algorithm. It is _trivial to parallize_ or what is known as
an [Embarrassingly Parallel](https://en.wikipedia.org/wiki/Embarrassingly_parallel) problem.

Why?

If we have a file of N size and T threads (or cores) each thread
gets _part_ of the file -- size N/T -- which is _independent_ of the other threads!

Each thread does this:

* Generate a hash for each line
* Use binary search to see if the hash is in the dictionary
* If not, insert it in sorted order

If we were simply counting lines we would be Good-To-Go (TM). But life is
(almost usually) never that simple.

The _snafu_ is that we are counting _unique_ lines.  That means we need
to generate a hash for the _entire_ line. If a line straddles the boundary
between two threads we need to adjust the _end of the previous_ thread,
and adjust the _start of the next_ thread.

Example:

We have this file with 3 lines of text (12 bytes)

```
    abcdef
    gh
    i
```

It can be generated like this:

```bash
    echo -e -n "abcdef\ngh\ni\n" > 12.txt
```

The file is just a linear stream of bytes:

```bash
    hexdump -C 12.txt

    00000000  61 62 63 64 65 66 0a 67  68 0a 69 0a              |abcdef.gh.i.|
    0000000c
```

Splitting the work up amongst 3 threads means each thread gets 12/3 = 4 bytes of data
allocated to it:


```
    abcdef$gh$i$
    \__/\__/\--/
     0   1   2
```

We need to move the "end" of thread 0 and the start of thread 1:

```
    abcdef$gh$i$
    \____/\/\--/
     0     1 2
```

Similarly, we need to adjust end of 1 and start of 2:

```
    abcdef$gh$i$
    \____/\__/\/
     0     1   2
```

Each thread just does a simple loop:

```c
    while( beg < end )
    {
        hash = fnv1a_string( line )
        find_insert( line )
    }
```

# Gather

In the _scatter_ phase each thread handles **local duplicates**.
In the _gather_ phase we need to handle **global duplicates** across all threads.

Once all threads are done we need to count non-duplicates. Let's pretend we have 4 cores (or threads) and these are the hashes:

|Core 0|Core 1|Core 2|Core 3|
|:-----|:-----|:-----|:-----|
|a     |b     |b     |a     |
|b     |c     |d     |e     |

The sorted order should be:

```
a
b
c
d
e
```

One of the simpliest ways to remove duplicates is to perform an N-way [merge sort](https://en.wikipedia.org/wiki/Merge_sort)

We need to keep track of:

* The current minimum value, and
* the `head` for each thread's array of hashes _in sorted order_ that should next be processed:

Pseudo-code:

```
    Left = Sum all unique lines that each thread has counted
    Initialize array of thread's head to 0

    Declare minimum hash
    Total = 0

    While there are lines left
        Set minimum column to none

        // Find minimum hash across all threads
        For each thread id
            if we have not processed all hashes in this thread
                if we have no minumim column
                    set minimum column to curent thread id
                else
                    if minimum hash > thread's head hash
                        minimum hash = thread's head has
                        minimum column = current thread id

        // Skip duplicate hashes NOT in the minimum column
        For each thread id
            if we have not processed all hashes in this thread
                if this thread is different from the minum hash
                    if this thread's head hash is equal to the minimum hash
                        increase thread's head pointer to next array item

        // Found a unique line, add it
        if we have a minimum column
            Total = Total + 1
            increase thread's head pointer to next array item
            Left = Left - 1
```

The implementation is thus straight-forward from the algorithm:

```c
    // n-way MergeSort into 1 unique column
    // We don't actually need to store the final array
    // as we only need to keep track of the lowest current hash across all columns
    while( nLeft > 0 )
    {
        iMinCol = -1;

        for( iThread = 0; iThread <  gnThreadsActive; ++iThread )
        {
            i = _anThreadHead[ iThread ];
            n = ganThreadWord[ iThread ];

            // If this column has not finished merging ...
            if( i < n )
            {
                hash = gapThreadData[ iThread ][ i ];

                // Look for new lowest candidate
                if( iMinCol < 0 )
                {
                    iMinCol = iThread;
                    nMinVal = hash;
                }
                else
                    if( nMinVal > hash )
                    {
                        nMinVal = hash ;
                        iMinCol = iThread;
                    }
            }
        }

        for( iThread = 0; iThread <  gnThreadsActive; ++iThread )
        {
            if( iMinCol != iThread )
            {
                i = _anThreadHead[ iThread ];
                n = ganThreadWord[ iThread ];

                if( i < n )
                {
                    hash = gapThreadData[ iThread ][ i ];

                    if( nMinVal == hash ) // remove duplicate if not in min col
                    {
                        _anThreadHead[ iThread ]++;
                        nLeft--;
                    }
                }
            }
        }

        if( iMinCol >= 0)
        {
            _anThreadHead[ iMinCol ]++;
            gnWords++;

            nLeft--;
        }
    }
```


I've annotated the OpenMP additions via:

```c
// BEGIN OMP

// END OMP
```

See [test3 opt6.c](src/test3_opt6.c)

# Benchmarks

Using a dictinary of 79,339 words from [data/words.txt](data/words.txt)
here are the `test3` metrics from 2.8 GHz i7 MacBook Pro (Mid 2014):

|Command        |Description             | Time   | Memory (bytes) |
|:--------------|:-----------------------|-------:|---------------:|
|`perl test3.pl`|Perl                    | 0.061s |     13,283,328 |
|`test3opt1`    |Fixed C Test3           |22.944s |     20,480,256 |
|`test3opt2`    |FNV1a Hash              | 0.911s |        320,256 |
|`test3opt3`    |Read file into memory   | 0.917s |        934,671 |
|`test3opt4`    |Binary search           | 0.181s |        934,671 |
|`test3opt5`    |Merged BinSearchInsKey  | 0.179s |        934,671 |
|`test3opt6 -j1`|OpenMP 1 thread         | 0.182s |      3,073,350 |
|`test3opt6 -j2`|OpenMP 2 threads        | 0.045s |      3,073,350 |
|`test3opt6 -j3`|OpenMP 3 threads        | 0.024s |      3,073,350 |
|`test3opt6 -j4`|OpenMP 4 threads        | 0.017s |      3,073,342 |
|`test3opt6 -j5`|OpenMP 5 threads        | 0.013s |      3,073,350 |
|`test3opt6 -j6`|OpenMP 6 threads        | 0.010s |      3,073,350 |
|`test3opt6 -j7`|OpenMP 7 threads        | 0.010s |      3,073,350 |
|`test3opt6 -j8`|OpenMP 8 threads        | 0.009s |      3,073,326 |

The hashing proves that this program is CPU bound -- the majority
of time is checking to see:

 * Is a word is unique; that is, does this word exist in the unique word list?
 * If not, add it.

While the single threaded optimized C version is half as slow
as the Perl reference version
it only uses 7.0% of the memory of the bloated Perl version.

The multithreaded optimized C version screams at ~ 0.010s!
It also only uses 23% of the memory compared to the Perl version.
Not bad for ~300 lines of code !

# TL:DR; Take-aways

Reviewing the performance and time required to write these programs:

|Language             |Time spent writing| Run Time |
|:--------------------|-----------------:|:---------|
| Perl                | 5 mins           | Good     |
| C (single-threaded) | 1/2 day          | Poor     |
| C (multi-threaded)  | 1 day            | Great!   |

This is just anecdotal evidence that confirms the adage:

You can have either _Fast or Cheap_, pick **one.**

The advantage of writing the single-threaded version is that
you can use that as _momentum_ to write the multi-threaded version.
Also, bugs found in one will help the other version and vice versa.

For debugging the multi-threaded version you can comment out _one line_ -- the `pragma#` !

```c
//#pragma omp parallel for
    for( iThread = 0; iThread < gnThreadsActive; ++iThread )
```

Take-aways:

_Use the right tool for the right job._

If you care about:

* _programmer time_, use a high level language
* _run time_, use
 * a better algorithm
 * multi-thread it
 * use a lower language

# Advanced Binary Search

What if we wanted something _like_ Binary Search but with minor differences.
That is, instead of searching for an _exact_ match we wanted to find keys
that are "close" -- where close can mean one of 6 things:

* `LT`:   Find the first item Less Then the key, or -1 if not found
* `LTE`:  Find the first item Less Then Or Equal to the key, or -1 if not found
* `LTEN`: Find the next item Less Then or Equal to the Key or Size if not found
* `LTN`:  Find the next item Less Then the Key or Size if not found
* `GTE`:  Find the first item Greater Than or Equal to the key
* `GT`:   Find the first item Greater Than the key

For example, given the array:

```Javascript
    [ 1, 3, 5, 7, 9, 11 ]
```

We could enumerate the various search results:

```
|Key|  == | <  | <= | <=n |  <n | >= | > |
|:--|----:|---:|---:|----:|:----|---:|--:|
| -1|  n/a|   0|   0|    0|    0| n/a|n/a|
|  0|  n/a|   0|   0|    0|    0| n/a|n/a|
|  1|    0|   1|   0|    0|    1|   0|n/a|
|  2|  n/a|   1|   1|    1|    1|   0|  0|
|  3|    1|   2|   1|    1|    2|   1|  0|
|  4|  n/a|   2|   2|    2|    2|   1|  1|
|  5|    2|   3|   2|    2|    3|   2|  1|
|  6|  n/a|   3|   3|    3|    3|   2|  2|
|  7|    3|   4|   3|    3|    4|   3|  2|
|  8|  n/a|   4|   4|    4|    4|   3|  3|
|  9|    4|   5|   4|    4|    5|   4|  3|
| 10|  n/a|   5|   5|    5|    5|   4|  3|
| 11|    5| n/a|   5|    5|    6|   5|  4|
| 12|  N/a| n/a| n/a|    5|    6|   5|  5|

Legend:

  ==  Regular (Exact) Search: pos where key == data[pos], else -1
  <   Advanced Binary Search: pos where key <  data[pos]
  <=  Advanced Binary Search: pos where key <= data[pos]
 <=n  Advanced Binary Search: pos where key <= data[pos], or last if not found
  <n  Advanced Binary Search: pos where key <  data[pos], or last if not found
  >=  Advanced Binary Search: pos where key >= data[pos]
  >   Advanced Binary Search: pos where key >  data[pos]
```

How do we find these algorithms?

Instead of returning when we find a key, lets output the current state:

```Javascript
    // ==================================================
    function BinSearchInspect( key, data )
    {
        var num = data.length - 1;
        var min = 0;
        var max = num;

        while( min <= max )
        {
            var mid = min + ((max - min) / 2) | 0;

            /**/ if (data[mid] < key)   min  = mid + 1;
            else if (data[mid] > key)   max  = mid - 1;
            else /* (data[mid] = key)*/ break;
        }
        console.log( "key: %d, min: %d, mid: %d, max: %d", key, min, mid, max );

    }
```

Running `BinSearchInspect()` on `data` we get:

```
key: -1, min: 0, mid: 0, max: -1
key:  0, min: 0, mid: 0, max: -1
key:  1, min: 0, mid: 0, max:  1
key:  2, min: 1, mid: 1, max:  0
key:  3, min: 1, mid: 1, max:  1
key:  4, min: 2, mid: 1, max:  1
key:  5, min: 0, mid: 2, max:  5
key:  6, min: 3, mid: 3, max:  2
key:  7, min: 3, mid: 3, max:  3
key:  8, min: 4, mid: 3, max:  3
key:  9, min: 3, mid: 4, max:  5
key: 10, min: 5, mid: 5, max:  4
key: 11, min: 5, mid: 5, max:  5
key: 12, min: 6, mid: 5, max:  5
```


## Advanced Binary Search C Code

```c
int BinSearchLessThanEqual( int key, const int size, int data[] )
{
    const int num = size - 1;
    /* */ int min = 0;
    /* */ int max = num;
    // var max = data.length - 1; // Javascript, Java conversion

    while( min <= max )
    {
        int mid = min + ((max - min) / 2);

        /**/ if (data[mid] < key)   min  = mid + 1;
        else if (data[mid] > key)   max  = mid - 1;
        else /* (data[mid] = key)*/ return mid    ;
    }

    if( max < 0 )
        return 0;  // key < data[0]
    else
    if( min > num )
        return -1; // key >= data[ num ] // KEY_NOT_FOUND
    else
        return (min < max)
            ? min
            : max + 1;
}

int BinSearchLessThanEqualOrLast( int key, const int size, int data[] )
{
    const int num = size - 1;
    /* */ int min = 0;
    /* */ int max = num;
    // var max = data.length - 1; // Javascript, Java conversion

    while( min <= max )
    {
        int mid = min + ((max - min) / 2);

        /**/ if (data[mid] < key)   min  = mid + 1;
        else if (data[mid] > key)   max  = mid - 1;
        else /* (data[mid] = key)*/ return mid    ;
    }

    if( max < 0 )
        return 0;     // key < data[0]
    else
    if( min > num )
        return num; // key >= data[ size-1 ]
    else
        return (min < max)
            ? min
            : max + 1;
}

int BinSearchGreaterThen( int key, const int size, int data[] )
{
    const int num = size - 1;
    /* */ int min = 0;
    /* */ int max = num;

    while( min <= max )
    {
        // To convert to Javascript:
        // var mid = min + ((max - min) / 2) | 0;
        int mid = min + ((max - min) / 2);

        /**/ if (data[mid] < key) min = mid + 1;
        else if (data[mid] > key) max = mid - 1;
        else return                     mid + 1;
    }

    if( max < 0 )
        return 0;   // key < data[0]
    else
    if( min > num )
        return num; // key >= data[ size-1 ]
    else
        return (min < max)
            ? min + 1
            : max + 1;
}
```

Corresponding Javascript versions:

```Javascript
// ==================================================
function BinSearch( key, data )
{
    var num = data.length - 1;
    var min = 0;
    var max = num;

    while( min <= max )
    {
        var mid = min + ((max - min) / 2) | 0;

        /**/ if (data[mid] < key)   min  = mid + 1;
        else if (data[mid] > key)   max  = mid - 1;
        else /* (data[mid] = key)*/ return mid    ;
    }

    return -1; // KEY_NOT_FOUND
}

// ==================================================
function BinSearchLessThanEqual( key, data )
{
    var num = data.length - 1;
    var min = 0;
    var max = num;

    while( min <= max )
    {
        var mid = min + ((max - min) / 2) | 0;

        /**/ if (data[mid] < key)   min  = mid + 1;
        else if (data[mid] > key)   max  = mid - 1;
        else /* (data[mid] = key)*/ return mid    ;
    }

    if( max < 0 )
        return 0;  // key < data[0]
    else
    if( min > num )
        return -1; // key >= data[ num ] // KEY_NOT_FOUND
    else
        return (min < max)
            ? min
            : max + 1;
}

// ==================================================
function BinSearchLessThanEqualOrLast( key, data )
{
    var num = data.length - 1;
    var min = 0;
    var max = num;

    while( min <= max )
    {
        var mid = min + ((max - min) / 2) | 0;

        /**/ if (data[mid] < key)   min  = mid + 1;
        else if (data[mid] > key)   max  = mid - 1;
        else /* (data[mid] = key)*/ return mid    ;
    }

    if( max < 0 )
        return 0;     // key < data[0]
    else
    if( min > num )
        return num; // key >= data[ size-1 ]
    else
        return (min < max)
            ? min
            : max + 1;
}

// ==================================================
function BinSearchGreaterThen( key, data )
{
    var num = data.length - 1;
    var min = 0;
    var max = num;

    while( min <= max )
    {
        // var mid = min + ((max - min) / 2) | 0;
        var mid = (min + ((max - min) / 2)) | 0;

        /**/ if (data[mid] < key)   min  = mid + 1;
        else if (data[mid] > key)   max  = mid - 1;
        else /* (data[mid] = key)*/ return mid + 1;
    }

    if( max < 0 )
        return 0;   // key < data[0]
    else
    if( min > num )
        return num; // key >= data[ size-1 ]
    else
        return (min < max)
            ? min + 1
            : max + 1;
}
```

# Data

I used this word list: ospd.txt


## Wordlist

* http://www.puzzlers.org/pub/wordlists/ospd.txt

There are also public dictionaries one could use:


## Dictionary

* http://foldoc.org/Dictionary.txt

* http://dreamsteep.com/downloads/word-games-and-wordsmith-utilities/120-the-english-open-word-list-eowl/file.html

NOTE: The .html file is actually a .zip file !

```sh
wget -O eowl.zip http://dreamsteep.com/downloads/word-games-and-wordsmith-utilities/120-the-english-open-word-list-eowl/file.html
```

```sh
wget -O 2852.txt http://www.gutenberg.org/files/2852/2852-0.txt
```


# References

* http://stackoverflow.com/questions/6441975/where-can-i-download-english-dictionary-database-in-a-text-fomat
* http://coapp.org/reference/garrett-flavored-markdown.html

* https://en.wikipedia.org/wiki/Binary_search_algorithm
* http://stackoverflow.com/questions/24348613/c-two-different-binary-search-implementation-one-stuck-in-infinite-loop?rq=1
* http://stackoverflow.com/questions/504335/what-are-the-pitfalls-in-implementing-binary-search?rq=1
* http://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed

# Special Thanks

* Chris for proofreading

--

Wednesday, January 18, 2017

Michael "Code Poet" Pohoreski, aka Michaelangel007
* _"I have no keyboard, and I must code."_

