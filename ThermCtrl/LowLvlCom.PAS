unit LowLvlCom;

interface

uses
  Classes,
  SysUtils,
  Dialogs,
  windows;

type
  EnType = (eByte, eSingle, eIRT5502_AllCH);
  TenRW = (enRdWr = 0, enOnlyRd = 1, enOnlyWr = 2, enNotRdWr = 3);
  TSign = (SignNIni = 0, SignTrue = 1, SignChng = 2, SignErr = 255);

  rID = record
    pVal: pointer;
    RW: TenRW;
    id: Word;
    TP: EnType;
    Name: String;
    hint: String;
    represent: BYTE; // Представление 0-отсутствует 1-combobox 2-checkBox
    // List: aList; // набор параметров для визуализации
    sign: TSign; // достоверность /цвет
    strErr: string; // код ошибки
    // Hart: rIDHart;
  end;

  TPID = ^rID;

  rByte = record
    val: UInt8; // значение
    id: rID;
  end;

  rSingle = record
    val: single; // значение
    id: rID;
  end;

  rIRT5502_AllCHval = Packed record
    bitRele: BYTE;
    bitUst1: BYTE;
    bitUst2: BYTE;
    bitUst3: BYTE;
    bitPID: BYTE;
    ch1err: BYTE;
    ch1val: single;
    ch2err: BYTE;
    ch2val: single;
    ch3err: BYTE;
    ch3val: single;
    bitInput: BYTE;
  end;

  PIRT5502_AllCHval = ^rIRT5502_AllCHval;

  rIRT5502_AllCH = Packed record
    val: rIRT5502_AllCHval;
    id: rID;
  end;

  TFlt = record
    case Boolean of
      True:
        (ValFlt: single);
      False:
        (ValInt: Integer);
  end;

  typeID = record
    Ch: BYTE;
    id: Word;
  end;

  ArrayMass = array [0 .. 250] of BYTE;

  rAddVal = record
    add: Int16;
    val: Int16;
  end;

  TRxTxBuff = array [0 .. 250] of AnsiChar;

  TPCL0_Regs = array of rAddVal;
  TPCL0_Data = array of Int16;

  EnBCUAI = (UNDEF = 2, UAI_LIGHT = 1, UAI_FULL = 0);

  EnBCRet = (EZERO = 0, // успешное завершение.
    ENOCMD = 1, // команда не найдена.
    EINVALCMD = 2, // неверный формат команды.
    ENOPAR = 3, // параметр не найден.
    EPERM = 4, // нарушение прав доступа.
    EACCESS = 5, // ошибка чтения (в том числе при несовпадении CRC) и записи параметра (параметр только для чтения, область данных хранения параметра защищена от записи).
    ERANGE = 6, // выход параметра за границу диапазона
    ENOENT = 7, // файл не найден
    ENOMEM = 8, // нехватает памяти.
    EBADF = 9, // недопустимый идентификатор файла или идентификатор не ссылается на открытый файл.
    ENOSPC = 10, // отсутствует место на физическом носителе.
    EUNDEF = 11, EFALSE = 255);
  EnBCStat = (Success = 0, Pass = 1, Failure = 2, Unsuccessful = 3);
  EnErr = (OK = 0, EBUSY = 1, // если ком порт занят
    EUNDEFTYPE = 2, // неподдерживаемый тип прибора
    EIMPOSSIBLE = 3, // невозможное поле
    EEXCEPT = 5, // произола ошибка преобразовани
    EPROTOCOL = 6, // неподдерживаемый протокол
    EFAIL = 7, // завершение операции не привело к успеху
    EDISASSEMBLE = 8, // ошибка преобразования
    ETOT = 9 // нет ответа
      );

  TPCL0_CMD = (enPCL0_CMD_RSD, enPCL0_CMD_RRD, enPCL0_CMD_WSD, enPCL0_CMD_WRD,
    enPCL0_CMD_STD, enPCL0_CMD_CLD);

  // TPHandle=^THandle;
  TProtocolNet_PhisicalLvl = class // (TObject)
    private
      dcb: TDCB;
      ct: COMMTIMEOUTS;
      signBeginRx, signBeginTx, signEndRx: AnsiChar;
      fEnCRC: Boolean;
      NCom: string; // номер порта
      NSpd: string; // скорость порта
      NAdd: BYTE; // сетевой адрес
      NStopBit: BYTE; // сетевой адрес

      phPort: ^THandle;
      buffRx: AnsiString; // строка вывода
      OnMessPreview: function(bRx: AnsiString; len: Integer): Boolean of Object;
      // OnCreatParcel : function (str: AnsiString): AnsiString of Object;

      semi: array [0 .. 10] of BYTE; // позиционированние знаков ';' в buffRx
      nSemi: BYTE; // количество точек с запятыми
      IndEnd, IndStart: BYTE;

      function MessPreview(bRx: AnsiString; len: Integer): Boolean;
      function CreatParcelASCII(str: AnsiString): AnsiString;
      function Transaction(TxStr: AnsiString; TOut: Word): EnErr;

    public
      Listing: TStrings;

      function TransactionMBUS(TxMass: ArrayMass; TxLen: Integer;
        TOut: Word): EnErr;
      function TransactionLL(TxBuff: TRxTxBuff; TxLen: Integer;
        TOut: Word): EnErr;

      function CalcCRC16Table(str: AnsiString; Lenght: BYTE): Word;
      function CalcCRCMBUS(PMass: PByte; Lenght: BYTE): Word;
      procedure NewSettingComm(var hPort: THandle; NCom: string; NAdd: BYTE);
      procedure SetNAdd(NAdd: BYTE);
      function GetNAdd(): BYTE;
      procedure SetNSpd(NSpd: string);
      function GetNSpd(): string;

      function OpenComPort(): Boolean;
      procedure ClosePort;
      procedure AddList(strWho: string; strWhat: string);
      procedure AddLstMess(strWhat: string);
      procedure StrToAnsi(var AStr: AnsiString; var str: string);
      procedure AnsiToStr(var str: string; var AStr: AnsiString);
      function IntToAnsi(i: Integer): AnsiString;

      constructor Create(var hPort: THandle; com, spd: string; add: BYTE;
        signBeginTx, signBeginRx, signEndRx: AnsiChar; fEnCRC: Boolean);
      destructor Destroy; override;
  end;

  TLowLvlUAI = class(TProtocolNet_PhisicalLvl)
    private
      Err: EnErr;

      BC: record TCom: EnBCUAI; // тип комманды двухтакт/однатактна
      RW: BYTE; // комманда чтения или записи
      NCmd: BYTE; // номер комманды
      TDev: BYTE; // тип прибора
      Stat: EnBCStat; // состояние регистра статуса
      CRet: EnBCRet; // код ошибки возврата
      fVal: real; // результат комманды 'cmd1'
  end;

function isxdigit_M(b: AnsiChar): BYTE;
procedure HexToStr(pHex: PByte; pStr: PAnsiChar; len: BYTE);
function StrToHex_M(str: AnsiString; var ptrByte: ArrayMass): Boolean;
function ECodReturn(pErr: PAnsiChar): EnBCRet;
function EnNameBCStat(var v: EnBCStat; i: BYTE): Boolean;
function EnNameBCUAI(var v: EnBCUAI; i: BYTE): Boolean;
// procedure EnNameBCRet(var v: EnBCRet;i:byte);
function UAI(cmd: BYTE; buffTx: AnsiString): EnErr;

public

  val: record // принятые данные,выделенные из buffRx и преобразов в численный вид
      arr: ArrayMass; // данные
  n: BYTE; // размер данных
  end;
  fTestMode: Boolean;
  // TpeID:EnType;

  function GetErr(): EnErr;
  function FindDev(add: BYTE): Boolean;
  function ResetDev(): Boolean;
  // function SetSpd(NSpd:AnsiString):boolean;
  function SetAdd(NAdd: BYTE): Boolean;
  procedure SetTCom(TCom: EnBCUAI);
  function CMD1(Ch: BYTE): Boolean;
  function OldProtocol_SetAddEEp(add: BYTE): Boolean;
  function OldProtocol_ReadNByteEEp(NByte: BYTE): Boolean;
  function RdPar(id: typeID; var len: BYTE): Boolean;
  function WrPar(id: typeID; pDat: PByte; var len: BYTE): Boolean;
  function OpenFile(FileName: BYTE; FLAG, mode: BYTE): Boolean;
  function CloseFile(FileName: BYTE): Boolean;
  function SeekFile(FileName: BYTE; offset: dword; origin: BYTE): Boolean;
  function AttFile(FileName: BYTE; mode: BYTE): Boolean;
  function DelFile(FileName: BYTE): Boolean;
  function TellFile(FileName: BYTE): Boolean;
  function RdFile(FileName: BYTE; var len: BYTE): Boolean;
  function WrFile(FileName: BYTE; pDat: pointer; var len: BYTE): Boolean;
  function getStrErr: string;
  function getStrErrUAI: string;
  procedure StrToArrayMass(s: AnsiString);

  // function CreatResponceMessage():AnsiString;
  constructor Create(var hPort: THandle; com, spd: string; add, TDev: BYTE);
  destructor Destroy;
  override;
  end;

  TLLvl_PCL0 = class(TProtocolNet_PhisicalLvl)private function Transaction
      (str: AnsiString; TOut: Word; fTestMode: Boolean;
    TCMD: TPCL0_CMD): EnErr;
  overload;
