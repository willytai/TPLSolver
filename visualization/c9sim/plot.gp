set terminal canvas                             # set output terminal as html
set style fill transparent solid 0.5 noborder   # set fill style
unset xtics                                     # remove x lables
unset ytics                                     # remove y labes
set nokey                                       # remove legend
set output 'c9_result.html'
plot\
'c9_red.txt' with filledcurves lc rgb "gray",\
'c9_red.txt' using 1:2 with lines lc rgb "gray",\
'c9_green.txt' with filledcurves lc rgb "green",\
'c9_green.txt' using 1:2 with lines lc rgb "green",\
'c9_blue.txt' with filledcurves lc rgb "blue",\
'c9_blue.txt' using 1:2 with lines lc rgb "blue",\
'c9_conflict.txt' with filledcurves lc rgb "red",\
'c9_conflict.txt' using 1:2 with lines lc rgb "red"
