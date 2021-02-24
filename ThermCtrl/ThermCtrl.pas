unit ThermCtrl;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, TeEngine, Series, TeeProcs, Chart, StdCtrls, Mask, RzEdit,
  IniFiles, ThHCC, ComCtrls, RzSpnEdt, Grids, AdvObj, BaseGrid, AdvGrid,
  Common, SyncObjs;

type

  TPoint = record
    temp: single;
    time: integer;
    delay: integer;
  end;

  TProject = record
    points: array of TPoint;
  end;

  // TTime=record
  // neHour:array of TRzNumericEdit;
  // neMinuts:array of TRzNumericEdit;
  // end;

  TCmd_HCC = class(TObject)
    private
      PRcd_HCC: TPRcd_HCC;
      Connect: boolean;
    public
      procedure PwrOffClick();
      procedure PwrOnClick(ValT: single);
      function GetT(var ValT: single): boolean;

      constructor Create(var rcd_HCC: TRcd_HCC); overload;
  end;

  TForm3 = class(TForm)
    pnlChart: TPanel;
    Panel1: TPanel;
    Chart: TChart;
    Series1: TLineSeries;
    gbFind: TGroupBox;
    cbTermo: TComboBox;
    cbCPort: TComboBox;
    gbManual: TGroupBox;
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    neValManual: TRzNumericEdit;
    gbAuto: TGroupBox;
    bttStartAuto: TButton;
    bttStopAuto: TButton;
    bttFind: TButton;
    sbInfo: TStatusBar;
    Splitter1: TSplitter;
    neAdd: TRzNumericEdit;
    lAdd: TLabel;
    neSetT: TRzNumericEdit;
    Label1: TLabel;
    Label2: TLabel;
    seNTT: TRzSpinEdit;
    Label3: TLabel;
    Series2: TPointSeries;
    procedure LoadPoint();
    procedure SavePoint();
    procedure FormCreate(Sender: TObject);
    procedure OnGetEditorTypeAdvSg(Sender: TObject; ACol, ARow: integer;
      var AEditor: TEditorType);

    procedure OnlClickCellAdvSG(Sender: TObject; ARow, ACol: integer);
    procedure OnGetAlignmentAdvSG(Sender: TObject; ARow, ACol: integer;
      var HAlign: TAlignment; var VAlign: TAsgVAlignment);
    procedure OnKeyPress(Sender: TObject; var Key: Char);
    // procedure OnGetEditorTypeAdvSg(Sender: TObject; ACol, ARow: integer; var AEditor: TEditorType);
    // procedure OnCellValidate(Sender: TObject; ACol, ARow: Integer; var Value: String; var Valid: Boolean);
    // procedure OnEditCellDone(Sender: TObject; ACol, ARow: Integer);

    procedure TimeToSring(var valHour: integer; var valMinuts: integer;
      sTime: string);
    procedure Load();
    procedure Save();
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure bttFindClick(Sender: TObject);
    procedure OnTerminate(Sender: TObject);
    procedure cbTermoCloseUp(Sender: TObject);
    procedure Panel1CanResize(Sender: TObject;
      var NewWidth, NewHeight: integer; var Resize: boolean);
    procedure Button3Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure seNTTChange(Sender: TObject);
    procedure seNTTChanging(Sender: TObject; NewValue: Extended;
      var AllowChange: boolean);
    procedure Panel1Resize(Sender: TObject);
    procedure bttStartAutoClick(Sender: TObject);
    procedure HCCMeasuring(val: single);
    procedure bttStopAutoClick(Sender: TObject);
    procedure FormShow(Sender: TObject);

    private
      { Private declarations }
    public
      AdvSg_TI: TAdvStringGrid;
      lstPortNumber, lstPortName: TStrings;
      idxPort: integer;
      mtxNeedCom: THANDLE;
      HCC: TRcd_HCC;
      Prj: TProject;
      CPoint: integer; // 0,1,2....
      Stage: integer; // 0,1,2....
      StartTime: TDateTime;
      StopTime: TDateTime;
      OThHCC: ThHCC_UN;
      OEvt_Break: TEvent;
      CurrPoint: TPoint;
      delta: integer;
      // ODelay:array of TTime;
      // OMsrm:array of TTime;
      procedure ScanPort(NameCom, NumberCom: TStrings);
      { Public declarations }
  end;

