object Form1: TForm1
  Left = 1003
  Top = 132
  Width = 270
  Height = 446
  Caption = 'rip'#39'r.m'#178
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 3
    Top = 29
    Width = 20
    Height = 13
    Caption = 'from'
  end
  object Label2: TLabel
    Left = 75
    Top = 29
    Width = 9
    Height = 13
    Caption = 'to'
  end
  object Image1: TImage
    Left = 0
    Top = 165
    Width = 257
    Height = 251
    Center = True
    Proportional = True
  end
  object Label3: TLabel
    Left = 248
    Top = 168
    Width = 3
    Height = 13
  end
  object Label4: TLabel
    Left = 248
    Top = 184
    Width = 3
    Height = 13
  end
  object Label5: TLabel
    Left = 64
    Top = 256
    Width = 32
    Height = 13
    Caption = 'Label5'
  end
  object Label6: TLabel
    Left = 64
    Top = 280
    Width = 32
    Height = 13
    Caption = 'Label6'
  end
  object Label7: TLabel
    Left = 64
    Top = 304
    Width = 32
    Height = 13
    Caption = 'Label7'
  end
  object Label8: TLabel
    Left = 64
    Top = 328
    Width = 32
    Height = 13
    Caption = 'Label8'
  end
  object Label9: TLabel
    Left = 128
    Top = 256
    Width = 32
    Height = 13
    Caption = 'Label9'
  end
  object Label10: TLabel
    Left = 128
    Top = 280
    Width = 38
    Height = 13
    Caption = 'Label10'
  end
  object Label11: TLabel
    Left = 128
    Top = 304
    Width = 38
    Height = 13
    Caption = 'Label11'
  end
  object Label12: TLabel
    Left = 128
    Top = 328
    Width = 38
    Height = 13
    Caption = 'Label12'
  end
  object path: TEdit
    Left = 1
    Top = 1
    Width = 208
    Height = 21
    TabOrder = 0
    Text = 'e:\screenshoot\'
  end
  object imgNbStart: TEdit
    Left = 29
    Top = 25
    Width = 41
    Height = 21
    TabOrder = 1
    Text = '1'
  end
  object imgNbEnd: TEdit
    Left = 89
    Top = 25
    Width = 42
    Height = 21
    TabOrder = 2
    Text = '374'
  end
  object startBtn: TButton
    Left = 212
    Top = 1
    Width = 48
    Height = 22
    Caption = 'start job !'
    TabOrder = 3
    OnClick = startBtnClick
  end
  object log: TMemo
    Left = 3
    Top = 52
    Width = 253
    Height = 100
    TabOrder = 4
  end
  object loadColorArrayChk: TCheckBox
    Left = 136
    Top = 29
    Width = 121
    Height = 17
    Caption = 'load color array'
    Checked = True
    State = cbChecked
    TabOrder = 5
  end
  object progressBar: TProgressBar
    Left = 4
    Top = 152
    Width = 252
    Height = 9
    Min = 0
    Max = 100
    TabOrder = 6
  end
end
