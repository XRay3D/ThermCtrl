unit MdlLvlCom;

interface

uses
  Classes,
  SysUtils,
  Dialogs,
  windows,
  LowLvlCom;

type
  EnStateFile = (enCLOSE, enOPEN);

  EnpMode = (enModenil = $00, enModeRW = $06);
  EnATTRIBUTE = (enOnlyRead = $00, enOnlyWrit = $01, enRW = $02,
    enCreate = $10, enTrunc = $20, enCrtRW = $12, enCrtRWandTrunc = $32,
    O_APPEND = $08);

  TArrayOfByte = array of byte;

  TMdlLvlCom = class // (TObject)
    private
      LLCom: TLowLvlUAI;
    public
      CErr: boolean; // наличие критической ошибки
      sbSign: boolean; // работать с изменЄнными параметрами
      ErrCrF: boolean; // наличие критической ошибки
      ErrCrName: string; // описание критической ошибки
      function FindDev(var Add: byte): boolean;
      function GetTrase(var str: string): boolean;
      procedure SetVal(var id: rID); // @LLCom.Val.arr,LLCom.Val.n,Def,sign,strErr);
      function RdPar(ch: byte; var Def: rID; len: byte): boolean; overload;
      function RdPar(var Def: rID; len: byte): boolean; overload;

      function WrPar(var Def: rID; len: byte): boolean; overload;
      function WrPar(ch: byte; var Def: rID; len: byte): boolean; overload;
      function AttFile(IDFile: byte; mode: EnpMode): boolean;
      function DelFile(IDFile: byte): boolean;
      function TrRd(tDef: typeID; n: byte): EnErr;
      procedure SetCriticalErr(Err: EnErr);

      function OpenFile(IDFile: byte; Att: EnATTRIBUTE; mode: EnpMode): boolean;
      function CloseFile(IDFile: byte): boolean;
      function SeekFile(IDFile: byte; offset: dword; origin: byte): boolean;
      function TellFile(IDFile: byte; var offset: Integer): boolean;
      function Reset(): boolean;
      function WrFile(IDFile: byte; buffer: pointer; var len: byte): boolean;
      function RdFile(IDFile: byte; var buffer: pointer;
        var len: byte): boolean;
      function cmd1(ch: byte; var f: Single): boolean;
      function OldProtocol_RdNByte(AddEep, NByte: Integer;
        var str: AnsiString): boolean;
      constructor Create(var hPort: THandle; com, Spd: string; Add, tdev: byte);
      destructor Destroy; override;
      procedure ClosePort();
      procedure Set_TestMode(fTestMode: boolean);
      procedure NewSet(var hPort: THandle; NewPort: string; NewNAdd: byte);
  end;

implementation

constructor TMdlLvlCom.Create(var hPort: THandle; com, Spd: string;
  Add, tdev: byte);
  begin
    sbSign := false;
    LLCom := TLowLvlUAI.Create(hPort, com, Spd, Add, tdev);
    LLCom.SetTCom(EnBCUAI.UAI_LIGHT);
    inherited Create;
  end;

destructor TMdlLvlCom.Destroy;
  begin
    // LLCom.ClosePort;
    LLCom.fTestMode := false;
    LLCom.Free;
    inherited Destroy;
  end;

procedure TMdlLvlCom.Set_TestMode(fTestMode: boolean);
  begin
    LLCom.fTestMode := fTestMode;
  end;

function TMdlLvlCom.FindDev(var Add: byte): boolean;
  var
    fe: EnErr;
  begin
    result := false;
    LLCom.FindDev(Add);
    fe := LLCom.GetErr;
    ErrCrF := false;
    if (fe = OK) then
      begin
        Add := LLCom.Val.arr[0];
        result := true;
      end
    else
      begin
        if fe = EBUSY then
          begin
            ErrCrF := true;
            ErrCrName := ' омпорт зан€т!';
          end;
      end;
  end;

function TMdlLvlCom.GetTrase(var str: string): boolean;
  begin
    if LLCom.Listing.Count > 0 then
      begin
        str := LLCom.Listing.Strings[0];
        LLCom.Listing.Delete(0);
        GetTrase := true;
      end
    else
      begin
        LLCom.Listing.Clear;
        GetTrase := false;
      end;
  end;

// procedure TMdlLvlCom.SetVal(p);//@LLCom.Val.arr,LLCom.Val.n,Def,sign,strErr);
// begin
//
// end;