var
  Form3: TForm3;

implementation

uses time;
{$R *.dfm}

{ ****************************************************************************** }
constructor TCmd_HCC.Create(var rcd_HCC: TRcd_HCC);
  begin
    inherited Create();
    self.PRcd_HCC := @rcd_HCC;
  end;

procedure TCmd_HCC.PwrOffClick();
  var
    pThread: Pointer;
    hHCC: THANDLE;
  begin
    if Connect then
      begin
        ThHCC_UN(pThread) := ThHCC_UN.CreatePwOff(PRcd_HCC^);
        ThHCC_UN(pThread).OnTerminate := nil;
        ThHCC_UN(pThread).FreeOnTerminate := true;
        hHCC := ThHCC_UN(pThread).Handle;
        ThHCC_UN(pThread).Start;
        // ThHCC_UN(pThread).Resume;
        if WaitForSingleObject(hHCC, 5000) <> WAIT_TIMEOUT then
          begin ;
          end
        else
          begin
            if assigned(ThHCC_UN(pThread)) then
              ThHCC_UN(pThread).Terminate;
            ShowMessage('Термокамера не отвечает.');
          end;
      end
    else
      begin
        ShowMessage('Отсутствует связь с камерой тепла/холода.');
      end;
  end;

procedure TCmd_HCC.PwrOnClick(ValT: single);
  var
    pThread: Pointer;
    hHCC: THANDLE;
  begin
    if Connect then
      begin
        ThHCC_UN(pThread) := ThHCC_UN.CreatePwOn(PRcd_HCC^, ValT);
        ThHCC_UN(pThread).OnTerminate := nil;
        ThHCC_UN(pThread).FreeOnTerminate := true;
        hHCC := ThHCC_UN(pThread).Handle;
        ThHCC_UN(pThread).Start;
        if WaitForSingleObject(hHCC, 5000) <> WAIT_TIMEOUT then
          begin ;
          end
        else
          begin
            if assigned(ThHCC_UN(pThread)) then
              ThHCC_UN(pThread).Terminate;
            ShowMessage('Термокамера не отвечает.');
          end;
      end
    else
      begin
        ShowMessage('Отсутствует связь с камерой тепла/холода.');
      end;
  end;

function TCmd_HCC.GetT(var ValT: single): boolean;
  var
    pThread: Pointer;
    hHCC: THANDLE;
  begin
    result := false;
    if Connect then
      begin
        ThHCC_UN(pThread) := ThHCC_UN.CreateGetT(PRcd_HCC^);
        // ThHCC_UN(pThread).OnTerminate:=nil;
        // ThHCC_UN(pThread).FreeOnTerminate:=false;
        hHCC := ThHCC_UN(pThread).Handle;
        ThHCC_UN(pThread).Start;
        // ThHCC_UN(pThread).Resume;
        if WaitForSingleObject(hHCC, 65000) <> WAIT_TIMEOUT then
          begin
            ValT := ThHCC_UN(pThread).run_T;
            ThHCC_UN(pThread).Free;
            result := true;
          end
        else
          begin
            ThHCC_UN(pThread).Suspended := true;
            ThHCC_UN(pThread).FreeOnTerminate := true;
            ThHCC_UN(pThread).Free;
            // ThHCC_UN(pThread).Resume;
            // ThHCC_UN(pThread).Terminate;
            ShowMessage('Термокамера не отвечает.');
            ValT := -1000
          end;
      end
    else
      begin
        ShowMessage('Отсутствует связь с камерой тепла/холода.');
        ValT := -1000
      end;
  end;

{ ****************************************************************************** }

procedure TForm3.ScanPort(NameCom, NumberCom: TStrings);
  var
    i, err: integer;
    hPort: THANDLE;
  begin
    { \\.\ }
    for i := 0 to 999 do
      begin
        hPort := CreateFile(PChar('\\.\COM' + IntToStr(i)),
          GENERIC_READ or GENERIC_WRITE, 0, Nil, OPEN_EXISTING,
          FILE_ATTRIBUTE_NORMAL, 0);
        if (hPort <> INVALID_HANDLE_VALUE) then
          begin
            CloseHandle(hPort);
            NameCom.Add('COM' + IntToStr(i));
            NumberCom.Add(IntToStr(i));
          end
        else
          begin
            err := GetLastError();
            if err = ERROR_ACCESS_DENIED then
              begin
                NameCom.Add('COM' + IntToStr(i));
                NumberCom.Add(IntToStr(i));
              end;
          end;
      end;
  end;

