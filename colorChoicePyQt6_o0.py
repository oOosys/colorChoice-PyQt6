#!/usr/bin/python3
#									oOosys							     2024-09-30_05:26
#													last update:		     2025-10-22_21:46
'''	Text-FREE color choice interface showing all possible colors in channel slider strips    
				and a large area showing the currently selected color					     '''
##					Designed for 1920x1080 pixel display only
#	Requires installed cv2 and for the color name functionality the webcolors module.
##				~ $ pip3 install  opencv-python   webcolors   PyQt6
fullscreenMode		=  "Yes"	# [  'Yes' ,  ''  ]
# =======================================================================
# 	   Preset values provided below are best choce, so just let them stay as they are:
# Restore chosen color and slider settings from .ini file: 
try: 
	with open(__file__ + ".ini", 'r') as f :
		exec(  f.read() )
except FileNotFoundError :
	with open(__file__ + ".ini", 'w') as f :
		f.write("""\
firstSliderValue, secondSliderValue, thirdSliderValue = 254, 127, 97
colorMode 			= 'RGB'
activeChannelIndex	=  0											""") 
	with open(__file__ + ".ini", 'r') as f :
		exec(  f.read() )
#print(f" {firstSliderValue=}, {secondSliderValue=}, {thirdSliderValue=}, {colorMode=}, {activeChannelIndex=}")
#firstSliderValue, secondSliderValue, thirdSliderValue = 255, 127, 127
#colorMode 			= 'RGB'
#activeChannelIndex	=  0

withOutputOfClosestColorName =  'Yes'		#	[  'Yes' ,  ''  ]
wID = "[f]:Fullscreen  ||  nextActiveSlider:[Tab], [Left/Right/Up/Down], [PageUp/Down]   ||   [r]/[h]/[l]/[a] -> RGB/HSV/HLS/Lab   ||   Enter/Click to print (r,g,b) to stdout and EXIT."
windowXpos	=	  30
windowYpos	=	  10
windowWidth	 = 1890
windowHeight	 = 1050
# ---
widthColorStripColor			=	7
heightColorStrip				= 130
verticalSpacingBetweenStrips	=   12
topStripsOffset					= 590
topMargin						=   10
bottomMargin					=   10
leftMargin						=   30
rightMargin						=   30
sliderPositionMarkerLineWidth	=	3
sliderPositionMarkerColor		=  (0,0,0)
activeSliderPositionMarkerColor = {
	'RGB' : (     0,     0, 255)	, 	# RED
	'HSV' : (255, 255,      0)	, 	# YELLOW
	'HLS' : (	     0, 255, 255)	, 	# CYAN
	'Lab' : (	     0, 255,      0)	,   }	# GREEN
sliderChangeOnPageUpDown	=   10
sliderChangeOnUpDown		=	5
# ---
offsetX, offsetY		= 30, 30

# =======================================================================

from PyQt6.QtWidgets import QApplication, QWidget
from PyQt6.QtGui import QPainter, QColor, QPen
from PyQt6.QtCore import Qt, QRect
from cv2 import (
	cvtColor							,
		COLOR_Lab2RGB				,
		COLOR_HSV2RGB_FULL			,
		COLOR_HLS2RGB_FULL			,

		COLOR_RGB2Lab				,
		COLOR_RGB2HSV_FULL			,
		COLOR_RGB2HLS_FULL			,

)
from numpy import uint8
if withOutputOfClosestColorName :
	from webcolors import (
		rgb_to_name							,
		name_to_rgb							,
		names			as WebColorNameList	,
		hex_to_rgb								,
	)
import sys

