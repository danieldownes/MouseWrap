//---------------------------------------------------------------------------

#ifndef optionH
#define optionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#include <string>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TCheckBox *chkStartupLoad;
	TCheckBox *chkRecogActive;
	TGroupBox *GroupBox2;
	TButton *Button1;
	TButton *Button2;
	TUpDown *sclRecogTimeout;
	TEdit *txtRecogTimeout;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TComboBox *lstHorizontal;
	TComboBox *lstVertical;
	TLabel *Label4;
	TLabel *Label5;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall sclRecogTimeoutClick(TObject *Sender, TUDBtnType Button);
	void __fastcall txtRecogTimeoutChange(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif

