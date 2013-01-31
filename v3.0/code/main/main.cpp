#define COMPILE_WRAP 1
#define COMPILE_RECON 0

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "trayicon"
#pragma resource "*.dfm"

TfrmMain *frmMain;



#if COMPILE_RECON
// Movement Recognition
void CheckRecognisedMovements(void);


struct MoveDetect_S
{
	int iDir;       // 8 = Up; 2 = Down; etc (See Num pad)
	DWORD fTime;
};

MoveDetect_S stMovement[8];

int iDetectTimeOut;

int iCurDir;

AnsiString sCmd;

int iSelVertAction;
int iSelHoriAction;
#endif

POINT pOld[3];

// Stats


void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
        char *ReadReg(char *sKey);

        char strT[1000];

        AnsiString sTemp;

	TrayIcon->Visible = TRUE;
//        frmMain->WindowState = wsMinimized;
        frmMain->Left = -2300;
        frmMain->Top  = -2300;

	// Read varible values ...

#if COMPILE_RECON

        strcpy(strT, ReadReg("HoriRecog"));
        sTemp = strT;
        iSelHoriAction = StrToInt(sTemp);


        strcpy(strT, ReadReg("VertRecog"));
        sTemp = strT;
        iSelVertAction = StrToInt(sTemp);


        strcpy(strT, ReadReg("RecogTimeout"));
        sTemp = strT;
	    iDetectTimeOut = StrToInt(sTemp);

#endif


}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
#if COMPILE_RECON
        sCmd = Application->Hint.c_str();
#endif
}



void __fastcall TfrmMain::timControlTimer(TObject *Sender)
{
	POINT temp;

	GetCursorPos(&temp);

        // Size of the screen resolution
	int width=::GetSystemMetrics(SM_CXSCREEN);
	int height=::GetSystemMetrics(SM_CYSCREEN);


        // Size of desktop (e.g. minus dockable panels such as the taskbar)
        RECT rect;
        SystemParametersInfo( SPI_GETWORKAREA, NULL, &rect, NULL );



	// Check and 'Wrap' ...


	if(temp.x == 0)
	{
		SetCursorPos(width - 2, temp.y);

#if COMPILE_RECON
		temp.x = width - 2;
		pOld[0].x = width;
		temp.y = pOld[0].y;
#endif
	}

	if(temp.x == width - 1)
	{
		SetCursorPos(1, temp.y);

#if COMPILE_RECON
		temp.x = 1;
		pOld[0].x = 0;
		temp.y = pOld[0].y;
#endif
	}

	if(temp.y == 0)
	{
		SetCursorPos(temp.x, height - 2);

#if COMPILE_RECON
		temp.y = height - 2;
		pOld[0].y = height - 1;
		temp.x = pOld[0].x;
#endif
	}

	if(temp.y == height - 1)
	{
		SetCursorPos(temp.x, 1);

#if COMPILE_RECON
		temp.y = 1;
		pOld[0].y = 0;
		temp.x = pOld[0] .x;
#endif
	}


	//////////////////////////

        // Check if pointer is 'stuck' on a taskbar
        // or docked panel edge ...

//        Label1->Caption = temp.x;
//        Label2->Caption = rect.left;

        // Taskbar at bottom
        if( rect.bottom != height )
        {
            if( rect.bottom-1 == temp.y && temp.y == pOld[0].y && pOld[0].y == pOld[1].y
                && pOld[1].y == pOld[2].y )
            {
                SetCursorPos(temp.x, 2);
                pOld[0].y = 0;
            }
        }

        // Taskbar at top
        if( rect.top != 0 )
        {
            if( rect.top+1 == temp.y && temp.y == pOld[0].y && pOld[0].y == pOld[1].y
                && pOld[1].y == pOld[2].y )
            {
                SetCursorPos(temp.x, height - 2);
                pOld[0].y = height-1;
            }
        }

        // Taskbar at left
        if( rect.left != 0 )
        {
            if( rect.left == temp.x && temp.x == pOld[0].x && pOld[0].x == pOld[1].x
                && pOld[1].x == pOld[2].x )
            {
                SetCursorPos(width-2, temp.y);
                pOld[0].x = width-1;
            }
        }

        // Taskbar at right
        if( rect.right != width )
        {
            if( rect.right-1 == temp.x && temp.x == pOld[0].x && pOld[0].x == pOld[1].x
                && pOld[1].x == pOld[2].x )
            {
                SetCursorPos(2, temp.y);
                pOld[0].x = 0;
            }
        }


        // Keep record of the last few mouse coords.
        pOld[2] = pOld[1];
        pOld[1] = pOld[0];
        pOld[0] = temp;


#if COMPILE_RECON
	//////////////////////////

	// Movment recognition ...

	int iXdiff;
	int iYdiff;

	iXdiff = temp.x - pOld[1].x;
	iYdiff = temp.y - pOld[1].y;



	if( iXdiff == 0 && iYdiff == 0 )
	{
		// Mouse has not moved
		iCurDir = 0;
	}
	else
	{
		// Mouse has moved

		if( abs(iXdiff) > abs(iYdiff) )
		{
			// We are mostly moving on X plain

			if( iXdiff > 0 )
			{
				// Mouse is moving /?Up?/
				iCurDir = 6;
			}
			else
			{
				iCurDir = 4;
			}
		}
		else
		{
			// We are mostly moving on Y plain
			if( iYdiff > 0 )
			{
				iCurDir = 2;
			}
			else
			{
				iCurDir = 8;
			}
		}
	}

	// Record new movment
	if(iCurDir != 0)
	{
		if(stMovement[0].iDir != iCurDir)
		{

			// Shift other timings up
			for(int n = 6; n >= 0; n--)
			{
				stMovement[n + 1].iDir = stMovement[n].iDir;
				stMovement[n + 1].fTime = stMovement[n].fTime;
			}

			// Insert into space just made
			stMovement[0].iDir = iCurDir;
			stMovement[0].fTime = timeGetTime();
		}
	}

	// Evaluate movments
	bool bOk;
	int iNextDir;
	int n;
	DWORD dTemp;

	bOk = true;
	iNextDir = stMovement[0].iDir;
	n = 0;

	dTemp = timeGetTime();

	do
	{
		if(stMovement[n].iDir != iNextDir)
		{
			bOk = false;
			n = 0;
		}

		// Check if still within timeout period
		if(dTemp < stMovement[n].fTime + iDetectTimeOut)
		{

			switch(iNextDir)
			{
				case 8:
					iNextDir = 2;
					break;

				case 2:
					iNextDir = 8;
					break;

				case 4:
					iNextDir = 6;
					break;

				case 6:
					iNextDir = 4;
					break;
			}

			n++;
		}
		else
		{
			bOk = false;
			n = 0;
		}

		// If ok after n direction changes, then trigger action
		if(n >= 7)
		{
            //Application->MessageBox("action!","Mouse Wrap",0);

			bOk = false;

			//ClearArray();  ...
            for(int n = 0; n <= 7; n++)
            {
    			stMovement[n].iDir = 0;
                stMovement[n].fTime = 0;
            }

			if(iNextDir == 8 || iNextDir == 2)
			{
				//Application->MessageBox("Vertical trigger!","Mouse Wrap",0);

                                switch(iSelVertAction)
                                {
                                        case 1:
                                                CtrlCut();
                                                break;
                                        case 2:
                                                CtrlCopy();
                                                break;
                                        case 3:
                                                CtrlPaste();
                                                break;
                                        case 4:
                                                BackInIE();
                                                break;
                                        case 5:
                                                ForwardInIE();
                                                break;
                                }
			}

			if(iNextDir == 6 || iNextDir == 4)
			{
				//Application->MessageBox("Horizontal trigger!","Mouse Wrap",0);

				switch(iSelHoriAction)
                                {
                                        case 1:
                                                CtrlCut();
                                                break;
                                        case 2:
                                                CtrlCopy();
                                                break;
                                        case 3:
                                                CtrlPaste();
                                                break;
                                        case 4:
                                                BackInIE();
                                                break;
                                        case 5:
                                                ForwardInIE();
                                                break;
                                }
			}

		}

	}while(bOk);

	////////////////////////
#endif


	////////////////////////




}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::mnuExitClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------