def updateSliderColors() :
	global FirstSliderColors , SecondSliderColors , ThirdSliderColors
	if colorMode == 'RGB' :
		FirstSliderColors	= [ (tmpSliderValue	, secondSliderValue	, thirdSliderValue	) for tmpSliderValue in range(256) ]
		SecondSliderColors	= [ (firstSliderValue	, tmpSliderValue	, thirdSliderValue	) for tmpSliderValue in range(256) ]
		ThirdSliderColors	= [ (firstSliderValue	, secondSliderValue	, tmpSliderValue	) for tmpSliderValue in range(256) ]
	if colorMode == 'HLS' :
		colorConversionFLAG = COLOR_HLS2RGB_FULL
		FirstSliderColors	= [ tuple( cvtColor( uint8([[( tmpSliderValue	, secondSliderValue	, thirdSliderValue	)]]), colorConversionFLAG )[0][0].tolist() ) for tmpSliderValue in range(256) ]
		SecondSliderColors	= [ tuple( cvtColor( uint8([[( firstSliderValue	, tmpSliderValue	, thirdSliderValue	)]]), colorConversionFLAG )[0][0].tolist() ) for tmpSliderValue in range(256) ]
		ThirdSliderColors	= [ tuple( cvtColor( uint8([[( firstSliderValue	, secondSliderValue	, tmpSliderValue	)]]), colorConversionFLAG )[0][0].tolist() ) for tmpSliderValue in range(256) ]
	if colorMode == 'HSV' :
		colorConversionFLAG = COLOR_HSV2RGB_FULL
		FirstSliderColors	= [ tuple( cvtColor( uint8([[( tmpSliderValue	, secondSliderValue	, thirdSliderValue	)]]), colorConversionFLAG )[0][0].tolist() ) for tmpSliderValue in range(256) ]
		SecondSliderColors	= [ tuple( cvtColor( uint8([[( firstSliderValue	, tmpSliderValue	, thirdSliderValue	)]]), colorConversionFLAG )[0][0].tolist() ) for tmpSliderValue in range(256) ]
		ThirdSliderColors	= [ tuple( cvtColor( uint8([[( firstSliderValue	, secondSliderValue	, tmpSliderValue	)]]), colorConversionFLAG )[0][0].tolist() ) for tmpSliderValue in range(256) ]
	if colorMode == 'Lab' :
		colorConversionFLAG = COLOR_Lab2RGB
		FirstSliderColors	= [ tuple( cvtColor( uint8([[( tmpSliderValue	, secondSliderValue	, thirdSliderValue	)]]), colorConversionFLAG )[0][0].tolist() ) for tmpSliderValue in range(256) ]
		SecondSliderColors	= [ tuple( cvtColor( uint8([[( firstSliderValue	, tmpSliderValue	, thirdSliderValue	)]]), colorConversionFLAG )[0][0].tolist() ) for tmpSliderValue in range(256) ]
		ThirdSliderColors	= [ tuple( cvtColor( uint8([[( firstSliderValue	, secondSliderValue	, tmpSliderValue	)]]), colorConversionFLAG )[0][0].tolist() ) for tmpSliderValue in range(256) ]

def saveSettings():
	with open(__file__ + ".ini", 'w') as f :
		f.write(f"""\
firstSliderValue, secondSliderValue, thirdSliderValue = {firstSliderValue}, {secondSliderValue}, {thirdSliderValue}
colorMode 			= '{colorMode}'
activeChannelIndex	=  {activeChannelIndex}""")

def printColorChoiceToStdout_andEXIT():

	if colorMode == 'RGB' : 
		colorConversionFLAG = None
	if colorMode == 'HLS' : 
		colorConversionFLAG = COLOR_HLS2RGB_FULL
	if colorMode == 'HSV' : 
		colorConversionFLAG = COLOR_HSV2RGB_FULL
	if colorMode == 'Lab' : 
		colorConversionFLAG = COLOR_Lab2RGB

	if colorConversionFLAG is None :
		r,g,b = firstSliderValue	, secondSliderValue	, thirdSliderValue
	else :
		r,g,b =  tuple( cvtColor( uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue )]]), colorConversionFLAG )[0][0].tolist() )

	if withOutputOfClosestColorName  : 
		print(f"#{r:02X}{g:02X}{b:02X} --> rgb({r},{g},{b}) --> {exactOrClosestWebColorName( (r,g,b) )}")
	else : 
		print(f"rgb( {r}, {g}, {b} )  -->  #{r:02X}{g:02X}{b:02X}")

	saveSettings()
	OApp.quit()

