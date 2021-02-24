unit Common;

interface

uses Classes, Dialogs, Windows, Series, Chart, Grids, TeEngine,
  BaseGrid, AdvGrid, AdvObj, IniFiles, Controls, StdCtrls, RzEdit, RzPanel,
  RzButton,
  RzStatus, RzPrgres, Graphics, ComObj, Forms, SysUtils, ExcelXP, RzSpnEdt,
  Math,
  ExtCtrls, Menus, Messages, Character, SyncObjs, RzLstBox,
  RzChkLst, Types;

type

  TAEvent = array of TEvent;
  ArrayOfByte = array of byte;
  TOnResetFin = procedure(Sender: TObject) of object;

  TOAEvent = class(TObject)
    AEvent: TAEvent;
    function SignaledAllEvent(): boolean;
    procedure AddAEvent(var Event: TEvent);
    constructor Create(); overload;
    destructor Destroy(); override;
  end;

  TTh = class(TThread) // это некоторый поток
    procedure Execute; override;
    public
      OnExecute: procedure() of Object;
      OnBeforeStart: procedure() of Object;
      OnAfterStop: procedure() of Object;

  end;

  TObjPrototip = class(TObject)
    private
      Th: TTh;
      { Private declarations }
    public
      { Public declarations }
      OnPrototip: procedure() of Object;
      OnBeforeStart: procedure() of Object;
      OnAfterStop: procedure() of Object;

      procedure Action();
  end;


  // TChartSeriesOperate= class (TObject)
  // private
  // procedure AddValSeriesAVal(var LS:TLineSeries;var AVal:rAVal);
  // procedure AddValSeriesXY(var LS:TLineSeries;var ValX:TArrayDateTime;var ValY:TArrayDouble);
  // public
  // onGetTypPrintVal:procedure(var PV_RV:TPV_RV)of Object;
  // onGetValCalc: procedure (TMV:TMeasuringVal;ch:integer;var valCalc:TCalc )of Object;
  // onGetValLS:   procedure (TMV:TMeasuringVal;ch:integer;var LS:TLineSeries)of Object;
  // procedure AddValSeriesTCalc(ch:integer;TMV:TMeasuringVal);
  // end;

  TMyAdvStringGrid = class(TAdvStringGrid)
    private
      FOnMouseWheelDown: TMouseWheelUpDownEvent;
      FOnMouseWheelUp: TMouseWheelUpDownEvent;
      // FOnMouseWheel:  TMouseWheelEvent;
    protected
      function DoMouseWheelDown(Shift: TShiftState; MousePos: TPoint): boolean;
          override;
      function DoMouseWheelUp(Shift: TShiftState; MousePos: TPoint): boolean;
          override;
      // function DoMouseWheel     (Sender: TObject; Shift: TShiftState;WheelDelta: Integer; MousePos: TPoint; var Handled: Boolean): Boolean;overload;
      // function DoMouseWheel     (                 Shift: TShiftState; WheelDelta: Integer; MousePos: TPoint): Boolean; override;
    public
    published

      property OnMouseWheelDown
          : TMouseWheelUpDownEvent read FOnMouseWheelDown
          write FOnMouseWheelDown;
      property OnMouseWheelUP
          : TMouseWheelUpDownEvent read FOnMouseWheelUp
          write FOnMouseWheelUp;
      // property OnMouseWheel:      TMouseWheelEvent       read FOnMouseWheel     write FOnMouseWheel;
      // constructor Create(AOwner:TComponent);overload;

  end;

  TMyWaitResult = (enMWR_Rest, enMWR_SetTimout, enMWR_GetMsgExit,
    enMWR_SetEvetObj, enMWR_SetEvetBrk, enMWR_SetEvetRpt, enMWR_SetEvetPse);

  TImgBtt = (en_ImgBtt_Exit, en_ImgBtt_Pause, en_ImgBtt_Resume,
    en_ImgBtt_Conti, en_ImgBtt_Start, en_ImgBtt_Repeat);

  TBttControlState = (en_BCS_WaitStartNew, en_BCS_WaitStartContinius,
    en_BCS_Operate, en_BCS_OperateRpt, en_BCS_WaitOperateResume);

  TOnClickBtt = procedure(var f: boolean) of object;

  TBttControl = class(TObject)
    AOwner: TWinControl;
    OnEvtStart, OnEvtRpt, OnEvtExit, OnEvtPause, OnEvtResume,
        OnEvtContinue: TOnClickBtt;
    MS: TBttControlState;
    OEvtRpt, OEvtPause, OEvtBreak: TEvent;
    bbExit, bbPause, bbStart, bbRpt: TRzBitBtn;
    flgBttRpt: boolean;
    // flgBttPse:boolean;
    OIL: TImageList;
    procedure ClickbbStart(Sender: TObject);
    procedure ClickbbPause(Sender: TObject);
    procedure ClickbbRpt(Sender: TObject);
    procedure ClickbbExit(Sender: TObject);

    function getMS(): TBttControlState;
    procedure SiuationNew();
    procedure SiuationContinue();
    constructor Create(var OIL: TImageList; AOwner: TWinControl); overload;
    destructor Destroy(); Override;
  end;

  TOWait_Quit = class(TObject)
    idTimer: UINT;
    procedure SetDelay(mls: DWORD);
    function ProcessMessages(): TMyWaitResult;
    function WaitFor(mls: DWORD): TMyWaitResult; overload;
    function WaitForObjectAndPause(mls: DWORD; var Event_Brk, Event_Pse,
      Event_Obj: TEvent): TMyWaitResult;
    function WaitForObject(mls: DWORD; var Event_Brk, Event_Obj: TEvent)
        : TMyWaitResult;
    function WaitForBreak(mls: DWORD; var Event_Brk: TEvent): TMyWaitResult;
    function WaitForContinue(mls: DWORD; var Event_Brk, Event_Pse: TEvent)
        : TMyWaitResult;
    function WaitFor(mls: DWORD; var Event_Brk, Event_Pse: TEvent)
        : TMyWaitResult; overload;
    function WaitForRepeat(mls: DWORD; var Event_Brk, Event_Pse,
      Event_Rpt: TEvent): TMyWaitResult;
    function WaitForMulipleObject(mls: DWORD; var Event_Brk: TEvent;
      var OAEvent: TOAEvent): TMyWaitResult;
    function WaitForMulipleAndPause(mls: DWORD;
      var Event_Brk, Event_Pse: TEvent; var OAEvent: TOAEvent)
        : TMyWaitResult;

    function WaitForThTerminate(mls: DWORD; var Event_Obj: TEvent)
        : TMyWaitResult;
    constructor Create(); overload;
    destructor Destroy(); Override;
  end;

  TCellStatus = (en_cs_rw, en_cs_or);

  // TAdvSGLdData=class(TAdvStringGrid)
  // public
  // onLoadData:procedure(Obj:TCalc_V0)of object; //по умолчанию пытается вызыватся
  // procedure LoadData(Obj:TCalc_V0);virtual;
  // end;


  // TAdvSG=class(TAdvSGLdData)
  // private
  // nCh:integer;
  // public
  // OnGetChConnect:function (ch:integer):boolean of object;
  // OnGetChEnable:function (ch:integer):boolean of object;
  // OnSetChState:procedure(ch:integer;state:boolean) of object;
  //
  // function NParamToCol(NParam:integer):integer;
  // function ChToRow(ch:integer):integer;
  // function RowToCh(row:integer):integer;
  // procedure SetReadOnlyStatus(ch:integer;cs:TCellStatus);
  // procedure GetAlignmentAdvSG(Sender: TObject; ARow, ACol: integer;
  // var HAlign: TAlignment; var VAlign: TAsgVAlignment);
  // procedure GetEditorTypeAdvSG(Sender:TObject;ACol,ARow: Integer;var AEditor:TEditorType);
  // procedure SetCBState();
  //
  // procedure ClickCellAdvSG(Sender: TObject; ARow, ACol: integer);
  // procedure CheckDecimalSeparatorOnKeyPress(Sender: TObject; var Key: Char);
  //
  // constructor Create_V0(AOwner:TComponent;Parent:TWinControl;CCnt,RCnt,FCCnt,FRCnt,nCh,top,left:integer);
  // destructor Destroy(); override;
  // end;
  //
  //
  // TOAdvStringGrid_M0= class(TAdvStringGrid)
  // constructor Create_V0(AOwner:TComponent;Parent:TWinControl;CCnt,RCnt,FCCnt,FRCnt,top,left:integer);
  // destructor Destroy(); override;
  // private
  // procedure GetAlignment(Sender: TObject; ARow, ACol: Integer; var HAlign: TAlignment;var VAlign: TAsgVAlignment);
  // procedure _KeyPress(Sender: TObject; var Key: Char);
  // end;
  //
  // TCustomSeriesMy=class(TCustomSeries)
  // procedure SetProperty(sps:TSeriesPointerStyle);
  // constructor CreateLineSeries(Owner:TComponent;sps:TSeriesPointerStyle);overload;
  // constructor CreatePointSeries(Owner:TComponent;sps:TSeriesPointerStyle);overload;
  /// /      constructor Create(TypeSeries:integer;Owner:TComponent;sps:TSeriesPointerStyle);overload;
  // destructor Destroy(); override;
  // end;

  // TLineSeriesMy=class(TLineSeries)
  // constructor Create(Owner:TComponent;sps:TSeriesPointerStyle);overload;
  // destructor Destroy(); override;
  // end;
  //
  // TPointSeriesMy=class(TPointSeries)
  // constructor Create(Owner:TComponent;sps:TSeriesPointerStyle);overload;
  // destructor Destroy(); override;
  // end;

procedure CreatDlgSave(var Dlg: TSaveDialog; Owner: TComponent;
  Title, Filter, InitDir: string);
