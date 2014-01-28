//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h" // main windows
#include "Unit2.h" // output map windows
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

//---------------------------------------------------------------------------
void addToLog(AnsiString s , int jump);
void addToLog(const char *s, int jump);
int *loadBmp(AnsiString file);
void setNextImgPath(void);
int getColorNumber(int*buffer, int size, int *pal);
int searchAndAddColor(int*pal,int*nb,int*buf,int size);
int checkForMove(int zone,int percent, int pos);

int frmHeight,       frmWidth ;     // frame size, in pixel
int outHeight,       outWidth ;     // map size in pixels too
int xFrmPosInOut,    yFrmPosInOut ; // position of the curent frame in the map
int *frmBuffer=0, *outBuffer=0 ; // 2 32bits buffer, for curent frame and output map
int  *oldFrm[4] ; // 4 32 bits pointer to save old frmBuffer
int oldmovex[4] ; // also save the move between these frame
int oldmovey[4] ;
AnsiString imgPath ; // path of the curent frame
int imgNb ; // curent img number (number who said file name)
int *palette, paletteColorNb=0 ; // palette array and a var to specifi the color number
int *suspectColors, *tilesColors, *twiceColors, *spritesColors ; // some Array to keep colors .. first entry define entry's number
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) // form constructor
        : TForm(Owner)
{ addToLog("welcome ! ... rip'r .method II -- april 2k5 by r043v !",0) ; // add some credits into the log :P
  log->ScrollBars = ssVertical ; // add a scrollbar at the memo..
  palette = (int*)malloc(2048*4); // alloc memory for 2048 32bits colors
  suspectColors = (int*)malloc(512*4*4) ; // alloc ram for 4 colors array of 512 entry
  tilesColors   = suspectColors + 512 ;  // set colors array pointer to the new alocated memory
  twiceColors   =  tilesColors  + 512 ;
  spritesColors =  twiceColors  + 512 ;
}
//---------------------------------------------------------------------------
void addToLog(AnsiString s, int jump=1) // add a string to the log and add (if jump==1) a line return
{ if(jump) Form1->log->Text = Form1->log->Text + "\r\n" + s ;
   else    Form1->log->Text = Form1->log->Text + s ;
  Form1->log->SelStart = Form1->log->Text.Length(); Form1->log->SelLength = 0; // to got cursor at end (and see last info)
}

void addToLog(const char *s, int jump=1) // the same, but who work with a classic char* string
{ AnsiString str = s ;
  addToLog(str,jump);
}

int searchColorInList(int *list, int *color)
{  int nb = *list++ ;
   for(int c=0;c<nb;c++)
    if(*list++ == *color) return 1 ;
   return 0 ;
}

TImage *im = new TImage(Form1) ;  // create an bitmap object in memory

int *loadBmp(AnsiString file)
{ int y,*p,*buffer,*o ;
  addToLog("*loading " + file + " ... ") ;  // log file loading and result
  if(!FileExists(file)) { addToLog("failed.",0) ;
                          Application->MessageBoxA("file not found !","error",MB_OK);
                          return 0 ;
                        } addToLog("ok",0);
  im->Picture->LoadFromFile(file); //   and load the bmp inside
  im->Picture->Bitmap->PixelFormat = pf32bit ; // set deep to 32 bits for an easy read
 // Form1->Image1->Picture->Bitmap->Assign(im->Picture->Bitmap) ;
 // Form1->Image1->Refresh();
  o = buffer = (int*)malloc(frmHeight*frmWidth*4); // alloc some ram for the frame's buffer

  if((frmHeight != im->Picture->Bitmap->Height)||(frmWidth != im->Picture->Bitmap->Width))
    Application->MessageBoxA("frame size error !","error",MB_OK);

  for(y=0;y<frmHeight;y++)
  { p = (int*)im->Picture->Bitmap->ScanLine[y];
    memcpy(o,p,4*frmWidth); o+=frmWidth ;
  }; /*delete im ;*/ return buffer ; // delete the object and return the buffer
}

