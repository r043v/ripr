//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void setBuffer(int *buf);
void drawIndex(void);
void loadIndex(void);
int *getBuffer(TImage *bmp,int*Sx,int*Sy);
void addColorInList(int *list, int *nb, int color);
void deleteColorInList(int *list, int *nb, int color);
void drawColorArray(TImage *bmp,int *array,int nb);
void searchForNewColor(int *buffer, int sze);
void searchForTilesColors(void);
void refreshOutput(void);
void getRealCursorPos(TImage*bmp,int *X, int *Y);

void getRealCursorPos(TImage*bmp,int *X, int *Y)
{ float imgSizex = bmp->Picture->Width ;
  float wndSizex = bmp->Width ;
  float ratiox ;
  if(wndSizex>imgSizex) ratiox = imgSizex/wndSizex ;
   else                 ratiox = wndSizex/imgSizex ;

  float imgSizey = bmp->Picture->Height ;
  float wndSizey = bmp->Height ;
  float ratioy ;
  if(wndSizey>imgSizey) ratioy = imgSizey/wndSizey ;
   else                 ratioy = wndSizey/imgSizey ;

  (*X) *= ratiox ;  (*Y) *= ratioy ;
}

int *tilesColors=0,*spritesColors=0,*suspiciousColors=0,*twiceColors=0 ;
int tilesColorNb=0,spritesColorNb=0,suspiciousColorNb=0,twiceColorNb=0 ;

struct index
{ int x,y,szx,szy,enable ;
  int *line[4] ; // 0,up 1,down 2,left 3,right
  int *color, colorNb ;
};

struct index frmIndex ;
int indexBoundColor  = 0x00ff00 ;
int transparentColor = 0xff00ff ;

void deleteColorInList(int *list, int *nb, int color)
{ if(!list || !*nb) return ;
  for(int c=0;c<*nb;c++)
   if(list[c]==color)
   { *nb -= 1 ; for(c=c;c<*nb;c++) list[c]=list[c+1];
     return ;
   }
}

void addColorInList(int *list, int *nb, int color)
{ deleteColorInList(list,nb,color);
  if(color != transparentColor) { list[*nb]=color ; *nb += 1 ; }
}

int  searchColorInList(int *list, int *nb, int color)
{ if(!list || !*nb) return 0 ;
  for(int c=0;c<*nb;c++)
   if(list[c]==color)
    return c+1 ;
  return 0 ;
}

int isBgColor(int color)
{ return (searchColorInList(tilesColors,&tilesColorNb,color) || searchColorInList(twiceColors,&twiceColorNb,color));
}

int searchInTwiceAndSpriteColor(int color)
{ return (searchColorInList(spritesColors,&spritesColorNb,color) || searchColorInList(twiceColors,&twiceColorNb,color)) ;
}

void drawColorArray(TImage *bmp,int *array,int nb)
{    if(!bmp || !array || nb<0 || nb>255) return ;

     bmp->Picture->Bitmap->PixelFormat = pf32bit ; // set deep to 32 bits
     int lnb = nb>>2 ;
     bmp->Picture->Graphic->Height = lnb + (nb>(lnb<<2))*1 ;
     bmp->Picture->Graphic->Width  = 4 ;
     if(!array || !nb) return ;
     int c=0, cpt=0 ;
     int l=0, *ptr = (int*)(bmp->Picture->Bitmap->ScanLine[l]);
               for(lnb=0;lnb<4;lnb++)ptr[lnb]=0xd6d3ce ;
     while(c<nb)
     { ptr[c%4] = array[c++] ;
       if(++cpt == 4)
        if(++l < bmp->Picture->Graphic->Height)
        { ptr = (int*)(bmp->Picture->Bitmap->ScanLine[l]);
          for(lnb=0;lnb<4;lnb++) ptr[lnb]=0xd6d3ce ; cpt=0 ;
        }
     }; bmp->Refresh();
}

void searchForNewColor(int *buffer, int sze)
{ int *clrArray, *clrNb, found=0 ;
  static int firstTime=1 ;
  for(int n=0;n<sze;n++) // for eatch pixel
  { clrArray = spritesColors ; clrNb = &spritesColorNb ;
    for(int c=0;c<*clrNb;c++)
     if(buffer[n]==clrArray[c]) { found=1 ; break ; }
    if(!found)
    { clrArray = tilesColors ; clrNb = &tilesColorNb ;
      for(int c=0;c<*clrNb;c++)
       if(buffer[n]==clrArray[c]) { found=1 ; break ; }
    }
    if(!found)
    { clrArray = suspiciousColors ; clrNb = &suspiciousColorNb ;
      for(int c=0;c<*clrNb;c++)
       if(buffer[n]==clrArray[c]) { found=1 ; break ; }
    }
    if(!found)
    { clrArray = twiceColors ; clrNb = &twiceColorNb ;
      for(int c=0;c<*clrNb;c++)
       if(buffer[n]==clrArray[c]) { found=1 ; break ; }
    }
        if(!found) { if(firstTime) tilesColors[tilesColorNb++] = buffer[n] ;
                      else suspiciousColors[suspiciousColorNb++] = buffer[n] ;
                   } else found=0 ;
  };
  firstTime=0 ;
}

