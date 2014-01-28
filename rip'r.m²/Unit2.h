//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// Composants gérés par l'EDI
        TImage *map;
        TButton *saveBtn;
        void __fastcall FormResize(TObject *Sender);
        void __fastcall saveBtnClick(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
        __fastcall TForm2(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif
