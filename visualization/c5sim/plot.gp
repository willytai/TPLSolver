set terminal canvas                             # set output terminal as html
set style fill transparent solid 0.5 noborder   # set fill style
unset xtics                                     # remove x lables
unset ytics                                     # remove y labes
set nokey                                       # remove legend
set output 'c5_result.html'
plot\
'c5_red.txt' with filledcurves lc rgb "gray",\
'c5_red.txt' using 1:2 with lines lc rgb "gray",\
'c5_green.txt' with filledcurves lc rgb "green",\
'c5_green.txt' using 1:2 with lines lc rgb "green",\
'c5_blue.txt' with filledcurves lc rgb "blue",\
'c5_blue.txt' using 1:2 with lines lc rgb "blue",\
'c5_conflict.txt' with filledcurves lc rgb "red",\
'c5_conflict.txt' using 1:2 with lines lc rgb "red"