public
  Date: TPCL0_Data;
  fTestMode: Boolean;
  function Cmd_RRD(var Regs: TPCL0_Regs): EnErr;
  function Cmd_WRD(var Regs: TPCL0_Regs): EnErr;
  function Cmd_WSD(AddDReg: Integer; var VReg: TPCL0_Data): EnErr;
  function GetListing(var str: string): Boolean;

  constructor Create(var hPort: THandle; com, spd: string; add: BYTE);
  destructor Destroy;
  override;
  end;

  TLLvl_MBUS = class(TProtocolNet_PhisicalLvl)private add: BYTE;
  cmd: BYTE;
  regAdd: Integer;
  lenRxData: Integer;
  lenRxTotal: Integer;
  Err: BYTE;
  data: ArrayMass;
  function MessPreview(bRx: AnsiString; len: Integer): Boolean;
  overload;
public
  function Cmd_03(regAdd: Word; var len: Word; PReg: PByte): BYTE;
  function Cmd_16(regAdd: Word; var len: Word; PReg: PByte): BYTE;
  procedure SetNAdd(NAdd: BYTE);
  overload;

  constructor Create(var hPort: THandle; com, spd: string; add: BYTE);
  destructor Destroy;
  override;
  end;

implementation

const
  tableCRC16Hi: array [0 .. 255] of BYTE = ($00, $C1, $81, $40, $01, $C0, $80,
    $41, $01, $C0, $80, $41, $00, $C1, $81, $40, $01, $C0, $80, $41, $00, $C1,
    $81, $40, $00, $C1, $81, $40, $01, $C0, $80, $41, $01, $C0, $80, $41, $00,
    $C1, $81, $40, $00, $C1, $81, $40, $01, $C0, $80, $41, $00, $C1, $81, $40,
    $01, $C0, $80, $41, $01, $C0, $80, $41, $00, $C1, $81, $40, $01, $C0, $80,
    $41, $00, $C1, $81, $40, $00, $C1, $81, $40, $01, $C0, $80, $41, $00, $C1,
    $81, $40, $01, $C0, $80, $41, $01, $C0, $80, $41, $00, $C1, $81, $40, $00,
    $C1, $81, $40, $01, $C0, $80, $41, $01, $C0, $80, $41, $00, $C1, $81, $40,
    $01, $C0, $80, $41, $00, $C1, $81, $40, $00, $C1, $81, $40, $01, $C0, $80,
    $41, $01, $C0, $80, $41, $00, $C1, $81, $40, $00, $C1, $81, $40, $01, $C0,
    $80, $41, $00, $C1, $81, $40, $01, $C0, $80, $41, $01, $C0, $80, $41, $00,
    $C1, $81, $40, $00, $C1, $81, $40, $01, $C0, $80, $41, $01, $C0, $80, $41,
    $00, $C1, $81, $40, $01, $C0, $80, $41, $00, $C1, $81, $40, $00, $C1, $81,
    $40, $01, $C0, $80, $41, $00, $C1, $81, $40, $01, $C0, $80, $41, $01, $C0,
    $80, $41, $00, $C1, $81, $40, $01, $C0, $80, $41, $00, $C1, $81, $40, $00,
    $C1, $81, $40, $01, $C0, $80, $41, $01, $C0, $80, $41, $00, $C1, $81, $40,
    $00, $C1, $81, $40, $01, $C0, $80, $41, $00, $C1, $81, $40, $01, $C0, $80,
    $41, $01, $C0, $80, $41, $00, $C1, $81, $40);
  { Table of CRC values for low–order byte }
  tableCRC16Lo: array [0 .. 255] of BYTE = ($00, $C0, $C1, $01, $C3, $03, $02,
    $C2, $C6, $06, $07, $C7, $05, $C5, $C4, $04, $CC, $0C, $0D, $CD, $0F, $CF,
    $CE, $0E, $0A, $CA, $CB, $0B, $C9, $09, $08, $C8, $D8, $18, $19, $D9, $1B,
    $DB, $DA, $1A, $1E, $DE, $DF, $1F, $DD, $1D, $1C, $DC, $14, $D4, $D5, $15,
    $D7, $17, $16, $D6, $D2, $12, $13, $D3, $11, $D1, $D0, $10, $F0, $30, $31,
    $F1, $33, $F3, $F2, $32, $36, $F6, $F7, $37, $F5, $35, $34, $F4, $3C, $FC,
    $FD, $3D, $FF, $3F, $3E, $FE, $FA, $3A, $3B, $FB, $39, $F9, $F8, $38, $28,
    $E8, $E9, $29, $EB, $2B, $2A, $EA, $EE, $2E, $2F, $EF, $2D, $ED, $EC, $2C,
    $E4, $24, $25, $E5, $27, $E7, $E6, $26, $22, $E2, $E3, $23, $E1, $21, $20,
    $E0, $A0, $60, $61, $A1, $63, $A3, $A2, $62, $66, $A6, $A7, $67, $A5, $65,
    $64, $A4, $6C, $AC, $AD, $6D, $AF, $6F, $6E, $AE, $AA, $6A, $6B, $AB, $69,
    $A9, $A8, $68, $78, $B8, $B9, $79, $BB, $7B, $7A, $BA, $BE, $7E, $7F, $BF,
    $7D, $BD, $BC, $7C, $B4, $74, $75, $B5, $77, $B7, $B6, $76, $72, $B2, $B3,
    $73, $B1, $71, $70, $B0, $50, $90, $91, $51, $93, $53, $52, $92, $96, $56,
    $57, $97, $55, $95, $94, $54, $9C, $5C, $5D, $9D, $5F, $9F, $9E, $5E, $5A,
    $9A, $9B, $5B, $99, $59, $58, $98, $88, $48, $49, $89, $4B, $8B, $8A, $4A,
    $4E, $8E, $8F, $4F, $8D, $4D, $4C, $8C, $44, $84, $85, $45, $87, $47, $46,
    $86, $82, $42, $43, $83, $41, $81, $80, $40);

  { ****************************************************************************** }
function TLLvl_MBUS.MessPreview(bRx: AnsiString; len: Integer): Boolean;
  var
    buff: ArrayMass;
    // len:integer;
    i, j: Integer;
    crc: Word;
  begin
    { написать разбор посылки }
    result := False;
    if len >= lenRxTotal then
      begin
        result := True;

        for i := 0 to length(buff) - 1 do
          if i < len then
            buff[i] := BYTE(bRx[i + 1])
          else
            buff[i] := 0;

        for i := 0 to len - 1 do
          begin
            if buff[i] = self.add then
              if (buff[i + 1] and $7F) = self.cmd then
                begin
                  if (buff[i + 1] and $80) = 80 then
                    begin // ответ с кодом ошибки
                      if (i + 5) <= len then
                        begin
                          crc := self.CalcCRCMBUS(@buff[i], 3);
                          if HIBYTE(crc) = buff[i + 5 - 2] then
                            if LOBYTE(crc) = buff[i + 5 - 1] then
                              begin
                                self.Err := buff[i + 2];
                                result := True;
                              end;
                        end;
                    end
                  else
                    begin //
                      if (i + lenRxTotal) <= len then
                        begin
                          crc := self.CalcCRCMBUS(@buff[i], lenRxTotal - 2);
                          if LOBYTE(crc) = buff[i + lenRxTotal - 2] then
                            if HIBYTE(crc) = buff[i + lenRxTotal - 1] then
                              begin
                                self.Err := 0;
                                for j := i to i + lenRxTotal - 1 do
                                  self.data[j - i] := buff[j];
                                result := True;
                              end;
                        end;
                    end;
                end;

          end;
      end;

  end;

constructor TLLvl_MBUS.Create(var hPort: THandle; com, spd: string; add: BYTE);
  begin { *** }
    Randomize;
    inherited Create(hPort, com, spd, add, AnsiChar($00), AnsiChar($00),
      AnsiChar($00), True);
    NStopBit := 2;
    self.OnMessPreview := self.MessPreview;
    self.add := add;
  end;

