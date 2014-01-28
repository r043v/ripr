object Form2: TForm2
  Left = 444
  Top = 397
  Width = 362
  Height = 413
  Caption = 'output map'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Visible = True
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object map: TImage
    Left = 0
    Top = 0
    Width = 353
    Height = 385
    Center = True
    Proportional = True
    Stretch = True
  end
  object saveBtn: TButton
    Left = 0
    Top = 3
    Width = 41
    Height = 15
    Caption = 'save'
    TabOrder = 0
    OnClick = saveBtnClick
  end
end
