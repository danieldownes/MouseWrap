//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("option.cpp", frmOptions);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmd, int)
{



        HANDLE hInstanceMutex = ::CreateMutex(NULL,TRUE, "MouseWrap3Options");
        if(GetLastError() == ERROR_ALREADY_EXISTS)
        {
            if(hInstanceMutex)
                CloseHandle(hInstanceMutex);

//            Application->MessageBoxA("Mouse Wrap options is already running.","Mouse Wrap 3",0);
            exit(0);
        }


	try
	{
		Application->Initialize();

                // User can not run directly, must be run from 'startup'
                if(strcmp(cmd,"/ok"))
                {
                       Application->MessageBox("Can not be run directly, please use 'MouseWrap3StartUp.exe'.", "Mouse Wrap 3 component",0);
                       exit(0);
                }

		Application->CreateForm(__classid(TfrmOptions), &frmOptions);
        Application->Name = "Options";
		Application->Title = "Mouse Wrap 3 Options";
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