procedure TForm3.seNTTChange(Sender: TObject);
  var
    i: integer;
  begin
    DrowAdvSg(AdvSg_TI, seNTT.IntValue + 1, 2 + 3 + 1, 1, 2);

    AdvSg_TI.MergeCells(0, 0, AdvSg_TI.ColCount, 1);
    AdvSg_TI.MergeCells(0, 3, AdvSg_TI.ColCount, 1);

    for i := 0 to seNTT.IntValue - 1 do
      AdvSg_TI.Cells[i + 1, 1] := 'T' + IntToStr(i + 1);

    CheckSetSizeSGTabAndScrollBar(AdvSg_TI, gbAuto.Height - AdvSg_TI.Top - 30,
      Panel1.Width - gbAuto.left - AdvSg_TI.left - 40);
    gbAuto.Width := AdvSg_TI.left + AdvSg_TI.Width + 20;
    if (Panel1.Width <= (gbAuto.Width + 20)) then
      gbAuto.Width := Panel1.Width - 20;

    // CreatRzNumericEdit(var ne:TRzNumericEdit;AOwner:TComponent;Parent:TWinControl;top,left,Width,Height:integer;IntegersOnly:boolean;DisplayFormat:string);
    // ODelay
  end;

procedure TForm3.seNTTChanging(Sender: TObject; NewValue: Extended;
  var AllowChange: boolean);
  begin
    AllowChange := (NewValue > 0) and (NewValue < 40);
  end;

procedure TForm3.bttFindClick(Sender: TObject);
  var
    ThFndHCC: ThHCC_UN;
  begin
    self.HCC.com := cbCPort.Items[cbCPort.ItemIndex];
    self.HCC.Add := IntToStr(self.neAdd.IntValue);
    self.HCC.spd := '19200';

    self.HCC.IRT5502.Add := self.neAdd.IntValue;
    self.HCC.IRT5502.spd := '19200';

    self.HCC.hPort := 0;

    ThFndHCC := ThHCC_UN.CreateFind(self.HCC);
    ThFndHCC.OnTerminate := OnTerminate;
    ThFndHCC.Start;
    bttFind.Enabled := false;
    sbInfo.Panels.Items[1].Text := 'Поиск термокамеры..';
  end;

procedure TForm3.OnTerminate(Sender: TObject);
  begin
    bttFind.Enabled := true;
    self.HCC.Connect := ThHCC_UN(Sender).Connect;
    if (ThHCC_UN(Sender).Connect = true) then
      sbInfo.Panels.Items[1].Text := 'Термокамера найдена'
    else
      sbInfo.Panels.Items[1].Text := 'Нет связи';
  end;

procedure TForm3.Panel1CanResize(Sender: TObject;
  var NewWidth, NewHeight: integer; var Resize: boolean);
  begin
    if (NewWidth < 460) then
      NewWidth := 460;
  end;

procedure TForm3.Panel1Resize(Sender: TObject);
  begin
    seNTTChange(Sender);
  end;

procedure TForm3.Button1Click(Sender: TObject);
  var
    Cmd_HCC: TCmd_HCC;
  begin
    if (HCC.Connect = false) then
      begin
        sbInfo.Panels.Items[0].Text := '';
        sbInfo.Panels.Items[1].Text := 'Нет связи с термокамерой';
      end
    else
      begin
        Cmd_HCC := TCmd_HCC.Create(HCC);
        Cmd_HCC.Connect := HCC.Connect;
        Cmd_HCC.PwrOnClick(self.neSetT.Value);
        Cmd_HCC.Free;
        sbInfo.Panels.Items[0].Text := '';
        sbInfo.Panels.Items[1].Text := 'камера включена';
      end;

  end;

