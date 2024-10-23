#include "FileBase.hpp"

FsLib::FileBase::~FileBase()
{
    FileBase::Close();
}

void FsLib::FileBase::Close(void)
{
    fsFileClose(&m_FileHandle);
}
