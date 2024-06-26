//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

char *ReadReg(char *sKey);
void WriteReg(char *sKey, char *sValue);
int GetBaseDirectory(char *lpBuffer, int uSize);
bool TryCode(int );

int iXX = 6508;

TfrmMain *frmMain;

//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
        char strT[1000];


        AnsiString sTemp;

        Application->Title = "MouseWrap2Start";

      	GetBaseDirectory(strT,sizeof(strT));

        sTemp = ReadReg("RecogActive");

        if(sTemp == "1")
        {
                strcat(strT, "components\\MouseWrap2Recog.exe");

                sTemp = "/ok";
                sTemp = sTemp + ReadReg("RecogTimeout");

                ShellExecute(0, "open", strT, sTemp.c_str(), NULL, SW_NORMAL);
        }
        else
        {
                strcat(strT, "components\\MouseWrap2.exe");
                ShellExecute(0, "open", strT, "/ok", NULL, SW_NORMAL);
        }


        strcpy(strT, ReadReg("Misc"));
        sTemp = strT;
        txtCode->Text = sTemp;
        if(TryCode(iXX))
                exit(0);
                

        sTemp = ReadReg("Remind");

        sTemp = IntToStr( StrToInt(sTemp) - 1);

        if(StrToInt(sTemp) <= 0)
                 sTemp = "5";

        WriteReg("Remind", sTemp.c_str());

        if(StrToInt(sTemp) < 5)
                exit(0);


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

	if (RegCreateKey(HKEY_CURRENT_USER, "Software\\Ex-D Software DevelopmentTM\\MouseWrap2",&hk))
		Application->MessageBox("could not create registry key","Mouse Wrap 2 - Error: WriteRead");


	/* Set the Event ID message-file name. */

	strcpy(szBuf, sValue);


	/* Add the Event ID message-file name to the subkey. */

	if (RegSetValueEx(hk,             /* subkey handle         */
		sKey,                     /* value name            */
		0,                        /* must be zero          */
		REG_EXPAND_SZ,            /* value type            */
		(LPBYTE) szBuf,           /* address of value data */
		strlen(szBuf) + 1))       /* length of value data  */

	//Application->MessageBox("could not set event message file","Mouse Wrap 2 - Error: WriteRead");
	//ErrorExit("could not set event message file");

	RegCloseKey(hk);
}



void __fastcall TfrmMain::cmdCodeClick(TObject *Sender)
{



        if(!txtCode->Visible)
        {
                // Input field
                cmdLater->Visible = false;
                cmdNow->Visible = false;
                cmdCode->Caption = "&Try code";
                cmdCode->Default = true;
                txtCode->Visible = true;
                txtCode->Text = "";
                txtCode->SetFocus();
        }
        else
        {
                if(TryCode(iXX))
                {
                        Application->MessageBoxA("Thanks for your support, donate prompt has been disabled.", "Mouse Wrap 2",0);
                        WriteReg("Misc",txtCode->Text.c_str());
                        exit(0);
                }
                else
                {
                        Application->MessageBoxA("The code you have specified is incorrect.", "Mouse Wrap 2",0);
                }

        }
}
//---------------------------------------------------------------------------

bool TryCode(int iX)
{
   // Check code

   AnsiString sTemp;
   sTemp = IntToHex(899356524 * iX - 1,20);
   sTemp = AnsiLowerCase(sTemp);
   if(frmMain->txtCode->Text == sTemp)
        return true;
   else
        return false;


}

void __fastcall TfrmMain::cmdNowClick(TObject *Sender)
{
        ShellExecute(0, "open", "http://www.ex-d.net/products/utils/mousewrap/donate.asp", NULL, NULL, SW_SHOWMAXIMIZED);
        exit(0);
}

void __fastcall TfrmMain::cmdLaterClick(TObject *Sender)
{
        exit(0);
}
void __fastcall TfrmMain::ppClick(TObject *Sender)
{
        ShellExecute(0, "open", "http://www.ex-d.net/products/utils/mousewrap/donate.asp", NULL, NULL, SW_SHOWMAXIMIZED);
        exit(0);
}
//---------------------------------------------------------------------------