function TMdlLvlCom.TrRd(tDef: typeID; n: byte): EnErr;
  var
    rpt: byte; // количество повторных операций
    itWasBe: EnErr;
  begin
    itWasBe := OK;
    rpt := 1;

    while rpt > 0 do
      begin
        rpt := rpt - 1;
        LLCom.RdPar(tDef, n);

        case LLCom.GetErr of
          OK:
            begin
            end;
          EBUSY:
            begin // если ком порт зан€т
              if itWasBe <> EBUSY then
                rpt := 5;
            end;
          EUNDEFTYPE:
            begin // неподдерживаемый тип прибора
              if itWasBe <> EUNDEFTYPE then
                rpt := 0;
            end;
          EIMPOSSIBLE:
            begin // невозможное поле
              if itWasBe <> EIMPOSSIBLE then
                rpt := 1;
            end;
          EEXCEPT:
            begin // произола ошибка преобразовани
              if itWasBe <> EEXCEPT then
                rpt := 1;
            end;
          EFAIL:
            begin // завершение операции не привело к успеху
              if itWasBe <> EFAIL then
                rpt := 0;
            end;
          EDISASSEMBLE:
            begin //
              if itWasBe <> EDISASSEMBLE then
                rpt := 1;
            end;
          ETOT:
            begin // нет ответа
              if itWasBe <> ETOT then
                rpt := 1;
            end;

        end;
        itWasBe := LLCom.GetErr;
      end;
    SetCriticalErr(itWasBe);
    TrRd := itWasBe;
  end;

procedure TMdlLvlCom.SetCriticalErr(Err: EnErr);
  begin
    case Err of
      EBUSY:
        begin
          ErrCrF := true;
          ErrCrName := ' омпорт зан€т!';
        end;
      ETOT:
        begin
          ErrCrF := true;
          ErrCrName := 'Ќет ответа от прибора!';
        end;
      else
        begin
          ErrCrF := false;
          ErrCrName := '';
        end;
      end;
  end;

function TMdlLvlCom.RdPar(var Def: rID; len: byte): boolean;
// считать параметр
  begin
    result := RdPar(0, Def, len);
  end;

function TMdlLvlCom.RdPar(ch: byte; var Def: rID; len: byte): boolean;
  // считать параметр
  var
    // len:byte;
    id: typeID;
    sign: TSign;
    strErr: string;
  begin
    id.ch := ch;
    id.id := Def.id;
    // len := OperateID.GetLenVal(Def.TP);

    if sbSign then
      begin
        sign := Def.sign;
        if sign = SignTrue then
          begin
            result := true;
            exit;
          end;
      end;
    // LLCom.TpeID:=Def.TP;
    if (TrRd(id, len) = OK) and ((LLCom.Val.n = len) { or(Def.TP=eStr) } ) then
      begin
        // sign:=SignTrue;
        SetVal(Def);
        result := true;
      end
    else
      begin
        result := false;
        // sign:=SignErr;
        if LLCom.GetErr <> OK then
          if LLCom.GetErr = EFAIL then
            strErr := LLCom.getStrErrUAI
          else
            begin
              strErr := LLCom.getStrErr;
            end;
        CErr := true; // присутствие критической ошибки
      end;
    SetCriticalErr(LLCom.GetErr);
  end;

// @LLCom.Val.arr,LLCom.Val.n,Def,sign,strErr
procedure TMdlLvlCom.SetVal(var id: rID);
  var
    ptr: pointer;
  begin

    ptr := pointer(@LLCom.Val.arr[0]);

    case id.TP of
      eSingle:
        begin (PSingle(id.pVal))
          ^ := (PSingle(ptr))^;
        end;

      eIRT5502_AllCH:
        begin (PIRT5502_AllCHval(id.pVal))
          ^ := (PIRT5502_AllCHval(ptr))^;
        end;

      eByte:
        begin (PByte(id.pVal))
          ^ := (PByte(ptr))^;
        end;
    end;

  end;

function TMdlLvlCom.WrPar(ch: byte; var Def: rID; len: byte): boolean;
  // записать параметр
  var
    id: typeID;
    ptr: pointer;
    sign: TSign;
    strErr: string;
    // s:AnsiString;
  begin
    if sbSign then
      begin
        sign := Def.sign;
        if sign = SignTrue then // не пишитс€ если значение такоеже как и в приборе и приэтом
          begin // стоит галка проверки услови€
            result := true;
            exit;
          end;
      end;

    id.ch := ch;
    id.id := Def.id;
    result := false;
    CErr := false;
    sign := Def.sign;
    if sign = SignNIni then
      begin
        result := true;
        exit; // не пропишитс€ если параметры случайные
      end;
    ptr := Def.pVal;
    // len:=OperateID.GetLenVal(Def.TP);

    // LLCom.TpeID:=Def.TP;
    if LLCom.WrPar(id, ptr, len) then
      begin
        Def.sign := SignTrue;
        Def.strErr := LLCom.getStrErr;
        result := true;
      end
    else
      begin
        strErr := 'OK';
        if LLCom.GetErr <> OK then
          if LLCom.GetErr = EFAIL then
            strErr := LLCom.getStrErrUAI
          else
            begin
              strErr := LLCom.getStrErr;
            end;
        CErr := true; // присутствие критической ошибки
        Def.sign := SignErr;
        Def.strErr := strErr;
      end;
  end;

