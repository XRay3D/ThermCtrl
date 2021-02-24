unit Time;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Mask; // , RzEdit, RzSpnEdt;

type
  TFTime = class(TForm)
    bttYes: TButton;
    bttNo: TButton;
    GroupBox1: TGroupBox;
    Label2: TLabel;
    Label3: TLabel;
    private
      { Private declarations }
    public
      { Public declarations }
  end;

var
  FTime: TFTime;

implementation

{$R *.dfm}

end.