procedure TForm3.Button2Click(Sender: TObject);
  var
    Cmd_HCC: TCmd_HCC;
  begin
    if (HCC.Connect = false) then
      begin
        sbInfo.Panels.Items[0].Text := '';
        sbInfo.Panels.Items[1].Text := 'Нет связи с термокамерой';
      end
    else
      begin
        Cmd_HCC := TCmd_HCC.Create(HCC);
        Cmd_HCC.Connect := HCC.Connect;
        Cmd_HCC.PwrOffClick();
        Cmd_HCC.Free;

        sbInfo.Panels.Items[0].Text := '';
        sbInfo.Panels.Items[1].Text := 'камера остановлена';
      end;
  end;

procedure TForm3.Button3Click(Sender: TObject);
  var
    Cmd_HCC: TCmd_HCC;
    ValT: single;
  begin
    if (HCC.Connect = false) then
      begin
        sbInfo.Panels.Items[0].Text := '';
        sbInfo.Panels.Items[1].Text := 'Нет связи с термокамерой';
      end
    else
      begin
        Cmd_HCC := TCmd_HCC.Create(HCC);
        Cmd_HCC.Connect := self.HCC.Connect;
        if Cmd_HCC.GetT(ValT) then
          begin
            Cmd_HCC.Free;
            neValManual.Value := ValT;
          end
        else
          begin
            Cmd_HCC.Free;
            exit;
          end;

        sbInfo.Panels.Items[0].Text := '';
        sbInfo.Panels.Items[1].Text := 'чтение температуры';
      end;
  end;

procedure TForm3.cbTermoCloseUp(Sender: TObject);
  begin
    self.HCC.typ := HCC_TYPE.T_ABSENT;
    self.lAdd.Visible := false;
    self.neAdd.Visible := false;

    if (cbTermo.ItemIndex = 0) then
      self.HCC.typ := HCC_TYPE.T_T6800;
    if (cbTermo.ItemIndex = 1) then
      begin
        self.HCC.typ := HCC_TYPE.T_IRT5502;
        self.lAdd.Visible := true;
        self.neAdd.Visible := true;
      end;

  end;

procedure TForm3.FormClose(Sender: TObject; var Action: TCloseAction);
  begin
    LoadPoint();
    Save();
  end;

procedure TForm3.FormCreate(Sender: TObject);
  begin
    CreatAndDrowAdvSg(AdvSg_TI, self, self.gbAuto, 3, 6, 1, 2, seNTT.Top + 30,
      Label3.left);
    AdvSg_TI.OnGetAlignment := OnGetAlignmentAdvSG;
    AdvSg_TI.OnGetEditorType := OnGetEditorTypeAdvSg;
    AdvSg_TI.OnDblClickCell := OnlClickCellAdvSG;
    AdvSg_TI.OnClickCell := OnlClickCellAdvSG;
    AdvSg_TI.OnKeyPress := OnKeyPress;
    // AdvSg_TI.OnCellValidate   := OnCellValidate;
    // AdvSg_TI.OnEditCellDone   := OnEditCellDone;
    AdvSg_TI.DefaultColWidth := 80;
    AdvSg_TI.ColWidths[0] := 120;
    AdvSg_TI.Cells[0, 0] := 'Температура';
    AdvSg_TI.Cells[0, 2] := 'T[C°]';
    AdvSg_TI.Cells[0, 3] := 'Время';
    AdvSg_TI.Cells[0, 4] := 'Задержка  ';
    AdvSg_TI.Cells[0, 5] := 'Измерение ';

    sbInfo.Panels.Add;
    sbInfo.Panels.Add;
    sbInfo.Panels[0].Width := 100;

    lstPortNumber := TStringList.Create;
    lstPortName := TStringList.Create;
    lstPortName.Clear;
    lstPortNumber.Clear;
    ScanPort(lstPortName, lstPortNumber);
    cbCPort.Items := lstPortName;

    Load();
    SavePoint();
    OEvt_Break := TEvent.Create(false);
    self.HCC.mtxCom := CreateMutex(nil, false, nil);
    Chart.Series[0].Clear;
    Chart.Series[1].Clear;
    Chart.Series[0].Title := 'T[C°]';
    Chart.Series[1].Title := 'Метки';
  end;

