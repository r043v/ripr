//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Graphics.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// Composants gérés par l'EDI
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TImage *lastFrmImg;
        TImage *output;
        TPageControl *tabControl;
        TTabSheet *TabSheet3;
        TTabSheet *TabSheet4;
        TEdit *Edit1;
        TEdit *Edit2;
        TEdit *Edit3;
        TEdit *Edit4;
        TEdit *Edit5;
        TLabel *Label1;
        TImage *indexPalImg;
        TLabel *lblCurColor;
        TPopupMenu *PopupMenu1;
        TMenuItem *inpinkonlyinindex1;
        TMenuItem *onallbmp1;
        TMenuItem *asindexcolor1;
        TGroupBox *GroupBox1;
        TImage *tilesColorImg;
        TGroupBox *GroupBox4;
        TImage *spritesColorImg;
        TGroupBox *GroupBox2;
        TImage *suspiciousColorImg;
        TMenuItem *isatilecolor1;
        TMenuItem *isaspritecolor1;
        TMenuItem *markassuspicious1;
        TMenuItem *settranspcolor1;
        TGroupBox *GroupBox3;
        TImage *tlSprColorImg;
        TMenuItem *isthetwice;
        TLabel *Label2;
        TLabel *Label3;
        TButton *Button1;
        TButton *Button2;
        TButton *Button3;
        TButton *Button4;
        TCheckBox *searchForMap;
        TLabel *Label4;
        TLabel *Label5;
        TComboBox *ComboShowOnlyColor;
        TLabel *lblWhatColor;
        TCheckBox *chkIgnore;
        TLabel *Label6;
        TLabel *Label7;
        TMemo *Memo1;
        void __fastcall lastFrmImgMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall lastFrmImgMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall lastFrmImgMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall indexPalImgMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall onallbmp1Click(TObject *Sender);
        void __fastcall asindexcolor1Click(TObject *Sender);
        void __fastcall ComboShowOnlyColorChange(TObject *Sender);
        void __fastcall isatilecolor1Click(TObject *Sender);
        void __fastcall isaspritecolor1Click(TObject *Sender);
        void __fastcall markassuspicious1Click(TObject *Sender);
        void __fastcall settranspcolor1Click(TObject *Sender);
        void __fastcall tilesColorImgDblClick(TObject *Sender);
        void __fastcall spritesColorImgDblClick(TObject *Sender);
        void __fastcall isthetwiceClick(TObject *Sender);
        void __fastcall lastFrmImgDblClick(TObject *Sender);
        void __fastcall tabControlChange(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
private:	// Déclarations de l'utilisateur
        void virtual __fastcall WMDropFiles(TWMDropFiles &message);
public:		// Déclarations de l'utilisateur
        __fastcall TForm1(TComponent* Owner);
  // to drop a file      
  BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMDropFiles)
  END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
