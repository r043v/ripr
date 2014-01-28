//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class Tpreview : public TForm
{
__published:	// Composants gérés par l'EDI
        TImage *Image1;
        TComboBox *ComboShowOnlyColor;
        void __fastcall Image1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall Image1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall Image1MouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall ComboShowOnlyColorChange(TObject *Sender);
private:	// Déclarations de l'utilisateur
public:		// Déclarations de l'utilisateur
        __fastcall Tpreview(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tpreview *preview;
//---------------------------------------------------------------------------
#endif
