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
class TfrmOptions : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TCheckBox *chkStartupLoad;
	TCheckBox *chkRecogActive;
	TButton *Button1;
	TButton *Button2;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
//	void __fastcall sclRecogTimeoutClick(TObject *Sender, TUDBtnType Button);
//	void __fastcall txtRecogTimeoutChange(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmOptions(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmOptions *frmOptions;
//---------------------------------------------------------------------------
#endif

