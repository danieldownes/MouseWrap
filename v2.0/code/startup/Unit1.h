//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
        TMemo *Memo1;
        TButton *cmdLater;
        TButton *cmdNow;
        TButton *cmdCode;
        TEdit *txtCode;
    TImage *pp;
	void __fastcall FormCreate(TObject *Sender);
        void __fastcall cmdLaterClick(TObject *Sender);
        void __fastcall cmdNowClick(TObject *Sender);
        void __fastcall cmdCodeClick(TObject *Sender);
    void __fastcall ppClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif

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

int GetBaseDirectory(char *lpBuffer, int uSize)
{
	if(lpBuffer==NULL) {return 0;}

	DWORD resLen = GetModuleFileName(NULL, lpBuffer, uSize);
	if(resLen==0) {return 0;}

	//exclude file name
	do
	{
		resLen--;
	}while(lpBuffer[resLen-1] != '\\');
	lpBuffer[resLen] = 0;

	return resLen;
}