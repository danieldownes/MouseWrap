VERSION 5.00
Begin VB.Form frmAbout 
   BackColor       =   &H00000000&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "About MyApp"
   ClientHeight    =   3435
   ClientLeft      =   2340
   ClientTop       =   1935
   ClientWidth     =   5415
   ClipControls    =   0   'False
   Icon            =   "frmAbout.frx":0000
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2370.898
   ScaleMode       =   0  'User
   ScaleWidth      =   5084.965
   ShowInTaskbar   =   0   'False
   StartUpPosition =   2  'CenterScreen
   Begin VB.CommandButton cmdUpdate 
      Caption         =   "Ex-Version Check"
      Height          =   375
      Left            =   1920
      TabIndex        =   8
      ToolTipText     =   "Checks for new Ex-D Software Development(TM)  updates and any products that are not installed."
      Top             =   2880
      Width           =   1575
   End
   Begin VB.CommandButton cmdSysInfo 
      Caption         =   "System Info"
      Height          =   375
      Left            =   3720
      TabIndex        =   7
      Top             =   2880
      Width           =   1455
   End
   Begin VB.CommandButton cmdOk 
      Caption         =   "Ok"
      Default         =   -1  'True
      Height          =   375
      Left            =   240
      TabIndex        =   6
      Top             =   2880
      Width           =   1455
   End
   Begin VB.PictureBox Picture1 
      BorderStyle     =   0  'None
      Height          =   1800
      Left            =   2760
      Picture         =   "frmAbout.frx":000C
      ScaleHeight     =   1800
      ScaleWidth      =   2400
      TabIndex        =   3
      Top             =   840
      Width           =   2400
   End
   Begin VB.Label lblCopywriteTo 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      BackColor       =   &H00000000&
      Caption         =   "Copywrite ©2001..."
      ForeColor       =   &H00FFFFFF&
      Height          =   255
      Left            =   3480
      TabIndex        =   5
      Top             =   480
      Width           =   1695
   End
   Begin VB.Label lblDescription 
      BackColor       =   &H00000000&
      Caption         =   "A small utility to enhance the mouse pointer movablity in a Windows enviroment."
      ForeColor       =   &H00FFFFFF&
      Height          =   570
      Index           =   1
      Left            =   240
      TabIndex        =   4
      Top             =   720
      Width           =   2805
   End
   Begin VB.Label lblDescription 
      BackColor       =   &H00000000&
      Caption         =   "Version 1"
      ForeColor       =   &H00FFFFFF&
      Height          =   210
      Index           =   0
      Left            =   2880
      TabIndex        =   0
      Top             =   120
      Width           =   2685
   End
   Begin VB.Label lblTitle 
      Alignment       =   2  'Center
      BackColor       =   &H00000000&
      Caption         =   "MouseWrap™"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   20.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   600
      Left            =   120
      TabIndex        =   2
      Top             =   120
      Width           =   2670
   End
   Begin VB.Label lblDisclaimer 
      BackColor       =   &H00000000&
      Caption         =   $"frmAbout.frx":E14E
      ForeColor       =   &H00FFFFFF&
      Height          =   1305
      Left            =   240
      TabIndex        =   1
      Top             =   1560
      Width           =   2415
   End
End
Attribute VB_Name = "frmAbout"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit






Private Declare Function sndPlaySound Lib "winmm.dll" Alias "sndPlaySoundA" _
     (ByVal lpszSoundName As String, ByVal uFlags As Long) As Long