procedure TForm3.FormShow(Sender: TObject);
  begin
    cbTermoCloseUp(Sender);
    bttStopAuto.Enabled := not bttStartAuto.Enabled;
    seNTTChange(Sender);
  end;

procedure TForm3.LoadPoint();
  var
    vH, vM, i: integer;
    // sH,sM:string;
  begin
    SetLength(Prj.points, seNTT.IntValue);
    for i := 0 to seNTT.IntValue - 1 do
      begin

        TryStrToFloat(self.AdvSg_TI.Cells[1 + i, 2], Prj.points[i].temp);

        TimeToSring(vH, vM, AdvSg_TI.Cells[1 + i, 4]);
        Prj.points[i].time := vH * 60 + vM;

        TimeToSring(vH, vM, AdvSg_TI.Cells[1 + i, 5]);
        Prj.points[i].delay := vH * 60 + vM;
        // TryStrToInt(self.AdvSg_TI.Cells[1+i,4],Prj.points[i].time);
        // TryStrToInt(self.AdvSg_TI.Cells[1+i,5],Prj.points[i].delay);
      end;
  end;

procedure TForm3.SavePoint();
  var
    i: integer;
    vH, vM: integer;
    // sH,sM:string;
  begin
    seNTT.IntValue := Length(Prj.points);

    for i := 0 to seNTT.IntValue - 1 do
      begin
        self.AdvSg_TI.Cells[1 + i, 2] := FloatToStrF(Prj.points[i].temp,
          ffGeneral, 5, 2);

        vH := Prj.points[i].time div 60;
        vM := Prj.points[i].time - (vH * 60);
        self.AdvSg_TI.Cells[1 + i, 4] := IntToStr(vH) + 'ч. ' + IntToStr(vM)
            + 'м.';

        vH := Prj.points[i].delay div 60;
        vM := Prj.points[i].delay - (vH * 60);
        self.AdvSg_TI.Cells[1 + i, 5] := IntToStr(vH) + 'ч. ' + IntToStr(vM)
            + 'м.';
        // IntToStr(FTime.neHour.IntValue)+'ч. '+IntToStr(FTime.neMinuts.IntValue)+'м.';

        // self.AdvSg_TI.Cells[1+i,4]:=IntToStr(Prj.points[i].time);
        // self.AdvSg_TI.Cells[1+i,5]:=IntToStr(Prj.points[i].delay);
      end;
  end;

procedure TForm3.OnGetEditorTypeAdvSg(Sender: TObject; ACol, ARow: integer;
  var AEditor: TEditorType);
  begin
    case ARow of
      2:
        begin
          AEditor := edFloat;
        end;
      else
        AEditor := edNone; // edPositiveNumeric;
      end;
  end;

procedure TForm3.TimeToSring(var valHour: integer; var valMinuts: integer;
  sTime: string);
  var
    // s:string;
    sH, sM: string;
    idx0, idx1: integer;
  begin
    idx0 := 1;
    idx1 := pos('ч. ', sTime);
    sH := copy(sTime, idx0, idx1 - idx0);
    TryStrToInt(sH, valHour);

    idx0 := pos('ч. ', sTime);
    idx1 := pos('м.', sTime);
    sM := copy(sTime, idx0 + 3, idx1 - idx0 - 3);
    TryStrToInt(sM, valMinuts);
  end;

procedure TForm3.OnlClickCellAdvSG(Sender: TObject; ARow, ACol: integer);
  const
    Captions: array [0 .. 1] of string = ('Установить время задержки',
      'Установить время измерения');
  var
    Rect: TRect;
    H, M: integer;
  begin
    if ((ARow = 4) or (ARow = 5)) and (ACol > 0) then
      begin
        Rect := AdvSg_TI.CellRect(ACol, ARow);
        FTime.Top := Rect.Top + AdvSg_TI.Top + gbAuto.Top + self.Top;
        FTime.left := Rect.left + AdvSg_TI.left + gbAuto.left + self.left;
        FTime.Caption := Captions[ARow - 4];
        TimeToSring(H, M, AdvSg_TI.Cells[ACol, ARow]);
        FTime.neHour.IntValue := H;
        FTime.neMinuts.IntValue := M;
        if (FTime.ShowModal = mrYes) then
          begin
            AdvSg_TI.Cells[ACol, ARow] := IntToStr(FTime.neHour.IntValue)
                + 'ч. ' + IntToStr(FTime.neMinuts.IntValue) + 'м.';
          end;
      end;
  end;