int *frmBuffer = 0 , *savedFrmBf=0 ;//, *savedSaveFrmBuffer=0 ; // the buffer, his save, and the save of the save :D
int  frmSx, frmSy  ;

int *outBuffer = 0 ;
int outSx, outSy   ;
int curentInOutx, curentInOuty ;

int notAllowClassicTileSizeAsMove = 0  ;
int minPercentForFoundIndex       = 95 ;

int isIndex(int*img, int sx, int sy, int x, int y)
{ int c,ok=0,*ptr = img + x + y*sx, downSz=sx*(frmIndex.szy - 1), nbSpriteColorFound=0 ;
  for(c=0;c<frmIndex.szx;c++)
  { if(ptr[c] == (frmIndex.line[0])[c])ok++ ;        // see up
     else if(searchInTwiceAndSpriteColor(ptr[c])){ ok++ ; nbSpriteColorFound++; }
     // else search in sprite & twice color, if same color found -> ok++  and  nbSpriteColorFound++
    if(ptr[c+downSz] == (frmIndex.line[1])[c])ok++ ; // and down
     else if(searchInTwiceAndSpriteColor(ptr[c+downSz])){ ok++ ; nbSpriteColorFound++; }
  };
  for(int l=0;l<frmIndex.szy;l++)
   { ptr = img + sx*(l+y)+x ;
     if(*ptr==(frmIndex.line[2])[l])ok++ ;
      else if(searchInTwiceAndSpriteColor(*ptr)){ ok++ ; nbSpriteColorFound++; }
     if(*(ptr + frmIndex.szx - 1)==(frmIndex.line[3])[l])ok++ ;
      else if(searchInTwiceAndSpriteColor(*(ptr + frmIndex.szx - 1))){ ok++ ; nbSpriteColorFound++; }
   };   // if some sprite color, not allow it -------------> need to really check if it's the index
  if(nbSpriteColorFound > frmIndex.szx + frmIndex.szy) return 0 ;
  return (ok >= (( (frmIndex.szx)*2+(frmIndex.szy - 2)*2)*(minPercentForFoundIndex*10))>>10) ; // if some of good pixel, assuming it's the bg.
}

void checkBgMove(int*img, int sx, int sy, int x, int y, int *dx, int *dy,int searchLimit = 8)
{ int c ;
  if(!isIndex(img,sx,sy,x,y)) // bg has move, search for index in the 3 pixel around x.y point
  { for(int xx=-searchLimit;xx<=searchLimit;xx++)
     for(int yy=-searchLimit;yy<=searchLimit;yy++)
      if(xx!=0 || yy!=0)
       if(isIndex(img,sx,sy,x+xx,y+yy))
        { *dx=xx;*dy=yy ;
           if(notAllowClassicTileSizeAsMove)
           { if(xx==8 || -xx==8 || yy==8 || -yy==8) *dx=*dy=0xff;
              else if(xx==16 || -xx==16 || yy==16 || -yy==16) *dx=*dy=0xff;
                    else if(xx==32 || -xx==32 || yy==32 || -yy==32) *dx=*dy=0xff;
           }
          return ;
        }
  } else { *dx=*dy=0 ; return ; }
  *dx=*dy=0xff ; // 0xff > not found
  return ;
}

void searchForTilesColors(void)
{ if(!tilesColors)      tilesColors      = (int*)malloc(1024*4);
  if(!spritesColors)    spritesColors    = (int*)malloc(1024*4);
  if(!suspiciousColors) suspiciousColors = (int*)malloc(1024*4);
  if(!twiceColors)      twiceColors      = (int*)malloc(1024*4);
  searchForNewColor(frmBuffer,frmSx*frmSy);
  drawColorArray(Form1->tilesColorImg,tilesColors,tilesColorNb);
  drawColorArray(Form1->spritesColorImg,spritesColors,spritesColorNb);
  drawColorArray(Form1->suspiciousColorImg,suspiciousColors,suspiciousColorNb);
  drawColorArray(Form1->tlSprColorImg,twiceColors,twiceColorNb);
}

int *getBuffer(TImage *bmp=Form1->lastFrmImg, int*Sx=&frmSx, int*Sy=&frmSy)
{ int sx = bmp->Picture->Graphic->Width ;
  int sy = bmp->Picture->Graphic->Height ;
  int *buffer = (int*)malloc(sx*sy*4), *ptr ;
  *Sx=sx ; *Sy=sy ;
  bmp->Picture->Bitmap->PixelFormat = pf32bit ; // set deep to 8 bits
   for(int y=0;y<sy;y++) // fill buffer
    { ptr = (int*)(bmp->Picture->Bitmap->ScanLine[y]) ;
      for(int x=0;x<sx;x++) buffer[y*sx+x] = ptr[x] ;
    };
  return buffer ;
}