procedure TLLvl_MBUS.SetNAdd(NAdd: BYTE);
  begin
    self.add := NAdd;
  end;

function TLLvl_MBUS.Cmd_16(regAdd: Word; var len: Word; PReg: PByte): BYTE;
  var
    i: Integer;
    PR: PByte;
  begin
    PR := PReg;
    self.Err := $FF;
    self.cmd := $10;
    self.regAdd := regAdd;
    self.lenRxTotal := 8;
    self.lenRxData := 0;
    if (OpenComPort()) then
      begin
        self.data[0] := self.add;
        self.data[1] := self.cmd;
        self.data[2] := HIBYTE(regAdd);
        self.data[3] := LOBYTE(regAdd);
        self.data[4] := HIBYTE(len);
        self.data[5] := LOBYTE(len);
        self.data[6] := len * 2;
        for i := 0 to (len) - 1 do
          begin
            data[7 + i] := (PR + 1)^;
            data[7 + i + 1] := PR^;
            inc(PR);
            inc(PR);
          end;

        TransactionMBUS(self.data, 7 + len * 2, 500);
      end;
    result := Err;
  end;

function TLLvl_MBUS.Cmd_03(regAdd: Word; var len: Word; PReg: PByte): BYTE;
  var
    i: Integer;
  begin
    self.Err := $FF;
    self.cmd := $03;
    self.regAdd := regAdd;
    self.lenRxTotal := 5 + len * 2;
    self.lenRxData := 0;
    if (OpenComPort()) then
      begin
        self.data[0] := self.add;
        self.data[1] := self.cmd;
        self.data[2] := HIBYTE(regAdd);
        self.data[3] := LOBYTE(regAdd);
        self.data[4] := HIBYTE(len);
        self.data[5] := LOBYTE(len);
        self.lenRxData := len * 2;
        self.TransactionMBUS(self.data, 6, 300);
        if self.Err = 0 then
          begin
            for i := 0 to lenRxData - 1 do
              self.data[i] := data[i + 3];
          end
        else
          begin
            lenRxData := 0;
          end;
      end;

    for i := lenRxData to length(data) - 1 do
      self.data[i] := 0;
    for i := 0 to len * 2 - 1 do
      begin
        PReg^ := data[i];
        inc(PReg);
      end;

    len := lenRxData;

    result := self.Err;
  end;

destructor TLLvl_MBUS.Destroy;
  begin { *** }
    inherited Destroy;
  end;

{ ****************************************************************************** }
{ ****************************************************************************** }
{ ****************************************************************************** }
constructor TProtocolNet_PhisicalLvl.Create(var hPort: THandle;
  com, spd: string; add: BYTE; signBeginTx, signBeginRx,
  signEndRx: AnsiChar; fEnCRC: Boolean);
  begin
    self.signBeginRx := signBeginRx;
    self.signBeginTx := signBeginTx;
    self.signEndRx := signEndRx;
    self.fEnCRC := fEnCRC;

    phPort := @hPort;
    NCom := com;
    NSpd := spd;
    NAdd := add;
    NStopBit := 1;

    FillChar(buffRx, SizeOf(buffRx), 0);
    FillChar(semi, SizeOf(semi), 0);
    nSemi := 0;

    Listing := TStringList.Create;
    Listing.Clear;
    Randomize;
    OnMessPreview := MessPreview;
    inherited Create;
  end;

destructor TProtocolNet_PhisicalLvl.Destroy;
  begin
    Listing.Clear;
    Listing.free;
    inherited Destroy;
  end;

procedure TProtocolNet_PhisicalLvl.AddList(strWho: string; strWhat: string);
  var
    DateTime: TDateTime;
  begin
    DateTime := Time;
    Listing.add('(' + TimeToStr(DateTime)
          + ')' + NCom + ':" ' + strWho + '- ' + strWhat + '"');
  end;

procedure TProtocolNet_PhisicalLvl.StrToAnsi(var AStr: AnsiString;
  var str: string);
  var
    i: Integer;
  begin
    setlength(AStr, length(str));
    for i := 1 to length(str) do
      AStr[i] := AnsiChar(str[i]);
  end;

procedure TProtocolNet_PhisicalLvl.AnsiToStr(var str: string;
  var AStr: AnsiString);
  var
    i: Integer;
  begin
    setlength(str, length(AStr));
    for i := 1 to length(AStr) do
      str[i] := Char(AStr[i]);
  end;

function TProtocolNet_PhisicalLvl.IntToAnsi(i: Integer): AnsiString;
  begin
    result := AnsiString(IntToStr(i));
  end;

procedure TProtocolNet_PhisicalLvl.AddLstMess(strWhat: string);
  var
    DateTime: TDateTime;
  begin
    DateTime := Time;
    Listing.add('(' + TimeToStr(DateTime) + ')' + self.NCom + ':" ' + strWhat +
          '"');
  end;

procedure TProtocolNet_PhisicalLvl.ClosePort;
  begin
    if phPort^ <> 0 then
      CloseHandle(phPort^);
    phPort^ := 0;
  end;

procedure TProtocolNet_PhisicalLvl.NewSettingComm(var hPort: THandle;
  NCom: string; NAdd: BYTE);
  begin
    phPort := @hPort;
    self.NCom := NCom;
    self.NAdd := NAdd;
  end;

procedure TProtocolNet_PhisicalLvl.SetNAdd(NAdd: BYTE);
  begin
    self.NAdd := NAdd;
  end;

function TProtocolNet_PhisicalLvl.GetNAdd(): BYTE;
  begin
    result := self.NAdd;
  end;

procedure TProtocolNet_PhisicalLvl.SetNSpd(NSpd: string);
  begin
    self.NSpd := NSpd;
  end;

function TProtocolNet_PhisicalLvl.GetNSpd(): string;
  begin
    result := self.NSpd;
  end;

