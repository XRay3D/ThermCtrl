unit ThHCC;

interface

uses
  Windows, Messages, Math, SysUtils, Variants, Classes, Graphics, Controls, Forms, Dialogs, Menus, RzButton, ExtCtrls, StdCtrls, Grids, BaseGrid, AdvGrid, SyncObjs, LowLvlCom, MdlLvlCom;

const
  wm_MymessageERR = WM_USER + 0;

type

  THCC_UN = (enHCC_UN_Mthr, // enHCC_UN_Wr, // enHCC_UN_Rd, enHCC_UN_Find, enHCC_UN_GetT, enHCC_UN_PwrOn, enHCC_UN_PwrOff);

  HCC_D_REG = (en_DREG_TEMP_PV = 0001, // текущая температура
    en_DREG_TEMP_SP = 0002, // установленная температура
    en_DREG_STATUS_MODE = 0101, en_DREG_OP_MODE = 0104, en_DREG_PWR_MODE = 0105);
  HCC_STATUS_MODE = (en_RUN = 1, en_HOLD = 2, en_STEP = 3, en_STOP = 4);
  HCC_OP_MODE = (PROG = 0, FIX = 1);
  HCC_PWR_MODE = (COLD = 0, HOT = 1);

  HCC_TYPE = (T_ABSENT, T_T6800, T_IRT5502, T_TMETER);

  TOnMeasuring = procedure(val: single) of object;
  TOnAddListTrase = procedure(s: string) of object;

  rInterfse_IRT5502 = record // термокамера с контроллером IRT5502
    add: integer;
    spd: string;
  end;

  TRcd_HCC = record
    IRT5502: rInterfse_IRT5502;
    connect: boolean;
    typ: HCC_TYPE;
    com: string;
    add: string;
    spd: string;
    hPort: THandle;
    fTestMode: boolean;
    mtxCom: THandle;
    criticalHandle: HWND;
    OnListTrase: TOnAddListTrase;
  end;

  TPRcd_HCC = ^TRcd_HCC;

  THCC_T6800_RW = class(TObject) // применяется при поиске приборов
    protected
      ID: rID;

      Lvl_PCL0: TLLvl_PCL0;
      Data: TPCL0_Data;
      sTr: string;
      Thr: TThread;
      procedure ListTrase();
    public
      Regs: TPCL0_Regs;
      OnListTrase: TOnAddListTrase;
      procedure AddRegs(var Regs: TPCL0_Regs; add, val: UInt16); overload;
      procedure AddRegs(var Regs: TPCL0_Regs; add: UInt16); overload;
      procedure ClrRegs(var Regs: TPCL0_Regs);
      function RRD(var Regs: TPCL0_Regs): EnErr;
      function WRD(var Regs: TPCL0_Regs): EnErr;
      // procedure PrtCriticalMessage(msg:AnsiString;criticalHandle:HWND);

      constructor Create(Thr: TThread; var hPort: THandle; com, spd: string;
        add: byte; fTestMode: boolean); overload;
      destructor Destroy; override;
  end;

  THCC_IRT5502_RW = class(TObject)
    protected
      spd: string;
      add: byte;
      MdLvl: TMdlLvlCom;
      sTr: string;
      Thr: TThread;
      procedure ListTrase();
    public
      OnListTrase: TOnAddListTrase;
      function Find(): boolean;
      constructor Create(Thr: TThread; var hPort: THandle; com, spd: string;
        add: byte; fTestMode: boolean); overload;
      destructor Destroy; override;
  end;

  ThHCC_UN = class(TThread) // применяется при измерениях (поверка градуировка и т.д.)
    protected
      fix_T: single;
      test_Tag: single;
      HCC_T6800_RW: THCC_T6800_RW;
      HCC_IRT5502_RW: THCC_IRT5502_RW;
      // fStatus:boolean;
      PRcd_HCC: TPRcd_HCC; // handle HCC

      OnMeasuring: TOnMeasuring;
      // OnGetStatus:TOnGetStatus;
      fPause: boolean;
      valAll: rIRT5502_AllCH;
      valByte: rByte;
      valSingle: rSingle;

      HCC_UN: THCC_UN;
      procedure Operate;
      // procedure GetStatus();
      procedure WaitForPause;
      procedure GetFPause();
    public
      run_T: single;
      connect: boolean;
      OEvt_Pause: TEvent;
      OEvt: TEvent;
      OnGetFPause: function(): boolean of Object;

      constructor CreateMthr(var rcd_HCC: TRcd_HCC
        { ;OnGetStatus:TOnGetStatus } ; OnMeasuring: TOnMeasuring); overload;
      constructor CreatePwOn(var rcd_HCC: TRcd_HCC; fix_T: single); overload;
      constructor CreateGetT(var rcd_HCC: TRcd_HCC);
      constructor CreatePwOff(var rcd_HCC: TRcd_HCC); overload;
      // constructor CreateWr      (var rcd_HCC:TRcd_HCC;Regs:TPCL0_Regs); overload;
      // constructor CreateRd      (var rcd_HCC:TRcd_HCC;Regs:TPCL0_Regs); overload;
      constructor CreateFind(var rcd_HCC: TRcd_HCC); overload;
      procedure Execute; override;
      procedure PrtCriticalMessage(msg: string; criticalHandle: HWND);
      destructor Destroy; override;

  end;

implementation

// uses Main;

const
  TOUT_CPORT_BUSY = 6000;

  { ****************************************************************************** }
destructor THCC_IRT5502_RW.Destroy;
  begin
    if Assigned(MdLvl) then
      MdLvl.Free;
    inherited Destroy;
  end;

constructor THCC_IRT5502_RW.Create(Thr: TThread; var hPort: THandle;
  com, spd: string; add: byte; fTestMode: boolean);
  begin
    self.spd := spd;
    self.add := add;
    MdLvl := TMdlLvlCom.Create(hPort, com, spd, add, 54);
    self.Thr := Thr;
    inherited Create;
  end;

procedure THCC_IRT5502_RW.ListTrase();
  begin
    if Assigned(OnListTrase) then
      OnListTrase(sTr);
  end;

{ procedure THCC_IRT5502_RW.PrtCriticalMessage(msg:AnsiString;criticalHandle:HWND);
  var
  At:word;
  begin
  At:=GlobalAddAtom(PWideChar(string(msg)));
  PostMessage(criticalHandle,WM_USER + 0,At,0);
  end; }

function THCC_IRT5502_RW.Find(): boolean;
  begin
    result := MdLvl.FindDev(self.add);
  end;

{ ****************************************************************************** }

destructor THCC_T6800_RW.Destroy;
  begin
    if Assigned(Lvl_PCL0) then
      Lvl_PCL0.Free;
    inherited Destroy;
  end;

constructor THCC_T6800_RW.Create(Thr: TThread; var hPort: THandle;
  com, spd: string; add: byte; fTestMode: boolean);
  begin
    Lvl_PCL0 := TLLvl_PCL0.Create(hPort, com, spd, add);
    Lvl_PCL0.fTestMode := fTestMode;
    ClrRegs(Regs);
    self.Thr := Thr;
    inherited Create;
  end;

procedure THCC_T6800_RW.AddRegs(var Regs: TPCL0_Regs; add: UInt16);
  begin
    SetLength(Regs, Length(Regs) + 1);
    Regs[Length(Regs) - 1].add := add;
  end;

procedure THCC_T6800_RW.AddRegs(var Regs: TPCL0_Regs; add, val: UInt16);
  begin
    SetLength(Regs, Length(Regs) + 1);
    Regs[Length(Regs) - 1].add := add;
    Regs[Length(Regs) - 1].val := val;
  end;

procedure THCC_T6800_RW.ClrRegs(var Regs: TPCL0_Regs);
  begin
    SetLength(Regs, 0);
  end;

procedure THCC_T6800_RW.ListTrase();
  begin
    if Assigned(OnListTrase) then
      OnListTrase(sTr);
  end;

function THCC_T6800_RW.RRD(var Regs: TPCL0_Regs): EnErr;
  begin
    result := Lvl_PCL0.Cmd_RRD(Regs);
  end;

function THCC_T6800_RW.WRD(var Regs: TPCL0_Regs): EnErr;
  begin
    result := Lvl_PCL0.Cmd_WRD(Regs);
  end;

{ ****************************************************************************** }

constructor ThHCC_UN.CreateMthr(var rcd_HCC: TRcd_HCC
  { ;OnGetStatus:TOnGetStatus } ; OnMeasuring: TOnMeasuring);
  begin
    inherited Create(true);
    OEvt := TEvent.Create(false);

    FreeOnTerminate := true;
    self.PRcd_HCC := @rcd_HCC;
    // self.OnGetStatus:=OnGetStatus;
    self.OnMeasuring := OnMeasuring;
    self.HCC_UN := THCC_UN.enHCC_UN_Mthr;

    valAll.ID.pVal := @valAll.val;
    valAll.ID.ID := $00FF;
    valAll.ID.TP := eIRT5502_AllCH;
    valAll.ID.sign := SignChng;
  end;

constructor ThHCC_UN.CreatePwOn(var rcd_HCC: TRcd_HCC; fix_T: single);
  begin
    inherited Create(true);
    FreeOnTerminate := true;
    OEvt := TEvent.Create(false);

    self.PRcd_HCC := @rcd_HCC;
    self.fix_T := fix_T;
    self.HCC_UN := THCC_UN.enHCC_UN_PwrOn;

    valSingle.val := fix_T;
    valSingle.ID.pVal := @valSingle.val;
    valSingle.ID.ID := $DA66;
    valSingle.ID.TP := eSingle;
    valSingle.ID.sign := SignChng;

    valByte.val := $01;
    valByte.ID.pVal := @valByte.val;
    valByte.ID.ID := $DA65;
    valByte.ID.TP := eByte;
    valByte.ID.sign := SignChng;
  end;

constructor ThHCC_UN.CreatePwOff(var rcd_HCC: TRcd_HCC);
  begin
    inherited Create(true);
    OEvt := TEvent.Create(false);

    FreeOnTerminate := true;
    self.PRcd_HCC := @rcd_HCC;
    self.HCC_UN := THCC_UN.enHCC_UN_PwrOff;

    valByte.val := $00;
    valByte.ID.pVal := @valByte.val;
    valByte.ID.ID := $DA65;
    valByte.ID.TP := eByte;
    valByte.ID.sign := SignChng;
  end;

constructor ThHCC_UN.CreateGetT(var rcd_HCC: TRcd_HCC);
  begin
    inherited Create(true);
    OnTerminate := nil;
    FreeOnTerminate := false;
    OEvt := TEvent.Create(false);
    self.PRcd_HCC := @rcd_HCC;
    self.HCC_UN := THCC_UN.enHCC_UN_GetT;

    valAll.ID.pVal := @valAll.val;
    valAll.ID.ID := $00FF;
    valAll.ID.TP := eIRT5502_AllCH;
    valAll.ID.sign := SignChng;

  end;

constructor ThHCC_UN.CreateFind(var rcd_HCC: TRcd_HCC);
  begin
    inherited Create(true);
    FreeOnTerminate := true;
    OEvt := TEvent.Create(false);
    self.PRcd_HCC := @rcd_HCC;
    self.HCC_UN := THCC_UN.enHCC_UN_Find;
  end;

destructor ThHCC_UN.Destroy;
  begin
    inherited Destroy;
  end;

procedure ThHCC_UN.WaitForPause;
  begin
    Synchronize(GetFPause);
    while (self.fPause) and (not Terminated) do
      begin
        sleep(500);
        Synchronize(GetFPause);
      end;
  end;

procedure ThHCC_UN.GetFPause();
  begin
    self.fPause := false;
    if Assigned(self.OnGetFPause) then
      self.fPause := self.OnGetFPause;
  end;

procedure ThHCC_UN.Execute;
    procedure UpDateRunT_T6800;
      begin
        run_T := self.HCC_T6800_RW.Regs[0].val;

        if self.HCC_T6800_RW.Lvl_PCL0.fTestMode then
          begin
            run_T := ((Tan(test_Tag / 100 - 2) + 11.5) * 2 +
                  (random / 10 - 0.05));
            test_Tag := test_Tag + 10;
          end
        else
          run_T := run_T / 10;
      end;

    procedure function_IRT5502();
      var
        rc: boolean;
      begin
        self.HCC_IRT5502_RW := THCC_IRT5502_RW.Create(self, PRcd_HCC.hPort, PRcd_HCC.com, PRcd_HCC.IRT5502.spd, PRcd_HCC.IRT5502.add, PRcd_HCC.fTestMode);
        self.HCC_IRT5502_RW.OnListTrase := PRcd_HCC.OnListTrase;
        try
          case self.HCC_UN of
            enHCC_UN_Mthr:
              begin
                ReleaseMutex(PRcd_HCC.mtxCom);
                while not self.Terminated do
                  begin
                    // Synchronize(GetStatus);
                    // if self.fStatus then
                    // begin
                    if WaitForSingleObject(PRcd_HCC.mtxCom, TOUT_CPORT_BUSY) <> WAIT_TIMEOUT then
                      begin
                        rc := self.HCC_IRT5502_RW.MdLvl.RdPar(self.valAll.ID, sizeof(valAll.val));
                        ReleaseMutex(PRcd_HCC.mtxCom);
                        if rc then
                          begin
                            self.run_T := self.valAll.val.ch1val;
                            Synchronize(Operate);
                          end;
                      end
                    else
                      begin
                        // HCC_T6800_RW.PrtCriticalMessage('ThHCC_UN'+rcd_HCC.com+' порт занят',self.rcd_HCC.criticalHandle);
                      end;
                    // end;
                    // HCC_IRT5502_RW.MdLvl.ClosePort;
                    sleep(1200);
                    WaitForPause();
                  end;
              end;
            enHCC_UN_Find:
              begin
                // Мay be need close port ONLE if not connection ?
                connect := HCC_IRT5502_RW.Find();
              end;
            enHCC_UN_GetT:
              begin
                self.HCC_IRT5502_RW.MdLvl.RdPar(self.valAll.ID, sizeof(valAll.val));
                self.run_T := self.valAll.val.ch1val;
              end;
            enHCC_UN_PwrOn:
              begin
                self.HCC_IRT5502_RW.MdLvl.WrPar($01, self.valSingle.ID, sizeof(valSingle.val));
                self.HCC_IRT5502_RW.MdLvl.WrPar($01, self.valByte.ID, sizeof(valByte.val));
              end;
            enHCC_UN_PwrOff:
              begin
                self.HCC_IRT5502_RW.MdLvl.WrPar($01, self.valByte.ID, sizeof(valByte.val));
              end;
          end;
        finally
          HCC_IRT5502_RW.MdLvl.ClosePort;
          self.HCC_IRT5502_RW.Free;
        end;
      end;

    procedure function_T6800();
      begin
        HCC_T6800_RW := THCC_T6800_RW.Create(self, PRcd_HCC.hPort, PRcd_HCC.com, '9600', 1, PRcd_HCC.fTestMode);
        HCC_T6800_RW.OnListTrase := PRcd_HCC.OnListTrase;
        try
          case self.HCC_UN of
            enHCC_UN_Mthr:
              begin
                ReleaseMutex(PRcd_HCC.mtxCom);
                HCC_T6800_RW.ClrRegs(HCC_T6800_RW.Regs);
                HCC_T6800_RW.AddRegs(HCC_T6800_RW.Regs, ord(en_DREG_TEMP_PV));
                while not self.Terminated do
                  begin
                    // Synchronize(GetStatus);
                    // if self.fStatus then
                    // begin
                    if WaitForSingleObject(PRcd_HCC.mtxCom, TOUT_CPORT_BUSY)
                        <> WAIT_TIMEOUT then
                      begin
                        if HCC_T6800_RW.RRD(HCC_T6800_RW.Regs) = OK then
                          begin
                            UpDateRunT_T6800();
                            Synchronize(Operate);
                          end;
                        ReleaseMutex(PRcd_HCC.mtxCom);
                      end
                    else
                      begin
                        // HCC_T6800_RW.PrtCriticalMessage('ThHCC_UN'+rcd_HCC.com+' порт занят',self.rcd_HCC.criticalHandle);
                      end;
                    // end;
                    sleep(1000);
                    WaitForPause();
                  end;
              end;
            enHCC_UN_GetT:
              begin
                run_T := -5000;
                HCC_T6800_RW.AddRegs(HCC_T6800_RW.Regs, ord(en_DREG_TEMP_PV));
                if HCC_T6800_RW.RRD(HCC_T6800_RW.Regs) = OK then
                  begin
                    run_T := self.HCC_T6800_RW.Regs[0].val;
                    run_T := run_T / 10;
                  end
                else
                  run_T := -1000;
              end;
            enHCC_UN_Find:
              begin
                connect := false;
                HCC_T6800_RW.AddRegs(HCC_T6800_RW.Regs, ord(en_DREG_STATUS_MODE));
                if HCC_T6800_RW.RRD(HCC_T6800_RW.Regs) = OK then
                  begin
                    connect := true;
                  end
              end;
            enHCC_UN_PwrOn: // включение термокамеры
              begin
                HCC_T6800_RW.AddRegs(HCC_T6800_RW.Regs, ord(en_DREG_STATUS_MODE), ord(en_STOP));
                HCC_T6800_RW.WRD(HCC_T6800_RW.Regs);

                HCC_T6800_RW.ClrRegs(HCC_T6800_RW.Regs);
                HCC_T6800_RW.AddRegs(HCC_T6800_RW.Regs, ord(0102), trunc(fix_T * 10));
                HCC_T6800_RW.WRD(HCC_T6800_RW.Regs);
                sleep(1000);

                HCC_T6800_RW.ClrRegs(HCC_T6800_RW.Regs);
                HCC_T6800_RW.AddRegs(HCC_T6800_RW.Regs, ord(en_DREG_STATUS_MODE), ord(en_RUN));
                HCC_T6800_RW.WRD(HCC_T6800_RW.Regs);
              end;
            enHCC_UN_PwrOff:
              begin
                HCC_T6800_RW.AddRegs(HCC_T6800_RW.Regs, ord(en_DREG_STATUS_MODE), ord(en_STOP));
                HCC_T6800_RW.WRD(HCC_T6800_RW.Regs);
              end;
          end;
        finally
          HCC_T6800_RW.Lvl_PCL0.ClosePort;
          HCC_T6800_RW.Free;
        end;
      end;

  begin
    OEvt.ResetEvent;
    randomize();
    test_Tag := 100;
    // ждём когда освободится компорт
    if WaitForSingleObject(PRcd_HCC.mtxCom, 5000) <> WAIT_TIMEOUT then
      begin
        case self.PRcd_HCC.typ of
          T_T6800:
            begin
              function_T6800();
            end;
          T_IRT5502:
            begin
              function_IRT5502();
            end;
        end;
        ReleaseMutex(PRcd_HCC.mtxCom);
      end
    else // возвращает ошибки устройства
      PrtCriticalMessage('ThHCC_UN' + PRcd_HCC.com + ' порт занят', self.PRcd_HCC.criticalHandle);
    OEvt.SetEvent();
    OEvt.Free;
  end;

procedure ThHCC_UN.PrtCriticalMessage(msg: string; criticalHandle: HWND);
  var
    At: word;
  begin
    At := GlobalAddAtom(PWideChar(string(msg)));
    PostMessage(criticalHandle, wm_MymessageERR, At, 0);
  end;

procedure ThHCC_UN.Operate;
  begin
    OnMeasuring(run_T);
  end;

// procedure ThHCC_UN.GetStatus();
// begin if @OnGetStatus<>nil then self.OnGetStatus(fStatus) else fStatus:=true;
// end;

end.