void setNextImgPath(void) // set the path of the curent bitmap with the next file
{ imgPath = Form1->path->Text ;
  if(imgNb < 10)          imgPath+="000" ; // add some zero to the path ... why ?
   else if(imgNb < 100)   imgPath+="00" ;  //  this program is designed to work with the xport emu's
    else if(imgNb < 1000) imgPath+="0" ;   //   who add these zero at filename of screen shoots.
  imgPath += imgNb ; imgPath += ".bmp" ; imgNb++ ;
}

int getColorNumber(int*buffer, int size, int *pal=0) // count colors in a buffer, can fill a palette
{ int color[1024],nb=1, found=0 ;
  color[0]=*buffer ;
  for(int c=1;c<size;c++)  // for eatch pixel of the buffer
  { for(int n=0;n<nb;n++) //   and eatch actual color of the palette
     if(buffer[c] == color[n]){ found=1; break; } // if color found, skip to the next pixel
    if(!found)  color[nb++] = buffer[c]; else found=0 ; // else, add color in palette
  }; if(pal) memcpy(pal,color,nb*4); return nb ; // copy colors if specified, and return color number
}

int searchAndAddColor(int*pal,int*nb,int*buf,int size) // scan a buffer and add all the new color in a palette
{ int found=0,add=0 ;
  for(int c=0;c<size;c++)
  { for(int n=0;n<*nb;n++)
     if(buf[c] == pal[n]){ found=1; break; }
    if(!found) { pal[*nb] = buf[c] ; *nb=(*nb)+1 ; add++ ; } else found=0 ;
  }; return add ; // return the new color number
}

int *saveBuffer(TImage*img)
{ int sy = img->Picture->Bitmap->Height ;
  int sx = img->Picture->Bitmap->Width  ;
  int *bf=(int*)malloc(sx*sy*4) ;// memset(bf,0xff,sx*sy*4);
  for(int y=0;y<sy;y++) memcpy(&(bf[sx*y]),img->Picture->Bitmap->ScanLine[y],sx*4) ;
  return bf ;
}

void increaseOutput(char way, int size) // increase output bitmap size
{ int *p,y,x ; AnsiString str ;
/*
  if(size<=0 || size>6) { str.printf("size error : %i",size) ;
                          Application->MessageBoxA(str.c_str(),"error",MB_OK) ;
                          return ;
                        }*/
  switch(way)
  { case 'u' :  // new line at up
     Form2->map->Picture->Bitmap->Height += size ; // increase size
     for(y=0;y<outHeight;y++) // draw old bmp into the resized buffer
     { //memset(Form2->map->Picture->Bitmap->ScanLine[y+size],0xff,outWidth*4) ;
       memcpy(Form2->map->Picture->Bitmap->ScanLine[y+size],&(outBuffer[y*outWidth]),outWidth*4) ;
     }
     for(y=0;y<=size;y++) // and new lines in pink
      { p = (int*)Form2->map->Picture->Bitmap->ScanLine[y] ;
        x = outWidth ; while(x--) *p++ = 0xff00ff ;
      }; outHeight+=size; yFrmPosInOut=0 ;
     str.printf("increase img about %i on up",size); addToLog(str);
    break ;

    case 'l' : // new colon at left
     Form2->map->Picture->Bitmap->Width += size ; // increase size
     for(y=0;y<outHeight;y++) // draw old bmp and set in pink new colons into the resized buffer
      { p = (int*)(Form2->map->Picture->Bitmap->ScanLine[y]); for(x=0;x<size;x++) *p++ = 0xff00ff ;
        //memmove(&(p[size]),p,outWidth*4);
        memcpy(p,&(outBuffer[y*outWidth]),outWidth*4) ;
      }; outWidth+=size; xFrmPosInOut=0 ;
     str.printf("increase img about %i on left",size); addToLog(str);
    break ;

    case 'r' : // new colon at right
     Form2->map->Picture->Bitmap->Width += size ; // increase size
     for(y=0;y<outHeight;y++) // draw old bmp and new colons into the resized buffer
      { p = (int*)Form2->map->Picture->Bitmap->ScanLine[y];
        memcpy(p,&(outBuffer[y*outWidth]),outWidth*4) ;
        p+=outWidth ; for(x=0;x<size;x++) *p++ = 0xff00ff ;
      }; outWidth+=size; xFrmPosInOut=outWidth-frmWidth ;
     str.printf("increase img about %i on right",size); addToLog(str);
    break ;

    case 'd' :  // new line at down
     Form2->map->Picture->Bitmap->Height += size ; // increase size
     for(y=0;y<outHeight;y++) // draw old bmp into the resized buffer
       memcpy(Form2->map->Picture->Bitmap->ScanLine[y],&(outBuffer[y*outWidth]),outWidth*4) ;
     for(y=0;y<size;y++) // copy the new line into the bmp
      { p  = (int*)Form2->map->Picture->Bitmap->ScanLine[y+outHeight] ;
        for(x=0;x<outWidth;x++) p[x] = 0xff00ff ;
      }; outHeight+=size ; yFrmPosInOut+=size ;
     str.printf("increase img about %i on down",size); addToLog(str);
    break ;

    default : return ;
  };
   free(outBuffer); outBuffer = saveBuffer(Form2->map); // save the new output buffer
}

