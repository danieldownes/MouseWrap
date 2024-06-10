//---------------------------------------------------------------------------

#ifndef frmLoader_WindowH
#define frmLoader_WindowH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
//---------------------------------------------------------------------------
class TfrmLoader : public TForm
{
__published:	// IDE-managed Components
        TImage *loader;
        TTimer *Timer;
        TLabel *Label1;
        void __fastcall TimerTimer(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfrmLoader(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmLoader *frmLoader;
//---------------------------------------------------------------------------
#endif