function TProtocolNet_PhisicalLvl.OpenComPort(): Boolean;
  var
    flagErr: Boolean;
    valSpd: Integer;
    i: BYTE;
    sOpenPort: String;
  begin

    if phPort^ <> 0 then
      begin
        if not tryStrToInt(NSpd, valSpd) then
          begin
            showMessage('Попытка установить кривую скорость: ' + NSpd);
            result := False;
            exit;
          end;

        GetCommState(phPort^, dcb);
        if Integer(dcb.BaudRate) = valSpd then
          begin
            OpenComPort := True;
            exit;
          end;

        CloseHandle(phPort^);
        phPort^ := 0;
      end;
    result := False;

    for i := 0 to 150 do
      begin
        phPort^ := CreateFile(PWideChar(string('\\.\' + NCom)),
          GENERIC_READ or GENERIC_WRITE, 0, Nil, OPEN_EXISTING,
          FILE_FLAG_OVERLAPPED, 0);
        if (phPort^ <> INVALID_HANDLE_VALUE) then
          Break
        else if GetLastError() = ERROR_ACCESS_DENIED then
          begin
            sleep(20);
          end
        else
          Break;
      end;

    if (phPort^ <> INVALID_HANDLE_VALUE) then
      begin
        FillChar(dcb, SizeOf(TDCB), 0);
        dcb.DCBlength := SizeOf(dcb);

        if self.NStopBit = 1 then
          sOpenPort := 'baud=' + NSpd + ' parity=N data=8 stop=1';
        if self.NStopBit = 2 then
          sOpenPort := 'baud=' + NSpd + ' parity=N data=8 stop=2';
        flagErr := BuildCommDCB(PWideChar(sOpenPort), dcb);

        if flagErr then
          begin
            dcb.EvtChar := self.signEndRx;
            dcb.EofChar := self.signEndRx;

            FillChar(ct, SizeOf(COMMTIMEOUTS), 0);
            ct.ReadIntervalTimeout := MAXDWORD;
            ct.ReadTotalTimeoutMultiplier := 0;
            ct.ReadTotalTimeoutConstant := 0;
            ct.WriteTotalTimeoutMultiplier := 0;
            ct.WriteTotalTimeoutConstant := 0;

            if SetCommState(phPort^, dcb) then
              begin
                EscapeCommFunction(phPort^, SETRTS);
                EscapeCommFunction(phPort^, CLRDTR);
                SetCommTimeouts(phPort^, ct);
                SetupComm(phPort^, 256, 256);
                SetCommMask(phPort^, EV_RXCHAR);
                PurgeComm(phPort^, PURGE_RXCLEAR or PURGE_TXCLEAR);

                result := True;
              end;
          end;
      end
    else
      begin
        AddList('OpenComPort', 'Неудалось открыть ком порт');
        phPort^ := 0;
      end;
  end;

constructor TLowLvlUAI.Create(var hPort: THandle; com, spd: string;
  add, TDev: BYTE);
  begin
    BC.TDev := TDev;
    BC.TCom := UNDEF;
    fTestMode := False;
    Randomize;

    inherited Create(hPort, com, spd, add, ':', '!', AnsiChar($0D), True);
  end;

destructor TLowLvlUAI.Destroy;
  begin
    inherited Destroy;
  end;

constructor TLLvl_PCL0.Create(var hPort: THandle; com, spd: string; add: BYTE);
  begin
    Randomize;
    fTestMode := False;
    inherited Create(hPort, com, spd, add, AnsiChar($02), AnsiChar($02),
      AnsiChar($0A), False);
  end;

function TLLvl_PCL0.GetListing(var str: string): Boolean;
  begin
    if Listing.Count > 0 then
      begin
        str := Listing.Strings[0];
        Listing.Delete(0);
        result := True;
      end
    else
      begin
        Listing.Clear;
        result := False;
      end;
  end;

function TLLvl_PCL0.Transaction(str: AnsiString; TOut: Word;
  fTestMode: Boolean; TCMD: TPCL0_CMD): EnErr;
  begin
    result := OK;
    if fTestMode then
      begin
        Transaction(str, TOut);
        case TCMD of
          enPCL0_CMD_RSD:
            ;
          enPCL0_CMD_RRD:
            buffRx := #02 + '01RRD,OK,0100' + AnsiChar($0D) + AnsiChar($0A);
          enPCL0_CMD_WSD:
            buffRx := #02 + '01WSD,OK' + AnsiChar($0D) + AnsiChar($0A);
          enPCL0_CMD_WRD:
            buffRx := #02 + '01WRD,OK' + AnsiChar($0D) + AnsiChar($0A);
          enPCL0_CMD_STD:
            ;
          enPCL0_CMD_CLD:
            ;
        end;
      end
    else
      begin
        result := Transaction(str, TOut);
      end;
  end;

function TLLvl_PCL0.Cmd_RRD(var Regs: TPCL0_Regs): EnErr;
  var
    buff, subBuff: AnsiString;
    idx, i, j, v, len: Integer;
    keyStr: AnsiString;
    s: string;
  begin
    result := EnErr.EFAIL;
    if (OpenComPort()) then
      begin
        s := format('RRD,%.2d', [length(Regs)]);
        for i := 0 to length(Regs) - 1 do
          s := s + format(',%.4d', [Regs[i].add]);

        StrToAnsi(buff, s);
        result := Transaction(buff, 800, fTestMode, TPCL0_CMD.enPCL0_CMD_RRD);
        if (result = OK) then
          begin
            s := format('%.2dRRD,OK', [self.NAdd]);
            StrToAnsi(keyStr, s);
            idx := pos(keyStr, buffRx) + length(keyStr) + 1;
            len := ((length(buffRx) - idx) div 5);
            if len = length(Regs) then
              begin
                for i := 0 to len - 1 do
                  begin
                    j := idx + i * 5;
                    subBuff := copy(buffRx, j, 4);
                    if tryStrToInt(string('$' + subBuff), v) then
                      begin
                        Regs[i].val := v;
                      end;
                  end;
              end
            else
              result := EnErr.EFAIL;
          end;
      end;
  end;

function TLLvl_PCL0.Cmd_WRD(var Regs: TPCL0_Regs): EnErr;
  var
    buff: AnsiString;
    i: Integer;
    // lenBuffRx:integer;
    keyStr: AnsiString;
    s: string;
  begin
    result := EnErr.EBUSY;
    if (OpenComPort()) then
      begin
        s := format('WRD,%.2d', [length(Regs)]);
        for i := 0 to length(Regs) - 1 do
          s := s + format(',%.4d,%.4X', [Regs[i].add, UInt16(Regs[i].val)]);
        StrToAnsi(buff, s);

        result := Transaction(buff, 800, fTestMode, TPCL0_CMD.enPCL0_CMD_WRD);
        if (result = OK) then
          begin
            s := format('%.2dWRD,OK', [self.NAdd]);
            StrToAnsi(keyStr, s);
            i := pos(keyStr, buffRx);
            if i = 0 then
              result := EnErr.EFAIL;
          end;
      end;
  end;

function TLLvl_PCL0.Cmd_WSD(AddDReg: Integer; var VReg: TPCL0_Data): EnErr;
  var
    buff, AStr: AnsiString;
    i: Integer;
    s: string;
  begin
    result := EnErr.EBUSY;
    if (OpenComPort()) then
      begin
        s := format('WSD,%.2d,%.4d', [length(VReg), AddDReg]);
        for i := 0 to length(VReg) - 1 do
          s := s + format(',%.4X', [VReg[i]]);
        StrToAnsi(buff, s);

        result := Transaction(buff, 800, fTestMode, TPCL0_CMD.enPCL0_CMD_WSD);
        if (result = OK) then
          begin
            s := IntToStr(self.NAdd);
            StrToAnsi(AStr, s);
            if pos(AStr + 'WSD,OK', buffRx) <> 0 then
            else
              result := EnErr.EFAIL;
          end;
      end;
  end;

destructor TLLvl_PCL0.Destroy;
  begin
    inherited Destroy;
  end;

function TLowLvlUAI.isxdigit_M(b: AnsiChar): BYTE;
  var
    v: BYTE;
  begin
    v := BYTE(b);
    if (v >= $30) and (v <= $39) then
      isxdigit_M := 1
    else if (v >= $61) and (v <= $66) then
      isxdigit_M := 2
    else if (v >= $41) and (v <= $46) then
      isxdigit_M := 3
    else
      isxdigit_M := 0;
  end;

procedure TLowLvlUAI.HexToStr(pHex: PByte; pStr: PAnsiChar; len: BYTE);
  var
    strL, strH, i: Integer;
  begin

    for i := 0 to len - 1 do
      begin
        strL := pHex^ and $0F; ;
        strH := pHex^ and $F0;
        strH := strH div 16;
        if (strH < 10) then
          strH := strH + $30
        else
          strH := strH + $37;
        pStr[i * 2] := AnsiChar(strH);
        if (strL < 10) then
          strL := strL + $30
        else
          strL := strL + $37;
        pStr[i * 2 + 1] := AnsiChar(strL);
        inc(pHex); // :=pHex;
      end;
  end;

function TLowLvlUAI.StrToHex_M(str: AnsiString;
  var ptrByte: ArrayMass): Boolean;
  var
    i, j, lByte, hByte: BYTE;
  begin
    result := False;
    hByte := 0;
    lByte := 0;
    for i := 1 to BYTE(length(str) div 2) do
      begin
        j := i * 2 - 1;
        case isxdigit_M(str[j]) of
          0:
            begin
              exit;
            end;
          1:
            begin
              hByte := BYTE(str[j]) - $30;
            end;
          2:
            begin
              hByte := BYTE(str[j]) - $61 + 10;
            end;
          3:
            begin
              hByte := BYTE(str[j]) - $41 + 10;
            end;
        end;
        hByte := hByte * 16;

        j := i * 2;
        case isxdigit_M(str[j]) of
          0:
            begin
              exit;
            end;
          1:
            begin
              lByte := BYTE(str[j]) - $30;
            end;
          2:
            begin
              lByte := BYTE(str[j]) - $61 + 10;
            end;
          3:
            begin
              lByte := BYTE(str[j]) - $41 + 10;
            end;
        end;

        ptrByte[i - 1] := hByte + lByte;

      end;
    result := True;
  end;

function TProtocolNet_PhisicalLvl.CalcCRC16Table(str: AnsiString;
  Lenght: BYTE): Word;
  // (X^16 + X^15 + X^2 + 1).
  var
    uIndex: BYTE; { /* will index into CRC lookup table */ }
    i: Word;
    CRCHi, CRCLo: Word;
    val: AnsiChar;
    j: BYTE;
  begin
    CRCHi := $FF;
    CRCLo := $FF;
    for j := 1 to Lenght do { /* pass through message buffer */ }
      begin
        val := str[j];
        uIndex := CRCHi xor Ord(BYTE(val));
        CRCHi := CRCLo xor tableCRC16Hi[uIndex];
        CRCLo := tableCRC16Lo[uIndex];
      end;
    i := CRCLo * 256;
    CalcCRC16Table := i or CRCHi;
  end;

function TProtocolNet_PhisicalLvl.CalcCRCMBUS(PMass: PByte; Lenght: BYTE): Word;
  // (X^16 + X^15 + X^2 + 1).
  var
    uIndex: BYTE; { /* will index into CRC lookup table */ }
    i: Word;
    CRCHi, CRCLo: Word;
    // val:Byte;
    j: BYTE;
  begin
    CRCHi := $FF;
    CRCLo := $FF;
    for j := 0 to Lenght - 1 do { /* pass through message buffer */ }
      begin
        uIndex := CRCHi xor PMass^;
        CRCHi := CRCLo xor tableCRC16Hi[uIndex];
        CRCLo := tableCRC16Lo[uIndex];
        inc(PMass);
      end;
    i := CRCLo * 256;
    CalcCRCMBUS := i or CRCHi;
  end;

function TProtocolNet_PhisicalLvl.CreatParcelASCII(str: AnsiString): AnsiString;
  var
    crc: Word;
    strCrc: AnsiString;
    s: string;
  begin
    strCrc := '';
    if self.fEnCRC then
      begin
        crc := CalcCRC16Table(str, length(str));
        s := IntToStr(crc);
        StrToAnsi(strCrc, s);
      end;
    result := AnsiChar($FF) + self.signBeginTx + str + strCrc + AnsiChar($0D)
        + AnsiChar($0A);
  end;

function TProtocolNet_PhisicalLvl.MessPreview(bRx: AnsiString;
  len: Integer): Boolean;
  var
    add, i, j, x: Integer;
    str: AnsiString;
  begin
    { написать разбор посылки }
    result := False;
    IndEnd := 0;
    IndStart := 0;
    setlength(str, 255);

    for i := len downto 1 do // поиск #13
      begin
        if (bRx[i] = self.signEndRx) then
          begin
            IndEnd := i;
            for j := i downto 1 do // поиск '!'
              begin
                if (bRx[j] = self.signBeginRx) then
                  begin
                    IndStart := j;
                    Break;
                  end;
              end;
            Break;
          end;
      end;
    if (IndStart <> 0) and (IndEnd <> 0) then
      begin
        if self.fEnCRC then
          begin
            nSemi := 0;
            for i := IndStart + 2 to IndEnd - 2 do // помечаем ';'
              begin
                if bRx[i] = ';' then
                  begin
                    semi[nSemi] := i;
                    nSemi := nSemi + 1;
                  end;
              end;
            nSemi := nSemi - 1;
            str := copy(bRx, IndStart + 1, semi[nSemi] - IndStart);
            x := CalcCRC16Table(str, length(str));
            str := IntToAnsi(x);
            if pos(str, bRx) <> 0 then
              begin
                result := True;
                if (NAdd <> 0) then
                  begin
                    str := copy(bRx, IndStart + 1, semi[0] - IndStart - 1);
                    if tryStrToInt(string(str), add) then
                      begin
                        if add <> NAdd then
                          begin
                            result := False;
                            AddList('MessPreview',
                              'ожидался адрес ' + IntToStr(NAdd)
                                  + ' получен ' + string(str));
                          end
                      end
                    else
                      AddList('MessPreview',
                        'ожидался адрес ' + IntToStr(NAdd)
                            + ' получен ' + string(str));

                  end;
              end
            else
              AddList('MessPreview', 'ошибка контрольной суммы' + string(bRx));
          end
        else
          begin
            result := True;
          end;
      end;
  end;

function TProtocolNet_PhisicalLvl.Transaction(TxStr: AnsiString;
  TOut: Word): EnErr;
  var
    i: Integer;
    TxBuff: TRxTxBuff;
    s: string;
    AStr: AnsiString;
  begin
    s := format('%.2d', [NAdd]);
    StrToAnsi(AStr, s);
    TxStr := CreatParcelASCII(AStr + TxStr);
    for i := 1 to length(TxStr) do
      TxBuff[i - 1] := TxStr[i];

    AddLstMess(string(TxStr));
    Transaction := TransactionLL(TxBuff, length(TxStr), TOut);

  end;

function TProtocolNet_PhisicalLvl.TransactionMBUS(TxMass: ArrayMass;
  TxLen: Integer; TOut: Word): EnErr;
  var
    crc: Word;
  begin
    crc := CalcCRCMBUS(@TxMass[0], TxLen);
    TxMass[TxLen] := LOBYTE(crc);
    TxMass[TxLen + 1] := HIBYTE(crc);
    TxLen := TxLen + 2;
    TransactionMBUS := TransactionLL(TRxTxBuff(TxMass), TxLen, TOut);
  end;

function TProtocolNet_PhisicalLvl.TransactionLL(TxBuff: TRxTxBuff;
  TxLen: Integer; TOut: Word): EnErr;
  var
    // len:integer;
    RxBuff: TRxTxBuff;
    ovl: OVERLAPPED;
    i, btr, temp, RstWait: dword;
    evFlag: cardinal;
    curstat: COMSTAT;
    // Buff : array[0..255] of byte;
    // s:AnsiString;
  begin
    if (phPort^ <> 0) and (phPort^ <> INVALID_HANDLE_VALUE) then
      begin
        buffRx := '';
        // pBuffTx:=@str[1];
        buffRx := '';
        FillChar(ovl, SizeOf(ovl), 0);
        ovl.hEvent := CreateEvent(nil, False, False, nil);
        WriteFile(phPort^, TxBuff { AnsiString(pBuffTx^) } , TxLen
          { length(str) } , i, @ovl);
        // set ovl.event= событие порта
        TransactionLL := ETOT;
        buffRx := '';
        setlength(buffRx, 0);
        while True do
          begin

            WaitCommEvent(phPort^, evFlag, @ovl);
            RstWait := WaitForSingleObject(ovl.hEvent, TOut);
            case RstWait of
              WAIT_OBJECT_0:
                begin
                  if (GetOverlappedResult(phPort^, ovl, temp, True)) then
                    // проверяем, успешно ли завершилась
                    // перекрываемая операция WaitCommEvent
                    if ((evFlag and EV_RXCHAR) <> 0) then
                      // если произошло именно событие прихода байта
                      begin
                        ClearCommError(phPort^, temp, @curstat);
                        // нужно заполнить структуру COMSTAT
                        btr := curstat.cbInQue;
                        // получить количество принятых байтов
                        if (btr <> 0) then // если в буфере порта есть непрочитанные байты
                          begin
                            for i := 0 to 250 - 1 do
                              begin
                                RxBuff[i] := 'A';
                              end;

                            ReadFile(phPort^, RxBuff, btr, temp, @ovl);
                            // прочитать байты из порта в буфер программы
                            for i := 0 to btr - 1 do
                              buffRx := buffRx + RxBuff[i];
                            try
                              if OnMessPreview(buffRx, length(buffRx)) then
                                begin
                                  TransactionLL := OK;
                                  Break;
                                end;
                            except
                              AddList('MessPreview',
                                'except ошибка преобразования');
                              TransactionLL := EDISASSEMBLE;
                              Break;
                            end;

                          end;
                      end;
                  // http://www.piclist.ru/S-COM-THREAD-RUS/S-COM-THREAD-RUS.html
                end;
              WAIT_TIMEOUT:
                begin
                  AddList('Transaction', 'Ошибка, истекло время ожидания!');
                  TransactionLL := ETOT;
                  Break;
                end;
            end;
          end;

        AddLstMess(string(buffRx));
        CloseHandle(ovl.hEvent);
      end
    else
      TransactionLL := EBUSY;
  end;

// procedure TLowLvlUAI.EnNameBCRet(var v: EnBCRet;i:byte);
// begin
// case i of
// 0: v:=EZERO;//     = 0, // успешное завершение.
// 1: v:=ENOCMD;//    = 1, // команда не найдена.
// 2: v:=EINVALCMD;// = 2, // неверный формат команды.
// 3: v:=ENOPAR;//    = 3, // параметр не найден.
// 4: v:=EPERM;//     = 4, // нарушение прав доступа.
// 5: v:=EACCESS;//   = 5, // ошибка чтения (в том числе при несовпадении CRC) и записи параметра (параметр только для чтения, область данных хранения параметра защищена от записи).
// 6: v:=ERANGE;//    = 6, // выход параметра за границу диапазона
// 7: v:=ENOENT;//    = 7, // файл не найден
// 8: v:=ENOMEM;//    = 8, // нехватает памяти.
// 9: v:=EBADF;//     = 9, // недопустимый идентификатор файла или идентификатор не ссылается на открытый файл.
// 10: v:=ENOSPC//    = 10, // отсутствует место на физическом носителе.
// else
// v:=EUNDEF;
// end;
// end;

function TLowLvlUAI.ECodReturn(pErr: PAnsiChar): EnBCRet;
  // var
  // Cod:EnBCRet;
  begin
    if (pErr^ <> '$') then
      begin
        ECodReturn := EFALSE;
      end
    else
      ECodReturn := EFALSE;
    // try
    // EnNameBCRet(Cod,StrToInt(pErr));
    // ECodReturn:=Cod;
    // except
    // ECodReturn:=EFALSE;
    // end;
  end;

function TLowLvlUAI.EnNameBCStat(var v: EnBCStat; i: BYTE): Boolean;
  begin
    EnNameBCStat := True;
    case i of
      0:
        v := Success;
      1:
        v := Pass;
      2:
        v := Failure;
      3:
        v := Unsuccessful;
      else
        EnNameBCStat := False;
      end;
  end;

function TLowLvlUAI.EnNameBCUAI(var v: EnBCUAI; i: BYTE): Boolean;
  begin
    EnNameBCUAI := True;
    case i of
      0:
        v := UAI_FULL;
      1:
        v := UAI_LIGHT;
      else
        begin
          v := UNDEF;
          EnNameBCUAI := False;
        end;
      end;
  end;

{ *************************************** }
{ procedure TLowLvlCom.AddList(strWho:AnsiString;strWhat:AnsiString);
  var
  DateTime : TDateTime;
  begin
  DateTime:= Time;
  Listing.Add('('+TimeToStr(DateTime)+')'+BC.NCom+':" '+strWho+'- '+strWhat+'"');
  end;

  procedure TLowLvlCom.AddLstMess(strWhat:AnsiString);
  var
  DateTime : TDateTime;
  begin
  DateTime:= Time;
  Listing.Add('('+TimeToStr(DateTime)+')'+BC.NCom+':" '+strWhat+'"');
  end;
}
{ *************************************** }
function TLowLvlUAI.UAI(cmd: BYTE; buffTx: AnsiString): EnErr;

  var
    str: AnsiString;
    SnglVal: real;
    LIntVal: Longint;
    DWVal: dword;
    i: Integer;
  begin
    setlength(str, 255);
    BC.CRet := EZERO;
    result := OK;
    if (OpenComPort()) then
      begin

        case cmd of
          0:
            begin
              result := Transaction(';0;', 500);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);

                      try
                        if BC.TDev <> strToInt(string(str)) then
                          begin
                            result := EUNDEFTYPE;
                            AddList('UAI',
                              'неподдерживаемый тип прибора ->' + string(str));
                          end
                        else
                          begin
                            str := copy(buffRx, IndStart + 1,
                              semi[0] - IndStart - 1);
                            val.arr[0] := strToInt(string(str));
                            val.n := 1;
                          end;
                      except
                        AddList('UAI',
                          'ошибка преобразования Str->Int' + string(str));
                        result := EEXCEPT;
                      end;
                    end;
                end;
            end;
          1:
            begin
              buffTx := ';1;' + buffTx + ';';
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      try
                        SnglVal := strToFloat(string(str)); (PSingle(@val.arr))
                        ^ := SnglVal;
                        val.n := 4;
                      except
                        AddList('UAI',
                          'ошибка преобразования Str->Float' + '"' + string
                              (str) + '"');
                        result := EEXCEPT;
                      end;
                    end;
                end;
            end;
          2:
            begin
              buffTx := ';2;' + buffTx + ';';
              // установить адресс на доступ в еепроме
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                  BC.CRet := ECodReturn(@str[1]);
                  if (BC.CRet <> EZERO) then
                    begin
                      AddList('UAI', ' Код ошибки=' + string(str));
                      result := EFAIL;
                    end
                  else
                    result := OK;
                end;
            end;

          8:
            begin
              buffTx := ';8;' + buffTx + ';';
              // считать из еепрома buffTx байт.
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                  if str[1] = '$' then
                    begin
                      AddList('UAI', ' Код ошибки=' + str[2]);
                      result := EFAIL;
                    end
                  else
                    begin
                      val.n := (semi[1] - semi[0] - 1);
                      for i := 0 to val.n - 1 do
                        val.arr[i] := BYTE(str[i + 1]);
                      result := OK;
                    end;
                end;
            end;
          199:
            begin
              buffTx := ';199;A5;'; // ресет!
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      { try
                        SnglVal:=strToFloat(str);
                        (PSingle(@Val.arr))^:=SnglVal;
                        Val.n:=4;
                        except
                        AddList('UAI','ошибка преобразования Str->Float'+'"'+str+'"');
                        result:=EEXCEPT;
                        end; }
                    end;
                end;
            end;
          32:
            begin
              result := Transaction(';32;', 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      try
                        if EnNameBCUAI(BC.TCom, strToInt(string(str))) then
                          result := OK
                        else
                          begin
                            AddList('UAI', 'неподдерживаемый протокол');
                            result := EPROTOCOL;
                          end;

                      except
                        AddList('UAI',
                          'ошибка преобразования Str->Int' + string(str));
                        result := EEXCEPT;
                      end;
                    end;
                end;
            end;
          33:
            begin
              buffTx := ';33;' + buffTx + ';';
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          AddList('UAI',
                            'ошибка преобразования строки:' + string(str));
                          result := EEXCEPT;
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI',
                                'Невозможно установ. новый адрес. Код ошибки='
                                    + string(str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;
          34:
            begin
              buffTx := ';34;' + buffTx + ';';
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          AddList('UAI',
                            'ошибка преобразования строки:' + string(str));
                          result := EEXCEPT;
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI',
                                'Невозможно установ. новую скороть. Код ошибки='
                                    + string(str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;
          35:
            begin
              buffTx := ';35;';
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      try
                        if (EnNameBCStat(BC.Stat,
                            strToInt(string(str))) <> True) then
                          begin
                            AddList('UAI',
                              'невозможное значение:' + string(str));
                            result := EIMPOSSIBLE;
                          end;
                      except
                        AddList('UAI',
                          'ошибка преобразования строки:' + string(str));
                        result := EEXCEPT;
                      end;
                    end;
                end;
            end;
          36:
            begin
              buffTx := ';36;' + IntToAnsi(val.n) + ';';
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          if (StrToHex_M(str, val.arr) = False) then
                            begin
                              AddList('UAI', 'ошибка преобразования строки');
                              result := EEXCEPT;
                            end
                          else
                            val.n := BYTE(length(str) div 2);
                        end
                      else
                        begin
                          AddList('UAI',
                            'Невозможно считать буфер возврата' + string(str));
                          result := EFAIL;
                        end;
                    end;
                end;
            end;
          37:
            begin
              buffTx := ';37;' + buffTx + ';';
              result := Transaction(buffTx, 1500);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          if (StrToHex_M(str, val.arr) = False) then
                            begin
                              AddList('UAI', 'ошибка преобразования строки');
                              result := EEXCEPT;
                            end
                          else
                            begin
                              val.n := BYTE(length(str) div 2);
                            end;
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI', 'код возврата' + string(str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;
          38:
            begin
              buffTx := ';38;' + buffTx + ';';
              result := Transaction(buffTx, 5000);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          AddList('UAI', 'невозможное значение:' + string(str));
                          result := EIMPOSSIBLE;
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI', 'код возврата' + string(str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;
          40:
            begin
              buffTx := ';40;' + buffTx + ';';
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          AddList('UAI',
                            'ошибка преобразования строки:' + string(str));
                          result := EEXCEPT;
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI',
                                'Невозможно открыть файл. Код ошибки=' + string
                                    (str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;
          41:
            begin
              buffTx := ';41;' + buffTx + ';';
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          AddList('UAI',
                            'ошибка преобразования строки:' + string(str));
                          result := EEXCEPT;
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI',
                                'Невозможно установит адрес в файле. Код ошибки='
                                    + string(str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;
          42:
            begin
              buffTx := ';42;' + buffTx + ';';
              result := Transaction(buffTx, 3000);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          if (StrToHex_M(str, val.arr) = False) then
                            begin
                              AddList('UAI', 'ошибка преобразования строки:');
                              result := EEXCEPT;
                            end
                          else
                            val.n := BYTE(length(str) div 2);
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI',
                                'Невозможно считать данные из файла. Код ошибки='
                                    + string(str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;
          43:
            begin
              buffTx := ';43;' + buffTx + ';';
              result := Transaction(buffTx, 6800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          AddList('UAI',
                            'ошибка преобразования строки:' + string(str));
                          result := EEXCEPT;
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI',
                                'Невозможно записать данные в файл. Код ошибки='
                                    + string(str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;
          44:
            begin
              buffTx := ';44;' + buffTx + ';';
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          AddList('UAI',
                            'ошибка преобразования строки:' + string(str));
                          result := EEXCEPT;
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI',
                                'Невозможно закрыть файл. Код ошибки=' + string
                                    (str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;
          45:
            begin
              buffTx := ';45;' + buffTx + ';';
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          if (tryStrToInt(string(str), LIntVal) = False) then
                            begin
                              AddList('UAI',
                                'ошибка преобразования строки:' + '"' + string
                                    (str) + '"');
                              result := EEXCEPT;
                            end
                          else
                            begin
                              PLongint(@val.arr[0])^ := LIntVal;
                              val.n := 4;
                              BC.CRet := EZERO;
                            end;
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI',
                                'Неудалось определить позицию в файле. Код ошибки='
                                    + string(str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;
          46:
            begin
              buffTx := ';46;' + buffTx + ';';
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          AddList('UAI',
                            'ошибка преобразования строки:' + string(str));
                          result := EEXCEPT;
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI',
                                'Невозможно установить атрибут файла. Код ошибки='
                                    + string(str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;

          47:
            begin
              buffTx := ';47;' + buffTx + ';';
              result := Transaction(buffTx, 800);
              if (result = OK) then
                begin
                  if nSemi = 1 then
                    begin
                      str := copy(buffRx, semi[0] + 1, semi[1] - semi[0] - 1);
                      BC.CRet := ECodReturn(@str[1]);
                      if BC.CRet = EFALSE then
                        begin
                          AddList('UAI',
                            'ошибка преобразования строки:' + string(str));
                          result := EEXCEPT;
                        end
                      else
                        begin
                          if (BC.CRet <> EZERO) then
                            begin
                              AddList('UAI',
                                'Невозможно стереть файл. Код ошибки=' + string
                                    (str));
                              result := EFAIL;
                            end;
                        end;
                    end;
                end;
            end;

        end;
      end
    else
      result := EBUSY;
  end;

{ procedure TLowLvlUAI.ClosePort;
  begin
  if phPort^<>0 then CloseHandle(phPort^);
  phPort^:=0;
  end;
}
{ procedure TLowLvlCom.NewPort(var hPort:THandle;nPort:AnsiString;NAdd:Byte);
  begin
  phPort:=@hPort;
  BC.NCom:=nPort;
  BC.NetAdd:=NAdd;
  end;

  procedure TLowLvlCom.NewAdd(NAdd:byte);
  begin
  end;
}

function TLowLvlUAI.FindDev(add: BYTE): Boolean;
  begin
    if fTestMode then
      begin
        sleep(300);
        Err := OK;
        result := True;
      end
    else
      begin
        self.SetNAdd(add);
        Err := UAI(0, '');
        if Err = OK then
          result := True
        else
          result := False;
      end;
  end;

function TLowLvlUAI.ResetDev(): Boolean;
  begin
    Err := UAI(199, '');
    if Err = OK then
      result := True
    else
      result := False;
  end;

function TLowLvlUAI.CMD1(Ch: BYTE): Boolean;
  begin
    Err := UAI(1, IntToAnsi(Ch));
    if Err = OK then
      result := True
    else
      result := False;
  end;

function TLowLvlUAI.OldProtocol_SetAddEEp(add: BYTE): Boolean;
  begin
    Err := UAI(2, IntToAnsi(add));
    if Err = OK then
      result := True
    else
      result := False;
  end;

function TLowLvlUAI.OldProtocol_ReadNByteEEp(NByte: BYTE): Boolean;
  begin
    Err := UAI(8, IntToAnsi(NByte));
    if Err = OK then
      result := True
    else
      result := False;
  end;

procedure TLowLvlUAI.SetTCom(TCom: EnBCUAI);
  begin
    self.BC.TCom := TCom;
  end;

function TLowLvlUAI.SetAdd(NAdd: BYTE): Boolean;
  begin
    Err := UAI(33, IntToAnsi(NAdd));
    if Err = OK then
      begin
        sleep(50);
        SetNAdd(NAdd);
        Err := UAI(0, '');
        if Err = OK then
          begin
            self.SetNAdd(NAdd);
            result := True;
          end
        else
          begin
            result := False;
          end;
      end
    else
      begin
        result := False;
      end;
  end;

function TLowLvlUAI.RdPar(id: typeID; var len: BYTE): Boolean;
  var
    FLAG: BYTE;
    AStr: AnsiString;
    str: string;
  begin
    result := False;
    str := IntToHex(id.Ch, 2) + IntToHex(id.id, 4);
    StrToAnsi(AStr, str);
    Err := UAI(37, AStr);

    if Err = OK then
      begin
        if BC.CRet = EFALSE then // если второй параметр не код возврата возврата значит сразу пришли данные
          begin
            result := True;
            exit;
          end;

        if BC.CRet <> EZERO then //
          begin
            result := False;
            exit;
          end;

        FLAG := 20;
        repeat
          sleep(20);
          Err := UAI(35, '');
          if Err = OK then // читаем регистр статуса
            begin
              case BC.Stat of
                Success:
                  begin
                    val.n := len;
                    Err := UAI(36, '');
                    len := val.n;
                    if Err = OK then
                      result := True
                    else
                      result := False;
                  end;
                Pass:
                  ;
                Failure:
                  ;

                Unsuccessful:
                  begin
                    val.n := 1;
                    Err := UAI(36, '');
                    len := val.n;
                    result := False;
                    Err := UAI(35, '');
                  end;
              end;
            end
          else
            begin
              result := False;
              Break;
            end;

          dec(FLAG);
        until not((BC.Stat = Failure) or (BC.Stat = Pass)) and (FLAG <> 0);
      end;

    // CloseHandle(hEvt);
  end;

function TLowLvlUAI.WrPar(id: typeID; pDat: PByte; var len: BYTE): Boolean;
  var
    FLAG: BYTE;
    str: string;
    buff: AnsiString;
  begin
    result := False;
    if (BC.TCom = UNDEF) then
      begin
        if self.fTestMode then
          begin
            Err := OK;
            BC.TCom := UAI_LIGHT;
          end
        else
          Err := UAI(32, '');

        if Err <> OK then
          begin
            result := False;
            exit;
          end;
      End;
    str := IntToHex(id.Ch, 2) + IntToHex(id.id, 4) + ';';
    StrToAnsi(buff, str);
    setlength(buff, length(buff) + len * 2);
    HexToStr(pDat, @buff[length(buff) - len * 2 + 1], len);

    if self.fTestMode then
      begin
        Err := OK;
        BC.TCom := UAI_LIGHT;
      end
    else
      begin
        Err := UAI(38, buff);
      end;

    if Err = OK then
      begin
        if (BC.TCom = UAI_FULL) then
          begin
            FLAG := 20;
            repeat
              sleep(20);
              Err := UAI(35, '');
              if Err = OK then // читаем регистр статуса
                begin
                  case BC.Stat of
                    Success:
                      begin
                        result := True;
                      end;
                    Pass:
                      ;
                    Failure:
                      ;

                    Unsuccessful:
                      begin
                        val.n := 1;
                        Err := UAI(36, '');
                        len := val.n;
                        Err := UAI(35, '');
                        result := False;
                      end;
                  end;
                end
              else
                begin
                  result := False;
                  Break;
                end;

              dec(FLAG);
            until not((BC.Stat = Failure) or (BC.Stat = Pass)) and (FLAG <> 0);
          end
        else
          result := True;
      end
  end;

function TLowLvlUAI.OpenFile(FileName: BYTE; FLAG, mode: BYTE): Boolean;
  var
    AStr, buff: AnsiString;
    str: string;
  begin
    result := False;
    Err := UAI(32, '');
    if Err = OK then
      begin
        if BC.TCom = UNDEF then
          begin
            result := False;
            exit;
          end;
      end
    else
      begin
        result := False;
        exit;
      end;
    str := IntToStr(FileName) + ';' + IntToHex(FLAG, 2);
    StrToAnsi(buff, str);
    if mode <> 0 then
      begin
        str := IntToHex(mode, 2);
        StrToAnsi(AStr, str);
        buff := buff + ';' + AStr;
      end;
    Err := UAI(40, buff);
    if Err = OK then
      begin
        if BC.CRet = EFALSE then // если второй параметр не код возврата возврата значит сразу пришли данные
          if BC.CRet <> EZERO then //
            begin
              result := False;
              exit;
            end;
        result := True;
      end;
  end;

function TLowLvlUAI.CloseFile(FileName: BYTE): Boolean;
  begin
    result := False;
    Err := UAI(32, '');
    if Err = OK then
      begin
        if BC.TCom = UNDEF then
          begin
            result := False;
            exit;
          end;
      end
    else
      begin
        result := False;
        exit;
      end;

    Err := UAI(44, IntToAnsi(FileName));
    if Err = OK then
      begin
        if BC.CRet = EFALSE then // если второй параметр не код возврата возврата значит сразу пришли данные
          if BC.CRet <> EZERO then //
            begin
              result := False;
              exit;
            end;
        result := True;
      end;
  end;

function TLowLvlUAI.SeekFile(FileName: BYTE; offset: dword;
  origin: BYTE): Boolean;
  // var
  // flag:byte;
  begin
    result := False;
    // buffTx:=;
    Err := UAI(41, IntToAnsi(FileName) + ';' + IntToAnsi(offset)
          + ';' + IntToAnsi(origin));
    if Err = OK then
      begin
        if BC.CRet = EFALSE then // если второй параметр не код возврата возврата значит сразу пришли данные
          if BC.CRet <> EZERO then //
            begin
              result := False;
              exit;
            end;
        result := True;
      end;
  end;

function TLowLvlUAI.AttFile(FileName: BYTE; mode: BYTE): Boolean;
  // var
  // flag:byte;
  begin
    result := False;

    Err := UAI(46, IntToAnsi(FileName) + ';' + IntToAnsi(mode));
    if Err = OK then
      begin
        if BC.CRet = EFALSE then //
          if BC.CRet <> EZERO then //
            begin
              result := False;
              exit;
            end;
        result := True;
      end;
  end;

function TLowLvlUAI.DelFile(FileName: BYTE): Boolean;
  // var
  // flag:byte;
  begin
    result := False;
    // buffTx:=;
    Err := UAI(47, IntToAnsi(FileName));
    if Err = OK then
      begin
        if BC.CRet = EFALSE then //
          if BC.CRet <> EZERO then //
            begin
              result := False;
              exit;
            end;
        result := True;
      end;
  end;

function TLowLvlUAI.TellFile(FileName: BYTE): Boolean;
  // var
  // flag:byte;
  begin
    result := False;
    // buffTx:=;
    Err := UAI(45, IntToAnsi(FileName));
    if Err = OK then
      begin
        if BC.CRet = EFALSE then // если второй параметр не код возврата возврата значит сразу пришли данные
          if BC.CRet <> EZERO then //
            begin
              result := False;
              exit;
            end;
        result := True;
      end;
  end;

function TLowLvlUAI.RdFile(FileName: BYTE; var len: BYTE): Boolean;
  var
    FLAG: BYTE;
  begin
    result := False;
    if BC.TCom = UNDEF then
      begin
        Err := UAI(32, '');
        if Err = OK then
          begin
            if BC.TCom = UNDEF then
              begin
                result := False;
                exit;
              end;
          end;
      end;

    Err := UAI(42, IntToAnsi(FileName) + ';' + IntToAnsi(len));
    if Err = OK then
      begin
        case BC.TCom of
          UAI_LIGHT:
            begin
              len := val.n;
              result := True;
            end;

          UAI_FULL:
            begin
              FLAG := 20;
              repeat
                sleep(20);
                Err := UAI(35, '');
                if Err = OK then // читаем регистр статуса
                  begin
                    case BC.Stat of
                      Success:
                        begin
                          val.n := len;
                          Err := UAI(36, '');
                          len := val.n;
                          if Err = OK then
                            result := True
                          else
                            result := False;
                        end;
                      Pass:
                        ;
                      Failure:
                        ;

                      Unsuccessful:
                        begin
                          val.n := 1;
                          Err := UAI(36, '');
                          len := val.n;
                          result := False;
                        end;
                    end;
                  end
                else
                  begin
                    result := False;
                    Break;
                  end;

                dec(FLAG);
              until not((BC.Stat = Failure) or (BC.Stat = Pass)) and
                  (FLAG <> 0);
            end;
        end;
      end
    else
      result := False;
  end;

function TLowLvlUAI.WrFile(FileName: BYTE; pDat: pointer;
  var len: BYTE): Boolean;
  var
    FLAG: BYTE;
    AStr, buff: AnsiString;
    str: string;
  begin
    result := False;
    if BC.TCom = UNDEF then
      begin
        Err := UAI(32, '');
        if Err = OK then
          begin
            if BC.TCom = UNDEF then
              begin
                result := False;
                exit;
              end;
          end
        else
          begin
            result := False;
            exit;
          end;
      end;

    setlength(buff, len * 2);
    HexToStr(pDat, @buff[1], len);

    str := IntToStr(FileName);
    StrToAnsi(AStr, str);
    buff := AStr + ';' + buff;

    Err := UAI(43, buff);
    if Err = OK then
      begin
        if (BC.TCom = UAI_FULL) then
          begin
            FLAG := 20;
            repeat
              sleep(20);
              Err := UAI(35, '');
              if Err = OK then // читаем регистр статуса
                begin
                  case BC.Stat of
                    Success:
                      begin
                        result := True;
                      end;
                    Pass:
                      ;
                    Failure:
                      ;

                    Unsuccessful:
                      begin
                        val.n := 1;
                        Err := UAI(36, '');
                        len := val.n;
                        Err := UAI(35, '');
                        result := False;
                      end;
                  end;
                end
              else
                begin
                  result := False;
                  Break;
                end;

              dec(FLAG);
            until not((BC.Stat = Failure) or (BC.Stat = Pass)) and (FLAG <> 0);
          end
        else
          result := True;
      end
    else
      result := False;
  end;

function TLowLvlUAI.GetErr(): EnErr;
  begin
    GetErr := Err;
  end;

function TLowLvlUAI.getStrErr: string;
  begin
    case Err of
      OK:
        getStrErr := 'успешное завершение';
      EBUSY:
        getStrErr := 'ком порт занят';
      EUNDEFTYPE:
        getStrErr := 'неподдерживаемый тип прибора';
      EIMPOSSIBLE:
        getStrErr := 'невозможное поле';
      EEXCEPT:
        getStrErr := 'произола ошибка преобразования';
      EPROTOCOL:
        getStrErr := 'неподдерживаемый протокол';
      EFAIL:
        getStrErr := 'завершение операции не привело к успеху';
      EDISASSEMBLE:
        getStrErr := 'ошибка преобразования';
      ETOT:
        getStrErr := 'нет ответа';
    end;
  end;

function TLowLvlUAI.getStrErrUAI: string;
  begin
    case BC.CRet of
      EZERO:
        getStrErrUAI := '$0 успешное завершение';
      ENOCMD:
        getStrErrUAI := '$1 команда не найдена';
      EINVALCMD:
        getStrErrUAI := '$2 неверный формат команды';
      ENOPAR:
        getStrErrUAI := '$3 параметр не найден';
      EPERM:
        getStrErrUAI := '$4 нарушение прав доступа';
      EACCESS:
        getStrErrUAI := '$5 ошибка чтения или записи';
      ERANGE:
        getStrErrUAI := '$6 выход параметра за границу диапазона';
      ENOENT:
        getStrErrUAI := '$7 файл не найден';
      ENOMEM:
        getStrErrUAI := '$8 нехватает памяти';
      EBADF:
        getStrErrUAI := '$9 файловая ошибка';
      ENOSPC:
        getStrErrUAI := '$10 отсутствует место на физическом носителе';
    end;
  end;

procedure TLowLvlUAI.StrToArrayMass(s: AnsiString);
  var
    i: Integer;
  begin
    for i := 0 to length(s) - 1 do
      val.arr[i] := BYTE(s[i + 1]);
    val.n := length(s) div 2;
  end;

// function TLowLvlUAI.CreatResponceMessage():AnsiString;
// function CrtFlt(r:integer):string;
// var
// g:string;
// c0,c1:Char;
// Flt:TFlt;
// begin
// Flt.ValFlt:=Random(r);
// g:=IntToHex(Flt.ValInt,8);
// c0:=g[1];c1:=g[2]; g[1]:=g[7];g[2]:=g[8];g[7]:=c0;g[8]:=c1;
// c0:=g[3];c1:=g[4]; g[3]:=g[5];g[4]:=g[6];g[5]:=c0;g[6]:=c1;
// result:=g;
// end;
// var
// s:string;
// NetAdd:integer;
// begin
// NetAdd:=self.GetNAdd();
// case self.TpeID of
// eByte:begin s:=IntToStr(NetAdd)+';'+'01'+';';
// end;
// eWord:begin s:=IntToStr(NetAdd)+';'+'0200'+';';
// end;
// eDWord:begin s:=IntToStr(NetAdd)+';'+'621E0000'+';';
// end;
// eDSInt:begin
// end;
// eZavNom:begin s:=IntToStr(NetAdd)+';'+'10270000'+';';
// end;
// eSmallInteger:begin
// end;
// eSingle:begin s:=IntToStr(NetAdd)+';'+CrtFlt(10)+';';    end;
// eRawVal:begin s:=IntToStr(NetAdd)+';'+'00'+CrtFlt(1000)+';';
// end;
// eCCM:begin
// end;
// eCC_SR:begin
// end;
// eCC_PQ:begin
// end;
// eCC_ABCDEF:begin
// end;
// eResalt:begin s:=IntToStr(NetAdd)+';'+'00'+CrtFlt(100)+';';
// end;
// eStr:begin s:=IntToStr(NetAdd)+';'+'56332E30332E3030204A616E2031322032303131200031393A'+';';
// end;
// eCC_XYZ:begin
// end;
// eCC_FAST:begin
// end;
// end;
/// /s:=AnsiChar('я')+'!'+s+IntToStr(CalcCRC16Table(s,length(s)))+#13;
//
// result:=s;
// end;

end.
