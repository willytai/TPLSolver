set terminal svg dynamic                        # set output terminal svg
set style fill transparent solid 0.5 noborder   # set fill style
unset xtics                                     # remove x lables
unset ytics                                     # remove y labes
set nokey                                       # remove legend
set output 'c1_result.svg'
plot\
'c1_red.txt' with filledcurves lc rgb "gray",\
'c1_red.txt' using 1:2 with lines lc rgb "gray",\
'c1_green.txt' with filledcurves lc rgb "green",\
'c1_green.txt' using 1:2 with lines lc rgb "green",\
'c1_blue.txt' with filledcurves lc rgb "blue",\
'c1_blue.txt' using 1:2 with lines lc rgb "blue",\
'c1_conflict.txt' with filledcurves lc rgb "red",\
'c1_conflict.txt' using 1:2 with lines lc rgb "red"
