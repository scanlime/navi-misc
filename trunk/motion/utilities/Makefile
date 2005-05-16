all:	AMCEditor.exe WriteData.exe

AMCEditor.exe:	AMCEditor.cs AMC.cs Color.cs CurveEditor.cs amc-editor.glade
	mcs -out:AMCEditor.exe -debug -target:exe AMCEditor.cs AMC.cs Color.cs CurveEditor.cs `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0` -resource:amc-editor.glade

WriteData.exe:	WriteData.cs AMC.cs
	mcs -out:WriteData.exe -debug -target:exe WriteData.cs AMC.cs `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0`
