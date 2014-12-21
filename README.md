PDF-XChange Viewer File Dumping Shim
====================================

This DLL intercepts document loading calls to PDF-XChange Viewer DLL to allow dumping of PDF files. It was designed to extract original PDF files from programs that use the PDF-XChange library where the originals are not readily accessible.

Usage
-----

Rename the existing `pxcview.dll` to `_pxcvw.dll`. Move this DLL to where the original `pxcview.dll` was. Next time when the program relying on PDF-XChange Viewer tries to load a PDF, you will be prompted to save the PDF into a convenient location. After the file is saved, the PDF will resume loading for the original program.

Compiling
---------

You need to put `pxcview.lib` from the SDK into a `lib` folder under the `PxcViewDumpingShim` project folder. Place all headers from the SDK into a folder named `pxcview_include` under the `PxcViewDumpingShim` project folder. After compiling, you will need to manually adjust the import DLL name for the original `pxcview.dll` so this DLL will not recurse upon its own functions.

You can obtain the PDF-XChange Viewer Simple DLL SDK from [here](http://www.tracker-software.com/product/pdf-xchange-viewer-simple-dll-sdk).