'  flag values for uFlags parameter
Const SND_SYNC = &H0            '  play synchronously (default)
Const SND_ASYNC = &H1           '  play asynchronously
Const SND_NODEFAULT = &H2       '  silence not default, if sound not found
Const SND_MEMORY = &H4          '  lpszSoundName points to a memory file
Const SND_ALIAS = &H10000       '  name is a WIN.INI [sounds] entry
Const SND_FILENAME = &H20000    '  name is a file name
Const SND_RESOURCE = &H40004    '  name is a resource name or atom
Const SND_ALIAS_ID = &H110000   '  name is a WIN.INI [sounds] entry identifier
Const SND_ALIAS_START = 0       '  must be > 4096 to keep strings in same section of resource file
Const SND_LOOP = &H8            '  loop the sound until next sndPlaySound
Const SND_NOSTOP = &H10         '  don't stop any currently playing sound
Const SND_VALID = &H1F          '  valid flags          / ;Internal /
Const SND_NOWAIT = &H2000       '  don't wait if the driver is busy
Const SND_VALIDFLAGS = &H17201F '  Set of valid flag bits.  Anything outside
                                '  this range will raise an error
Const SND_RESERVED = &HFF000000 '  In particular these flags are reserved
Const SND_TYPE_MASK = &H170007

'Private Sub Command1_Click()
'End Sub
'
'Private Sub Command2_Click()
'    Dim l As Long
'    l = sndPlaySound("", SND_SYNC Or SND_NODEFAULT)
'End Sub
















' Reg Key Security Options...
Const READ_CONTROL = &H20000
Const KEY_QUERY_VALUE = &H1
Const KEY_SET_VALUE = &H2
Const KEY_CREATE_SUB_KEY = &H4
Const KEY_ENUMERATE_SUB_KEYS = &H8
Const KEY_NOTIFY = &H10
Const KEY_CREATE_LINK = &H20
Const KEY_ALL_ACCESS = KEY_QUERY_VALUE + KEY_SET_VALUE + _
                       KEY_CREATE_SUB_KEY + KEY_ENUMERATE_SUB_KEYS + _
                       KEY_NOTIFY + KEY_CREATE_LINK + READ_CONTROL
                     
' Reg Key ROOT Types...
Const HKEY_LOCAL_MACHINE = &H80000002
Const ERROR_SUCCESS = 0
Const REG_SZ = 1                         ' Unicode nul terminated string
Const REG_DWORD = 4                      ' 32-bit number

Const gREGKEYSYSINFOLOC = "SOFTWARE\Microsoft\Shared Tools Location"
Const gREGVALSYSINFOLOC = "MSINFO"
Const gREGKEYSYSINFO = "SOFTWARE\Microsoft\Shared Tools\MSINFO"
Const gREGVALSYSINFO = "PATH"

Private Declare Function RegOpenKeyEx Lib "advapi32" Alias "RegOpenKeyExA" (ByVal hKey As Long, ByVal lpSubKey As String, ByVal ulOptions As Long, ByVal samDesired As Long, ByRef phkResult As Long) As Long
Private Declare Function RegQueryValueEx Lib "advapi32" Alias "RegQueryValueExA" (ByVal hKey As Long, ByVal lpValueName As String, ByVal lpReserved As Long, ByRef lpType As Long, ByVal lpData As String, ByRef lpcbData As Long) As Long
Private Declare Function RegCloseKey Lib "advapi32" (ByVal hKey As Long) As Long


Private Sub cmdSysInfo_Click()
  Call StartSysInfo
End Sub

Private Sub cmdOK_Click()
  Unload Me
End Sub

Private Sub cmdUpdate_Click()
    modExVersionCheckInterface.ShowDownloads
End Sub

Private Sub Form_Load()
    'Play sound...
    Dim l As Long
    l = sndPlaySound(App.Path & "\ex-d mono.wav", SND_ASYNC Or SND_NODEFAULT)
        
    Me.Caption = "About " & App.Title
    
End Sub

