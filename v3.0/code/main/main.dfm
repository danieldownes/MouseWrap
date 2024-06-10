object frmMain: TfrmMain
  Left = 246
  Top = 210
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'MouseWrap3'
  ClientHeight = 36
  ClientWidth = 144
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  Scaled = False
  WindowState = wsMinimized
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object timControl: TTimer
    Interval = 80
    OnTimer = timControlTimer
  end
  object TrayIcon: TTrayIcon
    Hint = 'On : Mouse Wrap 3 : QD Studios'
    PopupMenu = PopupMenu
    Hide = True
    RestoreOn = imDoubleClick
    PopupMenuOn = imRightClickUp
    Icons = ImageList
    OnClick = TrayIconClick
    Left = 72
  end
  object PopupMenu: TPopupMenu
    Left = 40
    object mnuHelpAbout: TMenuItem
      Caption = '&About Mouse Wrap v3.3'
      OnClick = mnuHelpAboutClick
    end
    object mnuMouseWrapHome: TMenuItem
      Caption = '&Check for new version'
      OnClick = mnuMouseWrapHomeClick
    end
    object mnuOptons: TMenuItem
      Caption = '&Options'
      OnClick = mnuOptonsClick
    end
    object mnuSep1: TMenuItem
      Caption = '-'
    end
    object mnuTurn: TMenuItem
      Caption = '&Turn OFF'
      Default = True
      OnClick = mnuTurnClick
    end
    object mnuSep2: TMenuItem
      Caption = '-'
    end
    object mnuExit: TMenuItem
      Caption = 'E&xit'
      OnClick = mnuExitClick
    end
  end
  object ImageList: TImageList
    Left = 104
    Bitmap = {
      494C010102000400040010001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000001000000001002000000000000010
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000024312400243124002431
      240027902E002431240024312400243124002431240024312400243124002790
      2E00243124002431240024312400000000000000000024312400243124002431
      2400243124002431240024312400243124002431240024312400243124002431
      2400243124002431240024312400000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000024312400FFFFFF00FFFFFF002790
      2E0027902E00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF002790
      2E0027902E00FFFFFF00FFFFFF0024312400243124000000B700FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF000000B700243124000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000024312400FFFFFF0027902E002DDC
      360027902E004F5050004F5050004F5050004F5050004F5050004F5050002790
      2E002DDC360027902E00FFFFFF00243124000000B7000B0BFF000000B7004F50
      50004F5050004F5050004F5050004F5050004F5050004F5050004F5050004F50
      50004F5050000000B7000B0BFF000000B7000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000002431240027902E002DDC36002DDC
      360027902E004F5050004F5050004F5050009B9B9B006A6A6A004F5050002790
      2E002DDC36002DDC360027902E00243124000000B7000B0BFF000B0BFF000000
      B7004F5050004F5050004F5050004F5050009B9B9B006A6A6A004F5050004F50
      50000000B7000B0BFF000B0BFF000000B7000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000027902E003EEF48003EEF48002DDC
      360027902E004F5050006A6A6A009B9B9B009B9B9B006A6A6A004F5050002790
      2E002DDC36003EEF48003EEF480027902E000000B7000B0BFF002F2FFF002F2F
      FF000000B7004F5050006A6A6A009B9B9B009B9B9B006A6A6A004F5050000000
      B7002F2FFF002F2FFF000B0BFF000000B7000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000003EEF48003EEF48003EEF48002DDC
      360027902E004F5050006A6A6A009B9B9B00B1AEB1009B9B9B004F5050002790
      2E002DDC36003EEF48003EEF48003EEF48000000B7000B0BFF002F2FFF002F2F
      FF002F2FFF000000B7006A6A6A009B9B9B00B1AEB1009B9B9B000000B7002F2F
      FF002F2FFF002F2FFF000B0BFF000000B7000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000027902E003EEF48003EEF48002DDC
      360027902E004F5050006A6A6A009B9B9B009B9B9B004F5050004F5050002790
      2E002DDC36003EEF48003EEF480027902E000000B7000B0BFF002F2FFF002F2F
      FF000000B7004F5050006A6A6A009B9B9B009B9B9B004F5050004F5050000000
      B7002F2FFF002F2FFF000B0BFF000000B7000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000002431240027902E002DDC36002DDC
      360027902E004F5050006A6A6A009B9B9B004F5050004F5050004F5050002790
      2E002DDC36002DDC360027902E00243124000000B7000B0BFF000B0BFF000000
      B7004F5050004F5050006A6A6A009B9B9B004F5050004F5050004F5050004F50
      50000000B7000B0BFF000B0BFF000000B7000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000024312400FFFFFF0027902E002DDC
      360027902E004F5050004F5050004F5050004F5050004F5050004F5050002790
      2E002DDC360027902E00FFFFFF00243124000000B7000B0BFF000000B7004F50
      50004F5050004F5050004F5050004F5050004F5050004F5050004F5050004F50
      50004F5050000000B7000B0BFF000000B7000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000024312400FFFFFF00FFFFFF002790
      2E0027902E00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF002790
      2E0027902E00FFFFFF00FFFFFF0024312400243124000000B700FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF000000B700243124000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000024312400243124002431
      240027902E002431240024312400243124002431240024312400243124002790
      2E00243124002431240024312400000000000000000024312400243124002431
      2400243124002431240024312400243124002431240024312400243124002431
      2400243124002431240024312400000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000100000000100010000000000800000000000000000000000
      000000000000000000000000FFFFFF00FFFFFFFF00000000FFFFFFFF00000000
      FFFFFFFF00000000800180010000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000008001800100000000
      FFFFFFFF00000000FFFFFFFF0000000000000000000000000000000000000000
      000000000000}
  end
end