void blitFrmInOutput(void)
{ int x,y,*c,*o ; c=frmBuffer ;// c+=frmWidth ;
  for(y=0;y<frmHeight;y++)
  {  o = (int*)Form2->map->Picture->Bitmap->ScanLine[y+yFrmPosInOut]; o+=xFrmPosInOut ; //*--o = 0x00ff00 ; o++ ;
     for(x=0;x<frmWidth;x++)
     { if(*o != *c)
       { if(*o == 0xff00ff) // if actual pixel in output is transparent, fill it with the curent color in frm
         { if(Form1->loadColorArrayChk->Checked)
            { if(!searchColorInList(spritesColors,c)) *o = *c ; }
           else *o = *c ;
         } else {  // not transparent, set priority to tiles & no sprites color (if color array loaded)
                   if(Form1->loadColorArrayChk->Checked)
                    { if(!searchColorInList(tilesColors,o))
                       if(!searchColorInList(twiceColors,o) || searchColorInList(tilesColors,c))
                        if(!searchColorInList(spritesColors,c))
                         *o = *c ;
                      if(searchColorInList(twiceColors,o) && searchColorInList(twiceColors,c)) // zZ method
                      { *o = 0xff00ff ;
                      }
                    }
                }
       } // end if(*o != *c)
       o++ ; c++ ;
     }; // end for
  }; Form2->map->Refresh(); // refresh output bmp
  free(outBuffer); outBuffer = saveBuffer(Form2->map);
}

int isMoveAbout(int suspectedx, int suspectedy, int percent=85, int pos=0) // check if the bg has move about suspected amout of pixel
{ int xstart, xend, ystart, yend, x,y, nb=0,ok=0,*out, *frm ;
  if(!pos){
     xstart = (frmWidth>>2) + xFrmPosInOut ;            // we take 1/4
     xend   = xstart + (frmWidth>>1) ;                  // of the frame
     ystart = (frmHeight>>2) + yFrmPosInOut ;           //  to compare
     yend   = ystart + (frmHeight>>1) ;
  } else {
     xstart = (frmWidth>>2) + (frmWidth/5) + xFrmPosInOut ;    // we take 1/4
     xend   = xstart + (frmWidth>>1) ;                         // of the frame
     ystart = (frmHeight>>2) + (frmHeight/5) + yFrmPosInOut ;  //  to compare
     yend   = ystart + (frmHeight>>1) ;
  }
  for(y=ystart;y<yend;y++)
   { frm = &(frmBuffer[y*frmWidth]); out = &(outBuffer[xFrmPosInOut + suspectedx + (y+suspectedy+yFrmPosInOut)*outWidth]);
     for(x=xstart;x<xend;x++) { if(*out == *frm++ || *out == 0xff00ff) ok++ ; nb++ ; out++ ; };
     if(!x) if(ok < (nb>>1)) return 0 ; // if some error on the first line, stop search
   };
    return (ok > (nb>>10)*percent*10) ; // is found ?
}