def exactOrClosestWebColorName( RGBtuple ):
	try:
		return f"'WebColorName: {rgb_to_name(RGBtuple)} (exact match)'"
	except ValueError:
		colorName, webColorRGB = closestWebColorName(RGBtuple)
		return f"'WebColorName: {colorName} (closest match) {webColorRGB}'"

def closestWebColorName( RGBtuple ):
	min_dist = float('inf')
	closestColorName = None
	r,g,b = RGBtuple
	for  colorName in WebColorNameList():
		colorNameR, colorNameG, colorNameB = name_to_rgb(colorName)
		dist = ( (colorNameR - r) ** 2 + (colorNameG - g) ** 2 + (colorNameB - b) ** 2) ** 0.5
		if dist < min_dist:
			min_dist	= dist
			colorRGB	= f"rgb({colorNameR},{colorNameG},{colorNameB})" 
			closestColorName = colorName
	return closestColorName, colorRGB

class ColorChoice(QWidget):
	def __init__(self):
		super().__init__()
		self.setWindowTitle(wID)
		#self.resize(windowWidth, windowHeight)
		#self.move(windowXpos, windowYpos)
		#self.setGeometry(f"{windowWidth}x{windowHeight}+{windowXpos}+{windowYpos}")
		self.setGeometry(windowXpos,windowYpos,windowWidth,windowHeight)
		self.fullscreen = fullscreenMode

	def paintEvent(self, event):
		painter = QPainter(self)
		if self.fullscreen : 
			painter.translate(offsetX, offsetY)
		painter.fillRect(self.rect(), QColor(230, 255, 230))
		painter.setPen(QPen(QColor(0, 0, 0), 1))
		painter.drawRect(1, 1, 
						 widthColorStripColor * 256 + leftMargin + rightMargin - 2, 
						 topStripsOffset + bottomMargin + 3*(heightColorStrip + verticalSpacingBetweenStrips) - 2)
		for channelIndex, (colorsChannelStrip, colorChannelValue) in enumerate([(FirstSliderColors, firstSliderValue), (SecondSliderColors, secondSliderValue), (ThirdSliderColors, thirdSliderValue)]):
			yOffset = channelIndex * (heightColorStrip + verticalSpacingBetweenStrips)
			for colorIndexInStrip, colorInStrip in enumerate(colorsChannelStrip):
				r, g, b = colorInStrip
				painter.setBrush(QColor(r, g, b))
				painter.setPen(Qt.PenStyle.NoPen)
				painter.drawRect(leftMargin + colorIndexInStrip * widthColorStripColor, 
								 topStripsOffset + yOffset, 
								 widthColorStripColor, 
								 heightColorStrip)
			#markerColor = activeSliderPositionMarkerColor[colorMode] if activeChannelIndex == channelIndex else sliderPositionMarkerColor
			if activeChannelIndex == channelIndex :
				markerColor = activeSliderPositionMarkerColor[colorMode]
				penSize	= 7
			else :
				markerColor = sliderPositionMarkerColor
				penSize = 4
			mr, mg, mb = markerColor
			painter.setPen(QPen(QColor(mr, mg, mb), penSize))
			painter.setBrush(Qt.BrushStyle.NoBrush)
			painter.drawRect(leftMargin + colorChannelValue * widthColorStripColor - sliderPositionMarkerLineWidth, 
							 topStripsOffset + yOffset - sliderPositionMarkerLineWidth, 
							 widthColorStripColor + 2 * sliderPositionMarkerLineWidth, 
							 heightColorStrip + 2 * sliderPositionMarkerLineWidth)
		r, g, b = FirstSliderColors[firstSliderValue]
		painter.setBrush(QColor(r, g, b))
		painter.setPen(Qt.PenStyle.NoPen)
		painter.drawRect(leftMargin, topMargin, 
						 256 * widthColorStripColor, 
						 topStripsOffset - 15 - topMargin)

	def mousePressEvent(self, event):
		global firstSliderValue, secondSliderValue, thirdSliderValue
		if event.button() == Qt.MouseButton.LeftButton:
			if self.fullscreen :
				mouseOffsetX, mouseOffsetY = offsetX, offsetY
			else :
				mouseOffsetX, mouseOffsetY = 0 , 0
			# :if
			x = event.pos().x() - mouseOffsetX
			y = event.pos().y() - mouseOffsetY
			if topMargin <= y <= topStripsOffset - 15 and leftMargin <= x <= leftMargin + 256 * widthColorStripColor:
				printColorChoiceToStdout_andEXIT()
			else:
				strip_height = heightColorStrip
				strip_width = 256 * widthColorStripColor
				yOffset0 = topStripsOffset + 0 * (strip_height + verticalSpacingBetweenStrips)
				yOffset1 = topStripsOffset + 1 * (strip_height + verticalSpacingBetweenStrips)
				yOffset2 = topStripsOffset + 2 * (strip_height + verticalSpacingBetweenStrips)
				if yOffset0 <= y <= yOffset0 + strip_height and leftMargin <= x <= leftMargin + strip_width:
					firstSliderValue = max(0, min(255, (x - leftMargin) // widthColorStripColor))
				elif yOffset1 <= y <= yOffset1 + strip_height and leftMargin <= x <= leftMargin + strip_width:
					secondSliderValue = max(0, min(255, (x - leftMargin) // widthColorStripColor))
				elif yOffset2 <= y <= yOffset2 + strip_height and leftMargin <= x <= leftMargin + strip_width:
					thirdSliderValue = max(0, min(255, (x - leftMargin) // widthColorStripColor))
				updateSliderColors()
				self.update()

	def keyPressEvent(self, event):
		global firstSliderValue, secondSliderValue, thirdSliderValue, activeChannelIndex, colorMode, fullscreenMode
		key = event.key()
		text = event.text().lower()

		if key in (Qt.Key.Key_Enter, Qt.Key.Key_Return):
			printColorChoiceToStdout_andEXIT()
		elif key == Qt.Key.Key_Escape:
			OApp.quit()
		# ---
		elif key == Qt.Key.Key_Tab:
			activeChannelIndex = (activeChannelIndex + 1) % 3
			self.update()
		elif text == 'r':
			if	colorMode == 'RGB' :
				return
			elif	colorMode == 'Lab' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_Lab2RGB )[0][0].tolist() 
			elif	colorMode == 'HLS' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_HLS2RGB_FULL )[0][0].tolist()
			elif	colorMode == 'HSV' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_HSV2RGB_FULL )[0][0].tolist()
			colorMode = 'RGB'
			updateSliderColors()
			self.update()
		elif text == 'h':
			if	colorMode == 'HSV' :
				return
			elif	colorMode == 'RGB' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_RGB2HSV_FULL )[0][0].tolist()
			elif	colorMode == 'HLS' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_HLS2RGB_FULL )[0][0].tolist()
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_RGB2HSV_FULL )[0][0].tolist()
			elif	colorMode == 'Lab' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_Lab2RGB )[0][0].tolist()
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_RGB2HSV_FULL )[0][0].tolist()
			colorMode = 'HSV'
			updateSliderColors()
			self.update()
		elif text == 'l':
			if	colorMode == 'HLS' :
				return
			elif	colorMode == 'RGB' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_RGB2HLS_FULL )[0][0].tolist()
			elif	colorMode == 'HSV' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_HSV2RGB_FULL )[0][0].tolist()
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_RGB2HLS_FULL )[0][0].tolist()
			elif	colorMode == 'Lab' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_Lab2RGB )[0][0].tolist()
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_RGB2HLS_FULL )[0][0].tolist()
			colorMode = 'HLS'
			updateSliderColors()
			self.update()
		elif text == 'a':
			if	colorMode == 'Lab' :
				return
			elif	colorMode == 'RGB' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_RGB2Lab )[0][0].tolist()
			elif	colorMode == 'HSV' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_HSV2RGB_FULL )[0][0].tolist()
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_RGB2Lab )[0][0].tolist()
			elif	colorMode == 'HLS' :
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_HLS2RGB_FULL )[0][0].tolist()
				firstSliderValue, secondSliderValue, thirdSliderValue = cvtColor(
					 uint8([[( firstSliderValue	, secondSliderValue	, thirdSliderValue	)]]), COLOR_RGB2Lab )[0][0].tolist()
			colorMode = 'Lab'
			updateSliderColors()
			self.update()
		elif text == 'f':
			if self.fullscreen == '':
				fullscreenMode = 'yes'
				self.fullscreen = 'yes'
				self.showFullScreen()
		elif key == Qt.Key.Key_Up:
			firstSliderValue = min(255, firstSliderValue + sliderChangeOnUpDown)
			secondSliderValue = min(255, secondSliderValue + sliderChangeOnUpDown)
			thirdSliderValue = min(255, thirdSliderValue + sliderChangeOnUpDown)
			updateSliderColors()
			self.update()
		elif key == Qt.Key.Key_Down:
			firstSliderValue = max(0, firstSliderValue - sliderChangeOnUpDown)
			secondSliderValue = max(0, secondSliderValue - sliderChangeOnUpDown)
			thirdSliderValue = max(0, thirdSliderValue - sliderChangeOnUpDown)
			updateSliderColors()
			self.update()
		elif key == Qt.Key.Key_Left:
			if activeChannelIndex == 0:
				firstSliderValue = max(0, firstSliderValue - 1)
			elif activeChannelIndex == 1:
				secondSliderValue = max(0, secondSliderValue - 1)
			elif activeChannelIndex == 2:
				thirdSliderValue = max(0, thirdSliderValue - 1)
			updateSliderColors()
			self.update()
		elif key == Qt.Key.Key_Right:
			if activeChannelIndex == 0:
				firstSliderValue = min(255, firstSliderValue + 1)
			elif activeChannelIndex == 1:
				secondSliderValue = min(255, secondSliderValue + 1)
			elif activeChannelIndex == 2:
				thirdSliderValue = min(255, thirdSliderValue + 1)
			updateSliderColors()
			self.update()
		elif key == Qt.Key.Key_PageUp:
			if activeChannelIndex == 0:
				firstSliderValue = max(0, firstSliderValue - sliderChangeOnPageUpDown)
			elif activeChannelIndex == 1:
				secondSliderValue = max(0, secondSliderValue - sliderChangeOnPageUpDown)
			elif activeChannelIndex == 2:
				thirdSliderValue = max(0, thirdSliderValue - sliderChangeOnPageUpDown)
			updateSliderColors()
			self.update()
		elif key == Qt.Key.Key_PageDown:
			if activeChannelIndex == 0:
				firstSliderValue = min(255, firstSliderValue + sliderChangeOnPageUpDown)
			elif activeChannelIndex == 1:
				secondSliderValue = min(255, secondSliderValue + sliderChangeOnPageUpDown)
			elif activeChannelIndex == 2:
				thirdSliderValue = min(255, thirdSliderValue + sliderChangeOnPageUpDown)
			updateSliderColors()
			self.update()


updateSliderColors()

OApp	= QApplication(sys.argv)
OAppO	= ColorChoice()
if fullscreenMode : 
	OAppO.showFullScreen()
else: 
	OAppO.show()
sys.exit(OApp.exec())

## ------------------------------------------------------------------------------------------------------------------------------------
# 	~ $ ./fileSmallerInSizeWithExecutableFlagSet_from_pythonScriptFile_o0.py colorChoicePyQt6.py
# 	fileSizeScriptText » fileSizeCompressedBytecode : 
#			17600 » 5934	
# 	The executable colorChoicePyQt6 using lzma requires 5934 bytes > (more than one 4kByte block of ext4 file system)
