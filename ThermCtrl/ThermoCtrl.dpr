program ThermoCtrl;

uses
  Forms,
  ThermCtrl in 'ThermCtrl.pas' {Form3},
  ThHCC in 'ThHCC.pas',
  LowLvlCom in 'LowLvlCom.PAS',
  MdlLvlCom in 'MdlLvlCom.pas',
  Common in 'Common.pas',
  Time in 'Time.pas' {FTime};

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TForm3, Form3);
  Application.CreateForm(TFTime, FTime);
  Application.Run;
end.
