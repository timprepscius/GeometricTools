cgc -profile vs_3_0 -bestprecision -DWM5_USE_DX9    -entry v_RootFinder -o RootFinder.vs_3_0.txt RootFinder.fx
cgc -profile ps_3_0 -bestprecision -DWM5_USE_DX9    -entry p_RootFinder -o RootFinder.ps_3_0.txt RootFinder.fx
cgc -profile vp40   -bestprecision -DWM5_USE_OPENGL -entry v_RootFinder -o RootFinder.arbvp1.txt RootFinder.fx
cgc -profile fp40   -bestprecision -DWM5_USE_OPENGL -entry p_RootFinder -o RootFinder.arbfp1.txt RootFinder.fx
