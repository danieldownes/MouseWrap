VERSION 5.00
Begin VB.Form frmMain 
   Caption         =   "MouseWrap"
   ClientHeight    =   1350
   ClientLeft      =   60
   ClientTop       =   375
   ClientWidth     =   2820
   Icon            =   "frmMain.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   1350
   ScaleWidth      =   2820
   StartUpPosition =   3  'Windows Default
   Visible         =   0   'False
   Begin VB.PictureBox picOff 
      Height          =   375
      Left            =   240
      Picture         =   "frmMain.frx":014A
      ScaleHeight     =   315
      ScaleWidth      =   315
      TabIndex        =   1
      Top             =   720
      Width           =   375
   End
   Begin VB.PictureBox picOn 
      Height          =   375
      Left            =   240
      Picture         =   "frmMain.frx":0294
      ScaleHeight     =   315
      ScaleWidth      =   315
      TabIndex        =   0
      Top             =   240
      Width           =   375
   End
   Begin VB.Timer Timer1 
      Interval        =   30
      Left            =   1560
      Top             =   120
   End
   Begin VB.Menu mnuPopup 
      Caption         =   "Popup"
      Visible         =   0   'False
      Begin VB.Menu mnuActive 
         Caption         =   "Active"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnuSep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuStartup 
         Caption         =   "Run at startup"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnuSep2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuAbout 
         Caption         =   "About"
      End
      Begin VB.Menu mnuExit 
         Caption         =   "Exit"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim intPos As POINTAPI

Dim intScreen As POINTAPI

Public Sub CreateIcon()
    Dim Tic As NOTIFYICONDATA
    Tic.cbSize = Len(Tic)
    Tic.hwnd = picOn.hwnd
    Tic.uID = 1&
    Tic.uFlags = NIF_DOALL
    Tic.uCallbackMessage = WM_MOUSEMOVE
    
    Select Case Timer1.Enabled
        Case True
            Tic.hwnd = picOn.hwnd
            Tic.hIcon = picOn.Picture
            Tic.szTip = "Mouse Wrap v1 [ON] - Ex-D Software Development(TM)" & Chr$(0)
            
        Case False
            Tic.hwnd = picOff.hwnd
            Tic.hIcon = picOff.Picture
            Tic.szTip = "Mouse Wrap v1 [OFF] - Ex-D Software Development(TM)" & Chr$(0)
    End Select
        
    
    Shell_NotifyIcon NIM_ADD, Tic
End Sub

Public Sub DeleteIcon()
    Dim Tic As NOTIFYICONDATA
    Tic.cbSize = Len(Tic)
    Tic.hwnd = picOn.hwnd
    Tic.uID = 1&
    
    Select Case Not Timer1.Enabled
        Case True
                Tic.hwnd = picOn.hwnd
                
        Case False
                Tic.hwnd = picOff.hwnd
    End Select
    
    Shell_NotifyIcon NIM_DELETE, Tic
End Sub

Sub UpDateState()
    DeleteIcon
    CreateIcon
End Sub


Private Sub Form_Load()
    
    ' Update the regisitry
    If GetSettingString(HKEY_LOCAL_MACHINE, "SOFTWARE\Microsoft\Windows\CurrentVersion\Run", "MouseWrap") = Empty Then
        ' Add to registery
        SaveSettingString HKEY_LOCAL_MACHINE, "SOFTWARE\Microsoft\Windows\CurrentVersion\Run", "MouseWrap", App.Path & "\MouseWrap.exe -s"
    End If
    
    'Get settings
    If GetSettingString(HKEY_CURRENT_USER, "Software\Ex-D Software DevelopmentTM\MouseWrap", "OnStartUp") = "0" Then
        mnuStartup.Checked = False
    Else
        mnuStartup.Checked = True
    End If
    
    'Running from a startup?
    If Command = "-s" And mnuStartup.Checked = False Then End
    
    
    'Already running?
    If App.PrevInstance = True Then
        MsgBox "MouseWrap is already running!"
        End
    End If
    

    Timer1.Enabled = True
    
    CreateIcon
    
    ' Store the current screen size
    intScreen.x = moddScreenSize.x
    intScreen.y = moddScreenSize.y
    
    
    ' Check for an update...
    modExVersionCheckInterface.CheckForUpdate


End Sub



Private Sub Menu(x As Single)
    x = x / Screen.TwipsPerPixelX
    
    'mnuPopup.Visible = False
    
    Select Case x
        Case WM_LBUTTONDOWN
            mnuActive_Click
            
        Case WM_RBUTTONDOWN
            
        '        ' Make the menu pictures.
        '        mnuPopup.Visible = True
        '        SetMenuBitmap Me, Array(0, 1, 0), imgStop.Picture
        '        SetMenuBitmap Me, Array(0, 1, 1), imgYield.Picture
        '        SetMenuBitmap Me, Array(0, 1, 2), imgCaution.Picture
        
                ' Display the popup menu.
                PopupMenu mnuPopup
                
        
        
        Case WM_MOUSEMOVE
            
        Case WM_LBUTTONDBLCLK
            
    End Select
    
    

End Sub



Private Sub Form_Unload(Cancel As Integer)
    
    ' Load on start?
    If mnuStartup.Checked = True Then
        SaveSettingString HKEY_CURRENT_USER, "Software\Ex-D Software DevelopmentTM\MouseWrap", "OnStartUp", "1"
    Else
        SaveSettingString HKEY_CURRENT_USER, "Software\Ex-D Software DevelopmentTM\MouseWrap", "OnStartUp", "0"
    End If
        
    Timer1.Enabled = Not Timer1.Enabled
    DeleteIcon
    End
End Sub

Private Sub mnuAbout_Click()
    Load frmAbout
    frmAbout.Visible = True
    
End Sub

Private Sub mnuActive_Click()
    mnuActive.Checked = Not mnuActive.Checked
    Timer1.Enabled = Not Timer1.Enabled
    UpDateState
End Sub

Private Sub mnuExit_Click()
    Form_Unload 0
End Sub

Private Sub mnuStartup_Click()
    mnuStartup.Checked = Not mnuStartup.Checked
End Sub

Private Sub picOff_MouseMove(Button As Integer, Shift As Integer, x As Single, y As Single)
    Menu x
End Sub

Private Sub picOn_MouseMove(Button As Integer, Shift As Integer, x As Single, y As Single)
    Menu x
End Sub

Private Sub Timer1_Timer()
    
    
    
    intPos = modGetMousePos

    
    If intPos.y = 0 Then
        intPos.y = intScreen.y - 2

        modSetMousePos intPos

    ElseIf intPos.y >= intScreen.y - 1 Then
        intPos.y = 1

        modSetMousePos intPos

    End If
    
    If intPos.x = 0 Then
        intPos.x = intScreen.x - 2
        
        modSetMousePos intPos
        
    ElseIf intPos.x = intScreen.x - 1 Then
        intPos.x = 1
        
        modSetMousePos intPos
    
    End If
    
    
    
    
    DoEvents
End Sub