int xmove, ymove ;

int checkForMove(int zone=6,int percent=85, int pos=0) // compare the map and the curent buffer to search a move
{ for(int y=-zone;y<=zone;y++)
   for(int x=-zone;x<=zone;x++)
    if(isMoveAbout(x,y,percent,pos))
    { //AnsiString s ; s.printf("frame found, move about %i on x and %i on y",x,y) ; addToLog(s);
      xmove = x ; ymove = y ;
      if(y<0) // bg scroll up
       { y = -y ;
         if(yFrmPosInOut < y) // we need to add line at up of map
          increaseOutput('u',y-yFrmPosInOut);
         else { yFrmPosInOut -= y ; if(yFrmPosInOut == 0) Application->MessageBoxA("error y !","",MB_OK); }
       } else if(y>0) // scroll down
                { int diff = outHeight - (yFrmPosInOut + frmHeight + y) ;
                  if( diff < 0 ) // we need to add line at down of map
                   increaseOutput('d',-diff);
                  else yFrmPosInOut+=y ;
                }

      if(x<0) // bg scroll left
       { x = -x ;
         if(xFrmPosInOut < x) // we need to add line at left of map
           increaseOutput('l',x-xFrmPosInOut);
         else xFrmPosInOut-=x ;
       } else if(x>0) // and scroll right
                { int diff = outWidth - (xFrmPosInOut + frmWidth + x) ;
                  if( diff < 0 ) // we need to add line at right of map
                   increaseOutput('r',-diff);
                  else xFrmPosInOut+=x ;
                }
       /*
        if(xFrmPosInOut < 0 || yFrmPosInOut < 0) Application->MessageBoxA("start pos is under 0","error",MB_OK) ;
        if(xFrmPosInOut + frmWidth > outWidth || yFrmPosInOut + frmHeight > outHeight)
         Application->MessageBoxA("start pos is too hight !","error",MB_OK) ;
         */
        blitFrmInOutput();
        Form1->Label3->Caption = xFrmPosInOut ;
        Form1->Label4->Caption = yFrmPosInOut ;
      return 1 ;
    } return 0 ;
}

  #include "stdio.h" // to get file i/o
  #include   <dos.h> // to get system time
  struct  time sysTime ;

