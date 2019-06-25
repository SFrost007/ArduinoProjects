var xPos, yPos, width, height;
var screenWidth = 1920;
var screenHeight = 1200;
var yOffset = Math.floor((1200-screenHeight)/2);


var numWide = 31;
var numTall = 20;

function writeLED(num, x, y, w, h, enabled) {
    console.log("[LED_"+num+"]");
    console.log("IsEnabled="+(enabled?"true":"false"));
    console.log("Position=@Point("+x+" "+y+")");
    console.log("Size=@Size("+w+" "+h+")");
    console.log("CoefRed=1");
    console.log("CoefGreen=1");
    console.log("CoefBlue=1");
    console.log("");
}

// Size calculation for each area
var realWidth = screenWidth/(numWide+2);
var realHeight = screenHeight/numTall;
var width = Math.floor(realWidth);
var height = Math.floor(realHeight);

var LEDNum = 1;

// Bottom edge
var xPos = realWidth; // Move along one position for left side
var yPos = (screenHeight-height)+yOffset;
for (var i=0; i<numWide; i++, LEDNum++) {
    writeLED(LEDNum, Math.floor(xPos), yPos, width, height, true);
    xPos += realWidth;
}

// Right edge
var xPos = screenWidth-width;
for (var i=0; i<numTall; i++, LEDNum++) {
    writeLED(LEDNum, xPos, Math.floor(yPos), width, height, true);
    yPos -= realHeight;
}

// Top edge
var xPos = screenWidth-(2*width);
var yPos = yOffset;
for (var i=0; i<numWide; i++, LEDNum++) {
    writeLED(LEDNum, Math.floor(xPos), yPos, width, height, true);
    xPos -= realWidth;
}

// Left edge
var xPos = 0;
var yPos = yOffset;
for (var i=0; i<numTall; i++, LEDNum++) {
    writeLED(LEDNum, xPos, Math.floor(yPos), width, height, true);
    yPos += realHeight;
}

// Write the disabled LEDs
for (var i=LEDNum; i<=255; i++) {
    writeLED(i, 0, 0, 0, 0, false);
}