// procedure TForm3.OnGetEditorTypeAdvSg(Sender: TObject; ACol, ARow: integer;
// var AEditor: TEditorType);
// var
// i:integer;
// begin
// case ARow of
// 2:begin
//
// end;
// end;
// end;
//

procedure TForm3.OnGetAlignmentAdvSG(Sender: TObject; ARow, ACol: integer;
  var HAlign: TAlignment; var VAlign: TAsgVAlignment);
  begin
    HAlign := taCenter;
    VAlign := vtaCenter;
  end;

procedure TForm3.OnKeyPress(Sender: TObject; var Key: Char);
  begin
    if (Key = '.') then
      Key := ',';
  end;


// procedure TForm3.OnCellValidate(Sender: TObject; ACol, ARow: Integer;
// var Value: String; var Valid: Boolean);
// begin
/// /if(ACol=1)and(ARow=2)then
/// /  begin
/// /  Valid:=true;
/// /  end;
// end;
//
// procedure TForm3.OnEditCellDone(Sender: TObject; ACol, ARow: Integer);
// begin
// if(ARow=2)then
// begin
// AdvSg_TI.FocusCell(ACol+1,ARow+1);
/// /  AdvSg_TI.ActiveCellShow(false);
/// /AdvSg_TI.Cells[ACol,ARow]:='2';
/// /AdvSg_TI.;
// end;
// end;

procedure TForm3.Load();
  var
    fFN: TMemIniFile;
    s: string;
    len, i: integer;
  begin
    fFN := TMemIniFile.Create(ExtractFilePath(Application.ExeName)
          + 'config.txt');
    s := fFN.ReadString('main', 'cport', 'COM1');
    cbCPort.ItemIndex := cbCPort.Items.IndexOf(s);
    cbTermo.ItemIndex := fFN.ReadInteger('main', 'cbTermo', 0);
    cbTermo.ItemIndex := fFN.ReadInteger('main', 'cbTermo', 0);
    neAdd.IntValue := fFN.ReadInteger('main', 'neAdd', 0);

    len := fFN.ReadInteger('main', 'NPnt', 2);
    SetLength(Prj.points, len);

    for i := 0 to len - 1 do
      begin
        Prj.points[i].temp := fFN.ReadFloat('main', 'Ptemp' + IntToStr(i), 20);
        Prj.points[i].time := fFN.ReadInteger('main', 'Ptime' + IntToStr(i),
          30);
        Prj.points[i].delay := fFN.ReadInteger('main', 'Pdelay' + IntToStr(i),
          15);
      end;

    fFN.Free;
  end;

procedure TForm3.Save();
  var
    fFN: TMemIniFile;
    len, i: integer;
  begin
    fFN := TMemIniFile.Create(ExtractFilePath(Application.ExeName)
          + 'config.txt');
    fFN.WriteString('main', 'cport', cbCPort.Items[cbCPort.ItemIndex]);
    fFN.WriteInteger('main', 'cbTermo', cbTermo.ItemIndex);
    fFN.WriteInteger('main', 'neAdd', neAdd.IntValue);

    len := Length(Prj.points);
    fFN.WriteInteger('main', 'NPnt', len);

    for i := 0 to len - 1 do
      begin
        fFN.WriteFloat('main', 'Ptemp' + IntToStr(i), Prj.points[i].temp);
        fFN.WriteInteger('main', 'Ptime' + IntToStr(i), Prj.points[i].time);
        fFN.WriteInteger('main', 'Pdelay' + IntToStr(i), Prj.points[i].delay);
      end;

    fFN.UpdateFile;
    fFN.Free;
  end;