void setBuffer(int *buf)
{ int sx = Form1->lastFrmImg->Picture->Graphic->Width ;
  int sy = Form1->lastFrmImg->Picture->Graphic->Height ;
  int *ptr ; if(!buf) return ;
  Form1->lastFrmImg->Picture->Bitmap->PixelFormat = pf32bit ; // set deep to 8 bits
  if(buf == frmBuffer && (Form1->ComboShowOnlyColor->ItemIndex)>0)
   {   int *cArray, cNb, *cArray2=0, cNb2=0, found, clr ;

       switch(Form1->ComboShowOnlyColor->ItemIndex)
       { case 1 : cArray=tilesColors;      cNb=tilesColorNb;      cArray2=twiceColors;  cNb2=twiceColorNb; break ; // bg & twice colors
         case 2 : cArray=spritesColors;    cNb=spritesColorNb;    cArray2=twiceColors;  cNb2=twiceColorNb; break ; // sprite & twice colors
         case 3 : cArray=twiceColors;      cNb=twiceColorNb; break ; // only twice colors
         case 4 : cArray=suspiciousColors; cNb=suspiciousColorNb; break ; // only suspicious colors
         case 5 : cArray=tilesColors;      cNb=tilesColorNb; break ;  // only tiles colors
         case 6 : cArray=spritesColors;    cNb=spritesColorNb; break ; // only sprites colors
       };
       if(!cArray) return ;
       for(int y=0;y<sy;y++) // fill buffer
        { ptr = (int*)(Form1->lastFrmImg->Picture->Bitmap->ScanLine[y]) ;
          if(ptr) for(int x=0;x<sx;x++)
                   { clr = buf[y*sx+x] ; found=0 ;
                     for(int c=0;c<cNb;c++) if(clr == cArray[c]) { found=1; break; }
                     if(!found && cArray2 && cNb2)
                      for(int c=0;c<cNb2;c++) if(clr == cArray2[c]) { found=1; break; }
                     if(found) ptr[x] = clr; else ptr[x] = transparentColor ;
                   };
        };
   } else {
       for(int y=0;y<sy;y++) // fill buffer
        { ptr = (int*)(Form1->lastFrmImg->Picture->Bitmap->ScanLine[y]) ;
          if(ptr) for(int x=0;x<sx;x++) ptr[x] = (int)buf[y*sx+x] ;
        };
   }
}

void loadIndex(void)
{ setBuffer(frmBuffer);
  int **line = frmIndex.line, color[256], colorNb=1 ;
  int  c=4 ; while(--c) if(line[c]) free(line[c]);
       c=2 ; while(c) line[--c] = (int*)malloc((frmIndex.szx)*4);
       c=2 ; while(c) line[(--c)+2] = (int*)malloc((frmIndex.szy)*4);
  int *ptr = (int*)(Form1->lastFrmImg->Picture->Bitmap->ScanLine[frmIndex.y]) ;
       ptr+= frmIndex.x ;
        for(c=0;c<frmIndex.szx;c++) (line[0])[c]=*ptr++ ; // fill up index
       ptr = (int*)(Form1->lastFrmImg->Picture->Bitmap->ScanLine[frmIndex.y+frmIndex.szy-1]) ;
       ptr+= frmIndex.x ;
        for(c=0;c<frmIndex.szx;c++) (line[1])[c]=*ptr++ ; // fill down index

       for(c=0;c<frmIndex.szy;c++)
        { ptr = (int*)(Form1->lastFrmImg->Picture->Bitmap->ScanLine[frmIndex.y+c]) ;
          (line[2])[c]= ptr[frmIndex.x] ; // fill left
          (line[3])[c]= ptr[frmIndex.x+frmIndex.szx-1] ; // and right
        };
      int n,found=0 ; *color=**line ;
      for(c=0;c<frmIndex.szx;c++) // count and get colors of up and down index line
        { for(n=0;n<colorNb;n++)
           if((line[0])[c] == color[n]) { found=1 ; break ; }
          if(!found) color[colorNb++] = (line[0])[c] ; else found=0 ;
          for(n=0;n<colorNb;n++)
           if((line[1])[c] == color[n]) { found=1 ; break ; }
          if(!found) color[colorNb++] = (line[1])[c] ; else found=0 ;
        };
      for(c=0;c<frmIndex.szy;c++) // and do the same for left & right
        { for(n=0;n<colorNb;n++)
           if((line[2])[c] == color[n]) { found=1 ; break ; }
          if(!found) color[colorNb++] = (line[2])[c] ; else found=0 ;
          for(n=0;n<colorNb;n++)
           if((line[3])[c] == color[n]) { found=1 ; break ; }
          if(!found) color[colorNb++] = (line[3])[c] ; else found=0 ;
        };
      frmIndex.colorNb = colorNb;
      frmIndex.color = (int*)malloc(colorNb*4);
      memcpy(frmIndex.color,color,colorNb*4);
}

