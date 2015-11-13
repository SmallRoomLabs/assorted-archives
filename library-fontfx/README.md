# library-fontfx

A library providing easy animated font support. The best way of figuring it out is to run the Demos using the included 
project. You can change the demo by changing the Gosub round line 40. 

Demo5 which it should be set to , shows a very simple game. Click on the screen to get 100 points. Nearly as much fun as Flappy Bird.

It shows three texts - one standard AGK2 - the actual score displayed, and two from this library, the "Get Ready!" that bounces in and the little '100' that appears whenever you click. 

I don't think this does anything that you can't already do with AGK, but it's much easier. The drop in "Get Ready!" for example consists of the following set up.

titleText as FFXText															// This is the drop down text
FFXCreateText(titleText,"Get Ready !",FONT_IMAGE)								// Create it
FFXSetAnimation(titleText,"movein,time=1.5,mod1=90,tween=bounceout:fadeout")	// MoveIn with bounce, then fadeout.
FFXSetColor(titleText,255,128,0,255)											// Recolour it.

and a call in the Sync() loop to update it - this is two sequential animations. The mod1 is a modifier for movein, specifying the angle it moves in from , 90 degrees, i.e. straight up.

FFXSync(titleText)

Obviously this is a bit more processor heavy because it's in AGK Basic. But these kind of uses are okay, if it isn't 
actually animating something the call to FFXSync() returns immediately, so there's not a huge overhead.

There are 5 demos, one of which is just the setup I used for testing so doesn't really count. The other three show (i) all the built in effects (ii) a simple animation chain (iii) how to code your own animation.

Paul Robson
paulscottrobson@googlemail.com
17th Feb 2015
