object preview: Tpreview
  Left = 263
  Top = 665
  Width = 257
  Height = 279
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  BorderStyle = bsSizeToolWin
  Color = clBtnFace
  TransparentColorValue = clFuchsia
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object Image1: TImage
    Left = 0
    Top = 0
    Width = 249
    Height = 252
    Center = True
    IncrementalDisplay = True
    Stretch = True
    OnMouseDown = Image1MouseDown
    OnMouseMove = Image1MouseMove
    OnMouseUp = Image1MouseUp
  end
  object ComboShowOnlyColor: TComboBox
    Left = 2
    Top = 0
    Width = 87
    Height = 21
    AutoDropDown = True
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 1
    TabOrder = 0
    Text = 'bg'
    OnChange = ComboShowOnlyColorChange
    Items.Strings = (
      'all'
      'bg'
      'sprite'
      'twice'
      'suspect'
      'bg only'
      'sprite only')
  end
end