procedure CreatDlgOpen(var Dlg: TOpenDialog; Owner: TComponent;
  Title, Filter, InitDir: string);
// procedure CreateSeriasList(var cs:TCustomSeries;TypeSeries:integer;Owner:TComponent;sps:TSeriesPointerStyle);
procedure CreatLabel(var Lbl: TLabel; AOwner: TComponent; Parent: TWinControl;
  Caption: string; top, left, Size: integer; Style: TFontStyles);
procedure CreatRzNumericEdit(var ne: TRzNumericEdit; AOwner: TComponent;
  Parent: TWinControl; top, left, Width, Height: integer;
  IntegersOnly: boolean; DisplayFormat: string);
procedure CreatRzSpinEdit(var se: TRzSpinEdit; AOwner: TComponent;
  Parent: TWinControl; top, left, Width, Height, min, max: integer;
  IntegersOnly: boolean);

procedure DrowAdvSg(var AdvSg: TAdvStringGrid;
  CCnt, RCnt, FCCnt, FRCnt: integer);
procedure CreatAdvSg(var AdvSg: TAdvStringGrid; AOwner: TComponent;
  Parent: TWinControl; top, left: integer);
procedure CreatMyAdvSg(var AdvSg: TMyAdvStringGrid; AOwner: TComponent;
  Parent: TWinControl; top, left: integer);
procedure CreatAndDrowMyAdvSg(var AdvSg: TMyAdvStringGrid; AOwner: TComponent;
  Parent: TWinControl; CCnt, RCnt, FCCnt, FRCnt, top, left: integer);
procedure CreatAndDrowAdvSg(var AdvSg: TAdvStringGrid; AOwner: TComponent;
  Parent: TWinControl; CCnt, RCnt, FCCnt, FRCnt, top, left: integer);
procedure OnGetAlignmentAdvSG(Sender: TObject; ARow, ACol: integer;
  var HAlign: TAlignment; var VAlign: TAsgVAlignment);
// procedure CreatGroupBox(var GB:TGroupBox;AOwner:TComponent; Parent:TWinControl;Caption:string;top,left,H,W:integer);
procedure CreatBitButton(var rzBB: TRzBitBtn; AOwner: TComponent;
  Parent: TWinControl; Caption: string; top, left, H, W: integer);
// procedure CreatRzEdit(var rzE:TRzEdit;AOwner:TComponent; Parent:TWinControl;Text:string;top,left,H,W:integer);
// procedure CreatEdit(var Edit:TEdit;AOwner:TComponent; Parent:TWinControl;Text:string;top,left,H,W:integer);
procedure CreatListBox(var ListBox: TListBox; AOwner: TComponent;
  Parent: TWinControl; top, left, H, W: integer; var PUp: TPopUpMenu);
procedure CreatComboBox(var CBox: TComboBox; AOwner: TComponent;
  Parent: TWinControl; top, left, H, W: integer; stl: TComboBoxStyle);

procedure CreatCheckList(var CheckList: TRzCheckList; AOwner: TComponent;
  Parent: TWinControl; top, left, H, W: integer);

function MyStrToFloat(s: String; var v: Double): boolean;
procedure CalcSizeSGTab(var Tab: TAdvStringGrid; var H, W: integer);

procedure SetSizeSGTab(var Tab: TAdvStringGrid);
procedure CheckSetSizeSGTabAndScrollBar(Tab: TAdvStringGrid;
  MaxH, MaxW: integer);

procedure CheckDecimalSeparatorOnKeyPress(Sender: TObject; var Key: AnsiChar);
function PrintSN(SN: integer): string;
function PrintZN(ZN: integer): string;
// function MessageSetT(str:string):integer;
function MessageDlgM(str: string; Size: integer; DlgType: TMsgDlgType;
  Buttons: TMsgDlgButtons): integer;
function StrInternetToDateTime(Value: string; var DTVal: TDateTime): integer;
Procedure Sound(Duration: integer; Freq: Word);
procedure PrintAdvSG(var AdvSg: TAdvStringGrid; ACol, ARow: integer; s: string;
  Color: TColor);
function E9FileStatus(const Origin: string): boolean;
function AddLog(FP: TFileName; N: string; str: string): boolean;
function adler32(pVal: PByte; buflength: integer): DWORD;
function AnsiCharToChar(var AChar: AnsiChar): Char;

function AnsiToStr(var AStr: AnsiString): string;
function CharToAnsiChar(var Chr: Char): AnsiChar;

// procedure AnsiToStr(var AStr:AnsiString;var Str:string);overload;
function StrToAnsi(var str: string): AnsiString;
function IsRussianLetter(ACh: Char): boolean;
function IsRussianAnsiLetter(ACh: AnsiChar): boolean;

// procedure ConvFromDOS(var Source: AnsiString);

// end;
implementation

{ ****************************************************************************** }
procedure TTh.Execute;
  // var
  // i:integer;
  begin
    FreeOnTerminate := true;
    if Assigned(OnBeforeStart) then
      Synchronize(OnBeforeStart);
    OnExecute();
    if Assigned(OnAfterStop) then
      Synchronize(OnAfterStop);
  end;

procedure TObjPrototip.Action();
  begin
    Th := TTh.Create(true);

    if Assigned(self.OnAfterStop) then
      Th.OnAfterStop := OnAfterStop;
    if Assigned(self.OnBeforeStart) then
      Th.OnBeforeStart := OnBeforeStart;
    if Assigned(self.OnPrototip) then
      Th.OnExecute := OnPrototip;
    Th.Start;
    // Th.Resume;
  end;

{ ****************************************************************************** }
// procedure TAdvSGLdData.LoadData(Obj:TCalc_V0);
// begin{}
// if Assigned(OnLoadData) then
// OnLoadData(Obj);
// end;

// function TAdvSG.NParamToCol(NParam:integer):integer;
// begin
// result:=self.FixedCols+NParam;
// end;
//
// function TAdvSG.ChToRow(ch:integer):integer;
// begin
// result:=(self.RowCount-self.nCh)+ch;
// end;
//
// function TAdvSG.RowToCh(row:integer):integer;
// begin
// result:=row-(self.RowCount-self.nCh);
// end;
//
// procedure TAdvSG.SetReadOnlyStatus(ch:integer;cs:TCellStatus);
// var cellProperty: TCellProperties;
// i,NCols:integer;
// begin
// cellProperty := self.GetCellProperties(0, self.RowCount-self.nCh+ch);
// cellProperty.ReadOnly :=cs=en_cs_or;
// NCols:=self.ColCount-self.FixedCols;
// if cs=en_cs_or then
// begin
// for i:=0 to NCols-1 do
// begin
// self.Cells[self.FixedCols+i,self.RowCount-self.nCh+ch]:='';
// self.CellProperties[self.FixedCols+i,self.RowCount-self.nCh+ch].BrushColor:=clWhite;
// end
// end;
// end;
//
// procedure TAdvSG.GetAlignmentAdvSG(Sender: TObject; ARow, ACol: integer;
// var HAlign: TAlignment; var VAlign: TAsgVAlignment);
// begin
// HAlign := taCenter;
// VAlign := vtaCenter;
// end;
//
// procedure TAdvSG.GetEditorTypeAdvSG(Sender:TObject;ACol,ARow: Integer;var AEditor:TEditorType);
// begin
// AEditor:=edNone;
// end;
//
// procedure TAdvSG.SetCBState();
// var
// ch: integer;
// statCn,statEn:boolean;
// begin
// if (not Assigned(OnGetChConnect))or (not Assigned(OnGetChEnable)) then exit;
//
// for ch:=0 to nCh-1 do
// begin
// statCn:=OnGetChConnect(ch);
// statEn:=self.OnGetChEnable(ch);
// if statCn then
// SetReadOnlyStatus(ch,TCellStatus.en_cs_rw)
// else
// SetReadOnlyStatus(ch,TCellStatus.en_cs_or);
//
// self.SetCheckBoxState(0, self.RowCount-self.nCh+ch,statEn);
// end;
// end;

{ procedure TAdvSG.SetColReadProperty(Ch: integer;f: boolean);
  var
  ARow: integer;
  cellProperty: TCellProperties;
  begin
  ARow:=self.ChToRow(ch);
  cellProperty := self.GetCellProperties(0,ARow);
  cellProperty.ReadOnly := f;
  end;
}
// procedure TAdvSG.ClickCellAdvSG(Sender: TObject; ARow, ACol: integer);
// var
// ch: integer;
// State: boolean;
// begin
// if not Assigned(OnSetChState)then exit;
//
//
// if self.GetCheckBoxState(ACol, ARow, State) then
// begin
// state:=not state;
// if self.CellProperties[0, ARow].ReadOnly then
// State := false;
// self.SetCheckBoxState(0, ARow, State);
// ch:=self.RowToCh(ARow);
// OnSetChState(ch,state);
// end;
// end;
//
// procedure TAdvSG.CheckDecimalSeparatorOnKeyPress(Sender: TObject;var Key: Char);
// begin
// if Key = ',' then Key := '.';
// end;
//
// constructor TAdvSG.Create_V0(AOwner:TComponent;Parent:TWinControl;CCnt,RCnt,FCCnt,FRCnt,nCh,top,left:integer);
// var ch:integer;
// begin
// inherited Create(AOwner);
// self.nCh:=nCh;
// self.Parent:=Parent;
// self.ParentFont:=true;
// self.Top:=top;
// self.Left:=left;
// self.Options:=self.Options+[goEditing];
// self.Navigation.AlwaysEdit:=true;
// self.Navigation.AdvanceAuto:=true;
// self.MouseActions.DirectEdit:=true;
// self.MouseActions.DirectComboDrop:=true;
// self.MouseActions.DirectDateDrop:=true;
// self.ShowSelection:=false;
// self.RowCount:=RCnt;
// self.FixedRows:=FRCnt;
// self.ColCount:=CCnt;
// self.FixedCols:=FCCnt;
// self.ColWidths[0]:=20;
// self.ColWidths[1]:=20;
// if(self.ColCount>2)then
// self.ColWidths[2]:=90;
//
// for ch := 0 to nCh-1 do
// self.Cells[1,RCnt-nCh+ch] := intToStr(ch + 1);
//
// for ch := 0 to nCh-1 do
// begin
// self.AddCheckBox(0,RCnt-nCh+ch, false, false);
// end;
//
// for ch := 0 to nCh-1 do
// self.SetReadOnlyStatus(ch,TCellStatus.en_cs_or);
//
// self.Top:=Top;
// self.Left:=Left;
// self.OnGetAlignment :=GetAlignmentAdvSG;
// self.OnKeyPress     :=CheckDecimalSeparatorOnKeyPress;
// self.OnClickCell    :=ClickCellAdvSG;
// self.OnGetEditorType:=GetEditorTypeAdvSG;
// end;
//
// destructor TAdvSG.Destroy();
// begin
// inherited Destroy();
// end;

