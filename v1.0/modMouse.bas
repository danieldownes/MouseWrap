Attribute VB_Name = "modMouse"
Option Explicit

Public Type POINTAPI
  x As Long
  y As Long
End Type

Declare Function GetCursorPos Lib "user32" (lpPoint As POINTAPI) As Long
'Declare Function SetCursorPosition& Lib "user32" (ByVal x As Long, ByVal y As Long)
Declare Function SetCursorPos Lib "user32" (ByVal x As Long, ByVal y As Long) As Long


Declare Function ShowCursor& Lib "user32" _
(ByVal bShow As Long)





Function modGetMousePos() As POINTAPI
    GetCursorPos modGetMousePos
End Function

Sub modSetMousePos(PosAPI As POINTAPI)
    SetCursorPos PosAPI.x, PosAPI.y
End Sub





Sub modHideMouse()
    ShowCursor& 0
End Sub

Sub modShowMouse()
    ShowCursor& 1
End Sub