function TMdlLvlCom.WrPar(var Def: rID; len: byte): boolean;
  // записать параметр
  begin
    result := WrPar(0, Def, len);
  end;

function TMdlLvlCom.AttFile(IDFile: byte; mode: EnpMode): boolean;
  begin
    if (LLCom.AttFile(IDFile, byte(mode))) then
      AttFile := true
    else
      AttFile := false;
  end;

function TMdlLvlCom.DelFile(IDFile: byte): boolean;
  begin
    if (LLCom.DelFile(IDFile)) then
      DelFile := true
    else
      DelFile := false;
    // LLCom.ClosePort;
  end;

function TMdlLvlCom.OpenFile(IDFile: byte; Att: EnATTRIBUTE;
  mode: EnpMode): boolean;
  begin
    if (LLCom.OpenFile(IDFile, byte(Att), byte(mode))) then
      OpenFile := true
    else
      OpenFile := false;
    // LLCom.ClosePort;
  end;

function TMdlLvlCom.CloseFile(IDFile: byte): boolean;
  begin
    if (LLCom.CloseFile(IDFile)) then
      CloseFile := true
    else
      CloseFile := false;
    // LLCom.ClosePort;
  end;

function TMdlLvlCom.SeekFile(IDFile: byte; offset: dword;
  origin: byte): boolean;
  begin
    if (LLCom.SeekFile(IDFile, offset, origin)) then
      SeekFile := true
    else
      SeekFile := false;
    // LLCom.ClosePort;
  end;

function TMdlLvlCom.TellFile(IDFile: byte; var offset: Longint): boolean;
  begin
    if (LLCom.TellFile(IDFile)) then
      begin
        TellFile := true;
        offset := PLongint(@LLCom.Val.arr[0])^;
      end
    else
      TellFile := false;
    // LLCom.ClosePort;
  end;

function TMdlLvlCom.Reset(): boolean;
  begin
    result := LLCom.ResetDev();
  end;

function TMdlLvlCom.cmd1(ch: byte; var f: Single): boolean;
  begin
    if (LLCom.cmd1(ch)) then
      begin
        f := (PSingle(@LLCom.Val.arr))^;
        cmd1 := true;
      end
    else
      cmd1 := false;
  end;

function TMdlLvlCom.OldProtocol_RdNByte(AddEep, NByte: Integer;
  var str: AnsiString): boolean;
  var
    i, j: Integer;
  begin
    OldProtocol_RdNByte := false;
    if LLCom.OldProtocol_SetAddEEp(AddEep) then
      begin
        if LLCom.OldProtocol_ReadNByteEEp(NByte) then
          begin
            setLength(str, LLCom.Val.n);
            j := 1;
            for i := 0 to LLCom.Val.n - 1 do
              begin
                str[i + 1] := AnsiChar(LLCom.Val.arr[LLCom.Val.n - 1 - i - j]);
                if j < 0 then
                  j := 1
                else
                  j := -1;
              end;
            OldProtocol_RdNByte := true;
          end;
      end;
  end;

function TMdlLvlCom.RdFile(IDFile: byte; var buffer: pointer;
  var len: byte): boolean;
  begin
    buffer := @(LLCom.Val.arr[0]);
    if (LLCom.RdFile(IDFile, len)) then
      RdFile := true
    else
      RdFile := false;
  end;

function TMdlLvlCom.WrFile(IDFile: byte; buffer: pointer;
  var len: byte): boolean;
  begin
    if (LLCom.WrFile(IDFile, buffer, len)) then
      WrFile := true
    else
      WrFile := false;
  end;

procedure TMdlLvlCom.ClosePort();
  begin
    LLCom.ClosePort;
  end;

procedure TMdlLvlCom.NewSet(var hPort: THandle; NewPort: string; NewNAdd: byte);
  begin
    LLCom.NewSettingComm(hPort, NewPort, NewNAdd);
  end;

end.
