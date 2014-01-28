//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm2::FormResize(TObject *Sender)
{ map->Height = ClientHeight ;
  map->Width  = ClientWidth ;
}
//---------------------------------------------------------------------------
void __fastcall TForm2::saveBtnClick(TObject *Sender)
{
     map->Picture->SaveToFile("c:\\map.bmp");
}
//---------------------------------------------------------------------------