void __fastcall TForm1::startBtnClick(TObject *Sender) // start job !
{ AnsiString msg ;
  imgNb = atoi(imgNbStart->Text.c_str()); // get first frame file number
  gettime(&sysTime);
  msg.printf("%2i:%2i:%2i ** job started !",sysTime.ti_hour,sysTime.ti_min,sysTime.ti_sec);
  addToLog(msg);
  setNextImgPath(); // set first frame path
  Form2->map->Picture->LoadFromFile(imgPath); // load the bitmap
  Form2->map->Refresh(); // refresh image in the windows
  // set size of frame and map with the loaded bitmap size
  outHeight = frmHeight = Form2->map->Picture->Bitmap->Height;
  outWidth  = frmWidth  = Form2->map->Picture->Bitmap->Width  ;
  xFrmPosInOut = yFrmPosInOut = 0 ; // frame pos in map is 0.0 at start...
  Form2->map->Picture->Bitmap->PixelFormat = pf32bit ;
  outBuffer = loadBmp(imgPath);    // load the bmp into the map buffer
  if(loadColorArrayChk->Checked)
  {  FILE *f = fopen("c:\\color.rip","r") ;
     if(f) { fread(tilesColors ,4,1,f); int n = tilesColors[0]   ;
             fread(spritesColors,4,1,f);    n+= spritesColors[0] ;
             fread(suspectColors,4,1,f);    n+= suspectColors[0] ;
             fread(twiceColors,4,1,f);      n+= twiceColors[0]   ;
             fread(&(tilesColors[1])  ,4*(tilesColors[0]),1,f);
             fread(&(spritesColors[1]),4*(spritesColors[0]),1,f);
             fread(&(suspectColors[1]),4*(suspectColors[0]),1,f);
             fread(&(twiceColors[1])  ,4*(twiceColors[0]),1,f);
             fclose(f); AnsiString s ;
             s.printf("colors array loaded, there is %i colors",n); addToLog(s);
           } else addToLog("color array not found, sure 'c:\color.rip' exist ?") ;
  }

  searchAndAddColor(palette,&paletteColorNb,outBuffer,frmHeight*frmWidth) ;
  imgPath.printf("frame size is %i*%i, %i colors found",frmWidth,frmHeight,paletteColorNb);
  addToLog(imgPath); // log the size & color number
  int imgEnd = atoi(imgNbEnd->Text.c_str()); AnsiString s ; // get last frame file number
  progressBar->Max = imgEnd-imgNb ;   progressBar->Position=0 ;
  Image1->Picture->Bitmap->PixelFormat = pf32bit ;
  Image1->Picture->Bitmap->Height = frmHeight ;
  Image1->Picture->Bitmap->Width  = frmWidth  ;

  while(imgNb <= imgEnd)  // now, scan eatch other bitmap ;
  { setNextImgPath(); // set curent file path
    if(oldFrm[3]) free(oldFrm[3]); // free last buffer (if not empty)
    oldFrm[3] = oldFrm[2] ; oldmovex[3] = oldmovex[2] ;
    oldFrm[2] = oldFrm[1] ; oldmovex[2] = oldmovex[1] ;
    oldFrm[1] = oldFrm[0] ; oldmovex[1] = oldmovex[0] ;
    oldFrm[0] = frmBuffer ; oldmovex[0] = xmove ;
    oldmovey[3] = oldmovey[2] ;
    oldmovey[2] = oldmovey[1] ;
    oldmovey[1] = oldmovey[0] ;
    oldmovey[0] = ymove ;

    Label5->Caption = oldmovex[3] ;
    Label9->Caption = oldmovey[3] ;
    Label6->Caption = oldmovex[2] ;
    Label10->Caption = oldmovey[2] ;
    Label7->Caption = oldmovex[1] ;
    Label11->Caption = oldmovey[1] ;
    Label7->Caption = oldmovex[0] ;
    Label11->Caption = oldmovey[0] ;

    frmBuffer = loadBmp(imgPath);  // and load it with the new frame
    int newColorNb = searchAndAddColor(palette,&paletteColorNb,frmBuffer,frmHeight*frmWidth); // there is any new color ?
    if(newColorNb) { s.printf("  found %i new color -> %i colors",newColorNb,paletteColorNb) ; addToLog(s); } // if yes, log it

    if(!checkForMove(3,85))
     if(!checkForMove(3,85,1))
      if(!checkForMove(3,80))
       if(!checkForMove(3,80,1))
    if(!checkForMove(6,85))
     if(!checkForMove(6,85,1))
    if(!checkForMove(3,75))
     if(!checkForMove(3,75,1))
    if(!checkForMove(6,75))
     if(!checkForMove(6,75,1))
    if(!checkForMove(3,70))
     if(!checkForMove(3,70,1))
    if(!checkForMove(6,70))
     if(!checkForMove(6,70,1))
    if(!checkForMove(9,85,1))
     if(!checkForMove(9,75))
      if(!checkForMove(9,70,1))
      {  addToLog("no move possibility found.");
         break ; // stop if not found move
      }

    Label8->Caption = xmove ;
    Label12->Caption = ymove ;

    // blit curent frm in dedicated bitmap
    for(int u=0;u<frmHeight;u++)
     memcpy(Image1->Picture->Bitmap->ScanLine[u],&(frmBuffer[u*frmWidth]),frmWidth*4);
    Image1->Refresh();

      progressBar->Position++ ; Sleep(0);
  };
  gettime(&sysTime);
  msg.printf("%2i:%2i:%2i ** job finish !",sysTime.ti_hour,sysTime.ti_min,sysTime.ti_sec);
  addToLog(msg);
}
//---------------------------------------------------------------------------

