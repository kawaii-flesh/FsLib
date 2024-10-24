#include "FileBase.hpp"

FsLib::FileBase::~FileBase()
{
    FileBase::Close();
}

void FsLib::FileBase::Close(void)
{
    FSFILE_Close(m_FileHandle);
}

bool FsLib::FileBase::IsOpen(void) const
{
    return m_IsOpen;
}

uint64_t FsLib::FileBase::Tell(void) const
{
    return m_Offset;
}

uint64_t FsLib::FileBase::GetSize(void) const
{
    return m_FileSize;
}

bool FsLib::FileBase::EndOfFile(void) const
{
    return m_Offset >= m_FileSize;
}

void FsLib::FileBase::Seek(int64_t Offset, FsLib::SeekOrigin Origin)
{
    switch (Origin)
    {
        case FsLib::SeekOrigin::Beginning:
        {
            m_Offset = 0 + Offset;
        }
        break;

        case FsLib::SeekOrigin::Current:
        {
            m_Offset += Offset;
        }
        break;

        case FsLib::SeekOrigin::End:
        {
            m_Offset = m_FileSize + Offset;
        }
        break;
    }
    FileBase::EnsureOffsetIsValid();
}

void FsLib::FileBase::EnsureOffsetIsValid(void)
{
    if (m_Offset < 0)
    {
        m_Offset = 0;
    }
    else if (m_Offset > m_FileSize)
    {
        m_Offset = m_FileSize;
    }
}
