//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("option.cpp", Form1);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmd, int)
{



        HANDLE hInstanceMutex = ::CreateMutex(NULL,TRUE, "MouseWrap2Options");
        if(GetLastError() == ERROR_ALREADY_EXISTS)
        {
            if(hInstanceMutex)
                CloseHandle(hInstanceMutex);

//            Application->MessageBoxA("Mouse Wrap 2 options is already running.","Mouse Wrap 2 : Ex-D Software Development(TM)",0);
            exit(0);
        }


	try
	{
		Application->Initialize();

                // User can not run directly, must be run from 'startup'
                if(strcmp(cmd,"/ok"))
                {
                       Application->MessageBox("Can not be run directly, please use 'MouseWrap2StartUp.exe'.", "Mouse Wrap 2 component",0);
                       exit(0);
                }

		Application->CreateForm(__classid(TForm1), &Form1);
        Application->Name = "Options";
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}

    ReleaseMutex(hInstanceMutex);
    CloseHandle(hInstanceMutex);


	return 0;
}
//---------------------------------------------------------------------------