Public Sub StartSysInfo()
    On Error GoTo SysInfoErr
  
    Dim rc As Long
    Dim SysInfoPath As String
    
    ' Try To Get System Info Program Path\Name From Registry...
    If GetKeyValue(HKEY_LOCAL_MACHINE, gREGKEYSYSINFO, gREGVALSYSINFO, SysInfoPath) Then
    ' Try To Get System Info Program Path Only From Registry...
    ElseIf GetKeyValue(HKEY_LOCAL_MACHINE, gREGKEYSYSINFOLOC, gREGVALSYSINFOLOC, SysInfoPath) Then
        ' Validate Existance Of Known 32 Bit File Version
        If (Dir(SysInfoPath & "\MSINFO32.EXE") <> "") Then
            SysInfoPath = SysInfoPath & "\MSINFO32.EXE"
            
        ' Error - File Can Not Be Found...
        Else
            GoTo SysInfoErr
        End If
    ' Error - Registry Entry Can Not Be Found...
    Else
        GoTo SysInfoErr
    End If
    
    Call Shell(SysInfoPath, vbNormalFocus)
    
    Exit Sub
SysInfoErr:
    MsgBox "System Information Is Unavailable At This Time", vbOKOnly
End Sub

Public Function GetKeyValue(KeyRoot As Long, KeyName As String, SubKeyRef As String, ByRef KeyVal As String) As Boolean
    Dim i As Long                                           ' Loop Counter
    Dim rc As Long                                          ' Return Code
    Dim hKey As Long                                        ' Handle To An Open Registry Key
    Dim hDepth As Long                                      '
    Dim KeyValType As Long                                  ' Data Type Of A Registry Key
    Dim tmpVal As String                                    ' Tempory Storage For A Registry Key Value
    Dim KeyValSize As Long                                  ' Size Of Registry Key Variable
    '------------------------------------------------------------
    ' Open RegKey Under KeyRoot {HKEY_LOCAL_MACHINE...}
    '------------------------------------------------------------
    rc = RegOpenKeyEx(KeyRoot, KeyName, 0, KEY_ALL_ACCESS, hKey) ' Open Registry Key
    
    If (rc <> ERROR_SUCCESS) Then GoTo GetKeyError          ' Handle Error...
    
    tmpVal = String$(1024, 0)                             ' Allocate Variable Space
    KeyValSize = 1024                                       ' Mark Variable Size
    
    '------------------------------------------------------------
    ' Retrieve Registry Key Value...
    '------------------------------------------------------------
    rc = RegQueryValueEx(hKey, SubKeyRef, 0, _
                         KeyValType, tmpVal, KeyValSize)    ' Get/Create Key Value
                        
    If (rc <> ERROR_SUCCESS) Then GoTo GetKeyError          ' Handle Errors
    
    If (Asc(Mid(tmpVal, KeyValSize, 1)) = 0) Then           ' Win95 Adds Null Terminated String...
        tmpVal = Left(tmpVal, KeyValSize - 1)               ' Null Found, Extract From String
    Else                                                    ' WinNT Does NOT Null Terminate String...
        tmpVal = Left(tmpVal, KeyValSize)                   ' Null Not Found, Extract String Only
    End If
    '------------------------------------------------------------
    ' Determine Key Value Type For Conversion...
    '------------------------------------------------------------
    Select Case KeyValType                                  ' Search Data Types...
    Case REG_SZ                                             ' String Registry Key Data Type
        KeyVal = tmpVal                                     ' Copy String Value
    Case REG_DWORD                                          ' Double Word Registry Key Data Type
        For i = Len(tmpVal) To 1 Step -1                    ' Convert Each Bit
            KeyVal = KeyVal + Hex(Asc(Mid(tmpVal, i, 1)))   ' Build Value Char. By Char.
        Next
        KeyVal = Format$("&h" + KeyVal)                     ' Convert Double Word To String
    End Select
    
    GetKeyValue = True                                      ' Return Success
    rc = RegCloseKey(hKey)                                  ' Close Registry Key
    Exit Function                                           ' Exit
    
GetKeyError:      ' Cleanup After An Error Has Occured...
    KeyVal = ""                                             ' Set Return Val To Empty String
    GetKeyValue = False                                     ' Return Failure
    rc = RegCloseKey(hKey)                                  ' Close Registry Key
End Function
