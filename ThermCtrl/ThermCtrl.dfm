object Form3: TForm3
  Left = 826
  Top = 179
  Caption = #1059#1087#1088#1072#1074#1083#1077#1085#1080#1077' '#1090#1077#1088#1084#1086#1082#1072#1084#1077#1088#1086#1081
  ClientHeight = 637
  ClientWidth = 932
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 465
    Top = 0
    Height = 618
    ExplicitLeft = 521
    ExplicitTop = 19
  end
  object pnlChart: TPanel
    Left = 468
    Top = 0
    Width = 464
    Height = 618
    Align = alClient
    TabOrder = 0
    object Chart: TChart
      Left = 1
      Top = 1
      Width = 462
      Height = 616
      Legend.LegendStyle = lsSeries
      Title.Text.Strings = (
        #1058#1077#1084#1087#1077#1088#1072#1090#1091#1088#1072' '#1074' '#1082#1072#1084#1077#1088#1077)
      BottomAxis.DateTimeFormat = 'hh:mm:ss'
      View3D = False
      Align = alClient
      TabOrder = 0
      PrintMargins = (
        31
        15
        31
        15)
      object Series1: TLineSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Visible = False
        Title = 'T'
        Pointer.InflateMargins = True
        Pointer.Style = psRectangle
        Pointer.Visible = False
        XValues.DateTime = True
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
        Data = {
          001900000000000000003C844000000000001088400000000000A88B40000000
          00006088400000000000D0864000000000007C8A400000000000DC8940000000
          0000308B400000000000308B4000000000005C8C400000000000148940000000
          000090854000000000006C86400000000000F0894000000000008C8940000000
          0000108D400000000000E08F400000000000408F400000000000608D40000000
          0000548F400000000000908F400000000000988C400000000000748D40000000
          00008C8E400000000000129140}
      end
      object Series2: TPointSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Shadow.Color = 8618883
        Marks.Visible = False
        ClickableLine = False
        Pointer.InflateMargins = True
        Pointer.Style = psDownTriangle
        Pointer.Visible = True
        XValues.DateTime = True
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 465
    Height = 618
    Align = alLeft
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnCanResize = Panel1CanResize
    OnResize = Panel1Resize
    object gbFind: TGroupBox
      Left = 12
      Top = 16
      Width = 439
      Height = 65
      Caption = ' '#1055#1086#1080#1089#1082' '#1090#1077#1088#1084#1086#1082#1072#1084#1077#1088#1099' '
      TabOrder = 0
      object lAdd: TLabel
        Left = 205
        Top = 27
        Width = 40
        Height = 16
        Caption = #1040#1076#1088#1077#1089':'
      end
      object cbTermo: TComboBox
        Left = 113
        Top = 24
        Width = 82
        Height = 24
        Style = csDropDownList
        TabOrder = 0
        OnCloseUp = cbTermoCloseUp
        Items.Strings = (
          'Terchi T6800'
          #1048#1056#1058'5502')
      end
      object cbCPort: TComboBox
        Left = 20
        Top = 24
        Width = 75
        Height = 24
        Style = csDropDownList
        TabOrder = 1
      end
      object bttFind: TButton
        Left = 296
        Top = 24
        Width = 89
        Height = 25
        Caption = #1055#1086#1080#1089#1082
        TabOrder = 2
        OnClick = bttFindClick
      end
    end
    object gbManual: TGroupBox
      Left = 12
      Top = 96
      Width = 439
      Height = 90
      Caption = ' '#1056#1091#1095#1085#1086#1077' '#1091#1087#1088#1072#1074#1083#1077#1085#1080#1077' '
      TabOrder = 1
      object Label1: TLabel
        Left = 246
        Top = 24
        Width = 24
        Height = 16
        Caption = '['#176'C]'
      end
      object Label2: TLabel
        Left = 246
        Top = 59
        Width = 24
        Height = 16
        Caption = '['#176'C]'
      end
      object Button1: TButton
        Left = 20
        Top = 24
        Width = 149
        Height = 25
        Caption = #1057#1090#1072#1088#1090' '#1085#1072' '#1091#1089#1090#1072#1074#1082#1091' :'
        TabOrder = 0
        OnClick = Button1Click
      end
      object Button2: TButton
        Left = 333
        Top = 40
        Width = 75
        Height = 25
        Caption = #1057#1090#1086#1087
        TabOrder = 1
        OnClick = Button2Click
      end
      object Button3: TButton
        Left = 20
        Top = 55
        Width = 149
        Height = 25
        Caption = #1063#1090#1077#1085#1080#1077' '#1090#1077#1084#1087#1077#1088#1072#1090#1091#1088#1099':'
        TabOrder = 2
        OnClick = Button3Click
      end
    end
    object gbAuto: TGroupBox
      Left = 20
      Top = 192
      Width = 439
      Height = 297
      Caption = ' '#1040#1074#1090#1086#1084#1072#1090#1080#1095#1077#1089#1082#1086#1077' '#1091#1087#1088#1072#1074#1083#1077#1085#1080#1077' '
      TabOrder = 2
      DesignSize = (
        439
        297)
      object Label3: TLabel
        Left = 12
        Top = 22
        Width = 17
        Height = 16
        Caption = 'T :'
      end
      object bttStartAuto: TButton
        Left = 3
        Top = 260
        Width = 75
        Height = 25
        Caption = #1057#1090#1072#1088#1090
        TabOrder = 0
        OnClick = bttStartAutoClick
      end
      object bttStopAuto: TButton
        Left = 352
        Top = 260
        Width = 75
        Height = 25
        Anchors = [akTop, akRight]
        Caption = #1057#1090#1086#1087
        TabOrder = 1
        OnClick = bttStopAutoClick
      end
    end
  end
  object sbInfo: TStatusBar
    Left = 0
    Top = 618
    Width = 932
    Height = 19
    Panels = <>
  end
end
