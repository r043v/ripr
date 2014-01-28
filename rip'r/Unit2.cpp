//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tpreview *preview;
//---------------------------------------------------------------------------
__fastcall Tpreview::Tpreview(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
extern void getRealCursorPos(TImage*bmp,int *X, int *Y) ;
//int xAtClick,yAtClick,click=0;

struct index
{ int x,y,szx,szy,enable ;
  int *line[4] ; // 0,up 1,down 2,left 3,right
  int *color, colorNb ;
};

extern struct index frmIndex ;
extern void drawIndex(void);

void __fastcall Tpreview::Image1MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{ // click=0 ;
    getRealCursorPos(Image1,&X,&Y) ;
    int x = frmIndex.x ; int y = frmIndex.y ;
        if(X > Image1->Picture->Width  ||
           Y > Image1->Picture->Height ||
           x==X || y==Y || X<0 || Y<0 || frmIndex.enable!=1) return ;

        frmIndex.enable=2 ; // index is set
        if(X>x) frmIndex.szx = X-x ;
        else  { frmIndex.szx = x-X ;
                frmIndex.x   = X ;
              }
        if(Y>y) frmIndex.szy = Y-y ;
        else  { frmIndex.szy = y-Y ;
                frmIndex.y   = Y ;
              }
        drawIndex();
        Image1->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall Tpreview::Image1MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{ /*if(click)
  { if(X < xAtClick)preview->Left -= (xAtClick - X);
     else  if(X > xAtClick)preview->Left += ( X - xAtClick);

    if(Y < yAtClick)preview->Top -= (yAtClick - Y);
     else  if(Y > yAtClick)preview->Top += ( Y - yAtClick);
  } */

  getRealCursorPos(Image1,&X,&Y) ;

  Form1->Label2->Caption = X ;
  Form1->Label3->Caption = Y ;
}
//---------------------------------------------------------------------------

void __fastcall Tpreview::Image1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{   //xAtClick=X; yAtClick=Y; click=1;

    frmIndex.enable=1 ; // edit index pos&size
    getRealCursorPos(Image1,&X,&Y);
    frmIndex.x=X ; frmIndex.y=Y ;
}
//---------------------------------------------------------------------------
void __fastcall Tpreview::FormResize(TObject *Sender)
{     Image1->Height = preview->ClientHeight ;
      Image1->Width  = preview->ClientWidth ;
}
//---------------------------------------------------------------------------

void __fastcall Tpreview::ComboShowOnlyColorChange(TObject *Sender)
{  Form1->ComboShowOnlyColor->ItemIndex = ComboShowOnlyColor->ItemIndex ;
   Form1->ComboShowOnlyColorChange(Sender);
}
//---------------------------------------------------------------------------

