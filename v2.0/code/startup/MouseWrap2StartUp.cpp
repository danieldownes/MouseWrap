//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("Unit1.cpp", frmMain);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmd, int)
{
	try
	{
                char *ReadReg(char *sKey);

        	if(!strcmp(cmd,"/start") && !strcmp(ReadReg("OnStartUp"), "0"))
	        {
        	        exit(0);
	        }

		Application->Initialize();
		Application->Title = "MouseWrap2Start";
                Application->CreateForm(__classid(TfrmMain), &frmMain);
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
	return 0;
}
//---------------------------------------------------------------------------

char *ReadReg(char *sKey)
{
	HKEY hk;
	DWORD dwData;
	UCHAR szBuf[80];

	/*
	 * Add your source name as a subkey under the Application
	 * key in the EventLog service portion of the registry.
	 */

	if(RegCreateKey(HKEY_CURRENT_USER, "Software\\Ex-D Software DevelopmentTM\\MouseWrap2",&hk))
		Application->MessageBox("could not open registry key","Mouse Wrap 2 - Error: RegRead");


	/* Add the Event ID message-file name to the subkey. */
	DWORD Size;
	Size = 80;

	RegQueryValueEx(hk,             	/* subkey handle         */
		sKey,                   		/* value name            */
		NULL,                        	/* must be zero          */
		NULL,            				/* value type            */
		(LPBYTE) szBuf,           		/* address of value data */
		&Size);       					/* length of value data  */


	RegCloseKey(hk);

//	Application->MessageBoxA(szBuf,"Mouse Wrap 2 - Error: RegRead");

	return szBuf;
}
