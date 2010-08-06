/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The implementation of FakeFile class.
 *
 *****************************************************************************/

// SFTA header files
#include <sfta/fake_file.hh>

// Standard library header files
#include <stdexcept>
#include <cstdlib>


using SFTA::Private::FakeFile;


FakeFile::FakeFile()
	: ptrBuffer_(static_cast<char*>(0)),
	  bufferSize_(0),
		ptrFile_(static_cast<FILE*>(0)),
		hasBeenOpened_(false),
		isClosed_(false),
		writeMode_(false)
{ }


FILE* FakeFile::OpenWrite()
{
	if (hasBeenOpened_)
	{	// in case the stream has already been opened
		throw std::runtime_error("Opening memory stream more than once");
	}

	if ((ptrFile_ = open_memstream(&ptrBuffer_, &bufferSize_))
		== static_cast<FILE*>(0))
	{	// in case the stream could not be created
		throw std::runtime_error("Could not create memory stream");
	}

	hasBeenOpened_ = true;
	isClosed_ = false;
	writeMode_ = true;

	return ptrFile_;
}


FILE* FakeFile::OpenRead(std::string str)
{
	if (hasBeenOpened_)
	{	// in case the stream has already been opened
		throw std::runtime_error("Opening memory stream more than once");
	}

	if ((ptrFile_ = fmemopen(const_cast<char*>(str.c_str()), str.length(), "r"))
		== static_cast<FILE*>(0))
	{	// in case the stream could not be created
		throw std::runtime_error("Could not create memory stream");
	}

	hasBeenOpened_ = true;
	isClosed_ = false;

	return ptrFile_;
}


void FakeFile::Close()
{
	if (isClosed_)
	{	// if the file has already been closed
		throw std::runtime_error("Closing already closed file");
	}

	if (!hasBeenOpened_)
	{	// if the file has not been opened
		throw std::runtime_error("Closing file that has not been opened");
	}

	if (fclose(ptrFile_))
	{	// if the file could not be closed
		throw std::runtime_error("Could not close file");
	}

	isClosed_ = true;
}


std::string FakeFile::GetContent() const
{
	if (!hasBeenOpened_)
	{	// in case the file has not been opened
		return "";
	}

	if (!isClosed_)
	{	// in case the file has not been closed
		throw std::runtime_error("Attempt to read content of unclosed file");
	}

	if (!writeMode_)
	{	// in case the file has not been opened for writing
		throw std::runtime_error("Attempt to read content of read-only file");
	}

	return std::string(ptrBuffer_, bufferSize_);
}


FakeFile::~FakeFile()
{
	if (!hasBeenOpened_)
	{	// if the file has not been opened, simply return
		return;
	}

	if (!isClosed_)
	{	// in case the file has not been closed
		if (fclose(ptrFile_))
		{	// if the file could not be closed
			throw std::runtime_error("Could not close file");
		}
	}

	// free the memory used for the buffer
	if (ptrBuffer_ != static_cast<char*>(0))
	{	// in case some buffer has been allocated
		free(ptrBuffer_);
	}
}