{ ****************************************************************************** }

constructor TBttControl.Create(var OIL: TImageList; AOwner: TWinControl);
  begin { }
    flgBttRpt := false;
    // flgBttPse:=true;
    self.OIL := OIL;
    self.AOwner := AOwner;
    OEvtPause := TEvent.Create(false);
    OEvtBreak := TEvent.Create(false);
    OEvtRpt := TEvent.Create(false);
    CreatBitButton(self.bbStart, self.AOwner, self.AOwner, 'Старт', 2, 130,
      self.AOwner.Height - 4, 130);
    CreatBitButton(self.bbRpt, self.AOwner, self.AOwner, 'Повтор', 2,
      bbStart.left + bbStart.Width + 20, self.AOwner.Height - 4, 100);
    CreatBitButton(self.bbPause, self.AOwner, self.AOwner, 'Пауза', 2,
      bbRpt.left + bbRpt.Width + 10, self.AOwner.Height - 4, 120);
    CreatBitButton(self.bbExit, self.AOwner, self.AOwner, 'Выход', 2,
      bbPause.left + bbPause.Width + 100, self.AOwner.Height - 4, 70);
    self.MS := en_BCS_WaitStartNew;
    self.bbRpt.Images := self.OIL;
    self.bbStart.Images := self.OIL;
    self.bbExit.Images := self.OIL;
    self.bbPause.Images := self.OIL;
    self.bbRpt.OnClick := ClickbbRpt;
    self.bbStart.OnClick := ClickbbStart;
    self.bbPause.OnClick := ClickbbPause;
    self.bbExit.OnClick := ClickbbExit;
  end;

destructor TBttControl.Destroy();
  begin { }
    OEvtPause.Free;
    OEvtBreak.Free;
    OEvtRpt.Free;

    self.bbRpt.Free;
    self.bbStart.Free;
    self.bbExit.Free;
    self.bbPause.Free;
    inherited Destroy();
  end;

procedure TBttControl.ClickbbStart(Sender: TObject);
  var
    f: boolean;
  begin { }// продолжаем прерваное или старт заново
    f := true;
    if bbStart.Caption = 'Продолжить' then
      begin
        if Assigned(OnEvtContinue) then
          self.OnEvtContinue(f);
      end
    else
      begin
        if Assigned(OnEvtStart) then
          self.OnEvtStart(f);
      end;
    if not f then
      exit;
    self.bbStart.Visible := false;
    self.bbPause.Visible := true;
    // self.bbPause.Visible:=flgBttPse;
    // if flgBttPse then begin self.bbPause.Caption:='Пауза';self.bbPause.ImageIndex:=ord(TImgBtt.en_ImgBtt_Pause);end;
    self.bbPause.Caption := 'Пауза';
    self.bbPause.ImageIndex := ord(TImgBtt.en_ImgBtt_Pause);

    self.bbExit.Visible := true;
    self.MS := TBttControlState.en_BCS_Operate;

    OEvtPause.ResetEvent;
    OEvtBreak.ResetEvent;
    OEvtRpt.ResetEvent;
  end;

procedure TBttControl.ClickbbRpt(Sender: TObject);
  var
    f: boolean;
  begin
    flgBttRpt := false;
    f := true;
    if Assigned(OnEvtRpt) then
      self.OnEvtRpt(f);
    if not f then
      exit;
    self.bbRpt.Visible := false;
    self.bbPause.Caption := 'Пауза';
    self.bbPause.ImageIndex := ord(TImgBtt.en_ImgBtt_Pause);
    self.MS := TBttControlState.en_BCS_OperateRpt;

    OEvtRpt.ResetEvent;
    OEvtBreak.ResetEvent;
  end;

procedure TBttControl.ClickbbPause(Sender: TObject);
  var
    f: boolean;
  begin { }// ставим на паузу или продолжаем работу
    f := true;
    if self.bbPause.Caption = 'Пауза' then
      begin
        if Assigned(OnEvtPause) then
          self.OnEvtPause(f);
        if not f then
          exit;
        if (flgBttRpt) then
          begin
            self.bbRpt.Visible := true;
            self.bbRpt.Caption := 'Повторить';
            self.bbRpt.Enabled := true;
            self.bbRpt.ImageIndex := ord(TImgBtt.en_ImgBtt_Repeat);

            OEvtRpt.SetEvent;
          end;
        self.bbPause.Caption := 'Продолжить';
        self.bbPause.ImageIndex := ord(TImgBtt.en_ImgBtt_Resume);
        self.MS := TBttControlState.en_BCS_WaitOperateResume;

        OEvtPause.SetEvent;
        OEvtBreak.ResetEvent;
      end
    else
      begin
        if Assigned(OnEvtResume) then
          self.OnEvtResume(f);
        if not f then
          exit;
        flgBttRpt := false;
        self.bbRpt.Visible := false;
        self.bbPause.Caption := 'Пауза';
        self.bbPause.ImageIndex := ord(TImgBtt.en_ImgBtt_Pause);
        self.MS := TBttControlState.en_BCS_Operate;

        OEvtPause.ResetEvent;
        OEvtBreak.ResetEvent;
      end;
    flgBttRpt := false;
  end;

procedure TBttControl.ClickbbExit(Sender: TObject);
  var
    f: boolean;
  begin { }// выход
    f := true;
    if Assigned(OnEvtExit) then
      self.OnEvtExit(f);
    if not f then
      exit;
    self.bbRpt.Visible := false;
    self.bbStart.Visible := true;
    self.bbStart.Caption := 'Старт';
    self.bbStart.Enabled := true;
    self.bbStart.ImageIndex := ord(TImgBtt.en_ImgBtt_Start);
    self.bbPause.Visible := false;
    self.bbPause.Caption := 'Пауза';
    self.bbPause.ImageIndex := ord(TImgBtt.en_ImgBtt_Pause);
    self.bbExit.Visible := false;
    self.bbExit.Caption := 'Выход';
    self.bbExit.ImageIndex := ord(TImgBtt.en_ImgBtt_Exit);
    self.MS := TBttControlState.en_BCS_WaitStartNew;

    OEvtBreak.SetEvent;
  end;

procedure TBttControl.SiuationNew();
  begin { }
    OEvtPause.SetEvent;
    OEvtRpt.SetEvent;
    OEvtBreak.ResetEvent;

    flgBttRpt := false;
    // flgBttPse:=true;
    self.bbRpt.Visible := false;
    self.bbStart.Visible := true;
    self.bbStart.Caption := 'Старт';
    self.bbStart.Enabled := true;
    self.bbStart.ImageIndex := ord(TImgBtt.en_ImgBtt_Start);
    self.bbPause.Visible := false;
    self.bbPause.Caption := 'Пауза';
    self.bbPause.ImageIndex := ord(TImgBtt.en_ImgBtt_Pause);
    self.bbExit.Visible := false;
    self.bbExit.Caption := 'Выход';
    self.bbExit.ImageIndex := ord(TImgBtt.en_ImgBtt_Exit);
    self.MS := TBttControlState.en_BCS_WaitStartNew;
  end;

procedure TBttControl.SiuationContinue();
  begin { }
    OEvtPause.SetEvent;
    OEvtRpt.SetEvent;
    OEvtBreak.ResetEvent;

    flgBttRpt := false;
    // flgBttPse:=true;
    self.bbRpt.Visible := false;
    self.bbStart.Visible := true;
    self.bbStart.Caption := 'Продолжить';
    self.bbStart.Enabled := true;
    self.bbStart.ImageIndex := ord(TImgBtt.en_ImgBtt_Conti);
    self.bbPause.Visible := false;
    self.bbPause.Caption := 'Пауза';
    self.bbPause.ImageIndex := ord(TImgBtt.en_ImgBtt_Pause);
    self.bbExit.Visible := true;
    self.bbExit.Caption := 'Выход';
    self.bbExit.ImageIndex := ord(TImgBtt.en_ImgBtt_Exit);
    self.MS := TBttControlState.en_BCS_WaitStartContinius;
  end;

function TBttControl.getMS(): TBttControlState;
  begin
    result := self.MS;
  end;

{ ****************************************************************************** }
function TOAEvent.SignaledAllEvent: boolean;
  var
    i: integer;
  begin
    result := true;
    for i := 0 to Length(self.AEvent) - 1 do
      result := result and (self.AEvent[i].WaitFor(0) = wrSignaled);
    result := result;
  end;

procedure TOAEvent.AddAEvent(var Event: TEvent);
  var
    len: integer;
  begin
    len := Length(self.AEvent);
    setLength(self.AEvent, len + 1);
    self.AEvent[len] := Event;
  end;

