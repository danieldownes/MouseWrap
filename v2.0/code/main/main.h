//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "trayicon.h"
#include <Menus.hpp>
#include <ImgList.hpp>

#include <string>

#include <mmsystem.h>

#include <winuser.h>
#include <ImgList.hpp>


#include "mw2_move_recon.cpp"
#include "mw2_stats.cpp"


//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
    TTimer *timControl;
	TTrayIcon *TrayIcon;
	TPopupMenu *PopupMenu;
	TMenuItem *mnuExit;
	TImageList *ImageList;
	TMenuItem *mnuOptons;
	TMenuItem *mnuMouseWrapHome;
        TMenuItem *mnuHelpAbout;
        TMenuItem *mnuSep2;
        TMenuItem *mnuTurn;
        TMenuItem *mnuSep1;
        TLabel *Label1;
        TLabel *Label2;
    void __fastcall timControlTimer(TObject *Sender);
    void __fastcall mnuExitClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall mnuMouseWrapHomeClick(TObject *Sender);
	void __fastcall mnuOptonsClick(TObject *Sender);
        void __fastcall TrayIconClick(TObject *Sender);
        void __fastcall mnuTurnClick(TObject *Sender);
        void __fastcall mnuHelpAboutClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif

//////////  Own functions


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

/*
void SetNumLock( BOOL bState )
{
	BYTE keyState[256];

	GetKeyboardState((LPBYTE)&keyState);

	if( (bState && !(keyState[VK_NUMLOCK] & 1)) ||
	(!bState && (keyState[VK_NUMLOCK] & 1)) )
	{
		// Simulate a key press
		keybd_event( VK_NUMLOCK,
		0x45,
		KEYEVENTF_EXTENDEDKEY | 0,
		0 );

		// Simulate a key release
		keybd_event( VK_NUMLOCK,
		0x45,
		KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
		0);
	}

}
*/

void CtrlCut()
{
	BYTE keyState[256];

	GetKeyboardState((LPBYTE)&keyState);

	// Simulate key presses
	keybd_event( VK_CONTROL,
	0x45,
	KEYEVENTF_EXTENDEDKEY | 0,
	0 );


	keybd_event( 88,
	0x45,
	KEYEVENTF_EXTENDEDKEY | 0,
	0 );

	// Simulate key releases
	keybd_event( 88,
	0x45,
	KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
	0);


	keybd_event( VK_CONTROL,
	0x45,
	KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
	0); 

}

void CtrlCopy()
{
	BYTE keyState[256];

	GetKeyboardState((LPBYTE)&keyState);

	// Simulate key presses
	keybd_event( VK_CONTROL,
	0x45,
	KEYEVENTF_EXTENDEDKEY | 0,
	0 );

	keybd_event( 67,
	0x45,
	KEYEVENTF_EXTENDEDKEY | 0,
	0 );

	// Simulate key releases
	keybd_event( 67,
	0x45,
	KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
	0);

	keybd_event( VK_CONTROL,
	0x45,
	KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
	0);
}


void CtrlPaste()
{
	BYTE keyState[256];

	GetKeyboardState((LPBYTE)&keyState);

	// Simulate key presses
	keybd_event( VK_CONTROL,
	0x45,
	KEYEVENTF_EXTENDEDKEY | 0,
	0 );

	keybd_event( 86,
	0x45,
	KEYEVENTF_EXTENDEDKEY | 0,
	0 );

	// Simulate key releases
	keybd_event( 86,
	0x45,
	KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
	0);

	keybd_event( VK_CONTROL,
	0x45,
	KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
	0);
}

void ForwardInIE()
{
	BYTE keyState[256];

	GetKeyboardState((LPBYTE)&keyState);

	// Simulate key presses
	keybd_event( VK_MENU,
	0x45,
	KEYEVENTF_EXTENDEDKEY | 0,
	0 );

	keybd_event( VK_RIGHT,
	0x45,
	KEYEVENTF_EXTENDEDKEY | 0,
	0 );

	// Simulate key releases
	keybd_event( VK_RIGHT,
	0x45,
	KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
	0);

	keybd_event( VK_MENU,
	0x45,
	KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
	0);
}

void BackInIE()
{
	BYTE keyState[256];

	GetKeyboardState((LPBYTE)&keyState);

	// Simulate key presses
	keybd_event( VK_MENU,
	0x45,
	KEYEVENTF_EXTENDEDKEY | 0,
	0 );

	keybd_event( VK_LEFT,
	0x45,
	KEYEVENTF_EXTENDEDKEY | 0,
	0 );

	// Simulate key releases
	keybd_event( VK_LEFT,
	0x45,
	KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
	0);

	keybd_event( VK_MENU,
	0x45,
	KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
	0);
}


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
		Application->MessageBox("An error occured please reinstall Mouse Wrap2","Mouse Wrap 2 - Error: RegRead");


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
