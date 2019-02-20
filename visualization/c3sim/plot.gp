set terminal canvas                             # set output terminal as html
set style fill transparent solid 0.5 noborder   # set fill style
unset xtics                                     # remove x lables
unset ytics                                     # remove y labes
set nokey                                       # remove legend
set output 'c3_result.html'
plot\
'c3_red.txt' with filledcurves lc rgb "gray",\
'c3_red.txt' using 1:2 with lines lc rgb "gray",\
'c3_green.txt' with filledcurves lc rgb "green",\
'c3_green.txt' using 1:2 with lines lc rgb "green",\
'c3_blue.txt' with filledcurves lc rgb "blue",\
'c3_blue.txt' using 1:2 with lines lc rgb "blue",\
'c3_conflict.txt' with filledcurves lc rgb "red",\
'c3_conflict.txt' using 1:2 with lines lc rgb "red"
