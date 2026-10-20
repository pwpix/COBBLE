#pragma once

#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <memoryapi.h>
#include <minwindef.h>
#include <windows.h>
#include <winnt.h>

#include <cstddef>
#include <print>


class MappedFile {
    public:
    MappedFile (const std::string filename)
    {
        if (!OpenFile (filename.c_str ()))
            return;
        if (!CreateMapping ())
            return;
        if (!CreateMapView ())
            return;

        isMapped = true;
        mSize    = GetMappedSize ();
        pData    = static_cast<char*> (lpMapAddress);
    }

    class Iterator {
        public:
        using iterator_concept = std::contiguous_iterator_tag;
        using iterator_category =
        std::random_access_iterator_tag;
        using value_type = char; // all chars are because file buffer.

        using difference_type = std::ptrdiff_t;
        using pointer         = const char*;
        using reference       = const value_type&;

        const char* ch;

        public:
        Iterator () noexcept : ch (nullptr)
        {
        }
        explicit Iterator (char* _start) : ch (_start)
        {
        }
        Iterator& operator++ ()
        {
            ++ch;
            return *this;
        }
        Iterator operator++ (int)
        {
            Iterator retval = *this;
            ++(*this);
            return retval;
        }

        Iterator& operator-- ()
        {
            --ch;
            return *this;
            ;
        }
        Iterator operator-- (int)
        {
            Iterator retval = *this;
            --(*this);
            return retval;
        }

        bool operator!() const
        {
            return ch == nullptr;
        }

        bool operator== (Iterator other) const
        {
            return ch == other.ch;
        }
        bool operator!= (Iterator other) const
        {
            return !(*this == other);
        }

        reference operator* () const
        {
            return *ch;
        }
        pointer operator->() const
        {
            return ch;
        }
        reference operator[] (difference_type n) const
        {
            return ch[n];
        }


        auto operator<=> (const Iterator& other) const noexcept
        {
            return ch <=> other.ch;
        };

        Iterator& operator+= (difference_type n)
        {
            ch += n;
            return *this;
        }
        Iterator& operator-= (difference_type n)
        {
            ch -= n;
            return *this;
        }


        friend Iterator operator+ (Iterator it, difference_type n)
        {
            it += n;
            return it;
        }
        friend Iterator operator+ (difference_type n, Iterator it)
        {
            it += n;
            return it;
        }
        friend Iterator operator- (Iterator it, difference_type n)
        {
            it -= n;
            return it;
        }

        // https://codereview.stackexchange.com/questions/254577/random-access-iterator-implementation
        friend constexpr difference_type
        operator- (const Iterator& a, const Iterator& b) noexcept
        {
            return a.ch - b.ch;
        }
    };

    Iterator Begin ()
    {
        return Iterator (pData);
    }

    Iterator End ()
    {
        // size_t size = GetFileSize();
        return Iterator (pData + mSize);
    }

    size_t Size ()
    {
        return GetFileSize ();
    }

    MappedFile (const MappedFile& other)            = delete;
    MappedFile (MappedFile&& other)                 = delete;
    MappedFile& operator= (const MappedFile& other) = delete;
    MappedFile& operator= (MappedFile&& other)      = delete;

    ~MappedFile ()
    {
        if (lpMapAddress)
            bFlag = UnmapViewOfFile (lpMapAddress);
        if (hMapping)
            bFlag = CloseHandle (hMapping);
        if (!bFlag) {
            std::println ("Error {} occured closing the mapping object!", GetLastError ());
        }

        // close the file itself
        if (hFile != INVALID_HANDLE_VALUE)
            bFlag = CloseHandle (hFile);
        if (!bFlag) {
            std::println ("Error {} occured closing the file", GetLastError ());
        }
    }


    private:
    int OpenFile (const char* lpcFilename)
    {
        hFile = CreateFile (lpcFilename, GENERIC_READ | GENERIC_WRITE, 0,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (hFile == INVALID_HANDLE_VALUE) {
            std::println ("hFile is NULL");
            std::println ("Target file is {}", lpcFilename);
            return 0;
        }
        return 1;
    }

    int CreateMapping ()
    {
        if (!hFile) {
            return 0;
        }
        hMapping = CreateFileMapping (hFile, nullptr, PAGE_READWRITE, 0, 0, nullptr);

        if (hMapping == nullptr) {
            std::println ("Could not create file mapping object {}", GetLastError ());
            return 0;
        }
        return 1;
    }

    int CreateMapView ()
    {
        if (!hMapping) {
            return 0;
        }

        lpMapAddress = MapViewOfFile (hMapping, FILE_MAP_READ, 0, 0, 0);

        if (lpMapAddress == nullptr) {
            std::println ("Could not map view of file {}", GetLastError ());
            return 0;
        }

        return 1;
    }

    size_t GetFileSize ()
    {
        if (GetFileSizeEx (hFile, &fileSize)) {
            if (fileSize.QuadPart == 0) {
                std::println ("File is empty!");
                return 0;
            }
        }
        return static_cast<size_t> (fileSize.QuadPart);
    }

    size_t GetMappedSize ()
    {
        if (!VirtualQuery (lpMapAddress, &mbi, sizeof (mbi))) {
            std::println ("Error fetching size of mapped region!");
        }
        return mbi.RegionSize;
    }

    HANDLE hFile    = INVALID_HANDLE_VALUE;
    HANDLE hMapping = nullptr;

    bool bFlag;
    bool isMapped;

    // DWORD dwFileSize;
    // DWORD dwMapViewSize;
    // DWORD dwFileMapStart;

    LPVOID lpMapAddress = nullptr; // pointer ot the base address of the memory-mapped
                                   // region

    LARGE_INTEGER fileSize;

    MEMORY_BASIC_INFORMATION mbi;

    char* pData; // pointer to the data
    size_t mSize;
};