constructor TOAEvent.Create();
  begin
    setLength(self.AEvent, 0);
  end;

destructor TOAEvent.Destroy();
  begin
    setLength(self.AEvent, 0);
  end;

{ ****************************************************************************** }
// procedure TChartSeriesOperate.AddValSeriesTCalc(ch:integer;TMV:TMeasuringVal);
// var
// valCalc:TCalc;
// LS:TLineSeries;
// PV_RV:TPV_RV;
// begin
// if assigned(onGetValCalc)then onGetValCalc(TMV,ch,valCalc) else begin showMessage('error absent onGetValCalc');exit; end;
// if assigned(onGetValLS)then onGetValLS(TMV,ch,LS) else begin showMessage('error absent onGetValLS');exit;end;
// if assigned(onGetTypPrintVal)then onGetTypPrintVal(PV_RV) else begin showMessage('error absent onGetTypPrintVal');exit;end;
// if (LS=nil)or(valCalc=nil) then exit;
//
// case PV_RV of
// en_pvrv_instant: AddValSeriesAVal(LS,valCalc.arrIinstant);
// en_pvrv_average:AddValSeriesAVal(LS,valCalc.arrAverage);
// en_pvrv_sko:AddValSeriesAVal(LS,valCalc.arrSko);
// en_pvrv_spd:AddValSeriesAVal(LS,valCalc.arrSpd);
// end;
// end;

// procedure TChartSeriesOperate.AddValSeriesAVal(var LS:TLineSeries;var AVal:rAVal);
// begin
// AddValSeriesXY(LS,AVal.time,AVal.val);
// LS.Title:=AVal.Title;
// end;
//
// procedure TChartSeriesOperate.AddValSeriesXY(var LS:TLineSeries;var ValX:TArrayDateTime;var ValY:TArrayDouble);
// begin
// LS.XValues.Value:=TChartValues(ValX);
// LS.XValues.Count:=length(ValX);
// LS.XValues.Modified:=true;
//
// LS.YValues.Value:=TChartValues(ValY);
// LS.YValues.Count:=length(ValY);
// LS.YValues.Modified:=true;
// end;
{ ****************************************************************************** }
// constructor TMyAdvStringGrid.Create(AOwner:TComponent);
// begin{}
// inherited Create(AOwner);
// end;
// function DoMouseWheel(Shift: TShiftState; WheelDelta: Integer; MousePos: TPoint): Boolean;
// function TMyAdvStringGrid.DoMouseWheel(Sender: TObject; Shift: TShiftState;
// WheelDelta: Integer; MousePos: TPoint; var Handled: Boolean): Boolean;
// begin
// Result := False;
// if Assigned(FOnMouseWheel) then
// FOnMouseWheel(Sender,Shift,WheelDelta,MousePos,Result);
// end;

function TMyAdvStringGrid.DoMouseWheelDown(Shift: TShiftState;
  MousePos: TPoint): boolean;
  begin
    result := false;
    SelectBaseCell;
    if Assigned(FOnMouseWheelDown) then
      FOnMouseWheelDown(self, Shift, MousePos, result);
  end;

function TMyAdvStringGrid.DoMouseWheelUp(Shift: TShiftState;
  MousePos: TPoint): boolean;
  begin
    result := false;
    SelectBaseCell;
    if Assigned(FOnMouseWheelUp) then
      FOnMouseWheelUp(self, Shift, MousePos, result);
  end;

