Attribute VB_Name = "modExVersionCheckInterface"
' Ex-Version Checker MUST be referenced


Dim dllExVersionCheck As New exVersionChecker



Sub CheckForUpdate()
    If dllExVersionCheck.CheckForNew("MouseWrap", Trim(Str(App.Major)) & "." & Trim(Str(App.Minor))) = 3 Then dllExVersionCheck.ExVersionCheck
End Sub

Sub ShowDownloads()

    dllExVersionCheck.ExVersionCheck
End Sub