void drawIndex(void)
{  loadIndex();
   int *ptr, color=indexBoundColor ;
   Form1->lastFrmImg->Picture->Bitmap->PixelFormat = pf32bit ;
   if(frmIndex.enable == 2) // draw 4 pink line to see index pos
   { ptr = (int*)(Form1->lastFrmImg->Picture->Bitmap->ScanLine[frmIndex.y]) ;
     for(int x=frmIndex.x;x<frmIndex.x+frmIndex.szx;x++) ptr[x]=color ;
     for(int y=frmIndex.y +1;y<frmIndex.y+frmIndex.szy -1;y++)
     { ptr = (int*)(Form1->lastFrmImg->Picture->Bitmap->ScanLine[y]) ;
       ptr[frmIndex.x]=color ; ptr[frmIndex.x+frmIndex.szx - 1]=color ;
     };
     ptr = (int*)(Form1->lastFrmImg->Picture->Bitmap->ScanLine[frmIndex.y+frmIndex.szy - 1]) ;
     for(int x=frmIndex.x;x<frmIndex.x+frmIndex.szx;x++) ptr[x]=color ;
     // draw index colors in the right down img.
     Form1->indexPalImg->Picture->Bitmap->PixelFormat = pf32bit ; // set deep to 32 bits
     int lnb = (frmIndex.colorNb)>>2 ;
     Form1->indexPalImg->Picture->Graphic->Height = lnb + ((frmIndex.colorNb)>(lnb<<2))*1 ;
     Form1->indexPalImg->Picture->Graphic->Width  = 4 ;
     Form1->indexPalImg->Top = Form1->lblCurColor->Top - (Form1->indexPalImg->Picture->Graphic->Height)*24 ;
     Form1->Edit3->Text = frmIndex.colorNb ;
     Form1->Edit4->Text = frmIndex.szx ;
     Form1->Edit5->Text = frmIndex.szy ;
     int c=0, cpt=0 ;
     int l=0 ; ptr = (int*)(Form1->indexPalImg->Picture->Bitmap->ScanLine[l]);
               for(lnb=0;lnb<4;lnb++)ptr[lnb]=0xd6d3ce ; // 0xd6d3ce -> window gray 
     while(c<frmIndex.colorNb) // show color under index
     { ptr[c%4] = frmIndex.color[c++] ;
       if(++cpt == 4)
        if(++l < Form1->indexPalImg->Picture->Graphic->Height)
        { ptr = (int*)(Form1->indexPalImg->Picture->Bitmap->ScanLine[l]);
          for(lnb=0;lnb<4;lnb++)ptr[lnb]=0xd6d3ce ; cpt=0 ;
        }
     };
     Form1->lastFrmImg->Refresh();
     Form1->indexPalImg->Refresh();
     Form1->Label1->Caption = isIndex(frmBuffer,frmSx,frmSy,frmIndex.x,frmIndex.y);
     if(preview) if(preview->Visible) preview->Image1->Picture->Assign(Form1->lastFrmImg->Picture) ;
   }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::lastFrmImgMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{      X = X - lastFrmImg->Left - ((lastFrmImg->Width  - lastFrmImg->Picture->Width)>>1)  ;
       Y = Y - lastFrmImg->Top  - ((lastFrmImg->Height - lastFrmImg->Picture->Height)>>1) ;
     if(X > lastFrmImg->Picture->Width  ||
        Y > lastFrmImg->Picture->Height || X<0 || Y<0) return ;
        frmIndex.enable=1 ; // edit index pos&size
     frmIndex.x=X ; frmIndex.y=Y ;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::lastFrmImgMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{    int x = frmIndex.x ; int y = frmIndex.y ;   
       X = X - lastFrmImg->Left - ((lastFrmImg->Width  - lastFrmImg->Picture->Width)>>1)  ;
       Y = Y - lastFrmImg->Top  - ((lastFrmImg->Height - lastFrmImg->Picture->Height)>>1) ;
        if(X > lastFrmImg->Picture->Width  ||
           Y > lastFrmImg->Picture->Height ||
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
        lastFrmImg->Refresh();
}
//---------------------------------------------------------------------------

void refreshOutput(void)
{  for(int y=0;y<outSy;y++)
   { int*p     = (int*)Form1->output->Picture->Bitmap->ScanLine[y];
     int *iptr = outBuffer + outSx*y ;
      for(int x=0;x<outSx;x++)
       if(!isBgColor(iptr[x])) p[x]=0xff00ff ;
        else                   p[x]=iptr[x] ;  
   };
}

void __fastcall TForm1::FormCreate(TObject *Sender)
{  frmIndex.enable=2 ; frmIndex.x=frmIndex.y=frmIndex.szx=frmIndex.szy=42 ;
   frmBuffer = getBuffer();
   searchForTilesColors();
   output->Picture->Assign(lastFrmImg->Picture);
   outBuffer = getBuffer(output,&outSx,&outSy);
   Label2->Caption = outSx ;
   Label3->Caption = outSy ;
   curentInOutx=curentInOuty=0 ;
   refreshOutput();
   drawIndex();
   DragAcceptFiles(Handle, True); // accept drag and drop
}
//---------------------------------------------------------------------------
void __fastcall TForm1::lastFrmImgMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{  X = X - lastFrmImg->Left - ((lastFrmImg->Width  - lastFrmImg->Picture->Width)>>1);
   Y = Y - lastFrmImg->Top  - ((lastFrmImg->Height - lastFrmImg->Picture->Height)>>1);
   Edit1->Text = X ; Edit2->Text = Y ;
   if(frmIndex.enable!=1) return ;
   Edit4->Text = abs(frmIndex.x-X) ;
   Edit5->Text = abs(frmIndex.y-Y) ;
}
//---------------------------------------------------------------------------
int selectedColor ;

void drawCurentColorType(void)
{  if(searchColorInList(tilesColors,&tilesColorNb,selectedColor))
    {  Form1->lblWhatColor->Caption = "tiles color" ; return ; }
   if(searchColorInList(twiceColors,&twiceColorNb,selectedColor))
    {  Form1->lblWhatColor->Caption = "twice color" ; return ; }
   if(searchColorInList(spritesColors,&spritesColorNb,selectedColor))
    {  Form1->lblWhatColor->Caption = "sprites color" ; return ; }
   if(searchColorInList(suspiciousColors,&suspiciousColorNb,selectedColor))
    {  Form1->lblWhatColor->Caption = "suspect color" ; return ; }
   if(selectedColor == transparentColor)
    {  Form1->lblWhatColor->Caption = "transparent color" ; return ; }
   Form1->lblWhatColor->Caption = "unreferenced color" ;
}

#define setCurColorCaption(buffer) { selectedColor=(buffer)[X+4*Y] ;\
                                     lblCurColor->Caption = lblCurColor->Caption.sprintf("0x%x",selectedColor);\
                                     drawCurentColorType();\
                                   } else { lblCurColor->Caption = ""; PopupMenu1->AutoPopup=0 ; }

void __fastcall TForm1::indexPalImgMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{  X/=24 ; Y/=24 ; selectedColor=transparentColor ; // get real x and y (stretch -> 1pixel = 24pixel)

   isatilecolor1->Enabled=1 ; // enable all popup choice
   isaspritecolor1->Enabled = 1 ;
   markassuspicious1->Enabled = 1 ;
   isthetwice->Enabled = 1 ;
   PopupMenu1->AutoPopup=1 ;

       // check sender and disable option who mark color as sender
              if(Sender == indexPalImg)
               {  if(X+4*Y < frmIndex.colorNb) setCurColorCaption(frmIndex.color)
               }
      else    if(Sender == tilesColorImg)
               {  if(X+4*Y < tilesColorNb) setCurColorCaption(tilesColors)
                  isatilecolor1->Enabled=0 ;
               }
      else    if(Sender == spritesColorImg)
               {  if(X+4*Y < spritesColorNb) setCurColorCaption(spritesColors)
                  isaspritecolor1->Enabled=0 ;
               }
      else    if(Sender == suspiciousColorImg)
               {  if(X+4*Y < suspiciousColorNb) setCurColorCaption(suspiciousColors)
                  markassuspicious1->Enabled=0 ;
               }
      else    if(Sender == tlSprColorImg)
               {  if(X+4*Y < twiceColorNb) setCurColorCaption(twiceColors)
                  isthetwice->Enabled=0 ;
               }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::onallbmp1Click(TObject *Sender)
{ for(int n=0;n<lastFrmImg->Picture->Width * lastFrmImg->Picture->Height;n++)
    if(frmBuffer[n]==selectedColor)frmBuffer[n]=transparentColor ;
  drawIndex();
  setBuffer(frmBuffer);
  tilesColorNb=spritesColorNb=suspiciousColorNb=0 ;
  searchForTilesColors();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::asindexcolor1Click(TObject *Sender)
{ indexBoundColor = selectedColor ;
  drawIndex();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboShowOnlyColorChange(TObject *Sender)
{  if(Sender != preview->ComboShowOnlyColor)
   preview->ComboShowOnlyColor->ItemIndex = ComboShowOnlyColor->ItemIndex ;
   if(frmIndex.enable!=2)
   { frmIndex.szx = 16 ;
     frmIndex.szy = 16 ;
     frmIndex.enable = 2 ;
   }
    drawIndex();
}
//---------------------------------------------------------------------------

void drawColorsArray(void)
{       drawColorArray(Form1->tilesColorImg,tilesColors,tilesColorNb);
        drawColorArray(Form1->spritesColorImg,spritesColors,spritesColorNb);
        drawColorArray(Form1->suspiciousColorImg,suspiciousColors,suspiciousColorNb);
        drawColorArray(Form1->tlSprColorImg,twiceColors,twiceColorNb);
        drawIndex();
}

void __fastcall TForm1::isatilecolor1Click(TObject *Sender)
{       addColorInList(tilesColors,&tilesColorNb,selectedColor);
        deleteColorInList(twiceColors,&twiceColorNb,selectedColor);
        deleteColorInList(spritesColors,&spritesColorNb,selectedColor);
        deleteColorInList(suspiciousColors,&suspiciousColorNb,selectedColor);
        drawColorsArray();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::isaspritecolor1Click(TObject *Sender)
{       addColorInList(spritesColors,&spritesColorNb,selectedColor);
        deleteColorInList(twiceColors,&twiceColorNb,selectedColor);
        deleteColorInList(tilesColors,&tilesColorNb,selectedColor);
        deleteColorInList(suspiciousColors,&suspiciousColorNb,selectedColor);
        drawColorsArray();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::markassuspicious1Click(TObject *Sender)
{       deleteColorInList(suspiciousColors,&suspiciousColorNb,selectedColor);
        addColorInList(suspiciousColors,&suspiciousColorNb,selectedColor);
        deleteColorInList(twiceColors,&twiceColorNb,selectedColor);
        deleteColorInList(spritesColors,&spritesColorNb,selectedColor);
        deleteColorInList(tilesColors,&tilesColorNb,selectedColor);
        drawColorsArray();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::isthetwiceClick(TObject *Sender)
{       addColorInList(twiceColors,&twiceColorNb,selectedColor);
        deleteColorInList(tilesColors,&tilesColorNb,selectedColor);
        deleteColorInList(spritesColors,&spritesColorNb,selectedColor);
        deleteColorInList(suspiciousColors,&suspiciousColorNb,selectedColor);
        drawColorsArray();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::settranspcolor1Click(TObject *Sender)
{       transparentColor = selectedColor ;
        drawIndex();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::tilesColorImgDblClick(TObject *Sender)
{   isaspritecolor1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::spritesColorImgDblClick(TObject *Sender)
{   isatilecolor1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::lastFrmImgDblClick(TObject *Sender)
{   preview->Visible ^= 1 ;
    if(preview->Visible) preview->Image1->Picture->Assign(lastFrmImg->Picture) ;
    frmIndex.enable = 2 ;
}
//---------------------------------------------------------------------------
void error(const char *err)
{ Application->MessageBoxA(err,"error",MB_OK);
}

int *saveBuffer(TImage*img)
{ int sy = img->Picture->Bitmap->Height ;
  int sx = img->Picture->Bitmap->Width  ;
  int *out=(int*)malloc(sx*sy*4), *optr, *iptr ,x ;
  for(int y=0;y<sy;y++)
   { optr = out+sx*y ;
     iptr = (int*)img->Picture->Bitmap->ScanLine[y] ;
     for(x=0;x<sx;x++) *optr++ = *iptr++ ;
   };
  return out ;
}

void addToMemo(AnsiString str)
{ Form1->Memo1->Text = Form1->Memo1->Text + str + "\r\n" ; }

void increaseOutput(char way, int size)
{ int *p,y,x ; AnsiString str ;

  if(size<=0) { str.printf("size error : %i",size) ;
                Application->MessageBoxA(str.c_str(),"error",MB_OK) ;
                return ;
              }  

  switch(way)
  { case 'u' :  // new line at up
     Form1->output->Picture->Bitmap->Height += size ; // increase size
     for(y=0;y<outSy;y++) // draw old bmp into the resized buffer
       memcpy(Form1->output->Picture->Bitmap->ScanLine[y+size],
              &(outBuffer[y*(Form1->output->Picture->Bitmap->Width)]),
              (Form1->output->Picture->Bitmap->Width)*4) ;
     for(y=0;y<size;y++) // copy the new line into the bmp
      { p  = (int*)Form1->output->Picture->Bitmap->ScanLine[y] ;
        for(x=0;x<Form1->output->Picture->Bitmap->Width;x++) p[x] = transparentColor ;//pf[x] ;
      };
     free(outBuffer); outBuffer = saveBuffer(Form1->output); // save the buffer
     outSy += size ;     curentInOuty = 0 ;
     str.printf("increase img about %i on up",size); addToMemo(str);
    break ;

    case 'd' :  // new line at down
     Form1->output->Picture->Bitmap->Height += size ; // increase size
     for(y=0;y<outSy;y++) // draw old bmp into the resized buffer
       memcpy(Form1->output->Picture->Bitmap->ScanLine[y],
              &(outBuffer[y*(Form1->output->Picture->Bitmap->Width)]),
              (Form1->output->Picture->Bitmap->Width)*4) ;
     for(y=0;y<size;y++) // copy the new line into the bmp
      { p  = (int*)Form1->output->Picture->Bitmap->ScanLine[y+outSy] ;
        for(x=0;x<Form1->output->Picture->Bitmap->Width;x++) p[x] = transparentColor ;//pf[x] ;
      };
     free(outBuffer); outBuffer = saveBuffer(Form1->output); // save the buffer
     outSy += size ;  curentInOuty = outSy - frmSy ;
     str.printf("increase img about %i on down curentInOuty %i",size,curentInOuty); addToMemo(str);
    break ;

    case 'l' : // new colon at left
     Form1->output->Picture->Bitmap->Width += size ; // increase size
     for(y=0;y<outSy;y++) // draw old bmp and new colons into the resized buffer
      { p  = (int*)Form1->output->Picture->Bitmap->ScanLine[y];
        for(x=0;x<size;x++) p[x]=transparentColor ; p+=size ;
        memcpy(p,&(outBuffer[y*outSx]),outSx*4) ;
      };
     free(outBuffer); outBuffer = saveBuffer(Form1->output); // save the buffer
     outSx += size ;  curentInOutx = 0 ;
     str.printf("increase img about %i on left",size); addToMemo(str);
    break ;

    case 'r' : // new colon at right
     Form1->output->Picture->Bitmap->Width += size ; // increase size
     for(y=0;y<outSy;y++) // draw old bmp and new colons into the resized buffer
      { p  = (int*)Form1->output->Picture->Bitmap->ScanLine[y];
        memcpy(p,&(outBuffer[y*outSx]),outSx*4) ;
        p+=outSx ;  for(x=0;x<size;x++) p[x]=transparentColor ;//memcpy(p,pf,size*4) ;
      };
     free(outBuffer); outBuffer = saveBuffer(Form1->output); // save the buffer
     outSx += size ;  curentInOutx = outSx - frmSx ;
     str.printf("increase img about %i on right curentInOutx %i",size,curentInOutx); addToMemo(str);
    break ;

    default : return ;
  };
     refreshOutput(); // delete bad colors
     Form1->output->Refresh(); // refresh bmp
     Form1->Label6->Caption = curentInOutx ;
     Form1->Label7->Caption = curentInOuty ;
}

int *getOutput(int x, int y)
{  return (outBuffer+(y*outSx)+x) ;
}

void drawCurentInOut(void) // draw curent frame in output
{ int *fptr, *optr, x,y ;
  AnsiString s ; s.printf("curentInOutx %i\r\ncurentInOuty %i",curentInOutx,curentInOuty) ;
  Application->MessageBoxA(s.c_str(),"info",MB_OK);
  for(y=0;y<frmSy;y++)
   { optr = outBuffer + curentInOutx + (curentInOuty+y)*outSx ;//getOutput(curentInOutx,curentInOuty+y) ;
     fptr = frmBuffer + y*frmSx ;
     for(x=0;x<frmSx;x++)
      { if(*fptr != *optr) // the 2 color are !=
         {   if(!searchColorInList(tilesColors,&tilesColorNb,*optr)) // if color in out is a tile color, not change
              { if(*optr == transparentColor) // if out color is transparent
                 if(isBgColor(*fptr)) *optr = *fptr ;
              }
         }
        optr++ ; fptr++ ;
      };
   };
   Form1->output->Refresh();
}

void __fastcall TForm1::WMDropFiles(TWMDropFiles &message) // put a new frame
{ AnsiString FileName;
  FileName.SetLength(MAX_PATH);
  FileName.SetLength(DragQueryFile((HDROP)message.Drop, 0,FileName.c_str(), MAX_PATH));

  static int firstDrop=1 ;
  if(UpperCase(ExtractFileExt(FileName)) == ".BMP")
  {   if(savedFrmBf && savedFrmBf != frmBuffer) free(savedFrmBf) ;
      lastFrmImg->Picture->LoadFromFile(FileName) ;
      lastFrmImg->Refresh();
      savedFrmBf = frmBuffer ;
      frmBuffer = getBuffer();
      searchForTilesColors();

      if(firstDrop)
      { firstDrop=0 ;
        output->Picture->Assign(lastFrmImg->Picture);
        free(outBuffer) ; outBuffer = getBuffer(output,&outSx,&outSy);
        refreshOutput(); firstDrop=0;
        curentInOutx=curentInOuty=0 ;
      } else if(searchForMap->Checked)
              { int movex, movey ;
                checkBgMove(frmBuffer,frmSx,frmSy,frmIndex.x,frmIndex.y,&movex,&movey,2) ;
                if(movex==255) checkBgMove(frmBuffer,frmSx,frmSy,frmIndex.x,frmIndex.y,&movex,&movey,5) ;
                if(movex==255) checkBgMove(frmBuffer,frmSx,frmSy,frmIndex.x,frmIndex.y,&movex,&movey,10) ;
                if(movex==255) checkBgMove(frmBuffer,frmSx,frmSy,frmIndex.x,frmIndex.y,&movex,&movey,20) ;
                Label2->Caption = movex ;
                Label3->Caption = movey ;

                if(movex != 255) // increase output if need
                { AnsiString str ;
                  if(movex>0) { if(curentInOutx - movex < 0) increaseOutput('l',-(curentInOutx - movex)); else curentInOutx-=movex ; }
                  else { if(movex<0) if(curentInOutx + frmSx - movex >= outSx) increaseOutput('r',(curentInOutx + frmSx - movex)-outSx); else curentInOutx-=movex ; }
                  if(movey>0) { if(curentInOuty - movey < 0) increaseOutput('u',-(curentInOuty - movey)); else curentInOuty-=movey ; }
                  else if(movey<0) { if(curentInOuty + frmSy - movey >= outSy) increaseOutput('d',(curentInOuty + frmSy - movey)-outSy); else curentInOuty-=movey ; }
                  //if(curentInOutx < 0) curentInOutx=0 ; if(curentInOuty < 0) curentInOuty=0 ;
                  str.printf("movex  %i movey %i curentInOutx %i curentInOuty %i",movex,movey,curentInOutx,curentInOuty); addToMemo(str);
                  str.printf("outSx %i outSy %i frmSx %i frmSy %i",outSx,outSy,frmSx,frmSy); addToMemo(str);
                  drawCurentInOut();
                } else Application->MessageBoxA("no move found in this frame.","error",MB_OK);
              }
      drawIndex();
  }
}

void __fastcall TForm1::tabControlChange(TObject *Sender)
{
  if(tabControl->TabIndex == 1) // output map
    refreshOutput(); // delete bad colors
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{ int up   = 0xffff, down  = 0 ;
  int left = 0xffff, right = 0 ;
  int x,y,*p ;
  lastFrmImg->Picture->Bitmap->PixelFormat = pf32bit ; // for an easy read
  setBuffer(frmBuffer); // to delete index color

  int colorOnIndex = transparentColor ;

  if(chkIgnore->Checked)
  { colorOnIndex = (frmIndex.color)[0] ;
    if(frmIndex.colorNb > 1 && colorOnIndex == transparentColor)
     colorOnIndex = (frmIndex.color)[1] ;
  }

  for(y=0;y<frmIndex.szy;y++)
  { p = (int*)lastFrmImg->Picture->Bitmap->ScanLine[y + frmIndex.y] ;
    for(x=0;x<frmIndex.szx;x++)
     if(p[x + frmIndex.x] != transparentColor)
      {  int found=0 ;
         if(chkIgnore->Checked)
            for(int c=0;c<frmIndex.colorNb;c++) if(frmIndex.color[c] == p[x + frmIndex.x]) { found=1 ; break ; }

         if(!found)
         { if(y > down)  down=y ;
           if(y < up)    up=y ;
           if(x > right) right=x ;
           if(x < left)  left=x ;
         }
      }
  };
  right++ ; down++ ;
  if(up==0xffff){ Label2->Caption = "no sprite" ;
                  Label3->Caption = " found !" ;
                  return ;
                }
  frmIndex.x += left ; frmIndex.szx = right-left ;
  frmIndex.y += up   ; frmIndex.szy = down-up ;
  drawIndex();
  Label2->Caption = Label2->Caption.sprintf("up %i down %i",up,down) ;
  Label3->Caption = Label3->Caption.sprintf("left %i right %i",left,right) ;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{  int *tmp, nb ;
   tmp = tilesColors ;  tilesColors = spritesColors  ; spritesColors = tmp ;
   nb = tilesColorNb ; tilesColorNb = spritesColorNb ; spritesColorNb = nb ;
   drawColorsArray();
}
//---------------------------------------------------------------------------

#include <stdio.h>

void __fastcall TForm1::Button3Click(TObject *Sender) // save all colors array
{ remove("c:\\color.rip");
  FILE *f = fopen("c:\\color.rip","w+") ;
  fwrite(&tilesColorNb,4,1,f);
  fwrite(&spritesColorNb,4,1,f);
  fwrite(&suspiciousColorNb,4,1,f);
  fwrite(&twiceColorNb,4,1,f);
  fwrite(tilesColors,4*tilesColorNb,1,f);
  fwrite(spritesColors,4*spritesColorNb,1,f);
  fwrite(suspiciousColors,4*suspiciousColorNb,1,f);
  fwrite(twiceColors,4*twiceColorNb,1,f);
  fclose(f);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)  // load all colors array
{ FILE *f = fopen("c:\\color.rip","r") ;
  fread(&tilesColorNb,4,1,f);
  fread(&spritesColorNb,4,1,f);
  fread(&suspiciousColorNb,4,1,f);
  fread(&twiceColorNb,4,1,f);
  fread(tilesColors,4*tilesColorNb,1,f);
  fread(spritesColors,4*spritesColorNb,1,f);
  fread(suspiciousColors,4*suspiciousColorNb,1,f);
  fread(twiceColors,4*twiceColorNb,1,f);
  fclose(f);
  drawColorsArray();
}
//---------------------------------------------------------------------------