{ function TMyAdvStringGrid.DoMouseWheelDown(Shift: TShiftState;
  MousePos: TPoint): Boolean;//override;
  var c,r:integer;
  begin
  r:=Row;c:=Col;
  Result := inherited DoMouseWheelDown(Shift,MousePos);
  Row:=r;Col:=c;
  {if (Row - TopRow > VisibleRowCount ) and

  (Row < RowCount - 1) and FHideLastRow then
  TopRow := TopRow + 1;

  SelectBaseCell;
  end; }

{ function TCommon.isxdigit_M(b:AnsiChar):byte;
  var
  v:byte;
  begin
  v:=byte(b);
  if (v >= $30) and (v <= $39) then isxdigit_M:=1
  else
  if (v >=$61)  and (v <= $66) then isxdigit_M:=2
  else
  if (v >= $41) and (v <= $46) then isxdigit_M:=3
  else isxdigit_M:=0;
  end;

  function TCommon.StrToHex_Forward(str:AnsiString; var buff:ArrayOfByte):boolean;
  var
  i,j, lByte,hByte,len: byte;
  begin
  result:=false;
  hByte:=0;
  lByte:=0;
  len:=(byte(length(str)div 2));
  SetLength(buff,len);

  for i:=1 to len do
  begin
  j:=i*2-1;
  case isxdigit_M(str[j]) of
  0:  begin
  exit;
  end;
  1:  begin
  hByte:=byte(str[j])-$30;
  end;
  2:  begin
  hByte:=byte(str[j])-$61+10;
  end;
  3:  begin
  hByte:=byte(str[j])-$41+10;
  end;
  end;
  hByte:=hByte*16;

  j:=i*2;
  case isxdigit_M(str[j]) of
  0:  begin
  exit;
  end;
  1:  begin
  lByte:=byte(str[j])-$30;
  end;
  2:  begin
  lByte:=byte(str[j])-$61+10;
  end;
  3:  begin
  lByte:=byte(str[j])-$41+10;
  end;
  end;

  buff[i-1]:=hByte+lByte;

  end;
  result:=true;
  end; }

// constructor TOAdvStringGrid_M0.Create_V0(AOwner:TComponent;Parent:TWinControl;CCnt,RCnt,FCCnt,FRCnt,top,left:integer);
// begin
// inherited Create(AOwner);
// self.Parent:=Parent;
// self.ParentFont:=true;
// self.Top:=top;
// self.Left:=left;
//
// self.Options:=self.Options+[goEditing];
// self.Navigation.AlwaysEdit:=true;
// self.Navigation.AdvanceAuto:=true;
// self.MouseActions.DirectEdit:=true;
// self.MouseActions.DirectComboDrop:=true;
// self.MouseActions.DirectDateDrop:=true;
// self.ShowSelection:=false;
//
// self.ColCount:=CCnt;
// self.RowCount:=RCnt;
// self.FixedCols:=FCCnt;
// self.FixedRows:=FRCnt;
//
// self.OnGetAlignment:=self.GetAlignment;
// self.OnKeyPress:=self._KeyPress;
// end;
//
// destructor TOAdvStringGrid_M0.Destroy();
// begin
// inherited Destroy();
// end;
//
// procedure TOAdvStringGrid_M0.GetAlignment(Sender: TObject; ARow, ACol: Integer;
// var HAlign: TAlignment;var VAlign: TAsgVAlignment);
// begin
// HAlign:=taCenter;
// VAlign:=vtaCenter;
// end;
//
// procedure TOAdvStringGrid_M0._KeyPress(Sender: TObject; var Key: Char);
// begin
// if Key=',' then Key:='.';
// end;
{ ****************************************************************************** }
// procedure TCustomSeriesMy.SetProperty(sps:TSeriesPointerStyle);
// begin
// self.Pointer.HorizSize:=4;
// self.Pointer.VertSize:=4;
// self.LineHeight:=1;
// self.Active:=false;
// self.Marks.ArrowLength := 8;
// self.Marks.Style := smsValue;
// self.Marks.Visible := false;
// self.PercentFormat := '##0.##,%';
// self.Pointer.Style:=sps;
// self.Pointer.Visible := true;
// self.Pointer.InflateMargins:=true;
// self.XValues.DateTime := False;
// self.XValues.Name := 'X';
// self.XValues.Order := loAscending;
// self.YValues.DateTime := False;
// self.YValues.Name := 'Y';
// self.YValues.Order := loNone;
// self.Visible:=true;
// end;
//
// constructor TCustomSeriesMy.CreateLineSeries(Owner:TComponent;sps:TSeriesPointerStyle);
// begin
// inherited Create(Owner);
// TLineSeries(self):=TLineSeries.Create(Owner);
// SetProperty(sps);
// end;
//
// constructor TCustomSeriesMy.CreatePointSeries(Owner:TComponent;sps:TSeriesPointerStyle);
// begin
// inherited Create(Owner);
// TPointSeries(self):=TPointSeries.Create(Owner);
// self.Pointer.HorizSize:=4;
// self.Pointer.VertSize:=4;
// self.SetProperty(sps);
// end;
//
// destructor TCustomSeriesMy.Destroy();
// begin
// if self.ClassName='TLineSeries' then
// TLineSeries(self).Free
// else
// if self.ClassName='TPointSeries' then
// TPointSeries(self).Free;
// inherited Destroy;
// end;

// constructor TCustomSeriesMy.Create(Owner:TComponent;sps:TSeriesPointerStyle);
// begin
// case TypeSeries of
// 0:begin
// TLineSeries(self):=TLineSeries.Create(Owner);
// self.SetProperty(sps);
// end;
// 1:begin
// TPointSeries(self):=TPointSeries.Create(Owner);
// self.Pointer.HorizSize:=4;
// self.Pointer.VertSize:=4;
// self.SetProperty(sps);
// end;
// end;
// end;


//
// constructor TPointSeriesMy.Create(Owner:TComponent;sps:TSeriesPointerStyle);
// begin
// inherited Create(Owner);
// self.Pointer.HorizSize:=4;
// self.Pointer.VertSize:=4;
/// /self.SetProperty(sps);
// end;
//
// destructor TPointSeriesMy.Destroy();
// begin
// inherited Free;
// end;
//
// constructor TLineSeriesMy.Create(Owner:TComponent;sps:TSeriesPointerStyle);
// begin
// inherited Create(Owner);
/// /self.SetProperty(sps);
// end;
//
// destructor TLineSeriesMy.Destroy();
// begin
// inherited Free;
// end;

{ ****************************************************************************** }
procedure CreatDlgSave(var Dlg: TSaveDialog; Owner: TComponent;
  Title, Filter, InitDir: string);
  begin
    Dlg := TSaveDialog.Create(Owner);
    if Title <> '' then
      Dlg.Title := Title;
    if Filter <> '' then
      Dlg.Filter := Filter;
    if InitDir <> '' then
      Dlg.InitialDir := InitDir;
  end;

procedure CreatDlgOpen(var Dlg: TOpenDialog; Owner: TComponent;
  Title, Filter, InitDir: string);
  begin
    Dlg := TOpenDialog.Create(Owner);
    if Title <> '' then
      Dlg.Title := Title;
    if Filter <> '' then
      Dlg.Filter := Filter;
    if InitDir <> '' then
      Dlg.InitialDir := InitDir;
  end;

// procedure CreateSeriasList(var cs:TCustomSeries;TypeSeries:integer;Owner:TComponent;sps:TSeriesPointerStyle);
// begin
// case TypeSeries of
// 0:TCustomSeriesMy(cs):=TCustomSeriesMy.CreateLineSeries(Owner,sps);
// 1:TCustomSeriesMy(cs):=TCustomSeriesMy.CreatePointSeries(Owner,sps);
// else
// cs:=nil;
// end
// end;

procedure { TCommon. } CreatLabel(var Lbl: TLabel; AOwner: TComponent;
  Parent: TWinControl; Caption: string; top, left, Size: integer;
  Style: TFontStyles);
  begin
    Lbl := TLabel.Create(AOwner);
    Lbl.Caption := Caption;
    Lbl.Font.Style := Style;
    if Size <> 0 then
      Lbl.Font.Size := Size;
    Lbl.top := top;
    Lbl.left := left;
    Lbl.Parent := Parent;
    Lbl.ParentFont := true;
  end;

procedure CreatRzNumericEdit(var ne: TRzNumericEdit; AOwner: TComponent;
  Parent: TWinControl; top, left, Width, Height: integer;
  IntegersOnly: boolean; DisplayFormat: string);
  begin
    ne := TRzNumericEdit.Create(AOwner);
    ne.IntegersOnly := IntegersOnly;
    ne.Parent := Parent;
    ne.ParentFont := true;
    ne.left := left;
    ne.top := top;
    ne.Width := Width;
    ne.Height := Height;
    ne.DisplayFormat := DisplayFormat;
    ne.AutoSize := false;
    ne.Font.Style := [];
    ne.Font.Size := 8;
  end;

procedure CreatRzSpinEdit(var se: TRzSpinEdit; AOwner: TComponent;
  Parent: TWinControl; top, left, Width, Height, min, max: integer;
  IntegersOnly: boolean);
  begin
    se := TRzSpinEdit.Create(AOwner);
    se.IntegersOnly := IntegersOnly;
    se.min := min;
    se.max := max;
    se.Parent := Parent;
    se.left := left;
    se.top := top;
    se.Width := Width;
    se.Height := Height;
    se.Font.Style := [];
    se.Font.Size := 8;
    se.AutoSize := false;
  end;

procedure { TCommon. } OnGetAlignmentAdvSG(Sender: TObject;
  ARow, ACol: integer; var HAlign: TAlignment; var VAlign: TAsgVAlignment);
  begin
    HAlign := taCenter;
    VAlign := vtaCenter;
  end;

procedure CreatMyAdvSg(var AdvSg: TMyAdvStringGrid; AOwner: TComponent;
  Parent: TWinControl; top, left: integer);
  begin
    AdvSg := TMyAdvStringGrid.Create(AOwner);
    AdvSg.Parent := Parent;
    AdvSg.top := top;
    AdvSg.left := left;

    AdvSg.Options := AdvSg.Options + [goEditing];
    AdvSg.Navigation.AlwaysEdit := true;
    AdvSg.Navigation.AdvanceAuto := true;
    AdvSg.MouseActions.DirectEdit := true;
    AdvSg.MouseActions.DirectComboDrop := true;
    AdvSg.MouseActions.DirectDateDrop := true;
    AdvSg.ShowSelection := false;
  end;

procedure { TCommon. } CreatAdvSg(var AdvSg: TAdvStringGrid;
  AOwner: TComponent; Parent: TWinControl; top, left: integer);
  begin
    AdvSg := TAdvStringGrid.Create(AOwner);
    AdvSg.Parent := Parent;
    AdvSg.ParentFont := true;
    AdvSg.top := top;
    AdvSg.left := left;

    AdvSg.Options := AdvSg.Options + [goEditing];
    AdvSg.Navigation.AlwaysEdit := true;
    AdvSg.Navigation.AdvanceAuto := true;
    AdvSg.MouseActions.DirectEdit := true;
    AdvSg.MouseActions.DirectComboDrop := true;
    AdvSg.MouseActions.DirectDateDrop := true;
    AdvSg.ShowSelection := false;
  end;

procedure { TCommon. } DrowAdvSg(var AdvSg: TAdvStringGrid;
  CCnt, RCnt, FCCnt, FRCnt: integer);
  begin
    AdvSg.ColCount := CCnt;
    AdvSg.RowCount := RCnt;
    AdvSg.FixedCols := FCCnt;
    AdvSg.FixedRows := FRCnt;
  end;

procedure { TCommon. } CreatAndDrowAdvSg(var AdvSg: TAdvStringGrid;
  AOwner: TComponent; Parent: TWinControl; CCnt, RCnt, FCCnt, FRCnt, top,
  left: integer);
  begin
    CreatAdvSg(AdvSg, AOwner, Parent, top, left);
    DrowAdvSg(AdvSg, CCnt, RCnt, FCCnt, FRCnt);
  end;

procedure { TCommon. } CreatAndDrowMyAdvSg(var AdvSg: TMyAdvStringGrid;
  AOwner: TComponent; Parent: TWinControl; CCnt, RCnt, FCCnt, FRCnt, top,
  left: integer);
  begin
    CreatMyAdvSg(AdvSg, AOwner, Parent, top, left);
    DrowAdvSg(TAdvStringGrid(AdvSg), CCnt, RCnt, FCCnt, FRCnt);
  end;

// procedure {TCommon.}CreatGroupBox(var GB:TGroupBox;AOwner:TComponent; Parent:TWinControl;Caption:string;top,left,H,W:integer);
// begin
// GB:=TGroupBox.Create(FMain);
// GB.Parent:=Parent;
// GB.Top:=Top;
// GB.Left:=left;
// GB.Width:=W;
// GB.Height:=H;
// GB.Caption:=Caption;
// end;
//
procedure { TCommon. } CreatBitButton(var rzBB: TRzBitBtn; AOwner: TComponent;
  Parent: TWinControl; Caption: string; top, left, H, W: integer);
  begin
    rzBB := TRzBitBtn.Create(AOwner);
    rzBB.Parent := Parent;
    rzBB.top := top;
    rzBB.left := left;
    rzBB.Width := W;
    rzBB.Height := H;
    rzBB.Caption := Caption;
    rzBB.Font.Size := 8;
  end;

// procedure {TCommon.}CreatRzEdit(var rzE:TRzEdit;AOwner:TComponent; Parent:TWinControl;Text:string;top,left,H,W:integer);
// begin
// rzE:=TRzEdit.Create(FMain);
// rzE.Parent:=Parent;
// rzE.Top:=Top;
// rzE.Left:=left;
// rzE.Width:=W;
// rzE.Height:=H;
// rzE.Text:=Text;
// end;

// procedure {TCommon.}CreatEdit(var Edit:TEdit;AOwner:TComponent; Parent:TWinControl;Text:string;top,left,H,W:integer);
// begin
// Edit:=TEdit.Create(FMain);
// Edit.Parent:=Parent;
// Edit.ParentFont:=true;
// Edit.Top:=Top;
// Edit.Left:=left;
// Edit.Width:=W;
// Edit.Height:=H;
// Edit.Text:=Text;
// end;
procedure CreatListBox(var ListBox: TListBox; AOwner: TComponent;
  Parent: TWinControl; top, left, H, W: integer; var PUp: TPopUpMenu);
  begin
    ListBox := TListBox.Create(AOwner);
    ListBox.Parent := Parent;
    ListBox.ParentFont := true;
    ListBox.top := top;
    ListBox.left := left;
    ListBox.Height := H;
    ListBox.Width := W;
    ListBox.PopupMenu := PUp;
  end;

procedure CreatComboBox(var CBox: TComboBox; AOwner: TComponent;
  Parent: TWinControl; top, left, H, W: integer; stl: TComboBoxStyle);
  begin
    CBox := TComboBox.Create(AOwner);
    CBox.Parent := Parent;
    CBox.ParentFont := true;
    CBox.top := top;
    CBox.left := left;
    CBox.Height := H;
    CBox.Width := W;
    CBox.Style := stl;
  end;

procedure CreatCheckList(var CheckList: TRzCheckList; AOwner: TComponent;
  Parent: TWinControl; top, left, H, W: integer);
  begin
    CheckList := TRzCheckList.Create(AOwner);
    CheckList.Parent := Parent;
    CheckList.ParentFont := true;
    CheckList.top := top;
    CheckList.left := left;
    CheckList.Height := H;
    CheckList.Width := W;
  end;

function { TCommon. } MyStrToFloat(s: String; var v: Double): boolean;
  var
    DS: Char;
    e: extended;
    ch: Char;
    i: byte;
  begin
    DS := DecimalSeparator;
    if DS = '.' then
      ch := ','
    else
      ch := '.';
    if Length(s) > 0 then
      begin
        for i := 1 to Length(s) do
          begin
            if s[i] = ch then
              s[i] := DS;
          end;
      end;
    e := 0;
    result := tryStrToFloat(s, e);
    v := e;
  end;

procedure CalcSizeSGTab(var Tab: TAdvStringGrid; var H, W: integer);
  var
    i: integer;
  begin
    H := 5;
    for i := 0 to Tab.RowCount - 1 do
      H := H + Tab.RowHeights[i];
    W := 5;
    for i := 0 to Tab.ColCount - 1 do
      W := W + Tab.ColWidths[i];
  end;

procedure { TCommon. } SetSizeSGTab(var Tab: TAdvStringGrid);
  var
    H, W: integer;
  begin
    CalcSizeSGTab(Tab, H, W);
    Tab.Height := H;
    Tab.Width := W;
  end;

{ procedure SetSizeSGTabAndScrollBar(Tab:TAdvStringGrid;MaxH:Integer);
  var
  H,W,i:integer;
  begin
  CalcSizeSGTab(Tab,H,W);
  Tab.Height:=H;Tab.Width:=W;

  if Tab.Height>MaxH then
  begin//разрешаем полосу прокрутки
  Tab.ScrollBars:=ssVertical;
  Tab.Width:=Tab.Width+Tab.ScrollWidth+3;
  Tab.Height:=MaxH;
  end
  else
  begin //запрещаем полосу прокрутки
  Tab.ScrollBars:=ssNone;
  end;
  end; }

{ procedure CheckSetSizeSGTabAndScrollBar1(Tab:TAdvStringGrid;MaxH:Integer);
  var
  H,W,i:integer;

  begin
  CalcSizeSGTab(Tab,H,W);

  if H>MaxH then
  begin//разрешаем полосу прокрутки
  if Tab.ScrollBars<>ssVertical then
  begin
  Tab.ScrollBars:=ssVertical;
  Tab.Width:=Tab.Width+Tab.ScrollWidth+1;
  end;
  Tab.Height:=MaxH;
  end
  else
  begin
  if Tab.Height<>H then Tab.Height:=H;
  if Tab.ScrollBars<>ssNone then
  begin //запрещаем полосу прокрутки
  Tab.ScrollBars:=ssNone;
  Tab.Width:=Tab.Width-Tab.ScrollWidth-1;
  end;
  end;
  end; }

procedure { TCommon. } CheckSetSizeSGTabAndScrollBar(Tab: TAdvStringGrid;
  MaxH, MaxW: integer);
  var
    H, W, i: integer;
  begin

    CalcSizeSGTab(Tab, H, W);

    i := 0;
    if H > MaxH then
      i := i + 1;
    if W > MaxW then
      i := i + 2;
    case i of
      1:
        W := W + Tab.ScrollWidth;
      2:
        H := H + Tab.ScrollWidth;
    end;
    i := 0;
    if H > MaxH then
      begin
        i := i + 1;
        H := MaxH;
      end;
    if W > MaxW then
      begin
        i := i + 2;
        W := MaxW;
      end;
    if Tab.Height <> H then
      Tab.Height := H;
    if Tab.Width <> W then
      Tab.Width := W;

    case i of
      0:
        // if Tab.ScrollBars<>ssNone then
        Tab.ScrollBars := ssNone;
      1:
        // if Tab.ScrollBars<>ssVertical then
        Tab.ScrollBars := ssVertical;
      2:
        // if Tab.ScrollBars<>ssHorizontal then
        Tab.ScrollBars := ssHorizontal;
      3:
        // if Tab.ScrollBars<>ssBoth then
        Tab.ScrollBars := ssBoth;
      else
        Tab.ScrollBars := ssNone;
      end;
  end;

procedure CheckDecimalSeparatorOnKeyPress(Sender: TObject; var Key: AnsiChar);
  begin
    if Key = ',' then
      Key := '.';
  end;

function PrintSN(SN: integer): string;
  begin
    result := FormatCurr('0000000', SN);
  end;

function PrintZN(ZN: integer): string;
  begin
    result := FormatCurr('00000000', ZN);
  end;

// function MessageSetT(str:string):integer;
// var
/// /HorzMargin, VertMargin, HorzSpacing, VertSpacing, ButtonWidth,
/// /ButtonHeight, ButtonSpacing, ButtonCount, ButtonGroupWidth,
/// /IconTextWidth, IconTextHeight, X, ALeft: Integer;
/// /B, CancelButton: TMsgDlgBtn;
//
/// /t,d,l,i,w,h:integer;
// lbl,lbl0:TLabel;
// btt:array of TButton;
// r:TRect;
// frm:TForm;
// begin
// frm:=TForm.Create(Application);
// frm.Caption:='';
// with frm do
// begin
// BiDiMode := Application.BiDiMode;
// BorderStyle := bsDialog;
// Canvas.Font := Font;
// KeyPreview := True;
// PopupMode := pmAuto;
// Position := poDesigned;
// SetRect(r, 0, 0, Screen.Width div 2, 0);
// DrawText(Canvas.Handle, str, Length(str)+1, r,
// DT_EXPANDTABS or DT_CALCRECT or DT_WORDBREAK or DrawTextBiDiModeFlagsReadingOnly);
//
// {
// Windows.DrawText(
// canvas.handle,
// ButtonCaptions[B],
// -1,
// TextRect,
// DT_CALCRECT or DT_LEFT or DT_SINGLELINE or DrawTextBiDiModeFlagsReadingOnly);
// }
// end;
//
// frm.ShowModal;
// frm.Free;
//
//
//
/// /frm:=CreateMessageDialog(str,DlgType,Buttons);
// {  frm := TMessageForm.CreateNew(Application);
// with frm do
// begin
// BiDiMode := Application.BiDiMode;
// BorderStyle := bsDialog;
// Canvas.Font := Font;
// KeyPreview := True;
// PopupMode := pmAuto;
// Position := poDesigned;
// OnKeyDown := TMessageForm(Result).CustomKeyDown;
// DialogUnits := GetAveCharSize(Canvas);
// HorzMargin := MulDiv(mcHorzMargin, DialogUnits.X, 4);
// VertMargin := MulDiv(mcVertMargin, DialogUnits.Y, 8);
// HorzSpacing := MulDiv(mcHorzSpacing, DialogUnits.X, 4);
// VertSpacing := MulDiv(mcVertSpacing, DialogUnits.Y, 8);
// ButtonWidth := MulDiv(mcButtonWidth, DialogUnits.X, 4);
// Font.Assign(Screen.MessageFont);
// TextRect := Rect(0,0,0,0);
// Windows.DrawText(
// canvas.handle,
// ButtonCaptions[B],
// -1,
// TextRect,
// DT_CALCRECT or DT_LEFT or DT_SINGLELINE or DrawTextBiDiModeFlagsReadingOnly);
//
// end;
// }
// end;

function MessageDlgM(str: string; Size: integer; DlgType: TMsgDlgType;
  Buttons: TMsgDlgButtons): integer;
  var
    t, d, l, i, W, H: integer;
    Lbl: TLabel;
    btt: array of TButton;
    // r:TRect;
    frm: TForm;
  begin
    frm := CreateMessageDialog(str, DlgType, Buttons);

    Lbl := TLabel.Create(frm);
    Lbl.AutoSize := true;
    Lbl.Parent := frm;
    Lbl.Caption := str;
    Lbl.Font.Size := Size;
    W := Lbl.Width + 40;
    H := Lbl.Height;
    Lbl.Free;

    Lbl := nil;
    for i := 0 to frm.ControlCount - 1 do
      if frm.Controls[i].ClassName = 'TLabel' then
        Lbl := TLabel(frm.Controls[i]);

    for i := 0 to frm.ControlCount - 1 do
      if frm.Controls[i].ClassName = 'TButton' then
        begin
          l := Length(btt);
          setLength(btt, l + 1);
          btt[l] := TButton(frm.Controls[i]);
        end;

    if (Lbl <> nil) then
      begin
        Lbl.Font.Size := Size;

        Lbl.Font.Size := Size;
        Lbl.Width := W;
        Lbl.Height := H;

        t := Lbl.top + Lbl.Height + 10;
        if t < 50 then
          t := 55;
        d := frm.Width;
        frm.Width := Lbl.left + Lbl.Width + 40;
        frm.Height := t + btt[0].Height + 50;
        d := (frm.Width - d) div 2;
        for i := 0 to Length(btt) - 1 do
          begin
            btt[i].top := t;
            btt[i].left := d + btt[i].left;
          end;
      end;

    frm.Position := poScreenCenter;
    result := frm.ShowModal;
    frm.Free;
  end;

function StrInternetToDateTime(Value: string; var DTVal: TDateTime): integer;
  const
    Months: array [0 .. 11] of string = ('янв', 'фев', 'мар', 'апр', 'май',
      'июн', 'июл', 'авг', 'сен', 'окт', 'ноя', 'дек');
  var
    i, j: integer;
    v: string;
  begin
    result := -1;
    for i := 1 to Length(Value) do
      if Value[i] = ' ' then
        Value[i] := '.';

    if TryStrToDate(Value, DTVal) = true then
      begin
        result := 0;
      end
    else
      begin
        Value := LowerCase(Value);
        v := '';
        for i := 1 to Length(Value) do
          if IsDigit(Value, i) then
            v := v + Value[i]
          else
            break;
        v := v + '.';
        for i := 0 to Length(Months) - 1 do
          begin
            j := pos(Months[i], Value);
            if j > 0 then
              break;
          end;
        if i >= Length(Months) then
          exit;
        v := v + IntToStr(i + 1);
        v := v + '.';
        for i := Length(Value) downto 1 do
          if not IsDigit(Value, i) then
            begin
              for j := i + 1 to Length(Value) do
                v := v + Value[j];
              break;
            end;
        if TryStrToDate(v, DTVal) = true then
          result := 1;
      end;
  end;

Procedure Sound(Duration: integer; Freq: Word);
  var
    Ver: TOsVersionInfo;
  begin
    Ver.dwOSVersionInfoSize := SizeOf(Ver);
    GetVersionEx(Ver);
    if Ver.dwPlatformId = VER_PLATFORM_WIN32_NT then
      Windows.Beep(Duration, Freq)
    else
      begin
    asm
      mov al,0b6H
      out 43H,al
      mov ax,Freq
      out 42h,al
      ror ax,8
      out 42h,al
      in  al,61H
      or  al,03H
      out 61H,al
    end;
        sleep(Duration);
    asm
      in  al,61H
      and al,0fcH
      out 61H,al
    end;
      end;
  end;

procedure PrintAdvSG(var AdvSg: TAdvStringGrid; ACol, ARow: integer; s: string;
  Color: TColor);
  begin
    AdvSg.Cells[ACol, ARow] := s;
    AdvSg.CellProperties[ACol, ARow].BrushColor := Color;
  end;

function E9FileStatus(const Origin: string): boolean;
  var
    f: TFileStream;
  begin
    {
      Значение             Описание
      fmCreate             Созда¸т файл с данным именем. Если файл существует, то открыть его в
      режиме записи.
      fmOpenRead           Открыть файл только для чтения.
      fmOpenWrite          Открыть файл только на запись. При этом запись в файл заменит вс¸ его
      содержимое.
      fmOpenReadWrite Открыть файл скорее для изменения содержимого чем для замены его.

      Режим доступа должен иметь одно из следующих значений:

      Значение            Описание
      fmShareCompat       Доступ к файлу совместим с FCB.
      fmShareExclusive    Другое приложение не может открыть файл для различных целей.
      fmShareDenyWrite    Другое приложение может открыть файл для чтения, но не для записи.
      fmShareDenyRead     Другое приложение может открыть файл для записи, но не для чтения.
      fmShareDenyNone     Разрешить другим файлам делать с файлом и чтени и запись.

      Если файл невозможно открыть, то Create сгенерирует исключение.
      Возвращает true если файл не заблокирован
      }
    if FileExists(Origin) then
      begin
        try
          f := TFileStream.Create(Origin, fmOpenReadWrite or fmShareExclusive);
          try
            result := true;
          finally
            f.Free;
          end;
        except
          result := false;
        end;
      end
    else
      result := true;
  end;

function AddLog(FP: TFileName; N: string; str: string): boolean;
  var
    f: TextFile;
    // Text: String;
    i: integer;
    FFN: TFileName;
  begin
    try
      for i := 1 to Length(N) - 1 do
        if (N[i] = '.') or (N[i] = ':') then
          N[i] := '_';
      FFN := FP + N + '.txt';
      AssignFile(f, FFN);
      if FileExists(FFN) then
        Append(f)
      else
        Rewrite(f);
      WriteLn(f, str);
      CloseFile(f);
    except
      result := false;
      exit;
    end;
    result := true;
  end;

function adler32(pVal: PByte; buflength: integer): DWORD;
  var
    s1, s2: DWORD;
    val: byte;
    i: integer;
  begin
    s1 := 1;
    s2 := 0;
    for i := 0 to buflength - 1 do
      begin
        val := pVal^;
        s1 := (s1 + val) mod 65521;
        s2 := (s2 + s1) mod 65521;
        dec(pVal);
      end;
    result := (s2 Shl 16) + s1;
  end;

function AnsiCharToChar(var AChar: AnsiChar): Char;
  var
    l, H: integer;
  begin
    result := Char(#0);

    if not IsRussianAnsiLetter(AChar) then
      result := Char(AChar)
    else
      begin
        l := ord(AChar) - ord(AnsiChar('а'));
        H := ord(AChar) - ord(AnsiChar('А'));
        if ((ord(AChar) >= ord(AnsiChar('А'))) and
              (ord(AChar) <= ord(AnsiChar('Я')))) then
          begin
            result := Char(ord(Char('А')) + H);
          end;
        if ((ord(AChar) >= ord(AnsiChar('а')))
          { and (Ord(AStr[i]) <= Ord(AnsiChar('я'))) } ) or
            (ord(AChar) = ord(AnsiChar('ё'))) then
          begin
            result := Char(ord(Char('а')) + l);
          end;
        if (ord(AChar) = ord(AnsiChar('Ё'))) then
          result := Char('Ё');
        if (ord(AChar) = ord(AnsiChar('ё'))) then
          result := Char('ё');
      end;

  end;

function AnsiToStr(var AStr: AnsiString): string;
  var
    x, i: integer;
  begin
    x := Length(AStr);
    for i := 1 to Length(AStr) do
      if (AStr[i] = AnsiChar(0)) then
        begin
          x := i - 1;
          break;
        end;

    setLength(result, x);
    for i := 1 to Length(AStr) do
      result[i] := AnsiCharToChar(AStr[i]);

    // SetLength(result,x);
    // for i:=1 to length(AStr) do
    // if not IsRussianAnsiLetter(AStr[i])then
    // result[i]:=Char(AStr[i])
    // else
    // begin
    // l:=Ord(AStr[i])-Ord(AnsiChar('а'));
    // h:=Ord(AStr[i])-Ord(AnsiChar('А'));
    // if((Ord(AStr[i]) >= Ord(AnsiChar('А'))) and (Ord(AStr[i]) <= Ord(AnsiChar('Я')))) then
    // begin result[i]:=Char(Ord(Char('А'))+h); end;
    // if ((Ord(AStr[i]) >= Ord(AnsiChar('а'))) {and (Ord(AStr[i]) <= Ord(AnsiChar('я')))}) or (Ord(AStr[i]) = Ord(AnsiChar('ё'))) then
    // begin result[i]:=Char(Ord(Char('а'))+l); end;
    // if (Ord(AStr[i]) = Ord(AnsiChar('Ё'))) then result[i]:=Char('Ё');
    // if (Ord(AStr[i]) = Ord(AnsiChar('ё'))) then result[i]:=Char('ё');
    // end;
  end;

// procedure AnsiToStr(var AStr:AnsiString;var Str:string);
// var
// i:integer;
// l,h:integer;
// begin
// SetLength(Str,length(AStr));
// for i:=1 to length(AStr) do
// Str[i]:=Char(AStr[i])
// end;

function CharToAnsiChar(var Chr: Char): AnsiChar;
  var
    l, H: integer;
  begin
    result := Char(#0);

    if not IsRussianLetter(Chr) then
      result := AnsiChar(Chr)
    else
      begin
        l := ord(Chr) - ord(Char('а'));
        H := ord(Chr) - ord(Char('А'));
        if ((ord(Chr) >= ord(Char('А'))) and (ord(Chr) <= ord(Char('Я')))) then
          begin
            result := AnsiChar(ord(AnsiChar('А')) + H);
          end;
        if ((ord(Chr) >= ord(Char('а'))) and (ord(Chr) <= ord(Char('я')))) or
            (ord(Chr) = ord(Char('ё'))) then
          begin
            result := AnsiChar(ord(AnsiChar('а')) + l);
          end;
        if (ord(Chr) = ord(Char('Ё'))) then
          result := AnsiChar('Ё');
        if (ord(Chr) = ord(Char('ё'))) then
          result := AnsiChar('ё');
      end;
  end;

function StrToAnsi(var str: string): AnsiString;
  var
    x, i: integer;
  begin
    x := Length(str);
    for i := 1 to Length(str) do
      if (str[i] = Char(0)) then
        begin
          x := i - 1;
          break;
        end;
    setLength(result, x);
    for i := 1 to Length(str) do
      result[i] := CharToAnsiChar(str[i]);

    // SetLength(result,x);
    // for i:=1 to length(Str) do
    // if not IsRussianLetter(Str[i])then
    // result[i]:=AnsiChar(Str[i])
    // else
    // begin
    // l:=Ord(Str[i])-Ord(Char('а'));
    // h:=Ord(Str[i])-Ord(Char('А'));
    // if((Ord(Str[i]) >= Ord(Char('А'))) and (Ord(Str[i]) <= Ord(Char('Я')))) then
    // begin result[i]:=AnsiChar(Ord(AnsiChar('А'))+h); end;
    // if ((Ord(Str[i]) >= Ord(Char('а'))) and (Ord(Str[i]) <= Ord(Char('я')))) or (Ord(Str[i]) = Ord(Char('ё'))) then
    // begin result[i]:=AnsiChar(Ord(AnsiChar('а'))+l); end;
    // if (Ord(Str[i]) = Ord(Char('Ё'))) then result[i]:=AnsiChar('Ё');
    // if (Ord(Str[i]) = Ord(Char('ё'))) then result[i]:=AnsiChar('ё');
    // end;
  end;

function IsRussianLetter(ACh: Char): boolean;
  begin
    result := ((ord(ACh) >= ord(Char('А'))) and (ord(ACh) <= ord(Char('Я'))))
        or (ord(ACh) = ord(Char('Ё'))) or
        ((ord(ACh) >= ord(Char('а'))) and (ord(ACh) <= ord(Char('я')))) or
        (ord(ACh) = ord(Char('ё')));
    // Result := (ACh >= 'а') and (ACh <= 'я') or
    // (ACh >= 'А') and (ACh <= 'Я') or
    // (ACh = 'ё') or (ACh = 'Ё');

  end;

function IsRussianAnsiLetter(ACh: AnsiChar): boolean;
  begin
    result := ((ord(ACh) >= ord(AnsiChar('А'))) and
          (ord(ACh) <= ord(AnsiChar('Я')))) or (ord(ACh) = ord(AnsiChar('Ё')))
        or ((ord(ACh) >= ord(AnsiChar('а')))
      { and (Ord(ACh) <= Ord(AnsiChar('я'))) }
    ) or (ord(ACh) = ord(AnsiChar('ё')));
    // Result := (ACh >= 'а') and (ACh <= 'я') or
    // (ACh >= 'А') and (ACh <= 'Я') or
    // (ACh = 'ё') or (ACh = 'Ё');

  end;

// procedure ConvFromDOS(var Source: AnsiString);
// var
// i: integer;
// ACh:
// begin
// for i := 1 to Length(Source) do
// begin
// (ACh >= 'а') and (ACh <= 'я') or
// (ACh >= 'А') and (ACh <= 'Я') or
// (ACh = 'ё') or (ACh = 'Ё');
//
//
//
//
/// /      case Ord(Source[i]) of
/// /        240: Source[i] := Chr(168);                                               //  буква Ё (большая)
/// /        241: Source[i] := Chr(184);                                               //  буква ё (маленькая)
/// /        252: Source[i] := Chr(185);                                               //  знак №  (номер)
/// /          else
/// /        if (Ord(Source[i]) > 127) then
/// /          if Ord(Source[i]) < 176 then
/// /            Source[i] := AnsiChar(Ord(Source[i]) + 64)
/// /              else
/// /            Source[i] := AnsiChar(Ord(Source[i]) + 16);
/// /      end;
// end;
// end;

{ ****************************************************************************** }
constructor TOWait_Quit.Create();
  begin
    inherited Create();
    idTimer := 0;
  end;

destructor TOWait_Quit.Destroy();
  begin { }
    if (idTimer <> 0) then
      KillTimer(0, idTimer);
    inherited Destroy();
  end;

procedure TOWait_Quit.SetDelay(mls: DWORD);
  begin
    if INFINITE <> mls then
      begin
        if (idTimer <> 0) then
          KillTimer(0, idTimer);
        self.idTimer := SetTimer(0, 0, mls, nil);
      end;
  end;

function TOWait_Quit.WaitForMulipleObject(mls: DWORD; var Event_Brk: TEvent;
  var OAEvent: TOAEvent): TMyWaitResult;
  begin
    SetDelay(mls);
    result := enMWR_Rest;
    while true do
      begin
        if OAEvent.SignaledAllEvent() then
          begin
            result := TMyWaitResult.enMWR_SetEvetObj;
            break;
          end;
        if Event_Brk.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetBrk;
            break;
          end;
        result := ProcessMessages();
        if (result = enMWR_GetMsgExit) or (result = enMWR_SetTimout) then
          break;
      end;
  end;

function TOWait_Quit.WaitForMulipleAndPause(mls: DWORD;
  var Event_Brk, Event_Pse: TEvent; var OAEvent: TOAEvent)
    : TMyWaitResult;
  begin
    SetDelay(mls);
    result := enMWR_Rest;
    while true do
      begin
        if OAEvent.SignaledAllEvent() then
          begin
            result := TMyWaitResult.enMWR_SetEvetObj;
            break;
          end;
        if Event_Pse.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetPse;
            break;
          end;
        if Event_Brk.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetBrk;
            break;
          end;
        result := ProcessMessages();
        if (result = enMWR_GetMsgExit) or (result = enMWR_SetTimout) then
          break;
      end;
  end;

function TOWait_Quit.WaitForThTerminate(mls: DWORD;
  var Event_Obj: TEvent): TMyWaitResult;
  // var
  // Msg: TMsg;
  // fTimerStop:boolean;
  // fMsgExists:boolean;
  // fUnicode:boolean;
  // fHandled:boolean;
  begin
    SetDelay(mls);
    // fTimerStop:=idTimer=0;
    result := enMWR_Rest;
    while true do
      begin
        if Event_Obj.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetObj;
            break;
          end;
        result := ProcessMessages(); // выгребаем всю кучу сообщений
        if (result = enMWR_GetMsgExit) or (result = enMWR_SetTimout) then
          break;
      end;
  end;

function TOWait_Quit.WaitForObjectAndPause(mls: DWORD;
  var Event_Brk, Event_Pse, Event_Obj: TEvent): TMyWaitResult;
  // var
  // Msg: TMsg;
  // fTimerStop:boolean;
  // fMsgExists:boolean;
  // fUnicode:boolean;
  // fHandled:boolean;
  begin
    SetDelay(mls);
    // fTimerStop:=idTimer=0;
    result := enMWR_Rest;
    while true do
      begin
        if Event_Pse.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetPse;
            break;
          end;
        if Event_Obj.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetObj;
            break;
          end;
        if Event_Brk.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetBrk;
            break;
          end;
        result := ProcessMessages(); // выгребаем всю кучу сообщений
        if (result = enMWR_GetMsgExit) or (result = enMWR_SetTimout) then
          break;
      end;
  end;

function TOWait_Quit.WaitForObject(mls: DWORD; var Event_Brk, Event_Obj: TEvent)
    : TMyWaitResult;
  // var
  // Msg: TMsg;
  // fTimerStop:boolean;
  // fMsgExists:boolean;
  // fUnicode:boolean;
  // fHandled:boolean;
  begin
    SetDelay(mls);
    // fWMQuit:=false;
    // fTimerStop:=idTimer=0;
    result := enMWR_Rest;
    while true do
      begin
        if Event_Obj.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetObj;
            break;
          end;
        if Event_Brk.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetBrk;
            break;
          end;
        result := ProcessMessages(); // выгребаем всю кучу сообщений
        if (result = enMWR_GetMsgExit) or (result = enMWR_SetTimout) then
          break;
      end;
  end;

function TOWait_Quit.WaitForBreak(mls: DWORD; var Event_Brk: TEvent)
    : TMyWaitResult;
  // var
  // Msg: TMsg;
  // fTimerStop:boolean;
  // fMsgExists:boolean;
  // fUnicode:boolean;
  // fHandled:boolean;
  begin
    SetDelay(mls);
    // fTimerStop:=idTimer=0;
    result := enMWR_Rest;
    while true do
      begin
        if Event_Brk.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetBrk;
            break;
          end;
        result := ProcessMessages();
        if (result = enMWR_GetMsgExit) or (result = enMWR_SetTimout) then
          break;
      end;
  end;

function TOWait_Quit.WaitForContinue(mls: DWORD;
  var Event_Brk, Event_Pse: TEvent): TMyWaitResult;
  begin
    SetDelay(mls);
    result := enMWR_Rest;
    while true do
      begin
        if Event_Brk.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetBrk;
            break;
          end;
        if Event_Pse.WaitFor(0) = wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetObj;
            break;
          end;

        result := ProcessMessages();
        if (result = enMWR_GetMsgExit) or (result = enMWR_SetTimout) then
          break;
      end;
  end;

function TOWait_Quit.WaitFor(mls: DWORD; var Event_Brk, Event_Pse: TEvent)
    : TMyWaitResult;
  begin
    SetDelay(mls);
    result := enMWR_Rest;
    while true do
      begin
        if Event_Brk.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetBrk;
            break;
          end;
        if Event_Pse.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetPse;
            break;
          end;

        result := ProcessMessages();
        if (result = enMWR_GetMsgExit) or (result = enMWR_SetTimout) then
          break;
      end;
  end;

function TOWait_Quit.WaitForRepeat(mls: DWORD; var Event_Brk, Event_Pse,
  Event_Rpt: TEvent): TMyWaitResult;
  // var
  // Msg: TMsg;
  // fTimerStop:boolean;
  // fMsgExists:boolean;
  // fUnicode:boolean;
  // fHandled:boolean;
  begin
    SetDelay(mls);
    // fTimerStop:=idTimer=0;
    result := enMWR_Rest;
    while true do
      begin
        if Event_Pse.WaitFor(0) = wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetObj;
            Event_Rpt.ResetEvent;
            break;
          end;
        if Event_Rpt.WaitFor(0) = wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetRpt;
            Event_Pse.ResetEvent;
            break;
          end;
        if Event_Brk.WaitFor(0) <> wrTimeout then
          begin
            result := TMyWaitResult.enMWR_SetEvetBrk;
            break;
          end;
        result := ProcessMessages();
        if (result = enMWR_GetMsgExit) or (result = enMWR_SetTimout) then
          break;
      end;
  end;

function TOWait_Quit.WaitFor(mls: DWORD): TMyWaitResult;
  // var
  // Msg: TMsg;
  // fTimerStop:boolean;
  begin
    result := enMWR_Rest;
    SetDelay(mls);
    // fTimerStop:=idTimer=0;
    // result:=ProcessMessages();
    while true do
      begin
        result := ProcessMessages();
        if (result = enMWR_GetMsgExit) or (result = enMWR_SetTimout) then
          break;
      end;
  end;

function TOWait_Quit.ProcessMessages(): TMyWaitResult;
  var
    Msg: TMsg;
  begin
    result := enMWR_Rest;
    sleep(1);
    while PeekMessage(Msg, 0, 0, 0, PM_REMOVE) do
      begin
        case Msg.message of
          WM_QUIT:
            begin
              result := enMWR_GetMsgExit;
              PostMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
              break;
            end;
          WM_TIMER:
            begin
              if (UINT(Msg.wParam) = idTimer) and (idTimer <> 0) then
                begin
                  KillTimer(0, idTimer);
                  idTimer := 0;
                  result := enMWR_SetTimout;
                  break;
                end;
            end
        end;

        if (result <> enMWR_SetTimout) and (result <> enMWR_SetTimout) then
          begin
            TranslateMessage(Msg);
            DispatchMessage(Msg);
          end;
      end;
  end;

{ ****************************************************************************** }
end.
