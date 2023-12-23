with import <nixpkgs> {};

stdenv.mkDerivation {
	name = "Sudoku OCR";

	nativeBuildInputs = [
		bear
		texlive.combined.scheme-full
		pkg-config
	];

	buildInputs = [
        SDL2
        SDL2.dev
        SDL2_image
		atk
		cairo
		gdk-pixbuf
		glade
		glib
		gtk3
		gtk3-x11
		harfbuzzFull
		pango
		zlib
	];
}

