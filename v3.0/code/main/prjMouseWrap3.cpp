//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop



//---------------------------------------------------------------------------
USEFORM("main.cpp", frmMain);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmd, int)
{
        AnsiString sCmd;
        sCmd = cmd;

        // Hide program from taskbar...

        DWORD dwExStyle = GetWindowLong(Application->Handle, GWL_EXSTYLE);
        dwExStyle |= WS_EX_TOOLWINDOW;
        SetWindowLong(Application->Handle, GWL_EXSTYLE, dwExStyle);


        //  Check if program already running...

        HANDLE hInstanceMutex = ::CreateMutex(NULL,TRUE, "MouseWrap2");
        if(GetLastError() == ERROR_ALREADY_EXISTS)
        {
            if(hInstanceMutex)
                CloseHandle(hInstanceMutex);

            Application->MessageBoxA("Mouse Wrap 3 is already running.","Mouse Wrap 3 : QD Studios",0);
            exit(0);
        }


        try
        {
                 Application->Initialize();

                 Application->Hint = sCmd;



                 // User can not run directly, must be run from 'startup'
                 if(StrLIComp(cmd, "/ok", 3))
                 {
                        Application->MessageBox("Can not be run directly, please use 'MouseWrap3StartUp.exe'.", "Mouse Wrap 3",0);
                        exit(0);
                 }

                 Application->CreateForm(__classid(TfrmMain), &frmMain);
                 //Application->ShowMainForm = false;
                 Application->Title = "MouseWrap3";

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
