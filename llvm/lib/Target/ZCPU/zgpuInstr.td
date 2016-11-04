def DTEST:I<200,(outs), (ins),[], "DTEST">; //Output a test pattern to screen
def DRECT_TEST:I<200,(outs), (ins),[], "DRECT_TEST">; //DTEST
def DEXIT:I<201,(outs), (ins),[], "DEXIT">; //End execution of the current frame
def DVSYNC:I<201,(outs), (ins),[], "DVSYNC">; //Wait until next frame (only in asynchronous thread)
def DCLR:I<202,(outs), (ins),[], "DCLR">; //Clear screen color to black
def DCLRTEX:I<203,(outs), (ins),[], "DCLRTEX">; //Clear background with texture
def DVXFLUSH:I<204,(outs), (ins),[], "DVXFLUSH">; //Flush current vertex buffer to screen
def DVXCLEAR:I<205,(outs), (ins),[], "DVXCLEAR">; //Clear vertex buffer
def DSETBUF_VX:I<206,(outs), (ins),[], "DSETBUF_VX">; //Set frame buffer to vertex output
def DSETBUF_SPR:I<207,(outs), (ins),[], "DSETBUF_SPR">; //Set frame buffer to sprite buffer
def DBACKBUF:I<207,(outs), (ins),[], "DBACKBUF">; //Set frame buffer to back buffer
def DSETBUF_FBO:I<208,(outs), (ins),[], "DSETBUF_FBO">; //Set frame buffer to view buffer
def DFRONTBUF:I<208,(outs), (ins),[], "DFRONTBUF">; //Set frame buffer to front buffer
def DSWAP:I<209,(outs), (ins),[], "DSWAP">; //Copy back buffer to front buffer
def DVXPIPE:I<210,(outs), (ins),[], "DVXPIPE">; //Set vertex pipe
def DCVXPIPE:I<211,(outs), (ins),[], "DCVXPIPE">; //DCPIPE
def DCPIPE:I<211,(outs), (ins),[], "DCPIPE">; //Set coordinate pipe
def DENABLE:I<212,(outs), (ins),[], "DENABLE">; //Enable parameter
def DDISABLE:I<213,(outs), (ins),[], "DDISABLE">; //Disable parameter
def DCLRSCR:I<214,(outs), (ins),[], "DCLRSCR">; //Clear screen with color
def DCOLOR:I<215,(outs), (ins),[], "DCOLOR">; //Set current color
def DTEXTURE:I<216,(outs), (ins),[], "DTEXTURE">; //Set current texture
def DSETFONT:I<217,(outs), (ins),[], "DSETFONT">; //Set current font
def DSETSIZE:I<218,(outs), (ins),[], "DSETSIZE">; //Set font size
def DMOVE:I<219,(outs), (ins),[], "DMOVE">; //Set drawing position offset
def DVXDATA_2F:I<220,(outs), (ins),[], "DVXDATA_2F">; //"Draw a solid 2D polygon (pointer to 2D dat
def DVXPOLY:I<220,(outs), (ins),[], "DVXPOLY">; //"Draw a solid 2D polygon (pointer to 2D dat
def DVXDATA_2F_TEX:I<221,(outs), (ins),[], "DVXDATA_2F_TEX">; //"Draw a textured 2D polygon (pointer to 2D dat
def DVXTEXPOLY:I<221,(outs), (ins),[], "DVXTEXPOLY">; //"Draw a textured 2D polygon (pointer to 2D dat
def DVXDATA_3F:I<222,(outs), (ins),[], "DVXDATA_3F">; //"Draw a solid 3D polygon (pointer to 3D dat
def DVXDATA_3F_TEX:I<223,(outs), (ins),[], "DVXDATA_3F_TEX">; //"Draw a textured 3D polygon (pointer to 3D dat
def DVXDATA_3F_WF:I<224,(outs), (ins),[], "DVXDATA_3F_WF">; //"Draw a wireframe 3D polygon (pointer to 3D dat
def DRECT:I<225,(outs), (ins),[], "DRECT">; //Draw a rectangle (by endpoints)
def DCIRCLE:I<226,(outs), (ins),[], "DCIRCLE">; //Draw a circle with radius Y
def DLINE:I<227,(outs), (ins),[], "DLINE">; //Draw a line
def DRECTWH:I<228,(outs), (ins),[], "DRECTWH">; //"Draw a rectangle (by offse
def DORECT:I<229,(outs), (ins),[], "DORECT">; //Draw an outlined rectangle
def DTRANSFORM2F:I<230,(outs), (ins),[], "DTRANSFORM2F">; //Transform vector and write it to first operand
def DTRANSFORM3F:I<231,(outs), (ins),[], "DTRANSFORM3F">; //Transform vector and write it to first operand
def DSCRSIZE:I<232,(outs), (ins),[], "DSCRSIZE">; //Set screen size
def DROTATESCALE:I<233,(outs), (ins),[], "DROTATESCALE">; //"Rotate by 
def DORECTWH:I<234,(outs), (ins),[], "DORECTWH">; //Draw an outlined rectangle by width/height
def DCULLMODE:I<235,(outs), (ins),[], "DCULLMODE">; //Set cullmode and lighting mode
def DPIXEL:I<238,(outs), (ins),[], "DPIXEL">; //Draw a pixel to screen
def DWRITE:I<240,(outs), (ins),[], "DWRITE">; //Write a string
def DWRITEI:I<241,(outs), (ins),[], "DWRITEI">; //Write an integer value
def DWRITEF:I<242,(outs), (ins),[], "DWRITEF">; //Write a float value
def DENTRYPOINT:I<243,(outs), (ins),[], "DENTRYPOINT">; //Set entry point
def DSETLIGHT:I<244,(outs), (ins),[], "DSETLIGHT">; //Set light
def DGETLIGHT:I<245,(outs), (ins),[], "DGETLIGHT">; //Get light
def DWRITEFMT:I<246,(outs), (ins),[], "DWRITEFMT">; //Write a formatted string
def DWRITEFIX:I<247,(outs), (ins),[], "DWRITEFIX">; //Write a fixed value
def DTEXTWIDTH:I<248,(outs), (ins),[], "DTEXTWIDTH">; //Return text width
def DTEXTHEIGHT:I<249,(outs), (ins),[], "DTEXTHEIGHT">; //Return text height
def DLOOPXY:I<259,(outs), (ins),[], "DLOOPXY">; //2D loop by ECX/EDX registers
def MLOADPROJ:I<271,(outs), (ins),[], "MLOADPROJ">; //Load matrix into view matrix
def MREAD:I<272,(outs), (ins),[], "MREAD">; //Write view matrix into matrix
def DT:I<274,(outs), (ins),[], "DT">; //Set X to frame length time
def DSHADE:I<276,(outs), (ins),[], "DSHADE">; //Shade the current color
def DSETWIDTH:I<277,(outs), (ins),[], "DSETWIDTH">; //Set line width
def MLOAD:I<278,(outs), (ins),[], "MLOAD">; //Load matrix into model matrix
def DSHADENORM:I<279,(outs), (ins),[], "DSHADENORM">; //Shade the current color and normalize it
def DSHADECOL:I<279,(outs), (ins),[], "DSHADECOL">; //DSHADENORM
def DDFRAME:I<280,(outs), (ins),[], "DDFRAME">; //Draw bordered frame
def DDBAR:I<281,(outs), (ins),[], "DDBAR">; //Draw a progress bar
def DDGAUGE:I<282,(outs), (ins),[], "DDGAUGE">; //Draw gauge needle
def DRASTER:I<283,(outs), (ins),[], "DRASTER">; //Set rasterizer quality level
def DDTERRAIN:I<284,(outs), (ins),[], "DDTERRAIN">; //Draw terrain
def DLOADBYTES:I<290,(outs), (ins),[], "DLOADBYTES">; //Load into texture slot by pointer
def RESERVED:I<292,(outs), (ins),[], "RESERVED">; //
def DMULDT:I<294,(outs), (ins),[], "DMULDT">; //X = Y * dT
def DSMOOTH:I<297,(outs), (ins),[], "DSMOOTH">; //Smooth X with smoothness Y
def DBEGIN:I<298,(outs), (ins),[], "DBEGIN">; //Begin rendering (from async thread)
def DEND:I<299,(outs), (ins),[], "DEND">; //End rendering (from async thread)
def DROTATE:I<300,(outs), (ins),[], "DROTATE">; //Rotate model by vector
def DTRANSLATE:I<301,(outs), (ins),[], "DTRANSLATE">; //Translate model by vector
def DSCALE:I<302,(outs), (ins),[], "DSCALE">; //Scale model by vector
def DXTEXTURE:I<303,(outs), (ins),[], "DXTEXTURE">; //Bind a specific external texture
