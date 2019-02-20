set terminal canvas                             # set output terminal as html
set style fill transparent solid 0.5 noborder   # set fill style
unset xtics                                     # remove x lables
unset ytics                                     # remove y labes
set nokey                                       # remove legend
set output 'c4_result.html'
plot\
'c4_red.txt' with filledcurves lc rgb "gray",\
'c4_red.txt' using 1:2 with lines lc rgb "gray",\
'c4_green.txt' with filledcurves lc rgb "green",\
'c4_green.txt' using 1:2 with lines lc rgb "green",\
'c4_blue.txt' with filledcurves lc rgb "blue",\
'c4_blue.txt' using 1:2 with lines lc rgb "blue",\
'c4_conflict.txt' with filledcurves lc rgb "red",\
'c4_conflict.txt' using 1:2 with lines lc rgb "red"