procedure TForm3.bttStartAutoClick(Sender: TObject);
  var
    Cmd_HCC: TCmd_HCC;
    OWait: TOWait_Quit;
    // rc:TMyWaitResult;
    i: integer;
  begin

    if (HCC.Connect = false) then
      begin
        sbInfo.Panels.Items[0].Text := '';
        sbInfo.Panels.Items[1].Text := 'Нет связи с термокамерой';
      end;

    delta := 0;
    OEvt_Break.ResetEvent();
    OWait := TOWait_Quit.Create;
    bttStartAuto.Enabled := false;
    bttStopAuto.Enabled := not bttStartAuto.Enabled;
    LoadPoint();

    Chart.Series[0].Clear;
    Chart.Series[1].Clear;
    StartTime := Now;

    for i := 0 to Length(Prj.points) - 1 do
      begin
        CPoint := i;
        Stage := 0;
        // CurrPoint:=prj.points[CPoint];
        // включаем термокамеру на определённую температуру
        Cmd_HCC := TCmd_HCC.Create(HCC);
        Cmd_HCC.Connect := HCC.Connect;
        Cmd_HCC.PwrOnClick(Prj.points[CPoint].temp);
        Cmd_HCC.Free;
        // включаем опрос термокамеры
        OThHCC := ThHCC_UN.CreateMthr(HCC, HCCMeasuring);
        OThHCC.Start;
        OWait.WaitForThTerminate(INFINITE, OThHCC.OEvt);

        if (OEvt_Break.WaitFor(0) = wrSignaled) then
          break;
      end;

    Cmd_HCC := TCmd_HCC.Create(HCC);
    Cmd_HCC.Connect := HCC.Connect;
    Cmd_HCC.PwrOffClick();
    Cmd_HCC.Free;

    sbInfo.Panels.Items[0].Text := '';
    sbInfo.Panels.Items[1].Text := 'Камера остановлена';

    ShowMessage('Термокамера отключена!');

    OWait.Free;
    bttStartAuto.Enabled := true;
    bttStopAuto.Enabled := not bttStartAuto.Enabled;
  end;

procedure TForm3.HCCMeasuring(val: single);
  begin
    // Chart.Series[0].AddXY((now - StartTime) * 1440, Val);
    Chart.Series[0].AddXY(Now, val);
    sbInfo.Panels.Items[0].Text := 'Точка Т' + IntToStr(CPoint + 1)
        + '=' + FloatToStrF(Prj.points[CPoint].temp, ffFixed, 4, 2);

    case Stage of
      0:
        begin // ожидаем приближение к точке по температуре
          sbInfo.Panels.Items[1].Text :=
              'Ожидаем приближение к точке по температуре: ' + FloatToStrF
              (abs(Prj.points[CPoint].temp - val), ffFixed, 4, 2) + '>=1';
          // inc(delta);//для проверки
          if (abs(Prj.points[CPoint].temp - val) < 1) then
            // if(delta>10)then//для проверки
            begin
              Chart.Series[1].AddXY(Now, val, '', clRed);
              inc(Stage);
              StopTime := ((Now - StartTime) * 1440) + Prj.points[CPoint].time;
              //
            end;
        end;
      1:
        begin // ожидаем окончание таймера Задержки
          sbInfo.Panels.Items[1].Text :=
              'До истечении таймера "Задержки" осталось: ' + FloatToStrF
              (StopTime - ((Now - StartTime) * 1440), ffFixed, 4, 1);
          if (StopTime < ((Now - StartTime) * 1440)) then
            begin
              Chart.Series[1].AddXY(Now, val, '', clYellow);
              inc(Stage);
              StopTime := ((Now - StartTime) * 1440) + Prj.points[CPoint].delay;
              // time;
            end;

        end;
      2:
        begin // ожидаем окончание таймера Измерения
          sbInfo.Panels.Items[1].Text :=
              'До истечении таймера "Измерения" осталось: ' + FloatToStrF
              (StopTime - ((Now - StartTime) * 1440), ffFixed, 4, 1);
          if (StopTime < ((Now - StartTime) * 1440)) then
            begin
              Chart.Series[1].AddXY(Now, val, '', clOlive);
              OThHCC.Terminate;
            end;
        end;
    end;

  end;

procedure TForm3.bttStopAutoClick(Sender: TObject);
  begin
    OEvt_Break.SetEvent;
    if self.OThHCC <> nil then
      TThread(self.OThHCC).Terminate;

  end;

end.
