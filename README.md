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


# Bugs

There are _numerous_ things wrong with the C version.
How many **noob mistakes** can you spot?

Take a minute.

That _wasn't_ just a rhetorical question.
One of the best ways to become a better code is to
_find_ bugs in existing code that already has them.

Once you're ready for all the answers here is the full list of everything wrong
in [src/test3_annotated.c](src/test3_annotated.c)

* https://github.com/Michaelangel007/perl_crap_reference/blob/master/src/test3_annotated.c

The 3 biggest problems are:

* The bloody thing doens't even compile !
* Has an off-by-one bug!
* Not scalable

# Apples to Oranges

Table 19-8 has benchmark times showing that the Perl version is faster then the C version.

|Operation                                 | Test 3|
|:-----------------------------------------|--------:|
|Compile C program once, execute 100 times | 1103.75 |
|Execute interpreted Perl script 100 times |  197.00 |

First, where is the _data file_ to independently verify the timing???

Second, except this an _apples-to-oranges-comparision_ -- **different algorithms** are used!
Of course the Perl version is faster.
Obviously, the _better algorithm_ in a slower language
is going to "win" against the _worse algorithm_ in a faster language!
**[No shit, Sherlock!](http://knowyourmeme.com/memes/no-shit-sherlock--2)**

The author gives the _excuse:_

> We could have used a binary tree or even a hashing system,
> but that would have added significantly more lines to an already long C program.

1. _No one give a shit how many lines of code C is_ -- the **entire** point of using
  **C is for SPEED;** not some _bullshit metric!_

 Does that make _-2,000 Lines of Code_ even better?!

 * http://www.folklore.org/StoryView.py?project=Macintosh&story=Negative_2000_Lines_Of_Code.txt&sortOrder=Sort+by+Date&topic=Management

2. Apparently 34 lines of code is long?!?!?! Wait till the author finds
  out how this _toy example_ compares in the _Real World (TM)_ where there are
  _hundreds of thousands of lines of code in a project!_

  Apparently including an additional 38 lines for a total of 72 lines was too much work !

/sarcasm _"Hey, that would have been twice the work, right?"_


# Optimizations

There are numerous optimizations that can be done:

1. Read the entire file into memory at once bypassing the C library buffering.
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

 Notice the crappy variable names:

  * `x` which means `data`
  * the lowercase L, `l`, is horrible to read in some fonts, instead of `lo`
  * `u` is OK, but better would be `hi`
  * `m` is ambigious; does it mean minimum? median? maximum?
  * What we are searching for is called the badly named non-descript `t` instead of a good name like `key`

 Also the argument order should always be in a consistent fashion:

  * Array Size
  * Array Pointer

 Here is a cleaned up version with _good_ variable names:

 ```c
    int BinarySearch( uint32_t needle, int size, uint32_t *haystack )

        int min = 0;
        int mid = 0;
        int max = size-1;

        while( min <= max )
        {
            mid = (min + max) >> 1;

            /**/ if( haystack[ mid ] == needle )   return mid  ;
            else if( haystack[ mid ] >  needle )   max  = mid-1;
            else /*                  <  needle )*/ min  = mid+1;
        }
 ```

 We are finding a `needle` in a `haystack`. :)

 If we are doing a binary search and then insert if not found,
 we can tweak the binary search to return the _negative position of middle._
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
this is a _lock-free_ algorithm. It is _trivial to parallized_ or what is known as
an [Embarrassingly parallel](https://en.wikipedia.org/wiki/Embarrassingly_parallel) problem.

Why?

If we have a file of N size and T threads (or cores) each thread
gets _part_ of the file N/T -- which is independent of the other threads!

Each thread does this:

* Generate a hash for each line
* Use binary search to see if the hash is in the dictionary
* If not, insert it in sorted order

If we were simply counting lines we would be Good-to-go (TM). But life is
(almost usually) never that simple.

The _snafu_ is that we are counting _unique_ lines.  That means we need
to generate a hash for the _entire_ line. If a line straddles the boundary
between two threads we need to adjust the end of the previous thread,
and ajust he start of the next thread.

Example:

We have this file with 3 lines of text (12 bytes)

```bash
    echo -e -n "abcdef\ngh\ni\n" > 12.txt
```

Which looks like:

```
    abcdef
    gh
    i
```

Which is a linear stream of bytes:

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

Once all threads are done let's pretend we have 4 cores (or threads) and these are the hashes:

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

One of the simpliest ways to  remove duplicates is to perform an N-way [merge sort](https://en.wikipedia.org/wiki/Merge_sort)

We need to keep track of:

* Current minimum value
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
Here are the `test3` metrics:

|Command        |Description             | Time   | Memory (bytes) |
|:--------------|:-----------------------|-------:|---------------:|
|`perl test3.pl`|Perl                    | 0.061s |     13,283,328 |
|`test3opt1`    |Fixed C Test3           |22.944s |     20,480,256 |
|`test3opt2`    |FNV1a Hash              | 0.911s |        320,256 |
|`test3opt3`    |Read file into memory   | 0.917s |        934,671 |
|`test3opt4`    |Binary search           | 0.181s |        934,671 |
|`test3opt5`    |Merged BinSearchInsKey  | 0.179s |        934,671 |
|`test3opt6 -j1`|OpenMP 1 thread         | 0.182s |      3,073,350 |
|`test3opt6 -j1`|OpenMP 2 threads        | 0.045s |      3,073,350 |
|`test3opt6 -j1`|OpenMP 3 threads        | 0.024s |      3,073,350 |
|`test3opt6 -j1`|OpenMP 4 threads        | 0.017s |      3,073,342 |
|`test3opt6 -j1`|OpenMP 5 threads        | 0.013s |      3,073,350 |
|`test3opt6 -j1`|OpenMP 6 threads        | 0.010s |      3,073,350 |
|`test3opt6 -j1`|OpenMP 7 threads        | 0.010s |      3,073,350 |
|`test3opt6 -j1`|OpenMP 7 threads        | 0.009s |      3,073,326 |

The hashing proves that this program is CPU bound -- the majority
of time is checking to see if an word is unique; that is,
does this word exist in the unique word list? If not, add it.

While the single threaded optimized C version is half as slow,
it only uses 7.0% of the memory of the bloated Perl version.

The multithreaded optimized C version screams at ~ 0.010s!
It also only uses 23% of the memory compared to the Perl version.
Not for for ~300 lines of code !

# TL:DR; Take-aways

Reviewing the performance and time required to write these programs:

|Language             |Time spent writing| Run Time |
|:--------------------|-----------------:|:---------|
| Perl                | 5 mins           | Good     |
| C (single-threaded) | 1/2 day          | Poor     |
| C (multi-threaded)  | 1 day            | Great!   |

This is just anecdotal evidence that confirms the adage:

_You can have either _Fast_ or _Quick_, pick one._

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

--

Friday, January 13, 2017

Michael "Code Poet" Pohoreski, aka Michaelangel007
_"I have no keyboard, and I must code."_

