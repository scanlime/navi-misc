AMCEditor.exe:	AMCEditor.cs AMC.cs amc-editor.glade
	mcs -out:AMCEditor.exe -debug -target:exe AMCEditor.cs AMC.cs `pkg-config --libs gtk-sharp-2.0 glade-sharp-2.0` -resource:amc-editor.glade