void __fastcall TfrmMain::mnuMouseWrapHomeClick(TObject *Sender)
{
	ShellExecute(0, "open", "http://www.qdstudios.com/mousewrap/index.php?v=3.3", NULL, NULL, SW_SHOWMAXIMIZED);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::mnuOptonsClick(TObject *Sender)
{
	char sTemp[1000];

	//std::string sOut;

	GetBaseDirectory(sTemp,sizeof(sTemp));

	strcat(sTemp, "options.exe");


	ShellExecute(0, "open", sTemp, "/ok", NULL, SW_SHOWNORMAL);
}


void ToggleStatus(void)
{
        if(frmMain->timControl->Enabled)
        {
                frmMain->TrayIcon->SetIconIndex(1);
                frmMain->TrayIcon->Hint = "OFF : Mouse Wrap 3 : QD Studios";
                frmMain->mnuTurn->Caption = "&Turn ON";
        }
        else
        {
                frmMain->TrayIcon->SetIconIndex(0);
                frmMain->TrayIcon->Hint = "ON : Mouse Wrap 3 : QD Studios";
                frmMain->mnuTurn->Caption = "&Turn OFF";
        }
        frmMain->timControl->Enabled = !frmMain->timControl->Enabled;
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::TrayIconClick(TObject *Sender)
{
        ToggleStatus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::mnuTurnClick(TObject *Sender)
{
        ToggleStatus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::mnuHelpAboutClick(TObject *Sender)
{
      	ShellExecute(0, "open", "http://www.qdstudios.com/mousewrap/about.php?v=3.3", NULL, NULL, SW_SHOWMAXIMIZED);
}
//---------------------------------------------------------------------------


