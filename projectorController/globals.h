#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdlib>
#include <string>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::IO;

static std::string gc2std(System::String^ s)
{
	using System::IntPtr;
	using System::Runtime::InteropServices::Marshal;

	IntPtr ip = Marshal::StringToHGlobalAnsi(s);
	std::string ss;
	try 
	{
		ss = static_cast<const char*>(ip.ToPointer());
	} catch (std::bad_alloc&) 
	{
		Marshal::FreeHGlobal(ip);
		throw;
	}
	Marshal::FreeHGlobal(ip);
	return ss;
}

ref class FileInfoNameComparer : IComparer
{
	public: virtual int Compare(Object^ x, Object^ y)
	{
		FileInfo^ objX = (FileInfo^)x;
		FileInfo^ objY = (FileInfo^)y;
		return objX->Name->CompareTo(objY->Name)* -1;
	}
};

static int getImages(IplImage**& imagesBuffer, int numImages, String^ dirLocation, String^ filePattern)
{
	DirectoryInfo^ dir = gcnew DirectoryInfo(dirLocation);
	if(!dir->Exists)
		return 0;

	array<FileInfo^>^ files = dir->GetFiles(filePattern);
	files->Sort(files, gcnew FileInfoNameComparer());
	
	numImages = (numImages < files->Length)?numImages:files->Length;
	if(numImages <= 0)
		return 0;

	imagesBuffer = new IplImage* [numImages];
	for(int i=0; i<numImages; i++)
		imagesBuffer[i] = cvLoadImage(gc2std(files[i]->FullName).c_str());
	
	return numImages;
}

#endif