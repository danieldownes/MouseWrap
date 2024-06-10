// Registry values must be present; no error trapping here

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "option.h"

void WriteReg(char *sKey, char *sValue);
char *ReadReg(char *sKey);
//bool CheckRecogTimeOut(void);

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmOptions *frmOptions;

int iRecogTimeout;


//---------------------------------------------------------------------------
__fastcall TfrmOptions::TfrmOptions(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmOptions::FormCreate(TObject *Sender)
{
	AnsiString temp;

        /* commented out v3.00
	lstHorizontal->Items->Add("Disabled");
	lstHorizontal->Items->Add("Cut");
	lstHorizontal->Items->Add("Copy");
	lstHorizontal->Items->Add("Paste");
	lstHorizontal->Items->Add("Back in IE");
	lstHorizontal->Items->Add("Forward in IE");

	lstVertical->Items->Add("Disabled");
	lstVertical->Items->Add("Cut");
	lstVertical->Items->Add("Copy");
	lstVertical->Items->Add("Paste");
	lstVertical->Items->Add("Back in IE");
	lstVertical->Items->Add("Forward in IE");
        */

	// Load values

	temp = ReadReg("OnStartUp");						// Start-Up setting
	chkStartupLoad->Checked = temp.ToInt();

        /*
	temp = ReadReg("RecogActive");                      // Pattern Recognition setting
	chkRecogActive->Checked = StrToInt(temp);

	temp = ReadReg("HoriRecog");						// Horizontal action
	lstHorizontal->ItemIndex = StrToInt(temp);

	temp = ReadReg("VertRecog");						// Vertical action
	lstVertical->ItemIndex = StrToInt(temp);

	temp = ReadReg("RecogTimeout");						// Recognition Time-out
	iRecogTimeout = StrToInt(temp);
	CheckRecogTimeOut();
	txtRecogTimeout->Text = IntToStr(iRecogTimeout);
        */
}
//---------------------------------------------------------------------------

void __fastcall TfrmOptions::Button1Click(TObject *Sender)
{
	AnsiString temp;
	char sTemp[10];

	// Save all settings...

	//if(CheckRecogTimeOut() == true)
	//{
		if(chkStartupLoad->Checked == 1)       			        // Start-Up setting
			temp = "1";
		else
			temp = "0";
		strcpy(sTemp,temp.c_str());
		WriteReg("OnStartUp", sTemp);
             /*
		if(chkRecogActive->Checked == 1)       			        // Pattern Recognition setting
			temp = "1";
		else
			temp = "0";
		strcpy(sTemp,temp.c_str());
		WriteReg("RecogActive", sTemp);

		temp = IntToStr(lstHorizontal->ItemIndex);		        // Horizontal action
		strcpy(sTemp,temp.c_str());
		WriteReg("HoriRecog", sTemp);

		temp = IntToStr(lstVertical->ItemIndex);		        // Vertical action
		strcpy(sTemp,temp.c_str());
		WriteReg("VertRecog", sTemp);

		temp = IntToStr(iRecogTimeout);					// Recogition Time-out
		strcpy(sTemp,temp.c_str());
		WriteReg("RecogTimeout", sTemp);
              */
		//Application->MessageBox("New settings will take affect next time Mouse Wrap 3 is started.","Mouse Wrap 3");

		exit(0);
	//}
	//else
	//{
	//	txtRecogTimeout->Text = IntToStr(iRecogTimeout);
	//}
}
//---------------------------------------------------------------------------



void WriteReg(char *sKey, char *sValue)
{
	HKEY hk;
	DWORD dwData;
	UCHAR szBuf[80];

	/*
	 * Add your source name as a subkey under the Application
	 * key in the EventLog service portion of the registry.
	 */

	if (RegCreateKey(HKEY_CURRENT_USER, "Software\\QDStudios\\MouseWrap3",&hk))
		Application->MessageBox("could not create registry key","Mouse Wrap 3 - Error: WriteRead");


	/* Set the Event ID message-file name. */

	strcpy(szBuf, sValue);


	/* Add the Event ID message-file name to the subkey. */

	if (RegSetValueEx(hk,             /* subkey handle         */
		sKey,                     /* value name            */
		0,                        /* must be zero          */
		REG_EXPAND_SZ,            /* value type            */
		(LPBYTE) szBuf,           /* address of value data */
		strlen(szBuf) + 1))       /* length of value data  */

	Application->MessageBox("could not set event message file","Mouse Wrap 3 - Error: WriteRead");
	//ErrorExit("could not set event message file");

	RegCloseKey(hk);
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

	if(RegCreateKey(HKEY_CURRENT_USER, "Software\\QDStudios\\MouseWrap3",&hk))
		Application->MessageBox("could not open registry key","Mouse Wrap 3 - Error: RegRead");


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

//	Application->MessageBoxA(szBuf,"Mouse Wrap 3 - Error: RegRead");

	return szBuf;
}
//---------------------------------------------------------------------------

  /*
void __fastcall TfrmOptions::sclRecogTimeoutClick(TObject *Sender, TUDBtnType Button)
{
	switch(Button)
	{
		case btNext:
			iRecogTimeout += 5;

			break;

		case btPrev:
			iRecogTimeout -= 5;

			break;
	}

	if(CheckRecogTimeOut() == true)
	{
		txtRecogTimeout->Text = IntToStr(iRecogTimeout);
	}
}
//---------------------------------------------------------------------------


bool CheckRecogTimeOut(void)
{
	bool bOk = true;

	if(iRecogTimeout < 50)
	{
		Application->MessageBox("Timeout must be between 50 and 5000 m/secs","Mouse Wrap 3");
		iRecogTimeout = 50;
		bOk = false;
	}

	if(iRecogTimeout > 5000)
	{
		Application->MessageBox("Timeout must be between 50 and 5000 m/secs","Mouse Wrap 3");
		iRecogTimeout = 5000;
		bOk = false;
	}

	return bOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOptions::txtRecogTimeoutChange(TObject *Sender)
{
	if(txtRecogTimeout->Text != "")
	{
		iRecogTimeout = StrToInt(txtRecogTimeout->Text);
	}
}
//---------------------------------------------------------------------------
*/


void __fastcall TfrmOptions::Button2Click(TObject *Sender)
{
	exit(0);
}
//---------------------------------------------------------------------------



